#include "video.h"

SDL_Texture *load_resource(SDL_Renderer *renderer, const char *filename) {
  SDL_Surface *tmp;
  dbglogger_log("RESOURCE: %s", filename);
#ifdef USE_PNG
  tmp = IMG_Load(filename);
  if (tmp == NULL) {
    dbglogger_log("IMG_Load: %s", SDL_GetError());
    return NULL;
  }
#else
  tmp = SDL_LoadBMP(filename);
  if (tmp == NULL) {
    dbglogger_log("SDL_LoadBMP: %s", SDL_GetError());
    return NULL;
  }
#endif

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

  return texture;
}

void fade_in_out(SDL_Renderer *renderer, SDL_Texture *image, bool in_out) {

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

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  // loop
  for (int alpha = alpha_init; alpha != alpha_end; alpha += alpha_step) {

    // Draw the constructed surface to the primary surface now
    SDL_RenderClear(renderer);
    SDL_SetTextureAlphaMod(image, alpha);
    SDL_RenderCopy(renderer, image, NULL, NULL);
    SDL_RenderPresent(renderer);
    SDL_Delay(100);
  }
}
