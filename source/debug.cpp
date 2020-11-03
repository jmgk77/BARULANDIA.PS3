#include "debug.h"

void debug_joystick(SDL_Joystick *joystick) {
  dbglogger_printf("LX %d LY %d RX %d RY %d",
                   SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTX),
                   SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTY),
                   SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_RIGHTX),
                   SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_RIGHTY));
#define LOG_BTN(X)                                                             \
  if (SDL_JoystickGetButton(joystick, X) == SDL_PRESSED)                       \
  dbglogger_printf(#X)
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
  int nbDriver = SDL_GetNumRenderDrivers();

  for (int i = 0; i < nbDriver; i++) {
    SDL_RendererInfo info;
    SDL_GetRenderDriverInfo(i, &info);
    dbglogger_printf(
        "DRIVER [%d] %s (0x%04x): %s %s %s %s\n", i,
        info.name ? info.name : "Invalid driver", info.flags,
        (info.flags & SDL_RENDERER_SOFTWARE) ? "software" : "",
        (info.flags & SDL_RENDERER_ACCELERATED) ? "accelerated" : "",
        (info.flags & SDL_RENDERER_PRESENTVSYNC) ? "vsync" : "",
        (info.flags & SDL_RENDERER_TARGETTEXTURE) ? "texture_target" : "");

    dbglogger_printf("\tAVAILABLE TEXTURE FORMATS: %d\n",
                     info.num_texture_formats);
    for (Uint32 t = 0; t < info.num_texture_formats; t++) {
      int tf = info.texture_formats[t];

      dbglogger_printf("[%d.%d]\n", i, t);
      debug_format(tf);
    }
  }
}

void ret2psload() {
#ifdef PS3
  sysProcessExitSpawn2("/dev_hdd0/game/PSL145310/RELOAD.SELF", NULL, NULL, NULL,
                       0, 1001, SYS_PROCESS_SPAWN_STACK_SIZE_1M);
#endif
}

