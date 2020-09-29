#!/bin/sh
rm a.out
gcc -O2 -Wall barulandia.cpp sdl_help.cpp -lSDL -lSDL_image
./a.out
