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
#ifndef _configuration_h
#define _configuration_h

#ifdef __cplusplus
extern "C" {
#endif

#include <config.h>

#define DEFAULT_CAPTURE_RATE 20

#define ARG_HELP "--help"
#define ARG_DEVICE "--device"
#define ARG_WIDTH "--width"
#define ARG_HEIGHT "--height"
#define ARG_FPS "--fps"
#define ARG_DUMPPATH "--dumppath"
#define ARG_DUMPMODE "--dumpmode"
#define ARG_LOGO "--logo"
#define ARG_FULLSCREEN "--fullscreen"
#define ARG_MINIMODE "--minimode"
#define ARG_COPYING "--copying"
#define ARG_WARRENTY "--warrenty"
#define ARG_DEBUG "--debug"


typedef struct _ProgCFG {
	char start_in_minimode;
	char start_in_fullscreen;
	int v4l_fps;
	int dump_mode;			// 0=jpeg 1=png 2=bmp
	int dump_quality;		// jpeg quality
	int dump_speed;			// dump after x seconds
	int dump_count;			// dump x pictures;
	unsigned long long dump_next;	// cpu_ticks when the next picture is taken
					// if (dump_count>0 && dump_next<CPU_Timestamp)

	char *powered_by_image_path;
	int powered_by_blitpos; // 0=not blitted, 1=upper left, 2=upper right, 3=lower left, 4=lower right, 5=center

	char *ftp_host;
	char *ftp_user;
	char *ftp_pass;
	char *ftp_temp;
	char *ftp_file;

	int debug;
	/* the following is subject to change*/
	char *FONT1;
	char *FONT2;
	char *FONT3;
} ProgCFG;

extern ProgCFG configuration; /* usable from anywhere in the program */

int create_default_config (void);
int parse_args(int argc, char **argv);
void print_commandLineOptions(void);
void print_progname (void);
char *find_file (const char *filename);
void warrenty(void);

#ifdef __cplusplus
	}
#endif


#endif // _configuration_h
