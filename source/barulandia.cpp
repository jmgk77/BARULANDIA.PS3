#include "barulandia.h"

int main(int argc, char **argv) {
  dbglogger_init_str("tcp:" DBG_IP ":" DBG_PORT);
  dbglogger_log("BARULANDIA for ps3 (c) jmgk 2020");

#ifdef DEBUG
  atexit(ret2psload);
#endif

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  // init sdl
  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
    dbglogger_log("SDL_Init: %s", SDL_GetError());
    return -1;
  }

#ifdef USE_PNG
  // init sdl_image
  int flags = IMG_INIT_JPG | IMG_INIT_PNG;
  int initted = IMG_Init(flags);
  if ((initted & flags) != flags) {
    dbglogger_log("IMG_Init: %s\n", IMG_GetError());
    return -1;
  }
#endif

#ifdef PS3
  // init joystick
  SDL_Joystick *joystick = SDL_JoystickOpen(0);

  // joystick button state
  bool joystick_buttonstate[JOYBUTTONS];
  bool joystick_oldbuttonstate[JOYBUTTONS];
  for (int i = 0; i < JOYBUTTONS; ++i) {
    joystick_oldbuttonstate[i] = joystick_buttonstate[i] = false;
  }
#endif

  // init screen
  SDL_Window *window = SDL_CreateWindow(
      "BARULANDIA.PS3", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0,
      SDL_WINDOW_FULLSCREEN_DESKTOP);
  if (window == NULL) {
    dbglogger_log("SDL_CreateWindow: %s\n", SDL_GetError());
    return -1;
  }

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
  if (renderer == NULL) {
    dbglogger_log("SDL_CreateRenderer: %s", SDL_GetError());
    return -1;
  }

  // print info
  debug_video();

  // white is our "clear screen' color
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

  // set "virtual" size
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
  SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  // load logo
  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
#ifndef SKIP_INTRO
  SDL_Texture *logo = load_texture(renderer, DATA_PATH "LOGO" GRAPH_EXT);

  // fade logo in and out
  fade_in_out(renderer, logo, true, true);
  fade_in_out(renderer, logo, true, false);

  // free logo
  SDL_DestroyTexture(logo);
#endif
  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  // load start screen
  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
#ifndef SKIP_STARTSCREEN
  SDL_Texture *fundo = load_texture(renderer, DATA_PATH "FUNDO" GRAPH_EXT);
  SDL_Texture *titulo = load_texture(renderer, DATA_PATH "TITULO" GRAPH_EXT);
  SDL_Texture *start = load_texture(renderer, DATA_PATH "START" GRAPH_EXT);

  // fade start screen in
  fade_in_out(renderer, fundo, false, true);

  // reset alpha
  SDL_SetTextureAlphaMod(fundo, SDL_ALPHA_OPAQUE);

  // init positions
  SDL_Rect t_pos, s_pos;

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

  int t_pos_x_max = t_pos.x + 50;
  int t_pos_x_min = t_pos.x - 50;
  int t_pos_x_delta = -1;

  int s_pos_y_max = s_pos.y + 20;
  int s_pos_y_min = s_pos.y - 20;
  int s_pos_y_delta = -1;

  // start screen loop
  bool start_active = true;

  while (start_active) {

#ifdef PS3
    if (joystick) {
      SDL_JoystickUpdate();
      if (SDL_JoystickGetButton(joystick, SDL_CONTROLLER_BUTTON_START) ==
          SDL_PRESSED) {
        start_active = false;
      }
    }
#else
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        start_active = false;
      }
      if ((e.type == SDL_KEYDOWN) && (e.key.keysym.sym == SDLK_ESCAPE)) {
        start_active = false;
      }
    }
#endif

    // move
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
  }

  fade_in_out(renderer, fundo, false, false);

  SDL_DestroyTexture(fundo);
  SDL_DestroyTexture(titulo);
  SDL_DestroyTexture(start);

