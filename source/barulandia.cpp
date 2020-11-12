#include "barulandia.h"

int main(int argc, char **argv) {
  dbglogger_init_str("tcp:" DBG_IP ":" DBG_PORT);
  dbglogger_printf("BARULANDIA for ps3 (c) jmgk 2020");

#ifdef DEBUG
  atexit(ret2psload);
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
  TTF_Font *font = TTF_OpenFont(DATA_PATH "BRADBUNR.TTF", 48);
  if (!font) {
    dbglogger_printf("TTF_OpenFont: %s\n", TTF_GetError());
    return -1;
  }
  /*debug_font(font);*/

  // sdl sound init
  sound_init();

  // joystick/keyboard button state
  bool joystick_buttonstate[JOYBUTTONS];
  bool joystick_oldbuttonstate[JOYBUTTONS];
  for (int i = 0; i < JOYBUTTONS; ++i) {
    joystick_oldbuttonstate[i] = joystick_buttonstate[i] = false;
  }

  // init joystick
  SDL_Joystick *joystick = SDL_JoystickOpen(0);

  // init mouse
  SDL_SetRelativeMouseMode(SDL_TRUE);

  // init screen
  SDL_Window *window = SDL_CreateWindow(
      "BARULANDIA.PS3", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH,
      HEIGHT, SDL_WINDOW_FULLSCREEN);
  if (window == NULL) {
    dbglogger_printf("SDL_CreateWindow: %s\n", SDL_GetError());
    return -1;
  }

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
  if (renderer == NULL) {
    dbglogger_printf("SDL_CreateRenderer: %s", SDL_GetError());
    return -1;
  }

  // print info
  /*debug_video();
  debug_window(window);
  debug_renderer(renderer);*/

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
  SDL_Texture *logo;

  // startscreen vars
  SDL_Texture *fundo, *titulo, *start, *gallery;
  SDL_Rect t_pos, s_pos, g_pos;
  int t_pos_x_max, t_pos_x_min, t_pos_x_delta, s_pos_y_max, s_pos_y_min,
      s_pos_xy_delta, g_pos_y_max, g_pos_y_min, g_pos_xy_delta;
  bool start_gallery;
  int next_phase;

  // gallery vars
  struct draws {
    char *name;
    SDL_Surface *surface;
    SDL_Texture *texture;
  };
  list<draws> drawings;
  list<draws>::iterator drawing_ptr;

  SDL_Surface *draw_continue = NULL;

  // main vars
  SDL_Rect cursor;
  SDL_Texture *tcursor, *aux;
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
      if (e.type == SDL_QUIT) {
        start_active = false;
      }
    }

    // save old joystick status and reset state
    for (int i = 0; i < JOYBUTTONS; ++i) {
      joystick_oldbuttonstate[i] = joystick_buttonstate[i];
      joystick_buttonstate[i] = false;
    }

    // convert mouse state to joystick state
    int xx, yy;
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
      joystick_buttonstate[SDL_CONTROLLER_BUTTON_DOWN] = true;

    // convert keyboard state to joystick state
    const Uint8 *state = SDL_GetKeyboardState(NULL);
