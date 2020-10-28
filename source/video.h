#pragma once

#include "barulandia.h"

SDL_Texture *load_resource(SDL_Renderer *renderer, const char *filename);
void fade_in_out(SDL_Renderer *renderer, SDL_Texture *image, bool in_out);