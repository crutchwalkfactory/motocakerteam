#include "myam.h"
#include "zgui.h"
#include "GUI_Define.h"
#include "ZFileDlg.h"

#include "stdio.h"
#include <stdlib.h>
#include <unistd.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <ezxres.h>
#include <ZApplication.h>
#include <ZListBox.h>
#include <ZSoftKey.h>
#include <ZOptionsMenu.h>
#include <ZMessageDlg.h>
#include <qfileinfo.h>
#include <qcopchannel_qws.h>

//using namespace std;


#define Z6KEY_CENTER            0x1004
#define Z6KEY_LEFT              0x1012
#define Z6KEY_UP                0x1013
#define Z6KEY_RIGHT             0x1014
#define Z6KEY_DOWN              0x1015
#define Z6KEY_GREEN             0x1030
#define Z6KEY_RED               0x1031
#define Z6KEY_LEFT_SOFTKEY      0x1038
#define Z6KEY_SIDE_SELECT       0x1005
#define Z6KEY_RIGHT_SOFTKEY     0x103a
#define Z6KEY_C                 0x1032
#define Z6KEY_MUSIC             0x1033
#define Z6KEY_VOLUP	            0x1016
#define Z6KEY_VOLDOWN           0x1017
#define Z6KEY_CAMERA            0x1034
#define SLIDER_CLOSE            0x1046


RES_ICON_Reader iconReader;
QString Idioma;
int ProgCount;
int seleccionados, totalnotes;
QString ventana;

ZGui::ZGui ( QWidget* parent, const char* name, WFlags fl )
    : ZKbMainWidget ( ZHeader::FULL_TYPE, NULL, "ZMainWidget", 0 )
{
/*
  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma = "/ezxlocal/download/mystuff/.system/LinXtend/usr/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma).isFile() ) Idioma = "/ezxlocal/download/mystuff/.system/LinXtend/usr/languages/en-us";
*/
	
  QString ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma = ProgDir + "/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma).isFile() ) Idioma = ProgDir + "/languages/en-us";

  CreateWindow ( parent );
  ( ( ZApplication* ) qApp )->showMainWidget ( this );

}

ZGui::~ZGui()
{

}

void ZGui::CreateWindow ( QWidget* parent )
{
  ZConfig IDini(Idioma, false);

  QWidget *myWidget = new ZWidget ( this, NULL, 0);
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );
  myWidget->setFixedWidth(240);
  pAIA = new ZAppInfoArea(myWidget, NULL, 0);
  pAIA->setActArea(ZAppInfoArea::no_progress_meter);
  setAppInfoArea(pAIA);
  myVBoxLayout->addWidget ( pAIA, 1, 1 );
  pAIA->appendText(ZAppInfoArea::activity, IDini.readEntry(QString("TEXT"), QString("INFO"), ""), "name");

  browser = new myListBox(QString("%C18%I%M"), myWidget, 0);
  browser->setFixedWidth(240);
  connect(browser, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(brClicked()));

  seleccionados=0;

  ventana = "browser";
  CargarBrowser();

  setContentWidget ( myWidget );

  softKey = new ZSoftKey ( "CST_2A", this, this );

  Create_MenuApp();
  this->setCSTWidget( softKey );


  int y = 0;
  y = y + this->headerSize().height();
  y = y + softKey->height();
  y = y + pAIA->height();
  
  browser->setFixedHeight(320-y); 
  browser->setFocus();

  QString val = IDini.readEntry(QString("NOTEPAD"), QString("NOTES"), "");
  pAIA->appendText(ZAppInfoArea::activity, QString("%1").arg(val), "name");	

  setMainWidgetTitle( val );

}