void debug_format(int tf) {
  const char *s;
  switch (SDL_PIXELTYPE(tf)) {
  case SDL_PIXELTYPE_UNKNOWN:
    s = "SDL_PIXELTYPE_UNKNOWN";
    break;
  case SDL_PIXELTYPE_INDEX1:
    s = "SDL_PIXELTYPE_INDEX1";
    break;
  case SDL_PIXELTYPE_INDEX4:
    s = "SDL_PIXELTYPE_INDEX4";
    break;
  case SDL_PIXELTYPE_INDEX8:
    s = "SDL_PIXELTYPE_INDEX8";
    break;
  case SDL_PIXELTYPE_PACKED8:
    s = "SDL_PIXELTYPE_PACKED8";
    break;
  case SDL_PIXELTYPE_PACKED16:
    s = "SDL_PIXELTYPE_PACKED16";
    break;
  case SDL_PIXELTYPE_PACKED32:
    s = "SDL_PIXELTYPE_PACKED32";
    break;
  case SDL_PIXELTYPE_ARRAYU8:
    s = "SDL_PIXELTYPE_ARRAYU8";
    break;
  case SDL_PIXELTYPE_ARRAYU16:
    s = "SDL_PIXELTYPE_ARRAYU16";
    break;
  case SDL_PIXELTYPE_ARRAYU32:
    s = "SDL_PIXELTYPE_ARRAYU32";
    break;
  case SDL_PIXELTYPE_ARRAYF16:
    s = "SDL_PIXELTYPE_ARRAYF16";
    break;
  case SDL_PIXELTYPE_ARRAYF32:
    s = "SDL_PIXELTYPE_ARRAYF32";
    break;
  default:
    s = "ERROR";
    break;
  }
  dbglogger_printf("\tSDL_PIXELTYPE: %s\n", s);

  switch (SDL_PIXELORDER(tf)) {
  case SDL_BITMAPORDER_NONE:
    s = "SDL_BITMAPORDER_NONE";
    break;
  case SDL_BITMAPORDER_4321:
    s = "SDL_BITMAPORDER_4321";
    break;
  case SDL_BITMAPORDER_1234:
    s = "SDL_BITMAPORDER_1234";
    break;

    /* case SDL_PACKEDORDER_NONE:
       s1 = "SDL_PACKEDORDER_NONE";
       break;
     case SDL_PACKEDORDER_XRGB:
       s1 = "SDL_PACKEDORDER_XRGB";
       break;
     case SDL_PACKEDORDER_RGBX:
       s1 = "SDL_PACKEDORDER_RGBX";
       break;*/
  case SDL_PACKEDORDER_ARGB:
    s = "SDL_PACKEDORDER_ARGB";
    break;
  case SDL_PACKEDORDER_RGBA:
    s = "SDL_PACKEDORDER_RGBA";
    break;
  case SDL_PACKEDORDER_XBGR:
    s = "SDL_PACKEDORDER_XBGR";
    break;
  case SDL_PACKEDORDER_BGRX:
    s = "SDL_PACKEDORDER_BGRX";
    break;
  case SDL_PACKEDORDER_ABGR:
    s = "SDL_PACKEDORDER_ABGR";
    break;
  case SDL_PACKEDORDER_BGRA:
    s = "SDL_PACKEDORDER_BGRA";
    break;

  /*case SDL_ARRAYORDER_NONE:
    s2 = "SDL_ARRAYORDER_NONE";
    break;
  case SDL_ARRAYORDER_RGB:
    s2 = "SDL_ARRAYORDER_RGB";
    break;
  case SDL_ARRAYORDER_RGBA:
    s2 = "SDL_ARRAYORDER_RGBA";
    break;
  case SDL_ARRAYORDER_ARGB:
    s2 = "SDL_ARRAYORDER_ARGB";
    break;
  case SDL_ARRAYORDER_BGR:
    s2 = "SDL_ARRAYORDER_BGR";
    break;
  case SDL_ARRAYORDER_BGRA:
    s2 = "SDL_ARRAYORDER_BGRA";
    break;
  case SDL_ARRAYORDER_ABGR:
    s2 = "SDL_ARRAYORDER_ABGR";
    break;*/
  default:
    s = "ERROR";
    break;
  }
  dbglogger_printf("\tSDL_PIXELORDER: %s\n", s);

  switch (SDL_PIXELLAYOUT(tf)) {

  case SDL_PACKEDLAYOUT_NONE:
    s = "SDL_PACKEDLAYOUT_NONE";
    break;
  case SDL_PACKEDLAYOUT_332:
    s = "SDL_PACKEDLAYOUT_332";
    break;
  case SDL_PACKEDLAYOUT_4444:
    s = "SDL_PACKEDLAYOUT_4444";
    break;
  case SDL_PACKEDLAYOUT_1555:
    s = "SDL_PACKEDLAYOUT_1555";
    break;
  case SDL_PACKEDLAYOUT_5551:
    s = "SDL_PACKEDLAYOUT_5551";
    break;
  case SDL_PACKEDLAYOUT_565:
    s = "SDL_PACKEDLAYOUT_565";
    break;
  case SDL_PACKEDLAYOUT_8888:
    s = "SDL_PACKEDLAYOUT_8888";
    break;
  case SDL_PACKEDLAYOUT_2101010:
    s = "SDL_PACKEDLAYOUT_2101010";
    break;
  case SDL_PACKEDLAYOUT_1010102:
    s = "SDL_PACKEDLAYOUT_1010102";
    break;
  default:
    s = "ERROR";
    break;
  }
  dbglogger_printf("\tSDL_PIXELLAYOUT: %s\n", s);

  dbglogger_printf("\tBITS(%d) BYTES(%d)\n", SDL_BITSPERPIXEL(tf),
                   SDL_BYTESPERPIXEL(tf));

  dbglogger_printf("\tHAVE PALETTE: %s\n",
                   SDL_ISPIXELFORMAT_INDEXED(tf) ? "TRUE" : "FALSE");
  dbglogger_printf("\tHAVE ALPHA: %s\n",
                   SDL_ISPIXELFORMAT_ALPHA(tf) ? "TRUE" : "FALSE");
  dbglogger_printf("\tUNIQUE FORMAT: %s\n",
                   SDL_ISPIXELFORMAT_FOURCC(tf) ? "TRUE" : "FALSE");
  return;
}

