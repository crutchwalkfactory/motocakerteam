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
#ifndef _init_h
#define _init_h
#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_NLS
#	define _(string) gettext (string)
#	ifdef HAVE_LIBINTL_H
#		include <libintl.h>
#	endif /* HAVE_LIBINTL_H */
#else  /* not HAVE_NLS */
#	define _(string) string
#endif /* not HAVE_NLS */

extern int sdl_reinit;
int initialise_sdl      (void);
int initialise_sdl_video(int w ,int h);
int toggle_fullscreen   (void);
int toggle_mini_mode    (void);
char get_mini_mode      (void);
int close_sdl           (void);
#ifdef __cplusplus
	}
#endif


#endif

