/*
    C-Dogs SDL
    A port of the legendary (and fun) action/arcade cdogs.
    Copyright (C) 1995 Ronny Wester
    Copyright (C) 2003 Jeremy Chin 
    Copyright (C) 2003 Lucas Martin-King 

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

-------------------------------------------------------------------------------

 grafx.c - graphics related functions 

*/

#include <stdio.h>
#include <stdlib.h>
// #include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include "SDL.h"

// #include <conio.h>
// #include <i86.h>
#include "defs.h"
#include "grafx.h"
#include "sprcomp.h"
#include "files.h"

SDL_Surface *screen;

//static int oldMode;

int InitVideo(int mode)
{	
/*     int i; */
	char title[32];

	switch (mode) {
		case VID_FULLSCREEN: printf("-- Fullscreen Video.\n"); break;
		case VID_WIN_NORMAL: printf("-- Windowed Video. There may be some slowdown.\n"); break;
		case VID_WIN_SCALE:  printf("-- Scaled Windowed Video. There may be major slowdown.\n"); break;
	}
	
	if (mode == VID_FULLSCREEN) {
		screen = SDL_SetVideoMode(320, 240, 8,
				SDL_HWPALETTE | SDL_SWSURFACE |
				     SDL_FULLSCREEN);
		SDL_ShowCursor(SDL_DISABLE);
	} else if (mode == VID_WIN_NORMAL) {
		screen = SDL_SetVideoMode(320, 240, 8,
				SDL_HWPALETTE | SDL_SWSURFACE);
		SDL_ShowCursor(SDL_DISABLE);
	} else if (mode == VID_WIN_SCALE) {
		screen = SDL_SetVideoMode(640, 480, 8,
				SDL_HWPALETTE | SDL_SWSURFACE);
	}

	if (screen == NULL) {
		printf("ERROR: InitVideo: %s\n", SDL_GetError() );
		return -1;
	}
	
 	sprintf(title, "C-Dogs %s [Port %s]", CDOGS_VERSION, CDOGS_SDL_VERSION); 
 	SDL_WM_SetCaption(title, NULL); 
 	SDL_WM_SetIcon(SDL_LoadBMP(GetDataFilePath("cdogs_icon.bmp")), NULL); 
	
	return 0;
}

int ReadPics(const char *filename, void **pics, int maxPics,
	     color * palette)
{
	int f;
	int eof = 0;
	unsigned short int size;
	int i = 0;

	f = open(filename, O_RDONLY);
	if (f >= 0) {
		if (palette)
			read(f, palette, sizeof(TPalette));
		else
			lseek(f, sizeof(TPalette), SEEK_CUR);
		while (!eof && i < maxPics) {
			read(f, &size, sizeof(size));
			if (size) {
				pics[i] = malloc(size);
				if (read(f, pics[i], size) == 0)
					eof = 1;
			} else
				pics[i] = NULL;
			i++;
		}
		close(f);
	}
	return i;
}

int AppendPics(const char *filename, void **pics, int startIndex,
	       int maxPics)
{
	int f;
	int eof = 0;
	unsigned short int size;
	int i = startIndex;

	f = open(filename, O_RDONLY);
	if (f >= 0) {
		lseek(f, sizeof(TPalette), SEEK_CUR);
		while (!eof && i < maxPics) {
			read(f, &size, sizeof(size));
			if (size) {
				pics[i] = malloc(size);
				if (read(f, pics[i], size) == 0)
					eof = 1;
			} else
				pics[i] = NULL;
			i++;
		}
		close(f);
	}
	return i - startIndex;
}

int CompilePics(int picCount, void **pics, void **compiledPics)
{
	int i, size, total = 0;
	int skipped = 0;

	for (i = 0; i < picCount; i++) {
		if (pics[i]) {
			size = compileSprite(pics[i], NULL);
			total += size;
			if (size) {
				compiledPics[i] = malloc(size);
				compileSprite(pics[i], compiledPics[i]);
			} else {
				compiledPics[i] = NULL;
				skipped++;
			}
		} else
			compiledPics[i] = NULL;
	}
	if (skipped)
		printf("%d solid pics not compiled\n", skipped);
	return total;
}

int RLEncodePics(int picCount, void **pics, void **rlePics)
{
	int i, size, total = 0;
	int skipped = 0;

	for (i = 0; i < picCount; i++) {
		if (pics[i]) {
			size = RLEncodeSprite(pics[i], NULL);
			total += size;
			if (size) {
				rlePics[i] = malloc(size);
				RLEncodeSprite(pics[i], rlePics[i]);
			} else {
				rlePics[i] = NULL;
				skipped++;
			}
		} else
			rlePics[i] = NULL;
	}
	if (skipped)
		printf("%d solid pics not RLE'd\n", skipped);
	return total;
}

void vsync(void)
{
//      while ((inp(0x03da) & 8) != 0);
//      while ((inp(0x03da) & 8) == 0);
	return;
}

int PicWidth(void *pic)
{
    if (!pic)
	return 0;

    return ((short*)pic)[0];
}

int PicHeight(void *pic)
{

    if (!pic)
	return 0;

    return ((short*)pic)[1];
}

void SetColorZero(int r, int g, int b)
{
	SDL_Color col;
	col.r = r;
	col.g = g;
	col.b = b;
	SDL_SetPalette(screen, SDL_PHYSPAL, &col, 0, 1);
	return;
}
