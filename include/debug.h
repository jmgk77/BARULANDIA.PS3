#pragma once

#include "barulandia.h"

#define DBG_IP "192.168.0.11"
#define DBG_PORT "18194"

#ifdef DEBUG
#ifndef PS3
#define dbglogger_init_str(x)
#define dbglogger_stop(x)
#define dbglogger_printf printf
#define dbglogger_printf printf
#define dbglogger_screenshot_tmp(x)
#endif
#else
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#define dbglogger_init_str(x)
#define dbglogger_stop(x)
#define dbglogger_printf(...)
#define dbglogger_printf(...)
#define dbglogger_screenshot_tmp(x)
#endif

void debug_joystick(SDL_Joystick *joystick);
void debug_video();
void ret2psload();
void debug_format(int tf);
void debug_texture(SDL_Texture *texture);
void debug_font(TTF_Font *font);
void debug_window(SDL_Window *window);
void debug_renderer(SDL_Renderer *renderer);
void debug_audio();
void debug_audio_spec(SDL_AudioSpec *a);
