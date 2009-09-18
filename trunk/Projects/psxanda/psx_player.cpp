/*
 * Sony PlayStation Emulator for EZX Modile Phone
 * Copyright (C) 2006 OopsWare. CHINA.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: psx_player.cpp,v 0.01 2006/07/29 $
 */

#include <qapplication.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#include "psx_player.h"
#include "ezxaudio.h"
#include "font.h"

#include "PsxCommon.h"

long LoadCdBios = 0;

//static unsigned int ncd_keys = ~0;

struct keymap_item PSX_KEYMAP[] = {
		{"-----",		0,	false },
		{"START",		1,	false },
		{"SELECT",		2,	false },
		{"L-Analog",	3,	false },
		{"R-Analog",	4,	false },
		
		{"Triangle",	5,	false },
		{"Circle", 		6,	false },
		{"Cross",		7,	false },
		{"Square", 		8,	false },
		
		{"L2",			9,	false },
		{"R2",			10,	false },
		{"L1",			11,	false },
		{"R1",			12,	false }
		
//		{"LAnlaogX",	13,	false },
//		{"LAnlaogY",	14,	false },
//		{"RAnlaogX",	15,	false },
//		{"RAnlaogY",	16,	false }
	};

unsigned short EZXPSX_KEY = 0;

void do_keypad()
{
//	static unsigned int turbo = 0;
//	turbo ++;
	
	unsigned short keys = 0;
	
	if ( HKCS[EZX_KEY_UP].down ) 		keys |= 0x1000;
	if ( HKCS[EZX_KEY_DOWN].down ) 		keys |= 0x4000;
	if ( HKCS[EZX_KEY_LEFT].down ) 		keys |= 0x8000;
	if ( HKCS[EZX_KEY_RIGHT].down )		keys |= 0x2000;
	
	bool function_keys[13] = {false, false, false, false, false, false, false, 
							  false, false, false, false, false, false };
	int func_index;
	
	for (int i=0;i<10;i++) {
		func_index = GameKeymap[i];
		if ( func_index && HKCS[i].down) {
			function_keys[ PSX_KEYMAP[func_index].keypad ] = true;
//			if (HKCS[i].down && PSX_KEYMAP[func_index].turbo)
//				function_keys[ PSX_KEYMAP[func_index].keypad ] = turbo & 0x2;
		}
	}
	
	if ( function_keys[ 1] )	keys |= 0x0800;	// Start
	if ( function_keys[ 2] )	keys |= 0x0100; // Select
	if ( function_keys[ 3] )	keys |= 0x0200; // Left Analog
	if ( function_keys[ 4] )	keys |= 0x0400; // Right Analog
	
	if ( function_keys[ 5] )	keys |= 0x0010; // Triangle
	if ( function_keys[ 6] )	keys |= 0x0020; // Circle
	if ( function_keys[ 7] )	keys |= 0x0040; // Cross
	if ( function_keys[ 8] )	keys |= 0x0080; // Square
	
	if ( function_keys[ 9] )	keys |= 0x0001; // L2
	if ( function_keys[10] )	keys |= 0x0002; // R2
	if ( function_keys[11] )	keys |= 0x0004; // L1
	if ( function_keys[12] )	keys |= 0x0008; // R1

	EZXPSX_KEY = ~keys;
//	printf("PAD : 0x%04x\n", EZXPSX_KEY);
}


//extern QString AppPath;
extern QString SavePath;

/*
void now_loading()
{
	DrawString ("Now loading ...", (uint16 *)&VideoBuffer, 20, 200);
	wgGame->UpdateRect(20, 200, 120, 10);
}


void now_seeking(int m, int s, int f)
{
	char buf[50];
	
	sprintf(buf, "CDR %3d:%02d:%02d", m, s, f);
	DrawString (buf, (uint16 *)&VideoBuffer, 32, 200);
	
	wgGame->UpdateRect(32, 200, 120, 10);
}
*/

extern int BiosFile;	/* main.cpp */
extern char CDImageFileName[128];
extern char CDImageZFileName[128];
extern int CDImageIsCompressed;

void EZX_Flip()
{
	wgGame->UpdateRect(0, 0, 320, 240);
}

