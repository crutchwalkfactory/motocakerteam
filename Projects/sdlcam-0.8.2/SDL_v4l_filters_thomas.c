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

#include "SDL_v4l_filters.h"
#include "SDL_v4l_filters_thomas.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "dyn_misc.h"
#include "misc.h"

#ifdef MMX
char SHIFT[8];
int MMX_add_sub_shr (unsigned char *bild, unsigned char *buf, unsigned short *summe, unsigned int len, unsigned int shr) {{{ // emms
	/* 
	   summe = summe + bild_a - bild_b
	   bild_b = bild_a
	   bild_a = summe >> shr 
	 */
	SHIFT[0]=shr;
	SHIFT[1]=0;
	SHIFT[2]=0;
	SHIFT[3]=0;
	SHIFT[4]=0;
	SHIFT[5]=0;
	SHIFT[6]=0;
	SHIFT[7]=0;
	if (len < 8) return 0;              // image size must be at least 8 bytes 
	return 1;
	}}}
#endif

/*
#define LIMIT(x) ((x)>0xFFFFFF?0xff: ((x)<=0xffff?0:((x)>>16)))
const int rvScale =  91881;
const int guScale = -22553;
const int gvScale = -46801;
const int buScale = 116129;
const int yScale  =  65536;		
r = rvScale * v + y * yScale;
g = guScale * u + y * yScale + gvScale * v;
b = rvScale * u + y * yScale;
*/

#define CLIP         320
#define GREEN2_MUL  ( -512/6)	// = -85
#define GREEN2_ADD  (65536/6)	// = 10923

/* lookup tables */
static unsigned int  ng_yuv_g2  [256];
static unsigned char ng_clip    [256 + 2 * CLIP];
static void yuv2rgb_init(void) {{{
	int i;
	/* init Lookup tables */
	for (i = 0; i < 256; i++) {
		ng_yuv_g2[i]   = (GREEN2_ADD + i * GREEN2_MUL) >> 8;
		}
	if(!g_filter_stats.invert) {
		for (i = 0; i <     CLIP      ; i++) ng_clip[i] = 0;
		for (     ; i <     CLIP + 256; i++) ng_clip[i] = i - CLIP;
		for (     ; i < 2 * CLIP + 256; i++) ng_clip[i] = 255;
		}
	else
		{
		for (i = 0; i <     CLIP      ; i++) ng_clip[i] = 255;
		for (     ; i <     CLIP + 256; i++) ng_clip[i] = 255 - (i - CLIP);
		for (     ; i < 2 * CLIP + 256; i++) ng_clip[i] = 0;
		}
	}}}
void yuv420p_to_rgb32(size_t width,size_t height,unsigned char *y,unsigned char *u,unsigned char *v,unsigned char *d) {{{
static	int init=1;
static 	int invers=0;
	unsigned int j;
	g_filter_stats.use_yuv2rgb=1;
	if(init || invers!=g_filter_stats.invert) {
		yuv2rgb_init();
		init=0;
		invers=g_filter_stats.invert;
		}
	width>>=1;
	while(height--) {
		for (j = 0; j < width; j++) {{{
			d+=8;
			y+=2; 
			++u; 
			++v;
			}}}
		if ( height % 2) {
			u -= width;
			v -= width;
			}
		}
	}}}
void yuv420p_to_bgr32(size_t width,size_t height,unsigned char *y,unsigned char *u,unsigned char *v,unsigned char *d) {{{
static	int init=1;
static 	int invers=0;
	unsigned int j;
	g_filter_stats.use_yuv2rgb=1;
	if(init || invers!=g_filter_stats.invert) {
		yuv2rgb_init();
		init=0;
		invers=g_filter_stats.invert;
		}
	width>>=1;
	while(height--) {
		for (j = 0; j < width; j++) {{{
			d+=8;
			y+=2; 
			++u; 
			++v;
			}}}
		if ( height % 2) {
			u -= width;
			v -= width;
			}
		}
	}}}

int *SIN_glas=NULL;
int *DREH_x = NULL;
int *DREH_y = NULL;
static void Dreh_cleanup (void) {{{
  if(DREH_x!=NULL) free(DREH_x);
  if(DREH_y!=NULL) free(DREH_y);
  }}}
