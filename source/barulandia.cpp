// barulandia for ps3 (c) jmgk 2020

#include "sdl_help.h"

#define DBG_IP "192.168.0.11"
#define DBG_PORT "18194"

const int width = 1280, height = 720;

int main(int argc, char **argv) {
    LOG_INIT("tcp:" DBG_IP ":" DBG_PORT);
    LOG("barulandia for ps3 (c) jmgk 2020");

    // init sdl
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
        LOG("%s", SDL_GetError());
        return -1;
    }

    // init sdl_image
    int flags = IMG_INIT_JPG | IMG_INIT_PNG;
    int initted = IMG_Init(flags);
    if ((initted & flags) != flags) {
        LOG("IMG_Init: Failed to init required jpg and png support!\n");
        LOG("IMG_Init: %s\n", IMG_GetError());
        return -1;
    }

    // init joystick
    SDL_Joystick *joystick = SDL_JoystickOpen(0);

    // init screen
    SDL_Surface *screen = SDL_SetVideoMode(
                              width, height, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
    if (!screen) {
        LOG("%s", SDL_GetError());
        return -1;
    }

    // load logo
    SDL_Surface *tmp;
    if ((tmp = IMG_Load("/dev_hdd0/game/SDL00BARU/LOGO.PNG")) == 0) {
        LOG("Error could not load LOGO");
        return -1;
    }
    // Convert the logo to optimal display format
    SDL_Surface *logo = SDL_DisplayFormatAlpha(tmp);
    // Free the temporary surface
    SDL_FreeSurface(tmp);

    // sisplay logo (center)
    SDL_Rect r;
    r.x = (screen->w - logo->w) / 2;
    r.y = (screen->h - logo->h) / 2;
    r.w = logo->w;
    r.h = logo->h;
    SDL_FillRect(screen, 0, SDL_MapRGBA(screen->format, 255, 255, 255, 255));
    SDL_BlitSurface(logo, 0, screen, &r);
    SDL_Flip(screen);
    SDL_Delay(3 * 1000);

    // blank screen
    SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 255, 0, 0));

    // init cursors
    int cursor_x = width / 2;
    int cursor_y = height / 2;
    int cursor2_x = width / 2;
    int cursor2_y = height / 2;

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

            debug_joystick(joystick);

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
        cursor_x = (cursor_x < 0) ? width : (cursor_x > width - 1) ? 0 : cursor_x;
        cursor_y =
            (cursor_y < 0) ? height - 1 : (cursor_y > height - 1) ? 0 : cursor_y;
        cursor2_x =
            (cursor2_x < 0) ? width : (cursor2_x > width - 1) ? 0 : cursor2_x;
        cursor_y =
            (cursor2_y < 0) ? height - 1 : (cursor2_y > height - 1) ? 0 : cursor2_y;

        // draw cursors
        SDL_LockSurface(screen);
        PutPixel32_nolock(screen, cursor_x, cursor_y,
                          SDL_MapRGB(screen->format, 0, 0, 255));
        PutPixel32_nolock(screen, cursor2_x, cursor2_y,
                          SDL_MapRGB(screen->format, 0, 255, 0));
        SDL_UnlockSurface(screen);

        SDL_Flip(screen);
    }

    //cleanup
    SDL_JoystickClose(joystick);
    IMG_Quit();
    SDL_Quit();
    LOG_END();

    return 0;
}
