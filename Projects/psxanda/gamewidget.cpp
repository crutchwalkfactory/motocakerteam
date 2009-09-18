/*
 * EZX Emulator for MOTO EZX Modile Phone
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
 * $Id: gamewidget.cpp,v 0.10 2006/06/07 $
 */

#include <qapplication.h>
#include <stdio.h>
#include <qfileinfo.h>
#include <qstringlist.h>
#include <time.h>

#include <linux/fb.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include "gamewidget.h"
#include "configfile.h"
#include "font.h"

extern QString SavePath;			/* main.cpp */
extern QFileInfo * RomFileInfo; 
extern ConfigFile *AppConf;
extern QString CapturePath;

GameWidget *wgGame	= 0;

bool GameLooping	= true;
bool GamePauseing	= false;
int  GameScreenMode	= 0;
int  GameFrameSkip	= -1;
bool GameMute		= true;
int  GameGain		= 0;
int  GameKeymap[10]	= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

unsigned short VideoBuffer[240][320];
//unsigned short VideoBufferH [320][240];
//unsigned short VideoBufferTmp2 [2][320];

static char *z6_dev_name = NULL;
static int fb_dev_fd;
static struct fb_var_screeninfo fb_vinfo;

struct HARD_KEY_CODE HKCS[EZX_KEY_COUNT];

void initHKCS()
{
	for(int i=0; i<EZX_KEY_COUNT; i++)
		HKCS[i].down = false;
}

GameWidget::GameWidget(QWidget *parent)
	: QWidget(parent), my_painter(0), my_locked(0)
{
	setBackgroundMode(NoBackground);
	memset( &VideoBuffer, 0x00, sizeof(VideoBuffer) ); //  320 * 240 * 2
	
	RomSaveName 	= SavePath + "/";
	RomSaveName	   += RomFileInfo->fileName();
	
	GamePauseing	= false;
	needUpdateMenu	= false;

	b_LastCapKey	= false;
	n_LoadSaveIndex = 0;
	n_ActiveMenuItem = 0;
	n_ActiveMenuItem2= 0;
	n_lastMouseKey 	= 0;
	
	initHKCS();
	
	n_MenuMode		= 0;
}

bool GameWidget::lockScreen(bool force) {
	if(!my_painter) {
		if(force || (isVisible() && isActiveWindow()))
			my_painter = new QDirectPainter(&painterWidget);
    	else
			return false;
	}
	my_locked++; // Increate lock refcount
	return true;
}
void GameWidget::clearFrameBuffer () {
    lockScreen(true);
    if (my_painter)
        memset(my_painter->frameBuffer(), 0, 240*320*3);
    unlockScreen();
}
void GameWidget::unlockScreen() {
	if( my_locked > 0 )
		my_locked--; // decrease lock refcount;

	if(!my_locked && my_painter) {
		my_painter->end();
		//delete my_painter;
		my_painter = 0;
	}
}

//#define _USE_64BIT_ROTBLT_

//#ifdef _USE_64BIT_ROTBLT_

// 64bit (4pixel) per rotate .

static inline void fastRotateBlit( ushort *fb, ushort *bits, int x, int y, int w, int h )
{
	// FIXME: this only works correctly for 240x320 displays
	ushort * pfb = fb;
	long long * ppfb;
	long long aa; 
	
	ushort * pbits = bits; 
	register ushort *ppbits1, *ppbits2, *ppbits3, *ppbits4;
	
	pfb += ( (320 - 1 - x) * 240 + y );
	pbits += ( y * 320 + x );
	
	register int i, j = h >> 2;
	while ( j-- ) {
		
		if ( GamePauseing )
			break;
		
		ppfb = (long long *)pfb;
		ppbits1 = pbits;
		ppbits2 = pbits + 320;
		ppbits3 = pbits + 640;
		ppbits4 = pbits + 960;
		
		pfb += 4;
		pbits += 1280;
		
		i = w;
		while ( i-- ) {
			aa  = *ppbits4;
			aa <<= 16;
			aa |= *ppbits3;
			aa <<= 16;
			aa |= *ppbits2;
			aa <<= 16;
			*ppfb = aa | *ppbits1;
			
			ppbits1 ++;
			ppbits2 ++;
			ppbits3 ++;
			ppbits4 ++;
			ppfb -= 60;
		}
	} 
}

//#else

