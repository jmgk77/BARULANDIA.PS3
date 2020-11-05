#include "sound.h"

void effect_play(int index) {
  char buf[128];
  snprintf(buf, 128, "%sSOUND%d.WAV", DATA_PATH, index);
  dbglogger_printf("PLAYING: %s\n", buf);

  SAMPLE *sfx1 = Sample_Load(buf);
  if (!sfx1) {
    dbglogger_printf("Could not load: %s\n", MikMod_strerror(MikMod_errno));
    return;
  }

  int v1 = Sample_Play(sfx1, 0, SFX_CRITICAL);
  do {
    MikMod_Update();
    SDL_Delay(10);
  } while (!Voice_Stopped(v1));

  Sample_Free(sfx1);
}

void sound_init() {
  dbglogger_printf("MikMod version %ld.%ld.%ld\n", LIBMIKMOD_VERSION_MAJOR,
                   LIBMIKMOD_VERSION_MINOR, LIBMIKMOD_REVISION);

  MikMod_InitThreads();

  MikMod_RegisterAllDrivers();
  MikMod_RegisterAllLoaders();

  // md_mode |= DMODE_SOFT_MUSIC | DMODE_STEREO | DMODE_HQMIXER | DMODE_16BITS;
  // md_mode |= DMODE_SOFT_SNDFX;

  char s[] = "";
  if (MikMod_Init(s)) {
    dbglogger_printf("Could not initialize sound: %s",
                     MikMod_strerror(MikMod_errno));
    sound_end();
    return;
  }
  /* reserve 2 voices for sound effects */
  MikMod_SetNumVoices(-1, 2);
  /* get ready to play */
  MikMod_EnableOutput();
  debug_audio();
}

void sound_end() {
  MikMod_DisableOutput();
  MikMod_Exit();
}
