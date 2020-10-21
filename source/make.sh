#!/bin/sh
rm a.out
gcc -O2 -Wall \
barulandia.cpp \
debug.cpp \
sdl_help.cpp \
video.cpp \
 -lSDL -lSDL_image