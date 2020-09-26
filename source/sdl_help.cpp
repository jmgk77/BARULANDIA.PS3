// barulandia for ps3 (c) jmgk 2020

#include "sdl_help.h"

void PutPixel32_nolock(SDL_Surface *surface, int x, int y, Uint32 color) {
    Uint8 *pixel = (Uint8 *)surface->pixels;
    pixel += (y * surface->pitch) + (x * sizeof(Uint32));
    *((Uint32 *)pixel) = color;
}

Uint32 GetPixel32_nolock(SDL_Surface *surface, int x, int y) {
    Uint8 *pixel = (Uint8 *)surface->pixels;
    pixel += (y * surface->pitch) + (x * sizeof(Uint32));
    return *((Uint32 *)pixel);
}

void debug_joystick(SDL_Joystick *joystick) {
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
    LOG_BTN(SDL_CONTROLLER_BUTTON_CIRCLE);
    LOG_BTN(SDL_CONTROLLER_BUTTON_TRIANGLE);
    LOG_BTN(SDL_CONTROLLER_BUTTON_R1);
    LOG_BTN(SDL_CONTROLLER_BUTTON_L1);
    LOG_BTN(SDL_CONTROLLER_BUTTON_R2);
    LOG_BTN(SDL_CONTROLLER_BUTTON_L2);
}