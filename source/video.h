#pragma once

#include "barulandia.h"

SDL_Surface *load_surface(const char *filename);
SDL_Texture *load_texture(SDL_Renderer *renderer, const char *filename);
void fade_in_out(SDL_Renderer *renderer, SDL_Texture *image, bool resize,
                 bool in_out);