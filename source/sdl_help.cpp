//barulandia for ps3 (c) jmgk 2020

#include "sdl_help.h"

void PutPixel32_nolock(SDL_Surface * surface, int x, int y, Uint32 color) {
    Uint8 * pixel = (Uint8*)surface->pixels;
    pixel += (y * surface->pitch) + (x * sizeof(Uint32));
    *((Uint32*)pixel) = color;
}


Uint32 GetPixel32_nolock(SDL_Surface * surface, int x, int y) {
    Uint8 * pixel = (Uint8*)surface->pixels;
    pixel += (y * surface->pitch) + (x * sizeof(Uint32));
    return *((Uint32*)pixel);
}

