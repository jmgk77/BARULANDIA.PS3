#include "sound.h"

SDL_AudioDeviceID deviceId;

void effect_play(int index) {
  char buf[MAX_STRING];
  snprintf(buf, MAX_STRING, "%sSOUND%d.WAV", DATA_PATH, index);
  dbglogger_printf("PLAYING: %s\n", buf);

  // load wav
  SDL_AudioSpec wave;
  Uint8 *wavBuffer;
  Uint32 wavLength;

  SDL_AudioSpec *t = SDL_LoadWAV(buf, &wave, &wavBuffer, &wavLength);
  // check wave.format becoz PS3 SDL dont report error
  if ((t == NULL) || (!wave.format)) {
    fprintf(stderr, "Could not open %s: %s\n", buf, SDL_GetError());
    return;
  }
  /*debug_audio_spec(&wave);*/

  // stop previous audio
  SDL_ClearQueuedAudio(deviceId);

  // play new audio
  SDL_QueueAudio(deviceId, wavBuffer, wavLength);
  SDL_FreeWAV(wavBuffer);

  SDL_PauseAudioDevice(deviceId, 0);
}

void sound_init() {
  // sdl
  SDL_AudioSpec want, have;
  SDL_zero(want);
  // avoid PS3's default AUDIO_F32MSB
  want.format = AUDIO_S16LSB;
  want.freq = 16000;
  // mono
  want.channels = 1;
  deviceId = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
  if (deviceId == 0) {
    dbglogger_printf("Failed to open audio: %s", SDL_GetError());
  }

  debug_audio_spec(&have);
}

void sound_end() {
  if (deviceId)
    SDL_CloseAudioDevice(deviceId);
}
