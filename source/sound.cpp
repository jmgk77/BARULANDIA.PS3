#include "sound.h"

int do_effect_play(void *index) {
  char buf[128];
  snprintf(buf, 128, "%sSOUND%d.WAV", DATA_PATH, index);
  dbglogger_printf("PLAYING: %s\n", buf);

  // mikmod
  SAMPLE *wave = Sample_Load(buf);
  if (!wave) {
    dbglogger_printf("Could not load: %s\n", MikMod_strerror(MikMod_errno));
    return 0;
  }

  int id = Sample_Play(wave, 0, SFX_CRITICAL);
  Voice_SetVolume(id, 256);
  do {
    MikMod_Update();
    SDL_Delay(10);
  } while (!Voice_Stopped(id));

  Sample_Free(wave);
  return 0;
}

void effect_play(int index) {
  SDL_Thread *thread = SDL_CreateThread(do_effect_play, (void *)index);

  if (thread == NULL) {
    dbglogger_printf("SDL_CreateThread failed: %s\n", SDL_GetError());
  }
}

void sound_init() {
  dbglogger_printf("MikMod version %ld.%ld.%ld\n", LIBMIKMOD_VERSION_MAJOR,
                   LIBMIKMOD_VERSION_MINOR, LIBMIKMOD_REVISION);

  MikMod_InitThreads();

  MikMod_RegisterAllDrivers();
  MikMod_RegisterAllLoaders();

  char s[] = "";
  if (MikMod_Init(s)) {
    dbglogger_printf("Could not initialize sound: %s",
                     MikMod_strerror(MikMod_errno));
    sound_end();
    return;
  }
  MikMod_SetNumVoices(-1, 2);
  MikMod_EnableOutput();
  debug_audio();
}

void sound_end() {
  MikMod_DisableOutput();
  MikMod_Exit();
}
