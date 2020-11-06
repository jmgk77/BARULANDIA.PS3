#include "sound.h"

#ifndef USE_MIKMOD
SDL_AudioDeviceID deviceId;
#endif

void effect_play(int index) {
  char buf[128];
  snprintf(buf, 128, "%sSOUND%d.WAV", DATA_PATH, index);
  dbglogger_printf("PLAYING: %s\n", buf);
#ifdef USE_MIKMOD
  SAMPLE *wave = Sample_Load(buf);
  if (!wave) {
    dbglogger_printf("Could not load: %s\n", MikMod_strerror(MikMod_errno));
    return;
  }

  int id = Sample_Play(wave, 0, SFX_CRITICAL);
  do {
    MikMod_Update();
    SDL_Delay(10);
  } while (!Voice_Stopped(id));

  Sample_Free(wave);
#else
  SDL_AudioSpec wave;
  Uint8 *wavBuffer;
  Uint32 wavLength;

  if (SDL_LoadWAV(buf, &wave, &wavBuffer, &wavLength) == NULL) {
    fprintf(stderr, "Could not open %s: %s\n", buf, SDL_GetError());
  }
  debug_audio_spec(&wave);
  SDL_QueueAudio(deviceId, wavBuffer, wavLength);
  SDL_FreeWAV(wavBuffer);

  SDL_PauseAudioDevice(deviceId, 0);
#endif
}

void sound_init() {
#ifdef USE_MIKMOD
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
#else
  SDL_AudioSpec want, have;
  SDL_zero(want);

  deviceId =
      SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_ANY_CHANGE);
  if (deviceId == 0) {
    dbglogger_printf("Failed to open audio: %s", SDL_GetError());
  }

  debug_audio_spec(&have);
#endif
}

void sound_end() {
#ifdef USE_MIKMOD
  MikMod_DisableOutput();
  MikMod_Exit();
#else
  if (deviceId)
    SDL_CloseAudioDevice(deviceId);
#endif
}
