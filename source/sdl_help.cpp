// barulandia for ps3 (c) jmgk 2020

#include "sdl_help.h"

void PutPixel32_nolock(SDL_Surface *surface, int x, int y, Uint32 color) {
    Uint8 *pixel = (Uint8 *)surface->pixels;
    pixel += (y * surface->pitch) + (x * sizeof(Uint32));
    *((Uint32 *)pixel) = color;
}

Uint32 GetPixel32_nolock(SDL_Surface *surface, int x, int y) {
    Uint8 *pixel = (Uint8 *)surface->pixels;
    pixel += (y * surface->pitch) + (x * sizeof(Uint32));
    return *((Uint32 *)pixel);
}

void debug_joystick(SDL_Joystick *joystick) {
    dbglogger_log("LX %d LY %d RX %d RY %d",
                  SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTX),
                  SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTY),
                  SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_RIGHTX),
                  SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_RIGHTY));
#define LOG_BTN(X)                                                             \
  if (SDL_JoystickGetButton(joystick, X) == SDL_PRESSED)                       \
  dbglogger_log(#X)
    LOG_BTN(SDL_CONTROLLER_BUTTON_LEFT);
    LOG_BTN(SDL_CONTROLLER_BUTTON_DOWN);
    LOG_BTN(SDL_CONTROLLER_BUTTON_RIGHT);
    LOG_BTN(SDL_CONTROLLER_BUTTON_UP);
    LOG_BTN(SDL_CONTROLLER_BUTTON_START);
    LOG_BTN(SDL_CONTROLLER_BUTTON_R3);
    LOG_BTN(SDL_CONTROLLER_BUTTON_L3);
    LOG_BTN(SDL_CONTROLLER_BUTTON_SELECT);
    LOG_BTN(SDL_CONTROLLER_BUTTON_SQUARE);
    LOG_BTN(SDL_CONTROLLER_BUTTON_CROSS);
    LOG_BTN(SDL_CONTROLLER_BUTTON_CIRCLE);
    LOG_BTN(SDL_CONTROLLER_BUTTON_TRIANGLE);
    LOG_BTN(SDL_CONTROLLER_BUTTON_R1);
    LOG_BTN(SDL_CONTROLLER_BUTTON_L1);
    LOG_BTN(SDL_CONTROLLER_BUTTON_R2);
    LOG_BTN(SDL_CONTROLLER_BUTTON_L2);
}

void debug_video() {
    const SDL_VideoInfo *vinfo = SDL_GetVideoInfo();
    /*
       typedef struct{
       Uint32 hw_available:1;
       Uint32 wm_available:1;
       Uint32 blit_hw:1;
       Uint32 blit_hw_CC:1;
       Uint32 blit_hw_A:1;
       Uint32 blit_sw:1;
       Uint32 blit_sw_CC:1;
       Uint32 blit_sw_A:1;
       Uint32 blit_fill;
       Uint32 video_mem;
       SDL_PixelFormat *vfmt;
       } SDL_VideoInfo;
     */
    dbglogger_printf("<<SDL_VideoInfo>>\nhw_available\t%d\nwm_available\t%"
                     "d\nblit_hw\t\t%d\nblit_hw_"
                     "CC\t%d\nblit_hw_A\t%d\n",
                     vinfo->hw_available, vinfo->wm_available, vinfo->blit_hw,
                     vinfo->blit_hw_CC, vinfo->blit_hw_A);
    dbglogger_printf(
        "blit_sw\t\t%d\nblit_sw_CC\t%d\nblit_sw_A\t%d\nblit_fill\t%d\nvideo_"
        "mem\t0x%x\n",
        vinfo->blit_sw, vinfo->blit_sw_CC, vinfo->blit_sw_A, vinfo->blit_fill,
        vinfo->video_mem);
    /*
       typedef struct SDL_PixelFormat {
       SDL_Palette *palette;
       Uint8  BitsPerPixel;
       Uint8  BytesPerPixel;
       Uint8  Rloss, Gloss, Bloss, Aloss;
       Uint8  Rshift, Gshift, Bshift, Ashift;
       Uint32 Rmask, Gmask, Bmask, Amask;
       Uint32 colorkey;
       Uint8  alpha;
       } SDL_PixelFormat;
     */
    dbglogger_printf("<<SDL_PixelFormat>>\npalette\t\t0x%x\n",
                     vinfo->vfmt->palette);
    dbglogger_printf(
        "BitsPerPixel\t0x%x\nBytesPerPixel\t0x%x\nRloss\t\t0x%x\nGloss\t\t0x%"
        "x\nBloss\t\t0x%x\nAloss\t\t0x%x\n",
        vinfo->vfmt->BitsPerPixel, vinfo->vfmt->BytesPerPixel, vinfo->vfmt->Rloss,
        vinfo->vfmt->Gloss, vinfo->vfmt->Bloss, vinfo->vfmt->Aloss);
    dbglogger_printf(
        "Rmask\t\t0x%x\nGmask\t\t0x%x\nBmask\t\t0x%x\nAmask\t\t0x%x\n",
        vinfo->vfmt->Rshift, vinfo->vfmt->Gshift, vinfo->vfmt->Bshift,
        vinfo->vfmt->Ashift);
    dbglogger_printf(
        "Rmask\t\t0x%x\nGmask\t\t0x%x\nBmask\t\t0x%x\nAmask\t\t0x%x\n",
        vinfo->vfmt->Rmask, vinfo->vfmt->Gmask, vinfo->vfmt->Bmask,
        vinfo->vfmt->Amask);
#ifndef PS3
    dbglogger_printf("colorkey\t0x%x\nalpha\t\t0x%x\n", vinfo->vfmt->colorkey,
                     vinfo->vfmt->alpha);
#endif
}

void fade_in_out(SDL_Surface *screen, SDL_Surface *image, Uint32 color,
                 int delay, bool in_out) {
    // center image in display
    SDL_Rect r;
    r.x = (screen->w - image->w) / 2;
    r.y = (screen->h - image->h) / 2;
    r.w = image->w;
    r.h = image->h;

    // Create a blank surface that is the same size as our screen
    SDL_Surface *tmp =
        SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, WIDTH, HEIGHT, 32,
                             rmask, gmask, bmask, amask);
    // Convert it to the format of the screen
    SDL_Surface *tmp_screen = SDL_DisplayFormat(tmp);
    // Free the created surface
    SDL_FreeSurface(tmp);

    int alpha_init, alpha_end, alpha_step;
    if (in_out) {
        // fade in
        alpha_init = 0;
        alpha_end = 255;
        alpha_step = 1;
    } else {
        // fade out
        alpha_init = 255;
        alpha_end = 0;
        alpha_step = -1;
    }
    for (int alpha = alpha_init; alpha != alpha_end; alpha += alpha_step) {
        // Clear both the screens
        SDL_FillRect(tmp_screen, 0, color);
        SDL_FillRect(screen, 0, color);
        // Draw the bitmap to the constructed vitual screen
        SDL_BlitSurface(image, NULL, tmp_screen, &r);
        // Set the alpha of the constructed screen
        SDL_SetAlpha(tmp_screen, SDL_SRCALPHA, alpha);
        // Draw the constructed surface to the primary surface now
        SDL_BlitSurface(tmp_screen, NULL, screen, 0);

        SDL_Flip(screen);
        SDL_Delay(delay);
    }
}
