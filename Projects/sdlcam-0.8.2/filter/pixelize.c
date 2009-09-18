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

static int Apply_pixelize(int size, int mode, SDL_v4l_image *image);

static int   status = 0;

/*
	Shows the whiteness/greenness/blueness/redness of the image.

	Author: Raphael Assenat

*/
int Apply(int mode, SDL_v4l_image *image)
{
	int len=image->len;
	int i;
	unsigned char new_color;

	if (mode == 0) return 0;


	if (status==1) { Apply_pixelize(2, mode, image); }
	if (status==2) { Apply_pixelize(4, mode, image); }
	if (status==3) { Apply_pixelize(8, mode, image); }
	if (status==4) { Apply_pixelize(16, mode, image); }
	if (status==5) { Apply_pixelize(32, mode, image); }
	if (status==6) { Apply_pixelize(64, mode, image); }

	return 0;
}

#define HIGH_QUALITY
static int Apply_pixelize(int size, int mode, SDL_v4l_image *image)
{
	int len=image->len;
	int x, y, i, j;
#ifdef HIGH_QUALITY
	unsigned int new_r=0, new_g=0, new_b=0;
	int count=0;
#endif

	if (mode == 0) return 0;

	for (y=0; y<image->h; y+=size)
	{
		for (x=0; x<image->w; x+=size)
		{
#ifdef HIGH_QUALITY
			new_r = new_g = new_b = 0;
			count = 0;
			for (i=0; i<size; i++)
				for (j=0; j<size; j++)
				{
					if (x+j>=image->w) { continue; }
					if (y+i>=image->h) { continue; }
					new_r += image->data[(y+i)*image->pitch+(x+j)*4];
					new_g += image->data[(y+i)*image->pitch+(x+j)*4+1];
					new_b += image->data[(y+i)*image->pitch+(x+j)*4+2];
					count++;
				}
			new_r /= count;
			new_g /= count;
			new_b /= count;
#endif
			for (i=0; i<size; i++)
			{
				for (j=0; j<size; j++)
				{
					if (x+j>=image->w) { continue; }
					if (y+i>=image->h) { continue; }
#ifndef HIGH_QUALITY
					image->data[(y+i)*image->pitch+(x+j)*4]=image->data[(y)*image->pitch+(x)*4];
					image->data[(y+i)*image->pitch+(x+j)*4+1]=image->data[(y)*image->pitch+(x)*4+1];
					image->data[(y+i)*image->pitch+(x+j)*4+2]=image->data[(y)*image->pitch+(x)*4+2];
#else
					image->data[(y+i)*image->pitch+(x+j)*4]=new_r;
					image->data[(y+i)*image->pitch+(x+j)*4+1]=new_g;
					image->data[(y+i)*image->pitch+(x+j)*4+2]=new_b;
#endif
				}
			}
		}
	}

	return 1;
}



int   Type   	(void ) { return 1; }				// MANDATORY
int   Toggle 	(void ) { return status = (status + 1) % 7; }	// RECOMENDED ( else always  ON    )
int   Mode_set  	(int a) { return status = a % 2; }		// RECOMENDED ( else unknown STATE )
int   Mode_get  	(void ) { return status; }			// RECOMENDED ( else unknown STATE )
const  char *Name	(void ) { return "pixelize"; }			// MANDATORY
const  char *Display	(void )
{
	if (status==0) { return "pixelize"; }
	if (status==1) { return "pixel 2"; }
	if (status==2) { return "pixel 4"; }
	if (status==3) { return "pixel 8"; }
	if (status==4) { return "pixel 16"; }
	if (status==5) { return "pixel 32"; }
	if (status==6) { return "pixel 64"; }
}             	// RECOMENDED ( else display filter name )

/*
 * vim600: fdm=marker
 */
