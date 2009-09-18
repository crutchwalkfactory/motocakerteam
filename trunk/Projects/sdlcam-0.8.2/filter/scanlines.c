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

static int Apply_3_4(int mode, SDL_v4l_image *image);
static int Apply_1_2(int mode, SDL_v4l_image *image);
static int Apply_1_4(int mode, SDL_v4l_image *image);
static int Apply_0(int mode, SDL_v4l_image *image);
static int interlace(int mode, SDL_v4l_image *image);

static int   status = 0;

static char parity=0;

/*
	This filter darkens even scanlines at 75%, 50%, 25% or 0% of their
	original intensity.

	Author: Raphael Assenat

*/
int Apply(int mode, SDL_v4l_image *image)
{
	int len=image->len;
	int i;


	if (mode == 0) return 0;

	if (status==1) { Apply_3_4(mode, image); }
	if (status==2) { Apply_1_2(mode, image); }
	if (status==3) { Apply_1_4(mode, image); }
	if (status==4) { Apply_0(mode, image); }
	if (status==5) { interlace(mode, image); }

	return 0;
}

static int interlace(int mode, SDL_v4l_image *image)
{
	int len=image->len;
	int i, j;
	unsigned char *data=image->data;

	if (mode == 0) return 0;
	parity = !(parity);

	if (parity) { data+=image->pitch; }

	for (i=0; i<((image->h>>1)-parity); i++)
	{
		for (j=0; j<image->pitch; j+=4)
		{
			data[j]=(data[j]+(data[j]>>1))>>1;
			data[j+1]=(data[j+1]+(data[j+1]>>1))>>1;
			data[j+2]=(data[j+2]+(data[j+1]>>1))>>1;
		}
		data+=image->pitch;
		data+=image->pitch;
	}

	return 1;
}

static int Apply_3_4(int mode, SDL_v4l_image *image)
{
	int len=image->len;
	int i, j;
	unsigned char *data=image->data;

	if (mode == 0) return 0;

	for (i=0; i<image->h>>1; i++)
	{
		for (j=0; j<image->pitch; j+=4)
		{
			data[j]=(data[j]+(data[j]>>1))>>1;
			data[j+1]=(data[j+1]+(data[j+1]>>1))>>1;
			data[j+2]=(data[j+2]+(data[j+1]>>1))>>1;
		}
		data+=image->pitch;
		data+=image->pitch;
	}

	return 1;
}

static int Apply_1_2(int mode, SDL_v4l_image *image)
{
	int len=image->len;
	int i, j;
	unsigned char *data=image->data;

	if (mode == 0) return 0;

	for (i=0; i<image->h>>1; i++)
	{
		for (j=0; j<image->pitch; j+=4)
		{
			data[j]=data[j]>>1;
			data[j+1]=data[j+1]>>1;
			data[j+2]=data[j+2]>>1;
		}
		data+=image->pitch;
		data+=image->pitch;
	}

	return 1;
}

static int Apply_1_4(int mode, SDL_v4l_image *image)
{
	int len=image->len;
	int i, j;
	unsigned char *data=image->data;

	if (mode == 0) return 0;

	for (i=0; i<image->h>>1; i++)
	{
		for (j=0; j<image->pitch; j+=4)
		{
			data[j]=data[j]>>2;
			data[j+1]=data[j+1]>>2;
			data[j+2]=data[j+2]>>2;
		}
		data+=image->pitch;
		data+=image->pitch;
	}

	return 1;
}

static int Apply_0(int mode, SDL_v4l_image *image)
{
	int len=image->len;
	int i, j;
	unsigned char *data=image->data;

	if (mode == 0) return 0;

	for (i=0; i<image->h>>1; i++)
	{
		for (j=0; j<image->pitch; j+=4)
		{
			data[j]=0;
			data[j+1]=0;
			data[j+2]=0;
		}
		data+=image->pitch;
		data+=image->pitch;
	}

	return 1;
}

int   Type   	(void ) { return 1; }				// MANDATORY
int   Toggle 	(void ) { return status = (status + 1) % 6; }	// RECOMENDED ( else always  ON    )
int   Mode_set  	(int a) { return status = a % 2; }		// RECOMENDED ( else unknown STATE )
int   Mode_get  	(void ) { return status; }			// RECOMENDED ( else unknown STATE )
const  char *Name	(void ) { return "scanlines"; }			// MANDATORY
const  char *Display	(void )
{
	if (status==0) { return "full scanlines"; }
	if (status==1) { return "75% scanlines"; }
	if (status==2) { return "50% scanlines"; }
	if (status==3) { return "25% scanlines"; }
	if (status==4) { return "0% scanlines"; }
	if (status==5) { return "interlace"; }
}             	// RECOMENDED ( else display filter name )

/*
 * vim600: fdm=marker
 */