// 32bit (4pixel) per rotate .
/*
static inline void fastRotateBlit( ushort *fb, ushort *bits, int x, int y, int w, int h )
{
	// FIXME: this only works correctly for 240x320 displays
	ushort * pfb = fb;
	register uint * ppfb;
	
	ushort * pbits = bits; 
	register ushort *ppbits1, *ppbits2;
	
	pfb += ( (320 - 1 - x) * 240 + y );
	pbits += ( y * 320 + x );
	
	register int i, j = h >> 1;
	while ( j-- ) {
		
		if ( GamePauseing )
			break;
		
		ppfb = (uint *)pfb;
		ppbits1 = ppbits2 = pbits;
		ppbits2 += 320;
		
		pfb += 2;
		pbits += 640;
		
		i = w;
		while ( i-- ) {
			*ppfb = (*ppbits2 << 16) | *ppbits1;
			ppbits1 ++;
			ppbits2 ++;
			ppfb -= 120;
		}
	} 
}

#endif
*/
void GameWidget::repaintRect(const QRect &rect) {
	if(!my_painter || !rect.width() || !rect.height())
		return;
	ushort *fb = (ushort*)my_painter->frameBuffer();
	ushort *buf = (ushort *) &VideoBuffer;
	fastRotateBlit(fb, buf, rect.left(), rect.top(), rect.width(), rect.height() );
}

void GameWidget::UpdateRect(int x, int y, int w, int h, bool force)
{
	lockScreen( force );
	if(my_painter && w && h) {
		ushort *fb = (ushort*)my_painter->frameBuffer();
		ushort *buf = (ushort *) &VideoBuffer;
		fastRotateBlit(fb, buf, x, y, w, h );
	}
	//usleep(1000);
	unlockScreen();
}

void GameWidget::UpdateRect2(int, int, int, int, bool force)
{
	lockScreen( force );
	if( my_painter )
		memcpy( my_painter->frameBuffer(), &VideoBuffer, 320 * 240 * 2 );
	unlockScreen();
}

uchar *GameWidget::lockFrameBuffer(bool force)
{
	if(!my_painter) {
		if(force || (isVisible() && isActiveWindow()))
			my_painter = new QDirectPainter(this);
    	else
			return 0;
	}
	my_locked++; // Increate lock refcount
	return my_painter->frameBuffer();
}

void GameWidget::unlockFrameBuffer()
{
	if( my_locked > 0 )
		my_locked--; // decrease lock refcount;

	if(!my_locked && my_painter) {
		my_painter->end();
		delete my_painter;
		my_painter = 0;
	}
}

void GameWidget::paintEvent(QPaintEvent *)
{
//	UpdateRect(0, 0, 320, 240, true);
}

bool GameWidget::qwsEvent( QWSEvent *ev )
{
	QWSKeyEvent *kv;
	int k, b;
	
	switch ( ev->type ) {
	case QWSEvent::Key :
		kv = (QWSKeyEvent *)ev;
		k = kv->simpleData.keycode;
		b = kv->simpleData.is_press;
		
		if (!b) {
			if ((k == HKCS[EZX_KEY_HANG].key_code) && b_LastCapKey) {
				GameLooping = false;
				return true;
			}
			b_LastCapKey = (k == HKCS[EZX_KEY_CAP].key_code);
		}

		if ( GamePauseing ) {
			doPauseMenuKey( k, b );
			return true;
		}
		if (! kv->simpleData.is_auto_repeat)
			for (int i=0; i<EZX_KEY_COUNT; i++)
				if (k == HKCS[i].key_code) {
					HKCS[i].down = b;
					
					if (b)
						switch( i ) {
						case EZX_KEY_UP:
							HKCS[EZX_KEY_DOWN].down = false;
							break;
						case EZX_KEY_DOWN:
							HKCS[EZX_KEY_UP].down = false;
							break;
						case EZX_KEY_LEFT:
							HKCS[EZX_KEY_RIGHT].down = false;
							break;
						case EZX_KEY_RIGHT:
							HKCS[EZX_KEY_LEFT].down = false;
							break;
						}
					
					break;
				}

		return true;
	}
	return QWidget::qwsEvent(ev);
}

void GameWidget::focusInEvent( QFocusEvent * ev )
{
	if ( GameLooping && GamePauseing ) {
		UpdateRect(0, 0, 320, 240, true);
		needUpdateMenu	= true;
	}
	QWidget::focusInEvent(ev);
}

void GameWidget::focusOutEvent( QFocusEvent * ev )
{
	GamePauseing = true;
	qApp->processEvents();
	doGamePause( true );
	doConfigValues( false );
	QWidget::focusOutEvent(ev);
	qApp->processEvents();
}

#define POS_MENU_X	66
#define POS_MENU_Y	52
#define POS_MENU_H	15

#define MOUSE_KEY_POPUP_MENU		1
#define MOUSE_KEY_MENU_OK			2
#define MOUSE_KEY_MENU_RETURN		3

