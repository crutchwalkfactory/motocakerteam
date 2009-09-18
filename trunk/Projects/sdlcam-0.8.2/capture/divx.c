#include "../SDL_v4l_image.h"
#include "../dyn_misc.h"
#include <stdlib.h>

#include <stdio.h>
//#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <encore2.h>
#include <decore.h>

#define CHUNK_SIZE 1024

/*
 * parse command line
 */
static void *handle;
int init(int *size, int width, int height) {
	/** Create encoder **/
	ENC_PARAM param;
	memset(&param, 0, sizeof(param));
	param.x_dim	    = width;
	param.y_dim	    = height;
	param.framerate     = 20;
	param.min_quantizer = 4;
	param.max_quantizer = 4; 
	param.quality       = 1;   
	encore (0, ENC_OPT_INIT, &param, 0);
	handle = param.handle;
	*size = width * height * 6;
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
static unsigned char *divx_buffer;
static int ofile;
static SDL_v4l_image divx_image;

void Cleanup (void) {
  	if (running != 1) return;
	running = 2;
	encore(handle, ENC_OPT_RELEASE, 0, 0);
	free(divx_buffer);
	close(ofile);
	if (divx_image.data != NULL) free(divx_image.data);
	divx_image.data = NULL;
	}

int Apply (int mode, SDL_v4l_image *image) {
static int old_w;
static int old_h;

  int size;
  ENC_FRAME fr;
  ENC_RESULT res;


  if (running == 2 || mode == 0) return 0;

  if (running == 1 && (old_w != image->w || old_h != image->h)) {
	Cleanup ();
	return 0;
  	}

  if (running == 0) {
	char *file = getNewFilename (Dump_Path(), Dump_Prefix(), ".divx");
  	if(!init(&size, image->w, image->h)) return 0;
	ofile = open (file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	free (file);
	if(ofile<0) return 0;
  	divx_buffer = (unsigned char *) malloc(size);
	old_w   = image->w;
	old_h   = image->h;
	running = 1;
  	}
  Call_Filter_bb ("rgb2yuv"    , 1, image      , &divx_image);
  Call_Filter_bb ("yuv_yuv420p", 1, &divx_image, &divx_image);

  fr.colorspace = ENC_CSP_YV12;
  fr.mvs   = 0;
  fr.image = divx_image.data;
  fr.bitstream = divx_buffer;
  encore(handle, ENC_OPT_ENCODE, &fr, &res);
  
  Write(ofile, divx_buffer, fr.length);
  return 1;
}

static int   status = 0;

int   Type       (void ) { return 1; }                           // MANDATORY
int   Toggle     (void ) { return status = (status + 1) % 2; }   // RECOMENDED ( else always  ON    )
int   Mode_set   (int a) { return status = a % 2; }              // RECOMENDED ( else unknown STATE )
int   Mode_get   (void ) { return status; }                      // RECOMENDED ( else unknown STATE )
const  char *Name       (void ) { return "divx"; }               // MANDATORY
const  char *Display    (void ) { return "DivX-Dump"; }          // RECOMENDED ( else display filter name )

/*
 * vim600: fdm=marker
 */

