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
#ifndef _capture_h
#define _capture_h

#ifdef __cplusplus
extern "C" {
#endif

#include "SDL.h"

int interface_capture(SDL_Surface *screen, int w, int h);
SDL_Surface *get_ImageSurface(SDL_Surface *screen, int w, int h);
#ifdef __cplusplus
	}
#endif


#endif // _capture_h

