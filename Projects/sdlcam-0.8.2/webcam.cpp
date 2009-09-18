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

/* Functions that controls the webcam, does ioctls, change settings... */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/videodev.h>
#include <sys/ioctl.h>

#include "SDL.h"
#include "SDL_v4l.h"
#include "webcam.h"
#include "configuration.h"

/* Video for linux and philips webcam includes */
#include "pwc-ioctl.h"
/*
int set_webcam_led_cycle(int ms_on, int ms_off) {{{
	struct pwc_leds LED;
	LED.led_on = ms_on;
	LED.led_off = ms_off;
	ioctl(fd, VIDIOCPWCSLED, &LED);
	return 0;
}}}
*/
// static int g_last_white_balance_mode=0;
cWebcam::cWebcam (const char *name,int mmap):cV4L(name, mmap) {{{
/*
	Input_set ("factory"	, -1);	// Restore to default settings
	Input_set ("fps"	, configuration.v4l_fps);
 	Input_set ("white_m"	,      0);
	Input_set ("agc"	,     -1);
	Input_set ("shutter"	,     -1);
	Input_set ("compression",      0);
	Input_set ("gamma"	,  32768);
	Input_set ("colour"	,  65535);
*/
	//ov511
//	Input_set ("brightness", 90);
}}}
int cWebcam::Parameter (const char *name, int val) {{{
	struct video_window  	v_win;
	struct pwc_whitebalance v_awb;
		if (val > 65535) val =  65535;
	else	if (val <-65536) val = -65535;
	if (0 == strcasecmp ("reset"	  , name)) { return ioctl(fd, VIDIOCPWCRUSER  , NULL); }
	if (0 == strcasecmp ("preset"	  , name)) { return ioctl(fd, VIDIOCPWCSUSER  , NULL); }
	if (0 == strcasecmp ("factory"	  , name)) { return ioctl(fd, VIDIOCPWCFACTORY, NULL); }
	if (0 == strcasecmp ("compression", name)) { val = val & 3; return ioctl(fd, VIDIOCPWCSCQUAL, &val); }
	if (0 == strcasecmp ("shutter"	  , name)) { return ioctl(fd, VIDIOCPWCSSHUTTER  ,  val);}
	if (0 == strcasecmp ("backligh"	  , name)) { return ioctl(fd, VIDIOCPWCSBACKLIGHT, &val);}
	if (0 == strcasecmp ("flicker"    , name)) { return ioctl(fd, VIDIOCPWCSFLICKER  , &val);}
	if (0 == strcasecmp ("agc"        , name)) { return ioctl(fd, VIDIOCPWCSAGC      , &val);} // 0-65535
	if (0 == strcasecmp ("contour"    , name)) { return ioctl(fd, VIDIOCPWCSCONTOUR  , &val);}
	if (0 == strcasecmp ("noise"      , name)) { return ioctl(fd, VIDIOCPWCSDYNNOISE , &val);}
	if (0 == strcasecmp ("fps"        , name)) {{{
		val = (val<3)?3:(val>30)?30:val;
		ioctl(fd, VIDIOCGWIN, &v_win);
		v_win.flags = (v_win.flags & (~PWC_FPS_FRMASK)) | (val << PWC_FPS_SHIFT);
		ioctl(fd, VIDIOCSWIN, &v_win);
		ioctl(fd, VIDIOCGWIN, &v_win);
		return (v_win.flags & PWC_FPS_FRMASK) >> PWC_FPS_SHIFT;
	}}}
	if (0 == strcasecmp ("white_m"    , name)) {{{
		if (ioctl (fd, VIDIOCPWCGAWB, &v_awb) < 0) return -1;
		//printf("set white_m: %d\n", val);
		switch (val) {
			case 0 : v_awb.mode = PWC_WB_AUTO; 	break;
			case 1 : v_awb.mode = PWC_WB_MANUAL; 	break;
			case 2 : v_awb.mode = PWC_WB_INDOOR; 	break;
			case 3 : v_awb.mode = PWC_WB_OUTDOOR; 	break;
			case 4 : v_awb.mode = PWC_WB_FL; 	break;
			default: break;
		}
		if (ioctl (fd, VIDIOCPWCSAWB, &v_awb) < 0) return 0;
//		g_last_white_balance_mode = mode;
	}}}
	if (0 == strcasecmp ("white_r"    , name)) {{{
		if (ioctl (fd, VIDIOCPWCGAWB, &v_awb) < 0) return -1;
		v_awb.manual_red = val;
		if (ioctl(fd, VIDIOCPWCSAWB , &v_awb) < 0) return -1;
		return val;
	}}}
	if (0 == strcasecmp ("white_b"    , name)) {{{
		if (ioctl (fd, VIDIOCPWCGAWB, &v_awb) < 0) return -1;
		v_awb.manual_blue = val;
		if (ioctl(fd, VIDIOCPWCSAWB , &v_awb) < 0) return -1;
		return val;
	}}}
	return cV4L::Parameter (name, val);
	}}}

int cWebcam::Parameter (const char *name) {{{
	int val_i;
	struct video_window  	v_win;
	struct pwc_whitebalance v_awb;
	if (0 == strcasecmp ("compression", name)) { ioctl(fd, VIDIOCPWCGCQUAL		, &val_i); return val_i; }
	if (0 == strcasecmp ("backligh"	  , name)) { ioctl(fd, VIDIOCPWCGFLICKER	, &val_i); return val_i; }
	if (0 == strcasecmp ("flicker"	  , name)) { ioctl(fd, VIDIOCPWCGFLICKER	, &val_i); return val_i; }
	if (0 == strcasecmp ("agc"	  , name)) { ioctl(fd, VIDIOCPWCGAGC		, &val_i); return val_i; }
	if (0 == strcasecmp ("contour"	  , name)) { ioctl(fd, VIDIOCPWCGCONTOUR	, &val_i); return val_i; }
	if (0 == strcasecmp ("noise"	  , name)) { ioctl(fd, VIDIOCPWCGDYNNOISE	, &val_i); return val_i; }
	if (0 == strcasecmp ("fps"	  , name)) { ioctl(fd, VIDIOCGWIN		, &v_win); return (v_win.flags & PWC_FPS_FRMASK) >> PWC_FPS_SHIFT; }
	if(0 == strcasecmp ("white_m"	 , name)) {{{
		ioctl(fd, VIDIOCPWCGAWB		, &v_awb);
		//printf("get white_m\n");
		switch (v_awb.mode) {
			case PWC_WB_AUTO	: return  0;
			case PWC_WB_MANUAL	: return  1;
			case PWC_WB_INDOOR	: return  2;
			case PWC_WB_OUTDOOR	: return  3;
			case PWC_WB_FL		: return  4;
			default 		: return -1;
			}
		return -1;
		}}}
	if(0 == strcasecmp ("white_r"	, name)) {{{
		if (ioctl(fd, VIDIOCPWCGAWB, &v_awb)<0) return -1;
		return v_awb.read_red;
		}}}
	if(0 == strcasecmp ("white_b"	, name)) {{{
		if (ioctl(fd, VIDIOCPWCGAWB, &v_awb)<0) return -1;
		return v_awb.read_blue;
		}}}
	return cV4L::Parameter (name);
	}}}

/*
 * vim600: fdm=marker
 */
