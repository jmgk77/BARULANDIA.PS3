#include "video.h"

SDL_Surface *load_surface(const char *filename) {
  SDL_Surface *tmp;
  dbglogger_printf("LOADED: %s\n", filename);
  tmp = IMG_Load(filename);
  if (tmp == NULL) {
    dbglogger_printf("IMG_Load: %s", SDL_GetError());
    return NULL;
  }

  // Convert to optimal display format
  SDL_Surface *surface = SDL_DisplayFormatAlpha(tmp);
  // Free the original bitmap
  SDL_FreeSurface(tmp);
  return surface;
}

void fade_in_out(SDL_Surface *screen, SDL_Surface *image, bool in_out) {
  // center image in display
  SDL_Rect r;
  r.x = (screen->w - image->w) / 2;
  r.y = (screen->h - image->h) / 2;
  r.w = image->w;
  r.h = image->h;

  // Create a blank surface that is the same size as our screen
  SDL_Surface *tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, screen->w, screen->h,
                                          32, rmask, gmask, bmask, amask);
  if (tmp == NULL) {
    dbglogger_printf("SDL_CreateRGBSurface: %s", SDL_GetError());
    return;
  }
  // Convert it to the format of the screen
  SDL_Surface *tmp_screen = SDL_DisplayFormat(tmp);
  // Free the created surface
  SDL_FreeSurface(tmp);

  int alpha_init, alpha_end, alpha_step;
  if (in_out) {
    // fade in
    alpha_init = 0;
    alpha_end = 64;
    alpha_step = 2;
  } else {
    // fade out
    alpha_init = 64;
    alpha_end = 0;
    alpha_step = -2;
  }
#ifdef PS3
  alpha_step *= 4;
#endif
  Uint32 color = SDL_MapRGBA(tmp_screen->format, 255, 255, 255, 255);
  SDL_FillRect(tmp_screen, 0, color);
  // get data
  int bpp = tmp_screen->format->BytesPerPixel;
  int pitch_padding = (tmp_screen->pitch - (bpp * tmp_screen->w));
  // loop
  for (int alpha = alpha_init; alpha != alpha_end; alpha += alpha_step) {
    // Draw the bitmap to the constructed virtual screen

    SDL_BlitSurface(image, NULL, tmp_screen, &r);
#ifdef PS3
    SDL_LockSurface(tmp_screen);

    Uint8 *pixels = (Uint8 *)tmp_screen->pixels;
// Big Endian will have an offset of 0, otherwise it's 3 (R, G and B)
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    pixels += 3;
#endif

    for (int row = 0; row < tmp_screen->h; ++row) {
      for (int col = 0; col < tmp_screen->w; ++col) {
        // apply only on non-white (check only r channel)
        if (*(pixels + 1) != 0xff) {
          *pixels = (Uint8)alpha * 3;
        }
        pixels += bpp;
      }
      pixels += pitch_padding;
    }
    SDL_UnlockSurface(tmp_screen);
#else
    // Set the alpha of the constructed screen
    SDL_SetAlpha(tmp_screen, SDL_SRCALPHA, alpha * 3);
    SDL_Delay(50);
#endif
    // Draw the constructed surface to the primary surface now
    SDL_FillRect(screen, 0, color);
    SDL_BlitSurface(tmp_screen, NULL, screen, 0);

    SDL_Flip(screen);
  }
  SDL_FreeSurface(tmp_screen);
}