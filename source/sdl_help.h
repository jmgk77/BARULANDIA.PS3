// barulandia for ps3 (c) jmgk 2020

#pragma once

//#define LOGO_PNG 1

#include <SDL/SDL.h>
#ifdef LOGO_PNG
#include <SDL/SDL_image.h>
#endif
#ifdef PS3
#include <dbglogger.h>
#include <io/pad.h>
#include <ppu-types.h>
#include <sysutil/msg.h>
#include <sysutil/sysutil.h>
#include <sys/process.h>
#define DATA_PATH "/dev_hdd0/game/SDL00BARU/"
#else
#define dbglogger_init_str(x)
#define dbglogger_stop(x)
#define dbglogger_log printf
#define dbglogger_printf printf
#undef SDL_FULLSCREEN
#define SDL_FULLSCREEN 0
#define DATA_PATH "../release/"
#endif

#define DBG_IP "192.168.0.11"
#define DBG_PORT "18194"

#define WIDTH 1280
#define HEIGHT 720

#define SDL_CONTROLLER_AXIS_LEFTX 0
#define SDL_CONTROLLER_AXIS_LEFTY 1
#define SDL_CONTROLLER_AXIS_RIGHTX 2
#define SDL_CONTROLLER_AXIS_RIGHTY 3
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

void debug_joystick(SDL_Joystick *joystick);
void debug_video();

void fade_in_out(SDL_Surface *screen, SDL_Surface *image, Uint32 color,
                 int delay, bool in_out);

void ret2psload();
