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
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>

#include "SDL.h"
#include "misc.h"
#include "dyn_misc.h"

#include "SDL_v4l.h"
#include "SDL_v4l_filters.h"
extern "C" {
#include "SDL_v4l_filters_thomas.h"
#include "configuration.h"
	}
#include "webcam.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* Video for linux and philips webcam includes */
#include <linux/videodev.h>
#include <sys/ioctl.h>
#include "pwc-ioctl.h"

#include "SDL_v4l_filters_thomas.h"

int v4l_max = 0;
int v4l_akt = 0;
cInput **v4l = NULL;

int            cV4L::Palette_ini (void) {{{
	int ret;
	/* Set the capture palette */
	ioctl(fd, VIDIOCGPICT, &picture);
	picture.palette 	= VIDEO_PALETTE_YUV420P;
	picture.depth 		= 12;
	ret = ioctl(fd, VIDIOCSPICT, &picture);
	return ret;
	}}}

int            cV4L::Window_get (void) {{{
	if (fd < 0) return 0;
	return 0 <= ioctl(fd, VIDIOCGWIN, &window); 
	}}}
int            cV4L::Window_set (void) {{{
	if (fd < 0) return 0;
	return 0 <= ioctl(fd, VIDIOCSWIN, &window); 
	}}}
int            cV4L::Resolution (int *w, int *h ) {{{
	if (cur_w == 0 || cur_h == 0) {
		Window_get ();
		cur_w = window.width;
		cur_h = window.height;
		}
	*w = cur_w;
	*h = cur_h;
	return 1;
	}}}
int            cV4L::Resolution (int  w, int  h ) {{{
	int ret;
// printf ("v1 (%i)\n",fd);
	if (fd < 0) return 0;
	if (! Window_get ()) return 0;
	/* Set the resolution */
	window.width =w;
	window.height=h;
	ret = Window_set ();
	if (Window_get()) {{{
		cur_w = window.width;
		cur_h = window.height;
		}}}
	else {{{
		printf("Grab size changeg but can't get new one\n");
		exit(-1);
		}}}
	return ret;
	}}}
int            cV4L::Channel (void ) {{{
	if (fd < 0) return 0;
	if (ioctl(fd, VIDIOCGCHAN, &channel) < 0) return -1;
	return channel.channel;
	}}}
const char *   cV4L::Channel_sym (void ) {{{
	if (fd < 0) return 0;
	if (ioctl(fd, VIDIOCGCHAN, &channel) < 0) return "";
	return channel.name;
	}}}
cV4L::cV4L   (const char *dev, int mmap_use) {{{
	use_mmap = mmap_use;
	buffer = (unsigned char*)MAP_FAILED;
	fd = open (dev, O_RDONLY);
	image.len  = 0;
	image.data = NULL;
	image.w    = 0;
	image.h    = 0;
	cur_w      = 0;
	cur_h      = 0;
	if (fd < 0) return;
	/* Set the capture size */
	Palette_ini ();
	}}}
cV4L::~cV4L (void ) {{{
	if (fd < 0) return;
	if (image.data != NULL) free (image.data);
	if (buffer != MAP_FAILED) {
		if (use_mmap) 	munmap (buffer, mbuf.size);
		else		free   (buffer);
		}
	close (fd); 
	}}}
const char *   cV4L::Name (void           ) {{{
	if (fd < 0) return "";
	if (ioctl(fd, VIDIOCGCAP, &capability) < 0) return "";
	return capability.name;
	}}}
int            cV4L::getRGB32 (void) {{{
	/* This only works on webcams supporting YUV4:2:0P
	 * I cannot test other palettes since my webcam only supports
	 * YUV4:2:0P. */
	size_t	akt_size;
	int	akt_width;
	int	akt_height;
	akt_width  = image.w;
	akt_height = image.h;
	akt_size   = akt_width * akt_height;

	/* The mmap method */
	if (old_size != akt_size || buffer == MAP_FAILED) {{{
		mbuf.size = akt_size + akt_size / 2;
		if (buffer != MAP_FAILED) {
			if (use_mmap) 	munmap (buffer, akt_size + akt_size/2);
			else		free   (buffer);
			}
		if (use_mmap) {
			if (0>ioctl(fd, VIDIOCGMBUF, &mbuf)) use_mmap=0;
			if (use_mmap) buffer = (unsigned char*)mmap(NULL, mbuf.size, PROT_READ, MAP_SHARED, fd, 0);
			if (buffer == MAP_FAILED) { 
				printf("Could not use mmap (fallback to read)\n");
				use_mmap = 0; 
				}
			}
		if (!use_mmap) {
			mbuf.size = akt_size + akt_size / 2;
			buffer = (unsigned char*)xMalloc (akt_size + akt_size / 2);
			}
		akt_size = akt_size;
	}}}
	/* Get the Image Data */
	if (use_mmap) {{{
		struct video_mmap vm;
		int res;
		vm.frame  = 0;
		vm.format = VIDEO_PALETTE_YUV420P;
		vm.width  = akt_width;
		vm.height = akt_height;
		res = ioctl(fd, VIDIOCMCAPTURE, &vm);
		if (res < 0) {{{
			printf ("VIDIOCMCAPTURE failed\n");
			return 0;
			}}}
		res = 0;
		res = ioctl(fd, VIDIOCSYNC, &res);
		if (res < 0) {{{
			printf ("VIDIOCSYNC failed\n");
			return 0;
			}}}
		}}}
	else	read(fd, buffer, mbuf.size);      // Read  Y(akt_size) U(akt_size/4) V(akt_size/4)
	
	/* Transform them to RGB or BGR */
	if(!g_filter_stats.rgb2bgr) 	yuv420p_to_rgb32(akt_width, akt_height, buffer, buffer + akt_size, buffer + akt_size + akt_size / 4, image.data);
	else 				yuv420p_to_bgr32(akt_width, akt_height, buffer, buffer + akt_size, buffer + akt_size + akt_size / 4, image.data);
	image.inverted = 1;
	return 0;
	}}}