void debug_texture(SDL_Texture *texture) {
  Uint32 format;
  int access, w, h;

  SDL_QueryTexture(texture, &format, &access, &w, &h);

  dbglogger_printf("TEXTURE:\n");
  const char *s;
  switch (format) {

  case SDL_PIXELFORMAT_UNKNOWN:
    s = "SDL_PIXELFORMAT_UNKNOWN";
    break;
  case SDL_PIXELFORMAT_INDEX1LSB:
    s = "SDL_PIXELFORMAT_INDEX1LSB";
    break;
  case SDL_PIXELFORMAT_INDEX1MSB:
    s = "SDL_PIXELFORMAT_INDEX1MSB";
    break;
  case SDL_PIXELFORMAT_INDEX4LSB:
    s = "SDL_PIXELFORMAT_INDEX4LSB";
    break;
  case SDL_PIXELFORMAT_INDEX4MSB:
    s = "SDL_PIXELFORMAT_INDEX4MSB";
    break;
  case SDL_PIXELFORMAT_INDEX8:
    s = "SDL_PIXELFORMAT_INDEX8";
    break;
  case SDL_PIXELFORMAT_RGB332:
    s = "SDL_PIXELFORMAT_RGB332";
    break;
  case SDL_PIXELFORMAT_RGB444:
    s = "SDL_PIXELFORMAT_RGB444";
    break;
  case SDL_PIXELFORMAT_RGB555:
    s = "SDL_PIXELFORMAT_RGB555";
    break;
  case SDL_PIXELFORMAT_BGR555:
    s = "SDL_PIXELFORMAT_BGR555";
    break;
  case SDL_PIXELFORMAT_ARGB4444:
    s = "SDL_PIXELFORMAT_ARGB4444";
    break;
  case SDL_PIXELFORMAT_RGBA4444:
    s = "SDL_PIXELFORMAT_RGBA4444";
    break;
  case SDL_PIXELFORMAT_ABGR4444:
    s = "SDL_PIXELFORMAT_ABGR4444";
    break;
  case SDL_PIXELFORMAT_BGRA4444:
    s = "SDL_PIXELFORMAT_BGRA4444";
    break;
  case SDL_PIXELFORMAT_ARGB1555:
    s = "SDL_PIXELFORMAT_ARGB1555";
    break;
  case SDL_PIXELFORMAT_RGBA5551:
    s = "SDL_PIXELFORMAT_RGBA5551";
    break;
  case SDL_PIXELFORMAT_ABGR1555:
    s = "SDL_PIXELFORMAT_ABGR1555";
    break;
  case SDL_PIXELFORMAT_BGRA5551:
    s = "SDL_PIXELFORMAT_BGRA5551";
    break;
  case SDL_PIXELFORMAT_RGB565:
    s = "SDL_PIXELFORMAT_RGB565";
    break;
  case SDL_PIXELFORMAT_BGR565:
    s = "SDL_PIXELFORMAT_BGR565";
    break;
  case SDL_PIXELFORMAT_RGB24:
    s = "SDL_PIXELFORMAT_RGB24";
    break;
  case SDL_PIXELFORMAT_BGR24:
    s = "SDL_PIXELFORMAT_BGR24";
    break;
  case SDL_PIXELFORMAT_RGB888:
    s = "SDL_PIXELFORMAT_RGB888";
    break;
  case SDL_PIXELFORMAT_RGBX8888:
    s = "SDL_PIXELFORMAT_RGBX8888";
    break;
  case SDL_PIXELFORMAT_BGR888:
    s = "SDL_PIXELFORMAT_BGR888";
    break;
  case SDL_PIXELFORMAT_BGRX8888:
    s = "SDL_PIXELFORMAT_BGRX8888";
    break;
  case SDL_PIXELFORMAT_ARGB8888:
    s = "SDL_PIXELFORMAT_ARGB8888";
    break;
  case SDL_PIXELFORMAT_RGBA8888:
    s = "SDL_PIXELFORMAT_RGBA8888";
    break;
  /*case SDL_PIXELFORMAT_ABGR8888:
    s = "SDL_PIXELFORMAT_ABGR8888";
    break;
  case SDL_PIXELFORMAT_BGRA8888:
    s = "SDL_PIXELFORMAT_BGRA8888";
    break;*/
  case SDL_PIXELFORMAT_ARGB2101010:
    s = "SDL_PIXELFORMAT_ARGB2101010";
    break;
#ifndef PS3
  case SDL_PIXELFORMAT_RGBA32:
    s = "SDL_PIXELFORMAT_RGBA32";
    break;
  case SDL_PIXELFORMAT_ARGB32:
    s = "SDL_PIXELFORMAT_ARGB32";
    break;
#endif
  /*case SDL_PIXELFORMAT_BGRA32:
    s = "SDL_PIXELFORMAT_BGRA32";
    break;
  case SDL_PIXELFORMAT_ABGR32:
    s = "SDL_PIXELFORMAT_ABGR32";
    break;*/
  case SDL_PIXELFORMAT_YV12:
    s = "SDL_PIXELFORMAT_YV12";
    break;
  case SDL_PIXELFORMAT_IYUV:
    s = "SDL_PIXELFORMAT_IYUV";
    break;
  case SDL_PIXELFORMAT_YUY2:
    s = "SDL_PIXELFORMAT_YUY2";
    break;
  case SDL_PIXELFORMAT_UYVY:
    s = "SDL_PIXELFORMAT_UYVY";
    break;
  case SDL_PIXELFORMAT_YVYU:
    s = "SDL_PIXELFORMAT_YVYU";
    break;
    /*case SDL_PIXELFORMAT_NV12:
      s = "SDL_PIXELFORMAT_NV12";
    case SDL_PIXELFORMAT_NV21:
      s = "SDL_PIXELFORMAT_NV21";
      break;*/

  default:
    s = "ERROR";
    break;
  }
  dbglogger_printf("\tFORMAT: %s\n", s);

  switch (access) {

  case SDL_TEXTUREACCESS_STATIC:
    s = "SDL_TEXTUREACCESS_STATIC";
    break;
  case SDL_TEXTUREACCESS_STREAMING:
    s = "SDL_TEXTUREACCESS_STREAMIN";
    break;
  case SDL_TEXTUREACCESS_TARGET:
    s = "SDL_TEXTUREACCESS_TARGET";
    break;

  default:
    s = "ERROR";
    break;
  }
  dbglogger_printf("\tACCESS: %s\n", s);

  dbglogger_printf("\tSIZE: %d x %d \n", w, h);
}

