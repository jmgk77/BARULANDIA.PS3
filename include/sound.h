#pragma once

#include "barulandia.h"

#define SOUND_LOGO 1

// ffmpeg -i in.wav -acodec pcm_s16le -ar 16000 -ac 1 out.wav

void effect_play(int index);
void sound_end();
void sound_init();
