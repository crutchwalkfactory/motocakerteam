/*
 * PlayStation Emulator for MOTO EZX Modile Phone
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
 * $Id: psx_player.h,v 0.01 2006/07/29 $
 */

#ifndef _PSX_PLAYER_
#define _PSX_PLAYER_

#include "gamewidget.h"

void run_psx_emulator(const char *, int);

class PSX_GameWidget : public GameWidget {
	Q_OBJECT
public:
	PSX_GameWidget(QWidget *wg)
		: GameWidget (wg) {};
	~PSX_GameWidget() {};
	
protected:
	void doGamePause(bool);
	bool doGameReset();
	bool doSaveGame(const char *);
	bool doLoadGame(const char *);
	
	QString doConfigItemPerfix(QString const &) const;
	
	int getKeymapCount();
	struct keymap_item * getKeymapData();
	
};

#endif	// _PSX_PLAYER_
