/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

// draw.c

#include "r_local.h"


image_t		*draw_chars;				// 8*8 graphic characters

//=============================================================================

/*
================
Draw_FindPic
================
*/
image_t *Draw_FindPic (char *name)
{
	image_t	*image;
	char	fullname[MAX_QPATH];

	if (name[0] != '/' && name[0] != '\\')
	{
		Com_sprintf (fullname, sizeof(fullname), "pics/%s.pcx", name);
		image = R_FindImage (fullname, it_pic);
	}
	else
		image = R_FindImage (name+1, it_pic);

	return image;
}



/*
===============
Draw_InitLocal
===============
*/
void Draw_InitLocal (void)
{
	draw_chars = Draw_FindPic ("conchars");
}



/*
================
Draw_Char

Draws one 8*8 graphics character
It can be clipped to the top of the screen to allow the console to be
smoothly scrolled off.
================
*/
void Draw_Char (int x, int y, int num)
{
	pixel_t			*dest;
	pixel_t			*source;
	int				drawline;	
	int				row, col;

	num &= 255;

	if (num == 32 || num == 32+128)
		return;

	if (y <= -8)
		return;			// totally off screen

//	if ( ( y + 8 ) >= vid.height )
	if ( ( y + 8 ) > vid.height )		// PGM - status text was missing in sw...
		return;

#ifdef PARANOID
	if (y > vid.height - 8 || x < 0 || x > vid.width - 8)
		ri.Sys_Error (ERR_FATAL,"Con_DrawCharacter: (%i, %i)", x, y);
	if (num < 0 || num > 255)
		ri.Sys_Error (ERR_FATAL,"Con_DrawCharacter: char %i", num);
#endif

	row = num>>4;
	col = num&15;
	source = draw_chars->pixels[0] + (row<<10) + (col<<3);

	if (y < 0)
	{	// clipped
		drawline = 8 + y;
		source -= 128*y;
		y = 0;
	}
	else
		drawline = 8;


	dest = vid.buffer + y*vid.rowpixels + x;

	while (drawline--)
	{
		if (source[0].c != TRANSPARENT_COLOR)
			dest[0] = source[0];
		if (source[1].c != TRANSPARENT_COLOR)
			dest[1] = source[1];
		if (source[2].c != TRANSPARENT_COLOR)
			dest[2] = source[2];
		if (source[3].c != TRANSPARENT_COLOR)
			dest[3] = source[3];
		if (source[4].c != TRANSPARENT_COLOR)
			dest[4] = source[4];
		if (source[5].c != TRANSPARENT_COLOR)
			dest[5] = source[5];
		if (source[6].c != TRANSPARENT_COLOR)
			dest[6] = source[6];
		if (source[7].c != TRANSPARENT_COLOR)
			dest[7] = source[7];
		source += 128;
		dest += vid.rowpixels;
	}
}

/*
=============
Draw_GetPicSize
=============
*/
void Draw_GetPicSize (int *w, int *h, char *pic)
{
	image_t *gl;

	gl = Draw_FindPic (pic);
	if (!gl)
	{
		*w = *h = -1;
		return;
	}
	*w = gl->width;
	*h = gl->height;
}

/*
=============
Draw_StretchPicImplementation
=============
*/
void Draw_StretchPicImplementation (int x, int y, int w, int h, image_t	*pic)
{
	pixel_t			*dest, *source;
	int				v, u, sv;
	int				height;
	int				f, fstep;
	int				skip;

	if ((x < 0) ||
		(x + w > vid.width) ||
		(y + h > vid.height))
	{
		ri.Sys_Error (ERR_FATAL,"Draw_Pic: bad coordinates");
	}

	height = h;
	if (y < 0)
	{
		skip = -y;
		height += y;
		y = 0;
	}
	else
		skip = 0;

	dest = vid.buffer + y * vid.rowpixels + x;

	for (v=0 ; v<height ; v++, dest += vid.rowpixels)
	{
		sv = (skip + v)*pic->height/h;
		source = pic->pixels[0] + sv*pic->width;
		if (w == pic->width)
			memcpy (dest, source, w * sizeof(pixel_t));
		else
		{
			f = 0;
			fstep = pic->width*0x10000/w;
			for (u=0 ; u<w ; u+=4)
			{
				dest[u] = source[f>>16];
				f += fstep;
				dest[u+1] = source[f>>16];
				f += fstep;
				dest[u+2] = source[f>>16];
				f += fstep;
				dest[u+3] = source[f>>16];
				f += fstep;
			}
		}
	}
}

/*
=============
Draw_StretchPic
=============
*/
void Draw_StretchPic (int x, int y, int w, int h, char *name)
{
	image_t	*pic;

	pic = Draw_FindPic (name);
	if (!pic)
	{
		ri.Con_Printf (PRINT_ALL, "Can't find pic: %s\n", name);
		return;
	}
	Draw_StretchPicImplementation (x, y, w, h, pic);
}

/*
=============
Draw_StretchRaw
=============
*/
void Draw_StretchRaw (int x, int y, int w, int h, int cols, int rows, byte *data)
{
	image_t	pic;

	pic.pixels[0] = (pixel_t *) data; // XXX TODO BAD CAST
	pic.width = cols;
	pic.height = rows;
	Draw_StretchPicImplementation (x, y, w, h, &pic);
}