#define MOUSE_KEY_MENU_ITEM1		11
#define MOUSE_KEY_MENU_ITEM2		12
#define MOUSE_KEY_MENU_ITEM3		13
#define MOUSE_KEY_MENU_ITEM4		14
#define MOUSE_KEY_MENU_ITEM5		15
#define MOUSE_KEY_MENU_ITEM6		16
#define MOUSE_KEY_MENU_ITEM7		17
#define MOUSE_KEY_MENU_ITEM8		18
#define MOUSE_KEY_MENU_ITEM9		19
#define MOUSE_KEY_MENU_ITEM0		20

void GameWidget::mousePressEvent ( QMouseEvent * ev)
{
	if (!GameLooping) return;
	
	int x = 319 - ev->y();
	int y = ev->x();
	
	if (GamePauseing) {
		
		if ((x > POS_MENU_X) && (x < (POS_MENU_X + 130)) && (y > POS_MENU_Y) && ( y < (POS_MENU_Y + 10 * POS_MENU_H ) ) ) {
			// menu items
			n_lastMouseKey = MOUSE_KEY_MENU_ITEM1 + (y - POS_MENU_Y) / POS_MENU_H;
			switch ( n_MenuMode ) {
			case 0: // main menu item 
				if ((n_lastMouseKey - MOUSE_KEY_MENU_ITEM1) != n_ActiveMenuItem ) {
					n_ActiveMenuItem = n_lastMouseKey - MOUSE_KEY_MENU_ITEM1;
					needUpdateMenu	= true;
				}
				break;
			case 1: // key map menu
				if (((n_lastMouseKey - MOUSE_KEY_MENU_ITEM1) != n_ActiveMenuItem2) && ((n_lastMouseKey - MOUSE_KEY_MENU_ITEM1) < 10  ) ) {
					n_ActiveMenuItem2 = n_lastMouseKey - MOUSE_KEY_MENU_ITEM1;
					needUpdateMenu	= true;
				}
				break;
			}
		} else 
		if ((x > (POS_MENU_X + 132)) && (x < (POS_MENU_X + 132 + 50))) {
			if ((y > (POS_MENU_Y + 10)) && ( y < (POS_MENU_Y + 10 + 50))) {
				// ok button
				n_lastMouseKey = MOUSE_KEY_MENU_OK;
			} else
			if ((y > (POS_MENU_Y + 70)) && ( y < (POS_MENU_Y + 70 + 50))) {
				// return button
				n_lastMouseKey = MOUSE_KEY_MENU_RETURN;
			}
		}
	} else {
		if ( (x > 120) && (x < 200) && (y > 80) && (y < 160) )
			n_lastMouseKey	= MOUSE_KEY_POPUP_MENU;
		else
		if ( x > 220 ) {
			if (y < 80 ) 
				HKCS[EZX_KEY_SOFT1].down = true;
			else
			if (y > 160 ) 
				HKCS[EZX_KEY_SOFT3].down = true;
			else
				HKCS[EZX_KEY_SOFT2].down = true;
		}
	}
	
	if (( x < 80 ) && (y < 80 ))
		CaptureScreen();
	
}
bool GameWidget::setFrameBufferDepth( char depth) {
     z6_dev_name = strdup("/dev/fb/0");
     if ((fb_dev_fd = open(z6_dev_name, O_RDWR)) == -1) {
         printf("Can't open /dev/fb/0\n");
         return false;
     }
     if (ioctl(fb_dev_fd, FBIOGET_VSCREENINFO, &fb_vinfo)) {
         printf("Can't get info\n");
         return false;
     }
     printf("Old Bpp: %d\n",fb_vinfo.bits_per_pixel);
     fb_vinfo.bits_per_pixel = depth;
     if (ioctl(fb_dev_fd, FBIOPUT_VSCREENINFO, &fb_vinfo)) {
         printf("Can't set info\n");
         return false;
     }
     if (ioctl(fb_dev_fd, FBIOGET_VSCREENINFO, &fb_vinfo)) {
         printf("Can't get info\n");
         return false;
     }
     printf("New Bpp: %d\n",fb_vinfo.bits_per_pixel);
//     close(fb_dev_fd);
     return true;
}
void GameWidget::mouseReleaseEvent ( QMouseEvent * ev)
{
	if ( !GameLooping ) return;
	
	int x = 319 - ev->y();
	int y = ev->x();

	switch ( n_lastMouseKey ) {
	case MOUSE_KEY_POPUP_MENU:
		// pause game and show screen menu
		GamePauseing = true;
		doGamePause( true );
		doConfigValues( false );	// load current config values
		needUpdateMenu	= true;
		break;
	case MOUSE_KEY_MENU_OK:
		if ((x > (POS_MENU_X + 132)) && (x < (POS_MENU_X + 132 + 50)) && (y > (POS_MENU_Y + 10)) && ( y < (POS_MENU_Y + 10 + 50)) ) {
			if ( doMenuExec() ) {
				GamePauseing = false;
				//initHKCS();
				doGamePause( false );
			}
		}
		break;
	case MOUSE_KEY_MENU_RETURN:
		if ((x > (POS_MENU_X + 132)) && (x < (POS_MENU_X + 132 + 50)) && (y > (POS_MENU_Y + 70)) && ( y < (POS_MENU_Y + 70 + 50)) ) {
			if ( n_MenuMode == 0 ) {
				// canel screen menu and back to game
				GamePauseing = false;
				//initHKCS();
				doGamePause( false );
			} else {
				// cancel keymap config
				doKeymapValues( false ); 
				n_MenuMode = 0;
				needUpdateMenu = true;
			}
		}
		break;
	default:
		HKCS[EZX_KEY_SOFT1].down = false;
		HKCS[EZX_KEY_SOFT2].down = false;
		HKCS[EZX_KEY_SOFT3].down = false;
		break;
	}
	n_lastMouseKey = 0;
	
}

