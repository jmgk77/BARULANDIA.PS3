#pragma once

#include "barulandia.h"

SDL_Surface *load_surface(const char *filename);
void fade_in_out(SDL_Surface *screen, SDL_Surface *image, bool in_out);