void ZGui::Create_MenuApp()
{
  QString IDval; ZConfig IDini(Idioma, false);
  QRect rect;
  int idx = 0;

  if ( ventana == "Reorder" )
  {
	softKey->setOptMenu ( ZSoftKey::LEFT, NULL );
	softKey->setText ( ZSoftKey::LEFT, IDini.readEntry(QString("NOTEPAD"), "SELECT", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
	softKey->setText ( ZSoftKey::RIGHT, IDini.readEntry(QString("NOTEPAD"), "DONE", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
	softKey->setClickedSlot ( ZSoftKey::LEFT, this, SLOT ( brClicked() ) );
	softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_quit() ) );

  }
  else
  {
	menusend = new ZOptionsMenu ( rect, softKey, NULL, 0);
	menusend->setItemSpacing(10);  
	IDval = IDini.readEntry(QString("NOTEPAD"), QString("SEND_MSG"), "");
	menusend->insertItem ( IDval, NULL, NULL, true, idx, idx );
	menusend->connectItem ( idx, this, SLOT ( sendtomsg() ) );

	menu1 = new ZOptionsMenu ( rect, softKey, NULL, 0);
	menu1->setItemSpacing(10);  
	IDval = IDini.readEntry(QString("NOTEPAD"), QString("CREATE_NOTE"), "");
	menu1->insertItem ( IDval, NULL, NULL, true, idx, idx );
	menu1->connectItem ( idx, this, SLOT ( createnote() ) );
	++idx;
	IDval = IDini.readEntry(QString("NOTEPAD"), QString("FLAG"), "");
	menu1->insertItem ( IDval, NULL, NULL, true, idx, idx );
	menu1->connectItem ( idx, this, SLOT ( marcar() ) );
	++idx;
	IDval = IDini.readEntry(QString("NOTEPAD"), QString("EDIT"), "");
	menu1->insertItem ( IDval, NULL, NULL, true, idx, idx );
	menu1->connectItem ( idx, this, SLOT ( editnote() ) );
	++idx;
	IDval = IDini.readEntry(QString("NOTEPAD"), QString("DELETE"), "");
	menu1->insertItem ( IDval, NULL, NULL, true, idx, idx );
	menu1->connectItem ( idx, this, SLOT ( deletenote() ) );
	++idx;
	IDval = IDini.readEntry(QString("NOTEPAD"), QString("SEND_TO"), "");
	menu1->insertItem ( IDval, menusend, NULL, true, idx, idx );
	++idx;
	IDval = IDini.readEntry(QString("NOTEPAD"), QString("REORDER"), "");
	menu1->insertItem ( IDval, NULL, NULL, true, idx, idx );
	menu1->connectItem ( idx, this, SLOT ( Reordenar() ) );
	++idx;

	connect(menu1, SIGNAL(aboutToShow()), SLOT(checkNote()));

	IDval = IDini.readEntry(QString("NOTEPAD"), QString("MENU"), "");
	softKey->setText ( ZSoftKey::LEFT, IDval, ( ZSoftKey::TEXT_PRIORITY ) 0 );
	softKey->setTextForOptMenuHide(IDval);
	IDval = IDini.readEntry(QString("NOTEPAD"), QString("SELECT"), "");
	QString IDval2 = IDini.readEntry(QString("NOTEPAD"), QString("CANCEL"), "");
	softKey->setTextForOptMenuShow( IDval, IDval2);
	IDval = IDini.readEntry(QString("NOTEPAD"), QString("EXIT"), "");
	softKey->setText ( ZSoftKey::RIGHT, IDval, ( ZSoftKey::TEXT_PRIORITY ) 0 );
	softKey->setOptMenu ( ZSoftKey::LEFT, menu1 );
	softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_quit() ) );

  }
  
}

void ZGui::checkNote()
{
	ZConfig IDini(Idioma, false);
	int index = browser->currentItem() + 1;
	ZConfig PREini("/ezxlocal/download/appwrite/setup/ezx_notepad.cfg", true);
	QString flag = PREini.readEntry(QString("NOTES"), QString("Note%1flag").arg(index), "");
	
	if ( flag == "Yes" )
	{
		menu1->changeItem(1, IDini.readEntry(QString("NOTEPAD"), QString("REMOVE_FLAG"), "") );
		menu1->setItemEnabled(1,true);
	}
	else if ( flag == "No" )
	{
		menu1->changeItem(1, IDini.readEntry(QString("NOTEPAD"), QString("ADD_FLAG"), "") );
		menu1->setItemEnabled(1,true);
	}
	else
	{
		menu1->changeItem(1, IDini.readEntry(QString("NOTEPAD"), QString("ADD_FLAG"), "") );
		menu1->setItemEnabled(1,false);
	}
}

QString ZGui::getProgramDir()
{
  ProgDir = QString ( qApp->argv() [0] ) ;
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );
  return ProgDir;
}

