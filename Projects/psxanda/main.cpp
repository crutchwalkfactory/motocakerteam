/*
 * Sony PlayStation Emulator for MOTO EZX Modile Phone
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
 * $Id: main.cpp,v 0.10 2006/08/28 $
 *
 */

#include <qfileinfo.h>
#include <qdir.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdio.h>
#include <znoticedlg.h>

#include "main.h"

#define APP_PATH "/mmc/mmca1/psx/"
#define APP_CONF "ezxpsx.conf"

#include "psx_player.h"

MainWidget *wgMain = 0;
ZApplication *app;
ConfigFile *AppConf;
QString RomPath("");
QString SavePath("");
QString CapturePath("");
int BiosFile = 0;

QFileInfo * RomFileInfo 		= 0;
/*
MainWidget::MainWidget(QWidget *parent)
#ifndef EZX_E680
	: ZMainWidget(tr("EZX PSX"), false, parent, NULL, 0)
#else
	: ZMainWidget(false, parent, NULL, 0)
#endif
{
	QPopupMenu *pp = new QPopupMenu(this, NULL);
	mid_about = pp->insertItem(tr("About ..."));
	connect(pp, SIGNAL(activated(int)), SLOT(menuSelect(int)));
	
#ifndef EZX_E680
	cp = new UTIL_CST(this, tr("Run"));
	setCSTWidget(cp);
	ZPushButton *wp;
	wp = cp->getRightBtn();
	connect(wp, SIGNAL( clicked() ), SLOT( slotQuit() ));
	wp = cp->getMidBtn();
	connect(wp, SIGNAL(clicked()), SLOT(runEmulator()));
	wp = cp->getLeftBtn();
	wp->setPopup(pp);
#else
	cp = new QWidget(this, "E680_UTIL_CST");
	cp->move( 0, height() - UTIL_PushButton::defaultHeight + 2 );
	cp->resize( width(), UTIL_PushButton::defaultHeight - 2 );
	
	UTIL_PushButton *wp;
	wp = new UTIL_PushButton("CST_Menu", QString::null, cp, 0, UTIL_PushButton::defaultWidth, UTIL_PushButton::defaultHeight-2);
	wp->move(0, 0);
	wp->setPopup(pp);
	
	wp = new UTIL_PushButton("Run", QString::null, cp, 0, width() - UTIL_PushButton::defaultWidth * 2, UTIL_PushButton::defaultHeight-2);
	wp->move(UTIL_PushButton::defaultWidth, 0);
	connect(wp, SIGNAL(clicked()), SLOT(runEmulator()));

	wp = new UTIL_PushButton("CST_Exit", QString::null, cp, 0, UTIL_PushButton::defaultWidth, UTIL_PushButton::defaultHeight-2);
	wp->move(width() - UTIL_PushButton::defaultWidth, 0);
	connect(wp, SIGNAL( clicked() ), SLOT( slotQuit() ));
#endif

	romList = new QListBox(this);
	romList->setFrameStyle( QFrame::NoFrame );
	setContentWidget(romList);
	
	cp->show();
	romList->show();
	
	QDir d(RomPath, "*.bin *.iso *.img *.psx *.Z *.ZNX", QDir::Name | QDir::IgnoreCase, QDir::Files | (QDir::AccessMask & ~QDir::Writable) );
	for (unsigned int i=0; i<d.count(); i++)
		romList->insertItem( d[i] );
		
	if (romList->count() > 0 )
		romList->setCurrentItem(0);
}
*/
MainWidget::MainWidget(QWidget *parent, char* name)
	: ZKbMainWidget((ZHeader::HEADER_TYPE)3 , parent , name , WType_TopLevel)
{
        setMainWidgetTitle("eZxPSX");

        //soft keys
        softKey = new ZSoftKey(NULL , this , this);
        softKey->setText(ZSoftKey::LEFT, "Run", (ZSoftKey::TEXT_PRIORITY)0);
        softKey->setTextForOptMenuHide("Run");
        softKey->setText(ZSoftKey::RIGHT, "Exit", (ZSoftKey::TEXT_PRIORITY)0);
        softKey->setClickedSlot(ZSoftKey::RIGHT, this, SLOT(slotQuit()));
        softKey->setClickedSlot(ZSoftKey::LEFT, this, SLOT(runEmulator()));

        //scroll panel
        zsv = new ZScrollPanel(this , NULL , 0 ,  (ZSkinService::WidgetClsID)4);

	romBox = new ZComboBox();
        QDir d(RomPath, "*.bin *.iso *.img *.psx *.Z *.ZNX", QDir::Name | QDir::IgnoreCase, QDir::Files | (QDir::AccessMask & ~QDir::Writable) );
	for (unsigned int i=0; i<d.count(); i++)
		romBox->insertItem( d[i] );
//	if (romBox->count() > 0 )
//		romBox->setCurrentItem(0);
//        romBox->insertItem(QString ("Earthworm_Jim_2.bin"));

        zsv->addChild(romBox, 0, 20);
//	romList->setFrameStyle( QFrame::NoFrame );
//
        layout = getVBoxLayout();
        layout->addWidget(zsv);
        setSoftKey(softKey);
//	cp = new UTIL_CST(this, tr("Run"));
//	setCSTWidget(cp);
//	ZPressButton *wp;
//	wp = cp->getRightBtn();
//	connect(wp, SIGNAL( clicked() ), SLOT( slotQuit() ));
//	wp = cp->getMidBtn();
//	connect(wp, SIGNAL(clicked()), SLOT(runEmulator()));
//
//	QPopupMenu *pp = new QPopupMenu(this, NULL);
//	connect(pp, SIGNAL(activated(int)), SLOT(menuSelect(int)));
//	mid_about = pp->insertItem(tr("About ..."));
//	wp = cp->getLeftBtn();
//	wp->setPopup(pp);



//	romList->show();


}

