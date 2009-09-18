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
 * $Id: gamewidget.h,v 0.10 2006/06/07 $
 */

#ifndef _GAME_WIDGET_
#define _GAME_WIDGET_
 
//#include <zmainwidget.h>
#include <qwidget.h>
#include <qimage.h>
#include <qwsevent_qws.h>
#include <qdirectpainter_qws.h>

extern unsigned short VideoBuffer [240][320];

struct HARD_KEY_CODE {
	int key_code;
	bool down;
};

#define EZX_KEY_REAL		0
#define EZX_KEY_RADIO		1
#define EZX_KEY_VOL_UP		2
#define EZX_KEY_VOL_DOWN	3
#define EZX_KEY_CALL		4
#define EZX_KEY_HANG		5
#define EZX_KEY_OK			6

#define EZX_KEY_SOFT1		7
#define EZX_KEY_SOFT2		8
#define EZX_KEY_SOFT3		9

#define EZX_KEY_CAP			10

#define EZX_KEY_UP			11
#define EZX_KEY_DOWN		12
#define EZX_KEY_LEFT		13
#define EZX_KEY_RIGHT		14

#define EZX_KEY_COUNT		15

extern struct HARD_KEY_CODE HKCS[];

extern bool GameLooping;
extern bool GamePauseing;

extern int  GameScreenMode;
extern int  GameFrameSkip;
extern int  GameKeymap[10];
extern bool GameMute;
//int  GameGain		= 0;

/* SDL Timer */

void EZX_StartTicks(void);
unsigned int EZX_GetTicks (void);
void EZX_Delay (unsigned int ms);
int EZX_SoftStretch(void *src, int sx, int sy, int sw, int sh, int sp, void *dst, int dw, int dh);

struct keymap_item  {
	char name[10];
	int  keypad;
	bool turbo;
};

/* SDL_Screen */

class GameWidget : public QWidget {
	Q_OBJECT
public:
	QString RomSaveName;
	QWidget painterWidget;
	GameWidget(QWidget *);
	virtual ~GameWidget() {};
	
	bool lockScreen(bool force=false);
	void unlockScreen();
	void repaintRect(QRect const &);
	virtual void UpdateRect(int, int, int, int, bool force = false);
	virtual void UpdateRect2(int, int, int, int, bool force = false);
	
	uchar *lockFrameBuffer(bool force=false);
	void unlockFrameBuffer();
      void clearFrameBuffer();
        bool setFrameBufferDepth( char depth);
	void PauseProc();
	virtual void doLoadConfig();
	virtual void doSaveConfig(bool);
	
protected:
	QDirectPainter *my_painter;
	QString s_ConfigPerfix;
	
	void paintEvent( QPaintEvent * );
	bool qwsEvent( QWSEvent * );
	
	virtual void focusInEvent( QFocusEvent * );
	virtual void focusOutEvent( QFocusEvent * );
	virtual void mousePressEvent ( QMouseEvent * );
	virtual void mouseReleaseEvent ( QMouseEvent * );

	virtual void doGamePause(bool) = 0;
	virtual bool doGameReset() = 0;
	virtual bool doSaveGame(const char *) = 0;
	virtual bool doLoadGame(const char *) = 0;
	
	virtual QString doConfigItemPerfix(QString const &) const = 0;
	
	virtual int getKeymapCount() = 0;
	virtual struct keymap_item * getKeymapData() = 0;
	
private:
	unsigned int my_locked;
	bool needUpdateMenu;
	int n_ActiveMenuItem;
	int n_ActiveMenuItem2;
	int n_lastMouseKey;
	bool b_LastCapKey;
	int n_ScreenMode;
	int n_Volume;
	int n_keymap[10];
	bool b_Mute;
	int n_FrameSkip;

	int n_LoadSaveIndex;
	bool b_SaveGlobalCfg;
	
	int n_MenuMode;
	
	void drawPauseMenu();
	void doPauseMenuKey(int, bool);
	bool doMenuExec();
	void doConfigValues(bool);
	void doKeymapValues(bool);
	
	void CaptureScreen();
};

extern GameWidget *wgGame;

#endif	// _GAME_WIDGET_
