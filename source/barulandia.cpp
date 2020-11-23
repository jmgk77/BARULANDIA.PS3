#include "barulandia.h"

// globals
SDL_Surface *screen = NULL;
SDL_Joystick *joystick = NULL;
TTF_Font *font = NULL;

// cleanup
void cleanup() {
  sound_end();
  if (joystick) {
    SDL_JoystickClose(joystick);
    joystick = NULL;
  }
  if (font) {
    TTF_CloseFont(font);
    font = NULL;
  }
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
  dbglogger_stop();
  return;
}

// main
int main(int argc, char **argv) {
  dbglogger_init_str("tcp:" DBG_IP ":" DBG_PORT);
  dbglogger_printf("BARULANDIA for ps3 (c) jmgk 2020\n");

#ifdef DEBUG
  atexit(ret2psload);
#endif
  atexit(cleanup);

#ifdef PS3
  // XMB exit
  sysUtilRegisterCallback(
      SYSUTIL_EVENT_SLOT0,
      [](uint64_t status, uint64_t param, void *userdata) {
        if (status == SYSUTIL_EXIT_GAME) {
          cleanup();
          sysProcessExit(1);
        }
      },
      NULL);
#endif

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  // init sdl
  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  SDL_version compiled;

  SDL_VERSION(&compiled);
  dbglogger_printf("SDL version %d.%d.%d\n", compiled.major, compiled.minor,
                   compiled.patch);

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0) {
    dbglogger_printf("SDL_Init: %s", SDL_GetError());
    return -1;
  }

  // init sdl_image
  int flags = IMG_INIT_JPG | IMG_INIT_PNG;
  int initted = IMG_Init(flags);
  if ((initted & flags) != flags) {
    dbglogger_printf("IMG_Init: %s\n", IMG_GetError());
    return -1;
  }

  if (TTF_Init() == -1) {
    dbglogger_printf("TTF_Init: %s\n", TTF_GetError());
    return -1;
  }
  // Brady Bunch Remastered
  font = TTF_OpenFont(DATA_PATH "BRADBUNR.TTF", 48);
  if (!font) {
    dbglogger_printf("TTF_OpenFont: %s\n", TTF_GetError());
    return -1;
  }
  /*debug_font(font);*/

  // sdl sound init
  sound_init();

  // joystick button state
  bool joystick_buttonstate[JOYBUTTONS];
  bool joystick_oldbuttonstate[JOYBUTTONS];
  for (int i = 0; i < JOYBUTTONS; ++i) {
    joystick_oldbuttonstate[i] = joystick_buttonstate[i] = false;
  }

#ifndef PS3
  // keyboard state
  bool state[SDLK_LAST];
#endif

  // init joystick
  joystick = SDL_JoystickOpen(0);

  // init mouse
  /*  SDL_SetRelativeMouseMode(SDL_TRUE);*/

  // init screen
  screen = SDL_SetVideoMode(WIDTH, HEIGHT, 0,
                            SDL_SWSURFACE | SDL_DOUBLEBUF /*| SDL_FULLSCREEN*/);
  if (screen == NULL) {
    dbglogger_printf("SDL_SetVideoMode: %s", SDL_GetError());
    return -1;
  }

  // print info
  debug_video();

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  // click control
  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

#define CURSOR_AREA(x1, y1, x2, y2)                                            \
  if ((cursor.x > x1) && (cursor.y > y1) && (cursor.x < x2) && (cursor.y < y2))

