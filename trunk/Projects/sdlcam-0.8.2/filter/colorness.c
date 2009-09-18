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

static int Apply_whiteness(int mode, SDL_v4l_image *image);
static int Apply_redness(int mode, SDL_v4l_image *image);
static int Apply_blueness(int mode, SDL_v4l_image *image);
static int Apply_greenness(int mode, SDL_v4l_image *image);

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

	if (status==1) { Apply_whiteness(mode, image); }
	if (status==2) { Apply_blueness(mode, image); }
	if (status==3) { Apply_redness(mode, image); }
	if (status==4) { Apply_greenness(mode, image); }

	return 0;
}

/* note: whiteness should be equivalent to Gray Average, but I
wanted a different effect, so its sensitivity is settable via WHITE_DIV */
#define WHITE_DIV 2.0
static int Apply_whiteness(int mode, SDL_v4l_image *image)
{
	int len=image->len;
	int i;
	unsigned char new_color;
	int tmp_new;

	if (mode == 0) return 0;

	for (i=0; i<len; i+=4)
	{
		tmp_new = (int)255.0-((float)(  (255-image->data[i])+(255-image->data[i+1])+(255-image->data[i+2])  ))/((float)WHITE_DIV);

		new_color = tmp_new<0?0:tmp_new;

		image->data[i]=image->data[i+1]=image->data[i+2]=new_color;
	}
	return 1;
}

static int Apply_blueness(int mode, SDL_v4l_image *image)
{
	int len=image->len;
	int i;
	unsigned char new_color;

	if (mode == 0) return 0;

	for (i=0; i<len; i+=4)
	{
		new_color = image->data[i] - ((image->data[i+1]+image->data[i+2])*image->data[i])/512;

		image->data[i]=image->data[i+1]=image->data[i+2]=new_color;
	}
	return 1;
}

static int Apply_greenness(int mode, SDL_v4l_image *image)
{
	int len=image->len;
	int i;
	unsigned char new_color;

	if (mode == 0) return 0;

	for (i=0; i<len; i+=4)
	{
		new_color = image->data[i+1] - ((image->data[i+2]+image->data[i])*image->data[i+1])/512;

		image->data[i]=image->data[i+1]=image->data[i+2]=new_color;
	}
	return 1;
}


static int Apply_redness(int mode, SDL_v4l_image *image)
{
	int len=image->len;
	int i;
	unsigned char new_color;

	if (mode == 0) return 0;

	for (i=0; i<len; i+=4)
	{
		new_color = image->data[i+2] - ((image->data[i+1]+image->data[i])*image->data[i+2])/512;

		image->data[i]=image->data[i+1]=image->data[i+2]=new_color;
	}
	return 1;
}

int   Type   	(void ) { return 1; }				// MANDATORY
int   Toggle 	(void ) { return status = (status + 1) % 5; }	// RECOMENDED ( else always  ON    )
int   Mode_set  	(int a) { return status = a % 2; }		// RECOMENDED ( else unknown STATE )
int   Mode_get  	(void ) { return status; }			// RECOMENDED ( else unknown STATE )
const  char *Name	(void ) { return "colorness"; }			// MANDATORY
const  char *Display	(void )
{
	if (status==0) { return "colorness"; }
	if (status==1) { return "whiteness"; }
	if (status==2) { return "blueness"; }
	if (status==3) { return "redness"; }
	if (status==4) { return "greenness"; }
}             	// RECOMENDED ( else display filter name )

/*
 * vim600: fdm=marker
 */
