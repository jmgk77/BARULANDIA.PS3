#include "screen.h"

// https://discourse.libsdl.org/t/rectangle-with-round-corners-in-sdl/14097/5
// draws a rounded box with…
// corner radius of ‘r’
// width of ‘w’
// and height of ‘h’
// draws the box right and down of…
// x-offset xo
// y-offset yo

// returns 0 if 2*r is bigger than w or h
// and draws nothing
// returns 1 on success

int fill_rounded_box_b(SDL_Surface *dst, int xo, int yo, int w, int h, int r,
                       Uint32 color) {
  int yd = dst->pitch / dst->format->BytesPerPixel;
  Uint32 *pixels = NULL;
  int x, y, i, j;
  int rpsqrt2 = (int)(r / sqrt(2));

  w /= 2;
  h /= 2;

  xo += w;
  yo += h;

  w -= r;
  h -= r;

  if (w < 0 || h < 0)
    return 0;

  SDL_LockSurface(dst);
  pixels = (Uint32 *)(dst->pixels);

  int sy = (yo - h) * yd;
  int ey = (yo + h) * yd;
  int sx = (xo - w);
  int ex = (xo + w);
  for (i = sy; i <= ey; i += yd)
    for (j = sx - r; j <= ex + r; j++)
      pixels[i + j] = color;

  int d = -r;
  int x2m1 = -1;
  y = r;
  for (x = 0; x <= rpsqrt2; x++) {
    x2m1 += 2;
    d += x2m1;
    if (d >= 0) {
      y--;
      d -= (y * 2);
    }

    for (i = sx - x; i <= ex + x; i++)
      pixels[sy - y * yd + i] = color;
    for (i = sx - y; i <= ex + y; i++)
      pixels[sy - x * yd + i] = color;
    for (i = sx - y; i <= ex + y; i++)
      pixels[ey + x * yd + i] = color;
    for (i = sx - x; i <= ex + x; i++)
      pixels[ey + y * yd + i] = color;
  }
  SDL_UnlockSurface(dst);
  return 1;
}

#define MARGIN_X (WIDTH / 6)
#define MARGIN_Y (HEIGHT / 6)

SDL_Surface *create_exit_screen(TTF_Font *font) {
  SDL_Surface *field = load_surface(DATA_PATH "FUNDO" GRAPH_EXT);
  SDL_Surface *buttons = load_surface(DATA_PATH "BUTTONS" GRAPH_EXT);

  SDL_Color fg = {255, 0, 0, SDL_ALPHA_OPAQUE};

  SDL_Surface *t1 = TTF_RenderText_Blended(font, "VOCE DESEJA SAIR?", fg);
  SDL_Surface *t2 = TTF_RenderText_Blended(font, "SAIR", fg);
  SDL_Surface *t3 = TTF_RenderText_Blended(font, "VOLTAR", fg);

  fill_rounded_box_b(field, MARGIN_X, MARGIN_Y, field->w - (MARGIN_X * 2),
                     field->h - (MARGIN_Y * 2), 20,
                     SDL_MapRGBA(field->format, 16, 64, 16, SDL_ALPHA_OPAQUE));

  SDL_Rect dstrect;
  SDL_Rect srcrect;
  srcrect.w = BUTTONS_XY;
  srcrect.h = BUTTONS_XY;

  dstrect.x = (field->w - t1->w) / 2;
  dstrect.y = (field->h - t1->h) / 2;
  SDL_BlitSurface(t1, NULL, field, &dstrect);

  dstrect.x = (field->w / 8) * 2;
  dstrect.y = (field->h / 4) * 3;
  srcrect.x = CROSS_X * BUTTONS_XY;
  srcrect.y = CROSS_Y * BUTTONS_XY;
  SDL_BlitSurface(buttons, &srcrect, field, &dstrect);
  dstrect.x += BUTTONS_XY;
  SDL_BlitSurface(t2, NULL, field, &dstrect);

  dstrect.x = ((field->w / 8) * 5) - BUTTONS_XY;
  srcrect.x = CIRCLE_X * BUTTONS_XY;
  srcrect.y = CIRCLE_Y * BUTTONS_XY;
  SDL_BlitSurface(buttons, &srcrect, field, &dstrect);
  dstrect.x += BUTTONS_XY;
  SDL_BlitSurface(t3, NULL, field, &dstrect);

  SDL_FreeSurface(t3);
  SDL_FreeSurface(t2);
  SDL_FreeSurface(t1);
  SDL_FreeSurface(buttons);

  return field;
}

