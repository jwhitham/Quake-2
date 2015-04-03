/*
** RW_X11.C
**
** This file contains ALL Linux specific stuff having to do with the
** software refresh.  When a port is being made the following functions
** must be implemented by the port:
**
** SWimp_EndFrame
** SWimp_Init
** SWimp_InitGraphics
** SWimp_SetPalette
** SWimp_Shutdown
** SWimp_SwitchFullscreen
**
** void RW_IN_Init(in_state_t *in_state_p)
** void RW_IN_Shutdown(void)
** void RW_IN_Commands (void)
** void RW_IN_Move (usercmd_t *cmd)
** void RW_IN_Frame (void)
** void RW_IN_Activate(void)
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

#include "../ref_soft/r_local.h"
#include "../client/keys.h"
#include "../linux/rw_linux.h"

#define WIDTH 400
#define HEIGHT 300
#define INTERVAL 125
#define H_MAX_FRAMES 110000
/*#define H_MAX_FRAMES 2000*/

extern int curframe;
static FILE * outfile;

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
    if (!curframe) {
        curframe = 1;
        outfile = fopen ("video.bin", "wb");
        if (!outfile) {
    		Sys_Error("video.bin not created\n");
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
    curframe ++;

    if ((curframe % INTERVAL) == (INTERVAL / 2)) {
        fwrite ("F", 1, 1, outfile);
        fwrite (vid.buffer, WIDTH * HEIGHT, 1, outfile);
        printf ("frame %u -> %u bytes\n",
                curframe, (unsigned) ftell (outfile));
    }
    if (curframe >= H_MAX_FRAMES) {
        Sys_Error ("SWimp_EndFrame - quit\n");
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
	vid.rowbytes = WIDTH;
	vid.buffer = malloc(vid.rowbytes * vid.height);
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
    fwrite ("P", 1, 1, outfile);
    fwrite (palette, 4 * 256, 1, outfile);
}

/*
** SWimp_Shutdown
**
** System specific graphics subsystem shutdown routine.  Destroys
** DIBs or DDRAW surfaces as appropriate.
*/
void SWimp_Shutdown( void )
{
    if (outfile) {
        fclose (outfile);
        outfile = NULL;
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

