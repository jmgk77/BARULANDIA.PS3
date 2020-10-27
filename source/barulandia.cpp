#include "barulandia.h"

int main(int argc, char **argv) {
  dbglogger_init_str("tcp:" DBG_IP ":" DBG_PORT);
  dbglogger_log("BARULANDIA for ps3 (c) jmgk 2020");
  atexit(ret2psload);

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

  // init joystick
  SDL_Joystick *joystick = SDL_JoystickOpen(0);

  // joystick button state
  bool joystick_buttonstate[JOYBUTTONS];
  bool joystick_oldbuttonstate[JOYBUTTONS];
  for (int i = 0; i < JOYBUTTONS; ++i) {
    joystick_oldbuttonstate[i] = joystick_buttonstate[i] = false;
  }

  // init screen
  SDL_Surface *screen = SDL_SetVideoMode(
      WIDTH, HEIGHT, 0, SDL_SWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
  if (screen == NULL) {
    dbglogger_log("SDL_SetVideoMode: %s", SDL_GetError());
    return -1;
  }

  // video info
  debug_video();

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  // load logo
  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
#ifndef SKIP_INTRO
  SDL_Surface *logo = load_resource(DATA_PATH "LOGO" GRAPH_EXT);

  // fade logo in and out
  fade_in_out(screen, logo, true);
  dbglogger_screenshot_tmp(255);
  fade_in_out(screen, logo, false);
  SDL_FreeSurface(logo);
#endif
  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  // load start screen
  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
#ifndef SKIP_STARTSCREEN
  SDL_Surface *fundo = load_resource(DATA_PATH "FUNDO" GRAPH_EXT);
  SDL_Surface *titulo = load_resource(DATA_PATH "TITULO" GRAPH_EXT);
  SDL_Surface *start = load_resource(DATA_PATH "START" GRAPH_EXT);

  // fade start screen in
  fade_in_out(screen, fundo, true);

  // init position
  SDL_Rect t_pos, s_pos;

  t_pos.x = (screen->w - titulo->w) / 2;
  t_pos.y = (screen->h / 4) * 1;
  t_pos.w = titulo->w;
  t_pos.h = titulo->h;

  s_pos.x = (screen->w - start->w) / 2;
  s_pos.y = (screen->h / 4) * 3;
  s_pos.w = start->w;
  s_pos.h = start->h;

  int t_pos_x_max = t_pos.x + 50;
  int t_pos_x_min = t_pos.x - 50;
  int t_pos_x_delta = -1;

  int s_pos_y_max = s_pos.y + 20;
  int s_pos_y_min = s_pos.y - 20;
  int s_pos_y_delta = -1;

  // start screen loop
  bool start_active = true;
  while (start_active) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        start_active = false;
      }
      if ((e.type == SDL_KEYDOWN) && (e.key.keysym.sym == SDLK_ESCAPE)) {
        start_active = false;
        dbglogger_screenshot_tmp(255);
      }
    }

    if (joystick) {
      SDL_JoystickUpdate();
      if (SDL_JoystickGetButton(joystick, SDL_CONTROLLER_BUTTON_START) ==
          SDL_PRESSED) {
        start_active = false;
        dbglogger_screenshot_tmp(255);
      }
    }

    // move
    t_pos.x += t_pos_x_delta;
    if ((t_pos.x < t_pos_x_min) || (t_pos.x > t_pos_x_max)) {
      t_pos_x_delta = -t_pos_x_delta;
    }
    s_pos.y += s_pos_y_delta;
    if ((s_pos.y < s_pos_y_min) || (s_pos.y > s_pos_y_max)) {
      s_pos_y_delta = -s_pos_y_delta;
    }

    // paste fundo
    SDL_BlitSurface(fundo, NULL, screen, NULL);
    // paste barulandia
    SDL_BlitSurface(titulo, NULL, screen, &t_pos);
    // paste start button
    SDL_BlitSurface(start, NULL, screen, &s_pos);

    // render
    SDL_Flip(screen);

    SDL_Delay(10);
  }

  SDL_FreeSurface(fundo);
  SDL_FreeSurface(titulo);
  SDL_FreeSurface(start);

  // black screen
  SDL_FillRect(screen, NULL, SDL_MapRGBA(screen->format, 0, 0, 0, 255));
  SDL_Flip(screen);
#endif

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  // click control
  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

#define CURSOR_AREA(x1, y1, x2, y2)                                            \
  if ((cursor_x > x1) && (cursor_y > y1) && (cursor_x < x2) && (cursor_y < y2))