void ZGui::CargarBrowser()
{
  QPixmap p1;
  QImage image; 
  ZConfig PREini("/ezxlocal/download/appwrite/setup/ezx_notepad.cfg", true);
  ProgCount = PREini.readNumEntry(QString("NOTES"), QString("Count"), 0);

  browser->clear();
  int iconsize = 17;


/*  QFont f = browser->font();
  f.setPixelSize( 10 );
*/
/*  browser->setItemFont(ZListBox::LISTITEM_REGION_B, ZSkinBase::StStandard, f);
  browser->setItemFont(ZListBox::LISTITEM_REGION_B, ZSkinBase::StHighlighted, f);
  browser->setItemFont(ZListBox::LISTITEM_REGION_B, ZSkinBase::StSelected, f);
  browser->setItemFont(ZListBox::LISTITEM_REGION_B, ZSkinBase::StHighlightSelected, f);

*/
	  if ( ProgCount > 0 )
	  {
		for ( int j=0; j < ProgCount; ++j )
		{
			QPixmap pixmap;
			ZListBoxItem* listitem1 = new ZListBoxItem ( browser, QString ( "%I%M240%M20%M10%M10" ) );
			QString val = PREini.readEntry(QString("NOTES"), QString("Note%1text").arg(j+1), "");
			QString val2 = PREini.readEntry(QString("NOTES"), QString("Note%1flag").arg(j+1), "No");
			QString val3 = PREini.readEntry(QString("NOTES"), QString("Note%1date").arg(j+1), "");
			if ( val2 == "Yes" ) pixmap = QPixmap( iconReader.getIcon("gen_flag_small", false) );
			else pixmap.load("/ezxlocal/download/mystyff/.system/LinXtend/usr/bin/z6manager/null.png");
			QImage image; image = pixmap.convertToImage(); 
			image = image.smoothScale(iconsize, iconsize); pixmap.convertFromImage(image);
			listitem1->setPixmap ( 0, pixmap );
			listitem1->appendSubItem ( 1, val, false, 0 );
			listitem1->appendSubItem ( 2, val2, false, 0 );
			listitem1->appendSubItem ( 3, val3, false, 0 );
			listitem1->appendSubItem ( 4, "No", false, 0 );
			browser->insertItem ( listitem1, j, true );
		}
	  }


}


void ZGui::createnote()
{
	int index = browser->currentItem();
	ZSearch *zfod = new ZSearch(0);
	zfod->exec();
	delete zfod;
	zfod = NULL;
	CargarBrowser();
	browser->setCurrentItem(index,true);
}

void ZGui::editnote()
{
	int index = browser->currentItem() + 1;
	ZSearch *zfod = new ZSearch(index);
	zfod->exec();
	delete zfod;
	zfod = NULL;
	CargarBrowser();
	browser->setCurrentItem(index-1,true);

}

void ZGui::marcar()
{
	int index = browser->currentItem() + 1;
	ZConfig PREini("/ezxlocal/download/appwrite/setup/ezx_notepad.cfg", true);
	QString flag = PREini.readEntry(QString("NOTES"), QString("Note%1flag").arg(index), "No");
	
	if ( flag == "No" ) PREini.writeEntry(QString("NOTES"), QString("Note%1flag").arg(index), "Yes");
	else PREini.writeEntry(QString("NOTES"), QString("Note%1flag").arg(index), "No");

	CargarBrowser();
	browser->setCurrentItem(index-1,true);



}

void ZGui::deletenote()
{
	int index = browser->currentItem() + 1;

	ZConfig Wini("/ezxlocal/download/appwrite/setup/ezx_notepad.cfg", true);
	int TP = Wini.readNumEntry(QString("NOTES"), QString("Count"), 0);  
	if ( TP > 0 )
	{
		int index = browser->currentItem() + 1;
		system("cp /ezxlocal/download/appwrite/setup/ezx_notepad.cfg /tmp/ezx_notepad.cfg");
		ZConfig Wini2("/tmp/ezx_notepad.cfg", true);
		int pepe = 1;
		for ( int k=0; k<TP; ++k )
		{
		  if ( k == index-1 ) { ++pepe; }
			Wini.writeEntry ( QString("NOTES"), QString("Note%1flag").arg(k+1), Wini2.readEntry( QString("NOTES"), QString("Note%1flag").arg(pepe), "" ) );
			Wini.writeEntry ( QString("NOTES"), QString("Note%1text").arg(k+1), Wini2.readEntry( QString("NOTES"), QString("Note%1text").arg(pepe), "" ) );
			++pepe;
		}
		Wini.removeEntry ( QString("NOTES"), QString("Note%1flag").arg(TP) );
		Wini.removeEntry ( QString("NOTES"), QString("Note%1text").arg(TP) );
		system("rm /tmp/ezx_notepad.cfg");

		TP = TP - 1;
		Wini.writeEntry(QString("NOTES"), QString("Count"), QString("%1").arg(TP) );
		CargarBrowser();
		index = index - 1; if ( index == 0 ) index = 1;
		browser->setCurrentItem(index-1,true);
	}

}


