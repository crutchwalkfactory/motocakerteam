#ifndef SDL_V4L_IMAGE_H
#define	SDL_V4L_IMAGE_H

#include <string.h>
#include <stdio.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _SDL_v4l_image 
{
  unsigned int w, h;
  int pitch; /* usually w*4 */
  unsigned long len; /* could be calculated by w*h*4 since the palete is always RGBR8-8-8-8, but is useful */
  unsigned char *data;
  char	inverted;
} SDL_v4l_image;

int SDL_v4l_Buffer_Prepare (const SDL_v4l_image *SRC, SDL_v4l_image *DST);
#ifdef __cplusplus
	}
#endif


#endif
