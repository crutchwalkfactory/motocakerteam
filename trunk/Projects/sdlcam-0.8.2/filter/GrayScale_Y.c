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
#include <stdio.h>

/* int GrayScale_Y            (const SDL_v4l_image *SRC, SDL_v4l_image *DST) OK
 *
 * Gray scale the picture (model YUV)
 *
 * Input:
 * SDL_v4l_image *     Image
 * char                if it's the start
 *
 * Output:
 * int                 0/1 success
 *
 * Modify:
 * param[0]            New Image
 */
int GrayScale_Y (const SDL_v4l_image *SRC, SDL_v4l_image *DST) {{{
  unsigned int gray;
  unsigned long len = SRC->len >> 2;
  unsigned char *src=SRC->data;
  unsigned char *dst=DST->data;
  if (SRC->len&3) 
    {
      printf("Gray need len problem\n");
      printf("Gray len = %i\n", SRC->len);
      return 0;
    }
  SDL_v4l_Buffer_Prepare (SRC, DST);
  while(len--)
    {
      gray = 77 * (unsigned int)src[0] + 151 * (unsigned int)src[1] + 28 * (unsigned int)src[2];
      gray >>= 8;
      dst[0] = dst[1] = dst[2] = (unsigned char)gray;
      src += 4;
      dst += 4;
    }
  return 0;
}}}

/* int GrayScale_Average      (const SDL_v4l_image *SRC, SDL_v4l_image *DST) OK
 *
 * Gray scale the picture (model RBG average)
 *
 * Input:
 * SDL_v4l_image *     Image
 * char                if it's the start
 *
 * Output:
 * int                 0/1 success
 *
 * Modify:
 * param[0]            New Image
 */
int GrayScale_Average(const SDL_v4l_image *SRC, SDL_v4l_image *DST) {{{
  int gray;
  unsigned char *dst=DST->data;
  unsigned char *src=SRC->data;
  unsigned int len = SRC->len >> 2;
  SDL_v4l_Buffer_Prepare (SRC, DST);
  if (SRC->len&3) 
    {
      printf("Gray need len problem\n");
      printf("Gray len = %i\n", SRC->len);
      return 0;
    }
  while(len--)
    {
      gray = src[0]+src[1]+src[2];
      dst[0]=dst[1]=dst[2]=gray/3;
      src += 4;
      dst += 4;
    } 
  return 1;
}}}   

static int   status = 0;
       int   Type   	(void ) { return 0; }				// MANDATORY
       int   Toggle 	(void ) { return status = (status + 1) % 3; }	// RECOMENDED ( else always  ON    )
       int   Mode_set  	(int w) { return status = w % 3; }		// RECOMENDED ( else unknown STATE )
       int   Mode_get  	(void ) { return status; }			// RECOMENDED ( else unknown STATE )
const  char *Name	(void ) { return "gray"; }			// MANDATORY
const  char *Display	(void ) {                                        // RECOMENDED ( else display filter name )
	switch(status) 
	  {
	    default: return "Color";
	    case 1 : return "Gray-YUV";
	    case 2 : return "Gray-Avg";
          }
	return "Gray-BAD";
}
int Apply (int mode, const SDL_v4l_image *SRC, SDL_v4l_image *DST) {              // MANDATORY
  switch(mode)
    {
      default: return 1;
      case 1 : return GrayScale_Y 	(SRC, DST);
      case 2 : return GrayScale_Average (SRC, DST);
    }
  return -666;
}

/*
 * vim600: fdm=marker
 */