static inline void fastRotateBlit_menu( ushort *fb, ushort *bits, int x, int y, int w, int h )
{
	// FIXME: this only works correctly for 240x320 displays
	ushort * pfb = fb;
	register ushort * ppfb;
	ushort * pbits = bits; 
	register ushort * ppbits;
	
	pfb += ( (320 - x - w) * 240 + y );
	pbits += ( y * 320 + x + w - 1 );
	
	int i, j = w;
	while ( j-- ) {
		ppfb = pfb;
		ppbits = pbits;
		pfb += 240;
		pbits --;
		i = h;
		while ( i-- ) {
			*ppfb = *ppbits;
			ppbits += 320;
			ppfb ++;
		}
	} 
}

void GameWidget::PauseProc()
{
	if ( needUpdateMenu ) {
		
		drawPauseMenu();
		
		lockScreen( false );
		if ( my_painter ) {
			fastRotateBlit_menu((ushort* ) my_painter->frameBuffer(), 
								(ushort *) &VideoBuffer, 
								50L, 40L, 220L, 164L );
		}
		unlockScreen();
		needUpdateMenu = false;
	}
	
	qApp->processEvents();
	//EZX_Delay(50);
}

void GameWidget::drawPauseMenu()
{
	char name[20];
	
	DrawRect((uint16 *)&VideoBuffer, 50L, 42L, 220L, 164L, 0x000040 );
	if ( n_MenuMode == 0 ) { /* Main Menu */
		DrawRect((uint16 *)&VideoBuffer, POS_MENU_X, 
				 POS_MENU_Y + n_ActiveMenuItem * POS_MENU_H - 3, 
				 123, POS_MENU_H+1, 0xA00000 );
		
		sprintf(name, "1. Volume      %02d", n_Volume);
		DrawString (name, (uint16 *)&VideoBuffer, POS_MENU_X, POS_MENU_Y);
		
		if (n_FrameSkip < 0)
			sprintf(name, "2. FrameSkip %s", "AUTO");
		else
			sprintf(name, "2. FrameSkip   %02d", n_FrameSkip);
		DrawString (name, (uint16 *)&VideoBuffer, POS_MENU_X, POS_MENU_Y + POS_MENU_H);
		
		sprintf(name, "3. Save        %02d", n_LoadSaveIndex);
		DrawString (name, (uint16 *)&VideoBuffer, POS_MENU_X, POS_MENU_Y + POS_MENU_H * 2);
		sprintf(name, "4. Load        %02d", n_LoadSaveIndex);
		DrawString (name, (uint16 *)&VideoBuffer, POS_MENU_X, POS_MENU_Y + POS_MENU_H * 3);
		
		sprintf(name, "5. Sound      %s", b_Mute ? "Off" : " On");
		DrawString (name, (uint16 *)&VideoBuffer, POS_MENU_X, POS_MENU_Y + POS_MENU_H * 4);

		sprintf(name, "6. Screen  %s", (n_ScreenMode) ? "  Full" : "Normal");
		DrawString (name, (uint16 *)&VideoBuffer, POS_MENU_X, POS_MENU_Y + POS_MENU_H * 5);

		sprintf(name, "7. Keypad Remap");
		DrawString (name, (uint16 *)&VideoBuffer, POS_MENU_X, POS_MENU_Y + POS_MENU_H * 6);

		sprintf(name, "8. Save %s CFG", b_SaveGlobalCfg ? "Glob." : "Spec.");
		DrawString (name, (uint16 *)&VideoBuffer, POS_MENU_X, POS_MENU_Y + POS_MENU_H * 7);

		sprintf(name, "9. Reset");
		DrawString (name, (uint16 *)&VideoBuffer, POS_MENU_X, POS_MENU_Y + POS_MENU_H * 8);

		sprintf(name, "0. Exit");
		DrawString (name, (uint16 *)&VideoBuffer, POS_MENU_X, POS_MENU_Y + POS_MENU_H * 9);
		
		DrawRect((uint16 *)&VideoBuffer, POS_MENU_X + 132, POS_MENU_Y + 10, 50, 50, 0x208020 );
		DrawString ("OK", (uint16 *)&VideoBuffer, POS_MENU_X + 147, POS_MENU_Y + 30);
		
		DrawRect((uint16 *)&VideoBuffer, POS_MENU_X + 132, POS_MENU_Y + 70, 50, 50, 0x208020 );
		DrawString ("Return", (uint16 *)&VideoBuffer, POS_MENU_X + 134, POS_MENU_Y + 90);
		
	//	DrawString( "-= EZX Emulator v0.2 =-", (uint16 *)&VideoBuffer, POS_MENU_X + 6, POS_MENU_Y - POS_MENU_H - 2);
	//	DrawString( "Powered by OopsWare", (uint16 *)&VideoBuffer, POS_MENU_X + 10, POS_MENU_Y + POS_MENU_H * 9 + 2 );
	} else
	if (n_MenuMode == 1) {	/* Key Map Menu */ 
		
		struct keymap_item * pkm = getKeymapData();

		DrawRect((uint16 *)&VideoBuffer, POS_MENU_X, 
				 POS_MENU_Y + n_ActiveMenuItem2 * POS_MENU_H - 3, 
				 123, POS_MENU_H+1, 0xA00000 );
		
		sprintf(name, "1. REAL  %8s", pkm[n_keymap[ 0 ]].name );
		DrawString (name, (uint16 *)&VideoBuffer, POS_MENU_X, POS_MENU_Y);
		sprintf(name, "2. RADIO %8s", pkm[n_keymap[ 1 ]].name );
		DrawString (name, (uint16 *)&VideoBuffer, POS_MENU_X, POS_MENU_Y + POS_MENU_H );
		sprintf(name, "3. VOL+  %8s", pkm[n_keymap[ 2 ]].name );
		DrawString (name, (uint16 *)&VideoBuffer, POS_MENU_X, POS_MENU_Y + POS_MENU_H * 2 );
		sprintf(name, "4. VOL-  %8s", pkm[n_keymap[ 3 ]].name );
		DrawString (name, (uint16 *)&VideoBuffer, POS_MENU_X, POS_MENU_Y + POS_MENU_H * 3 );
		sprintf(name, "5. CALL  %8s", pkm[n_keymap[ 4 ]].name );
		DrawString (name, (uint16 *)&VideoBuffer, POS_MENU_X, POS_MENU_Y + POS_MENU_H * 4 );
		sprintf(name, "6. HANG  %8s", pkm[n_keymap[ 5 ]].name );
		DrawString (name, (uint16 *)&VideoBuffer, POS_MENU_X, POS_MENU_Y + POS_MENU_H * 5 );
		sprintf(name, "7. OK    %8s", pkm[n_keymap[ 6 ]].name );
		DrawString (name, (uint16 *)&VideoBuffer, POS_MENU_X, POS_MENU_Y + POS_MENU_H * 6 );
		sprintf(name, "8. SOFT1 %8s", pkm[n_keymap[ 7 ]].name );
		DrawString (name, (uint16 *)&VideoBuffer, POS_MENU_X, POS_MENU_Y + POS_MENU_H * 7 );
		sprintf(name, "9. SOFT2 %8s", pkm[n_keymap[ 8 ]].name );
		DrawString (name, (uint16 *)&VideoBuffer, POS_MENU_X, POS_MENU_Y + POS_MENU_H * 8 );
		sprintf(name, "0. SOFT3 %8s", pkm[n_keymap[ 9 ]].name );
		DrawString (name, (uint16 *)&VideoBuffer, POS_MENU_X, POS_MENU_Y + POS_MENU_H * 9 );
		
		DrawRect((uint16 *)&VideoBuffer, POS_MENU_X + 132, POS_MENU_Y + 10, 50, 50, 0x208020 );
		DrawString ("OK", (uint16 *)&VideoBuffer, POS_MENU_X + 147, POS_MENU_Y + 30);
		
		DrawRect((uint16 *)&VideoBuffer, POS_MENU_X + 132, POS_MENU_Y + 70, 50, 50, 0x208020 );
		DrawString ("Cancel", (uint16 *)&VideoBuffer, POS_MENU_X + 134, POS_MENU_Y + 90);
	}
	
}

