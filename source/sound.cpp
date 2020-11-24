#include "sound.h"

static Uint8 *audio_chunk, *audio_pos, *wavBuffer;
static Uint32 audio_len;

void fill_audio(void *udata, Uint8 *stream, int len) {
  if (audio_len == 0)
    return;
  len = (len > audio_len ? audio_len : len);
  SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);
  audio_pos += len;
  audio_len -= len;
}

void effect_play(int index) {
  char buf[MAX_STRING];
  snprintf(buf, MAX_STRING, "%sSOUND%d.WAV", DATA_PATH, index);
  dbglogger_printf("PLAYING: %s\n", buf);

  // pause and free
  SDL_PauseAudio(1);
  audio_len = 0;
  SDL_FreeWAV(wavBuffer);

  // load wav
  SDL_AudioSpec wave;

  SDL_AudioSpec *t = SDL_LoadWAV(buf, &wave, &audio_chunk, &audio_len);
  // check wave.format becoz PS3 SDL dont report error
  if ((t == NULL) || (!wave.format)) {
    fprintf(stderr, "Could not open %s: %s\n", buf, SDL_GetError());
    return;
  }
  debug_audio_spec(&wave);

  // play new audio
  audio_pos = audio_chunk;

  SDL_PauseAudio(0);
}

void sound_init() {
  SDL_AudioSpec wanted;

  /* Set the audio format */
  wanted.freq = 16000;
  wanted.format = AUDIO_S16LSB;
  wanted.channels = 1;   /* 1 = mono, 2 = stereo */
  wanted.samples = 1024; /* Good low-latency value for callback */
  wanted.callback = fill_audio;
  wanted.userdata = NULL;

  /* Open the audio device, forcing the desired format */
  if (SDL_OpenAudio(&wanted, NULL) < 0) {
    fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
  }
  debug_audio_spec(&wanted);
  return;
}

void sound_end() { SDL_CloseAudio(); }