#define CLICK_AREA                                                             \
  CURSOR_AREA(172, 17, 322, 147) { draw_new--; } /*prev draw*/                 \
  CURSOR_AREA(955, 17, 1110, 147) {              /*next draw*/                 \
    draw_new++;                                                                \
  }                                                                            \
  CURSOR_AREA(172, 565, 322, 707) { /*refresh draw*/                           \
    effect_play(SOUND_TRASH);                                                  \
    draw_refresh = true;                                                       \
  }                                                                            \
  CURSOR_AREA(172, 408, 322, 538) { /*eraser*/                                 \
    color_new = SDL_MapRGBA(field->format, 255, 255, 255, SDL_ALPHA_OPAQUE);   \
  }                                                                            \
  CURSOR_AREA(955, 408, 1110, 538) {} /*paint tool*/                           \
  CURSOR_AREA(1140, 15, 1270, 705) {  /*color picker*/                         \
    SDL_LockSurface(field);                                                    \
    Uint32 c = GetPixel32_nolock(field, cursor.x, cursor.y);                   \
    SDL_UnlockSurface(field);                                                  \
    if (c != SDL_MapRGBA(field->format, 0, 0, 0, SDL_ALPHA_OPAQUE)) {          \
      color_new = c;                                                           \
    }                                                                          \
  }                                                                            \
  CURSOR_AREA(353, 1, 927, 719) { /*paint*/                                    \
    floodfill(field, cursor.x, cursor.y, color_current);                       \
    redraw = true;                                                             \
  }

#define BUTTON_CHANGE(x)                                                       \
  if (joystick_oldbuttonstate[x] != joystick_buttonstate[x])
#define BUTTON_PRESSED(x)                                                      \
  if (!joystick_oldbuttonstate[x] && joystick_buttonstate[x])
#define BUTTON_PRESSED_ALT(x, y)                                               \
  if ((!joystick_oldbuttonstate[x] && joystick_buttonstate[x]) ||              \
      (!joystick_oldbuttonstate[y] && joystick_buttonstate[y]))
#define BUTTON_RELEASED(x)                                                     \
  if (joystick_oldbuttonstate[x] && !joystick_buttonstate[x])
#define BUTTON_PRESSED_CONT(x) if (joystick_buttonstate[x])

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  // variables
  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  // intro vars
  SDL_Surface *logo;

  // startscreen vars
  SDL_Surface *fundo, *titulo, *start, *gallery;
  SDL_Rect t_pos, s_pos, g_pos;
  int t_pos_x_max, t_pos_x_min, t_pos_x_delta, s_pos_y_max, s_pos_y_min,
      s_pos_xy_delta, g_pos_y_max, g_pos_y_min, g_pos_xy_delta;
  bool start_gallery;
  int next_phase;

  // gallery vars
  struct draws {
    char *name;
    SDL_Surface *surface;
  };
  list<draws> drawings;
  list<draws>::iterator drawing_ptr;
  SDL_Surface *buttons, *t_move, *t_continue, *t_delete, *t_cancel, *sfundo;
  SDL_Surface *draw_continue = NULL;

  // main vars
  SDL_Rect cursor;
  SDL_Surface *tcursor, *aux;
  SDL_Surface *field, *draw, *exit_surface, *help_surface, *credit_surface;
  int draw_current, draw_new, dx, dy, accell;
  Uint32 color_current, color_new;
  bool draw_refresh, redraw, exit_show, exit_screen, help_show, help_screen,
      credit_show, credit_screen;

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  // main loop
  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  // start screen loop
  bool start_active = true;
  int PHASE = INTRO_INIT;

  while (start_active) {

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      switch (e.type) {
      case SDL_QUIT:
        start_active = false;
        break;

#ifndef PS3

#define KSTATE(k, j)                                                           \
  if (e.key.keysym.sym == k) {                                                 \
    state[k] = j;                                                              \
  }
      case SDL_KEYDOWN:
        debug_keyboard(&e.key);
        if (e.key.keysym.sym == SDLK_ESCAPE) {
          start_active = false;
        }
        KSTATE(SDLK_w, true);
        KSTATE(SDLK_z, true);
        KSTATE(SDLK_s, true);
        KSTATE(SDLK_a, true);
        KSTATE(SDLK_LCTRL, true);
        KSTATE(SDLK_RCTRL, true);
        KSTATE(SDLK_LSHIFT, true);
        KSTATE(SDLK_RSHIFT, true);
        KSTATE(SDLK_DOWN, true);
        KSTATE(SDLK_UP, true);
        KSTATE(SDLK_RIGHT, true);
        KSTATE(SDLK_LEFT, true);
        KSTATE(SDLK_1, true);
        KSTATE(SDLK_2, true);
        KSTATE(SDLK_RALT, true);
        KSTATE(SDLK_LALT, true);
        break;

      case SDL_KEYUP:
        KSTATE(SDLK_w, false);
        KSTATE(SDLK_z, false);
        KSTATE(SDLK_s, false);
        KSTATE(SDLK_a, false);
        KSTATE(SDLK_LCTRL, false);
        KSTATE(SDLK_RCTRL, false);
        KSTATE(SDLK_LSHIFT, false);
        KSTATE(SDLK_RSHIFT, false);
        KSTATE(SDLK_DOWN, false);
        KSTATE(SDLK_UP, false);
        KSTATE(SDLK_RIGHT, false);
        KSTATE(SDLK_LEFT, false);
        KSTATE(SDLK_1, false);
        KSTATE(SDLK_2, false);
        KSTATE(SDLK_RALT, false);
        KSTATE(SDLK_LALT, false);
        break;
#endif
      default:
        break;
      }
    }

    // save old joystick status and reset state
    for (int i = 0; i < JOYBUTTONS; ++i) {
      joystick_oldbuttonstate[i] = joystick_buttonstate[i];
      joystick_buttonstate[i] = false;
    }

    // convert mouse state to joystick state
    /*    int xx, yy;
        int bb = SDL_GetRelativeMouseState(&xx, &yy);
        if (bb & SDL_BUTTON(SDL_BUTTON_LEFT))
          joystick_buttonstate[SDL_CONTROLLER_BUTTON_CROSS] = true;
        if (xx > 0)
          joystick_buttonstate[SDL_CONTROLLER_BUTTON_RIGHT] = true;
        if (xx < 0)
          joystick_buttonstate[SDL_CONTROLLER_BUTTON_LEFT] = true;
        if (yy < 0)
          joystick_buttonstate[SDL_CONTROLLER_BUTTON_UP] = true;
        if (yy > 0)
          joystick_buttonstate[SDL_CONTROLLER_BUTTON_DOWN] = true;*/

