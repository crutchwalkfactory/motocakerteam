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
#include <stdio.h>
SDL_v4l_image BUF_noise;

void Cleanup (void) {
  if (BUF_noise.data != NULL)
    free (BUF_noise.data);
}
int Apply(int mode, const SDL_v4l_image *SRC, SDL_v4l_image *DST) {{{
	unsigned int len = SRC->len;
	unsigned char *src = SRC->data;
	unsigned char *buf;
	if (mode == 0) return 0;
	SDL_v4l_Buffer_Prepare (SRC, DST);
	if (SDL_v4l_Buffer_Prepare (SRC, &BUF_noise))
	  memset (BUF_noise.data, 0, BUF_noise.len);
	buf = BUF_noise.data;
	while(len--) 
	  {
	  *buf = ((int)(*buf) + (int)(*buf) + (int)(*buf) + (int)(*src) ) >> 2;
	  buf++;
	  src++;
	  }
	memcpy (DST->data, BUF_noise.data, DST->len);
	return 1;
}}}

static int   status = 0;
       int   Type   	(void ) { return 0; status = 0;}		// MANDATORY
       int   Toggle 	(void ) { return status = (status + 1) % 2; }	// RECOMENDED ( else always  ON    )
       int   Mode_set  	(int a) { return status = a % 2; }		// RECOMENDED ( else unknown STATE )
       int   Mode_get  	(void ) { return status; }			// RECOMENDED ( else unknown STATE )
const  char *Name	(void ) { return "noise"; }			// MANDATORY
const  char *Display	(void ) { return "Noise"; } 			// RECOMENDED ( else display filter name )

/*
 * vim600: fdm=marker
 */
