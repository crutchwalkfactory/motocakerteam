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
	int i;
	unsigned int lower_r=255, higher_r=0,range_r;
	unsigned int lower_g=255, higher_g=0,range_g;
	unsigned int lower_b=255, higher_b=0,range_b;
	unsigned char *src = SRC->data;
	unsigned char *dst;
	unsigned int range_sum;
	unsigned int summe;
	unsigned int len;
	if (mode == 0) return 0;
	SDL_v4l_Buffer_Prepare (SRC, DST);
	Get_Level (SRC);
	/* Get the Minimum for R G B */
	for(summe=0,lower_r =0;lower_r  < 255 && summe < RANGE_IGNORE ;lower_r ++) summe+=Level_R[lower_r ];
	for(summe=0,lower_g =0;lower_g  < 255 && summe < RANGE_IGNORE ;lower_g ++) summe+=Level_G[lower_g ];
	for(summe=0,lower_b =0;lower_b  < 255 && summe < RANGE_IGNORE ;lower_b ++) summe+=Level_B[lower_b ];
	/* Get the Maximum for R G B but not lower than Minimum */
	for(summe=0,higher_r=255;higher_r > lower_r && summe < RANGE_IGNORE ;higher_r--) summe+=Level_R[higher_r];
	for(summe=0,higher_g=255;higher_g > lower_g && summe < RANGE_IGNORE ;higher_g--) summe+=Level_G[higher_g];
	for(summe=0,higher_b=255;higher_b > lower_b && summe < RANGE_IGNORE ;higher_b--) summe+=Level_B[higher_b];
	/* Since higher_r >= lower_r minimum is 0 */
	range_r = higher_r - lower_r;
	range_g = higher_g - lower_g;
	range_b = higher_b - lower_b;
	Level_med = Level_med - (lower_r+lower_g+lower_b)/3;

	/* only range correction if range > 0 , else only shifting */
	if (range_r) for(i=0;i<256;i++) pallete_r[i]=(i<=lower_r)?0:((i>=higher_r)?255:((i-lower_r)<<8)/range_r);
	else 	     for(i=0;i<256;i++)	pallete_r[i]=(i<=lower_r)?0:((i>=higher_r)?255: (i-lower_r));
	if (range_g) for(i=0;i<256;i++) pallete_g[i]=(i<=lower_g)?0:((i>=higher_g)?255:((i-lower_g)<<8)/range_g);
	else 	     for(i=0;i<256;i++)	pallete_g[i]=(i<=lower_g)?0:((i>=higher_g)?255: (i-lower_g));
	if (range_b) for(i=0;i<256;i++) pallete_b[i]=(i<=lower_b)?0:((i>=higher_b)?255:((i-lower_b)<<8)/range_b);
	else	     for(i=0;i<256;i++) pallete_b[i]=(i<=lower_b)?0:((i>=higher_b)?255: (i-lower_b));
/*	
	range_sum = range_r + range_g + range_b;
	// correct peak with the range
	if (range_sum>0)
	  Level_med = (Level_med * 256 *3) / range_sum;
	// Only Gamma correction if Peack not already at 127
	if(Level_med!=127)
	  {
		double my_gamma = (double)Level_med/128.0;
		printf("My gamma %f\n",my_gamma);
		for(i=0;i<256;i++)
		  {
		    pallete_r[i]=255*pow((double)pallete_r[i]/255.0,my_gamma);
		    pallete_g[i]=255*pow((double)pallete_g[i]/255.0,my_gamma);
		    pallete_b[i]=255*pow((double)pallete_b[i]/255.0,my_gamma);
		  }	
	  }
*/	  
	/* now apply the stuff */
	len = DST->len >> 2;
	dst = DST->data;
	while (len--)
	  {
		dst[0] = pallete_r[src[0]];
		dst[1] = pallete_g[src[1]];
		dst[2] = pallete_b[src[2]];
		src += 4;
		dst += 4;
	  }
	return 1;
}
/* }}} */

static int   status = 0;
       int   Type   	(void ) { return 0; }				// MANDATORY
       int   Toggle 	(void ) { return status = (status + 1) % 2; }	// RECOMENDED ( else always  ON    )
       int   Mode_set  	(int a) { return status = a % 2; }		// RECOMENDED ( else unknown STATE )
       int   Mode_get  	(void ) { return status; }			// RECOMENDED ( else unknown STATE )
const  char *Name	(void ) { return "range"; }			// MANDATORY
const  char *Display	(void ) { return "Range"; }             	// RECOMENDED ( else display filter name )

/*
 * vim600: fdm=marker
 */
