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
#include <stdlib.h>
#include <string.h>

/*
	Replace the old way(that was buggy) of rotating by
	a filter. Rotate the image by 90 CCW

	By Raphael Assenat
*/
unsigned int *buffer_rotate = NULL;
void Cleanup (void) {
	if (buffer_rotate != NULL)
	  free (buffer_rotate);
}

/* Maybe we can change this so that it do not use an buffer */
int Apply (int mode, const SDL_v4l_image *SRC, SDL_v4l_image *DST)
{
	static int last_len = 0;
	int x, y;
	int nw;
	int target_pitch;
	unsigned int *src;
	unsigned int *dst;
	if (mode == 0) return 0;

	SDL_v4l_Buffer_Prepare (SRC, DST);

	/* {{{ (re)alloc buffer if need */
	if (last_len < SRC->len)
	  {
		buffer_rotate = (unsigned int*)xRealloc (buffer_rotate, SRC->len);
		if (buffer_rotate==NULL)
		  return 0;
		last_len = SRC->len;
	  }
	/* }}} */
	dst = (unsigned int*)DST->data;
	src = buffer_rotate;

	memcpy(buffer_rotate, SRC->data, SRC->len);
	target_pitch = SRC->h<<2;

	for (y=0; y<SRC->w; y++)
	  for (x=0; x<SRC->h; x++)
	    dst[(SRC->w-y-1)*SRC->h+x]= buffer_rotate[(x*SRC->w)+y];

	/* I use a temporary storage (nw) because the SRC and DST may point at the same address */
	nw = SRC->h;
	DST->h 	   = SRC->w;
	DST->w 	   = nw;
	DST->pitch = target_pitch;

	return 1;
}

static int   status = 0;
       int   Type   	(void ) { return 0; }				// MANDATORY
       int   Toggle 	(void ) { return status = (status + 1) % 2; }	// RECOMENDED ( else always  ON    )
       int   Mode_set  	(int w) { return status = w % 2; }		// RECOMENDED ( else unknown STATE )
       int   Mode_get  	(void ) { return status; }			// RECOMENDED ( else unknown STATE )
const  char *Name	(void ) { return "rotate90"; }			// MANDATORY
const  char *Display	(void ) {                                       // RECOMENDED ( else display filter name )
  return "Rotate90";
}

/*
 * vim600: fdm=marker
 */
