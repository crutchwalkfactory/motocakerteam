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

/*
	Simple and inefficient dithering technique.
	Average is hard-coded to 127. Play with the AGC or
	brightness control to change the threshold.

	BTW, only the red channel is used, so it may be good to activate one
	of the grayscale filters before this filer.

	Author: Raphael Assenat
*/
int Dither_Average (const SDL_v4l_image *SRC, SDL_v4l_image *DST) {{{
	unsigned int len = SRC->len >> 2;
	unsigned int *src = (unsigned int*)SRC->data;
	unsigned int *dst;
	SDL_v4l_Buffer_Prepare (SRC, DST);
    	dst = (unsigned int*)DST->data;
	while (len--)
	  *(dst++) = ((*(src++)&0xFF)<127) ? 0x00000000 : 0xFFFFFFFF;
	return 1;
	Call_Filter_bb ("gray",  1, DST, DST);
}}}

/*
	Yet another dithering filter, this time using random dithering.
	This one is implemented using rand, and should be slow ;-)

	Author: Raphael Assenat
*/
static int Dither_Random (const SDL_v4l_image *SRC, SDL_v4l_image *DST) {{{
	unsigned int len = SRC->len >> 2;
	unsigned int *src = (unsigned int*)SRC->data;
	unsigned int *dst;
	SDL_v4l_Buffer_Prepare (SRC, DST);
	dst = (unsigned int*)DST->data;

	while(len--)
	{
		/* the number multiplicating rand must be a float */
		if ( ( ( (float)(*(src++)&0xFF)*rand()/RAND_MAX)  )>120 ) {
			*(dst++)=0xFFFFFFFF;
		} else {
			*(dst++)=0x00000000;
		}
	}
	Call_Filter_bb ("gray",  1, DST, DST);
	return 1;
}}}

static int   status = 0;
       int   Type   	(void ) { return 0; }				// MANDATORY
       int   Toggle 	(void ) { return status = (status + 1) % 3; }	// RECOMENDED ( else always  ON    )
       int   Mode_set  	(int a) { return status = a % 3; }		// RECOMENDED ( else unknown STATE )
       int   Mode_get  	(void ) { return status; }			// RECOMENDED ( else unknown STATE )
const  char *Name	(void ) { return "dither"; }			// MANDATORY
const  char *Display	(void ) {                                       // RECOMENDED ( else display filter name )
	switch(status) 
	  {
	    default: return "Dither";
	    case 1 : return "Dither-Rnd";
	    case 2 : return "Dither-Avg";
          }
	return "Dither-BAD";
}
int Apply (int mode, const SDL_v4l_image *SRC, SDL_v4l_image *DST) {              // MANDATORY
  switch(mode) 
    {
      default: return 1;
      case 1 : return Dither_Random  (SRC, DST);
      case 2 : return Dither_Average (SRC, DST);
    }
  return -666;
}

/*
 * vim600: fdm=marker
 */