#define CLICK_AREA                                                             \
  CURSOR_AREA(172, 17, 322, 147) { draw_new--; } /*prev draw*/                 \
  CURSOR_AREA(955, 17, 1110, 147) {              /*next draw*/                 \
    draw_new++;                                                                \
    dbglogger_screenshot_tmp(255);                                             \
  }                                                                            \
  CURSOR_AREA(172, 565, 322, 707) { draw_refresh = true; } /*refresh draw*/    \
  CURSOR_AREA(172, 408, 322, 538) {                        /*eraser*/          \
    new_color = SDL_MapRGBA(screen->format, 255, 255, 255, 255);               \
  }                                                                            \
  CURSOR_AREA(955, 408, 1110, 538) {} /*paint tool*/                           \
  CURSOR_AREA(1140, 15, 1270, 705) {  /*color picker*/                         \
    if (under_cursor_color != SDL_MapRGBA(screen->format, 0, 0, 0, 255)) {     \
      new_color = under_cursor_color;                                          \
    }                                                                          \
  }                                                                            \
  CURSOR_AREA(353, 1, 927, 719) { /*paint*/ /*restore content under cursor*/   \
    SDL_Rect c_pos;                                                            \
    c_pos.x = old_cursor_x;                                                    \
    c_pos.y = old_cursor_y;                                                    \
    c_pos.w = old_cursor->w;                                                   \
    c_pos.h = old_cursor->h;                                                   \
    SDL_BlitSurface(old_cursor, NULL, screen, &c_pos);                         \
    floodfill(screen, cursor_x, cursor_y,                                      \
              current_color); /* save content under cursor*/                   \
    SDL_BlitSurface(screen, &c_pos, old_cursor, NULL);                         \
    under_cursor_color = current_color;                                        \
  }

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  // main loop
  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  // init cursor
  int cursor_x = screen->w / 2;
  int cursor_y = screen->h / 2;
  int old_cursor_x;
  int old_cursor_y;
  SDL_Surface *cursor = load_resource(DATA_PATH "CURSOR" GRAPH_EXT);
  SDL_Surface *old_cursor = load_resource(DATA_PATH "CURSOR" GRAPH_EXT);

  // draw control
  int draw_current = 0;
  int draw_new = 0;
  bool draw_refresh = true;

  // main loop
  bool active = true;
  int dx = 0;
  int dy = 0;
  int accell = 0;

  // colors (default green)
  Uint32 current_color = -1;
  Uint32 under_cursor_color = -1;
  Uint32 new_color = SDL_MapRGBA(screen->format, 0, 255, 0, 255);

  // load resources
  SDL_Surface *field = load_resource(DATA_PATH "FIELD" GRAPH_EXT);
  SDL_Surface *draw = NULL;

  while (active) {

    old_cursor_x = cursor_x;
    old_cursor_y = cursor_y;

#ifndef PS3
    // handle sdl events
    SDL_Event e;
    while (SDL_PollEvent(&e)) {

      switch (e.type) {

      case SDL_QUIT:
        active = false;
        break;
        // handle keypresses for linux
      case SDL_KEYDOWN:
        /*dbglogger_printf("SDL_KEYDOWN:
         * %s\n",SDL_GetKeyName(e.key.keysym.sym));*/
        switch (e.key.keysym.sym) {
        case SDLK_ESCAPE:
          active = false;
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
        /*dbglogger_printf("SDL_UP: %s\n", SDL_GetKeyName(e.key.keysym.sym));*/
        switch (e.key.keysym.sym) {
          // cut keydown (movement)
        case SDLK_LEFT:
          if (dx < 0) {
            dx = 0;
          }
          break;
        case SDLK_RIGHT:
          if (dx > 0) {
            dx = 0;
          }
          break;
        case SDLK_UP:
          if (dy < 0) {
            dy = 0;
          }
          break;
        case SDLK_DOWN:
          if (dy > 0) {
            dy = 0;
          }
          break;
          ////
        // L1 & R1
        case SDLK_LCTRL:
          draw_new--;
          break;
        case SDLK_RCTRL:
          draw_new++;
          dbglogger_screenshot_tmp(255);
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

    cursor_x += dx;
    cursor_y += dy;
#endif

#ifdef PS3
    // handle joystick for PS3
    if (joystick) {
      SDL_JoystickUpdate();
      /*debug_joystick(joystick);*/
      for (int i = 0; i < JOYBUTTONS; ++i) {
        joystick_oldbuttonstate[i] = joystick_buttonstate[i];
        joystick_buttonstate[i] = SDL_JoystickGetButton(joystick, i);
      }

#define BUTTON_CHANGE(x)                                                       \
  if (joystick_oldbuttonstate[x] != joystick_buttonstate[x])
#define BUTTON_PRESSED(x)                                                      \
  if (!joystick_oldbuttonstate[x] && joystick_buttonstate[x])
#define BUTTON_RELEASED(x)                                                     \
  if (joystick_oldbuttonstate[x] && !joystick_buttonstate[x])

      // exit
      BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_START) active = false;

      // change drawing
      BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_L1) {
        draw_new--;
        if (draw_new < 0) {
          draw_new = MAX_DRAW - 1;
        }
      }
      BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_R1) {
        draw_new++;
        if (draw_new == MAX_DRAW) {
          draw_new = 0;
        }
      }
      BUTTON_PRESSED(SDL_CONTROLLER_BUTTON_CROSS) { CLICK_AREA }

      // update cursor (joystick L)
      int _x = SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTX);
      int _y = SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTY);
      dx = (_x > AXIS_DEADZONE) ? 1 : (_x < -AXIS_DEADZONE) ? -1 : 0;
      dy = (_y > AXIS_DEADZONE) ? 1 : (_y < -AXIS_DEADZONE) ? -1 : 0;
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
      cursor_x += dx + (dx * (accell / 10));
      cursor_y += dy + (dy * (accell / 10));
    }
