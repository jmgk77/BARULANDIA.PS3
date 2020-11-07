#pragma once

#include "barulandia.h"

// sounds 1 to 10 correspond to drawings
#define SOUND_LOGO 11
#define SOUND_CLICK 12
#define SOUND_TRASH 13
#define SOUND_PING 14

// ffmpeg -i in.wav -acodec pcm_s16le -ar 16000 -ac 1 out.wav

void effect_play(int index);
void sound_end();
void sound_init();
