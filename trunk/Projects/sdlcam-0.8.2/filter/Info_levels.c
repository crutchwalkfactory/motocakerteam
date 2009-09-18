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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
/*
	This function analyse the input image, and output a
	graphic showing saturation levels statistics for
	each channel.

	TODO: Use lines insteads on points.			(done Thomas Lußnig)
	TODO: Use a logarithmic scale instead of a linear one.	(done Thomas Lußnig -> mode param should become configurable)

	Author: Raphael Assenat
*/
static unsigned int level_info_h = 256;
int Info_levels(const SDL_v4l_image *SRC, SDL_v4l_image *DST) {{{
	unsigned int i;
	unsigned int new_len = 256 * level_info_h * 4;
	unsigned int *dst;
	unsigned int max = 0;
	int mode = 1;
	int lr,lg,lb,la;
	Get_Level (SRC);
	/* Set the new Display size (i hope it work) */
    	if (DST->len == new_len || DST->data == NULL)
		DST->data = xRealloc (DST->data, new_len);
    	DST->len   = new_len;
    	DST->w     = 256;
    	DST->h     = level_info_h;
    	DST->pitch = 256 * 4;
	for (i=0; i<256; i++) {{{				// Calc the Maximum
		if (Level_R[i] > max) max = Level_R[i];
		if (Level_G[i] > max) max = Level_G[i];
		if (Level_B[i] > max) max = Level_B[i];
//		if (Level_A[i] > max) max = Level_A[i];		// <- should never happen (A = (R+G+B)/3 <= max(R,G,B)
		}}}
	if (mode == 2) {{{					// Make the logarithm !!!!
		float b = level_info_h / log (max);
		for (i=0; i<256; i++) {
			Level_R[i] = b * log(Level_R[i]);
			Level_G[i] = b * log(Level_G[i]);
			Level_B[i] = b * log(Level_B[i]);
			Level_A[i] = b * log(Level_A[i]);
			}	
		max = level_info_h;
		}}}
	if (mode == 0) {{{					/* clip to AVG */
		for (i=0; i<256; i++) {
			Level_R[i] = (Level_R[i] > Level_avg) ? Level_avg : Level_R[i];
			Level_G[i] = (Level_G[i] > Level_avg) ? Level_avg : Level_G[i];
			Level_B[i] = (Level_B[i] > Level_avg) ? Level_avg : Level_B[i];
			Level_A[i] = (Level_A[i] > Level_avg) ? Level_avg : Level_A[i];
			}
		max = Level_avg;				// Only draw to the average
		}}}
	/* Draw the graphic */
    	dst = (unsigned int*)DST->data;
	memset(dst, 0, DST->len);
	dst = dst +255;
	i = 255;
	LineRGB (DST, 0, Level_avg, 255, Level_avg, 100, 100, 100);
	lr = Level_R[0] * level_info_h / max;
	lg = Level_G[0] * level_info_h / max;
	lb = Level_B[0] * level_info_h / max;
	la = Level_A[0] * level_info_h / max;
	for (i=1; i<256; i++) {{{ // Draw the Lines
	    /* Clip the values higher than average */
	    int r = Level_R[i] * level_info_h / max;
	    int g = Level_G[i] * level_info_h / max;
	    int b = Level_B[i] * level_info_h / max;
	    int a = Level_A[i] * level_info_h / max;
	    /* Draw the Lines */
	    LineRGB (DST, i-1, level_info_h-lr, i, level_info_h-r, 255,   0,   0);
	    LineRGB (DST, i-1, level_info_h-lg, i, level_info_h-g,   0, 255,   0);
	    LineRGB (DST, i-1, level_info_h-lb, i, level_info_h-b,   0,   0, 255);
	    LineRGB (DST, i-1, level_info_h-la, i, level_info_h-a, 255, 255, 255);
	    lr = r;
	    lg = g;
	    lb = b;
	    la = a;
	    }}}
	return 0;
	}}}

static int   status = 0;
       int   Type   	(void ) { return 0; }				// MANDATORY
       int   Toggle 	(void ) { return status = (status + 1) % 2; }	// RECOMENDED ( else always  ON    )
       int   Mode_get  	(void ) { return status; }			// RECOMENDED ( else unknown STATE )
       int   Mode_set  	(int a) { return status = a % 2; }		// RECOMENDED ( else unknown STATE )
const  char *Name	(void ) { return "infolevels"; }		// MANDATORY
const  char *Display	(void ) {return "Level"; } 	                // RECOMENDED ( else display filter name )

int Apply (int mode, const SDL_v4l_image *SRC, SDL_v4l_image *DST) {{{             // MANDATORY
  switch(mode) 
    {
      default: return 1;
      case 1 : return Info_levels (SRC, DST);
    }
  return -666;
}}}

/*
 * vim600: fdm=marker
 */
