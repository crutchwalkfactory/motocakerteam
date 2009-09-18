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
	int idx;
	int height= src->h;
	int width = src->w;
	int len=height*width;
	int Y_OFF=0;
	int U_OFF=len;
	int V_OFF=len+len/4;
	unsigned char *imageIn = src->data;
	unsigned char *imageOut = NULL;
	if (mode == 0) return 0;
	SDL_v4l_Buffer_Prepare (src, dst);
	if(src->data == dst->data)
	  imageOut = (unsigned char*)xMalloc (dst->len * sizeof (unsigned char));
	else imageOut = dst->data;
	if (! imageOut)
	  return (0);
	for(idx = 0; idx < len; idx++) imageOut[idx]=imageIn[idx*4+0];
	idx=0;
	for(r = 0; r+1<height; r+=2)
	  for(c = 0; c+1 < width; c+=2) {
	    idx++;
	    imageOut[U_OFF+idx]=(imageIn[(r*width+c)*4+1]+imageIn[(r*width+(c+1))*4+1]+imageIn[((r+1)*width+c)*4+1]+imageIn[((r+1)*width+(c+1))*4+1])>>2;
	    imageOut[V_OFF+idx]=(imageIn[(r*width+c)*4+2]+imageIn[(r*width+(c+1))*4+2]+imageIn[((r+1)*width+c)*4+2]+imageIn[((r+1)*width+(c+1))*4+2])>>2;
	    }
	memset(imageOut+len+len/2,0,len*2+len/2);
  	if(src->data == dst->data)
  	  {
       	    memcpy (dst->data, imageOut, dst->len);
       	    free (imageOut);
    	  }
  return (1);
  }}}

static int   status = 0;

       int   Type   	(void ) { return 0; status = 0;}		// MANDATORY
       int   Toggle 	(void ) { return status = (status + 1) % 2; }	// RECOMENDED ( else always  ON    )
       int   Mode_set  	(int a) { return status = a % 2; }		// RECOMENDED ( else unknown STATE )
       int   Mode_get  	(void ) { return status; }			// RECOMENDED ( else unknown STATE )
const  char *Name	(void ) { return "yuv_yuv420p"; }		// MANDATORY
const  char *Display	(void ) { return "yuv_yuv420p"; }               // RECOMENDED ( else display filter name )

/*
 * vim600: fdm=marker
 */
