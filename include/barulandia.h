#pragma once

#define DEBUG 1

typedef void SDL_Renderer;
typedef int SDL_AudioDeviceID;
typedef void SDL_Texture;

// #define SKIP_INTRO
//#define SKIP_STARTSCREEN
//#define SKIP_GALLERY
#define SKIP_MAIN

#define MAX_STRING 2048

#define DRAW_W 576
#define DRAW_H 720

#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

#if defined SKIP_INTRO || defined SKIP_STARTSCREEN || defined SKIP_MAIN
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#define MAX_DRAW 10

#define JOYBUTTONS 16

#define GRAPH_EXT ".PNG"

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_syswm.h>
#include <SDL/SDL_ttf.h>

#ifdef PS3
#include <dbglogger.h>
#include <io/pad.h>
#include <ppu-types.h>
#include <sys/process.h>
#include <sysutil/msg.h>
#include <sysutil/sysutil.h>
#endif

#ifdef PS3
#define DATA_PATH "/dev_hdd0/game/SDL12BARU/USRDIR/"
#else
#define DATA_PATH "../release/USRDIR/"
#endif

#define WIDTH 1280
#define HEIGHT 720

#include "debug.h"
#include "floodfill.h"
#include "save.h"
#include "screen.h"
#include "sdl_help.h"
#include "sound.h"
#include "video.h"

#include <bitset>
#include <cmath>
#include <dirent.h>
#include <list>
#include <queue>
#include <time.h>

using namespace std;

#define INTRO_INIT 1
#define INTRO_MAIN 2
#define INTRO_END 3
#define START_INIT 4
#define START_MAIN 5
#define START_END 6
#define GALLERY_INIT 7
#define GALLERY_MAIN 8
#define GALLERY_END 9
#define MAIN_INIT 10
#define MAIN_MAIN 11
#define MAIN_END 12
#define FINISHED 13
