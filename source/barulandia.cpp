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
  SDL_Surface *logo = load_resource(DATA_PATH "LOGO.PNG");

  // fade logo in and out
  fade_in_out(screen, logo, true);
  fade_in_out(screen, logo, false);
  SDL_FreeSurface(logo);

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  // load start screen
  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  SDL_Surface *fundo = load_resource(DATA_PATH "FUNDO.PNG");
  SDL_Surface *titulo = load_resource(DATA_PATH "TITULO.PNG");
  SDL_Surface *start = load_resource(DATA_PATH "START.PNG");

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
      }
    }

    if (joystick) {
      SDL_JoystickUpdate();
      if (SDL_JoystickGetButton(joystick, SDL_CONTROLLER_BUTTON_START) ==
          SDL_PRESSED) {
        start_active = false;
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

    SDL_Flip(screen);

    SDL_Delay(10);
  }

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  // main loop
  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  // blank screen
  SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 255, 0, 0));

  // init cursors
  int cursor_x = WIDTH / 2;
  int cursor_y = HEIGHT / 2;
  int cursor2_x = WIDTH / 2;
  int cursor2_y = HEIGHT / 2;

  // main loop
  bool active = true;
  while (active) {
    // handle sdl events
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        active = false;
      }
      if ((e.type == SDL_KEYDOWN) && (e.key.keysym.sym == SDLK_ESCAPE)) {
        active = false;
      }
    }

    // handle joystick
    if (joystick) {
      SDL_JoystickUpdate();

      // debug_joystick(joystick);

      if (SDL_JoystickGetButton(joystick, SDL_CONTROLLER_BUTTON_START) ==
          SDL_PRESSED) {
        active = false;
      }

      int dx, dy;
      // update cursor (joystick L)
      dx = SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTX);
      cursor_x += (dx > AXIS_DEADZONE) ? 1 : (dx < -AXIS_DEADZONE) ? -1 : 0;
      dy = SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTY);
      cursor_y += (dy > AXIS_DEADZONE) ? 1 : (dy < -AXIS_DEADZONE) ? -1 : 0;
      // update cursor (joystick R)
      dx = SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_RIGHTX);
      cursor2_x += (dx > AXIS_DEADZONE) ? 1 : (dx < -AXIS_DEADZONE) ? -1 : 0;
      dy = SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_RIGHTY);
      cursor2_y += (dy > AXIS_DEADZONE) ? 1 : (dy < -AXIS_DEADZONE) ? -1 : 0;
    }

    // screen boundaries check
    cursor_x = (cursor_x < 0) ? WIDTH : (cursor_x > WIDTH - 1) ? 0 : cursor_x;
    cursor_y =
        (cursor_y < 0) ? HEIGHT - 1 : (cursor_y > HEIGHT - 1) ? 0 : cursor_y;
    cursor2_x =
        (cursor2_x < 0) ? WIDTH : (cursor2_x > WIDTH - 1) ? 0 : cursor2_x;
    cursor2_y =
        (cursor2_y < 0) ? HEIGHT - 1 : (cursor2_y > HEIGHT - 1) ? 0 : cursor2_y;

    // draw cursors
    SDL_LockSurface(screen);
    PutPixel32_nolock(screen, cursor_x, cursor_y,
                      SDL_MapRGB(screen->format, 0, 0, 255));
    PutPixel32_nolock(screen, cursor2_x, cursor2_y,
                      SDL_MapRGB(screen->format, 0, 255, 0));
    SDL_UnlockSurface(screen);

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
