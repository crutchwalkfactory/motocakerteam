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


/* Functions to initialise SDL, load interface images, icons, fonts etc... */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "SDL.h"

#include "interface.h"
#include "init.h"
// #include "SDL_v4l.h"
#include "configuration.h"

SDL_Surface *screen;
static char full_screen = 0;
static char mini_mode   = 0;
int sdl_reinit = 1;

static void calculate_SDLwindow_geometry(int *W, int *H) {{{
	int image_bottom_y = 0;
	int image_bottom_x = 0;
	int w=0, h=0;
	int max_w = 0;
	int max_h = 0;
	
	/* 1. IMAGE must fit in the display */
	if (IMAGE_X < 0) image_bottom_x = *W; else image_bottom_x = IMAGE_X + *W;
	if (IMAGE_Y < 0) image_bottom_y = *H; else image_bottom_y = IMAGE_Y + *H;
	if (image_bottom_x > max_w) max_w = image_bottom_x;
	if (image_bottom_y > max_h) max_h = image_bottom_y;
	if (mini_mode) {
		/* In Minimode maximum size = Image size (not input_size because of resize filte) */
		*W = max_w;
		*H = max_h;
		return;
		}

	/* 2. Side-Bar must fit in the display */
	if (SIDE_BAR_X < 0) w = image_bottom_x - SIDE_BAR_X; else w = SIDE_BAR_X + SIDE_BAR_W;
	if (SIDE_BAR_Y < 0) h = image_bottom_y - SIDE_BAR_Y; else h = SIDE_BAR_Y + SIDE_BAR_H;
	if (w > max_w) max_w = w;
	if (h > max_h) max_h = h;

	/* 3. Bottom-Bar must fit in the display */
	if (BOTTOM_BAR_X < 0) w = image_bottom_x - BOTTOM_BAR_X; else w = BOTTOM_BAR_X + BOTTOM_BAR_W + SIDE_BAR_W;
	if (BOTTOM_BAR_Y < 0) h = image_bottom_y - BOTTOM_BAR_Y; else h = BOTTOM_BAR_Y + BOTTOM_BAR_H;
	if (w > max_w) max_w = w;
	if (h > max_h) max_h = h;

	/* Return the maximum size used */
	*W = max_w;
	*H = max_h;
	}}}

int initialise_sdl       (void) {{{
	/* setup status (fullscreen and mini_mode) */
	full_screen = configuration.start_in_fullscreen;
	mini_mode   = configuration.start_in_minimode;

	/* Initialise SDL sub systems */
	if (SDL_Init (SDL_INIT_VIDEO)) return -1;
	sdl_reinit = 1;
	return 0;
	}}}
int close_sdl() {{{
	SDL_Quit();
	return 0;
	}}}

int initialise_sdl_video (int w, int h) {{{
	Uint32 flags = SDL_SWSURFACE;

	/* Calculate window geometry */
	calculate_SDLwindow_geometry(&w, &h);

	if (mini_mode  ) flags |= SDL_NOFRAME;
	if (full_screen) flags |= SDL_FULLSCREEN;

	/* Initialise video mode */
	w = 320;
	h = 240;
	screen = SDL_SetVideoMode(w, h, 32, SDL_HWSURFACE|SDL_FULLSCREEN);
	if (screen==NULL) return -1;
	sdl_reinit = 0;
	return 0;
	}}}

int toggle_fullscreen(void) {{{
	full_screen = !(full_screen);
	sdl_reinit = 1;
	return 0;
	}}}
int toggle_mini_mode(void) {{{
	mini_mode = !(mini_mode);
	sdl_reinit = 1;
	return 0;
	}}}
char get_mini_mode(void) { return mini_mode; }

/*
 * vim600: fdm=marker
 */
