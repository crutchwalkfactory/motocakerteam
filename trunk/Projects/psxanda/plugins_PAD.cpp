#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "plugins.h"

//////////////////////////////////////////////////////////////////////////
// PAD 1
//////////////////////////////////////////////////////////////////////////

extern unsigned short EZXPSX_KEY;
void do_keypad();

//static unsigned char buf[256];
//static unsigned char stdpar[10] = { 0x00, 0x41, 0x5a, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
static unsigned char buf[10] = { 0x00, 0x41, 0x5a, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

static int bufcount, bufc;

long PAD1_open(void)
{
	return 0;
}

long PAD1_configure(void)
{
	return 0;
}

void PAD1_about(void)
{
}

long PAD1_init(long)
{
	bufc = 0;
	bufcount = 0;
	
	EZXPSX_KEY = 0xffff;
	buf[3] = 0xff;
	buf[4] = 0xff;
	
	return 0;
}

long PAD1_shutdown(void)
{
	return 0;
}

long PAD1_test(void)	
{
	return 0;
}

long PAD1_close(void)
{
	return 0;
}

long PAD1_query(void)
{
//	printf("PAD1_query();\n");
	return 3;
}

long PAD1_readPort1(PadDataS*)
{
	//printf("PAD1_readPort1\n");
	return 0;
}

long PAD1_readPort2(PadDataS*)
{
	//printf("PAD1_readPort2\n");
	return 0;
}

long PAD1_keypressed(void)
{
//	printf("PAD1_keypressed();\n");
	return 0;
}

unsigned char PAD1_startPoll(int pad)
{
//	printf("PAD1_startPoll(%d);\n", pad);
//	do_keypad();

	bufc = 0;
	buf[3] = EZXPSX_KEY >> 8;
	buf[4] = EZXPSX_KEY & 0xff;
//	memcpy(buf, stdpar, 5);
	bufcount = 4;
	
	return buf[bufc++];
}

unsigned char PAD1_poll(unsigned char value)
{
//	printf("PAD1_poll(%d);\n", value);
	if (bufc > bufcount) 
		return 0;
	return buf[bufc++];
}

void PAD1_setSensitive(int)
{
//	printf("PAD1_setSensitive\n");
}

//////////////////////////////////////////////////////////////////////////
// PAD 2
//////////////////////////////////////////////////////////////////////////

static unsigned char buf2[10] = { 0x00, 0x41, 0x5a, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
static int bufcount2, bufc2;

long PAD2_open(void)
{
	return 0;
}

long PAD2_configure(void)
{
	return 0;
}

void PAD2_about(void)
{
}

long PAD2_init(long)
{
	return 0;
}

long PAD2_shutdown(void)
{
	return 0;
}

long PAD2_test(void)
{
	return 0;
}

long PAD2_close(void)
{
	return 0;
}

long PAD2_query(void)
{
	return 3;
}

long PAD2_readPort1(PadDataS*)
{
	return 0;
}

long PAD2_readPort2(PadDataS*)
{
	return 0;
}

long PAD2_keypressed(void)
{
	return 0;
}

unsigned char PAD2_startPoll(int pad)
{
	//printf("PAD2_startPoll(%d);\n", pad);
	bufc2 = 0;
	bufcount2 = 4;
	return buf2[bufc2++];
}

unsigned char PAD2_poll(unsigned char value)
{
	//printf("PAD2_poll(%d);\n", value);
	if (bufc2 > bufcount2) 
		return 0;
	return buf2[bufc2++];	
}

void PAD2_setSensitive(int)
{

}


