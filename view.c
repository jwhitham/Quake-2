
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <SDL/SDL.h>

#define WIDTH       400
#define HEIGHT      300

static uint32_t tick (uint32_t interval, void * param)
{
    SDL_Event event;
    SDL_UserEvent userevent;

    userevent.type = SDL_USEREVENT;
    userevent.code = 0;
    userevent.data1 = NULL;
    userevent.data2 = NULL;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent (&event);
    return(interval);
}

int main (int argc, char ** argv)
{
    SDL_Surface *       window;
    SDL_Event           ev;
    SDL_Color           colour[256];
    uint8_t             buffer[WIDTH * HEIGHT];
    uint8_t             palette[256 * 4];
    uint8_t             old_palette[256 * 4];
    FILE *              fd;
    int                 running = 1;
    int                 waiting = 1;
    unsigned            i;

    memset (old_palette, 0, sizeof (old_palette));
    if (argc != 2) {
        fputs ("usage: view <frames.dat>", stderr);
        return 1;
    }

    fd = fopen(argv[1], "rb");
    assert(fd);

    // ready to draw
    if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        fputs ("SDL_Init failed", stderr);
        return 1;
    }
    window = SDL_SetVideoMode (WIDTH, HEIGHT, 8, 0);
    if (!window) {
        fputs ("SDL_SetVideoMode failed", stderr);
        SDL_Quit ();
        return 1;
    }
    SDL_AddTimer (40, tick, NULL);

    while (running
    && (fread (&palette, sizeof (palette), 1, fd) == 1)
    && (fread (&buffer, WIDTH * HEIGHT, 1, fd) == 1)) {

        // update palette if it changed
        if (memcmp (old_palette, palette, sizeof(palette)) != 0) {
            for (i = 0; i < 256; i++) {
                colour[i].r = palette[(i * 4) + 0];
                colour[i].g = palette[(i * 4) + 1];
                colour[i].b = palette[(i * 4) + 2];
            }
            SDL_SetColors (window, colour, 0, 256);
        }

        // blit 
        SDL_LockSurface (window);
        memcpy (window->pixels, buffer, WIDTH * HEIGHT);
        SDL_UnlockSurface (window);
        SDL_Flip (window);

        // events
        waiting = 1;
        while (waiting && SDL_WaitEvent(&ev)) {
            switch (ev.type) {
                case SDL_QUIT:
                    waiting = 0;
                    running = 0;
                    break;
                case SDL_USEREVENT:
                    waiting = 0;
                    break;
                default:
                    break;
            }
        }
    }
    SDL_Quit () ;
    return 0;
}

