#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <iostream>
#include <map>
#include <io/pad.h>
#include <sysutil/msg.h>
#include <sysutil/sysutil.h>
#include <dbglogger.h>



const int width = 1280, height = 720;

int main(int argc, char **argv) {
    dbglogger_init_str("tcp:192.168.0.11:18194");

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        dbglogger_log("%s\n", SDL_GetError());
    }

    SDL_Surface *screen =  SDL_SetVideoMode(1280, 720, 32 , SDL_SWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
    padInfo padinfo;
    padData paddata;

    dbglogger_log("Hello World with the Console press start to scroll. Press X to quit.\n");


    bool active = true;
    SDL_Event e;

    std::map<int, int> keys;

    while (active) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_KEYDOWN:
                if (e.key.keysym.sym == SDLK_ESCAPE) active = false;
                keys[e.key.keysym.sym] = 1;
                break;
            case SDL_KEYUP:
                keys[e.key.keysym.sym] = 0;
                break;
            case SDL_QUIT:
                active = false;
                break;
            }
        }


        SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 255, 0, 0));
        SDL_Flip(screen);

        ioPadGetInfo (&padinfo);
        for (int i = 0; i < MAX_PADS; i++) {
            if (padinfo.status[i]) {
                ioPadGetData (i, &paddata);
                if (paddata.BTN_START) {
                    dbglogger_log(">>START\n");
                }

                if (paddata.BTN_CROSS) {
                    dbglogger_log(">>X\n");
                    active = false;
                }
            }
        }

    }

    SDL_Quit();

    dbglogger_stop();

    return 0;
}