#ifndef PS3

// convert keyboard state to joystick state
#define KEYB_CONVERT(j, k)                                                     \
  if (state[k]) {                                                              \
    joystick_buttonstate[j] = true;                                            \
  }

    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_TRIANGLE, SDLK_w);
    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_CROSS, SDLK_z);
    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_CIRCLE, SDLK_s);
    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_SQUARE, SDLK_a);
    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_L1, SDLK_LCTRL);
    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_R1, SDLK_RCTRL);
    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_L2, SDLK_LSHIFT);
    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_R2, SDLK_RSHIFT);
    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_DOWN, SDLK_DOWN);
    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_UP, SDLK_UP);
    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_RIGHT, SDLK_RIGHT);
    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_LEFT, SDLK_LEFT);
    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_START, SDLK_1);
    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_SELECT, SDLK_2)
    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_R3, SDLK_RALT);
    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_L3, SDLK_LALT);
#endif
    // read joystick state
    if (joystick) {
      SDL_JoystickUpdate();
      debug_joystick(joystick);
      for (int i = 0; i < JOYBUTTONS; ++i) {
        joystick_buttonstate[i] = SDL_JoystickGetButton(joystick, i);
      }
    }

#ifdef DEBUG
    BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_R3) {
      dbglogger_screenshot_tmp(SDL_ALPHA_OPAQUE);
    }