void GameWidget::doPauseMenuKey(int k, bool d)
{
	int zz;
	if ((k == HKCS[EZX_KEY_UP].key_code) && d ) {
		switch( n_MenuMode ) {
		case 0:
			n_ActiveMenuItem = (n_ActiveMenuItem == 0) ? 9 : n_ActiveMenuItem - 1;
			break;
		case 1:
			n_ActiveMenuItem2 = (n_ActiveMenuItem2 == 0) ? 9 : n_ActiveMenuItem2 - 1;
			break;
		}
		needUpdateMenu = true;
	} else
	if ((k == HKCS[EZX_KEY_DOWN].key_code) && d ) {
		switch( n_MenuMode ) {
		case 0:
			n_ActiveMenuItem = (n_ActiveMenuItem == 9) ? 0 : n_ActiveMenuItem + 1;
			break;
		case 1:
			n_ActiveMenuItem2 = (n_ActiveMenuItem2 == 9) ? 0 : n_ActiveMenuItem2 + 1;
			break;
		}
		needUpdateMenu = true;
	} else
	if (d & (n_MenuMode == 0))
	switch ( n_ActiveMenuItem ) {
	case 0:	// Volume
		if (k == HKCS[EZX_KEY_LEFT].key_code) n_Volume-=5;  else
		if (k == HKCS[EZX_KEY_RIGHT].key_code) n_Volume+=5; else break;
		if (n_Volume > 80) n_Volume = 80;
		if (n_Volume < 10) n_Volume = 10;
		needUpdateMenu = true;
		break;
	case 1: // Frame Skip
		if (k == HKCS[EZX_KEY_LEFT].key_code) n_FrameSkip--;  else
		if (k == HKCS[EZX_KEY_RIGHT].key_code) n_FrameSkip++; else break;
		if (n_FrameSkip > 9) n_FrameSkip = -1;
		if (n_FrameSkip < -1) n_FrameSkip = 9;
		needUpdateMenu = true;
		break;
	case 2: // Load index 
	case 3: // Save index
		if (k == HKCS[EZX_KEY_LEFT].key_code) n_LoadSaveIndex--;	else
		if (k == HKCS[EZX_KEY_RIGHT].key_code) n_LoadSaveIndex++;else break;
		if (n_LoadSaveIndex > 99) n_LoadSaveIndex = 0;
		if (n_LoadSaveIndex < 0) n_LoadSaveIndex = 99;
		needUpdateMenu = true;
		break;
	case 4: // Sound On/Off
		if (( k == HKCS[EZX_KEY_LEFT].key_code ) || ( k == HKCS[EZX_KEY_RIGHT].key_code )) {
			b_Mute = !b_Mute;
			needUpdateMenu = true;
		}
		break;

	case 5: // Screen Mode
		if ( k == HKCS[EZX_KEY_LEFT].key_code ) n_ScreenMode --; else
		if ( k == HKCS[EZX_KEY_RIGHT].key_code )n_ScreenMode ++; else break;
		if (n_ScreenMode > 1) n_ScreenMode = 0;
		if (n_ScreenMode < 0) n_ScreenMode = 1;
		needUpdateMenu = true;
		break;
		
	case 7: // Save Global/Special Config
		if (( k == HKCS[EZX_KEY_LEFT].key_code ) || ( k == HKCS[EZX_KEY_RIGHT].key_code )) {
			b_SaveGlobalCfg = !b_SaveGlobalCfg;
			needUpdateMenu = true;
		}
		break;
	}
	else
	if (d & (n_MenuMode == 1)) {
		zz = n_keymap[ n_ActiveMenuItem2 ];
		if (k == HKCS[EZX_KEY_LEFT].key_code)	zz--; else
		if (k == HKCS[EZX_KEY_RIGHT].key_code)	zz++; else return;
		if (zz >= getKeymapCount()) zz = 0;
		if (zz < 0) zz = getKeymapCount()-1;
		n_keymap[ n_ActiveMenuItem2 ] = zz;
		needUpdateMenu = true;
	}
	
}

