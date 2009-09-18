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
#include <math.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

#include "SDL_v4l_filters.h"
#include "SDL_v4l_filters_thomas.h"
#include "parser.h"
#include "configuration.h"
#include "main.h"
#include "misc.h"
#include "interface.h"
#include "dyn_misc.h"


SDL_v4l_filterStatus g_filter_stats;
SDL_v4l_image background,buffer[10],*source;

int	background_capture;
int	started = 0;

char *FILTER_NOT_IN_CHAIN[]={
	"rgb2bgr",
	"generic-mmx-bbv",
	/*"mmx_halfsize",*/
	/*"halfsize",*/
#ifdef MMX
	"halfsize",
#else
	"mmx_halfsize",
#endif
	NULL};

/* {{{ int SDL_v4l_Filter_brightnes_contrast     (const SDL_v4l_image *SRC, SDL_v4l_image *DST,int brightness,int contrast) OK
 *
 * Manipulate Brightness and Contrast ( valid range -127 +127 )
 *
 * Input:
 * SDL_v4l_image *		Source/Dest
 *
 * Output:
 * int					0/0 succes
 *
 * Modify:
 * param[0]
 */
int SDL_v4l_Filter_brightnes_contrast (const SDL_v4l_image *SRC, SDL_v4l_image *DST,int brightness,int contrast)
{
	/* Find the lower and higher pixel value, and scale theses value to max and min */
	unsigned char levels[256];
	size_t i;
	unsigned char *src=SRC->data;
   	unsigned char *dst=DST->data;
	double b=((double)brightness)/255.0;
	double c=((double)contrast  )/127.0;
	SDL_v4l_Buffer_Prepare (SRC, DST);
    if(SRC->len&3)
      return 0;
	for(i=0;i<256;i++)
	  {
		double v=((double)i)/255.0;
		double t,p;
	
	    if (b < 0.0)	v = v * (1.0      + b);
	    else            v = v + (1.0 - v) * b;
	    /* apply contrast */
	    if (v > 0.5)	t = 1.0 - v;
	    else			t =       v;
	    if (t < 0.0)	t = 0.0;
	    if (c < 0.0)	p = 1.0 + c;
	    else 			p = (c == 1.0) ? 127 : 1.0 / (1.0 - c);
	    t = 0.5 * pow (2.0 * t, p);
	    if (v > 0.5)	v = 1.0 - t;
	    else			v =       t;
		
	    levels[i]=(int)(v*255.0);
	  }
	for (i=0; i<SRC->len; i+=4)
	  {
		dst[i  ] = levels[src[i  ]];
		dst[i+1] = levels[src[i+1]];
		dst[i+2] = levels[src[i+2]];
	  }
	return 0;
}
/* }}} */
/*
  * make the average on N images (N must be power of 2)
  *
  * Input:
  * SDL_v4l_image *	source
  *
  * Output:
  * int			0/1 success
  *
  * Modify:
  * param[0]		New Image
  */
SDL_v4l_image average_buffer[RANGE_COUNT_MAX];
unsigned short *average_summe = NULL;				// summe=summe+image-image[N]

static void SDL_v4l_Filter_average_cleanup (void) {{{
	int i;
	if (average_summe!=NULL)
	  free (average_summe);
	for( i = 0;i<RANGE_COUNT_MAX;i++)
	  if (average_buffer[i].data != NULL)
	    free (average_buffer[i].data);
}}}
static int  SDL_v4l_Filter_average(SDL_v4l_image *image,unsigned int max_lb) {{{
	static unsigned int last_len = 0;
	static int count = 0;
	int i = 0;
	int update = 0;
	if (max_lb > RANGE_COUNT_MAX_LB)
	  max_lb=RANGE_COUNT_MAX_LB;

/* {{{ initialise buffer's and summe */
	for (i=0; i< (1<<RANGE_COUNT_MAX_LB); i++) 
	  SDL_v4l_Buffer_Prepare (image,&(average_buffer[i]));
	if (last_len != image->len || update)
	  {
	   average_summe = (unsigned short*)xRealloc (average_summe, image->len * sizeof (unsigned short));
	   last_len = image->len;
	    memset (average_summe                , 0, image->len * sizeof (unsigned short));
		memset (average_buffer[count].data, 0, average_buffer[count].len);
		for (i=0; i< (1<<max_lb); i++) 
		  if(i!=count)
		    memset (average_buffer[i].data, 0, average_buffer[i].len);
	  }
/* }}} */	
#ifdef MMX
	MMX_add_sub_shr(image->data, average_buffer[count].data, average_summe, last_len, max_lb);
#else
	{{{
	  unsigned char *buf = average_buffer[count].data;
	  for (i=0; i<last_len; i+=4) {
		unsigned short mm0[4],mm2[4];
		mm0[0]=buf[i+0];
		mm0[1]=buf[i+1];
		mm0[2]=buf[i+2];
		mm2[0]=image->data[i+0];
		mm2[1]=image->data[i+1];
		mm2[2]=image->data[i+2];
		buf[i+0]=mm2[0];
       	buf[i+1]=mm2[1];
        buf[i+2]=mm2[2];
		mm2[0]=mm2[0]-mm0[0];
		mm2[1]=mm2[1]-mm0[1];
		mm2[2]=mm2[2]-mm0[2];
		mm2[0]+=average_summe[i+0];
		mm2[1]+=average_summe[i+1];
		mm2[2]+=average_summe[i+2];
		average_summe[i+0]=mm2[0];
		average_summe[i+1]=mm2[1];
		average_summe[i+2]=mm2[2];
		mm2[0]>>=RANGE_COUNT_MAX_LB;
     	mm2[1]>>=RANGE_COUNT_MAX_LB;
     	mm2[2]>>=RANGE_COUNT_MAX_LB;
		image->data[i+0]=mm2[0];
		image->data[i+1]=mm2[1];
		image->data[i+2]=mm2[2];
		}
	}}}
#endif
	count++;
	if(count==(1<<max_lb)) count=0;
	return 0;
}}}

