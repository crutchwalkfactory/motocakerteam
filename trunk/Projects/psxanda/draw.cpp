/***************************************************************************
                          draw.c  -  description
                             -------------------
    begin                : Sun Oct 28 2001
    copyright            : (C) 2001 by Pete Bernert
    email                : BlackDove@addcom.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version. See also the license.txt file for *
 *   additional informations.                                              *
 *                                                                         *
 ***************************************************************************/

//*************************************************************************//
// History of changes:
//
// 2005/06/11 - MxC
// - added HQ2X,HQ3X,Scale3X screen filters
//
// 2004/01/31 - Pete  
// - added zn stuff
//
// 2003/01/31 - stsp  
// - added zn stuff
//
// 2003/12/30 - Stefan Sperling <stsp@guerila.com>
// - improved XF86VM fullscreen switching a little (refresh frequency issues).
//
// 2002/12/30 - Pete
// - added Scale2x display mode - Scale2x (C) 2002 Andrea Mazzoleni - http://scale2x.sourceforge.net
//
// 2002/12/29 - Pete
// - added gun cursor display
//
// 2002/12/21 - linuzappz
// - some more messages for DGA2 errors
// - improved XStretch funcs a little
// - fixed non-streched modes for DGA2
//
// 2002/11/10 - linuzappz
// - fixed 5bit masks for 2xSai/etc2006-9-21 10:57ÉÏÎç
//
// 2002/11/06 - Pete
// - added 2xSai, Super2xSaI, SuperEagle
//
// 2002/08/09 - linuzappz
// - added DrawString calls for DGA2 (FPS display)
//
// 2002/03/10 - lu
// - Initial SDL-only blitting function 
// - Initial SDL stretch function (using an undocumented SDL 1.2 func)
// - Boht are triggered by -D_SDL -D_SDL2
//
// 2002/02/18 - linuzappz
// - NoStretch, PIC and Scanlines support for DGA2 (32bit modes untested)
// - Fixed PIC colors in CreatePic for 16/15 bit modes
//
// 2002/02/17 - linuzappz
// - Added DGA2 support, support only with no strecthing disabled (also no FPS display)
//
// 2002/01/13 - linuzappz
// - Added timing for the szDebugText (to 2 secs)
//
// 2002/01/05 - Pete
// - fixed linux stretch centering (no more garbled screens)
//
// 2001/12/30 - Pete
// - Added linux fullscreen desktop switching (non-SDL version, define USE_XF86VM in Makefile)
//
// 2001/12/19 - syo
// - support refresh rate change
// - added  wait VSYNC
//
// 2001/12/16 - Pete
// - Added Windows FPSE RGB24 mode switch
//
// 2001/12/05 - syo (syo68k@geocities.co.jp)
// - modified for "Use system memory" option
//   (Pete: fixed "system memory" save state pic surface)
//
// 2001/11/11 - lu
// - SDL additions
//
// 2001/10/28 - Pete
// - generic cleanup for the Peops release
//
//*************************************************************************//

//#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define _IN_DRAW

#include "GPU_externals.h"
#include "gpu.h"
#include "draw.h"
#include "prim.h"

//#include "menu.h"
//#include "interp.h"

////////////////////////////////////////////////////////////////////////////////////
// misc globals
////////////////////////////////////////////////////////////////////////////////////

int            iResX;
int            iResY;
long           lLowerpart;
BOOL           bIsFirstFrame = TRUE;
BOOL           bCheckMask=FALSE;
unsigned short sSetMask=0;
unsigned long  lSetMask=0;
int            iDesktopCol=16;
int            iShowFPS=0;
int            iWinSize; 
int            iUseScanLines=0;
int            iUseNoStretchBlt=0;
int            iFastFwd=0;
int            iDebugMode=0;
int            iFVDisplay=0;
PSXPoint_t     ptCursorPoint[8];
unsigned short usCursorActive=0;

//unsigned int   LUT16to32[65536];
//unsigned int   RGBtoYUV[65536];

#define USE_FAST_COLOR_TABLE 1

#if USE_FAST_COLOR_TABLE
unsigned short RGB15toBGR16[65536];
#endif

////////////////////////////////////////////////////////////////////////
// SDL Stuff ^^
////////////////////////////////////////////////////////////////////////

int           Xpitch, depth=32;
char *        Xpixels;
char *        pCaptionText;

