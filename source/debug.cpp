#include "debug.h"

void debug_keyboard(SDL_KeyboardEvent *key) {
  if (key->type == SDL_KEYUP)
    dbglogger_printf("Release:-");
  else
    dbglogger_printf("Press:-");
  dbglogger_printf("\tScancode: 0x%02X", key->keysym.scancode);
  dbglogger_printf(", Name: %s", SDL_GetKeyName(key->keysym.sym));

  if (key->type == SDL_KEYDOWN) {

    dbglogger_printf(", Unicode: ");
    if (key->keysym.unicode < 0x80 && key->keysym.unicode > 0) {
      dbglogger_printf("%c (0x%04X)", (char)key->keysym.unicode,
                       key->keysym.unicode);
    } else {
      dbglogger_printf("? (0x%04X)", key->keysym.unicode);
    }
  }
  int mod = (key->keysym.mod);
  dbglogger_printf("\tModifers: ");

  if (mod == KMOD_NONE) {
    dbglogger_printf("None\n");
    return;
  }

  if (mod & KMOD_NUM)
    dbglogger_printf("NUMLOCK ");
  if (mod & KMOD_CAPS)
    dbglogger_printf("CAPSLOCK ");
  if (mod & KMOD_LCTRL)
    dbglogger_printf("LCTRL ");
  if (mod & KMOD_RCTRL)
    dbglogger_printf("RCTRL ");
  if (mod & KMOD_RSHIFT)
    dbglogger_printf("RSHIFT ");
  if (mod & KMOD_LSHIFT)
    dbglogger_printf("LSHIFT ");
  if (mod & KMOD_RALT)
    dbglogger_printf("RALT ");
  if (mod & KMOD_LALT)
    dbglogger_printf("LALT ");
  if (mod & KMOD_CTRL)
    dbglogger_printf("CTRL ");
  if (mod & KMOD_SHIFT)
    dbglogger_printf("SHIFT ");
  if (mod & KMOD_ALT)
    dbglogger_printf("ALT ");
  dbglogger_printf("\n");
}