bool GameWidget::doMenuExec()
{
	QString s;
	
	if ( n_MenuMode == 0 ) {
		switch ( n_ActiveMenuItem ) {
		case 0: // 1. Volume
		case 1: // 2. Speed
		case 4: // 5. Sound
		case 5: // 6. Screen
			doConfigValues(true);
			return true;
		case 2: // 3. Save
			s = RomSaveName + ".s" + QString::number( n_LoadSaveIndex ).rightJustify(2, '0');
			if ( !doSaveGame( s.local8Bit() ) ) return false;
			return true;
		case 3: // 4. Load
			s = RomSaveName + ".s" + QString::number( n_LoadSaveIndex ).rightJustify(2, '0');
			if ( !doLoadGame( s.local8Bit() ) ) return false;
			return true;
		case 6: // 7. Key Map
			doKeymapValues(false);
			n_MenuMode = 1;
			needUpdateMenu = true;
			return false;
		case 7: // 8. Save Config
			doConfigValues(true);
			doSaveConfig( b_SaveGlobalCfg );
			return true;
		case 8: // 9. Reset
			doGameReset();
			return true;
		case 9: // 0. Exit
			GameLooping = false;
			return false;
		}
	} else 
	if ( n_MenuMode == 1 ) {
		doKeymapValues(true);
		n_MenuMode = 0;
		needUpdateMenu = true;
		return false;
	}
	return true;
}

