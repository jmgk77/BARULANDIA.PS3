#include "debug.h"

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
  dbglogger_printf("\n");
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
      dbglogger_printf("[%d]\tSDL_PIXELTYPE: %s\n", t, s);

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
    }
  }
}

void ret2psload() {
#ifdef PS3
  sysProcessExitSpawn2("/dev_hdd0/game/PSL145310/RELOAD.SELF", NULL, NULL, NULL,
                       0, 1001, SYS_PROCESS_SPAWN_STACK_SIZE_1M);
#endif
}
