/*  Pcsx - Pc Psx Emulator
 *  Copyright (C) 1999-2003  Pcsx Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __PLUGINS_H__
#define __PLUGINS_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined (__WIN32__)
#include "Win32\plugin.h"z
#elif defined(__LINUX__) || defined(__MACOSX__)
typedef void* HWND;
#define CALLBACK  
//#include "Linux/Plugin.h"
#elif defined(__DREAMCAST__)
typedef void* HWND;
#define CALLBACK
#include "Dreamcast/Plugin.h"
#endif

#include "PSEmu_Plugin_Defs.h"
#include "Decode_XA.h"

int  LoadPlugins();
void ReleasePlugins();
int  OpenPlugins();
void ClosePlugins();

// GPU Functions

typedef struct {
	unsigned long ulFreezeVersion;
	unsigned long ulStatus;
	unsigned long ulControl[256];
	unsigned char psxVRam[1024*512*2];
} GPUFreeze_t;

long GPU_open(unsigned long *, char *, void *);
long GPU_init(void);
long GPU_shutdown(void);
long GPU_close(void);
void GPU_writeStatus(unsigned long);
void GPU_writeData(unsigned long);
void GPU_writeDataMem(unsigned long *, int);
unsigned long GPU_readStatus(void);
unsigned long GPU_readData(void);
void GPU_readDataMem(unsigned long *, int);
long GPU_dmaChain(unsigned long *,unsigned long);
void GPU_updateLace(void);
long GPU_configure(void);
long GPU_test(void);
void GPU_about(void);
void GPU_makeSnapshot(void);
void GPU_keypressed(int);
void GPU_displayText(char *);
long GPU_freeze(unsigned long, GPUFreeze_t *);
long GPU_getScreenPic(unsigned char *);
long GPU_showScreenPic(unsigned char *);
void GPU_clearDynarec(void (*callback)(void));

// CDR Functions

struct CdrStat {
	unsigned long Type;
	unsigned long Status;
	unsigned char Time[3];
};

struct SubQ {
	char res0[11];
	unsigned char ControlAndADR;
	unsigned char TrackNumber;
	unsigned char IndexNumber;
	unsigned char TrackRelativeAddress[3];
	unsigned char Filler;
	unsigned char AbsoluteAddress[3];
	char res1[72];
};

long CDR_init(void);
long CDR_shutdown(void);
long CDR_open(void);
long CDR_close(void);
long CDR_getTN(unsigned char *);
long CDR_getTD(unsigned char , unsigned char *);
long CDR_readTrack(unsigned char *);
unsigned char * CDR_getBuffer(void);
long CDR_configure(void);
long CDR_test(void);
void CDR_about(void);
long CDR_play(unsigned char *);
long CDR_stop(void);
long CDR_getStatus(struct CdrStat *);
char* CDR_getDriveLetter(void);
unsigned char* CDR_getBufferSub(void);

// SPU Functions

typedef struct {
	unsigned char PluginName[8];
	unsigned long PluginVersion;
	unsigned long Size;
	unsigned char SPUPorts[0x200];
	unsigned char SPURam[0x80000];
	xa_decode_t xa;
	unsigned char *SPUInfo;
} SPUFreeze_t;

long SPU_open(void);
long SPU_init(void);
long SPU_shutdown(void);
long SPU_close(void);
void SPU_playSample(unsigned char);
void SPU_startChannels1(unsigned short);
void SPU_startChannels2(unsigned short);
void SPU_stopChannels1(unsigned short);
void SPU_stopChannels2(unsigned short);
void SPU_putOne(unsigned long,unsigned short);
unsigned short SPU_getOne(unsigned long);
void SPU_setAddr(unsigned char, unsigned short);
void SPU_setPitch(unsigned char, unsigned short);
void SPU_setVolumeL(unsigned char, short );
void SPU_setVolumeR(unsigned char, short );
//psemu pro 2 functions from now..
void SPU_writeRegister(unsigned long, unsigned short);
unsigned short SPU_readRegister(unsigned long);
void SPU_writeDMA(unsigned short);
unsigned short SPU_readDMA(void);
void SPU_writeDMAMem(unsigned short *, int);
void SPU_readDMAMem(unsigned short *, int);
void SPU_playADPCMchannel(xa_decode_t *);
void SPU_registerCallback(void (*callback)(void));
long SPU_configure(void);
long SPU_test(void);
void SPU_about(void);
long SPU_freeze(unsigned long, SPUFreeze_t *);
void SPU_async(unsigned long);

// PAD Functions

long PAD1_open(void);
long PAD1_configure(void);
void PAD1_about(void);
long PAD1_init(long);
long PAD1_shutdown(void);	
long PAD1_test(void);		
long PAD1_close(void);
long PAD1_query(void);
long PAD1_readPort1(PadDataS*);
long PAD1_readPort2(PadDataS*);
long PAD1_keypressed(void);
unsigned char PAD1_startPoll(int);
unsigned char PAD1_poll(unsigned char);
void PAD1_setSensitive(int);

long PAD2_open(void);
long PAD2_configure(void);
void PAD2_about(void);
long PAD2_init(long);
long PAD2_shutdown(void);	
long PAD2_test(void);		
long PAD2_close(void);
long PAD2_query(void);
long PAD2_readPort1(PadDataS*);
long PAD2_readPort2(PadDataS*);
long PAD2_keypressed(void);
unsigned char PAD2_startPoll(int);
unsigned char PAD2_poll(unsigned char);
void PAD2_setSensitive(int);

// NET plugin

typedef long (* GPUshowScreenPic)(unsigned char *);
typedef void (* GPUdisplayText)(char *);
typedef void (* PADsetSensitive)(int);

typedef struct {
	char EmuName[32];
	char CdromID[9];	// ie. 'SCPH12345', no \0 trailing character
	char CdromLabel[11];
	void *psxMem;
	GPUshowScreenPic GPU_showScreenPic;
	GPUdisplayText GPU_displayText;
	PADsetSensitive PAD_setSensitive;
	char GPUpath[256];	// paths must be absolute
	char SPUpath[256];
	char CDRpath[256];
	char MCD1path[256];
	char MCD2path[256];
	char BIOSpath[256];	// 'HLE' for internal bios
	char Unused[1024];
} netInfo;

long NET_init(void);
long NET_shutdown(void);
long NET_close(void);
long NET_configure(void);
long NET_test(void);
void NET_about(void);
void NET_pause(void);
void NET_resume(void);
long NET_queryPlayer(void);
long NET_sendData(void *, int, int);
long NET_recvData(void *, int, int);
long NET_sendPadData(void *, int);
long NET_recvPadData(void *, int);
long NET_setInfo(netInfo *);
long NET_keypressed(int);



#ifdef __cplusplus
};
#endif

#endif /* __PLUGINS_H__ */
