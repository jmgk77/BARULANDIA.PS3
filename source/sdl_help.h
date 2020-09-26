//barulandia for ps3 (c) jmgk 2020

#pragma once

#include <SDL/SDL.h>

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

#define AXIS_DEADZONE 8192

void PutPixel32_nolock(SDL_Surface * surface, int x, int y, Uint32 color) ;
Uint32 GetPixel32_nolock(SDL_Surface * surface, int x, int y) ;
