SRCS=barulandia.cpp debug.cpp sdl_help.cpp video.cpp floodfill.cpp screen.cpp

a.out: $(SRCS)
	gcc -Wall -g $(SRCS) `sdl2-config --cflags --libs` -lSDL2 -lSDL2_image -lstdc++ -lSDL2_ttf -lm

clean:
	rm -f a.out