#endif

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
  CURSOR_AREA(172, 565, 322, 707) { draw_refresh = true; } /*refresh draw*/    \
  CURSOR_AREA(172, 408, 322, 538) {                        /*eraser*/          \
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

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// main loop
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#ifndef SKIP_MAIN
  // init cursor
  SDL_Texture *tcursor = load_texture(renderer, DATA_PATH "CURSOR" GRAPH_EXT);

  int cw, ch;
  SDL_QueryTexture(tcursor, NULL, NULL, &cw, &ch);

  SDL_Rect cursor;
  cursor.x = WIDTH / 2;
  cursor.y = HEIGHT / 2;
  cursor.w = cw;
  cursor.h = ch;

  // draw control
  int draw_current = 0;
  int draw_new = 0;
  bool draw_refresh = true;

  // cursor movement
  int dx = 0;
  int dy = 0;
  int accell = 0;

  // load resources
  SDL_Surface *field = load_surface(DATA_PATH "FIELD" GRAPH_EXT);
  SDL_Surface *draw = NULL;

  // colors (default green)
  Uint32 color_current = -1;
  Uint32 color_new = SDL_MapRGBA(field->format, 0, 255, 0, SDL_ALPHA_OPAQUE);

  // surface to texture
  bool redraw = true;
  SDL_Texture *aux = NULL;

  // exit confirmation stuff
  bool exit_show = false;
  bool exit_screen = false;
  SDL_Surface *exit_surface = load_surface(DATA_PATH "EXIT" GRAPH_EXT);

  // exit confirmation stuff
  bool help_show = false;
  bool help_screen = false;
  SDL_Surface *help_surface = load_surface(DATA_PATH "HELP" GRAPH_EXT);

  // main loop
  bool active = true;

  while (active) {

#define BUTTON_CHANGE(x)                                                       \
  if (joystick_oldbuttonstate[x] != joystick_buttonstate[x])
#define BUTTON_PRESSED(x)                                                      \
  if (!joystick_oldbuttonstate[x] && joystick_buttonstate[x])
#define BUTTON_RELEASED(x)                                                     \
  if (joystick_oldbuttonstate[x] && !joystick_buttonstate[x])

#ifdef PS3
    // handle joystick for PS3
    if (joystick) {
      SDL_JoystickUpdate();
      /*debug_joystick(joystick);*/
      for (int i = 0; i < JOYBUTTONS; ++i) {
        joystick_oldbuttonstate[i] = joystick_buttonstate[i];
        joystick_buttonstate[i] = SDL_JoystickGetButton(joystick, i);
      }

      // exit screen button (confirm)
      if (exit_show) {
        BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_CROSS) active = false;
      }

      // exit & help screen button (back)
      if (exit_show || help_show) {
        BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_CIRCLE) {
          exit_show = false;
          exit_screen = false;
          help_show = false;
          help_screen = false;
          redraw = true;
        }
      } else {
        //normal buttons...
        // show exit screen
        BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_START) exit_show = true;

        // show help screen
        BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_SELECT) help_show = true;

        // change drawing
        BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_L1) draw_new--;
        BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_R1) draw_new++;

        // clicks
        BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_CROSS) { CLICK_AREA }

        // update cursor (joystick L)
        int _x = SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTX);
        int _y = SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTY);
        dx = (_x > AXIS_DEADZONE) ? 1 : (_x < -AXIS_DEADZONE) ? -1 : 0;
        dy = (_y > AXIS_DEADZONE) ? 1 : (_y < -AXIS_DEADZONE) ? -1 : 0;
      }
    }
