#include "floodfill.h"

void _floodfill(SDL_Surface *screen, int x, int y, Uint32 color) {
  queue<pair<int, int>> nodeQ;
  nodeQ.push({x, y});
  while (!nodeQ.empty()) {
    pair<int, int> currNode = nodeQ.front();
    nodeQ.pop();
    Uint32 c = GetPixel32_nolock(screen, currNode.first, currNode.second);
    if (c == color)
      continue;
    if (c == SDL_MapRGBA(screen->format, 0, 0, 0, 255))
      continue;
    PutPixel32_nolock(screen, currNode.first, currNode.second, color);
    if (currNode.first > 0)
      nodeQ.push({currNode.first - 1, currNode.second});
    if (currNode.first < screen->w)
      nodeQ.push({currNode.first + 1, currNode.second});
    if (currNode.second > 0)
      nodeQ.push({currNode.first, currNode.second - 1});
    if (currNode.second < (screen->h - 1))
      nodeQ.push({currNode.first, currNode.second + 1});
  }
}

void floodfill(SDL_Surface *screen, int x, int y, Uint32 color) {
  SDL_LockSurface(screen);
  _floodfill(screen, x, y, color);
  SDL_UnlockSurface(screen);
}
