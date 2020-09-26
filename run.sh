#!/bin/sh
gcc source/barulandia.cpp source/sdl_help.cpp -lSDL -o barulandia.ps3.linux.elf
./barulandia.ps3.linux.elf
