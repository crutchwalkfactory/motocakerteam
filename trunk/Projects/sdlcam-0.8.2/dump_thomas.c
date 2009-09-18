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

#include <stdlib.h>
#include <stdio.h>

#include <png.h>

#include "dump_thomas.h"
#include "misc.h"
#include "dyn_misc.h"


#ifdef __cplusplus
extern "C" {
#endif
#include <jpeglib.h>

int write_file_jpeg (char *file_name, unsigned char *data, int width, int height) {{{
  struct jpeg_compress_struct jpeg;
  struct jpeg_error_mgr jpeg_err;
  int i;
  unsigned char *line;
  FILE *fp = fopen(file_name,"w");
  if (fp==NULL) { return -1; }
  
  jpeg.err 		= jpeg_std_error(&jpeg_err);
  jpeg_create_compress (&jpeg);
  jpeg_stdio_dest      (&jpeg, fp);
  jpeg.in_color_space   = JCS_RGB;
  jpeg.image_width      = width;
  jpeg.image_height     = height;
  jpeg.input_components = 3;
  jpeg.dct_method	= JDCT_FLOAT;
  jpeg_set_defaults    (&jpeg);
  jpeg_set_quality     (&jpeg, 85, TRUE);	// solte in die config
  jpeg_start_compress  (&jpeg, TRUE);
  for (i = 0, line = data; i < height; i++, line += width*3)
    jpeg_write_scanlines(&jpeg, &line, 1);
  jpeg_finish_compress(&(jpeg));
  jpeg_destroy_compress(&(jpeg));
  fclose(fp);

  return 0;
}}}
int write_file_png (char *file_name, unsigned char *data, unsigned int width, unsigned int height) {{{
  FILE *fp;
  size_t i;
  png_info *info_ptr;
  png_struct *png_ptr;
  png_byte **bitmap;
  png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) 
    return (1);
  info_ptr = png_create_info_struct (png_ptr);
  if (info_ptr == NULL) 
    {
      png_destroy_write_struct (&png_ptr, (png_infopp)NULL);
      return (1);
    }
  bitmap = (png_byte **)xMalloc(height*sizeof(png_byte *));
  for (i = 0; i < height; i++)
    bitmap[i] = (png_byte *)data + (i*3*width);
  fp = fopen(file_name,"w");
  if (fp==NULL) 
  { 
	  png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	  free(bitmap); return -1; 
  }
  
  png_init_io 		  (png_ptr, fp);
  png_set_IHDR		  (png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB , PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT); 
  png_write_info	  (png_ptr, info_ptr);
  png_write_image	  (png_ptr, bitmap);
  png_write_end		  (png_ptr, NULL); 
  png_destroy_write_struct(&png_ptr, &info_ptr);
  free(bitmap);
  fclose(fp);
  return (0);
}}}

#ifdef __cplusplus
	}
#endif

/*
 * vim600: fdm=marker
 */
