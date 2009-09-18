/*
	SDLcam:  A program to view and apply effects in real-time to video
	Copyright (C) 2002, Raphael Assenat
	 
	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.
					 
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
								 
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "../SDL_v4l_image.h"
#include "../dyn_misc.h"
#include <stdlib.h>
#include <string.h>
/*
	Dark filter.
	Useful in low light conditions, when AGC is to maximum,
	the range and noise filter are enabled. In theese conditions,
	the CCD imperfections are very apparent, because they are amplificated.

	This filter removes all apparent ccd imperfections. Put your cam somewhere
	very dark, then enable this filter. The filter will put the first image
	it gets in it's buffer, and will substract it from all following images, and
	it will nullify CCD imperfections.

	I recommend to pass the resulting image in range filter on more time after this
	filter.

	Author: Raphael Assenat
*/
SDL_v4l_image BUF_dark;
static int   status = 0;
static char was_off = 1;

void Cleanup (void)
{
	if (BUF_dark.data != NULL)
	free (BUF_dark.data);
}

int Apply(int mode, const SDL_v4l_image *SRC, SDL_v4l_image *DST) {
	unsigned char *src = SRC->data;
	unsigned char *dark;
	unsigned char *dst;
	unsigned int len = SRC->len >> 2;

	if(mode == 0) { was_off=1; return 1; }
	SDL_v4l_Buffer_Prepare (SRC, DST);

	if ( SDL_v4l_Buffer_Prepare (SRC, &BUF_dark) || was_off)
	{
		memcpy (BUF_dark.data, SRC->data, BUF_dark.len);
		was_off = 0;
		return 0;
	}
	dark = BUF_dark.data;
	src  = SRC->data;
	dst  = DST->data;

	while (len--)
	{
		dst[0] = (src[0] > dark[0]) ? (src[0] - dark[0]) : 0;
		dst[1] = (src[1] > dark[1]) ? (src[1] - dark[1]) : 0;
		dst[2] = (src[2] > dark[2]) ? (src[2] - dark[2]) : 0;
		dst +=4;
		src +=4;
		dark+=4;
	}
	Call_Filter_bb ("range", 1 , DST, DST);
	return 1;
}

int Type(void ) // MANDATORY
{
	status = 0;
	return 0;
}
int Toggle(void ) // RECOMENDED ( else always  ON    )
{
	status = (status + 1) % 2;
	if (status) { was_off=1; } // if it is on after being toggled, then it was off before!
	return status;
}

int Mode_set(int a) { return status = a % 2; }		// RECOMENDED ( else unknown STATE )
int Mode_get(void ) { return status; }			// RECOMENDED ( else unknown STATE )

const char *Name	(void ) { return "dark"; }			// MANDATORY
const char *Display	(void ) { return "Dark"; }                  // RECOMENDED ( else display filter name )

/*
 * vim600: fdm=marker
 */
