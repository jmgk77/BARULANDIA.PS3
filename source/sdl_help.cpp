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

// http://www.sdltutorials.com/sdl-scale-surface
SDL_Surface *ScaleSurface(SDL_Surface *Surface, Uint16 Width, Uint16 Height) {
  if (!Surface || !Width || !Height)
    return 0;

  SDL_Surface *_ret = SDL_CreateRGBSurface(
      Surface->flags, Width, Height, Surface->format->BitsPerPixel,
      Surface->format->Rmask, Surface->format->Gmask, Surface->format->Bmask,
      Surface->format->Amask);
  SDL_LockSurface(_ret);

  double _stretch_factor_x =
             (static_cast<double>(Width) / static_cast<double>(Surface->w)),
         _stretch_factor_y =
             (static_cast<double>(Height) / static_cast<double>(Surface->h));

  for (Sint32 y = 0; y < Surface->h; y++)
    for (Sint32 x = 0; x < Surface->w; x++)
      for (Sint32 o_y = 0; o_y < _stretch_factor_y; ++o_y)
        for (Sint32 o_x = 0; o_x < _stretch_factor_x; ++o_x)
          PutPixel32_nolock(_ret,
                            static_cast<Sint32>(_stretch_factor_x * x) + o_x,
                            static_cast<Sint32>(_stretch_factor_y * y) + o_y,
                            GetPixel32_nolock(Surface, x, y));
  SDL_UnlockSurface(_ret);

  return _ret;
}