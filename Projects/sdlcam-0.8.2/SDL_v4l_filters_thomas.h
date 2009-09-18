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
#ifndef FILTER_THOMAS_H
#define FILTER_THOMAS_H
#include <sys/types.h>
#ifdef __cplusplus
extern "C" {
#endif


// cpy64_MMX /* memcpy is faster */
int MMX_trashmask	(const SDL_v4l_image *src , SDL_v4l_image *dst, unsigned int trash);
int MMX_bluebox		(SDL_v4l_image *buf , const SDL_v4l_image *back, unsigned int trash);	/* already in c */
#ifdef MMX
int MMX_add_sub_shr	(unsigned char *bild, unsigned char *buf, unsigned short *summe, unsigned int len, unsigned int shr);
#endif
void yuv420p_to_rgb32(size_t width,size_t height,unsigned char *y,unsigned char *u,unsigned char *v,unsigned char *d);
void yuv420p_to_bgr32(size_t width,size_t height,unsigned char *y,unsigned char *u,unsigned char *v,unsigned char *d);
void SDL_v4l_Reorder_cleanup (void);
int SDL_v4l_Reorder     (const SDL_v4l_image *SRC, SDL_v4l_image *DST,int off_x,int off_y,int typ,int radius,double effekt);
#ifdef __cplusplus
	}
#endif


#endif
