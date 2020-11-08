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
  SDL_Surface *exit_screen = load_surface(DATA_PATH "FUNDO" GRAPH_EXT);
  SDL_Surface *buttons = load_surface(DATA_PATH "BUTTONS" GRAPH_EXT);

  SDL_Color fg = {255, 0, 0, SDL_ALPHA_OPAQUE};

  SDL_Surface *t1 = TTF_RenderText_Blended(font, "VOCE DESEJA SAIR?", fg);
  SDL_Surface *t2 = TTF_RenderText_Blended(font, "SAIR", fg);
  SDL_Surface *t3 = TTF_RenderText_Blended(font, "VOLTAR", fg);

  fill_rounded_box_b(
      exit_screen, MARGIN_X, MARGIN_Y, exit_screen->w - (MARGIN_X * 2),
      exit_screen->h - (MARGIN_Y * 2), 20,
      SDL_MapRGBA(exit_screen->format, 16, 64, 16, SDL_ALPHA_OPAQUE));

  SDL_Rect dstrect;
  SDL_Rect srcrect;
  srcrect.w = BUTTONS_XY;
  srcrect.h = BUTTONS_XY;

  dstrect.x = (exit_screen->w - t1->w) / 2;
  dstrect.y = (exit_screen->h - t1->h) / 2;
  SDL_BlitSurface(t1, NULL, exit_screen, &dstrect);

  dstrect.x = (exit_screen->w / 8) * 2;
  dstrect.y = (exit_screen->h / 4) * 3;
  srcrect.x = CROSS_X * BUTTONS_XY;
  srcrect.y = CROSS_Y * BUTTONS_XY;
  SDL_BlitSurface(buttons, &srcrect, exit_screen, &dstrect);
  dstrect.x += BUTTONS_XY;
  SDL_BlitSurface(t2, NULL, exit_screen, &dstrect);

  dstrect.x = ((exit_screen->w / 8) * 5) - BUTTONS_XY;
  srcrect.x = CIRCLE_X * BUTTONS_XY;
  srcrect.y = CIRCLE_Y * BUTTONS_XY;
  SDL_BlitSurface(buttons, &srcrect, exit_screen, &dstrect);
  dstrect.x += BUTTONS_XY;
  SDL_BlitSurface(t3, NULL, exit_screen, &dstrect);

  SDL_FreeSurface(t3);
  SDL_FreeSurface(t2);
  SDL_FreeSurface(t1);
  SDL_FreeSurface(buttons);

  return exit_screen;
}