void GameWidget::doLoadConfig()
{
	ConfigFile * cfg = new ConfigFile( RomSaveName + ".cfg" );
	b_SaveGlobalCfg = false;
	if ( !cfg->fileExists() ) {
		delete cfg;
		b_SaveGlobalCfg = true;
		cfg = AppConf;
	}
	
	if ( cfg ) {
		b_Mute = GameMute = cfg->readString( doConfigItemPerfix("MUTE") ).toInt() != 0;
		n_Volume = AppConf->readString( "VOLUME" ).toInt();
		if (n_Volume < 10) n_Volume = 10;
		if (n_Volume > 80) n_Volume = 80;
		GameGain = n_Volume;
		
		n_FrameSkip = cfg->readString( doConfigItemPerfix("SPEED") ).toInt();
		if (n_FrameSkip < -1) n_FrameSkip = -1;
		if (n_FrameSkip >  9) n_FrameSkip =  9;
		GameFrameSkip = n_FrameSkip;
		//n_FrameSkip = 9;
		//GameFrameSkip = 9;
		n_ScreenMode = cfg->readString( doConfigItemPerfix("SCREEN") ).toInt();
		if ((n_ScreenMode < 0) || (n_ScreenMode > 1))
			n_ScreenMode = 0;
		GameScreenMode = n_ScreenMode;
		
		QStringList ss = QStringList::split( ",", cfg->readString( doConfigItemPerfix("KEYMAP") ), true );
		for(unsigned int i=0; i<10;i++) {
			if (ss.count() > i) {
				n_keymap[i] = ss[i].toInt();
				if ((n_keymap[i] < 0) || (n_keymap[i] >= getKeymapCount()))
					n_keymap[i] = 0;
			} else 
				n_keymap[i] = 0;
			GameKeymap[i] = n_keymap[i];
		}
	}
}

void GameWidget::doSaveConfig( bool globalConf )
{
	ConfigFile * cfg = AppConf;
	if (!globalConf)
		cfg = new ConfigFile( RomSaveName + ".cfg" );
	
	if ( cfg ) {
		AppConf->writeString("VOLUME", QString::number(n_Volume));
		cfg->writeString( doConfigItemPerfix("MUTE"), b_Mute ? "1" : "0" );
		cfg->writeString( doConfigItemPerfix("SPEED"), QString::number( n_FrameSkip ) );
		cfg->writeString( doConfigItemPerfix("SCREEN"), QString::number( n_ScreenMode ) );
			
		QString s("");
		for(int i=0;i<10;i++) {
			s += QString::number( n_keymap[i] );
			if (i!=9) s += ",";
		}
		cfg->writeString( doConfigItemPerfix("KEYMAP"), s );
	}
	
	if (!globalConf && cfg ) delete cfg;
}

void GameWidget::doConfigValues(bool apply)
{
	if ( !apply ) {
		b_Mute			= GameMute;
		n_Volume		= GameGain;
		n_FrameSkip		= GameFrameSkip;
		n_ScreenMode	= GameScreenMode;
	} else {

		if ( ( GameScreenMode != n_ScreenMode ) && (n_ScreenMode == 0) ) {
			// clear screen while change screen mode from full to normal
			memset( &VideoBuffer, 0x00, 240 * 320 * 2 );
			UpdateRect2(0, 0, 320, 240);
		}
		
		GameMute		= b_Mute;
		GameGain		= n_Volume;
		GameFrameSkip	= n_FrameSkip;
		GameScreenMode	= n_ScreenMode;
	}
}

