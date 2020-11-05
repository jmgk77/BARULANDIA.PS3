#include "save.h"

void save_png(SDL_Surface *screen) {
  char buf[128];
  time_t rawtime;
  struct tm *t;

  time(&rawtime);
  t = gmtime(&rawtime);

  snprintf(buf, 128, "%sSAVEDATA/DRAW_%d_%02d_%02d_%02d_%02d_%02d%s", DATA_PATH,
           t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min,
           t->tm_sec, GRAPH_EXT);
  dbglogger_printf("SAVING: %s\n", buf);

  // undocumented SDL_Image function...
  IMG_SavePNG(screen, buf);
}
