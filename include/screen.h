#pragma once

#include "barulandia.h"

#define BUTTONS_XY 64

// spritesheet positions (*64)
#define CROSS_X 0
#define CROSS_Y 0
#define TRIANGLE_X 1
#define TRIANGLE_Y 0
#define SQUARE_X 2
#define SQUARE_Y 0
#define CIRCLE_X 3
#define CIRCLE_Y 0
#define START_X 0
#define START_Y 1
#define SELECT_X 1
#define SELECT_Y 1
#define L_X 2
#define L_Y 1
#define L2_X 0
#define L2_Y 2
#define R2_X 1
#define R2_Y 2
#define L1_X 2
#define L1_Y 2
#define R1_X 3
#define R1_Y 2
#define DPAD_X 0
#define DPAD_Y 3
#define L3_X 1
#define L3_Y 3
#define R3_X 2
#define R3_Y 3
#define R_X 3
#define R_Y 3

SDL_Surface *create_exit_screen(TTF_Font *font);
SDL_Surface *create_help_screen(TTF_Font *font);
