#include <avifile/formats.h>
#include <avifile/avifile.h>
#include <avifile/version.h>
#include <avifile/videoencoder.h>
#include <avifile/creators.h>
#include <avifile/aviplay.h>
#include <avifile/fourcc.h>
#include <avifile/except.h>

#include "../SDL_v4l_image.h"
#include "../dyn_misc.h"

#include <string.h>		// for memset()
#include <unistd.h>
#include <stdio.h>
//#include <sys/types.h>
#include <assert.h>


static int running = 0;
static int status  = 0;
BITMAPINFOHEADER bi;
avm::IWriteFile	  	*write_file  ;
avm::IVideoWriteStream 	*video_stream;

void Cleanup (void) {
	if (running != 1) return;
	video_stream->Stop();
	delete write_file;
printf("stop divx2\n");
	}

unsigned char BUFFER[640*480*4];

extern "C" {
int Apply (int mode, SDL_v4l_image *image) {
static int old_w, old_h;
	if (running == 2 || mode == 0) return 0;
	if (running == 0) {
		char *data;
    		int fps = 5;			// default to 5 fps
    		int quality = 10000; 	// units = .01 percent
		char *file = getNewFilename (Dump_Path(), Dump_Prefix(), ".avi");
		if ( GetAvifileVersion() != AVIFILE_VERSION) {
			printf("Wrong avi version\n");
			return 0;	// Check for right version
			}
		BITMAPINFOHEADER bih;
		bih.biCompression = 0xffffffff;
		Creators::CreateVideoDecoder(bih, 0, 0);

    		memset(&bi, 0, sizeof(bi));
		bi.biSize	= sizeof(bi);
    		bi.biWidth  	= image->w;
    		bi.biHeight	= image->h;
    		bi.biPlanes 	= 1;
    		bi.biBitCount 	= 32;
		bi.biCompression= BI_RGB;
    		bi.biSizeImage	= image->w * image->h * 4;
		write_file   = avm::CreateWriteFile (file);
		assert (write_file != NULL);
		free (file);
		video_stream = write_file->AddVideoStream(fccDIV3, &bi, fps);
		assert (video_stream != NULL);
		video_stream->Start();
		video_stream->SetQuality(quality);
		old_w = image->w;
		old_h = image->h;
		running = 1;
		}
	if (running == 1 && (old_w != image->w || old_h != image->h)) {
		Cleanup();
		running = 2;
		return 0;
		}
	memcpy(BUFFER,image->data,image->w*image->h*4);
	CImage frame((BitmapInfo*)&bi, BUFFER, false);
	uint_t size;
	int key_frame;
	video_stream->AddFrame(&frame, &size, &key_frame);
	return 1;
	}
int   Type       	(void ) { return 1; }
const char *Name 	(void ) { return "divx2"; }
const char *Display	(void ) { return "DivX-Dump"; }
}