SDL_Surface *create_help_screen(TTF_Font *font) {
  SDL_Surface *help_screen = load_surface(DATA_PATH "FUNDO" GRAPH_EXT);
  SDL_Surface *buttons = load_surface(DATA_PATH "BUTTONS" GRAPH_EXT);

  SDL_Color fg = {255, 0, 0, SDL_ALPHA_OPAQUE};

  SDL_Surface *t1 = TTF_RenderText_Blended(font, "AJUDA", fg);
  SDL_Surface *t2 = TTF_RenderText_Blended(font, "MOVER", fg);
  SDL_Surface *t3 = TTF_RenderText_Blended(font, "PINTAR", fg);
  SDL_Surface *t4 = TTF_RenderText_Blended(font, "DESENHO ANTERIOR", fg);
  SDL_Surface *t5 = TTF_RenderText_Blended(font, "PROXIMO DESENHO", fg);
  SDL_Surface *t6 = TTF_RenderText_Blended(font, "SALVAR", fg);
  SDL_Surface *t7 = TTF_RenderText_Blended(font, "SAIR", fg);
  SDL_Surface *t9 = TTF_RenderText_Blended(font, "VOLTAR", fg);

  fill_rounded_box_b(
      help_screen, MARGIN_X, MARGIN_Y, help_screen->w - (MARGIN_X * 2),
      help_screen->h - (MARGIN_Y * 2), 20,
      SDL_MapRGBA(help_screen->format, 16, 64, 16, SDL_ALPHA_OPAQUE));

  SDL_Rect dstrect;
  SDL_Rect srcrect;
  srcrect.w = BUTTONS_XY;
  srcrect.h = BUTTONS_XY;

  // title
  dstrect.x = (help_screen->w - t9->w - BUTTONS_XY) / 2;
  dstrect.y = (MARGIN_Y * 5) - (MARGIN_Y / 2);
  srcrect.x = CIRCLE_X * BUTTONS_XY;
  srcrect.y = CIRCLE_Y * BUTTONS_XY;
  SDL_BlitSurface(buttons, &srcrect, help_screen, &dstrect);
  dstrect.x += BUTTONS_XY;
  SDL_BlitSurface(t9, NULL, help_screen, &dstrect);
  dstrect.x = (help_screen->w - t1->w) / 2;
  dstrect.y = MARGIN_Y;
  SDL_BlitSurface(t1, NULL, help_screen, &dstrect);

#define ROW_SPACING (MARGIN_Y * 0.45)
#define COLUMN_SPACING (BUTTONS_XY * 7.6)
#define COLUMN_SPACING2 (BUTTONS_XY * 6)

  // move
  srcrect.x = L_X * BUTTONS_XY;
  srcrect.y = L_Y * BUTTONS_XY;
  dstrect.x = help_screen->w / 4;
  dstrect.y += ROW_SPACING;
  SDL_BlitSurface(buttons, &srcrect, help_screen, &dstrect);
  dstrect.x += COLUMN_SPACING;
  SDL_BlitSurface(t2, NULL, help_screen, &dstrect);

  // paint
  srcrect.x = CROSS_X * BUTTONS_XY;
  srcrect.y = CROSS_Y * BUTTONS_XY;
  dstrect.x = help_screen->w / 4;
  dstrect.y += ROW_SPACING;
  SDL_BlitSurface(buttons, &srcrect, help_screen, &dstrect);
  dstrect.x += COLUMN_SPACING;
  SDL_BlitSurface(t3, NULL, help_screen, &dstrect);

  // save
  srcrect.x = TRIANGLE_X * BUTTONS_XY;
  srcrect.y = TRIANGLE_Y * BUTTONS_XY;
  dstrect.x = help_screen->w / 4;
  dstrect.y += ROW_SPACING;
  SDL_BlitSurface(buttons, &srcrect, help_screen, &dstrect);
  dstrect.x += COLUMN_SPACING;
  SDL_BlitSurface(t6, NULL, help_screen, &dstrect);

  // help
  srcrect.x = SELECT_X * BUTTONS_XY;
  srcrect.y = SELECT_Y * BUTTONS_XY;
  dstrect.x = help_screen->w / 4;
  dstrect.y += ROW_SPACING;
  SDL_BlitSurface(buttons, &srcrect, help_screen, &dstrect);
  dstrect.x += COLUMN_SPACING;
  SDL_BlitSurface(t1, NULL, help_screen, &dstrect);

  // exit
  srcrect.x = START_X * BUTTONS_XY;
  srcrect.y = START_Y * BUTTONS_XY;
  dstrect.x = help_screen->w / 4;
  dstrect.y += ROW_SPACING;
  SDL_BlitSurface(buttons, &srcrect, help_screen, &dstrect);
  dstrect.x += COLUMN_SPACING;
  SDL_BlitSurface(t7, NULL, help_screen, &dstrect);

  // prev draw
  srcrect.x = L1_X * BUTTONS_XY;
  srcrect.y = L1_Y * BUTTONS_XY;
  dstrect.x = help_screen->w / 4;
  dstrect.y += ROW_SPACING;
  SDL_BlitSurface(buttons, &srcrect, help_screen, &dstrect);
  dstrect.x += COLUMN_SPACING2;
  SDL_BlitSurface(t4, NULL, help_screen, &dstrect);

  // next draw
  srcrect.x = R1_X * BUTTONS_XY;
  srcrect.y = R1_Y * BUTTONS_XY;
  dstrect.x = help_screen->w / 4;
  dstrect.y += ROW_SPACING;
  SDL_BlitSurface(buttons, &srcrect, help_screen, &dstrect);
  dstrect.x += COLUMN_SPACING2;
  SDL_BlitSurface(t5, NULL, help_screen, &dstrect);

  // free
  SDL_FreeSurface(t9);
  SDL_FreeSurface(t7);
  SDL_FreeSurface(t5);
  SDL_FreeSurface(t4);
  SDL_FreeSurface(t3);
  SDL_FreeSurface(t2);
  SDL_FreeSurface(t1);
  SDL_FreeSurface(buttons);

  return help_screen;
}

SDL_Surface *create_credit_screen(TTF_Font *font) {
  SDL_Surface *credit_screen = load_surface(DATA_PATH "FUNDO" GRAPH_EXT);
  SDL_Surface *buttons = load_surface(DATA_PATH "BUTTONS" GRAPH_EXT);

  SDL_Color fg = {255, 0, 0, SDL_ALPHA_OPAQUE};

  SDL_Surface *t1 = TTF_RenderText_Blended(font, "CREDITOS", fg);
  //  SDL_Surface *t7 = TTF_RenderText_Blended(font, "SAIR", fg);
  SDL_Surface *t9 = TTF_RenderText_Blended(font, "VOLTAR", fg);

  fill_rounded_box_b(
      credit_screen, MARGIN_X, MARGIN_Y, credit_screen->w - (MARGIN_X * 2),
      credit_screen->h - (MARGIN_Y * 2), 20,
      SDL_MapRGBA(credit_screen->format, 16, 64, 16, SDL_ALPHA_OPAQUE));

  SDL_Rect dstrect;
  SDL_Rect srcrect;
  srcrect.w = BUTTONS_XY;
  srcrect.h = BUTTONS_XY;

  // title
  dstrect.x = (credit_screen->w - t9->w - BUTTONS_XY) / 2;
  dstrect.y = (MARGIN_Y * 5) - (MARGIN_Y / 2);
  srcrect.x = CIRCLE_X * BUTTONS_XY;
  srcrect.y = CIRCLE_Y * BUTTONS_XY;
  SDL_BlitSurface(buttons, &srcrect, credit_screen, &dstrect);
  dstrect.x += BUTTONS_XY;
  SDL_BlitSurface(t9, NULL, credit_screen, &dstrect);
  dstrect.x = (credit_screen->w - t1->w) / 2;
  dstrect.y = MARGIN_Y;
  SDL_BlitSurface(t1, NULL, credit_screen, &dstrect);
  //***
  return credit_screen;
}