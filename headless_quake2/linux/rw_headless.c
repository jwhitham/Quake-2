/*
** Headless mode output driver
*/

#include <ctype.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

#include "../ref_soft/r_local.h"
#include "../client/keys.h"
#include "../linux/rw_linux.h"
#include "../linux/crc.h"

#define WIDTH 400
#define HEIGHT 300

static int curframe;
static int endframe;
static int initialised;
static FILE * crc_file;
static FILE * frames_file;
static FILE * ref_file;
static unsigned char frames_palette[4 * 256];
static struct timeval start;
static struct timeval stop;

void Sys_Quit (void);

void RW_IN_Init(in_state_t *in_state_p)
{
}

void RW_IN_Shutdown(void)
{
}

/*
===========
IN_Commands
===========
*/
void RW_IN_Commands (void)
{
}

/*
===========
IN_Move
===========
*/
void RW_IN_Move (usercmd_t *cmd)
{
}

void RW_IN_Frame (void)
{
}

void RW_IN_Activate(void)
{
}

/*****************************************************************************/

/*
** SWimp_Init
**
** This routine is responsible for initializing the implementation
** specific stuff in a software rendering subsystem.
*/
int SWimp_Init( void *hInstance, void *wndProc )
{
    char * s;

    if (!initialised) {
        initialised = 1;
        gettimeofday (&start, NULL);
        curframe++;
        s = getenv ("BENCHMARK");
        if (s && atoi (s)) {
            /* enable benchmark */
        } else {
            /* test mode */
            crc_file = fopen ("crc.dat", "wt");
            if (!crc_file) {
                Sys_Error("crc.dat not created\n");
            }
            s = getenv ("FRAMEDATA");
            if (s && strlen (s)) {
                frames_file = fopen (s, "wb");
                if (!frames_file) {
                    Sys_Error("frames.dat not created\n");
                }
            }
            s = getenv ("REFDATA");
            if (s && strlen (s)) {
                ref_file = fopen (s, "rb");
                if (!ref_file) {
                    Sys_Error("ref data cannot open\n");
                }
            }
        }
        endframe = 100;
        s = getenv ("ENDFRAME");
        if (s && strlen (s)) {
            endframe = atoi (s);
        }
    }
    return true;
}


/*
** SWimp_EndFrame
**
** This does an implementation specific copy from the backbuffer to the
** front buffer.  In the Win32 case it uses BitBlt or BltFast depending
** on whether we're using DIB sections/GDI or DDRAW.
*/
void SWimp_EndFrame (void)
{
    unsigned crc;

    if (!crc_file) {
        goto fast;
    }

    crc = crc32_8bytes (vid.buffer, WIDTH * HEIGHT * sizeof (pixel_t), 0);

    if (crc_file) {
        fprintf (crc_file, "%08x\n", crc);
    }
    if (frames_file) {
        fwrite (frames_palette, sizeof (frames_palette), 1, frames_file);
        fwrite (vid.buffer, WIDTH * HEIGHT, sizeof (pixel_t), frames_file);
    }
    if ((curframe & 31) == 0) {
        printf ("headless: %u frames\n", curframe);
        fflush (crc_file);
    }
    if (ref_file) {
        pixel_t buf[WIDTH * HEIGHT];
        unsigned i, error, pos;

        fseek (ref_file, sizeof (frames_palette), SEEK_CUR);
        if (fread (buf, WIDTH * HEIGHT * sizeof (pixel_t), 1, ref_file) != 1) {
            Sys_Error ("unable to read from reference file");
        }
        for (error = i = pos = 0; i < (WIDTH * HEIGHT); i++) {
            if (buf[i].c != vid.buffer[i].c) {
                if (!error) {
                    pos = i;
                }
                error ++;
            }
        }
        if (error) {
            Sys_Error ("%u differences detected, curframe = %u, first location = %u",
                       error, curframe, pos);
        }
    }
fast:
    curframe ++;

     
    if (curframe >= endframe) {
        double total;

        gettimeofday (&stop, NULL);

        if (crc_file) {
            fflush (crc_file);
        }
        if (frames_file) {
            fflush (frames_file);
        }
        stop.tv_sec -= start.tv_sec;
        start.tv_sec = 0;
        total = (((double) stop.tv_sec) + (((double) stop.tv_usec) / 1e6)) -
                (((double) start.tv_sec) + (((double) start.tv_usec) / 1e6));
        printf ("\ntotal time: %7.3f seconds, frames %u\n",
                total, curframe);
        fflush (stdout);
        exit (0);
    }
}

/*
** SWimp_SetMode
*/
rserr_t SWimp_SetMode( int *pwidth, int *pheight, int mode, qboolean fullscreen )
{
    rserr_t retval = rserr_ok;

    vid.width = WIDTH;
    vid.height = HEIGHT;
    vid.rowpixels = WIDTH;
    vid.buffer = malloc(vid.rowpixels * vid.height * sizeof (pixel_t));
    ri.Vid_NewWindow (vid.width, vid.height);
    R_GammaCorrectAndSetPalette( ( const unsigned char * ) d_8to24table );

    return retval;
}

/*
** SWimp_SetPalette
**
** System specific palette setting routine.  A NULL palette means
** to use the existing palette.  The palette is expected to be in
** a padded 4-byte xRGB format.
*/
void SWimp_SetPalette( const unsigned char *palette )
{
    memcpy (frames_palette, palette, sizeof (frames_palette));
}

/*
** SWimp_Shutdown
**
** System specific graphics subsystem shutdown routine.  Destroys
** DIBs or DDRAW surfaces as appropriate.
*/
void SWimp_Shutdown( void )
{
    if (crc_file) {
        fclose (crc_file);
        crc_file = NULL;
    }
    if (frames_file) {
        fclose (frames_file);
        frames_file = NULL;
    }
    if (ref_file) {
        fclose (ref_file);
        ref_file = NULL;
    }
}

/*
** SWimp_AppActivate
*/
void SWimp_AppActivate( qboolean active )
{
}

/*
================
Sys_MakeCodeWriteable
================
*/
void Sys_MakeCodeWriteable (unsigned long startaddr, unsigned long length)
{
}


/*****************************************************************************/
/* KEYBOARD                                                                  */
/*****************************************************************************/

Key_Event_fp_t Key_Event_fp;

void KBD_Init(Key_Event_fp_t fp)
{
    Key_Event_fp = fp;
}

void KBD_Update(void)
{
// get events from x server
}

void KBD_Close(void)
{
}