int Load_Module            (const char *dir, const char *file) {{{
	      int    (*Filter_Type) (void);
	const char * (*Filter_Name) (void);
	Filter_plugin *filter;
	char *filename = (char*)xMalloc (strlen (dir) + 1 + strlen (file) + 1);
	sprintf(filename, "%s/%s", dir, file);

	filter = (Filter_plugin *) xMalloc (sizeof (Filter_plugin));
	if (NULL == (filter->handle = dlopen (filename, RTLD_NOW))) {{{
		if (configuration.debug)
		    fprintf (stderr, "Loading module %s : %s\n", file, dlerror());
		return -1;
	    //exit(-1);
	  }}}
	free (filename);

	filter->priority = 0;

	if (NULL == (filter->Apply.VOID = (tInt_func)dlsym(filter->handle, "Apply"))) {{{
	  fprintf (stderr,"in %s => %s\n", file, dlerror());
	  return -1;
	  //it(-1);
	  }}}
	if (NULL == (Filter_Type = (tVoid_func)dlsym(filter->handle, "Type" ))) {{{
	  fprintf (stderr,"%s\n",dlerror());
	  return -1;
	  //exit(-1);
	  }}}
	if (NULL == (Filter_Name = (tVoid2_func) dlsym(filter->handle, "Name" ))) {{{
	  fprintf (stderr,"%s\n",dlerror());
	  return -1;
	  //exit(-1);
	  }}}


	filter->Cleanup = (tVoid_func ) dlsym (filter->handle, "Cleanup");
	filter->Toggle  = (tVoid_func ) dlsym (filter->handle, "Toggle" );
	filter->Mode_set= (tInt_func  ) dlsym (filter->handle, "Mode_set");
	filter->Mode_get= (tVoid_func ) dlsym (filter->handle, "Mode_get");
	filter->Display = (tVoid2_func) dlsym (filter->handle, "Display");
	filter->type = Filter_Type();
	filter->name = xStrdup (Filter_Name ());
	filter->mode = (filter->Mode_get != NULL) ? filter->Mode_get() : 0;
	
	if (strcmp(filter->name, "noise")==0) {
		filter->priority = 1;
		printf("Priority enabled on noise filter\n");
	}


	if(filter->type<0 || filter->type>4) 
		{{{
		dlclose (filter->handle);
		fprintf (stderr, "Currently only Filter Type 0 - 2 supported (%s)\n", file);
		return 0;
		}}}

	filter->next = g_filter_stats.plugin;
	g_filter_stats.plugin=filter;

	if (configuration.debug) {
		printf("Loaded plugin filter : %32s %c %c %c %c %c %i\n", filter->name, 
			((filter->Cleanup !=NULL)?'C':' '),
			((filter->Toggle  !=NULL)?'T':' '),
			((filter->Mode_set!=NULL)?'S':' '),
			((filter->Mode_get!=NULL)?'G':' '),
			((filter->Display !=NULL)?'D':' '),
			filter->mode
			);
	} else {
		printf("."); fflush(stdout);
	}
	
  	if (filter->Mode_get != NULL)	filter->mode = filter->Mode_get();
  	else		  				   	filter->mode = 0;
  	return 1;
}}}
int Mode_Filter_set        (const char *name, int wish) {{{
  Filter_plugin *filter = Find_Filter (name);
  if (filter == NULL) return 0;
  if(filter->Mode_set != NULL) return filter->mode = filter->Mode_set (wish);
  return filter->mode = wish;
}}}
const char *Display_Filter (const char *name) {{{
  Filter_plugin *filter = Find_Filter (name);
  if (name == NULL) return 0;
  if (filter != NULL)
    {
      if (filter->Display == NULL) return filter->name;
	  return filter->Display ();
    }
  if(strcmp("average"	 ,name)==0) return "Average";
  if(strcmp("invert"	 ,name)==0) return "Invert";
  if(strcmp("userfilter" ,name)==0) return "Userfilter";
  if(strcmp("rgb-bgr"	 ,name)==0) return "RGB2BGR";
  return "";
}}}