/*
=============
Draw_Pic
=============
*/
void Draw_Pic (int x, int y, char *name)
{
	image_t			*pic;
	pixel_t			*dest, *source;
	int				v, u;
	pixel_t			tpixel;
	int				height;

	pic = Draw_FindPic (name);
	if (!pic)
	{
		ri.Con_Printf (PRINT_ALL, "Can't find pic: %s\n", name);
		return;
	}

	if ((x < 0) ||
		(x + pic->width > vid.width) ||
		(y + pic->height > vid.height))
		return;	//	ri.Sys_Error (ERR_FATAL,"Draw_Pic: bad coordinates");

	height = pic->height;
	source = pic->pixels[0];
	if (y < 0)
	{
		height += y;
		source += pic->width*-y;
		y = 0;
	}

	dest = vid.buffer + y * vid.rowpixels + x;

	if (!pic->transparent)
	{
		for (v=0 ; v<height ; v++)
		{
			memcpy (dest, source, pic->width * sizeof (pixel_t));
			dest += vid.rowpixels;
			source += pic->width;
		}
	}
	else
	{
		if (pic->width & 7)
		{	// general
			for (v=0 ; v<height ; v++)
			{
				for (u=0 ; u<pic->width ; u++)
					if ( (tpixel=source[u]).c != TRANSPARENT_COLOR)
						dest[u] = tpixel;

				dest += vid.rowpixels;
				source += pic->width;
			}
		}
		else
		{	// unwound
			for (v=0 ; v<height ; v++)
			{
				for (u=0 ; u<pic->width ; u+=8)
				{
					if ( (tpixel=source[u]).c != TRANSPARENT_COLOR)
						dest[u] = tpixel;
					if ( (tpixel=source[u+1]).c != TRANSPARENT_COLOR)
						dest[u+1] = tpixel;
					if ( (tpixel=source[u+2]).c != TRANSPARENT_COLOR)
						dest[u+2] = tpixel;
					if ( (tpixel=source[u+3]).c != TRANSPARENT_COLOR)
						dest[u+3] = tpixel;
					if ( (tpixel=source[u+4]).c != TRANSPARENT_COLOR)
						dest[u+4] = tpixel;
					if ( (tpixel=source[u+5]).c != TRANSPARENT_COLOR)
						dest[u+5] = tpixel;
					if ( (tpixel=source[u+6]).c != TRANSPARENT_COLOR)
						dest[u+6] = tpixel;
					if ( (tpixel=source[u+7]).c != TRANSPARENT_COLOR)
						dest[u+7] = tpixel;
				}
				dest += vid.rowpixels;
				source += pic->width;
			}
		}
	}
}

/*
=============
Draw_TileClear

This repeats a 64*64 tile graphic to fill the screen around a sized down
refresh window.
=============
*/
void Draw_TileClear (int x, int y, int w, int h, char *name)
{
	int			i, j;
	pixel_t		*psrc;
	pixel_t		*pdest;
	image_t		*pic;
	int			x2;

	if (x < 0)
	{
		w += x;
		x = 0;
	}
	if (y < 0)
	{
		h += y;
		y = 0;
	}
	if (x + w > vid.width)
		w = vid.width - x;
	if (y + h > vid.height)
		h = vid.height - y;
	if (w <= 0 || h <= 0)
		return;

	pic = Draw_FindPic (name);
	if (!pic)
	{
		ri.Con_Printf (PRINT_ALL, "Can't find pic: %s\n", name);
		return;
	}
	x2 = x + w;
	pdest = vid.buffer + y*vid.rowpixels;
	for (i=0 ; i<h ; i++, pdest += vid.rowpixels)
	{
		psrc = pic->pixels[0] + pic->width * ((i+y)&63);
		for (j=x ; j<x2 ; j++)
			pdest[j] = psrc[j&63];
	}
}


/*
=============
Draw_Fill

Fills a box of pixels with a single color
=============
*/
void Draw_Fill (int x, int y, int w, int h, int c)
{
	pixel_t			*dest;
	int				u, v;

	if (x+w > vid.width)
		w = vid.width - x;
	if (y+h > vid.height)
		h = vid.height - y;
	if (x < 0)
	{
		w += x;
		x = 0;
	}
	if (y < 0)
	{
		h += y;
		y = 0;
	}
	if (w < 0 || h < 0)
		return;
	dest = vid.buffer + y*vid.rowpixels + x;
	for (v=0 ; v<h ; v++, dest += vid.rowpixels)
		for (u=0 ; u<w ; u++)
			dest[u].c = c;
}
//=============================================================================

/*
================
Draw_FadeScreen

================
*/
void Draw_FadeScreen (void)
{
	int			x,y;
	pixel_t		*pbuf;
	int	t;

	for (y=0 ; y<vid.height ; y++)
	{
		pbuf = (vid.buffer + vid.rowpixels*y);
		t = (y & 1) << 1;

		for (x=0 ; x<vid.width ; x++)
		{
			if ((x & 3) != t)
				pbuf[x].c = 0;
		}
	}
}