void debug_font(TTF_Font *font) {
  dbglogger_printf("TTF_FontHeight          : %d\n", TTF_FontHeight(font));
  dbglogger_printf("TTF_FontAscent          : %d\n", TTF_FontAscent(font));
  dbglogger_printf("TTF_FontDescent         : %d\n", TTF_FontDescent(font));
  dbglogger_printf("TTF_FontLineSkip        : %d\n", TTF_FontLineSkip(font));
  dbglogger_printf("TTF_FontFaceIsFixedWidth: %d\n",
                   TTF_FontFaceIsFixedWidth(font));

  dbglogger_printf("TTF_FontFaceFamilyName  : \"%s\"\n",
                   TTF_FontFaceFamilyName(font));
  dbglogger_printf("TTF_FontFaceStyleName   : \"%s\"\n",
                   TTF_FontFaceStyleName(font));
  if (TTF_GlyphIsProvided(font, 'g')) {
    int minx, maxx, miny, maxy, advance;
    TTF_GlyphMetrics(font, 'g', &minx, &maxx, &miny, &maxy, &advance);
    dbglogger_printf("TTF_GlyphMetrics('g'):\n\tminx=%d\n\tmaxx=%d\n\tminy=%"
                     "d\n\tmaxy=%d\n\tadvance=%d\n",
                     minx, maxx, miny, maxy, advance);
  } else {
    dbglogger_printf("TTF_GlyphMetrics('g'): unavailable in font!\n");
  }
}

