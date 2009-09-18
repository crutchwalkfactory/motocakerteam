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
#include <stdio.h>
#include "SDL.h"
#include "SDL_image.h"

#include "logo.h"

/* Draw an image in the window passed by x, y, w, h */
int logo_show(SDL_Surface *target, unsigned int x, unsigned int y, unsigned int w, unsigned int h, char *path, int blitpos)
{
	static SDL_Surface *logo = NULL;
	SDL_Rect tgt;

	if (blitpos<1 || blitpos>5) return 0;

	if (logo == NULL)
	  if (NULL == (logo = IMG_Load (path))) { 
	    fprintf(stderr, "Warning: Could not load logo image %s\n", path); 
	    return 0;
	    }

	tgt.x = x; tgt.y=y;
	switch(blitpos)
	{
		default: break;
		case 2 : tgt.x += w-logo->w; 						
			 break;
		case 3 : tgt.x += w-logo->w;
		case 4 : tgt.y += h-logo->h;	 	
			 break;
		case 5 : tgt.x += (w>>1)-(logo->w>>1); 	tgt.y += (h>>1)-(logo->h>>1); 	
			 break;
	}
	SDL_BlitSurface(logo, NULL, target, &tgt);

	return 1;
}
