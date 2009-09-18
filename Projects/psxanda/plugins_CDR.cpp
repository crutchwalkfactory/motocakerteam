#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "plugins.h"

#include <zlib.h>

////////////////////////////////////////////////////////////////////////
// CD Frames
////////////////////////////////////////////////////////////////////////
#define bytesPerFrame				2352
#define secondsPerMinute			60
#define framesPerSecond				75

#define bytesPerSecond				(framesPerSecond * bytesPerFrame)
#define bytesPerMinute				(secondsPerMinute * bytesPerSecond)

//#define UncompressedBufferFrames	10

////////////////////////////////////////////////////////////////////////
// CD Time
////////////////////////////////////////////////////////////////////////
typedef struct {
	long minute;
	long sec;
	long frame;
} CDTime;


////////////////////////////////////////////////////////////////////////
// BCD Code functions
////////////////////////////////////////////////////////////////////////

#include "BCDCoder.h"

// converts uchar in c to BCD character
//#define intToBCD(c) (unsigned char)((c%10) | ((c/10)<<4))
// converts BCD number in c to uchar
//#define BCDToInt(c) (unsigned char)((c & 0x0F) + 10 * ((c & 0xF0) >> 4))

////////////////////////////////////////////////////////////////////////
// Globals
////////////////////////////////////////////////////////////////////////

char CDImageFileName[128];
char CDImageZFileName[128];
int CDImageIsCompressed = 0;

unsigned char CDImageData[framesPerSecond][bytesPerFrame];
unsigned char SubchannelFrameData[98];

static FILE * img = 0;
static FILE * zimg = 0;

static CDTime CDSeek;
static CDTime CDLength;

static long CDAbsoluteByte = 0;

////////////////////////////////////////////////////////////////////////
// CDR Functions
////////////////////////////////////////////////////////////////////////

long CDR_init(void)
{
	memset(SubchannelFrameData, 0, sizeof(SubchannelFrameData));

	CDSeek.minute	= 99;
	CDSeek.sec		= 59;
	CDSeek.frame	= 74;

	return 0;
}

long CDR_shutdown(void)
{
	return 0;
}

long CDR_open(void)
{
	if (CDImageFileName[0] == 0)
		return 0;
	
	if (img) fclose(img);
	if (zimg) {
		fclose(zimg);
		zimg = 0;
	}
	
	img = fopen( CDImageFileName, "rb" );
	if (!img) {
		printf("CDR: Error Open %s\n", CDImageFileName);
		return -1;
	}

	if (CDImageIsCompressed) {
		if (zimg) fclose(zimg);
		zimg = fopen( CDImageZFileName, "rb" );
		if (!zimg) {
			printf("CDR: Error Open %s\n", CDImageZFileName);
			return -1;
		}
	}
	
	// CDLength= CDTime(file.seekg(0, std::ios::end).tellg(), CDTime::abByte) + CDTime(0,2,0);
	fseek(img, 0, SEEK_END);
	CDAbsoluteByte = ftell(img);
	if (CDAbsoluteByte <= 0) {
		CDAbsoluteByte = 0;
		return -1;
	}
	
	if (CDImageIsCompressed == 1) {
		CDAbsoluteByte /= 6;
		CDAbsoluteByte *= bytesPerFrame;
	} else
	if (CDImageIsCompressed == 2) {
		CDAbsoluteByte /= 10;
		CDAbsoluteByte *= bytesPerFrame;
	}
	
	CDLength.minute = CDAbsoluteByte / bytesPerMinute;
	CDLength.sec	= (CDAbsoluteByte - CDLength.minute * bytesPerMinute) / bytesPerSecond;
	CDLength.frame	= (CDAbsoluteByte - CDLength.minute * bytesPerMinute - CDLength.sec * bytesPerSecond) / bytesPerFrame;
	
	CDLength.sec	+= 2;
	if (CDLength.sec >= secondsPerMinute) {
		CDLength.sec -= secondsPerMinute;
		CDLength.minute ++;
	}

printf("CDR: Image File: %d:%02d:%02d  %dBytes\n", (int)CDLength.minute, (int)CDLength.sec, (int)CDLength.frame, (int)CDAbsoluteByte);
	
	CDSeek.minute	= 99;
	CDSeek.sec		= 59;
	CDSeek.frame	= 74;
	
	return 0;
}

long CDR_close(void)
{
	if (img) {
		fclose(img);
		img = 0;
	}
	if (zimg) {
		fclose(zimg);
		zimg = 0;
	}
	return 0;
}

