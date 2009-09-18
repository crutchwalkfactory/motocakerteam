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
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "SDL.h"
#include "configuration.h"
#include "interface.h"
#include "dump_thomas.h"
#include "SDL_v4l_filters.h"
#include "ftp-upload.h"
#include "config_file.h"
#include "dyn_misc.h"

extern int IMAGE_X;
extern int IMAGE_Y;

/* Functions to capture an image to a file. (was in interface.c)*/
#ifdef __cplusplus
extern "C" {
#endif
#include "capture.h"
int interface_capture (SDL_Surface *screen, int w, int h) {{{
	const	char *extension[]={".jpeg",".png",".bmp"};
	/* This function is called to take a picure from the cam
	 * and save it to a file */
	char *capture_filename;
	SDL_Surface *tmp_surface;
	struct stat	file_stat;
	int		file_size = -1;

	/* Get a free filename */
	capture_filename = getNewFilename (Dump_Path(), Dump_Prefix(), extension [configuration.dump_mode]);
	if (NULL == capture_filename)
	  return -1;

	/* Capture the image*/
	tmp_surface = get_ImageSurface(screen, w, h);

	/* {{{ Write image */
	{
	  unsigned char *data;
	  int i;
	  unsigned char tmp;
	  data = (unsigned char*)tmp_surface->pixels;

	  if ( SDL_MUSTLOCK(screen) ) 
	    {
              if ( SDL_LockSurface(screen) < 0 ) 
		{
            	  fprintf(stderr, "Can't lock screen: %s\n", SDL_GetError());
		  free (capture_filename);
            	  return -1;
        	}
	      }
	  /* swap Red and blue */
	  for (i=0; i<tmp_surface->w*tmp_surface->h*3; i+=3)
	    {
	      tmp = data[i];
	      data[i] = data[i+2];
	      data[i+2] = tmp;
	    }

	  if ( SDL_MUSTLOCK (screen) ) 
	    {
              SDL_UnlockSurface (screen);
	    }
	  switch (configuration.dump_mode) 
	    {
	      case 0:
		write_file_jpeg (capture_filename, (unsigned char*)tmp_surface->pixels, tmp_surface->w, tmp_surface->h);
		break;
	      case 1:
		write_file_png  (capture_filename, (unsigned char*)tmp_surface->pixels, (unsigned int)tmp_surface->w, (unsigned int)tmp_surface->h);
		break;
	      case 2:
	        SDL_SaveBMP(tmp_surface, capture_filename);
		break;
	      default:
		fprintf(stderr, "Unsuported capture format.\n");
		break;
	    }
	}
	/* }}} */
	SDL_FreeSurface(tmp_surface);
//	if (configuration.debug > 0)
	printf("Saved file: %s\n", capture_filename);
/* {{{ FTP-Stuff */
	if(	configuration.ftp_host !=NULL &&
		configuration.ftp_user !=NULL &&
		configuration.ftp_pass !=NULL &&
		configuration.ftp_file !=NULL )
	  {
	    if (init_ftp (configuration.ftp_host, configuration.ftp_user, configuration.ftp_pass))
	      {
		if (configuration.debug > 0)
		  printf ("Starte FTP Upload\n");
		if (configuration.ftp_temp != NULL)
		  {
		    if (configuration.debug > 0)
		      printf ("Using FTP Rename Method\n");
		    if (ftp_send (capture_filename, configuration.ftp_temp))
		      ftp_rename (configuration.ftp_temp, configuration.ftp_file);
		  }
		else
		  ftp_send (capture_filename, configuration.ftp_file);
	     }
	  }
/* }}} */	
	if (!stat (capture_filename, &file_stat)) 
	  {
	    file_size = file_stat.st_size;
	  }
	free (capture_filename);
	return file_size;
}}}

SDL_Surface *get_ImageSurface(SDL_Surface *screen, int w, int h) {{{
	/* Return a new surface containing the webcam picture from the interface */
	SDL_Surface *tmp_surface;
	SDL_Rect source_rect;

	tmp_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 24, 0, 0, 0, 255);
	source_rect.w = w;
	source_rect.h = h;
	source_rect.x = IMAGE_X; //SIDE_BAR_W;
	source_rect.y = IMAGE_Y; //0;
	SDL_BlitSurface(screen, &source_rect, tmp_surface, NULL);

	return tmp_surface;
}}}

#ifdef __cplusplus
	}
#endif

/*
 * vim600: fdm=marker
 */
