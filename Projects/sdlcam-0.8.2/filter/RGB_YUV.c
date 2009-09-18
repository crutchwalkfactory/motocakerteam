#include "../SDL_v4l_image.h"
#include "../dyn_misc.h"
#include <stdlib.h>
#include <string.h>
/*
 Y = (0.257 * R) + (0.504 * G) + (0.098 * B) +  16
 U = (0.439 * R) - (0.368 * G) - (0.071 * B) + 128
 V =-(0.148 * R) - (0.291 * G) + (0.439 * B) + 128
 */  

int Apply(int mode,const SDL_v4l_image *src,SDL_v4l_image *dst) {{{
	/* edge detection */
	int r, c;
	int width = src->w;
	unsigned char *imageIn = src->data;
	unsigned char *imageOut = NULL;
	if (mode == 0) return 0;
	SDL_v4l_Buffer_Prepare (src, dst);
	if(src->data == dst->data)
	  imageOut = (unsigned char*)xMalloc (src->len * sizeof (unsigned char));
	else imageOut = dst->data;
	if (! imageOut)
	  return (0);

	for (r =0 ; r*4 < src->len ; r+= width) 
	  for (c = 0; c < width; c++) 
	    {
		int tmp[4];
		tmp[0]=imageIn[(r+c)*4+0];
		tmp[1]=imageIn[(r+c)*4+1];
		tmp[2]=imageIn[(r+c)*4+2];
	      	imageOut[(r+c)*4+0] =  16 + ( 263*tmp[0] + 516*tmp[1] + 100*tmp[2])/1024;
	      	imageOut[(r+c)*4+1] = 128 + ( 450*tmp[0] - 378*tmp[1] -  73*tmp[2])/1024;
	      	imageOut[(r+c)*4+2] = 128 + (-152*tmp[0] - 298*tmp[1] + 450*tmp[2])/1024;
	    }
  if(src->data == dst->data)
    {
	  memcpy (dst->data, imageOut, src->len);
  	  free (imageOut);
    }
  return (1);
  }}}

static int   status = 0;

       int   Type   	(void ) { return 0; status = 0;}		// MANDATORY
       int   Toggle 	(void ) { return status = (status + 1) % 2; }	// RECOMENDED ( else always  ON    )
       int   Mode_set  	(int a) { return status = a % 2; }		// RECOMENDED ( else unknown STATE )
       int   Mode_get  	(void ) { return status; }			// RECOMENDED ( else unknown STATE )
const  char *Name	(void ) { return "rgb2yuv"; }			// MANDATORY
const  char *Display	(void ) { return "rgb2yuv"; }                  // RECOMENDED ( else display filter name )

/*
 * vim600: fdm=marker
 */
