/*
 * NES for MOTO EZX Modile Phone
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
 * SPECIAL THANKS:
 *   Sam Revitch  	http://lsb.blogdns.net/ezx-devkit 
 *
 * $Id: main.h,v 0.10 2006/06/07 $
 */
 
#ifndef MAIN_WIDGET_H
#define MAIN_WIDGET_H

#include <stddef.h>
#include <stdio.h>
#include <zapplication.h>
#include <zkbmainwidget.h>
#include <ezxres.h>
#include <zscrollpanel.h>
#include <zcombobox.h>
#include <qlayout.h>
#include "configfile.h"
#include <zsoftkey.h>

#define	EZX_EMU_VER	"1.80"

class MainWidget : public ZKbMainWidget {
	Q_OBJECT
	
private:
	int gameLooping;
	int	mid_about;
        ZSoftKey *softKey;
        ZScrollPanel * zsv;
        ZComboBox *romBox;
        QVBoxLayout* layout;
public:
	MainWidget(QWidget *parent, char *);
	virtual ~MainWidget();
	void gameLoop();
public slots:	
	void slotQuit();
	void runEmulator();
	void menuSelect(int);
	
};
	
extern MainWidget *wgMain;

#endif // MAIN_WIDGET_H