MainWidget::~MainWidget() 
{
}

void MainWidget::slotQuit()
{
	delete AppConf;
	((ZApplication *)qApp)->tryQuit();
}

void MainWidget::runEmulator()
{
	//if (romList->currentItem() < 0) return;

	RomFileInfo = new QFileInfo(RomPath + "/" + romBox->currentText());
	if ( !RomFileInfo->isFile() || !RomFileInfo->isReadable() ) {
		delete RomFileInfo;
		// Load ROM Error!
		return;
	}
	
	QString ext = RomFileInfo->filePath().right(4).lower();
	QString ext2 = RomFileInfo->filePath().right(2).lower();
	
	if (ext == ".psx")
	{
	wgMain->setFullScreenMode(TRUE,FALSE);
        zsv->hide();
		run_psx_emulator( RomFileInfo->filePath().local8Bit(), 1 );	// .psx EXE-FILE
}
	else
	if (ext == ".bin")
{
	wgMain->setFullScreenMode(TRUE,FALSE);
        zsv->hide();
		run_psx_emulator( RomFileInfo->filePath().local8Bit(), 2 ); // .bin 
}
	else
	if (ext == ".iso")
	{
	wgMain->setFullScreenMode(TRUE,FALSE);
        zsv->hide();
		run_psx_emulator( RomFileInfo->filePath().local8Bit(), 3 ); // .iso
}
	else
	if (ext == ".img")
{
	wgMain->setFullScreenMode(TRUE,FALSE);
        zsv->hide();
		run_psx_emulator( RomFileInfo->filePath().local8Bit(), 4 ); // .img
}
	else
	if (ext == ".znx")
{
	wgMain->setFullScreenMode(TRUE,FALSE);
        zsv->hide();
		run_psx_emulator( RomFileInfo->filePath().local8Bit(), 8 ); // .znx
}
	else
	if (ext2 == ".z")
{wgMain->setFullScreenMode(TRUE,FALSE);
        zsv->hide();
		run_psx_emulator( RomFileInfo->filePath().local8Bit(), 9 ); // .Z
}
	else
{wgMain->setFullScreenMode(TRUE,FALSE);
        zsv->hide();
		run_psx_emulator( RomFileInfo->filePath().local8Bit(), 0 ); // BIOS
}
	
	delete RomFileInfo;
	RomFileInfo = 0;
	  wgMain->setFullScreenMode(FALSE,FALSE);
        wgMain->repaint();
        zsv->repaint();
        zsv->show();
        app->showMainWidget(wgMain);
        qApp->quit();
}