void debug_joystick(SDL_Joystick *joystick) {
#ifdef DEBUG
  // dbglogger_printf("LX %d LY %d RX %d RY %d\n",
  //                  SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTX),
  //                  SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTY),
  //                  SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_RIGHTX),
  //                  SDL_JoystickGetAxis(joystick,
  //                  SDL_CONTROLLER_AXIS_RIGHTY));
#define LOG_BTN(X)                                                             \
  if (SDL_JoystickGetButton(joystick, X) == SDL_PRESSED)                       \
  dbglogger_printf(#X "\n")
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
#endif
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

void ret2psload() {
#ifdef DEBUG
#ifdef PS3
  sysProcessExitSpawn2("/dev_hdd0/game/PSL145310/RELOAD.SELF", NULL, NULL, NULL,
                       0, 1001, SYS_PROCESS_SPAWN_STACK_SIZE_1M);
#endif
#endif
}

// void debug_format(int tf) {
// #ifdef DEBUG
//   const char *s;
//   switch (SDL_PIXELTYPE(tf)) {
//   case SDL_PIXELTYPE_UNKNOWN:
//     s = "SDL_PIXELTYPE_UNKNOWN";
//     break;
//   case SDL_PIXELTYPE_INDEX1:
//     s = "SDL_PIXELTYPE_INDEX1";
//     break;
//   case SDL_PIXELTYPE_INDEX4:
//     s = "SDL_PIXELTYPE_INDEX4";
//     break;
//   case SDL_PIXELTYPE_INDEX8:
//     s = "SDL_PIXELTYPE_INDEX8";
//     break;
//   case SDL_PIXELTYPE_PACKED8:
//     s = "SDL_PIXELTYPE_PACKED8";
//     break;
//   case SDL_PIXELTYPE_PACKED16:
//     s = "SDL_PIXELTYPE_PACKED16";
//     break;
//   case SDL_PIXELTYPE_PACKED32:
//     s = "SDL_PIXELTYPE_PACKED32";
//     break;
//   case SDL_PIXELTYPE_ARRAYU8:
//     s = "SDL_PIXELTYPE_ARRAYU8";
//     break;
//   case SDL_PIXELTYPE_ARRAYU16:
//     s = "SDL_PIXELTYPE_ARRAYU16";
//     break;
//   case SDL_PIXELTYPE_ARRAYU32:
//     s = "SDL_PIXELTYPE_ARRAYU32";
//     break;
//   case SDL_PIXELTYPE_ARRAYF16:
//     s = "SDL_PIXELTYPE_ARRAYF16";
//     break;
//   case SDL_PIXELTYPE_ARRAYF32:
//     s = "SDL_PIXELTYPE_ARRAYF32";
//     break;
//   default:
//     s = "ERROR";
//     break;
//   }
//   dbglogger_printf("\tSDL_PIXELTYPE: %s\n", s);

//   switch (SDL_PIXELORDER(tf)) {
//   case SDL_BITMAPORDER_NONE:
//     s = "SDL_BITMAPORDER_NONE";
//     break;
//   case SDL_BITMAPORDER_4321:
//     s = "SDL_BITMAPORDER_4321";
//     break;
//   case SDL_BITMAPORDER_1234:
//     s = "SDL_BITMAPORDER_1234";
//     break;

//     /* case SDL_PACKEDORDER_NONE:
//        s1 = "SDL_PACKEDORDER_NONE";
//        break;
//      case SDL_PACKEDORDER_XRGB:
//        s1 = "SDL_PACKEDORDER_XRGB";
//        break;
//      case SDL_PACKEDORDER_RGBX:
//        s1 = "SDL_PACKEDORDER_RGBX";
//        break;*/
//   case SDL_PACKEDORDER_ARGB:
//     s = "SDL_PACKEDORDER_ARGB";
//     break;
//   case SDL_PACKEDORDER_RGBA:
//     s = "SDL_PACKEDORDER_RGBA";
//     break;
//   case SDL_PACKEDORDER_XBGR:
//     s = "SDL_PACKEDORDER_XBGR";
//     break;
//   case SDL_PACKEDORDER_BGRX:
//     s = "SDL_PACKEDORDER_BGRX";
//     break;
//   case SDL_PACKEDORDER_ABGR:
//     s = "SDL_PACKEDORDER_ABGR";
//     break;
//   case SDL_PACKEDORDER_BGRA:
//     s = "SDL_PACKEDORDER_BGRA";
//     break;

//   /*case SDL_ARRAYORDER_NONE:
//     s2 = "SDL_ARRAYORDER_NONE";
//     break;
//   case SDL_ARRAYORDER_RGB:
//     s2 = "SDL_ARRAYORDER_RGB";
//     break;
//   case SDL_ARRAYORDER_RGBA:
//     s2 = "SDL_ARRAYORDER_RGBA";
//     break;
//   case SDL_ARRAYORDER_ARGB:
//     s2 = "SDL_ARRAYORDER_ARGB";
//     break;
//   case SDL_ARRAYORDER_BGR:
//     s2 = "SDL_ARRAYORDER_BGR";
//     break;
//   case SDL_ARRAYORDER_BGRA:
//     s2 = "SDL_ARRAYORDER_BGRA";
//     break;
//   case SDL_ARRAYORDER_ABGR:
//     s2 = "SDL_ARRAYORDER_ABGR";
//     break;*/
//   default:
//     s = "ERROR";
//     break;
//   }
//   dbglogger_printf("\tSDL_PIXELORDER: %s\n", s);

//   switch (SDL_PIXELLAYOUT(tf)) {

//   case SDL_PACKEDLAYOUT_NONE:
//     s = "SDL_PACKEDLAYOUT_NONE";
//     break;
//   case SDL_PACKEDLAYOUT_332:
//     s = "SDL_PACKEDLAYOUT_332";
//     break;
//   case SDL_PACKEDLAYOUT_4444:
//     s = "SDL_PACKEDLAYOUT_4444";
//     break;
//   case SDL_PACKEDLAYOUT_1555:
//     s = "SDL_PACKEDLAYOUT_1555";
//     break;
//   case SDL_PACKEDLAYOUT_5551:
//     s = "SDL_PACKEDLAYOUT_5551";
//     break;
//   case SDL_PACKEDLAYOUT_565:
//     s = "SDL_PACKEDLAYOUT_565";
//     break;
//   case SDL_PACKEDLAYOUT_8888:
//     s = "SDL_PACKEDLAYOUT_8888";
//     break;
//   case SDL_PACKEDLAYOUT_2101010:
//     s = "SDL_PACKEDLAYOUT_2101010";
//     break;
//   case SDL_PACKEDLAYOUT_1010102:
//     s = "SDL_PACKEDLAYOUT_1010102";
//     break;
//   default:
//     s = "ERROR";
//     break;
//   }
//   dbglogger_printf("\tSDL_PIXELLAYOUT: %s\n", s);

//   dbglogger_printf("\tBITS(%d) BYTES(%d)\n", SDL_BITSPERPIXEL(tf),
//                    SDL_BYTESPERPIXEL(tf));

//   dbglogger_printf("\tHAVE PALETTE: %s\n",
//                    SDL_ISPIXELFORMAT_INDEXED(tf) ? "TRUE" : "FALSE");
//   dbglogger_printf("\tHAVE ALPHA: %s\n",
//                    SDL_ISPIXELFORMAT_ALPHA(tf) ? "TRUE" : "FALSE");
//   dbglogger_printf("\tUNIQUE FORMAT: %s\n",
//                    SDL_ISPIXELFORMAT_FOURCC(tf) ? "TRUE" : "FALSE");
//   return;
// #endif
// }

// void debug_font(TTF_Font *font) {
// #ifdef DEBUG
//   dbglogger_printf("TTF_FontHeight          : %d\n", TTF_FontHeight(font));
//   dbglogger_printf("TTF_FontAscent          : %d\n", TTF_FontAscent(font));
//   dbglogger_printf("TTF_FontDescent         : %d\n", TTF_FontDescent(font));
//   dbglogger_printf("TTF_FontLineSkip        : %d\n", TTF_FontLineSkip(font));
//   dbglogger_printf("TTF_FontFaceIsFixedWidth: %d\n",
//                    TTF_FontFaceIsFixedWidth(font));

//   dbglogger_printf("TTF_FontFaceFamilyName  : \"%s\"\n",
//                    TTF_FontFaceFamilyName(font));
//   dbglogger_printf("TTF_FontFaceStyleName   : \"%s\"\n",
//                    TTF_FontFaceStyleName(font));
//   if (TTF_GlyphIsProvided(font, 'g')) {
//     int minx, maxx, miny, maxy, advance;
//     TTF_GlyphMetrics(font, 'g', &minx, &maxx, &miny, &maxy, &advance);
//     dbglogger_printf("TTF_GlyphMetrics('g'):\n\tminx=%d\n\tmaxx=%d\n\tminy=%"
//                      "d\n\tmaxy=%d\n\tadvance=%d\n",
//                      minx, maxx, miny, maxy, advance);
//   } else {
//     dbglogger_printf("TTF_GlyphMetrics('g'): unavailable in font!\n");
//   }
// #endif
// }

// void debug_audio_spec(SDL_AudioSpec *a) {
// #ifdef DEBUG
//   dbglogger_printf("freq %d\n", a->freq);
//   const char *s;
//   switch (a->format) {
//   case AUDIO_S8:
//     s = "AUDIO_S8";
//     break;
//   case AUDIO_U8:
//     s = "AUDIO_U8";
//     break;
//   case AUDIO_S16LSB:
//     s = "AUDIO_S16LSB";
//     break;
//   case AUDIO_S16MSB:
//     s = "AUDIO_S16MSB";
//     break;
//     /*  case AUDIO_S16SYS:
//         s = "AUDIO_S16SYS";
//         break;
//       case AUDIO_S16:
//         s = "AUDIO_S16";
//         break;
//       case AUDIO_S16LSB:
//         s = "AUDIO_S16LSB";
//         break;*/
//   case AUDIO_U16LSB:
//     s = "AUDIO_U16LSB";
//     break;
//   case AUDIO_U16MSB:
//     s = "AUDIO_U16MSB";
//     break;
//     /*  case AUDIO_U16SYS:
//         s = "AUDIO_U16SYS";
//         break;
//       case AUDIO_U16:
//         s = "AUDIO_U16";
//         break;
//       case AUDIO_U16LSB:
//         s = "AUDIO_U16LSB";
//         break;*/
//   case AUDIO_S32LSB:
//     s = "AUDIO_S32LSB";
//     break;
//   case AUDIO_S32MSB:
//     s = "AUDIO_S32MSB";
//     break;
//     /*  case AUDIO_S32SYS:
//         s = "AUDIO_S32SYS";
//         break;
//       case AUDIO_S32:
//         s = "AUDIO_S32";
//         break;
//       case AUDIO_S32LSB:
//         s = "AUDIO_S32LSB";
//         break;*/
//   case AUDIO_F32LSB:
//     s = "AUDIO_F32LSB";
//     break;
//   case AUDIO_F32MSB:
//     s = "AUDIO_F32MSB";
//     break;
//   /*  case AUDIO_F32SYS:
//       s = "AUDIO_F32SYS";
//       break;
//     case AUDIO_F32:
//       s = "AUDIO_F32";
//       break;
//     case AUDIO_F32LSB:
//       s = "AUDIO_F32LSB";
//       break;*/
//   default:
//     s = "???";
//     break;
//   }
//   dbglogger_printf("format %s\n", s);
//   dbglogger_printf("channels %d\n", a->channels);
//   dbglogger_printf("silence %d\n", a->silence);
//   dbglogger_printf("samples %d\n", a->samples);
//   dbglogger_printf("size %d\n", a->size);
// #endif
// }

// #ifdef DEBUG
// #ifndef PS3

// extern SDL_Renderer *renderer;

// void debug_screenshot() {
//   char buf[MAX_STRING];
//   time_t rawtime;
//   struct tm *t;
//   time(&rawtime);
//   t = gmtime(&rawtime);
//   snprintf(buf, MAX_STRING, "SCREENSHOT_%d_%02d_%02d_%02d_%02d_%02d%s",
//            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour,
//            t->tm_min,
//            t->tm_sec, GRAPH_EXT);
//   SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(
//       0, WIDTH, HEIGHT, 32, SDL_PIXELFORMAT_ARGB8888);
//   SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888,
//                        surface->pixels, surface->pitch);
//   IMG_SavePNG(surface, buf);
//   SDL_FreeSurface(surface);
// }
// #endif
// #endif
