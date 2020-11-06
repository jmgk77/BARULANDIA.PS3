TARGET  = a.out
INCLUDE = ../include
SRCS    = $(wildcard *.cpp)
FLAGS   = -Wall -O3 `sdl2-config --cflags --libs` `libmikmod-config --cflags --libs` -lSDL2_image -lSDL2_ttf -lstdc++

$(TARGET): $(SRCS)
	gcc $(SRCS) -I$(INCLUDE) $(FLAGS)

clean:
	rm -f $(TARGET)
