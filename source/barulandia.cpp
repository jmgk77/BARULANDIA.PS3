//barulandia for ps3 (c) jmgk 2020

#include <SDL/SDL.h>
#include "sdl_help.h"

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

    //blank screen
    SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 255, 0, 0));

    int cursor_x = width / 2;
    int cursor_y = height / 2;
    int cursor_xvel = 0, cursor_yvel = 0;

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
                    switch ( e.key.keysym.sym ) {
                        case SDLK_ESCAPE:
                            active = false;
                            break;
                        case SDLK_LEFT:
                            cursor_xvel = -1;
                            break;
                        case SDLK_RIGHT:
                            cursor_xvel =  1;
                            break;
                        case SDLK_UP:
                            cursor_yvel = -1;
                            break;
                        case SDLK_DOWN:
                            cursor_yvel =  1;
                            break;
                        default:
                            break;
                    }
                    break;

                case SDL_KEYUP:
                    switch ( e.key.keysym.sym ) {
                        case SDLK_LEFT:
                            if ( cursor_xvel < 0 )
                                cursor_xvel = 0;
                            break;
                        case SDLK_RIGHT:
                            if ( cursor_xvel > 0 )
                                cursor_xvel = 0;
                            break;
                        case SDLK_UP:
                            if ( cursor_yvel < 0 )
                                cursor_yvel = 0;
                            break;
                        case SDLK_DOWN:
                            if ( cursor_yvel > 0 )
                                cursor_yvel = 0;
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }

        }

        //update cursor (keyboard)
        cursor_x += cursor_xvel;
        cursor_y += cursor_yvel;

        if (joystick) {
            SDL_JoystickUpdate();

            LOG("LX %d LY %d RX %d RY %d",
                SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTX),
                SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTY),
                SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_RIGHTX),
                SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_RIGHTY));
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

            Sint16 dx = SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTX);
            cursor_x += (dx > AXIS_DEADZONE ) ? 1 : (dx < -AXIS_DEADZONE) ? -1 : 0;
            Sint16 dy = SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTY);
            cursor_y += (dy > AXIS_DEADZONE ) ? 1 : (dy < -AXIS_DEADZONE) ? -1 : 0;

        }

        //screen boundaries check
        cursor_x = (cursor_x < 0) ? width : (cursor_x > width - 1) ? 0 : cursor_x;
        cursor_y = (cursor_y < 0) ? height-1 : (cursor_y > height - 1) ? 0 : cursor_y;

        //draw cursor
        SDL_LockSurface(screen);
        PutPixel32_nolock(screen, cursor_x, cursor_y, SDL_MapRGB(screen->format, 0, 0, 255));
        SDL_UnlockSurface(screen);

        SDL_Flip(screen);

    }

    SDL_JoystickClose(joystick);
    SDL_Quit();
    LOG_END();

    return 0;
}

