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
#include <string.h>

int Apply (int mode, const SDL_v4l_image *SRC, SDL_v4l_image *DST) {
  unsigned int x;
  unsigned int y;
  unsigned int *src = (unsigned int *) SRC->data;
  unsigned int *dst;
  unsigned char *dst_c;
  // Become double size image buffer
  SDL_v4l_image TMP;
  if (mode == 0) return 0;
  TMP.w     = SRC->w     * 2 - 1;
  TMP.h     = SRC->h     * 2 - 1;
  TMP.pitch = TMP.w * 4;
  TMP.len   = TMP.h * TMP.pitch;
  TMP.data	= (unsigned char*) xMalloc (TMP.len);
  dst = (unsigned int *) TMP.data;
  for (y=0; y < SRC->h ; y++)
    {{{
	  for (x=0; x<SRC->w - 1; x++)
	    {
	      *(dst++) = *(src++);
		  dst ++;
	    }
      *(dst++) = *(src++);
	  dst = dst + TMP.w;
    }}}
  
  dst_c = TMP.data;
  for (y=0; y < TMP.len ; y+=TMP.pitch*2)
    for (x=4; x < TMP.pitch ; x+=8)
	  {{{
		dst_c[y+x+0] = (dst_c[y+x-4+0] + dst_c[y+x+4+0]) >> 1;
		dst_c[y+x+1] = (dst_c[y+x-4+1] + dst_c[y+x+4+1]) >> 1;
		dst_c[y+x+2] = (dst_c[y+x-4+2] + dst_c[y+x+4+2]) >> 1;
	  }}}
  for (y=TMP.pitch; y < TMP.len ; y+=TMP.pitch*2)
    for (x=0; x < TMP.pitch ; x+=8)
	  {{{
		dst_c[y+x+0] = (dst_c[y+x-TMP.pitch+0] + dst_c[y+x+TMP.pitch+0]) >> 1;
		dst_c[y+x+1] = (dst_c[y+x-TMP.pitch+1] + dst_c[y+x+TMP.pitch+1]) >> 1;
		dst_c[y+x+2] = (dst_c[y+x-TMP.pitch+2] + dst_c[y+x+TMP.pitch+2]) >> 1;
	  }}}
  for (y=TMP.pitch; y < TMP.len ; y+=TMP.pitch*2)
    for (x=4; x < TMP.pitch ; x+=8)
	  {{{
		dst_c[y+x+0] = (dst_c[y+x-TMP.pitch+0-4] + dst_c[y+x+TMP.pitch+0-4] + dst_c[y+x-TMP.pitch+0+4] + dst_c[y+x+TMP.pitch+0-4]) >> 2;
		dst_c[y+x+1] = (dst_c[y+x-TMP.pitch+1-4] + dst_c[y+x+TMP.pitch+1-4] + dst_c[y+x-TMP.pitch+1+4] + dst_c[y+x+TMP.pitch+1-4]) >> 2;
		dst_c[y+x+2] = (dst_c[y+x-TMP.pitch+2-4] + dst_c[y+x+TMP.pitch+2-4] + dst_c[y+x-TMP.pitch+2+4] + dst_c[y+x+TMP.pitch+2-4]) >> 2;
	  }}}
  if (DST->data!=NULL)
	free(DST->data);
  memcpy(DST,&TMP,sizeof(TMP));
  return 1;
}

static int   status = 0;
       int   Type   	(void ) { return 0; }		// MANDATORY
       int   Toggle 	(void ) { return status = (status + 1) % 2; }	// RECOMENDED ( else only toggle )
       int   Mode_set  	(int a) { return status = a % 2; }		// RECOMENDED ( else unknown STATE )
       int   Mode_get  	(void ) { return status; }			// RECOMENDED ( else unknown STATE )
const  char *Name	(void ) { return "doublesize"; }		// MANDATORY
const  char *Display	(void ) {return "Doublesize"; }			// RECOMENDED ( else display filter name )

/*
 * vim600: fdm=marker
 */