void Fetch_Plugins(const char *directory,int sorte) {{{
	DIR *dir = opendir (directory);
	struct dirent *direntp;
	if (dir==NULL) { 
		if (configuration.debug)
			fprintf(stderr, "failed to open directory '%s'. (%s)\n",
					directory, strerror(errno));
		return;
	} else {
		if (configuration.debug)
			printf("Scanning directory '%s'\n", directory);
	}
	while (NULL != (direntp = readdir (dir)))
		if(strlen (direntp->d_name)>3)
			if(strcmp(direntp->d_name + strlen(direntp->d_name) - 3,".so") == 0)
			{
				int res = Load_Module (directory, direntp->d_name);
				if (res<0) {
					if (configuration.debug) {
						printf("Skipping file: %s\n", direntp->d_name);
					}
				} else {
					g_filter_stats.plugin->sorte = sorte;
					if (configuration.debug) {
						printf("Load file: %s\n", direntp->d_name);
					}
				}
			}
	closedir (dir);
	}}}

int  SDL_InitFilters     (void) {{{
	
	printf("Loading plugins\n");
	
	/* Init all filter (default deaktivate) */
	g_filter_stats.invert			= 0;
	g_filter_stats.use_yuv2rgb		= 0;
	g_filter_stats.average_max_lb	= RANGE_COUNT_MAX;
	g_filter_stats.userdefined		= NULL;
	g_filter_stats.user				= 0;
	g_filter_stats.freeze			= 0;
	g_filter_stats.reorder_typ	  	= 0;
	g_filter_stats.reorder_radius 	= 45;
	g_filter_stats.reorder_effekt 	= 0.8;
	g_filter_stats.plugin      		= NULL;
	Fetch_Plugins (LIBDIR "/filters", 0);
	Fetch_Plugins (LIBDIR "/capture", 1);
	Fetch_Plugins ("./filter" , 0);
	Fetch_Plugins ("./capture", 1);
	printf("Done!\n");
	return 0;
}}}
void SDL_Filters_Cleanup (void) {{{
  int i;
  if (started) {
	  if (background.data != NULL)
		free (background.data);
	  for(i=0;i<10;i++)
		if(buffer[i].data)
		  free(buffer[i].data);
  	}
  while (g_filter_stats.plugin != NULL)
    {
	  Filter_plugin *akt=g_filter_stats.plugin;
	  if (akt->Cleanup != NULL) akt->Cleanup ();
	  free (akt->name);
	  dlclose (akt->handle);
	  g_filter_stats.plugin = akt->next;
	  akt->next = NULL;
	  free (akt);
	}
  SDL_v4l_Filter_average_cleanup 	();
  SDL_v4l_Reorder_cleanup			();
}}}

