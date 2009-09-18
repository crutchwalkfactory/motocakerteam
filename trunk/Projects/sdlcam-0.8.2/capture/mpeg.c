#include "../SDL_v4l_image.h"
#include "../dyn_misc.h"
#include <stdlib.h>

#include <fcntl.h>
#include <errno.h>
#include <fame.h>

#define DEFAULT_BUFFER_SIZE 1024*1024
#define CHUNK_SIZE 1024

/*
 * parse command line
 */
int init(fame_context_t *fc, fame_parameters_t *p, int *buffer_size, int width, int height) {
	fame_object_t *object = NULL;
	/* picture size: widthxheight or CIF,SIF,QCIF,QSIF */
  	p->width   = width;
  	p->height  = height;
  	p->quality =  95;	/* 100% */

  	p->frame_rate_num =  25; /* frames */
  	p->frame_rate_den =  1;	 /* fps = frame_rate_num / frame_rate_den */
//  	p->coding  = strdup("IBBPBBPBB");
  	p->verbose = 0;
  	*buffer_size = DEFAULT_BUFFER_SIZE;
//	p->frames_per_sequence = 20;	/* refresh: number of frames before a new sequence */
//	p->shape_quality = 100; /* 100% */
//	p->search_range = 10;		/* search range: motion estimation range */
	
      	switch (0) {	// motion: motion estimation algorithm
		default : object = NULL; break;
		case  1 : object = fame_get_object(fc, "motion/none"    ); break;
		case  2 : object = fame_get_object(fc, "motion/pmvfast" ); break;
		case  3 : object = fame_get_object(fc, "motion/fourstep"); break;
		}
	if(object) fame_register(fc, "motion", object);
	
	switch(1) {	// profile: name of the default profile to use
		default : object = NULL; break;
		case  1 : object = fame_get_object(fc, "profile/mpeg1");	break;
		case  2 : object = fame_get_object(fc, "profile/mpeg4");	break;
		case  3 : object = fame_get_object(fc, "profile/mpeg4/simple"); break;
		case  4 : object = fame_get_object(fc, "profile/mpeg4/shape");  break;
		}
	if(object) fame_register(fc, "profile", object);

  	return 1;
}

void Write(int fd,char *buf,int size) {
	int len=1;
	if(size<=0) return;
	do {
		len = write(fd,buf,1024);
		if(len>0) {
			buf +=len;
			size-=len;
			}
		else	{
			printf("Errno %s\n", strerror (errno));
			}
		} while(size>1024 &&len>0);
	if(size>0) len = write(fd,buf,size);
	}

static int running = 0;
static fame_parameters_t fp = FAME_PARAMETERS_INITIALIZER;
static fame_context_t *fc;
static fame_yuv_t yuv;
static unsigned char *mpeg_buffer;
static int ofile;
static SDL_v4l_image mpeg_image;

void Cleanup (void) {
  	if (running != 1) return;
  	Write(ofile, mpeg_buffer, fame_close(fc));
	running = 2;
	free(mpeg_buffer);
	close(ofile);
	if (mpeg_image.data != NULL) free(mpeg_image.data);
	mpeg_image.data = NULL;
	}

int Apply (int mode, SDL_v4l_image *image) {
static int old_w;
static int old_h;
  int size;
  if (running == 2 || mode == 0) return 0;

  if (running == 1 && (old_w != image->w || old_h != image->h)) {
	Cleanup ();
	return 0;
  	}
  Call_Filter_bb ("rgb2yuv"    , 1, image      , &mpeg_image);
  Call_Filter_bb ("yuv_yuv420p", 1, &mpeg_image, &mpeg_image);

  if (running == 0) {
	char *file = getNewFilename (Dump_Path(), Dump_Prefix(), ".mpeg");
  	fc = fame_open();
  	if (!init (fc, &fp, &size, image->w, image->h)) return 0;
	ofile = open (file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	free (file);
	if (ofile < 0) return 0;
  	yuv.y = mpeg_image.data;
  	yuv.u = yuv.y + fp.width * fp.height;
  	yuv.v = yuv.u + fp.width * fp.height / 4;
  	mpeg_buffer = (unsigned char *) malloc(size);
  	fame_init (fc, &fp, mpeg_buffer, size);
	old_w   = mpeg_image.w;
	old_h   = mpeg_image.h;
	running = 1;
  	}
  fame_start_frame (fc, &yuv, NULL);
  while (size = fame_encode_slice (fc)) 
	  Write(ofile, mpeg_buffer, size);
//  Write(ofile, mpeg_buffer, fame_encode_frame(fc, &yuv, NULL));
  return 1;
}

static int   status = 0;

int   Type       (void ) { return 1; }                           // MANDATORY
int   Toggle     (void ) { return status = (status + 1) % 2; }   // RECOMENDED ( else always  ON    )
int   Mode_set   (int a) { return status = a % 2; }              // RECOMENDED ( else unknown STATE )
int   Mode_get   (void ) { return status; }                      // RECOMENDED ( else unknown STATE )
const  char *Name       (void ) { return "mpeg"; }               // MANDATORY
const  char *Display    (void ) { return "Mpeg-Dump"; }          // RECOMENDED ( else display filter name )

/*
 * vim600: fdm=marker
 */

 