void GameWidget::doKeymapValues(bool apply)
{
	if ( !apply )
		for (int i=0;i<10;i++)
			n_keymap[i] = GameKeymap[i];
	else
		for (int i=0;i<10;i++)
			GameKeymap[i] = n_keymap[i];
}

void GameWidget::CaptureScreen()
{
	if (CapturePath.isEmpty())
		return;
	
	extern unsigned char  *psxVub;
	
#if 0

	QImage *img = new QImage( 1024, 512, 32 );
	unsigned int *p;
	unsigned short *q = (unsigned short *)psxVub;

	for (int j=0;j<512;j++) {
		p = (unsigned int *)img->scanLine(j);
		//q = &VideoBuffer[j+8][8];
		for (int i=0;i<1024;i++) {
			*p = ((*q & 0x7C00) >>  7) 
			   | ((*q & 0x03E0) <<  6) 
			   | ((*q & 0x001F) << 19);
			p++;
			q++;
		}
	}
	
#else

	QImage *img = new QImage( 320, 240, 32 );
	unsigned int *p;
	unsigned short *q;

	for (int j=0;j<240;j++) {
		p = (unsigned int *)img->scanLine(j);
		q = &VideoBuffer[j][0];
		for (int i=0;i<320;i++) {
			*p = ((*q & 0xF800) <<  8) 
			   | ((*q & 0x07E0) <<  5) 
			   | ((*q & 0x001F) <<  3);
			p++;
			q++;
		}
	}
	
#endif

	QString fn(CapturePath);
	time_t ltime;
    struct tm *today;

    time(&ltime);
    today = localtime(&ltime);
    
    char buf[30];
    sprintf(buf, "/%04d%02d%02d.%02d%02d%02d.png",
    		today->tm_year + 1900,
    		today->tm_mon + 1,
    		today->tm_mday,
    		today->tm_hour,
    		today->tm_min,
    		today->tm_sec
    		);
    fn += buf;

	img->save(fn.local8Bit(), "PNG");
	delete img;
}


//// ---------------------------------------------------------

#include <time.h>
#include <sys/time.h>
#include <errno.h>

static struct timeval start;

void EZX_StartTicks(void)
{
	gettimeofday(&start, NULL);
}

unsigned int EZX_GetTicks (void)
{
	unsigned int ticks;
	struct timeval now;
	gettimeofday(&now, NULL);
	ticks=(now.tv_sec-start.tv_sec)*1000+(now.tv_usec-start.tv_usec)/1000;
	return ticks;
}

void EZX_Delay (unsigned int ms)
{
	int was_error;
	struct timespec elapsed, tv;
	elapsed.tv_sec = ms/1000;
	elapsed.tv_nsec = (ms%1000)*1000000;
	do {
		errno = 0;
		tv.tv_sec = elapsed.tv_sec;
		tv.tv_nsec = elapsed.tv_nsec;
		was_error = nanosleep(&tv, &elapsed);
	} while ( was_error && (errno == EINTR) );
}

static void copy_row2(unsigned short *src, int src_w, unsigned short *dst, int dst_w)
{
	int i;
	int pos, inc;
	unsigned short pixel = 0;

	pos = 0x10000;
	inc = (src_w << 16) / dst_w;
	for ( i=dst_w; i>0; --i ) {
		while ( pos >= 0x10000L ) {
			pixel = *src++;
			pos -= 0x10000L;
		}
		*dst++ = pixel;
		pos += inc;
	}
}

int EZX_SoftStretch(void *src, int sx, int sy, int sw, int sh, int sp, void *dst, int dw, int dh)
{
	int pos, inc;
	int dst_width;
	int dst_maxrow;
	int src_row, dst_row;
	unsigned char *srcp = NULL;
	unsigned char *dstp;

	/* Set up the data... */
	pos = 0x10000;
	inc = (sh << 16) / dh;
	src_row = sy * sp; //srcrect->y;
	dst_row = 0; //dstrect->y;
	dst_width = dw * 2;

	/* Perform the stretch blit */
	for ( dst_maxrow = dst_row+dh; dst_row<dst_maxrow; ++dst_row ) {
		dstp = (unsigned char *)dst + (dst_row * 640);	// + (dtrect->x*bpp);
		while ( pos >= 0x10000L ) {
			srcp = (unsigned char *)src + src_row + sx * 2;
			src_row += sp;
			pos -= 0x10000L;
		}

		copy_row2((unsigned short *)srcp, sw, (unsigned short *)dstp, dw);
		pos += inc;
	}

	/* We need to unlock the surfaces if they're locked */
	return(0);
}