#define KEYB_CONVERT(j, k)                                                     \
  if (state[k]) {                                                              \
    joystick_buttonstate[j] = true;                                            \
  }

    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_TRIANGLE, SDL_SCANCODE_W);
    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_CROSS, SDL_SCANCODE_Z);
    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_CIRCLE, SDL_SCANCODE_S);
    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_SQUARE, SDL_SCANCODE_A);
    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_L1, SDL_SCANCODE_LCTRL);
    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_R1, SDL_SCANCODE_RCTRL);
    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_L2, SDL_SCANCODE_LSHIFT);
    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_R2, SDL_SCANCODE_RSHIFT);
    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_DOWN, SDL_SCANCODE_DOWN);
    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_UP, SDL_SCANCODE_UP);
    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_RIGHT, SDL_SCANCODE_RIGHT);
    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_LEFT, SDL_SCANCODE_LEFT);
    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_START, SDL_SCANCODE_1);
    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_SELECT, SDL_SCANCODE_2)
    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_R3, SDL_SCANCODE_RALT);
    KEYB_CONVERT(SDL_CONTROLLER_BUTTON_L3, SDL_SCANCODE_LALT);

    // read joystick state
    if (joystick) {
      SDL_JoystickUpdate();
      /*debug_joystick(joystick);*/
      for (int i = 0; i < JOYBUTTONS; ++i) {
        joystick_buttonstate[i] = SDL_JoystickGetButton(joystick, i);
      }
    }

    switch (PHASE) {
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// logo
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#ifndef SKIP_INTRO
    case INTRO_INIT: {
      // white is our "clear screen' color
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
      logo = load_texture(renderer, DATA_PATH "LOGO" GRAPH_EXT);
      PHASE = INTRO_MAIN;
    } break;

    case INTRO_MAIN: {
      // fade logo in and out
      set_alpha_rate(64);
      fade_in_out(renderer, logo, true, true);
      effect_play(SOUND_LOGO);
      fade_in_out(renderer, logo, true, false);
      PHASE = INTRO_END;
    } break;

    case INTRO_END: {
      // free logo
      SDL_DestroyTexture(logo);
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
      fundo = load_texture(renderer, DATA_PATH "FUNDO" GRAPH_EXT);
      titulo = load_texture(renderer, DATA_PATH "TITULO" GRAPH_EXT);
      start = load_texture(renderer, DATA_PATH "START" GRAPH_EXT);
      gallery = load_texture(renderer, DATA_PATH "GALERIA" GRAPH_EXT);

      // fade start screen in
      set_alpha_rate(30);
      fade_in_out(renderer, fundo, false, true);

      // reset alpha
      SDL_SetTextureAlphaMod(fundo, SDL_ALPHA_OPAQUE);

      // init positions
      int tw, th;
      SDL_QueryTexture(titulo, NULL, NULL, &tw, &th);
      t_pos.x = (WIDTH - tw) / 2;
      t_pos.y = (HEIGHT / 4) * 1;
      t_pos.w = tw;
      t_pos.h = th;

      SDL_QueryTexture(start, NULL, NULL, &tw, &th);
      s_pos.x = (WIDTH - tw) / 2;
      s_pos.y = (HEIGHT / 4) * 3;
      s_pos.w = tw;
      s_pos.h = th;

      SDL_QueryTexture(gallery, NULL, NULL, &tw, &th);
      g_pos.x = (WIDTH - tw) / 2;
      g_pos.y = (HEIGHT / 4) * 3.5;
      g_pos.w = tw;
      g_pos.h = th;

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

      // clear and paste fundo, titulo, start
      SDL_RenderClear(renderer);
      SDL_RenderCopy(renderer, fundo, NULL, NULL);
      SDL_RenderCopy(renderer, titulo, NULL, &t_pos);
      SDL_RenderCopy(renderer, start, NULL, &s_pos);
      SDL_RenderCopy(renderer, gallery, NULL, &g_pos);

      // render
      SDL_RenderPresent(renderer);

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
      set_alpha_rate(30);
      fade_in_out(renderer, fundo, false, false);
      SDL_DestroyTexture(fundo);
      SDL_DestroyTexture(titulo);
      SDL_DestroyTexture(start);
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
      fundo = load_texture(renderer, DATA_PATH "FUNDO" GRAPH_EXT);

      // create surfaces and textures
      if (DIR *dr = opendir(DATA_PATH "SAVEDATA/")) {
        while ((en = readdir(dr)) != NULL) {
          // our files have exatly 28 chars (DRAW_YYYY_MM_DD_HH_MM_SS.PNG)
          if (strlen(en->d_name) == 28) {
            i.name = new char[MAX_STRING];
            snprintf(i.name, MAX_STRING, "%sSAVEDATA/%s", DATA_PATH,
                     en->d_name);
            i.surface = load_surface(i.name);
            i.texture = load_texture(renderer, i.name);
            drawings.push_back(i);
          }
        }
        closedir(dr);
      } else {
        // if not files found, push empty values
        i.name = NULL;
        i.surface = NULL;
        i.texture = load_texture(renderer, DATA_PATH "NOTFOUND" GRAPH_EXT);
        drawings.push_back(i);
      }
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
        draw_continue = drawing_ptr->surface;
        PHASE = GALLERY_END;
      }
      // exit to menu
      BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_CIRCLE) {
        draw_continue = NULL;
        PHASE = GALLERY_END;
      }

      // delete drawing
      BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_TRIANGLE) {
        remove(drawing_ptr->name);
        // free
        SDL_FreeSurface(drawing_ptr->surface);
        SDL_DestroyTexture(drawing_ptr->texture);
        delete[] drawing_ptr->name;
        // remove from list
        drawing_ptr = drawings.erase(drawing_ptr);

        // if the removal left us after the last, rewind
        if (drawing_ptr == drawings.end()) {
          drawing_ptr--;
        }
      }

      // render fundo
      SDL_Rect dstrect;
      list<draws>::iterator ptr;
      SDL_RenderClear(renderer);
      SDL_RenderCopy(renderer, fundo, NULL, NULL);
      //***draw buttons

      // previous
      if (drawing_ptr != drawings.begin()) {
        if (prev(drawing_ptr) != drawings.begin()) {
          // n-2 exists
          ptr = drawing_ptr;
          ptr--;
          ptr--;
          dstrect.w = ptr->surface->w / 2.5;
          dstrect.h = ptr->surface->h / 2.5;
          dstrect.x = (WIDTH - dstrect.w) / 8;
          dstrect.y = (HEIGHT - dstrect.h) / 2;
          SDL_RenderCopy(renderer, ptr->texture, NULL, &dstrect);
        }
        // n-1 exists
        ptr = drawing_ptr;
        ptr--;
        dstrect.w = ptr->surface->w / 2;
        dstrect.h = ptr->surface->h / 2;
        dstrect.x = (WIDTH - dstrect.w) / 4;
        dstrect.y = (HEIGHT - dstrect.h) / 2;
        SDL_RenderCopy(renderer, ptr->texture, NULL, &dstrect);
      }

      // next
      if (drawing_ptr != --drawings.end()) {
        if (next(drawing_ptr) != --drawings.end()) {
          // n+2 exists
          ptr = drawing_ptr;
          ptr++;
          ptr++;
          dstrect.w = ptr->surface->w / 2.5;
          dstrect.h = ptr->surface->h / 2.5;
          dstrect.x = ((WIDTH - dstrect.w) / 8) * 7;
          dstrect.y = (HEIGHT - dstrect.h) / 2;
          SDL_RenderCopy(renderer, ptr->texture, NULL, &dstrect);
        }
        // n+1 exists
        ptr = drawing_ptr;
        ptr++;
        dstrect.w = ptr->surface->w / 2;
        dstrect.h = ptr->surface->h / 2;
        dstrect.x = ((WIDTH - dstrect.w) / 4) * 3;
        dstrect.y = (HEIGHT - dstrect.h) / 2;
        SDL_RenderCopy(renderer, ptr->texture, NULL, &dstrect);
      }

      // position of main draw
      dstrect.w = drawing_ptr->surface->w / 1.5;
      dstrect.h = drawing_ptr->surface->h / 1.5;
      dstrect.x = (WIDTH - dstrect.w) / 2;
      dstrect.y = (HEIGHT - dstrect.h) / 2;
      SDL_RenderCopy(renderer, drawing_ptr->texture, NULL, &dstrect);

      // show
      SDL_RenderPresent(renderer);

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
        if (i.texture)
          SDL_DestroyTexture(i.texture);
        if (i.name)
          delete[] i.name;
        drawings.pop_front();
      }

      SDL_DestroyTexture(fundo);

      if (draw_continue) {
        // exit to game

        PHASE = MAIN_INIT;
      } else {
        // exit to menu

        PHASE = START_INIT;
      }
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
      tcursor = load_texture(renderer, DATA_PATH "CURSOR" GRAPH_EXT);

      int cw, ch;
      SDL_QueryTexture(tcursor, NULL, NULL, &cw, &ch);

      cursor.x = WIDTH / 2;
      cursor.y = HEIGHT / 2;
      cursor.w = cw;
      cursor.h = ch;

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
        BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_TRIANGLE)
        save_png(renderer, field);

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
        SDL_DestroyTexture(aux);
        aux = SDL_CreateTextureFromSurface(renderer, exit_surface);
        exit_screen = true;
      }

      // show help screen
      if ((help_show) && (!help_screen)) {
        SDL_DestroyTexture(aux);
        aux = SDL_CreateTextureFromSurface(renderer, help_surface);
        help_screen = true;
      }

      // show help screen
      if ((credit_show) && (!credit_screen)) {
        SDL_DestroyTexture(aux);
        aux = SDL_CreateTextureFromSurface(renderer, credit_surface);
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
          effect_play(SOUND_MAGIC);
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
        d_pos.x = (WIDTH - draw->w) / 2;
        d_pos.y = 0;
        d_pos.w = draw->w;
        d_pos.h = draw->h;

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
      SDL_RenderClear(renderer);
      if (redraw) {
        // recreate aux texture
        SDL_DestroyTexture(aux);
        aux = SDL_CreateTextureFromSurface(renderer, field);
        redraw = false;
      }
      SDL_RenderCopy(renderer, aux, NULL, NULL);
      SDL_RenderCopy(renderer, tcursor, NULL, &cursor);
      SDL_RenderPresent(renderer);
    } break;

    case MAIN_END: {
      // free
      SDL_FreeSurface(draw);
      SDL_FreeSurface(exit_surface);
      SDL_FreeSurface(help_surface);
      SDL_FreeSurface(credit_surface);
      SDL_FreeSurface(field);
      SDL_DestroyTexture(aux);
      SDL_DestroyTexture(tcursor);
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
  // cleanup
  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  SDL_DestroyRenderer(renderer);

  SDL_DestroyWindow(window);

  sound_end();

  if (joystick) {
    SDL_JoystickClose(joystick);
  }

  TTF_CloseFont(font);
  TTF_Quit();

  IMG_Quit();

  SDL_Quit();
  dbglogger_stop();

  return 0;
}
