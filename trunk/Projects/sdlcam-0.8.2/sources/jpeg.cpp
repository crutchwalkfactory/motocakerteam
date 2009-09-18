/* This say that these header Files are not always c++ but use c syntax (and function calles declared here */
extern "C" {
#include <stdio.h>
#include <jpeglib.h>
#include <string.h>
	}

#include "../SDL_v4l_image.h"
#include "../input.h"
#include "../misc.h"
#include "../dyn_misc.h"

/* Class of the Input source, always be parent of cInput or
   and differed class, example would be an scaleable jpeg as (class cScaleJpeg : public cInput)
   */
class cJpeg : public cInput {
	private:
	/* Internal data used by this class,
	   Depends on what the source "staticly" use
	   */
	char		*name;
	unsigned int	 len,w,h;
	unsigned char	*data;
	SDL_v4l_image   *image;
        public:
	/* Constructor initialise this source,
	   (open files, allocate mem ... ) 
	   Always called the same way as the class name
	 */
			 cJpeg		(const char *name);
	/* Destructor of this Source,
	   clean up memory, filehandles etc,
	   if we close (destroy) this source
	   Always called the same way as the class name with ~ prefix
	   */
			~cJpeg		(void);
	/* The name of this source (Displayed in the title Line */
virtual const char      *Name           (void                           ) { return name;  }
	/* Return if this input stream is warkable
	   in this case we alny once fetch the picture,
	   data != NULL mean we have an picture,
	   in other case maybe FD!=-1
	   */
virtual bool             isOK           (void                           ) { return data != NULL;  }
	/* Try to set an an Selected resolution,
	   return 0/1 if the solution was set or not
	   */
virtual int              Resolution     (int  W, int  H                 ) { return image->w==W && image->h==H;  }
	/* Get the aktual Resolution of this source,
	   Should always read after setting the Resolution
	   since many source do not support any Resolution
	   */
virtual int              Resolution     (int *W, int *H                 ) { *W = image->w; *H = image->h; return 1; }
	/* Get the Next Image from this source */
virtual SDL_v4l_image   *Fetch          (void                           );
	/* Set an Parameter NAME to Value VAL
	   if this Parameter is not handeld by this class
	   return -1000000
	 */
virtual int              Parameter      (const char *name, int val      ) { return -1000000; }
	/* Get the Value of Parameter NAME
	   if not handeld Parameter return -1000000
	   Example ("brightness", "frenqenzy" ... )
	   */
virtual int              Parameter      (const char *name               ) { return -1000000; }
        };

/* Here come the previus declared Function's
   it is nicer, if only short single line functions included in the class definiton
   make it more readable.
   */
cJpeg::~cJpeg (void) {{{
	if (data != NULL) free (data);
	if (name != NULL) free (name);
	if (image != NULL) {
		if (image->data != NULL) free (image->data);
		free (image);
		}
	}}}
SDL_v4l_image *cJpeg::Fetch (void ) {{{
	if (image->len != this->len) {
		if (image->data != NULL) free (image->data);
		image->data = (unsigned char*) xMalloc (len);
		image->len  = len;
		}
	image->w        = w;
	image->pitch    = w * 4;
	image->h        = h;
	image->inverted = 0;
	memcpy (image->data, data, len);
	return image; 
	}}}
cJpeg::cJpeg (const char *filename) {{{
	FILE *fp = fopen (filename, "r");
	image = (SDL_v4l_image*) xMalloc (sizeof(SDL_v4l_image));
	if (fp == NULL) {
		name = xStrdup ("none");
		data = NULL;
		image->data = NULL;
		image->w = 0; 
		image->h = 0;
		return;
		}
	name = xStrdup (filename);
	/* Init jpeg */
	struct jpeg_error_mgr jerr;
	struct jpeg_decompress_struct cinfo;
	cinfo.err = jpeg_std_error(&jerr);

	jpeg_create_decompress	(&cinfo);
	jpeg_stdio_src		(&cinfo, fp);
	jpeg_read_header	(&cinfo, TRUE);
	cinfo.out_color_space = JCS_RGB;
	jpeg_start_decompress	(&cinfo);

	image->w = w = cinfo.output_width;
	image->h = h = cinfo.output_height;

	image->len  = len = image->w * image->h * 4;

	       data = (unsigned char*) xMalloc (image->len);
	image->data = (unsigned char*) xMalloc (image->len);

	/* Loop */
	for (int r = 0; r < image->h; r++) {
		unsigned char *line = data + r * image->w * 4;
		JSAMPROW row = (JSAMPLE *)line;
		jpeg_read_scanlines(&cinfo, &row, 1);
		for (int x=image->w - 1; x > 0; x--) {
			line [x * 4 + 0] = line [x * 3 + 2];
			line [x * 4 + 1] = line [x * 3 + 1];
			line [x * 4 + 2] = line [x * 3 + 0];
			line [x * 4 + 3] = 0;
			}
		}

	/* Done jpeg */
	jpeg_destroy_decompress(&cinfo);
	fclose(fp);
	}}}

/* Here we export the constructor for these source,
   >C++ is badly handeld with dynamic linking (mean mangles function names)<
   The function take the name for the constructor and return an new input
   object. Since the class is Parent of cInput please do not extend the constructor
   instead set extra parameter later with Parameter.
   Because these object files are linked on request.
   */
   
extern "C" {
cInput * newInput (const char *name) {{{
	return new cJpeg (name);
	}}}
	}

/*
 * vim600: fdm=marker
 */
