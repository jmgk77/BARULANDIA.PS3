// barulandia for ps3 (c) jmgk 2020

#include "sdl_help.h"

#define DBG_IP "192.168.0.11"
#define DBG_PORT "18194"

const int WIDTH = 1280, HEIGHT = 720;

int main(int argc, char **argv) {
    dbglogger_init_str("tcp:" DBG_IP ":" DBG_PORT);
    dbglogger_log("barulandia for ps3 (c) jmgk 2020");

    // init sdl
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
        dbglogger_log("SDL_Init: %s", SDL_GetError());
        return -1;
    }

    // init sdl_image
    int flags = IMG_INIT_JPG | IMG_INIT_PNG;
    int initted = IMG_Init(flags);
    if ((initted & flags) != flags) {
        dbglogger_log("IMG_Init: %s\n", IMG_GetError());
        return -1;
    }

    // init joystick
    SDL_Joystick *joystick = SDL_JoystickOpen(0);

    // init screen
    SDL_Surface *screen = SDL_SetVideoMode(WIDTH, HEIGHT, 0,
                                           SDL_SWSURFACE | SDL_DOUBLEBUF |
                                           SDL_FULLSCREEN );
    if (screen == NULL) {
        dbglogger_log("SDL_SetVideoMode: %s", SDL_GetError());
        return -1;
    }

    // video info
    debug_video();

    // load logo
#ifdef PNG_LOAD
    SDL_Surface *tmp = IMG_Load("/dev_hdd0/game/SDL00BARU/LOGO.PNG");
    if (tmp == NULL) {
        dbglogger_log("IMG_Load: %s", SDL_GetError());
        return -1;
    }
#else
    SDL_Surface *tmp = SDL_LoadBMP("/dev_hdd0/game/SDL00BARU/LOGO.BMP");
    if (tmp == NULL) {
        dbglogger_log("SDL_LoadBMP: %s", SDL_GetError());
        return -1;
    }
#endif
    // Convert the logo to optimal display format
    SDL_Surface *logo = SDL_DisplayFormat(tmp);
    // Free the original bitmap
    SDL_FreeSurface(tmp);

    // center logo in display
    SDL_Rect r;
    r.x = (screen->w - logo->w) / 2;
    r.y = (screen->h - logo->h) / 2;
    r.w = logo->w;
    r.h = logo->h;

    // Create a blank surface that is the same size as our screen
    tmp = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, WIDTH, HEIGHT, 32,
                               rmask, gmask, bmask, amask);
    // Convert it to the format of the screen
    SDL_Surface *tmp_screen = SDL_DisplayFormat(tmp);
    // Free the created surface
    SDL_FreeSurface(tmp);

    // fade in
    for (int alpha = 0; alpha < 255; alpha++) {
        // Clear both the screens
        SDL_FillRect(tmp_screen, 0,
                     SDL_MapRGBA(tmp_screen->format, 255, 255, 255, 255));
        SDL_FillRect(screen, 0,
                     SDL_MapRGBA(tmp_screen->format, 255, 255, 255, 255));
        // Draw the bitmap to the constructed vitual screen
        SDL_BlitSurface(logo, NULL, tmp_screen, &r);
        // Set the alpha of the constructed screen
        SDL_SetAlpha(tmp_screen, SDL_SRCALPHA, alpha);
        // Draw the constructed surface to the primary surface now
        SDL_BlitSurface(tmp_screen, NULL, screen, 0);

        SDL_Flip(screen);
        SDL_Delay(20);
    }
    /**/

    SDL_Delay(3 * 1000);

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
        cursor_y = (cursor_y < 0) ? HEIGHT - 1 : (cursor_y > HEIGHT - 1) ? 0 : cursor_y;
        cursor2_x = (cursor2_x < 0) ? WIDTH : (cursor2_x > WIDTH - 1) ? 0 : cursor2_x;
        cursor2_y = (cursor2_y < 0) ? HEIGHT - 1 : (cursor2_y > HEIGHT - 1) ? 0 : cursor2_y;

        // draw cursors
        SDL_LockSurface(screen);
        PutPixel32_nolock(screen, cursor_x, cursor_y,
                          SDL_MapRGB(screen->format, 0, 0, 255));
        PutPixel32_nolock(screen, cursor2_x, cursor2_y,
                          SDL_MapRGB(screen->format, 0, 255, 0));
        SDL_UnlockSurface(screen);

        SDL_Flip(screen);
    }

    // cleanup
    SDL_JoystickClose(joystick);
    IMG_Quit();
    SDL_Quit();
    dbglogger_stop();

    return 0;
}