void MainWidget::menuSelect(int param)
{
	if (param == mid_about) {
		QString info("<b>EZXPSX</b>");
		info += "<br>ver: 0.051(2007.04.02)";
		info += "<br>port of PCSX v1.5";
		info += "<br><br>(C)2006,OopsWare.China.";
		//ZMessageBox::information( this, NULL, info, QString(tr("OK")) );
	}
}

/*
 * application main() 
 */

//MainWidget *wgMain = 0;

bool fixPath(const QString &s){
	QFileInfo fi(s);

	if ( !fi.exists() ) {
                if ( mkdir( QFile::encodeName( fi.filePath() ), 0777 ) != 0 ) {
                    qWarning("Can't create this dir");
                    return false;
                }
	} else
	if ( !fi.isDir() ) {
            qWarning("This is not directory");
            return false;
        }
	return true;
}
	
int main( int argc, char **argv )
{
AppConf = new ConfigFile( QString (APP_PATH) + QString (APP_CONF) );

	RomPath = QString (APP_PATH) + AppConf->readString("ROM_PATH");
	SavePath = QString (APP_PATH) + AppConf->readString("SAVE_PATH");
	CapturePath = QString (APP_PATH) + AppConf->readString("CAP_PATH");
	
	//BiosFile = AppConf->readString("BIOS").toInt();
	BiosFile = 0;
	if (BiosFile < 0) BiosFile = 0;
	if (BiosFile > 1) BiosFile = 1;
	
	HKCS[EZX_KEY_OK].key_code	= AppConf->readString("KEY_OK").toInt();
	HKCS[EZX_KEY_OK].down		= false;
	HKCS[EZX_KEY_UP].key_code	= AppConf->readString("KEY_UP").toInt();
	HKCS[EZX_KEY_UP].down		= false;
	HKCS[EZX_KEY_DOWN].key_code	= AppConf->readString("KEY_DOWN").toInt();
	HKCS[EZX_KEY_DOWN].down		= false;
	HKCS[EZX_KEY_LEFT].key_code	= AppConf->readString("KEY_LEFT").toInt();
	HKCS[EZX_KEY_LEFT].down		= false;
	HKCS[EZX_KEY_RIGHT].key_code= AppConf->readString("KEY_RIGHT").toInt();
	HKCS[EZX_KEY_RIGHT].down	= false;
	HKCS[EZX_KEY_VOL_UP].key_code	= AppConf->readString("KEY_VOL_UP").toInt();
	HKCS[EZX_KEY_VOL_UP].down	= false;
	HKCS[EZX_KEY_VOL_DOWN].key_code	= AppConf->readString("KEY_VOL_DOWN").toInt();
	HKCS[EZX_KEY_VOL_DOWN].down	= false;
	HKCS[EZX_KEY_REAL].key_code	= AppConf->readString("KEY_REAL").toInt();
	HKCS[EZX_KEY_REAL].down		= false;
	HKCS[EZX_KEY_RADIO].key_code= AppConf->readString("KEY_RADIO").toInt();
	HKCS[EZX_KEY_RADIO].down	= false;
	HKCS[EZX_KEY_CAP].key_code	= AppConf->readString("KEY_CAP").toInt();
	HKCS[EZX_KEY_CAP].down		= false;
	HKCS[EZX_KEY_CALL].key_code	= AppConf->readString("KEY_CALL").toInt();
	HKCS[EZX_KEY_CALL].down		= false;
	HKCS[EZX_KEY_HANG].key_code	= AppConf->readString("KEY_HANG").toInt();
	HKCS[EZX_KEY_HANG].down		= false;

	if (!fixPath(RomPath)) {
		delete AppConf;
		return 1;
	}
	if (!fixPath(SavePath)) {
		delete AppConf;
		return 1;
	}
	if (!CapturePath.isEmpty())
		if (!fixPath(CapturePath)) {
			delete AppConf;
			return 1;
		}
	
   app = new ZApplication(argc, argv);
//    app.enableTouchSound(false);

//    app.enableHardKeyEventFilter(false);

    wgMain = new MainWidget( NULL , NULL ); //QApplication::desktop()
    wgMain->setGeometry(0,0,240,320);
    app->setMainWidget( wgMain );
    wgMain->show();
    return app->exec();
}
