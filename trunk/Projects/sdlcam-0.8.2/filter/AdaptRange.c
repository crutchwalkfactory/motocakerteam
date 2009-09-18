/*
	SDLcam:  A program to view and apply effects in real-time to video
	Copyright (C) 2002-2007, Raphael Assenat
	 
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
 * Strech the contrast (try to use compleate color space)
 *
 * Input:
 * SDL_v4l_image *		Source/Dest
 *
 * Output:
 * int					0/0 succes
 *
 * Modify:
 * param[0]
 */
#define RANGE_IGNORE	300
int Apply(int mode, const SDL_v4l_image *SRC, SDL_v4l_image *DST)
{
	/* Find the lower and higher pixel value, and scale theses value to max and min */
	unsigned char *src = SRC->data;
	unsigned char *Src;
	unsigned char *dst = DST->data;
	unsigned int len;
	unsigned char min_r, max_r;
	unsigned char min_g, max_g;
	unsigned char min_b, max_b;
	unsigned char tmp;
	int diff_r, diff_g, diff_b;
	int mindiff = 8;

	int y,x,i,X,Y;
	int avg_width=17; // must be odd
	int xs=(avg_width-1)/2;
	int ys = xs;
	
	unsigned char vals_r[avg_width*avg_width];
	unsigned char vals_g[avg_width*avg_width];
	unsigned char vals_b[avg_width*avg_width];
	
	SDL_v4l_Buffer_Prepare (SRC, DST);

	
	for (y=ys; y<SRC->h - ys ; y++)
	{
		for (x=xs; x<SRC->w - xs; x++)
		{
		
			/* Calculate the surrounding average for this location
			 * note: Very unoptimised */
			i = 0;
			for (Y=-ys; Y<=ys; Y++) {
				for (X=-xs; X<=xs; X++) {
					vals_r[i] = src[(Y+y) * SRC->pitch + (X+x) * 4];
					vals_g[i] = src[(Y+y) * SRC->pitch + (X+x) * 4 + 1];
					vals_b[i] = src[(Y+y) * SRC->pitch + (X+x) * 4 + 2];
					i++;
				}
			}
			min_r = min_g = min_b = 0xff;
			max_r = max_g = max_b = 0;
			for (i=0; i<avg_width*avg_width; i++) {
				if (vals_r[i] > max_r)	max_r = vals_r[i];
				if (vals_r[i] < min_r)	min_r = vals_r[i];
				if (vals_g[i] > max_g)	max_g = vals_g[i];
				if (vals_g[i] < min_g)	min_g = vals_g[i];
				if (vals_b[i] > max_b)	max_b = vals_b[i];
				if (vals_b[i] < min_b)	min_b = vals_b[i];
			}

			diff_r = max_r - min_r;
			if (diff_r < mindiff) diff_r = mindiff;
			diff_g = max_g - min_g;
			if (diff_g < mindiff) diff_g = mindiff;
			diff_b = max_b - min_b;
			if (diff_b < mindiff) diff_b = mindiff;
		
			dst = DST->data + y * DST->pitch + x * 4;
			Src = SRC->data + y * SRC->pitch + x * 4;
			
		//	dst[0] = diff_r;
		//	dst[1] = diff_g;
		//	dst[2] = diff_b;
			
			dst[0] = (Src[0] - min_r ) * 256 / diff_r; 
			dst[1] = (Src[1] - min_g ) * 256 / diff_g;
			dst[2] = (Src[2] - min_b ) * 256 / diff_b;
			
		}		
	}
	return 1;
}
/* }}} */

static int   status = 0;
       int   Type   	(void ) { return 0; }				// MANDATORY
       int   Toggle 	(void ) { return status = (status + 1) % 2; }	// RECOMENDED ( else always  ON    )
       int   Mode_set  	(int a) { return status = a % 2; }		// RECOMENDED ( else unknown STATE )
       int   Mode_get  	(void ) { return status; }			// RECOMENDED ( else unknown STATE )
const  char *Name	(void ) { return "adaptrange"; }			// MANDATORY
const  char *Display	(void ) { return "AdaptRange"; }             	// RECOMENDED ( else display filter name )

/*
 * vim600: fdm=marker
 */
