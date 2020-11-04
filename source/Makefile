TARGET  = a.out
INCLUDE = ../include
SRCS    = $(wildcard *.cpp)
FLAGS   = -Wall -O3 `sdl2-config --cflags --libs` -lSDL2 -lSDL2_image -lstdc++ -lSDL2_ttf -lm

$(TARGET): $(SRCS)
	gcc $(SRCS) -I$(INCLUDE) $(FLAGS)

clean:
	rm -f $(TARGET)
