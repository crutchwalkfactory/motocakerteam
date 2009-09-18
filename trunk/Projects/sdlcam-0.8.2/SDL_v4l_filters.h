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
#ifndef _SDL_v4l_filters_h
#define _SDL_v4l_filters_h

#ifdef __cplusplus
extern "C" {
#endif

#define RANGE_COUNT_MAX_LB 6 /* COUNT = 2^4   */
#define RANGE_COUNT_MAX (1<<RANGE_COUNT_MAX_LB)

#include "SDL_v4l_image.h"

typedef int (*tInt_func)(int);
typedef int (*tVoid_func)(void);
typedef const char* (*tVoid2_func)(void);

typedef struct _Filter_plugin {
	int priority;		// 0 second pass, 1 first pass
	int	sorte;					// 0 => normaler filter 1 => capture filter
	int 	    (*Toggle		) (void);
	int 	    (*Mode_get 		) (void);	// Lesen
	int 	    (*Mode_set 		) (int );	// Setzen
	int 	    (*Cleanup		) (void);
	union {
	int	    (*VOID		) (int mode);
	int 	    (*BB 		) (int mode, const SDL_v4l_image *SRC, 				 SDL_v4l_image *DST);
	int 	    (*B  		) (int mode,                           				 SDL_v4l_image *DST);
	int 	    (*BBB 		) (int mode, const SDL_v4l_image *A  , const SDL_v4l_image *B  , SDL_v4l_image *DST);
	int 	    (*BBv 		) (int mode, const SDL_v4l_image *A  , 				 SDL_v4l_image *DST, unsigned int val);
		} Apply;
	const char *(*Display		) (void);
	int	    mode;				// faster and fall back
	int	    type;				// fatser then always call
	char	   *name;				// faster than always call
	void 	   *handle;
	struct _Filter_plugin	*next;
	} Filter_plugin;

typedef struct _SDL_v4l_filterStatus {
	char invert;
	char average;
	unsigned char average_max_lb;
	char use_yuv2rgb;
	char rgb2bgr;
	char *userdefined;
	char user;
	char freeze;
	char   reorder_typ;
	int    reorder_radius;
	double reorder_effekt;
	Filter_plugin *plugin;
} SDL_v4l_filterStatus;

void LineRGB (SDL_v4l_image *image, int x_start, int y_start, int x2, int y2, unsigned char r, unsigned char g, unsigned char b );
int Call_Filter_bb    	   (const char *name, int mode, const SDL_v4l_image *SRC, 			    SDL_v4l_image *DST);
int Call_Filter_bbb    	   (const char *name, int mode, const SDL_v4l_image *A  , const SDL_v4l_image *B  , SDL_v4l_image *DST);
int Call_Filter_bbv    	   (const char *name, int mode, const SDL_v4l_image *A  			  , SDL_v4l_image *DST, unsigned int val);
int Toggle_Filter  	   (const char *name);
int Mode_Filter    	   (const char *name);
int Mode_Filter_set	   (const char *name, int wish);
const char *Display_Filter (const char *name);

extern int background_capture;
extern SDL_v4l_image background,buffer[10],*source;
extern SDL_v4l_filterStatus g_filter_stats; // holds which filters are enable/disable
#define TOGGLE_FILTER(FILTER_MEMBER) g_filter_stats. FILTER_MEMBER = !(g_filter_stats. FILTER_MEMBER )
#define TOGGLE_DEEP() g_filter_stats.deep  =(g_filter_stats.deep  +1)%8;
#define TOGGLE_REORDER() g_filter_stats.reorder_typ  =(g_filter_stats.reorder_typ  +1)%3;
#define TOGGLE_BLUEBOX() g_filter_stats.bluebox=!g_filter_stats.bluebox; background_capture=g_filter_stats.bluebox;
#define GET_FILTER(FILTER_MEMBER) g_filter_stats. FILTER_MEMBER

int SDL_InitFilters (void);
int SDL_ApplyFilters (SDL_v4l_image *image);

int SDL_v4l_Buffer_Prepare 		(const SDL_v4l_image *SRC, SDL_v4l_image *DST);
int SDL_v4l_Filter_GrayScale_Average	(const SDL_v4l_image *src, SDL_v4l_image *dst);
int SDL_v4l_Filter_brightnes_contrast   (const SDL_v4l_image *SRC, SDL_v4l_image *DST,int brightness,int contrast);
//int SDL_v4l_Filter_Enhance_YUV		(const SDL_v4l_image *src, SDL_v4l_image *dst);
//int SDL_v4l_Info_levels			(const SDL_v4l_image *SRC, SDL_v4l_image *DST);
void SDL_Filters_Cleanup (void);

#ifdef __cplusplus
	}
#endif


#endif