#endif

    switch (PHASE) {
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// logo
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#ifndef SKIP_INTRO
    case INTRO_INIT: {
      logo = load_surface(DATA_PATH "LOGO" GRAPH_EXT);
      PHASE = INTRO_MAIN;
    } break;

    case INTRO_MAIN: {
      // fade logo in and out
      fade_in_out(screen, logo, true);
      fade_in_out(screen, logo, false);
      PHASE = INTRO_END;
    } break;

    case INTRO_END: {
      // free logo
      SDL_FreeSurface(logo);
      PHASE = START_INIT;
    } break;
#endif
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// start screen
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#ifndef SKIP_STARTSCREEN
    case START_INIT: {
      fundo = load_surface(DATA_PATH "FUNDO" GRAPH_EXT);
      titulo = load_surface(DATA_PATH "TITULO" GRAPH_EXT);
      start = load_surface(DATA_PATH "START" GRAPH_EXT);
      gallery = load_surface(DATA_PATH "GALERIA" GRAPH_EXT);

      // fade start screen in
      fade_in_out(screen, fundo, true);

      // init positions
      t_pos.x = (WIDTH - titulo->w) / 2;
      t_pos.y = (HEIGHT / 4) * 1;
      t_pos.w = titulo->w;
      t_pos.h = titulo->h;

      s_pos.x = (WIDTH - start->w) / 2;
      s_pos.y = (HEIGHT / 4) * 3;
      s_pos.w = start->w;
      s_pos.h = start->h;

      g_pos.x = (WIDTH - gallery->w) / 2;
      g_pos.y = (HEIGHT / 4) * 3.5;
      g_pos.w = gallery->w;
      g_pos.h = gallery->h;

      // title position and delta
      t_pos_x_max = t_pos.x + 50;
      t_pos_x_min = t_pos.x - 50;
      t_pos_x_delta = -1;

      // start position and delta
      s_pos_y_max = s_pos.y + 3;
      s_pos_y_min = s_pos.y - 3;
      s_pos_xy_delta = 1;

      // gallery position and delta
      g_pos_y_max = g_pos.y + 3;
      g_pos_y_min = g_pos.y - 3;
      g_pos_xy_delta = 1;

      // play is default
      start_gallery = true;

      PHASE = START_MAIN;
    } break;

    case START_MAIN: {
      // title move
      t_pos.x += t_pos_x_delta;
      if ((t_pos.x < t_pos_x_min) || (t_pos.x > t_pos_x_max)) {
        t_pos_x_delta = -t_pos_x_delta;
      }

      // start move
      if (start_gallery) {
        s_pos.x += s_pos_xy_delta;
        s_pos.y += s_pos_xy_delta;
        s_pos.w += -(s_pos_xy_delta * 2);
        s_pos.h += -(s_pos_xy_delta * 2);
        if ((s_pos.y < s_pos_y_min) || (s_pos.y > s_pos_y_max)) {
          s_pos_xy_delta = -s_pos_xy_delta;
        }
      } else {
        // gallery move
        g_pos.x += g_pos_xy_delta;
        g_pos.y += g_pos_xy_delta;
        g_pos.w += -(g_pos_xy_delta * 2);
        g_pos.h += -(g_pos_xy_delta * 2);
        if ((g_pos.y < g_pos_y_min) || (g_pos.y > g_pos_y_max)) {
          g_pos_xy_delta = -g_pos_xy_delta;
        }
      }

      // paste fundo
      SDL_Surface *tmp;
      SDL_BlitSurface(fundo, NULL, screen, NULL);
      SDL_BlitSurface(titulo, NULL, screen, &t_pos);
      tmp = ScaleSurface(start, s_pos.w, s_pos.h);
      SDL_BlitSurface(tmp, NULL, screen, &s_pos);
      SDL_FreeSurface(tmp);
      tmp = ScaleSurface(gallery, g_pos.w, g_pos.h);
      SDL_BlitSurface(tmp, NULL, screen, &g_pos);
      SDL_FreeSurface(tmp);

      // render
      SDL_Flip(screen);

      // choose option
      dy = 0;
      if (joystick) {
        int _y = SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTY);
        dy = (_y > AXIS_DEADZONE) ? 1 : (_y < -AXIS_DEADZONE) ? -1 : dy;
      }
      BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_DOWN) dy = 1;
      BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_UP) dy = -1;

      // flip
      if (dy) {
        start_gallery = !start_gallery;
      }

      // start next
      BUTTON_PRESSED_ALT(SDL_CONTROLLER_BUTTON_START,
                         SDL_CONTROLLER_BUTTON_CROSS) {
        if (start_gallery) {
          next_phase = MAIN_INIT;
        } else {
          next_phase = GALLERY_INIT;
        }
        PHASE = START_END;
      }

