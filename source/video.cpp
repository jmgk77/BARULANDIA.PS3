#include "video.h"

SDL_Surface *load_surface(const char *filename) {
  SDL_Surface *tmp;
  dbglogger_printf("LOADED: %s\n", filename);
  tmp = IMG_Load(filename);
  if (tmp == NULL) {
    dbglogger_printf("IMG_Load: %s", SDL_GetError());
    return NULL;
  }

  SDL_PixelFormat *format = SDL_AllocFormat(SDL_PIXELFORMAT_ARGB32);
  SDL_Surface *output = SDL_ConvertSurface(tmp, format, 0);
  SDL_FreeFormat(format);

  SDL_FreeSurface(tmp);

  return output;
}

SDL_Texture *load_texture(SDL_Renderer *renderer, const char *filename) {
  SDL_Surface *tmp = load_surface(filename);

  // Convert the img to texture
  SDL_Texture *texture;
  if ((texture = SDL_CreateTextureFromSurface(renderer, tmp)) == NULL) {
    dbglogger_printf("SDL_CreateTextureFromSurface Error: %s\n",
                     SDL_GetError());
    return NULL;
  }

  // Free the original bitmap
  SDL_FreeSurface(tmp);
  //
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

  debug_texture(texture);

  return texture;
}

int alpha = 3;

void set_alpha_rate(int a) { alpha = a; }

void fade_in_out(SDL_Renderer *renderer, SDL_Texture *image, bool resize,
                 bool in_out) {
  int rw, rh;
  int tw, th;
  SDL_Rect r;

  SDL_GetRendererOutputSize(renderer, &rw, &rh);
  SDL_QueryTexture(image, NULL, NULL, &tw, &th);

  if (resize) {

    r.x = (rw - tw) / 2;
    r.y = (rh - th) / 2;
    r.w = tw;
    r.h = th;
  }

  dbglogger_printf("RENDER (%d x %d)\tTEXTURE (%d x %d)\n", rw, rh, tw, th);

  int alpha_init, alpha_end, alpha_step;
  if (in_out) {
    // fade in
    alpha_init = 0;
    alpha_end = 255;
    alpha_step = (alpha);
  } else {
    // fade out
    alpha_init = 255;
    alpha_end = 0;
    alpha_step = -(alpha);
  }

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  // loop
  for (int alpha = alpha_init; alpha != alpha_end; alpha += alpha_step) {

    // Draw the constructed surface to the primary surface now
    SDL_RenderClear(renderer);
    SDL_SetTextureAlphaMod(image, alpha);
    SDL_RenderCopy(renderer, image, NULL, resize ? &r : NULL);
    SDL_RenderPresent(renderer);
  }
}