void debug_window(SDL_Window *window) {
  SDL_SysWMinfo info;

  SDL_VERSION(
      &info.version); /* initialize info structure with SDL version info */

  if (SDL_GetWindowWMInfo(window,
                          &info)) { /* the call returns true on success */
    /* success */
    const char *subsystem = "an unknown system!";
    switch (info.subsystem) {
    case SDL_SYSWM_UNKNOWN:
      break;
    case SDL_SYSWM_WINDOWS:
      subsystem = "Microsoft Windows(TM)";
      break;
    case SDL_SYSWM_X11:
      subsystem = "X Window System";
      break;
#if SDL_VERSION_ATLEAST(2, 0, 3)
    case SDL_SYSWM_WINRT:
      subsystem = "WinRT";
      break;
#endif
    case SDL_SYSWM_DIRECTFB:
      subsystem = "DirectFB";
      break;
    case SDL_SYSWM_COCOA:
      subsystem = "Apple OS X";
      break;
    case SDL_SYSWM_UIKIT:
      subsystem = "UIKit";
      break;
#if SDL_VERSION_ATLEAST(2, 0, 2)
    case SDL_SYSWM_WAYLAND:
      subsystem = "Wayland";
      break;
    case SDL_SYSWM_MIR:
      subsystem = "Mir";
      break;
#endif
#if SDL_VERSION_ATLEAST(2, 0, 4)
    case SDL_SYSWM_ANDROID:
      subsystem = "Android";
      break;
#endif
#if SDL_VERSION_ATLEAST(2, 0, 5)
    case SDL_SYSWM_VIVANTE:
      subsystem = "Vivante";
      break;
#endif
    case SDL_SYSWM_OS2:
      subsystem = "OS/2";
      break;
#if SDL_VERSION_ATLEAST(2, 0, 13)
    case SDL_SYSWM_HAIKU:
      subsystem = "HAIKU";
      break;
#endif
    }

    dbglogger_printf("This program is running SDL version %d.%d.%d on %s\n",
                     (int)info.version.major, (int)info.version.minor,
                     (int)info.version.patch, subsystem);
  } else {
    /* call failed */
    dbglogger_printf("Couldn't get window information: %s\n", SDL_GetError());
  }
}

void debug_renderer(SDL_Renderer *renderer) {
  SDL_RendererInfo info;
  SDL_GetRendererInfo(renderer, &info);
  dbglogger_printf("CURRENT %s (0x%04x): %s %s %s %s\n",
                   info.name ? info.name : "Invalid driver", info.flags,
                   (info.flags & SDL_RENDERER_SOFTWARE) ? "software" : "",
                   (info.flags & SDL_RENDERER_ACCELERATED) ? "accelerated" : "",
                   (info.flags & SDL_RENDERER_PRESENTVSYNC) ? "vsync" : "",
                   (info.flags & SDL_RENDERER_TARGETTEXTURE) ? "texture_target"
                                                             : "");

  dbglogger_printf("\tAVAILABLE TEXTURE FORMATS: %d\n",
                   info.num_texture_formats);
  for (Uint32 t = 0; t < info.num_texture_formats; t++) {
    int tf = info.texture_formats[t];

    dbglogger_printf("[X.%d]\n", t);
    debug_format(tf);
  }
}
