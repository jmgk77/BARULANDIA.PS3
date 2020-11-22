#pragma once

#include "barulandia.h"

#define SDL_CONTROLLER_AXIS_LEFTX 0
#define SDL_CONTROLLER_AXIS_LEFTY 1
#define SDL_CONTROLLER_AXIS_RIGHTX 2
#define SDL_CONTROLLER_AXIS_RIGHTY 3
#ifdef PS3
#define SDL_CONTROLLER_BUTTON_LEFT 0
#define SDL_CONTROLLER_BUTTON_DOWN 1
#define SDL_CONTROLLER_BUTTON_RIGHT 2
#define SDL_CONTROLLER_BUTTON_UP 3
#define SDL_CONTROLLER_BUTTON_START 4
#define SDL_CONTROLLER_BUTTON_R3 5
#define SDL_CONTROLLER_BUTTON_L3 6
#define SDL_CONTROLLER_BUTTON_SELECT 7
#define SDL_CONTROLLER_BUTTON_SQUARE 8
#define SDL_CONTROLLER_BUTTON_CROSS 9
#define SDL_CONTROLLER_BUTTON_CIRCLE 10
#define SDL_CONTROLLER_BUTTON_TRIANGLE 11
#define SDL_CONTROLLER_BUTTON_R1 12
#define SDL_CONTROLLER_BUTTON_L1 13
#define SDL_CONTROLLER_BUTTON_R2 14
#define SDL_CONTROLLER_BUTTON_L2 15
#else
#define SDL_CONTROLLER_BUTTON_LEFT 12
#define SDL_CONTROLLER_BUTTON_DOWN 13
#define SDL_CONTROLLER_BUTTON_RIGHT 14
#define SDL_CONTROLLER_BUTTON_UP 15
#define SDL_CONTROLLER_BUTTON_START 9
#define SDL_CONTROLLER_BUTTON_R3 10
#define SDL_CONTROLLER_BUTTON_L3 11
#define SDL_CONTROLLER_BUTTON_SELECT 8
#define SDL_CONTROLLER_BUTTON_SQUARE 3
#define SDL_CONTROLLER_BUTTON_CROSS 2
#define SDL_CONTROLLER_BUTTON_CIRCLE 1
#define SDL_CONTROLLER_BUTTON_TRIANGLE 0
#define SDL_CONTROLLER_BUTTON_R1 5
#define SDL_CONTROLLER_BUTTON_L1 4
#define SDL_CONTROLLER_BUTTON_R2 7
#define SDL_CONTROLLER_BUTTON_L2 6
#endif

#define AXIS_DEADZONE 8192

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define rmask 0xff000000
#define gmask 0x00ff0000
#define bmask 0x0000ff00
#define amask 0x000000ff
#else
#define rmask 0x000000ff
#define gmask 0x0000ff00
#define bmask 0x00ff0000
#define amask 0xff000000
#endif

void PutPixel32_nolock(SDL_Surface *surface, int x, int y, Uint32 color);
Uint32 GetPixel32_nolock(SDL_Surface *surface, int x, int y);
SDL_Surface *ScaleSurface(SDL_Surface *Surface, Uint16 Width, Uint16 Height);
