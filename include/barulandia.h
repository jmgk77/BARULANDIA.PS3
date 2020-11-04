#pragma once

//#define DEBUG 1

//#define SKIP_INTRO 1
//#define SKIP_STARTSCREEN 1
//#define SKIP_MAIN

#if defined SKIP_INTRO || defined SKIP_STARTSCREEN || defined SKIP_MAIN
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

#define MAX_DRAW 10

#define JOYBUTTONS 16

#define GRAPH_EXT ".PNG"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_ttf.h>

#ifdef PS3
#include <dbglogger.h>
#include <io/pad.h>
#include <ppu-types.h>
#include <sys/process.h>
#include <sysutil/msg.h>
#include <sysutil/sysutil.h>
#endif

#ifdef PS3
#define DATA_PATH "/dev_hdd0/game/SDL20BARU/USRDIR/"
#else
#define DATA_PATH "../release/USRDIR/"
#endif

#define WIDTH 1280
#define HEIGHT 720

#include "debug.h"
#include "floodfill.h"
#include "screen.h"
#include "sdl_help.h"
#include "video.h"

#include <queue>

using namespace std;

#define INTRO_INIT 1
#define INTRO_MAIN 2
#define INTRO_END 3
#define START_INIT 4
#define START_MAIN 5
#define START_END 6
#define MAIN_INIT 7
#define MAIN_MAIN 8
#define MAIN_END 9
#define FINISHED 10

#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
