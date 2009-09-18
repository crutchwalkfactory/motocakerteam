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

int Apply (int mode, const SDL_v4l_image *src,SDL_v4l_image *dst) {{{
	/* This is an Blur effekt */
	int r, c;
	int width = src->w;
	unsigned char *imageIn = src->data;
	unsigned char *imageOut = NULL;
	if (mode == 0) return 0;
	if(src->data == dst->data)
	  imageOut = (unsigned char *)xMalloc (src->len * sizeof (unsigned char));
	else imageOut = dst->data;

	if (! imageOut)
	  return (0);
	for (r = width; (r+2*width)*4 < (src->len) ; r+= width) 
	  for (c = 1; c + 1 < width; c++) 
	    {
	      int temp[4];
	      temp[0] =	+ imageIn[(r-width + c-1)*4+0] + imageIn[(r-width + c)*4+0] + imageIn[(r-width+c+1)*4+0]
			     	+ imageIn[(r       + c-1)*4+0]							    + imageIn[(r      +c+1)*4+0]
					+ imageIn[(r+width + c-1)*4+0] + imageIn[(r+width + c)*4+0] + imageIn[(r+width+c+1)*4+0];
	      temp[1] =	+ imageIn[(r-width + c-1)*4+1] + imageIn[(r-width + c)*4+1] + imageIn[(r-width+c+1)*4+1]
		    	   	+ imageIn[(r       + c-1)*4+1]							    + imageIn[(r      +c+1)*4+1]
					+ imageIn[(r+width + c-1)*4+1] + imageIn[(r+width + c)*4+1] + imageIn[(r+width+c+1)*4+1];
	      temp[2] = + imageIn[(r-width + c-1)*4+2] + imageIn[(r-width + c)*4+2] + imageIn[(r-width+c+1)*4+2]
		  	     	+ imageIn[(r       + c-1)*4+2]			   		      	    + imageIn[(r      +c+1)*4+2]
					+ imageIn[(r+width + c-1)*4+2] + imageIn[(r+width + c)*4+2] + imageIn[(r+width+c+1)*4+2];
	      imageOut[(c + r)*4+0] = ( temp[0] + imageIn[(r + c)*4+0] ) / 9;
	      imageOut[(c + r)*4+1] = ( temp[1] + imageIn[(r + c)*4+1] ) / 9;
	      imageOut[(c + r)*4+2] = ( temp[2] + imageIn[(r + c)*4+2] ) / 9;
			}
	if(src->data == dst->data)
	  {
	    memcpy (dst->data, imageOut, src->len);
  	    free (imageOut);
	  }
	return (1);
	}}}

static int   status = 0;

       int   Type   	(void ) { return 0; status = 0;}		// MANDATORY
       int   Toggle 	(void ) { return status = (status + 1) % 2; }	// RECOMENDED ( else always  ON    )
       int   Mode_set  	(int a) { return status = a % 2; }		// RECOMENDED ( else unknown STATE )
       int   Mode_get  	(void ) { return status; }			// RECOMENDED ( else unknown STATE )
const  char *Name	(void ) { return "lowpass"; }			// MANDATORY
const  char *Display	(void ) {return "Lowpass"; }                  // RECOMENDED ( else display filter name )

/*
 * vim600: fdm=marker
 */
