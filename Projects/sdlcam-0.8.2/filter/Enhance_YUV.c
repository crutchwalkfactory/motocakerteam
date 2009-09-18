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

/*
	YUV420P Uses 1 color sample for a 2 by 2 square of pixels. That
	is great because it saves 5 bytes, but on the other hand, small
	artefacts will appear when there are sharp color edges that are
	not going vertically or horizontally.

	This filter will remove theese artefacts. Simply run the cam at
	double size, and this filter will reduce the image size by 2,
	thus removing the artefacts as well.

	The 4 Y samples are combined by computing their average, so it
	reduce noise as well.

	TIP: run your cam at a slow frame rate and without compression
	if possible to achieve best results.

	By: Raphael Assenat
*/
int Apply (int mode, const SDL_v4l_image *SRC, SDL_v4l_image *DST)
{
	int x, y;
	unsigned char *src_a = SRC->data;
	unsigned int   skip  = SRC->w << 2;
	unsigned char *src_b = SRC->data + skip;
	unsigned char *dst   = DST->data;

	if (mode == 0) return 0;
	DST->w     = SRC->w     >> 1;
	DST->h     = SRC->h     >> 1;
	DST->len   = SRC->len   >> 2;
	DST->pitch = SRC->pitch >> 1;
	y = DST->h;
	do
	  {
		x = DST->w;
		do
		{
			dst[0] = ( src_a[0] + src_a[4] + src_b[0] + src_b[4] ) >> 2;
			dst[1] = ( src_a[1] + src_a[5] + src_b[1] + src_b[5] ) >> 2;
			dst[2] = ( src_a[2] + src_a[6] + src_b[2] + src_b[6] ) >> 2;
			dst   += 4;
			src_a += 8;
			src_b += 8;
		} while (--x);
	  src_a = src_a + skip;
	  src_b = src_a + skip;
	  } while ( --y );
	return 1;
}

static int   status = 0;
       int   Type   	(void ) { return 0; status = 0;}		// MANDATORY
       int   Toggle 	(void ) { return status = (status + 1) % 2; }	// RECOMENDED ( else always  ON    )
       int   Mode_set  	(int a) { return status = a % 2; }		// RECOMENDED ( else unknown STATE )
       int   Mode_get  	(void ) { return status; }			// RECOMENDED ( else unknown STATE )
const  char *Name	(void ) { return "halfsize"; }			// MANDATORY
const  char *Display	(void ) { return "Halfsize"; }               // RECOMENDED ( else display filter name )

/*
 * vim600: fdm=marker
 */