#ifdef DEBUG
      BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_TRIANGLE) {
        next_phase = FINISHED;
        PHASE = START_END;
      }
#endif

    } break;

    case START_END: {
      fade_in_out(screen, fundo, false);
      SDL_FreeSurface(fundo);
      SDL_FreeSurface(titulo);
      SDL_FreeSurface(start);
      PHASE = next_phase;
    } break;
#endif
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// gallery
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#ifndef SKIP_GALLERY
    case GALLERY_INIT: {
      struct dirent *en;
      draws i;

      // load resources
      sfundo = load_surface(DATA_PATH "FUNDO" GRAPH_EXT);

      // create surfaces and textures
      if (DIR *dr = opendir(DATA_PATH "SAVEDATA/")) {
        while ((en = readdir(dr)) != NULL) {
          // our files have exatly 28 chars (DRAW_YYYY_MM_DD_HH_MM_SS.PNG)
          if (strlen(en->d_name) == 28) {
            i.name = new char[MAX_STRING];
            snprintf(i.name, MAX_STRING, "%sSAVEDATA/%s", DATA_PATH,
                     en->d_name);
            i.surface = load_surface(i.name); /* */
            drawings.push_back(i);
          }
        }
        closedir(dr);
      }

      // if not files found, push empty values
      if (drawings.empty()) {
        i.name = NULL;
        i.surface = load_surface(DATA_PATH "NOTFOUND" GRAPH_EXT);
        drawings.push_back(i);
      }

      // create texts
      SDL_Color fg = {255, 0, 0, SDL_ALPHA_OPAQUE};
      buttons = load_surface(DATA_PATH "BUTTONS" GRAPH_EXT);
      t_move = TTF_RenderUTF8_Blended(font, "ESCOLHER", fg);
      t_continue = TTF_RenderUTF8_Blended(font, "CONTINUAR", fg);
      t_delete = TTF_RenderUTF8_Blended(font, "APAGAR", fg);
      t_cancel = TTF_RenderUTF8_Blended(font, "VOLTAR", fg);

      // draw buttons
      SDL_Rect dstrect;
      SDL_Rect srcrect;
      srcrect.w = BUTTONS_XY;
      srcrect.h = BUTTONS_XY;

      dstrect.x = (((WIDTH / 4) - ((BUTTONS_XY) + t_move->w)) / 2);
      dstrect.y = HEIGHT - BUTTONS_XY;
      srcrect.x = DPAD_X * BUTTONS_XY;
      srcrect.y = DPAD_Y * BUTTONS_XY;
      SDL_BlitSurface(buttons, &srcrect, sfundo, &dstrect);
      dstrect.x += BUTTONS_XY;
      SDL_BlitSurface(t_move, NULL, sfundo, &dstrect);
      dstrect.x += t_move->w;

      dstrect.x =
          (WIDTH / 4) + (((WIDTH / 4) - ((BUTTONS_XY) + t_continue->w)) / 2);
      srcrect.x = CROSS_X * BUTTONS_XY;
      srcrect.y = CROSS_Y * BUTTONS_XY;
      SDL_BlitSurface(buttons, &srcrect, sfundo, &dstrect);
      dstrect.x += BUTTONS_XY;
      SDL_BlitSurface(t_continue, NULL, sfundo, &dstrect);
      dstrect.x += t_continue->w;

      dstrect.x = ((WIDTH / 4) * 2) +
                  (((WIDTH / 4) - ((BUTTONS_XY) + t_delete->w)) / 2);
      srcrect.x = TRIANGLE_X * BUTTONS_XY;
      srcrect.y = TRIANGLE_Y * BUTTONS_XY;
      SDL_BlitSurface(buttons, &srcrect, sfundo, &dstrect);
      dstrect.x += BUTTONS_XY;
      SDL_BlitSurface(t_delete, NULL, sfundo, &dstrect);
      dstrect.x += t_delete->w;

      dstrect.x = ((WIDTH / 4) * 3) +
                  (((WIDTH / 4) - ((BUTTONS_XY) + t_cancel->w)) / 2);
      srcrect.x = CIRCLE_X * BUTTONS_XY;
      srcrect.y = CIRCLE_Y * BUTTONS_XY;
      SDL_BlitSurface(buttons, &srcrect, sfundo, &dstrect);
      dstrect.x += BUTTONS_XY;
      SDL_BlitSurface(t_cancel, NULL, sfundo, &dstrect);
      /*dstrect.x += t_cancel->w;*/

      // free text surfaces
      SDL_FreeSurface(t_move);
      SDL_FreeSurface(t_continue);
      SDL_FreeSurface(t_delete);
      SDL_FreeSurface(t_cancel);
      SDL_FreeSurface(buttons);

      // pointers to first drawing retrieved
      drawing_ptr = drawings.begin();

      PHASE = GALLERY_MAIN;
    } break;

    case GALLERY_MAIN: {

      // move drawings
      BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_LEFT) {
        if (drawing_ptr != drawings.begin()) {
          drawing_ptr--;
        } else {
          effect_play(SOUND_ERROR);
        }
      }
      BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_RIGHT) {
        if (drawing_ptr != --drawings.end()) {
          drawing_ptr++;
        } else {
          effect_play(SOUND_ERROR);
        }
      }

      // exit to game
      BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_CROSS) {
        if (drawing_ptr->name) {
          draw_continue = drawing_ptr->surface;
        } else {
          draw_continue = NULL;
        }
        PHASE = GALLERY_END;
      }
      // exit to menu
      BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_CIRCLE) {
        draw_continue = NULL;
        PHASE = GALLERY_END;
      }

      // delete drawing
      BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_TRIANGLE) {
        if (drawing_ptr->name) {
          remove(drawing_ptr->name);
          // free
          SDL_FreeSurface(drawing_ptr->surface);
          delete[] drawing_ptr->name;
          // remove from list
          drawing_ptr = drawings.erase(drawing_ptr);

          // if all deleted, push empty values
          if (drawings.empty()) {
            draws i;
            i.name = NULL;
            i.surface = load_surface(DATA_PATH "NOTFOUND" GRAPH_EXT);
            drawings.push_back(i);
          }

          // if the removal left us after the last, rewind
          if (drawing_ptr == drawings.end()) {
            drawing_ptr--;
          }

          effect_play(SOUND_TRASH);
        } else {
          effect_play(SOUND_ERROR);
        }
      }

      // render fundo
      SDL_Rect dstrect;
      list<draws>::iterator ptr;
      SDL_BlitSurface(sfundo, NULL, screen, NULL);

      // previous
      if (drawing_ptr != drawings.begin()) {
        if (prev(drawing_ptr) != drawings.begin()) {
          // n-2 exists
          ptr = drawing_ptr;
          ptr--;
          ptr--;
          dstrect.w = DRAW_W / 2.5;
          dstrect.h = DRAW_H / 2.5;
          dstrect.x = (WIDTH - dstrect.w) / 8;
          dstrect.y = (HEIGHT - dstrect.h) / 2;
          scale_show(screen, ptr->surface, &dstrect);
        }
        // n-1 exists
        ptr = drawing_ptr;
        ptr--;
        dstrect.w = DRAW_W / 2;
        dstrect.h = DRAW_H / 2;
        dstrect.x = (WIDTH - dstrect.w) / 4;
        dstrect.y = (HEIGHT - dstrect.h) / 2;
        scale_show(screen, ptr->surface, &dstrect);
      }

      // next
      if (drawing_ptr != --drawings.end()) {
        if (next(drawing_ptr) != --drawings.end()) {
          // n+2 exists
          ptr = drawing_ptr;
          ptr++;
          ptr++;
          dstrect.w = DRAW_W / 2.5;
          dstrect.h = DRAW_H / 2.5;
          dstrect.x = ((WIDTH - dstrect.w) / 8) * 7;
          dstrect.y = (HEIGHT - dstrect.h) / 2;
          scale_show(screen, ptr->surface, &dstrect);
        }
        // n+1 exists
        ptr = drawing_ptr;
        ptr++;
        dstrect.w = DRAW_W / 2;
        dstrect.h = DRAW_H / 2;
        dstrect.x = ((WIDTH - dstrect.w) / 4) * 3;
        dstrect.y = (HEIGHT - dstrect.h) / 2;
        scale_show(screen, ptr->surface, &dstrect);
      }

      // position of main draw
      dstrect.w = DRAW_W / 1.5;
      dstrect.h = DRAW_H / 1.5;
      dstrect.x = (WIDTH - dstrect.w) / 2;
      dstrect.y = (HEIGHT - dstrect.h) / 2;
      scale_show(screen, drawing_ptr->surface, &dstrect);

      // show
      SDL_Flip(screen);

    } break;

    case GALLERY_END: {
      draws i;

      // free names, surfaces, and textures
      while (!drawings.empty()) {
        i = drawings.front();
        // dont free it if we'll continue drawing
        if (draw_continue != i.surface) {
          if (i.surface)
            SDL_FreeSurface(i.surface);
        }
        if (i.name)
          delete[] i.name;
        drawings.pop_front();
      }

      if (draw_continue) {
        // exit to game
        effect_play(SOUND_MAGIC);
        PHASE = MAIN_INIT;
      } else {
        // exit to menu
        PHASE = START_INIT;
      }

      fade_in_out(screen, sfundo, false);
      SDL_FreeSurface(sfundo);
    } break;
