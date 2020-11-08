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

  // mikmod init
  sound_init();

  // joystick/keyboard button state
  bool joystick_buttonstate[JOYBUTTONS];
  bool joystick_oldbuttonstate[JOYBUTTONS];
  for (int i = 0; i < JOYBUTTONS; ++i) {
    joystick_oldbuttonstate[i] = joystick_buttonstate[i] = false;
  }

  // init joystick
  SDL_Joystick *joystick = SDL_JoystickOpen(0);

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
#define BUTTON_RELEASED(x)                                                     \
  if (joystick_oldbuttonstate[x] && !joystick_buttonstate[x])

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  // variables
  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  // intro vars
  SDL_Texture *logo;

  // startscreen vars
  SDL_Texture *fundo, *titulo, *start;
  SDL_Rect t_pos, s_pos;
  int t_pos_x_max, t_pos_x_min, t_pos_x_delta, s_pos_y_max, s_pos_y_min,
      s_pos_y_delta;

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
#ifndef PS3
      // read keyboard state
      if ((e.type == SDL_KEYUP) || (e.type == SDL_KEYDOWN)) {
        bool keypressed = (e.type == SDL_KEYDOWN) ? true : false;
        // save joystick status
        for (int i = 0; i < JOYBUTTONS; ++i) {
          joystick_oldbuttonstate[i] = joystick_buttonstate[i];
        }
        int keyname;
        switch (e.key.keysym.sym) {
        case SDLK_DOWN:
          keyname = SDL_CONTROLLER_BUTTON_DOWN;
          break;
        case SDLK_UP:
          keyname = SDL_CONTROLLER_BUTTON_UP;
          break;
        case SDLK_RIGHT:
          keyname = SDL_CONTROLLER_BUTTON_RIGHT;
          break;
        case SDLK_LEFT:
          keyname = SDL_CONTROLLER_BUTTON_LEFT;
          break;
        case SDLK_1:
          keyname = SDL_CONTROLLER_BUTTON_START;
          break;
        case SDLK_2:
          keyname = SDL_CONTROLLER_BUTTON_SELECT;
          break;
        case SDLK_c:
          keyname = SDL_CONTROLLER_BUTTON_TRIANGLE;
          break;
        case SDLK_x:
          keyname = SDL_CONTROLLER_BUTTON_CROSS;
          break;
        case SDLK_z:
          keyname = SDL_CONTROLLER_BUTTON_CIRCLE;
          break;
        case SDLK_v:
          keyname = SDL_CONTROLLER_BUTTON_SQUARE;
          break;
        case SDLK_LCTRL:
          keyname = SDL_CONTROLLER_BUTTON_L1;
          break;
        case SDLK_RCTRL:
          keyname = SDL_CONTROLLER_BUTTON_R1;
          break;
        case SDLK_LSHIFT:
          keyname = SDL_CONTROLLER_BUTTON_L2;
          break;
        case SDLK_RSHIFT:
          keyname = SDL_CONTROLLER_BUTTON_R2;
          break;
        default:
          keyname = -1;
          break;
        }
        // simulate joystick input
        if (keyname != -1) {
          joystick_buttonstate[keyname] = keypressed;
        }
      }
#endif
    }

    // read joystick state
    if (joystick) {
      SDL_JoystickUpdate();
      /*debug_joystick(joystick);*/
      for (int i = 0; i < JOYBUTTONS; ++i) {
        joystick_oldbuttonstate[i] = joystick_buttonstate[i];
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

      t_pos_x_max = t_pos.x + 50;
      t_pos_x_min = t_pos.x - 50;
      t_pos_x_delta = -1;

      s_pos_y_max = s_pos.y + 20;
      s_pos_y_min = s_pos.y - 20;
      s_pos_y_delta = -1;
      PHASE = START_MAIN;
    } break;

    case START_MAIN: {
      t_pos.x += t_pos_x_delta;
      if ((t_pos.x < t_pos_x_min) || (t_pos.x > t_pos_x_max)) {
        t_pos_x_delta = -t_pos_x_delta;
      }
      s_pos.y += s_pos_y_delta;
      if ((s_pos.y < s_pos_y_min) || (s_pos.y > s_pos_y_max)) {
        s_pos_y_delta = -s_pos_y_delta;
      }

      // clear and paste fundo, titulo, start
      SDL_RenderClear(renderer);
      SDL_RenderCopy(renderer, fundo, NULL, NULL);
      SDL_RenderCopy(renderer, titulo, NULL, &t_pos);
      SDL_RenderCopy(renderer, start, NULL, &s_pos);

      // render
      SDL_RenderPresent(renderer);

      BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_START) PHASE = START_END;

    } break;

    case START_END: {
      set_alpha_rate(30);
      fade_in_out(renderer, fundo, false, false);
      SDL_DestroyTexture(fundo);
      SDL_DestroyTexture(titulo);
      SDL_DestroyTexture(start);
      PHASE = MAIN_INIT;
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
      dx = dy = accell = 0;

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
      int _x = SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTX);
      int _y = SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTY);
      dx = (_x > AXIS_DEADZONE) ? 1 : (_x < -AXIS_DEADZONE) ? -1 : 0;
      dy = (_y > AXIS_DEADZONE) ? 1 : (_y < -AXIS_DEADZONE) ? -1 : 0;

#ifndef PS3
      //dx = dy = 0;
      BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_RIGHT) dx = 1;
      BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_LEFT) dx = -1;
      BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_DOWN) dy = 1;
      BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_UP) dy = -1;
#endif

      // start/stop acceleration
      if (!dx && !dy) {
        accell = 0;
      } else {
        // limit acceleration
        if (accell < 60) {
          accell++;
        }
      }

      // accelerate cursor
      cursor.x += dx + (dx * (accell / 10));
      cursor.y += dy + (dy * (accell / 10));

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

        // load new
        char buf[128];
        snprintf(buf, 128, "%sDRAW%d%s", DATA_PATH, draw_current + 1,
                 GRAPH_EXT);
        draw = load_surface(buf);

        // play sound effect
        effect_play(draw_current + 1);

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
      PHASE = FINISHED;
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

  sound_end();

  SDL_JoystickClose(joystick);

  TTF_CloseFont(font);
  TTF_Quit();

  IMG_Quit();

  SDL_Quit();
  dbglogger_stop();

  return 0;
}
