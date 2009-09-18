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
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL.h"

#include "SDL_v4l.h"
#include "webcam.h"
extern "C" {
#include "init.h"
#include "SDL_v4l_filters.h"
#include "SDL_v4l_filters_thomas.h"
#include "ftp-upload.h"
#include "capture.h"
#include "interface.h"
#include "configuration.h"
#include "config_file.h"
#include "logo.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#ifdef HAVE_NLS
#include <locale.h>
#include <libintl.h>
#endif


/* Video for linux and philips webcam includes */
#include <linux/videodev.h>
#include <sys/ioctl.h>
#include <dlfcn.h>
#include "pwc-ioctl.h"
#include "misc.h"

	};
#include "main.h"

extern SDL_Surface *screen;
extern int webcam_descriptor;

unsigned long long cpu_hertz = 0;
unsigned long long fps   = 0;
int sum_fps = 0;
int cnt_fps = 0;

int flip_x=0;
int flip_y=0;

static int running = 1;

unsigned int current_capture_width=0;
unsigned int current_capture_height=0;

void Exit         (void) { running = 0; } 
void toggle_flip_x(void) { flip_x = !flip_x; } 
void toggle_flip_y(void) { flip_y = !flip_y; }

void Title_set (const char *input) {{{
	char *title = (char*)xMalloc (strlen ("SDLcam - ")+strlen (input) + 1); 
	sprintf (title, "SDLcam - %s", input);
	SDL_WM_SetCaption (title, NULL);
	free (title);
	}}}

typedef cInput* (*newInput) (const char *name);

int Add_Input (const char *type, const char *device) {{{
	cInput *neu = NULL;
	if (0 == strcmp ("Webcam" , type)) {{{
		neu = new cWebcam (device);
		}}} 
	if (0 == strcmp ("S-Video", type)) {{{
		neu = new cV4L (device);
		if (neu == NULL) return -1;
		neu->Parameter("channel", 2);
		}}}
	if (neu == NULL) {{{
		newInput  make = NULL;
		char *TYPE = (char*) xMalloc (strlen (LIBDIR "/sources/.so") + strlen (type) + 2);
		sprintf(TYPE, LIBDIR "/sources/%s.so", type);
		void *handle = dlopen (TYPE, RTLD_NOW);
		free (TYPE);
		if (handle == NULL) {
			printf("%s\n", dlerror());
			return -1;
			}
		if (NULL == (make = (newInput) dlsym (handle, "newInput"))) {
			printf("%s\n", dlerror());
			return -1;
			}
		if (NULL == (neu = make (device))) return -1;
		}}}
	if (! neu->isOK()) {{{
		delete neu;
		return -1;
		}}}
	v4l = (cInput**)xRealloc (v4l, v4l_max + 1);
	v4l[v4l_max] = neu;
	v4l_max++;
	printf (_("%s use %s (%s)\n"), type, device, neu->Name());
	return v4l_max - 1;
	}}}

int main(int argc, char **argv) {{{
	SDL_Event event;
	int res;
	int redraw=10;
	unsigned long long last_time = Get_Timestamp ();
	cpu_hertz = Get_Mhz ();

#ifdef HAVE_NLS
//	bindtextdomain ("wget", LOCALEDIR);
	setlocale (LC_ALL, "");
	bindtextdomain ("SDLcam", "/home/lussnig/cvs/SDLcam");
	textdomain ("SDLcam");
	printf("test\n");
	printf(_("test\n"));
	printf(gettext("test\n"));
	return 0;

#endif

	print_progname();
	
	/* Load config and command line options BEFORE filters. Otherwise,
	 * --debug is less useful */
	create_default_config(); /* init the config structures */
	find_and_parse_config_file(); /* guess what... */
	res = parse_args(argc-1, &argv[1]); /* Args have greater priority than config file */
	if (res<0) { return -1; }

	/* Initialise the filter stuff(all off) */
	SDL_InitFilters();


	if (initialise_sdl ()) return -1;
	interface_init();
	if (v4l_max<1) {
		fprintf(stderr, _("FATAL ERROR: No valid input source found. Aborting...\n"));
		return -1;
	}
	if (!v4l[v4l_akt]->isOK()) {
		return -1;
	}

	/* Set the window title bar text */
	Title_set (v4l[v4l_akt]->Name ());
	
	do {{{
		int mini;
		SDL_v4l_image *image;

		/* {{{ calculate actual fps and fps_stat */	  
		fps = Get_Timestamp ();
		fps -= last_time;
		fps = cpu_hertz / fps;
		sum_fps += fps;
		cnt_fps ++;
		last_time = Get_Timestamp ();
		/* }}} */
		/* {{{ handle SDL events */
		while (SDL_PollEvent(&event) && running )
        		switch (event.type)
			  {
				case SDL_MOUSEBUTTONDOWN:
					interface_click(event.button.x, event.button.y, screen);
					break;
				case SDL_KEYDOWN:
					interface_key(&event.key.keysym);
					break;
				case SDL_QUIT:
					running = 0;
					break;
			  }
		if (!running) break;
		if (configuration.dump_count > 0 && configuration.dump_next < Get_Timestamp ())
		  {
	 	    captured_size = interface_capture (screen ,image->w, image->h);
		    configuration.dump_count--;
		    configuration.dump_next = Get_Timestamp () + configuration.dump_speed * cpu_hertz;		
		  }			
				
		/* }}} */
		mini = get_mini_mode();
		image_x = (mini) ? 0 : IMAGE_X;
		image_y = (mini) ? 0 : IMAGE_Y;
		/* get an image */
		image = v4l[v4l_akt]->Fetch ();
		if (image == NULL) continue;
		if (image->len == 0) continue;
		//initialise_sdl_video (320, 240);
		/* Apply the filters. This step may change the final image size */
		SDL_ApplyFilters(image);
		
		if ( (image->w!=current_capture_width)||(image->h!=current_capture_height) || sdl_reinit ) {{{
			if (configuration.debug > 0) printf(_("Image size changed!\n"));
			current_capture_width = image->w;
			current_capture_height = image->h;
			initialise_sdl_video (image->w, image->h);
			 }}}

		/* Display the filtered image */
		if (! GET_FILTER (freeze))
			show_image(image, flip_y, screen);

		/* Brand it with an logo */
		if (configuration.powered_by_blitpos!=0)
			logo_show(screen, image_x, image_y, image->w, image->h, configuration.powered_by_image_path, configuration.powered_by_blitpos);

		/* Draw it on the screen */
		SDL_UpdateRect(screen, image_x, image_y, image->w, image->h);

		if (!mini && redraw > 5) {
			interface_draw(screen);
			redraw = 0;
			}
		redraw++;
		}}} while (running);
	SDL_Filters_Cleanup ();
	interface_close ();
	close_sdl ();
	for (int idx=0; idx<v4l_max; idx++) delete v4l[idx];
	free (v4l);
	ftp_quit ();
	return 0;
	}}}

/*
 * vim600: fdm=marker
 */