//void now_seeking(int m, int s, int f);

static unsigned char zbuf[4096]; 

long CDRseek(long m, long s, long f)
{
	if ((CDSeek.minute != m) || (CDSeek.sec != s)) {
printf("CDR: Seek: %3d:%02d.%02d\n", (int)m, (int)s, (int)f);		
//		now_seeking((int)m, (int)s, (int)f);
	}

	if (CDImageIsCompressed) {

		if ((CDSeek.minute != m) || (CDSeek.sec != s) || (CDSeek.frame != f)) {
			int zpos;
			short zlen;
			
			if (CDImageIsCompressed == 2) // .ZNX
				fseek( img, (((m * secondsPerMinute) + s) * framesPerSecond + f) * 10, SEEK_SET );
			else                          // .Z
				fseek( img, (((m * secondsPerMinute) + s) * framesPerSecond + f) * 6, SEEK_SET );
			
			fread( &zpos, 1, 4, img );
			fread( &zlen, 1, 2, img );
			
			//int zres;
			//fread( &zres, 1, 4, img );
			
			fseek( zimg, zpos, SEEK_SET );
			fread( &zbuf[0], 1, zlen, zimg );
			
			unsigned long unzlen = bytesPerFrame;
			int rc = uncompress( (unsigned char *)&CDImageData[f][0], &unzlen, (unsigned char *)&zbuf[0], zlen );
			if (rc != 0)
				printf("CDR: Uncompress Error at %3d:%02d.%02d rc = %d\n", (int)m, (int)s, (int)f, rc);
		}
	
	} else {

		if ((CDSeek.minute != m) || (CDSeek.sec != s)) {
			// Chech CDSeek > CDLength ???
			fseek( img, m * bytesPerMinute + s * bytesPerSecond, SEEK_SET );
			fread( CDImageData, 1, bytesPerFrame * framesPerSecond, img );
		}
	
	}
	
	CDSeek.minute	= m;
	CDSeek.sec		= s;
	CDSeek.frame	= f;

	return 0;
}

long CDR_getTN(unsigned char *tn)
{
//	printf("CDR_getTN(tn);\n");
	tn[0] = 1;
	tn[1] = intToBCD(1);	// intToBCD((char)theCD->getNumTracks());
	return 0;
}

long CDR_getTD(unsigned char td, unsigned char *dtd)
{
//	printf("CDR_getTD(%d);\n", (int)td);

	switch ( td ) {
	case 0:	// hole disk length
		dtd[0] = intToBCD( CDLength.minute );
		dtd[1] = intToBCD( CDLength.sec );
		dtd[2] = intToBCD( CDLength.frame );
		break;
	case 1: // first track start 
		dtd[0] = 0;
		dtd[1] = 2;
		dtd[2] = 0;
		break;
	}
	return 0;
}

long CDR_readTrack(unsigned char *tr)
{
	int m = BCDToInt( tr[0] );
	int s = BCDToInt( tr[1] );
	int f = BCDToInt( tr[2] );
	
	s -= 2;
	if (s < 0) {
		s += secondsPerMinute;
		m --;
	}
//	printf("CDR_readTrack(%d:%02d:%02d); 0x%08x\n", m, s, f, m * bytesPerMinute + s * bytesPerSecond + f * bytesPerFrame);

	CDRseek(m, s, f);
	return 0;
}

unsigned char * CDR_getBuffer(void)
{
//	printf("CDR_getBuffer(%d:%02d:%02d);\n", CDSeek.minute, CDSeek.sec, CDSeek.frame);
	return (&CDImageData[ CDSeek.frame ][12]);
}

long CDR_configure(void)
{
	return 0;
}

long CDR_test(void)
{
	return 0;
}

void CDR_about(void)
{
}

long CDR_play(unsigned char * /*tr*/)
{
	return 0;
}

long CDR_stop(void)
{
	return 0;
}

long CDR_getStatus(struct CdrStat *st)
{
//	printf("CDR_getStatus();\n");
	
	st->Type   = 0x01;
	st->Status = 0x20;

	st->Time[0] = intToBCD( CDSeek.minute );
	st->Time[1] = intToBCD( CDSeek.sec );
	st->Time[2] = intToBCD( CDSeek.frame );

	return 0;
}

char* CDR_getDriveLetter(void)
{
	//printf("CDR_getDriveLetter\n");	
	return 0;
}

unsigned char* CDR_getBufferSub(void)
{
//	printf("CDR_getBufferSub();\n");
//	return &SubchannelFrameData[0];
	return 0;
}