void run_psx_emulator(const char *fn, int rom_type)
{
	memset(CDImageFileName, 0, 128);
	
	if (rom_type != 1) sprintf(CDImageFileName, "%s", fn);
	
	switch (rom_type) {
	case 8: // .ZNX
		CDImageIsCompressed = 2;
		break;
	case 9: // .Z
		CDImageIsCompressed = 1;
		break;
	default:
		CDImageIsCompressed = 0;
	}
	
	if ( CDImageIsCompressed ) {
		sprintf(CDImageZFileName, "%s", CDImageFileName);
		sprintf(CDImageFileName, "%s.table", CDImageZFileName);
	}
	
//	printf("CD Image Type: %d\nFile Name:%s\nZ File Name: %s\n", CDImageIsCompressed, CDImageFileName, CDImageZFileName);

	wgGame = new PSX_GameWidget( 0 );
	wgGame->doLoadConfig();
	
	// Config system
	memset(&Config, 0, sizeof(Config));
	strcpy(Config.Net, "Disabled");
//	Config.PsxAuto = 1;
	QString AppPath = "/mmc/mmca1/psx/";
	sprintf(Config.Bios, "%s%s", (const char *)AppPath.local8Bit(), (BiosFile == 0) ? "scph1001.bin" : "scph7502.bin" );
	sprintf(Config.Mcd1, "%s/%s", (const char *)SavePath.local8Bit(), "Mcd001.mcr" );
	sprintf(Config.Mcd2, "%s/%s", (const char *)SavePath.local8Bit(), "Mcd002.mcr" );
//	Config.PsxType = 1;	// PAL

	Config.Xa = 1;
//	Config.Mdec = 1;
	
	
	//	ConfigurePlugins();
	LoadPlugins();
	LoadMcds(Config.Mcd1, Config.Mcd2);

	// Init CPU and System Memory ...
	SysMessage("Init PSX ...");
	if (psxInit() < 0) {
		printf("Error Init PSX ...\n");
		goto finish;
	}

//	LoadCdBios = 1;
	
	SysMessage("Init Plugins ...");
	if (OpenPlugins() < 0) {
		
		goto finish;
	}
	
	// reset cpu and memory, load BIOS file ...
	psxReset();

	switch (rom_type) {
	case 1:	// PSX-EXE file  .psx
		Load(fn);
		break;
	case 3: // .iso
		printf("This emulator does not support ISO-9660 images.\n");
		printf("If this is a binary image, rename it with a \".bin\" extension.\n");
	case 4: // .img
	case 2: // .bin
	case 8:	// .ZNX
	case 9:	// .Z
		
		CheckCdrom();
		if (LoadCdrom() == -1) {
			
			
			goto finish;
		}
		break;

	default: // BIOS 
		break;
	}
	wgGame->setFrameBufferDepth(16);
	wgGame->showFullScreen();
	wgGame->UpdateRect(0, 0, 320, 240, true);
	qApp->processEvents();

	SysMessage("Execute PSX ...");
	GameLooping = true;
	
//	psxCpu->Execute();
	
	while (GameLooping) {
		qApp->processEvents();
		if ( GamePauseing ) {
			wgGame->PauseProc();
			continue;
		}
		do_keypad();
		
		intExecuteCycs(1320000);
			
//		printf("DIV: %d  %d\n", my_div1_cnt, my_div2_cnt);
	}
	
finish:
	SysMessage("Shutdown PSX ...");
	
	ClosePlugins();
	psxShutdown();
	
	ReleasePlugins();
	
	/*if (wgGame) {
		delete wgGame;
		wgGame = 0;
	}
	*/
	//SysMessage("");
	 wgGame->setFrameBufferDepth(24);
}

void PSX_GameWidget::doGamePause(bool pause)
{
//qWarning("pause: %d\n", pause);
	if ( pause ) {
//		StopTimer ();
	} else {
//		InitTimer ();
	}
}

bool PSX_GameWidget::doGameReset()
{
	psxReset();
	return true;
}

bool PSX_GameWidget::doSaveGame(const char * fn)
{
	return SaveState(fn) == 0;
}

bool PSX_GameWidget::doLoadGame(const char * fn)
{
	return LoadState(fn) == 0;
//	return false;
}

QString PSX_GameWidget::doConfigItemPerfix(const QString &s) const
{
	return "PSX_" + s;
}

int PSX_GameWidget::getKeymapCount()
{
	return sizeof(PSX_KEYMAP) / sizeof(struct keymap_item);
}

struct keymap_item * PSX_GameWidget::getKeymapData()
{
	return &PSX_KEYMAP[0];
}


// ------------------------------------------------------------------------------

void SysPrintf(char *fmt, ...) {
	va_list list;
	char msg[512];

	va_start(list, fmt);
	vsprintf(msg, fmt, list);
	va_end(list);

	printf ("%s\n", msg);
}

void SysMessage(char *fmt, ...) {
	va_list list;
	char msg[512];

	va_start(list, fmt);
	vsprintf(msg, fmt, list);
	va_end(list);

	printf ("%s\n", msg);
}

void SysUpdate() {
//	PADhandleKey(PAD1_keypressed());
//	PADhandleKey(PAD2_keypressed());
}

void SysRunGui() {
//	RunGui();
}


