#include "save.h"

int do_save_png(void *screen) {
  char buf[MAX_STRING];
  time_t rawtime;
  struct tm *t;

  time(&rawtime);
  t = gmtime(&rawtime);

  snprintf(buf, MAX_STRING, "%sSAVEDATA/DRAW_%d_%02d_%02d_%02d_%02d_%02d%s",
           DATA_PATH, t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour,
           t->tm_min, t->tm_sec, GRAPH_EXT);
  dbglogger_printf("THREAD SAVING: %s\n", buf);

  SDL_Rect content;
  content.x = (WIDTH - 576) / 2;
  content.y = 0;
  content.w = 576;
  content.h = 720;

  // avoid lots of casts...
  SDL_Surface *in = (SDL_Surface *)screen;
  SDL_Surface *out = SDL_CreateRGBSurface(
      0, content.w, content.h, in->format->BitsPerPixel, in->format->Rmask,
      in->format->Gmask, in->format->Bmask, in->format->Amask);
  SDL_BlitSurface(in, &content, out, NULL);

  // undocumented SDL_Image function...
  IMG_SavePNG(out, buf);
  SDL_FreeSurface(out);

  return 0;
}

void save_png(SDL_Renderer *renderer, SDL_Surface *screen) {
  SDL_Thread *thread = SDL_CreateThread(do_save_png, "SAVE_PNG", screen);

  effect_play(SOUND_CLICK);

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  if (thread == NULL) {
    dbglogger_printf("SDL_CreateThread failed: %s\n", SDL_GetError());
  }
}