SDL_v4l_image *cV4L::Fetch (void) {{{
  unsigned int new_len;
  int a,b;
  //if (cur_w == 0 || cur_h == 0) Resolution (320, 240);
  Resolution (320,240);
  //Resolution (&a,&b);
  new_len = cur_w * cur_h * 4;
  if (image.len<new_len)
    {
      if (image.data != NULL) {
 	free (image.data);
        }
      image.data = (unsigned char*)xMalloc (new_len);
      image.len = new_len;
    }
  image.w     = cur_w;
  image.h     = cur_h;
  image.pitch = image.w * 4;
  getRGB32 ();
  return &image;
}}}

int cV4L::Parameter (const char *name, int val) {{{
	struct video_picture    v_pic;
	if (fd < 0) return 0;
	if (0 == strcasecmp ("gamma"      , name)) {{{
		ioctl (fd, VIDIOCGPICT, &v_pic);
		v_pic.whiteness = val;
		return ioctl (fd, VIDIOCSPICT, &v_pic);
	}}}
	if (0 == strcasecmp ("colour"     , name)) {{{
		ioctl (fd, VIDIOCGPICT, &v_pic);
		v_pic.colour = val;
                return ioctl (fd, VIDIOCSPICT, &v_pic);
	}}}
	if (0 == strcasecmp ("contrast"   , name)) {{{
		ioctl (fd, VIDIOCGPICT, &v_pic);
		v_pic.contrast = (val>65535)?65535:val;
                return ioctl (fd, VIDIOCSPICT, &v_pic);
	}}}
	if (0 == strcasecmp ("brightness" , name)) {{{
		ioctl (fd, VIDIOCGPICT, &v_pic);
		v_pic.brightness = (val>65535)?65535:val;
                return ioctl (fd, VIDIOCSPICT, &v_pic);
	}}}
	if (0 == strcasecmp ("channel"    , name)) {{{
		channel.channel = val;
		if (ioctl(fd, VIDIOCSCHAN, &channel) < 0) return 0;
		return 1;
		}}}
	return -1000000;
}}}
int cV4L::Parameter (const char *name) {{{
	struct video_picture    v_pic;
	if (0 == strcasecmp ("gamma"	  , name)) { ioctl(fd, VIDIOCGPICT, &v_pic); return v_pic.whiteness;  }
	if (0 == strcasecmp ("colour"	  , name)) { ioctl(fd, VIDIOCGPICT, &v_pic); return v_pic.colour;     }
	if (0 == strcasecmp ("contrast"	  , name)) { ioctl(fd, VIDIOCGPICT, &v_pic); return v_pic.contrast;   }
	if (0 == strcasecmp ("brightness" , name)) { ioctl(fd, VIDIOCGPICT, &v_pic); return v_pic.brightness; }
	return -1000000;
}}}

void  cV4L::Capability_info (void) {{{
	struct video_capability v4l_cap;
	if (ioctl(fd, VIDIOCGCAP, &v4l_cap) < 0) return;
	printf("V4L-Name: %s\n", v4l_cap.name);
	if (v4l_cap.type & VID_TYPE_CAPTURE      ) printf("- CAPTURE\n");
	if (v4l_cap.type & VID_TYPE_TUNER        ) printf("- TUNER\n");
	if (v4l_cap.type & VID_TYPE_TELETEXT     ) printf("- TELETEXT\n");
	if (v4l_cap.type & VID_TYPE_OVERLAY      ) printf("- OVERLAY\n");
	if (v4l_cap.type & VID_TYPE_CHROMAKEY    ) printf("- CHROMAKEY\n");
	if (v4l_cap.type & VID_TYPE_CLIPPING     ) printf("- CLIPPING\n");
	if (v4l_cap.type & VID_TYPE_FRAMERAM     ) printf("- FRAMERAM\n");
	if (v4l_cap.type & VID_TYPE_SCALES       ) printf("- SCALES\n");
	if (v4l_cap.type & VID_TYPE_MONOCHROME   ) printf("- MONOCHROME\n");
	if (v4l_cap.type & VID_TYPE_SUBCAPTURE   ) printf("- SUBCAPTURE\n");
	if (v4l_cap.type & VID_TYPE_MPEG_DECODER ) printf("- \n");
	if (v4l_cap.type & VID_TYPE_MPEG_ENCODER ) printf("- \n");
	if (v4l_cap.type & VID_TYPE_MJPEG_DECODER) printf("- \n");
	if (v4l_cap.type & VID_TYPE_MJPEG_ENCODER) printf("- \n");
	printf("V4L-Channels: %i\n", v4l_cap.channels);
	for (int i=0; i < v4l_cap.channels ;i++) {
		printf("- (%i) %s\n",i,Channel_sym());
		}
	printf ("V4L-Dimension: (%i x %i) - (%i x %i)\n", v4l_cap.minwidth, v4l_cap.minheight, v4l_cap.maxwidth, v4l_cap.maxheight);
	}}} 
/*
 * vim600: fdm=marker
 */