#endif

    // nextdraw boundaries check (circle)
    draw_new =
        (draw_new < 0) ? (MAX_DRAW - 1) : (draw_new == MAX_DRAW) ? 0 : draw_new;

    // screen boundaries check (limit)
    cursor_x = (cursor_x < 0)
                   ? 0
                   : (cursor_x >= screen->w - 1) ? (screen->w - 1) : cursor_x;
    cursor_y = (cursor_y < 0)
                   ? 0
                   : (cursor_y >= screen->h - 1) ? (screen->h - 1) : cursor_y;

    // change/reload current drawing
    if ((draw_current != draw_new) || (draw_refresh)) {
      draw_current = draw_new;
      draw_refresh = false;

      if (draw) {
        SDL_FreeSurface(draw);
      }

      char buf[128];
      snprintf(buf, 128, "%sDRAW%d%s", DATA_PATH, draw_current + 1, GRAPH_EXT);

      draw = load_resource(buf);

      // set position
      SDL_Rect d_pos;
      d_pos.x = (screen->w - draw->w) / 2;
      d_pos.y = 0;
      d_pos.w = draw->w;
      d_pos.h = draw->h;

      // paste field
      SDL_BlitSurface(field, NULL, screen, NULL);

      // paste new drawing in position
      SDL_BlitSurface(draw, NULL, screen, &d_pos);

      // save content under cursor
      SDL_Rect c_pos;
      c_pos.x = old_cursor_x;
      c_pos.y = old_cursor_y;
      c_pos.w = old_cursor->w;
      c_pos.h = old_cursor->h;
      SDL_BlitSurface(screen, &c_pos, old_cursor, NULL);

      // redo color picker sample
      current_color = -1;
    }

    // change current color
    if (current_color != new_color) {
      current_color = new_color;
      floodfill(screen, 1000, 650, new_color);
    }

    // restore content under cursor
    SDL_Rect c_pos;
    c_pos.x = old_cursor_x;
    c_pos.y = old_cursor_y;
    c_pos.w = old_cursor->w;
    c_pos.h = old_cursor->h;
    SDL_BlitSurface(old_cursor, NULL, screen, &c_pos);

    // save color under cursor
    SDL_LockSurface(screen);
    under_cursor_color = GetPixel32_nolock(screen, cursor_x, cursor_y);
    SDL_UnlockSurface(screen);

    // save content under cursor
    c_pos.x = cursor_x;
    c_pos.y = cursor_y;
    c_pos.w = old_cursor->w;
    c_pos.h = old_cursor->h;
    SDL_BlitSurface(screen, &c_pos, old_cursor, NULL);

    // draw cursors
    c_pos.x = cursor_x;
    c_pos.y = cursor_y;
    c_pos.w = cursor->w;
    c_pos.h = cursor->h;
    SDL_BlitSurface(cursor, NULL, screen, &c_pos);

    // render
    SDL_Flip(screen);
  }

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  // cleanup
  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  SDL_JoystickClose(joystick);
#ifdef USE_PNG
  IMG_Quit();
#endif
  SDL_Quit();
  dbglogger_stop();

  return 0;
}
