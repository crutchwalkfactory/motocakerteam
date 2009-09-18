#include "../SDL_v4l_image.h"
#include "../dyn_misc.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
int Apply(int mode,const SDL_v4l_image *src,SDL_v4l_image *dst) {{{
	/* edge detection */
	int r, c;
	int width = src->w;
	unsigned char *imageIn = src->data;
	unsigned char *imageOut = NULL;
	if (mode == 0) return 0;
	if(src->data == dst->data)
	  imageOut = (unsigned char*)xMalloc (src->len * sizeof (unsigned char));
	else imageOut = dst->data;
	if (! imageOut)
	  return (0);
printf("Highpass\n");
	for (r = width; (r+2*width)*4 < (src->len) ; r+= width) 
	  for (c = 1; c + 1 < width; c++) 
	    {
	      int temp[4];
	      temp[0] =	+ imageIn[(r-width + c-1)*4+0] +     imageIn[(r-width + c)*4+0] + imageIn[(r-width+c+1)*4+0]
		     	+ imageIn[(r       + c-1)*4+0] - 8 * imageIn[(r       + c)*4+0]	+ imageIn[(r      +c+1)*4+0]
			+ imageIn[(r+width + c-1)*4+0] +     imageIn[(r+width + c)*4+0] + imageIn[(r+width+c+1)*4+0];
	      temp[1] =	+ imageIn[(r-width + c-1)*4+1] +     imageIn[(r-width + c)*4+1] + imageIn[(r-width+c+1)*4+1]
		  	+ imageIn[(r       + c-1)*4+1] - 8 * imageIn[(r       + c)*4+1]	+ imageIn[(r      +c+1)*4+1]
			+ imageIn[(r+width + c-1)*4+1] +     imageIn[(r+width + c)*4+1] + imageIn[(r+width+c+1)*4+1];
	      temp[2] = + imageIn[(r-width + c-1)*4+2] +     imageIn[(r-width + c)*4+2] + imageIn[(r-width+c+1)*4+2]
			+ imageIn[(r       + c-1)*4+2] - 8 * imageIn[(r       + c)*4+2]	+ imageIn[(r      +c+1)*4+2]
			+ imageIn[(r+width + c-1)*4+2] +     imageIn[(r+width + c)*4+2] + imageIn[(r+width+c+1)*4+2];

	      if ( temp[0] > 0) imageOut[(c + r)*4+0] = 0;
	      else	       	imageOut[(c + r)*4+0] = temp[0]; 
	      if ( temp[1] > 0) imageOut[(c + r)*4+1] = 0;
	      else	     	imageOut[(c + r)*4+1] = temp[1];
	      if ( temp[2] > 0) imageOut[(c + r)*4+2] = 0;
	      else		imageOut[(c + r)*4+2] = temp[2];
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
const  char *Name	(void ) { return "highpass"; }			// MANDATORY
const  char *Display	(void ) { return "Highpass"; }                  // RECOMENDED ( else display filter name )

/*
 * vim600: fdm=marker
 */
