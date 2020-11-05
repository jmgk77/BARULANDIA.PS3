#include "save.h"

int do_save_png(void *screen) {
  char buf[128];
  time_t rawtime;
  struct tm *t;

  time(&rawtime);
  t = gmtime(&rawtime);

  snprintf(buf, 128, "%sSAVEDATA/DRAW_%d_%02d_%02d_%02d_%02d_%02d%s", DATA_PATH,
           t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min,
           t->tm_sec, GRAPH_EXT);
  dbglogger_printf("THREAD SAVING: %s\n", buf);

  // undocumented SDL_Image function...
  IMG_SavePNG((SDL_Surface *)screen, buf);
  return 0;
}

void save_png(SDL_Surface *screen) {

  SDL_Thread *thread = SDL_CreateThread(do_save_png, "SAVE_PNG", screen);

  if (thread == NULL) {
    dbglogger_printf("SDL_CreateThread failed: %s\n", SDL_GetError());
  }
}