#endif
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// main
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#ifndef SKIP_MAIN
    case MAIN_INIT: {
      // init cursor
      tcursor = load_surface(DATA_PATH "CURSOR" GRAPH_EXT);

      cursor.x = WIDTH / 2;
      cursor.y = HEIGHT / 2;
      cursor.w = tcursor->w;
      cursor.h = tcursor->h;

      // draw control
      draw_current = draw_new = 0;
      draw_refresh = redraw = true;

      // cursor movement
      accell = 0;

      // load resources
      field = load_surface(DATA_PATH "FIELD" GRAPH_EXT);
      draw = NULL;

      // colors (default green)
      color_current = -1;
      color_new = SDL_MapRGBA(field->format, 0, 255, 0, SDL_ALPHA_OPAQUE);

      // surface to texture
      aux = NULL;

      // exit confirmation stuff
      exit_show = exit_screen = false;
      exit_surface = create_exit_screen(font);

      // help screen stuff
      help_show = help_screen = false;
      help_surface = create_help_screen(font);

      // credit screen stuff
      credit_show = credit_screen = false;
      credit_surface = create_credit_screen(font);

      PHASE = MAIN_MAIN;
    } break;

    case MAIN_MAIN: {
      // exit screen button (confirm)
      if (exit_show) {
        BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_CROSS) PHASE = MAIN_END;
      }

      // exit & help screen button (back)
      if (exit_show || help_show || credit_show) {
        BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_CIRCLE) {
          exit_show = exit_screen = help_show = help_screen = credit_show =
              credit_screen = false;
          redraw = true;
        }
      } else {
        // normal buttons...
        // show exit screen
        BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_START) {
          exit_show = true;
          effect_play(SOUND_PING);
        };

        // show help screen
        BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_SELECT) {
          help_show = true;
          effect_play(SOUND_PING);
        };

        // show credit screen
        BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_R2)
        BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_L2)
        BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_SQUARE) {
          credit_show = true;
          effect_play(SOUND_PING);
        };

        // change drawing
        BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_L1) draw_new--;
        BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_R1) draw_new++;

        // save drawing
        BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_TRIANGLE) { save_png(field); }

        // clicks
        BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_CROSS) { CLICK_AREA }
      }

      // update cursor (joystick L)
      dx = dy = 0;
      if (joystick) {
        int _x = SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTX);
        int _y = SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTY);
        dx = (_x > AXIS_DEADZONE) ? 1 : (_x < -AXIS_DEADZONE) ? -1 : dx;
        dy = (_y > AXIS_DEADZONE) ? 1 : (_y < -AXIS_DEADZONE) ? -1 : dx;
      }

      BUTTON_PRESSED_CONT(SDL_CONTROLLER_BUTTON_RIGHT) dx = 1;
      BUTTON_PRESSED_CONT(SDL_CONTROLLER_BUTTON_LEFT) dx = -1;
      BUTTON_PRESSED_CONT(SDL_CONTROLLER_BUTTON_DOWN) dy = 1;
      BUTTON_PRESSED_CONT(SDL_CONTROLLER_BUTTON_UP) dy = -1;

      // start/stop acceleration
      if (!dx && !dy) {
        accell = 0;
      } else {
        // limit acceleration
        if (accell < 64) {
          accell++;
        }
      }

      // accelerate cursor
      cursor.x += dx + (dx * (accell / 8));
      cursor.y += dy + (dy * (accell / 8));

      // show exit screen
      if ((exit_show) && (!exit_screen)) {
        SDL_FreeSurface(aux);
        aux = SDL_DisplayFormatAlpha(exit_surface);
        exit_screen = true;
      }

      // show help screen
      if ((help_show) && (!help_screen)) {
        SDL_FreeSurface(aux);
        aux = SDL_DisplayFormatAlpha(help_surface);
        help_screen = true;
      }

      // show help screen
      if ((credit_show) && (!credit_screen)) {
        SDL_FreeSurface(aux);
        aux = SDL_DisplayFormatAlpha(credit_surface);
        credit_screen = true;
      }

      // nextdraw boundaries check (cicle)
      draw_new = (draw_new < 0) ? (MAX_DRAW - 1)
                                : (draw_new == MAX_DRAW) ? 0 : draw_new;

      // screen boundaries check (limit)
      cursor.x =
          (cursor.x < 0) ? 0 : (cursor.x >= WIDTH - 1) ? (WIDTH - 1) : cursor.x;
      cursor.y = (cursor.y < 0)
                     ? 0
                     : (cursor.y >= HEIGHT - 1) ? (HEIGHT - 1) : cursor.y;

      // change/reload current drawing
      if ((draw_current != draw_new) || (draw_refresh)) {
        draw_current = draw_new;
        draw_refresh = false;

        // free old
        SDL_FreeSurface(draw);

        // continue old?
        if (draw_continue) {
          draw = draw_continue;
          draw_continue = NULL;
        } else {
          // load new
          char buf[MAX_STRING];
          snprintf(buf, MAX_STRING, "%sDRAW%d%s", DATA_PATH, draw_current + 1,
                   GRAPH_EXT);
          draw = load_surface(buf);

          // play sound effect
          effect_play(draw_current + 1);
        }

        // set position
        SDL_Rect d_pos;
        d_pos.x = (WIDTH - DRAW_W) / 2;
        d_pos.y = 0;
        d_pos.w = DRAW_W;
        d_pos.h = DRAW_H;

        // paste draw into field
        SDL_BlitSurface(draw, NULL, field, &d_pos);

        // redo color picker sample
        color_current = -1;
        redraw = true;
      }

      // change current color
      if (color_current != color_new) {
        color_current = color_new;
        floodfill(field, 1000, 650, color_current);
        redraw = true;
      }

      // render
      if (redraw) {
        // recreate aux texture
        SDL_FreeSurface(aux);
        aux = SDL_DisplayFormatAlpha(field);
        redraw = false;
      }
      SDL_BlitSurface(aux, NULL, screen, NULL);
      SDL_BlitSurface(tcursor, NULL, screen, &cursor);
      SDL_Flip(screen);
    } break;

    case MAIN_END: {
      // free
      SDL_FreeSurface(draw);
      SDL_FreeSurface(exit_surface);
      SDL_FreeSurface(help_surface);
      SDL_FreeSurface(credit_surface);
      SDL_FreeSurface(field);
      SDL_FreeSurface(aux);
      SDL_FreeSurface(tcursor);
      PHASE = START_INIT;
    } break;
#endif
      /////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////////////////////////////
      // start screen
      /////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////////////////////////////

    default: {
      if (PHASE == FINISHED) {
        start_active = false;
      }
      // if phase isnt implemented, go to next
      PHASE++;
    } break;
    }
  }
  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  // cleanup (atexit)
  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  return 0;
}