SDL_Surface *create_help_screen(TTF_Font *font) {

  SDL_Surface *field = load_surface(DATA_PATH "FUNDO" GRAPH_EXT);
  SDL_Surface *buttons = load_surface(DATA_PATH "BUTTONS" GRAPH_EXT);

  SDL_Color fg = {255, 0, 0, SDL_ALPHA_OPAQUE};

  SDL_Surface *t1 = TTF_RenderText_Blended(font, "AJUDA", fg);
  SDL_Surface *t2 = TTF_RenderText_Blended(font, "MOVER", fg);
  SDL_Surface *t3 = TTF_RenderText_Blended(font, "PINTAR", fg);
  SDL_Surface *t4 = TTF_RenderText_Blended(font, "ANTERIOR", fg);
  SDL_Surface *t5 = TTF_RenderText_Blended(font, "PROXIMO", fg);
  SDL_Surface *t7 = TTF_RenderText_Blended(font, "SAIR", fg);
  SDL_Surface *t9 = TTF_RenderText_Blended(font, "VOLTAR", fg);

  fill_rounded_box_b(field, MARGIN_X, MARGIN_Y, field->w - (MARGIN_X * 2),
                     field->h - (MARGIN_Y * 2), 20,
                     SDL_MapRGBA(field->format, 16, 64, 16, SDL_ALPHA_OPAQUE));

  SDL_Rect dstrect;
  SDL_Rect srcrect;
  srcrect.w = BUTTONS_XY;
  srcrect.h = BUTTONS_XY;

  dstrect.x = (field->w - t9->w - BUTTONS_XY) / 2;
  dstrect.y = (MARGIN_Y * 5) - (MARGIN_Y / 2);
  srcrect.x = CIRCLE_X * BUTTONS_XY;
  srcrect.y = CIRCLE_Y * BUTTONS_XY;
  SDL_BlitSurface(buttons, &srcrect, field, &dstrect);
  dstrect.x += BUTTONS_XY;
  SDL_BlitSurface(t9, NULL, field, &dstrect);
  dstrect.x = (field->w - t1->w) / 2;
  dstrect.y = MARGIN_Y;
  SDL_BlitSurface(t1, NULL, field, &dstrect);

  //
  srcrect.x = L_X * BUTTONS_XY;
  srcrect.y = L_Y * BUTTONS_XY;
  dstrect.x = field->w / 4;
  dstrect.y += MARGIN_Y / 2;
  SDL_BlitSurface(buttons, &srcrect, field, &dstrect);
  dstrect.x += (BUTTONS_XY * 8);
  SDL_BlitSurface(t2, NULL, field, &dstrect);

  srcrect.x = CROSS_X * BUTTONS_XY;
  srcrect.y = CROSS_Y * BUTTONS_XY;
  dstrect.x = field->w / 4;
  dstrect.y += MARGIN_Y / 2;
  SDL_BlitSurface(buttons, &srcrect, field, &dstrect);
  dstrect.x += (BUTTONS_XY * 8);
  SDL_BlitSurface(t3, NULL, field, &dstrect);

  srcrect.x = SELECT_X * BUTTONS_XY;
  srcrect.y = SELECT_Y * BUTTONS_XY;
  dstrect.x = field->w / 4;
  dstrect.y += MARGIN_Y / 2;
  SDL_BlitSurface(buttons, &srcrect, field, &dstrect);
  dstrect.x += (BUTTONS_XY * 8);
  SDL_BlitSurface(t1, NULL, field, &dstrect);

  srcrect.x = START_X * BUTTONS_XY;
  srcrect.y = START_Y * BUTTONS_XY;
  dstrect.x = field->w / 4;
  dstrect.y += MARGIN_Y / 2;
  SDL_BlitSurface(buttons, &srcrect, field, &dstrect);
  dstrect.x += (BUTTONS_XY * 8);
  SDL_BlitSurface(t7, NULL, field, &dstrect);

  srcrect.x = L1_X * BUTTONS_XY;
  srcrect.y = L1_Y * BUTTONS_XY;
  dstrect.x = field->w / 4;
  dstrect.y += MARGIN_Y / 2;
  SDL_BlitSurface(buttons, &srcrect, field, &dstrect);
  dstrect.x += (BUTTONS_XY * 8);
  SDL_BlitSurface(t4, NULL, field, &dstrect);

  srcrect.x = R1_X * BUTTONS_XY;
  srcrect.y = R1_Y * BUTTONS_XY;
  dstrect.x = field->w / 4;
  dstrect.y += MARGIN_Y / 2;
  SDL_BlitSurface(buttons, &srcrect, field, &dstrect);
  dstrect.x += (BUTTONS_XY * 8);
  SDL_BlitSurface(t5, NULL, field, &dstrect);

  // free
  SDL_FreeSurface(t9);
  SDL_FreeSurface(t7);
  SDL_FreeSurface(t5);
  SDL_FreeSurface(t4);
  SDL_FreeSurface(t3);
  SDL_FreeSurface(t2);
  SDL_FreeSurface(t1);
  SDL_FreeSurface(buttons);

  return field;
}