void ZGui::brClicked()
{
  ZConfig IDini(Idioma, false);
  int index = browser->currentItem() + 1;


	if ( ventana == "Reorder" )
	{
		ZConfig IDini(Idioma, false);
		if ( ordenando == -1 )
		{
			qApp->processEvents();
			int index = browser->currentItem();
			if (index == -1) {return;}
			QString val;
			val = browser->item(index)->getSubItemText(1,0,true);
			QPixmap pixmap;
			pixmap = QPixmap( iconReader.getIcon("gen_reorder_arrw_small", false) );
			if ( pixmap.height() > 17 ) { QImage image; image = pixmap.convertToImage(); 
			image = image.smoothScale(17, 17); pixmap.convertFromImage(image); } 
			tPix = browser->item(index)->getPixmap(0);
			browser->item(index)->setPixmap ( 0, pixmap );
			browser->item(index)->setSubItem(4, 0, "Yes", false, NULL);
			softKey->setText ( ZSoftKey::LEFT, IDini.readEntry(QString("NOTEPAD"), "RELEASE", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
			ordenando=index;
		} else {
			QPixmap pixmap;
			RES_ICON_Reader iconReader; 
			int index = browser->currentItem();
			QString val = browser->item(index)->getSubItemText(2,0,true);
			if ( val == "Yes" ) pixmap = QPixmap( iconReader.getIcon("gen_flag_small", false) );
			else pixmap.load("/ezxlocal/download/mystyff/.system/LinXtend/usr/bin/z6manager/null.png");
			if ( pixmap.height() > 17 ) { QImage image; image = pixmap.convertToImage(); 
			image = image.smoothScale(17, 17); pixmap.convertFromImage(image); }     
			browser->item(index)->setPixmap ( 0, pixmap );
			browser->item(index)->setSubItem(4, 0, "No", false, NULL);
			softKey->setText ( ZSoftKey::LEFT, IDini.readEntry(QString("NOTEPAD"), "SELECT", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
			ordenando=-1;
		}
	}
	else
	{
		ZView *zfod = new ZView(index);
		zfod->exec();
		delete zfod;
		zfod = NULL;
		CargarBrowser();
		browser->setCurrentItem(index-1,true);
	}

}


void ZGui::sendtomsg()
{
  ZConfig Wini("/ezxlocal/download/appwrite/setup/ezx_notepad.cfg", true);
	int index = browser->currentItem();
	QString val = Wini.readEntry( QString("NOTES"), QString("Note%1text").arg(index+1), "" );
	ZSMS pepe;
	ZSMS_OutgoingMsg mail(QString(""),QString(val));
	pepe.composeMsg(mail);
}

void ZGui::Reordenar()
{
	ordenando = -1;
	ventana = "Reorder"; Create_MenuApp();
}

void ZGui::slot_quit()
{
	if ( ventana == "Reorder" )
	{
	  ZConfig ini("/ezxlocal/download/appwrite/setup/ezx_notepad.cfg", true); 
	  int cpr = ini.readNumEntry(QString("NOTES"), QString("Count"), 0);  
	  if ( cpr > 0 )
	  {
		for ( int j=0; j < cpr; ++j )
		{
			QString val = browser->item(j)->getSubItemText(1,0,true);
			ini.writeEntry( QString("NOTES"), QString("Note%1text").arg(j+1), QString(val) );
			val = browser->item(j)->getSubItemText(2,0,true);
			ini.writeEntry( QString("NOTES"), QString("Note%1flag").arg(j+1), QString(val) );
			val = browser->item(j)->getSubItemText(3,0,true);
			ini.writeEntry( QString("NOTES"), QString("Note%1date").arg(j+1), QString(val) );
		}
	  }
	  ventana = "browser";
	  Create_MenuApp();
	  CargarBrowser();
	}
	else qApp->quit();
}


