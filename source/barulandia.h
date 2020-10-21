#pragma once

#define USE_PNG 1

#include <SDL/SDL.h>

#ifdef USE_PNG
#include <SDL/SDL_image.h>
#endif

#ifdef PS3
#include <dbglogger.h>
#include <io/pad.h>
#include <ppu-types.h>
#include <sys/process.h>
#include <sysutil/msg.h>
#include <sysutil/sysutil.h>
#define DATA_PATH "/dev_hdd0/game/SDL00BARU/"

#else
#undef SDL_FULLSCREEN
#define SDL_FULLSCREEN 0
#define DATA_PATH "../release/"
#endif

#define WIDTH 1280
#define HEIGHT 720

#include "debug.h"
#include "sdl_help.h"
#include "video.h"
