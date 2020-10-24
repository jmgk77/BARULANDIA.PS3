#include "floodfill.h"

#ifdef FLOOD_SIMPLE
// Flood-fill (node, target-color, replacement-color):
//  1. If target-color is equal to replacement-color, return.
//  2. ElseIf the color of node is not equal to target-color, return.
//  3. Else Set the color of node to replacement-color.
//  4. Perform Flood-fill (one step to the south of node, target-color,
//  replacement-color).
//     Perform Flood-fill (one step to the north of node, target-color,
//     replacement-color). Perform Flood-fill (one step to the west of node,
//     target-color, replacement-color). Perform Flood-fill (one step to the
//     east of node, target-color, replacement-color).
//  5. Return.
//
void _floodfill(SDL_Surface *screen, int x, int y, Uint32 color) {
  Uint32 c;

  c = GetPixel32_nolock(screen, x, y);
  if (c == color)
    return;
  if (c == SDL_MapRGBA(screen->format, 0, 0, 0, 255))
    return;
  PutPixel32_nolock(screen, x, y, color);
  //
  if (y > 0)
    _floodfill(screen, x, y - 1, color);
  if (y < (screen->h - 1))
    _floodfill(screen, x, y + 1, color);
  _floodfill(screen, x - 1, y, color);
  _floodfill(screen, x + 1, y, color);
}

#else
// stack friendly and fast floodfill algorithm, using recursive function calls
void _floodfill(SDL_Surface *screen, int x, int y, Uint32 color) {

  Uint32 stop_color = SDL_MapRGBA(screen->format, 0, 0, 0, 255);

  if (GetPixel32_nolock(screen, x, y) == color)
    return;
  if (GetPixel32_nolock(screen, x, y) == stop_color)
    return;

  int x1;

  // draw current scanline from start position to the right
  x1 = x;
  while (GetPixel32_nolock(screen, x1, y) != stop_color) {
    PutPixel32_nolock(screen, x1, y, color);
    x1++;
  }

  // draw current scanline from start position to the left
  x1 = x - 1;
  while (GetPixel32_nolock(screen, x1, y) != stop_color) {
    PutPixel32_nolock(screen, x1, y, color);
    x1--;
  }

  // test for new scanlines above
  x1 = x;
  while (GetPixel32_nolock(screen, x1, y) == color) {
    if ((y > 0) && (GetPixel32_nolock(screen, x1, y - 1) != stop_color)) {
      _floodfill(screen, x1, y - 1, color);
    }
    x1++;
  }
  x1 = x - 1;
  while (GetPixel32_nolock(screen, x1, y) == color) {
    if ((y > 0) && (GetPixel32_nolock(screen, x1, y - 1) != stop_color)) {
      _floodfill(screen, x1, y - 1, color);
    }
    x1--;
  }

  // test for new scanlines below
  x1 = x;
  while (GetPixel32_nolock(screen, x1, y) == color) {
    if ((y < (screen->h - 1)) &&
        (GetPixel32_nolock(screen, x1, y + 1) != stop_color)) {
      _floodfill(screen, x1, y + 1, color);
    }
    x1++;
  }
  x1 = x - 1;
  while (GetPixel32_nolock(screen, x1, y) == color) {
    if ((y < (screen->h - 1)) &&
        (GetPixel32_nolock(screen, x1, y + 1) != stop_color)) {
      _floodfill(screen, x1, y + 1, color);
    }
    x1--;
  }
}
#endif

void floodfill(SDL_Surface *screen, int x, int y, Uint32 color) {
  SDL_LockSurface(screen);
  _floodfill(screen, x, y, color);
  SDL_UnlockSurface(screen);
}
