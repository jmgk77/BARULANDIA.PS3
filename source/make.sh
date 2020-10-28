#!/bin/sh
rm a.out
#gcc -O2 -Wall -g \
gcc -Wall -g \
barulandia.cpp \
debug.cpp \
sdl_help.cpp \
video.cpp \
floodfill.cpp \
`sdl2-config --cflags --libs` \
 -lSDL2 -lSDL2_image -lstdc++