//SDL_Surface *display;
extern unsigned short VideoBuffer [240][320];	/* gamewidget.cpp */
unsigned short * display;
//,*XFimage,*XPimage=NULL;
//SDL_Surface *Ximage16,*Ximage24;
//static Uint32 sdl_mask=SDL_HWSURFACE|SDL_HWACCEL;/*place or remove some flags*/
//Uint32 sdl_mask=SDL_HWSURFACE;
//SDL_Rect rectdst,rectsrc;

void DestroyDisplay(void)
{
	if(display){
//		if(Ximage16) SDL_FreeSurface(Ximage16);
//		if(Ximage24) SDL_FreeSurface(Ximage24);
//		if(XFimage) SDL_FreeSurface(XFimage); 

//		SDL_FreeSurface(display);//the display is also a surface in SDL
	}       
//	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void SetDisplay(void)
{
//	display = SDL_SetVideoMode(iResX,iResY,depth,SDL_FULLSCREEN|sdl_mask);
}

void CreateDisplay(void)
{
 	display = &VideoBuffer[0][0];
 	
//	display = SDL_SetVideoMode(iResX,iResY,depth,!iWindowMode*SDL_FULLSCREEN|sdl_mask); 					   
//	Ximage16= SDL_CreateRGBSurfaceFrom((void*)psxVub, 1024,512,16,2048 ,0x1f,0x1f<<5,0x1f<<10,0);
//	Ximage24= SDL_CreateRGBSurfaceFrom((void*)psxVub, 1024*2/3,512 ,24,2048 ,0xFF0000,0xFF00,0xFF,0);
//	XFimage= SDL_CreateRGBSurface(sdl_mask,170,15,depth,0x00ff0000,0x0000ff00,0x000000ff,0);

	iColDepth=depth;
	//memset(XFimage->pixels,255,170*15*4);//really needed???
	//memset(Ximage->pixels,0,ResX*ResY*4);

	//Xpitch=iResX*32; no more use
}


////////////////////////////////////////////////////////////////////////

void ShowGunCursor(unsigned char * /*surf*/,int /*iPitch*/)
{
}

////////////////////////////////////////////////////////////////////////

void NoStretchSwap(void)
{
	printf(">>>NoStretchSwap();\n");
}

////////////////////////////////////////////////////////////////////////
void EZX_Flip();	/* psx_player.cpp */
extern int GameFrameSkip;

void DoBufferSwap(void)                                // SWAP BUFFERS
{                                                      // (we don't swap... we blit only)
//	if (PreviousPSXDisplay.Range.x1 == 0) return;

	static int fs = 0;
	fs ++;
	if (fs <= GameFrameSkip) return;
	fs = 0;
	
	int x = PSXDisplay.DisplayPosition.x;
	int y = PSXDisplay.DisplayPosition.y;
	int w = PreviousPSXDisplay.Range.x1;
	int h = PreviousPSXDisplay.DisplayMode.y;
	int ww, ii;
	
	
	unsigned short * psrc = (unsigned short *)psxVub;
	unsigned short * ppsrc;
	int src_patch = 1024;
	
	unsigned char  * ccsrc;
	
	unsigned short * pdst = &VideoBuffer[0][0];
	unsigned short * ppdst;
	
	static int bbw = 0;
	static int bbh = 0;
	static bool bbd = 0;
	if ((w != bbw) || (h != bbh) || (PSXDisplay.RGB24 != bbd)) {
		printf("Display change to %d x %d x %dbits\n", w, h, (PSXDisplay.RGB24 ? 24 : 16));
		bbw = w;
		bbh = h;
		bbd = PSXDisplay.RGB24;
	}
	

	switch ( w ) {
	case 256:	// 256x240
		psrc += y * 1024 + x;
		pdst += 32;
		if (h > 240) {
			psrc += ((h - 240) >> 1) * 1024;
			h = 240;
		}
		while (h--) {
			ppsrc = psrc;
			ppdst = pdst;
			ww = w;
			while (ww--) {
#if USE_FAST_COLOR_TABLE
				*ppdst  = RGB15toBGR16[*ppsrc];
#else				
				*ppdst  = ((*ppsrc & 0x7C00) >> 10) 
						| ((*ppsrc & 0x03E0) <<  1) 
						| ((*ppsrc & 0x001F) << 11);
#endif
				ppsrc ++;
				ppdst ++;
			}
			psrc += 1024;
			pdst += 320;
		}
		break;

	case 320:	// 320x240
		psrc += y * 1024 + x;
		//pdst += 0;
		
		if (h > 320) {
			psrc += ((h - 480) >> 1) * 1024;
			h >>= 1;
			if (h > 240) h = 240;
			src_patch = 2048;
		} else		
		if (h > 240) {
			psrc += ((h - 240) >> 1) * 1024;
			h = 240;
		} else 
		if (h < 240) {
			pdst += ((240 - h) >> 1) * 320;
		}
		
		if (PSXDisplay.RGB24)
			while (h--) {
				ccsrc = (unsigned char *)psrc;
				ppdst = pdst;
				ww = w;
				while (ww--) {
					*ppdst  = ((ccsrc[2] & 0xF8) >> 3) 
							| ((ccsrc[1] & 0xF8) << 3) 
							| ((ccsrc[0] & 0xF8) << 8);
					ccsrc += 3;
					ppdst ++;
				}
				psrc += src_patch;
				pdst += 320;
			}
		else
			while (h--) {
				ppsrc = psrc;
				ppdst = pdst;
				ww = w;
				while (ww--) {
#if USE_FAST_COLOR_TABLE
					*ppdst  = RGB15toBGR16[*ppsrc];
#else				
					*ppdst  = ((*ppsrc & 0x7C00) >> 10) 
							| ((*ppsrc & 0x03E0) <<  1) 
							| ((*ppsrc & 0x001F) << 11);
#endif
					ppsrc ++;
					ppdst ++;
				}
				psrc += src_patch;
				pdst += 320;
			}
		break;
	case 368:  // 368x240 ( 23pix -> 20pix )
		psrc += y * 1024 + x;
		//pdst += 0;
		if (h > 320) {
			h >>= 1;
			src_patch = 2048;
		}
		
		if (h > 240) {
			psrc += ((h - 240) >> 1) * src_patch;
			h = 240;
		} else 
		if (h < 240) {
			pdst += ((240 - h) >> 1) * 320;
		}
		
		while (h--) {
			ppsrc = psrc;
			ppdst = pdst;
			ww = w;
			ii = 0;
			while (ww--) {
				
				switch( ii ) {
				case 3:
				case 11:
				case 19:
					break;
				case 22:
					ii = -1;
				default:
#if USE_FAST_COLOR_TABLE
					*ppdst  = RGB15toBGR16[*ppsrc];
#else				
					*ppdst  = ((*ppsrc & 0x7C00) >> 10) 
							| ((*ppsrc & 0x03E0) <<  1) 
							| ((*ppsrc & 0x001F) << 11);
#endif
					ppdst ++;						
				}
				
				ii ++;
				ppsrc ++;
			}
			psrc += src_patch;
			pdst += 320;
		}
		
		break;
	case 384:	// 384x240  (6pix -> 5pix)
		psrc += y * 1024 + x;

		if (h > 240) {
			psrc += ((h - 240) >> 1) * 1024;
			h = 240;
		}
		
		while (h--) {
			ppsrc = psrc;
			ppdst = pdst;
			ww = w;
			ii = 0;
			while (ww--) {
				
				if (ii == 5) 
					ii = 0;
				else {
#if USE_FAST_COLOR_TABLE
					*ppdst  = RGB15toBGR16[*ppsrc];
#else				
					*ppdst  = ((*ppsrc & 0x7C00) >> 10) 
							| ((*ppsrc & 0x03E0) <<  1) 
							| ((*ppsrc & 0x001F) << 11);
#endif
					ppdst ++;
					ii ++;
				}
				ppsrc ++;
			}
			psrc += 1024;
			pdst += 320;
		}
		
		break;	
	
	case 512:	// 512x240  (8pix -> 5pix)
		psrc += y * 1024 + x;
		//pdst += 0;
		if (h > 320) {
			h >>= 1;
			src_patch = 2048;
		}
		
		if (h > 240) {
			psrc += ((h - 240) >> 1) * src_patch;
			h = 240;
		} else 
		if (h < 240) {
			pdst += ((240 - h) >> 1) * 320;
		}
		
		if (PSXDisplay.RGB24)
			while (h--) {
				ccsrc = (unsigned char *)psrc;
				ppdst = pdst;
				ww = w;
				while (ww--) {
					switch( ww & 0x07 ) {
					case 0:
					case 2:
					case 3:
					case 5:
					case 6:
						*ppdst  = ((ccsrc[2] & 0xF8) >> 3) 
								| ((ccsrc[1] & 0xF8) << 3) 
								| ((ccsrc[0] & 0xF8) << 8);
						ppdst ++;
						break;
					}
					ccsrc += 3;
				}
				psrc += src_patch;
				pdst += 320;
			}
		else
			while (h--) {
				ppsrc = psrc;
				ppdst = pdst;
				ww = w;
				while (ww--) {
					
					switch( ww & 0x07 ) {
					case 0:
					case 2:
					case 3:
					case 5:
					case 6:
					
#if USE_FAST_COLOR_TABLE
						*ppdst  = RGB15toBGR16[*ppsrc];
#else				
						*ppdst  = ((*ppsrc & 0x7C00) >> 10) 
								| ((*ppsrc & 0x03E0) <<  1) 
								| ((*ppsrc & 0x001F) << 11);
#endif
						ppdst ++;
						break;
					}
					ppsrc ++;
				}
				psrc += src_patch;
				pdst += 320;
			}
		
		break;

	case 640:	// 640x478
		psrc += y * 1024 + x;

		w >>= 1;
		
		if (h > 320) {
			h >>= 1;
			src_patch = 2048;
		} else
		if (h > 240) {
			psrc += ((h - 240) >> 1) * src_patch;
			h = 240;
		} else 
		if (h < 240) {
			pdst += ((240 - h) >> 1) * 320;
		}
		
		pdst += ((240 - h) >> 1) * 320;
		
		while (h--) {
			ppsrc = psrc;
			ppdst = pdst;
			ww = w;
			while (ww--) {
#if USE_FAST_COLOR_TABLE
				*ppdst  = RGB15toBGR16[*ppsrc];
#else				
				*ppdst  = ((*ppsrc & 0x7C00) >> 10) 
						| ((*ppsrc & 0x03E0) <<  1) 
						| ((*ppsrc & 0x001F) << 11);
#endif
				ppsrc += 2;
				ppdst ++;
			}
			psrc += src_patch;
			pdst += 320;
		}
		break;

#if 0
	default:
		
	
		psrc += y * 1024 + x;
		//pdst += 0;
		if (h > 240) {
			psrc += ((h - 240) >> 1) * 1024;
			h = 240;
		}
		
		w = 320;
		
		while (h--) {
			ppsrc = psrc;
			ppdst = pdst;
			ww = w;
			while (ww--) {
#if USE_FAST_COLOR_TABLE
				*ppdst  = RGB15toBGR16[*ppsrc];
#else				
				*ppdst  = ((*ppsrc & 0x7C00) >> 10) 
						| ((*ppsrc & 0x03E0) <<  1) 
						| ((*ppsrc & 0x001F) << 11);
#endif
				ppsrc ++;
				ppdst ++;
			}
			psrc += 1024;
			pdst += 320;
		}
		
		break;
#endif		
	}
	

	EZX_Flip();
	
}

////////////////////////////////////////////////////////////////////////

void DoClearScreenBuffer(void)                         // CLEAR DX BUFFER
{
	/*SDL_BlitSurface(XCimage,NULL,display,NULL);*/
	memset(VideoBuffer, 0, 320*240*2);
	EZX_Flip();
}

////////////////////////////////////////////////////////////////////////

void DoClearFrontBuffer(void)                          // CLEAR DX BUFFER
{
	memset(VideoBuffer, 0, 320*240*2);
	EZX_Flip();
}


////////////////////////////////////////////////////////////////////////

int Xinitialize()
{
	bUsingTWin=FALSE;                          

//	InitMenu();                

	bIsFirstFrame = FALSE;                                // done

/*
	if(iShowFPS) {
		iShowFPS=0;
		ulKeybits|=KEY_SHOWFPS;
		szDispBuf[0]=0;
		BuildDispMenu(0);
	}
*/
	return 0;
}

////////////////////////////////////////////////////////////////////////

void Xcleanup()                                        // X CLEANUP
{
}

////////////////////////////////////////////////////////////////////////

unsigned long ulInitDisplay(void)
{
#if USE_FAST_COLOR_TABLE
	for (unsigned int i=0;i<65536;i++)
		RGB15toBGR16[i] = ((i & 0x7C00) >> 10)
						| ((i & 0x03E0) <<  1) 
						| ((i & 0x001F) << 11);
#endif
	CreateDisplay();                                      // x stuff
	Xinitialize();                                        // init x
	return (unsigned long)display;
}

////////////////////////////////////////////////////////////////////////

void CloseDisplay(void)
{                                                      
	Xcleanup();                                           // cleanup dx
	DestroyDisplay();
}

////////////////////////////////////////////////////////////////////////

void CreatePic(unsigned char * /*pMem*/)
{
}

///////////////////////////////////////////////////////////////////////////////////////

void DestroyPic(void)
{
}

///////////////////////////////////////////////////////////////////////////////////////

void DisplayPic(void)
{
}

///////////////////////////////////////////////////////////////////////////////////////

void ShowGpuPic(void)
{
}

///////////////////////////////////////////////////////////////////////////////////////

void ShowTextGpuPic(void)
{
}




