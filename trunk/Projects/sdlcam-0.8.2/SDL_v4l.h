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
#ifndef _SDL_v4l_h
#define _SDL_v4l_h

#include "SDL.h"
#include "SDL_v4l_filters.h"
#include <linux/videodev.h>
#include "input.h"

class cV4L : public cInput {
	private:
	int			use_mmap;
	int			size_changed;
	struct video_channel	channel;
	struct video_window	window;
	struct video_capability capability;
	struct video_picture	picture;
	int			cur_w;						// aktual height
	int			cur_h;						// aktual width
	int			palette;
	size_t			old_size;					// for get data
	unsigned char*		buffer;						// for get data
	struct video_mbuf 	mbuf;						// for get data
	SDL_v4l_image 		image;
	int		 getRGB32    (void);
	int     	 Palette_ini (void);
	protected:
	int			fd;
	int		 Window_get (void);
	int		 Window_set (void);
	public:	
			 cV4L 	    (const char *name, int mmap_use = 1);	// init  this Class
	virtual		~cV4L 	    (void);					// close this Class

	void  		 Capability_info (void);
	int		 Channel    (void);					// return aktual Channel number
	const char 	*Channel_sym(void);					// return symbolic Channel name

virtual	bool		 isOK	    (void) { return fd>=0; }
virtual	const char	*Name 	    (void);
virtual	int		 Resolution (int  w, int  h);				// return 1 OK 0 FAILED
virtual	int 		 Resolution (int *w, int *h);				// return 1 OK 0 FAILED
virtual	SDL_v4l_image   *Fetch	    (void);					// fetch an new RGB32 image
virtual	int		 Parameter  (const char *name, int val);		// Set an free parameter
virtual int		 Parameter  (const char *name) 	       ;		// Get an free parameter
	};

extern int v4l_max;
extern int v4l_akt;
extern cInput **v4l;

#endif