void LineRGB (SDL_v4l_image *image, int X1, int Y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b ) {{{
	int diry = image->w;
	int xinc = 1;
	int yinc = 1;
   	int dx, dy, d, inc_major, inc_minor;
	unsigned int  off;
	unsigned int *dat;
	unsigned int  col  = r;
	if (image->data == NULL) return;
	if (X1 <  0		  ) X1 = 0;
	if (x2 <  0		  ) x2 = 0;
	if (Y1 <  0		  ) Y1 = 0;
	if (y2 <  0		  ) y2 = 0;
	if (X1 >= image->w) X1 = image->w - 1;
	if (x2 >= image->w) x2 = image->w - 1;
	if (Y1 >= image->h) Y1 = image->h - 1;
	if (y2 >= image->h) y2 = image->h - 1;
	col = ((((col << 8) + g) << 8) + b);
	off = (Y1 * image->w + X1) << 2;
	dat = (unsigned int*)(image->data + off);
   	dx = x2 - X1;
   	dy = y2 - Y1;
	*dat = col;
   	if (dx < 0) {{{ 												// reflect across y-axis for negative dx
      		xinc = -xinc;
      		dx   = -dx;
   		}}}
   	if (dy < 0) {{{												// reflect across x-axis for negative dy
      		yinc = -yinc;
			diry = -diry;
      		dy   = -dy;
   		}}}
	if (dy == 0) {{{										// horizontal lines...
      		while (--dx) {
				dat += xinc;
				*dat = col;
      			}
		return;
   		}}}
	if (dx == 0) {{{				// vertical lines... 
      		while (--dy) {
				dat += diry;
				*dat = col;
      			}
		return;
   		}}}
	if (dx >= dy) {{{			/* Quadrant I, slopes <= than 1 and Quadrant IV, slopes >= than -1... */
      		inc_major = 2 * dy; /* increment for moving E */
      		d = inc_major - dx; /* initial value of d:  2 * dy - dx */
      		inc_minor = d - dx;	/* increment for moving NE or SE:  2 * (dy - dx) */
      		while (--dx) {
         		if ( d <= 0 ) d += inc_major;
         		else {
					d += inc_minor;
					dat += diry;
					}
				dat += xinc;
				*dat = col;
      		}
		return;
   		}}}
	if (dx <  dy) {{{		/* Quadrant I, slopes >  than 1 and Quadrant II, slopes <  than -1... */
		inc_major = 2 * dx; /* increment for moving N */
		d = inc_major - dy; /* initial value of d:  2 * dx - dy */
		inc_minor = d - dy;	/* increment for moving NE or NW:  2 * (dx - dy) */
		while (--dy) {
			if ( d <= 0 ) d += inc_major;
			else {
				d += inc_minor;
				dat += xinc;
				}
			dat += diry;
			*dat = col;
			}
		}}}
	}}}

void Apply            (SDL_v4l_image *image, int sorte) {{{
  Filter_plugin *akt;
 int prio;

 for (prio=1; prio >= 0; prio--) {

  for (akt = g_filter_stats.plugin; akt != NULL; akt = akt->next) {
	  int not_in_chain = 0;
	  int ok = 1;
	
		if (akt->priority != prio) 
			continue;

	  if (akt->mode == 0 || akt->sorte != sorte) continue;
	  for (not_in_chain = 0; FILTER_NOT_IN_CHAIN[not_in_chain]!=NULL && ok; not_in_chain++) {
		  ok &= ( 0 != strcmp(akt->name, FILTER_NOT_IN_CHAIN[not_in_chain]));
	      }
	  if (!ok) continue;
	  switch(akt->type) {
		case 0 : akt->Apply.BB  (akt->mode, image	   , image); break;
		case 1 : akt->Apply.B   (akt->mode,       	     image); break;
		case 2 : akt->Apply.BBB (akt->mode, &background, image, image); break;
		case 3 : akt->Apply.BBv (akt->mode, image      , image, 20   ); break;		   
		case 4 : akt->Apply.BBv (akt->mode, &background, image, 20   ); break;		   
		default: printf("Unsuported mode for %s\n filter", akt->name);
		}
  }

  }

}}}
int  SDL_ApplyFilters (SDL_v4l_image *image) {{{
/* Apply all selected filter if needed
 *
 * Input:
 * SDL_v4l_image *		Source/Destinaton
 */
	int i;
	source=image;
	if (image->len == 0) return 0;
	if (background.data==NULL || background.len != image->len) {{{
		if (background.data != NULL) free (background.data);
		background.data = (unsigned char *)xMalloc (image->len);
		background.w    = image->w;
		background.h    = image->h;
		background.len  = image->len;
		if (background.data != NULL)
		{
		    memcpy(background.data, image->data, image->len);
	        background_capture = 0;
		}
	}}}
	
	/* should be obsolete when all filter have on use DST correction */
	for(i=0;i<10;i++)  SDL_v4l_Buffer_Prepare (image, buffer+i);

	if (! g_filter_stats.use_yuv2rgb) Call_Filter_b  		("rgb2bgr", -1, image);
	if (! image->inverted) // Source was inverted on Fetch
	  if (g_filter_stats.invert  	) Call_Filter_bbv		("generic-mmx-bbv", 3, image, image,0xffffffff);
	if (g_filter_stats.average 		) SDL_v4l_Filter_average(image,g_filter_stats.average_max_lb);
	Apply (image, 0);

	if (g_filter_stats.reorder_typ)
	  SDL_v4l_Reorder	(image, image, mouse_img_x, mouse_img_y,g_filter_stats.reorder_typ,g_filter_stats.reorder_radius,g_filter_stats.reorder_effekt);
	if (g_filter_stats.user == 1) Assign (g_filter_stats.userdefined , NULL);

	Apply (image, 1);

	if (background_capture && background.data != NULL)
	  {
		memcpy(background.data, image->data, image->len);
        background_capture = 0;
	  }

	return 0;
}}}
/*
 * vim600: sw=4 ts=4 fdm=marker
 */
