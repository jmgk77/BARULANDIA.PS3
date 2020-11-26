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
  SDL_Rect rect;
  rect.x = (screen->w - image->w) / 2;
  rect.y = (screen->h - image->h) / 2;
  rect.w = image->w;
  rect.h = image->h;

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

  SDL_FillRect(tmp_screen, NULL, SDL_MapRGB(tmp_screen->format, 255, 255, 255));
  SDL_BlitSurface(image, NULL, tmp_screen, &rect);

#define PARTS_EFFECT 12
#define PARTS_STEPS 4
  SDL_Surface *parts[PARTS_EFFECT];
  int parts_x[PARTS_EFFECT];

  rect.x = 0;
  rect.w = tmp_screen->w;
  rect.h = (tmp_screen->h / PARTS_EFFECT);

  // divide image in PARTS_EFFECT parts
  for (int i = 0; i < PARTS_EFFECT; i++) {
    tmp =
        SDL_CreateRGBSurface(SDL_SWSURFACE, screen->w, screen->h / PARTS_EFFECT,
                             32, rmask, gmask, bmask, amask);
    if (tmp == NULL) {
      dbglogger_printf("SDL_CreateRGBSurface: %s", SDL_GetError());
      return;
    }
    // Convert it to the format of the screen
    parts[i] = SDL_DisplayFormat(tmp);
    // Free the created surface
    SDL_FreeSurface(tmp);
    rect.y = (tmp_screen->h / PARTS_EFFECT) * i;
    SDL_BlitSurface(tmp_screen, &rect, parts[i], NULL);
    parts_x[i] = 0;
    if (in_out) {
      parts_x[i] = (i % 2) ? -WIDTH : WIDTH;
    }
  }
  SDL_FreeSurface(tmp_screen);

  for (int x = 0; x < WIDTH + 1; x += PARTS_STEPS) {
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 255, 255, 255));
    for (int i = 0; i < PARTS_EFFECT; i++) {

      rect.x = parts_x[i];
      rect.y = (tmp_screen->h / PARTS_EFFECT) * i;
      parts_x[i] += (i % 2) ? PARTS_STEPS : -PARTS_STEPS;
      SDL_BlitSurface(parts[i], NULL, screen, &rect);
    }
    SDL_Flip(screen);
  }

  for (int i = 0; i < PARTS_EFFECT; i++) {
    SDL_FreeSurface(parts[i]);
  }
  SDL_Delay(50);
}