#include "sdl_help.h"



void PutPixel32_nolock(SDL_Surface *surface, int x, int y, Uint32 color) {
  Uint8 *pixel = (Uint8 *)surface->pixels;
  pixel += (y * surface->pitch) + (x * (surface->format->BytesPerPixel));
  *((Uint32 *)pixel) = color;
}



Uint32 GetPixel32_nolock(SDL_Surface *surface, int x, int y) {
  Uint8 *pixel = (Uint8 *)surface->pixels;
  pixel += (y * surface->pitch) + (x * (surface->format->BytesPerPixel));
  return *((Uint32 *)pixel);
}