#else
    // handle sdl events
    SDL_Event e;
    while (SDL_PollEvent(&e)) {

      switch (e.type) {

      case SDL_QUIT:
        exit_show = true;
        break;
        // handle keypresses for linux
      case SDL_KEYDOWN:
        dbglogger_printf("SDL_KEYDOWN: %s\n",SDL_GetKeyName(e.key.keysym.sym));
        switch (e.key.keysym.sym) {
        case SDLK_ESCAPE:
          exit_show = true;
          break;
        // keydown only for continuous input (movement)
        case SDLK_LEFT:
          dx = -1;
          break;
        case SDLK_RIGHT:
          dx = 1;
          break;
        case SDLK_UP:
          dy = -1;
          break;
        case SDLK_DOWN:
          dy = 1;
          break;
        ////
        default:
          break;
        }
        break;
      case SDL_KEYUP:
        dbglogger_printf("SDL_UP: %s\n", SDL_GetKeyName(e.key.keysym.sym));
        switch (e.key.keysym.sym) {
          // cut keydown (movement)
        case SDLK_LEFT:
          dx = 0;
          break;
        case SDLK_RIGHT:
          dx = 0;
          break;
        case SDLK_UP:
          dy = 0;
          break;
        case SDLK_DOWN:
          dy = 0;
          break;
          ////
        case SDLK_TAB:
          help_show = true;
          break;
        // Y & N exit screen
        case SDLK_x:
          if (exit_show)
            active = false;
          break;
        case SDLK_o:
          if (exit_show || help_show) {
            exit_show = false;
            exit_screen = false;
            help_show = false;
            help_screen = false;
            redraw = true;
          }
          break;
        // L1 & R1
        case SDLK_LCTRL:
          draw_new--;
          break;
        case SDLK_RCTRL:
          draw_new++;
          break;
          // X
        case SDLK_SPACE:
          CLICK_AREA
          break;
        default:
          break;
        }
        break;
      default:
        break;
      }
    }
#endif

    // exit screen
    if ((exit_show) && (!exit_screen)) {
      SDL_DestroyTexture(aux);
      aux = SDL_CreateTextureFromSurface(renderer, exit_surface);
      exit_screen = true;
    }

    // help screen
    if ((help_show) && (!help_screen)) {
      SDL_DestroyTexture(aux);
      aux = SDL_CreateTextureFromSurface(renderer, help_surface);
      help_screen = true;
    }

    // start/stop acceleration
    if (!dx && !dy) {
      accell = 0;
    } else {
      accell++;
    }

    // limit acceleration
    if (accell > 60) {
      accell = 60;
    }

    // accelerate cursor
    cursor.x += dx + (dx * (accell / 10));
    cursor.y += dy + (dy * (accell / 10));

    // nextdraw boundaries check (cicle)
    draw_new =
        (draw_new < 0) ? (MAX_DRAW - 1) : (draw_new == MAX_DRAW) ? 0 : draw_new;

    // screen boundaries check (limit)
    cursor.x =
        (cursor.x < 0) ? 0 : (cursor.x >= WIDTH - 1) ? (WIDTH - 1) : cursor.x;
    cursor.y =
        (cursor.y < 0) ? 0 : (cursor.y >= HEIGHT - 1) ? (HEIGHT - 1) : cursor.y;

    // change/reload current drawing
    if ((draw_current != draw_new) || (draw_refresh)) {
      draw_current = draw_new;
      draw_refresh = false;

      // free old
      SDL_FreeSurface(draw);

      // load new
      char buf[128];
      snprintf(buf, 128, "%sDRAW%d%s", DATA_PATH, draw_current + 1, GRAPH_EXT);
      draw = load_surface(buf);

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
  }

  // free
  SDL_FreeSurface(draw);
  SDL_FreeSurface(exit_surface);
  SDL_FreeSurface(help_surface);
  SDL_FreeSurface(field);
  SDL_DestroyTexture(aux);
  SDL_DestroyTexture(tcursor);

#endif
  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  // cleanup
  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  SDL_DestroyRenderer(renderer);

#ifdef PS3
  SDL_JoystickClose(joystick);
#endif

#ifdef USE_PNG
  IMG_Quit();
#endif
  SDL_Quit();
  dbglogger_stop();

  return 0;
}
