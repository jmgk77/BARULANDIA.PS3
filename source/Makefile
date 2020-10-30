SRCS=barulandia.cpp debug.cpp sdl_help.cpp video.cpp floodfill.cpp

a.out: $(SRCS)
	gcc -Wall -g $(SRCS) `sdl2-config --cflags --libs` -lSDL2 -lSDL2_image -lstdc++

clean:
	rm -f a.out
