//barulandia for ps3 (c) jmgk 2020

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#ifdef SDLPS3
//ps3
#include <io/pad.h>
#include <sysutil/msg.h>
#include <sysutil/sysutil.h>
#include <dbglogger.h>
#define LOG_INIT    dbglogger_init_str
#define LOG_END     dbglogger_stop
#define LOG         dbglogger_log
#else
//linux
#define LOG_INIT    printf
#define LOG_END     _noop
#define LOG         printf
void _noop() {};
#endif

#define DBG_IP      "192.168.0.11"
#define DBG_PORT    "18194"

#define SDL_CONTROLLER_AXIS_LEFTX       0
#define SDL_CONTROLLER_AXIS_LEFTY       1
#define SDL_CONTROLLER_AXIS_RIGHTX      2
#define SDL_CONTROLLER_AXIS_RIGHTY      3
#define SDL_CONTROLLER_BUTTON_LEFT      0
#define SDL_CONTROLLER_BUTTON_DOWN      1
#define SDL_CONTROLLER_BUTTON_RIGHT     2
#define SDL_CONTROLLER_BUTTON_UP        3
#define SDL_CONTROLLER_BUTTON_START     4
#define SDL_CONTROLLER_BUTTON_R3        5
#define SDL_CONTROLLER_BUTTON_L3        6
#define SDL_CONTROLLER_BUTTON_SELECT    7
#define SDL_CONTROLLER_BUTTON_SQUARE    8
#define SDL_CONTROLLER_BUTTON_CROSS     9
#define SDL_CONTROLLER_BUTTON_CIRCLE    10
#define SDL_CONTROLLER_BUTTON_TRIANGLE  11
#define SDL_CONTROLLER_BUTTON_R1        12
#define SDL_CONTROLLER_BUTTON_L1        13
#define SDL_CONTROLLER_BUTTON_R2        14
#define SDL_CONTROLLER_BUTTON_L2        15


const int width = 1280, height = 720;


int main(int argc, char **argv) {
    LOG_INIT("tcp:" DBG_IP ":" DBG_PORT);

    LOG("barulandia for ps3 (c) jmgk 2020");

    if (SDL_Init(SDL_INIT_VIDEO  | SDL_INIT_JOYSTICK) < 0) {
        LOG("%s", SDL_GetError());
        return -1;
    }

    SDL_Joystick *joystick = SDL_JoystickOpen(0);

#ifdef SDLPS3
    SDL_Surface *screen = SDL_SetVideoMode(width, height, 32 , SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
#else
    SDL_Surface *screen = SDL_SetVideoMode(width, height, 32 , SDL_HWSURFACE | SDL_DOUBLEBUF );
#endif
    if (!screen) {
        LOG("%s", SDL_GetError());
        return -1;
    }

    bool active = true;

    while (active) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {

            switch (e.type) {
                case SDL_QUIT:
                    active = false;
                    break;
                case SDL_KEYDOWN:
                    LOG( "SDL_KEYDOWN: %s\n", SDL_GetKeyName(e.key.keysym.sym) );
                    if (e.key.keysym.sym == SDLK_ESCAPE) {
                        active = false;
                    }
                    break;
            }
        }

        if (joystick) {
            SDL_JoystickUpdate();

            LOG("LX %d LY %d RX %d RY %d",
                SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTX) >> 4,
                SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTY) >> 4,
                SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_RIGHTX) >> 4,
                SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_RIGHTY) >> 4);
#define LOG_BTN(X) if (SDL_JoystickGetButton(joystick, X) == SDL_PRESSED) LOG(#X)
            LOG_BTN(SDL_CONTROLLER_BUTTON_LEFT);
            LOG_BTN(SDL_CONTROLLER_BUTTON_DOWN);
            LOG_BTN(SDL_CONTROLLER_BUTTON_RIGHT);
            LOG_BTN(SDL_CONTROLLER_BUTTON_UP);
            LOG_BTN(SDL_CONTROLLER_BUTTON_START);
            LOG_BTN(SDL_CONTROLLER_BUTTON_R3);
            LOG_BTN(SDL_CONTROLLER_BUTTON_L3);
            LOG_BTN(SDL_CONTROLLER_BUTTON_SELECT);
            LOG_BTN(SDL_CONTROLLER_BUTTON_SQUARE);
            LOG_BTN(SDL_CONTROLLER_BUTTON_CROSS);
            LOG_BTN(SDL_CONTROLLER_BUTTON_CIRCLE );
            LOG_BTN(SDL_CONTROLLER_BUTTON_TRIANGLE );
            LOG_BTN(SDL_CONTROLLER_BUTTON_R1 );
            LOG_BTN(SDL_CONTROLLER_BUTTON_L1 );
            LOG_BTN(SDL_CONTROLLER_BUTTON_R2 );
            LOG_BTN(SDL_CONTROLLER_BUTTON_L2 );

            if (SDL_JoystickGetButton(joystick, SDL_CONTROLLER_BUTTON_START) == SDL_PRESSED) {
                active = false;
            }

        }

        SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 255, 0, 0));
        SDL_Flip(screen);

    }

    SDL_JoystickClose(joystick);
    SDL_Quit();
    LOG_END();

    return 0;
}

