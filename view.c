
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <SDL/SDL.h>

#define WIDTH       400
#define HEIGHT      300
#define FRAME_SIZE  ((WIDTH * HEIGHT) + (256 * 4))
#define NULL_COLOUR 0xff

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
    uint8_t             top_buffer[WIDTH * HEIGHT];
    uint8_t             bottom_buffer[WIDTH * HEIGHT];
    uint8_t             diff_buffer[WIDTH * HEIGHT];
    uint8_t             palette[256 * 4];
    uint8_t             old_palette[256 * 4];
    FILE *              fd_top;
    FILE *              fd_bottom;
    int                 running = 1;
    int                 waiting = 1;
    int                 playing = 0;
    int                 searching = 0;
    off_t               pos = 0;
    off_t               limit = 0;
    unsigned            i, count;

    memset (old_palette, 0, sizeof (old_palette));
    if (argc != 3) {
        fputs ("usage: view <frames.ref.dat> <frames.dat>\n", stderr);
        return 1;
    }

    fd_top = fopen(argv[1], "rb");
    assert(fd_top);

    fd_bottom = fopen(argv[2], "rb");
    assert(fd_bottom);

    fseek (fd_top, 0, SEEK_END);
    fseek (fd_bottom, 0, SEEK_END);
    limit = ftell (fd_top);
    if (ftell (fd_bottom) > limit) {
        limit = ftell (fd_bottom);
    }

    fprintf (stderr, "top frame:    %s\n"
                     "bottom frame: %s\n"
                     "press p to play\n"
                     "press s to search for a difference\n"
                     "press left/right arrows to single step\n"
                     "press q to quit\n",
                     argv[1], argv[2]);

    // ready to draw
    if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        fputs ("SDL_Init failed\n", stderr);
        return 1;
    }
    window = SDL_SetVideoMode (WIDTH, HEIGHT * 3, 8, 0);
    if (!window) {
        fputs ("SDL_SetVideoMode failed\n", stderr);
        SDL_Quit ();
        return 1;
    }
    SDL_AddTimer (40, tick, NULL);

    while (running) {
        // winding 
        if (pos > limit) {
            pos = limit;
            playing = 0;
            searching = 0;
        }
        if (pos < 0) {
            pos = 0;
        }
        fseek (fd_top, pos, SEEK_SET);
        fseek (fd_bottom, pos, SEEK_SET);

        // read top data
        if ((fread (&palette, sizeof (palette), 1, fd_top) == 1)
        && (fread (&top_buffer, sizeof (top_buffer), 1, fd_top) == 1)) {
            // update palette if it changed
            if (memcmp (old_palette, palette, sizeof(palette)) != 0) {
                for (i = 0; i < 256; i++) {
                    colour[i].r = palette[(i * 4) + 0];
                    colour[i].g = palette[(i * 4) + 1];
                    colour[i].b = palette[(i * 4) + 2];
                }
                SDL_SetColors (window, colour, 0, 256);
            }
        } else {
            memset (top_buffer, NULL_COLOUR, sizeof (top_buffer));
            playing = 0;
            searching = 0;
        }

        // read bottom data
        if ((fread (&palette, sizeof (palette), 1, fd_bottom) == 1)
        && (fread (&bottom_buffer, sizeof (bottom_buffer), 1, fd_bottom) == 1)) {
        } else {
            memset (bottom_buffer, NULL_COLOUR, sizeof (bottom_buffer));
            playing = 0;
            searching = 0;
        }

        memset (diff_buffer, 0, sizeof (diff_buffer));
        count = 0;
        for (i = 0; i < sizeof (diff_buffer); i++) {
            if (top_buffer[i] != bottom_buffer[i]) {
                diff_buffer[i] = NULL_COLOUR;
                searching = 0;
                count ++;
            }
        }
        printf ("%08x of %08x - %u different\n",
            (unsigned) pos, (unsigned) limit, (unsigned) count);
        fflush (stdout);

        // blit 
        SDL_LockSurface (window);
        memcpy (&((unsigned char *) window->pixels)[WIDTH * HEIGHT * 0],
                top_buffer, WIDTH * HEIGHT);
        memcpy (&((unsigned char *) window->pixels)[WIDTH * HEIGHT * 1],
                bottom_buffer, WIDTH * HEIGHT);
        memcpy (&((unsigned char *) window->pixels)[WIDTH * HEIGHT * 2],
                diff_buffer, WIDTH * HEIGHT);
        SDL_UnlockSurface (window);
        SDL_Flip (window);

        // events
        waiting = 1;
        while (waiting) {
            if (searching) {
                if (!SDL_PollEvent (&ev)) {
                    waiting = 0;
                    break;
                }
            } else {
                SDL_WaitEvent(&ev);
            }
            switch (ev.type) {
                case SDL_QUIT:
                    waiting = 0;
                    running = 0;
                    break;
                case SDL_KEYDOWN :
                    playing = 0;
                    switch (ev.key.keysym.sym) {
                        case SDLK_LEFT:
                            pos -= FRAME_SIZE;
                            waiting = 0;
                            break;
                        case SDLK_RIGHT:
                            pos += FRAME_SIZE;
                            waiting = 0;
                            break;
                        case SDLK_p:
                            playing = 1;
                            searching = 0;
                            break;
                        case SDLK_s:
                            playing = 0;
                            searching = 1;
                            break;
                        case SDLK_ESCAPE:
                        case SDLK_q:
                            waiting = 0;
                            running = 0;
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_USEREVENT:
                    if (playing) {
                        waiting = 0;
                    }
                    break;
                default:
                    break;
            }
        }
        if (playing || searching) {
            pos += FRAME_SIZE;
        }
    }
    SDL_Quit () ;
    return 0;
}

