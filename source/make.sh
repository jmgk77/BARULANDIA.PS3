#!/bin/sh
rm a.out
#gcc -O2 -Wall -g \
gcc -Wall -g \
barulandia.cpp \
debug.cpp \
sdl_help.cpp \
video.cpp \
floodfill.cpp \
 -lSDL -lSDL_image -lstdc++