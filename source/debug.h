#pragma once

#include "barulandia.h"

#define DBG_IP "192.168.0.11"
#define DBG_PORT "18194"

#ifndef PS3
#define dbglogger_init_str(x)
#define dbglogger_stop(x)
#define dbglogger_log printf
#define dbglogger_printf printf
#define dbglogger_screenshot_tmp(x)                                            \
  char file[128];                                                              \
  snprintf(file, sizeof(file), "%8d.BMP", rand());                             \
  SDL_SaveBMP(screen, file);
#endif

void debug_joystick(SDL_Joystick *joystick);
void debug_video();
void ret2psload();
