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

/*
	Author: Thomas Luﬂnig <thomas.kussnig@bewegungsmelder.de>
	This filter double the image size.
	Use Noise or avarage for bether details.
*/

#include "../SDL_v4l_image.h"
#include "../dyn_misc.h"
#include <stdlib.h>

int Apply (int mode, SDL_v4l_image *SRC, SDL_v4l_image *DST) {{{	// emms
	/* TWO Problems need to be fixed here:
	   - currently only work smoth on W % 2 = 0 (w = 8 Byte
	     can be solved by an "smarter" "edx" value, 
	     but than we become an "off by one"
	     this can be fixed if we use "floor(dest_w/2)=ecx"
	     that mean reload "ecx" after each width loop
	   - then this is fixed then W>4
	   */
	unsigned char *src = SRC->data;
	unsigned char *dst = DST->data;
	unsigned int   dst_w = SRC->w >> 1;
	unsigned int   dst_h = SRC->h >> 1;
	unsigned int   versatz = (SRC->w << 2) + ((SRC->w & 1) << 2);
	if (mode == 0) return 0;
	DST->w     = dst_w;
	DST->h     = dst_h;
	DST->len   = SRC->len   >> 2;
	DST->pitch = SRC->pitch >> 1;
	return 1;
}}}

static int   status = 0;
       int   Type   	(void ) { return 0; status = 0;}		// MANDATORY
       int   Toggle 	(void ) { return status = (status + 1) % 2; }	// RECOMENDED ( else always  ON    )
       int   Mode_set  	(int a) { return status = a % 2; }		// RECOMENDED ( else unknown STATE )
       int   Mode_get  	(void ) { return status; }			// RECOMENDED ( else unknown STATE )
const  char *Name	(void ) { return "mmx_halfsize"; }		// MANDATORY
const  char *Display	(void ) {return "Halfsize"; }               // RECOMENDED ( else display filter name )

/*
 * vim600: fdm=marker
 */