static void Glas_cleanup (void) {{{
 if (SIN_glas != NULL)
   free(SIN_glas);
 SIN_glas = NULL;
}}}
static void Glas(const int x,const int y,const int r,double diop,int *ox,int *oy) {{{
/* {{{ vorausberechnen der verzerrung */	
  static int old_r=0;
  double dr = (double)(dr);
  if(old_r!=r || SIN_glas==NULL)
    {
	  int t;
	  if(SIN_glas!=NULL)
		free(SIN_glas);
	  SIN_glas = (int*)xMalloc ((2 * r + 1) * sizeof(int) );
	  for(t=-r;t<r;t++) 
	    {
		float temp = 3.1415*0.5*((double)t)/dr;
	    SIN_glas[t+r] = (int) (sin (temp) * dr * diop);
	    }
	  old_r=r;
    }
/* }}} */  
/* {{{ im kreis(r) verzehrung anwenden */
  if(x<r && y<r && -x<r && -y<r)
    if (x*x+y*y <= r*r)
      {
        *ox = SIN_glas[x+r];
        *oy = SIN_glas[y+r];
		return;
      }
/* }}} */  
/* {{{ sonst ohne verzehrung ausgeben */
  *ox=0;
  *oy=0;
  return;
/* }}} */  
  }}}
static void Dreh(const int x,const int y,const int r,double diop,int *ox,int *oy) {{{
static int last_r    = 0;
static double last_diop = 0;
  int idx;
  if(last_r!=r || last_diop!=diop)
    {
	  int tx,ty;
	  if (last_r!=r)
		{
	      DREH_x = (int*) xRealloc (DREH_x, (2*r+1)*(2*r+1) * sizeof(int));
	      DREH_y = (int*) xRealloc (DREH_y, (2*r+1)*(2*r+1) * sizeof(int));
		}
  	  for(ty=-r;ty<r;ty++)
	    for(tx=-r;tx<r;tx++)
		  {
		    idx = (r+ty)*(2*r+1)+(r+tx);
		    DREH_x[idx] = (int) ((double)tx*sin(diop*3.1415)-ty*cos(diop*3.1415));
		    DREH_y[idx] = (int) ((double)ty*sin(diop*3.1415)+tx*cos(diop*3.1415));
		  }
	  last_r=r;
	  last_diop=diop;
	}

/* {{{ im kreis(r) verzehrung anwenden */
  if(x<r && y<r && -x<r && -y<r && diop != 0)
    if (x*x+y*y <= r*r)
      {
	    idx = (r+y)*(2*r+1)+(r+x);
		*ox = DREH_x[idx];
		*oy = DREH_y[idx];
		return;
      }
/* }}} */  
/* {{{ sonst ohne verzehrung ausgeben */
  *ox=0;
  *oy=0;
  return;
/* }}} */  
  }}}

void SDL_v4l_Reorder_cleanup (void) {{{
	Glas_cleanup ();
	Dreh_cleanup ();
	}}}
int SDL_v4l_Reorder (const SDL_v4l_image *SRC, SDL_v4l_image *DST,int off_x,int off_y,int typ,int radius,double effekt) {{{
	SDL_v4l_image TMP;
	size_t x,y=0;
	unsigned int *src = (unsigned int *)SRC->data;
	unsigned int src_w = SRC->w;
	unsigned int dst_w;
	unsigned int *dst;
	TMP.len=0;
	TMP.data=NULL;
	SDL_v4l_Buffer_Prepare (SRC, &TMP);
	SDL_v4l_Buffer_Prepare (SRC,  DST);
	dst = (unsigned int *)TMP.data;
	dst_w = DST->w;

	for(y=0;y<DST->h;y++) {
	  for(x=0;x<DST->w;x++)
	    {
	      int src_x = x - off_x;
		  int src_y = y - off_y;
		  switch(typ) {
		  case  1 : Glas (src_x, src_y, radius, effekt, &src_x, &src_y); break;
		  case  2 : Dreh (src_x, src_y, radius, effekt, &src_x, &src_y); break;
		  default : break;
		    }
		  src_x += x;
		  src_y += y;
		  if (src_x>=0 && (unsigned int)src_x<=SRC->w && src_y>=0 && (unsigned int)src_y<=SRC->h)
	        *(dst++) = src[src_y*src_w+src_x];
		  else
			*(dst++) = src[y*src_w+x];
		}
	    }
	free(DST->data);
	memcpy(DST,&TMP,sizeof(TMP));
	return 1;
}}}

/*
 * vim600: sw=4 ts=4 fdm=marker
 */
