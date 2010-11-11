#include "myam.h"
#include "ZDefs.h"
#include "zCallDlg.h"
#include "ZMsg.h"
#include <qpixmap.h>
#include <qlabel.h>
#include <ZApplication.h>
#include <ZSoftKey.h>
#include "ScreenX.h"
#include <string.h>
#include <ZSingleCaptureDlg.h>
#include <ZScrollView.h>
#include <ZMessageDlg.h>
#include <ZNoticeDlg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <qfile.h>
#include <qdir.h>
#include <qlist.h>
#include <string.h>
#include <qtextstream.h>
#include <qvector.h>
#include <qtextcodec.h>
#include <ezxres.h>

QString ProgDir1;
QString lang;
QString num;

QCopChannel * notificaciones2;
int mcall, mmsg;

extern ScreenX * screenx;
extern ZApplication * app;


zCalls::zCalls(QString Idioma)
:ZPopup()
{


}


zCalls::~zCalls()
{

}

void zCalls::CreateDialog(QString Idioma)
{
	QString Fval1; ZConfig Fini1(QString("/ezxlocal/download/appwrite/setup/ezx_theme.cfg"), true);
	Fval1 = Fini1.readEntry(QString("THEME_GROUP_LOG"), QString("CurrentSkin"), "");
	ZConfig Fini3(Fval1 + "/phone_p.ini", false);
	int TextColor = Fini3.readNumEntry(QString("Font_DialingSmallStd"), QString("Color"), 0);

	lang = Idioma;
	//num = numero;
	setTitleIcon("call_enter_thb");
	ZConfig IDini(lang, true);
	QFont fuente; fuente.setPixelSize(30);
	setInstructFont(fuente);
	setInstructFontColor(QRgb(TextColor));
	setSoftkeyTextLeft(IDini.readEntry(QString("CALLS"), QString("CALL"), ""));
	setSoftkeyTextRight(IDini.readEntry(QString("CALLS"), QString("CANCEL"), ""));
	setTitle(IDini.readEntry(QString("CALLS"), QString("DIAL"), ""));
	
	//setInstructText(num);
	qApp->processEvents();

}

void zCalls::insertText(QString numero)
{
	setInstructText("");
	num = numero;
	setInstructText(num);
}

void zCalls::keyPressEvent(QKeyEvent* k)
{
	if ( ( (k->key() >= 0x0030) && (k->key() <= 0x0039) ) || (k->key() <= 0x0023) || (k->key() <= 0x002a) )
	{
		if ( k->key() == KEY_0 ) num += "0"; if ( k->key() == KEY_1 ) num += "1";
		if ( k->key() == KEY_2 ) num += "2"; if ( k->key() == KEY_3 ) num += "3";
		if ( k->key() == KEY_4 ) num += "4"; if ( k->key() == KEY_5 ) num += "5";
		if ( k->key() == KEY_6 ) num += "6"; if ( k->key() == KEY_7 ) num += "7";
		if ( k->key() == KEY_8 ) num += "8"; if ( k->key() == KEY_9 ) num += "9";
		if ( k->key() == KEY_AST ) num += "*"; if ( k->key() == KEY_NUM ) num += "#";
		setInstructText(num);
		qApp->processEvents();
	}
	else if ( k->key() == Z6KEY_C )
	{
		if ( num.length() > 0) num.remove( num.length() -1, 1);
		setInstructText(num);
		qApp->processEvents();
		if ( num == "" ) reject();

	}
	else if ( ( k->key() == Z6KEY_RIGHT_SOFTKEY ) || ( k->key() == Z6KEY_RED ) || ( k->key() == SLIDER_CLOSE ) )
	{
		reject();
	}
	else if ( ( k->key() == Z6KEY_LEFT_SOFTKEY ) || ( k->key() == Z6KEY_GREEN ) )
	{
		accept();
	}

	if (num == "*#06#")
	{
		screenx->showIMEI();
	}

	if (num == "*#9999#")
	{
		screenx->showFLEX();
	}


}

void zCalls::reject()
{
	screenx->cancelcall();
	this->hide();
}

void zCalls::accept()
{
	PHONE_VoiceCallSender pepe;
	pepe.makeCall(num, 0);
	reject();
}






zProfile::zProfile(QString Idioma)
:ZPopup()
{

}

void zProfile::CreateDialog(QString Idioma)
{
	lang = Idioma;
	ZConfig IDini(lang, true);

	SETUP_Utility pepe;
	ENUM_ALERT_STYLE xi; pepe.getAlertStyle(xi);
	int current = xi;

	setSoftkeyTextLeft(IDini.readEntry(QString("COMMON"), QString("MENU_SELECT"), ""));
	setSoftkeyTextRight(IDini.readEntry(QString("COMMON"), QString("MENU_CANCEL"), ""));

	setTitleIcon("action_required_pop");
	setTitle(IDini.readEntry(QString("ALERT"), QString("TITLE"), ""));
	listboxx2 = new ZListBox ( QString ( "%R%I%M" ), this, 0);
	QPixmap p1; RES_ICON_Reader iconReader;

	p1 = QPixmap(iconReader.getIcon("alert_ring_idle8", false));
	ZListBoxItem* listitem = new ZListBoxItem ( listboxx2, QString ( "%R%I20%M" ) );
	listitem->setPixmap ( 1, p1 );
	listitem->appendSubItem ( 2, IDini.readEntry(QString("ALERT"), QString("RING"), ""), false, 0 );
	listboxx2->insertItem ( listitem, 0, true );

	p1 = QPixmap(iconReader.getIcon("alert_vibrate_idle8", false));
	ZListBoxItem* listitem2 = new ZListBoxItem ( listboxx2, QString ( "%R%I20%M" ) );
	listitem2->setPixmap ( 1, p1 );
	listitem2->appendSubItem ( 2, IDini.readEntry(QString("ALERT"), QString("VIB"), ""), false, 0 );
	listboxx2->insertItem ( listitem2, 1, true );

	p1 = QPixmap(iconReader.getIcon("alert_silent_idle8", false));
	ZListBoxItem* listitem3 = new ZListBoxItem ( listboxx2, QString ( "%R%I20%M" ) );
	listitem3->setPixmap ( 1, p1 );
	listitem3->appendSubItem ( 2, IDini.readEntry(QString("ALERT"), QString("SILENT"), ""), false, 0 );
	listboxx2->insertItem ( listitem3, 2, true );

	p1 = QPixmap(iconReader.getIcon("alert_ring_n_vib_idle8", false));
	ZListBoxItem* listitem4 = new ZListBoxItem ( listboxx2, QString ( "%R%I20%M" ) );
	listitem4->setPixmap ( 1, p1 );
	listitem4->appendSubItem ( 2, IDini.readEntry(QString("ALERT"), QString("RING_VIB"), ""), false, 0 );
	listboxx2->insertItem ( listitem4, 3, true );

	p1 = QPixmap(iconReader.getIcon("alert_vib_then_ring_idle8", false));
	ZListBoxItem* listitem5 = new ZListBoxItem ( listboxx2, QString ( "%R%I20%M" ) );
	listitem5->setPixmap ( 1, p1 );
	listitem5->appendSubItem ( 2, IDini.readEntry(QString("ALERT"), QString("VIB_RING"), ""), false, 0 );
	listboxx2->insertItem ( listitem5, 4, true );

	if ( current == 0 ) listboxx2->checkItem(0, true);
	if ( current == 3 ) listboxx2->checkItem(1, true);
	if ( current == 4 ) listboxx2->checkItem(2, true);
	if ( current == 1 ) listboxx2->checkItem(3, true);
	if ( current == 2 ) listboxx2->checkItem(4, true);

	insertChild(listboxx2, 0);

	connect(listboxx2, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(brClicked()));

}

zProfile::~zProfile()
{
  
}

void zProfile::accept()
{
	brClicked();
}

void zProfile::keyPressEvent(QKeyEvent* k)
{
	if ( ( k->key() == Z6KEY_RED ) || ( k->key() == SLIDER_CLOSE ) )
	{
		reject();
	}
}

void zProfile::brClicked()
{
	qApp->processEvents();
	SETUP_Utility pepe2;
	if (listboxx2->currentItem() == 0 ) { pepe2.setActiveAlertStyle( (ENUM_ALERT_STYLE)0 ); }
	if (listboxx2->currentItem() == 1 ) { pepe2.setActiveAlertStyle( (ENUM_ALERT_STYLE)3 ); }
	if (listboxx2->currentItem() == 2 ) { pepe2.setActiveAlertStyle( (ENUM_ALERT_STYLE)4 ); }
	if (listboxx2->currentItem() == 3 ) { pepe2.setActiveAlertStyle( (ENUM_ALERT_STYLE)1 ); }
	if (listboxx2->currentItem() == 4 ) { pepe2.setActiveAlertStyle( (ENUM_ALERT_STYLE)2 ); }
	reject();
}

void zProfile::reject()
{
	screenx->cancelcall();
	this->hide();
}







zLock::zLock(QString Idioma)
:ZPopup()
{
	lang = Idioma;
	ZConfig IDini(lang, true);
	setSoftkeyTextLeft(IDini.readEntry(QString("COMMON"), QString("MENU_ACCEPT"), ""));
	setSoftkeyTextRight("");
	setTitle(IDini.readEntry(QString("TEXT"), QString("UNLOCK_TITLE"), ""));
	setInstructText(IDini.readEntry(QString("TEXT"), QString("UNLOCK_TEXT"), ""));

	connect( app, SIGNAL(sliderStatusChanged(int)), this, SLOT( reject() ) );
}


zLock::~zLock()
{
  
}


void zLock::keyPressEvent(QKeyEvent* k)
{
	if ( k->key() == Z6KEY_LEFT_SOFTKEY ) 
	{
		unlock();
	}
	else
	{
		reject();
	}

}

void zLock::unlock()
{
	accept();
}





myListBox::myListBox(QString type, QWidget* parent, WFlags f, ZSkinService::WidgetClsID clsId)
:ZListBox( type, parent, 0 )
{
	
}

myListBox::~myListBox()
{

}

void myListBox::keyPressEvent(QKeyEvent* k)
{
	if ( k->key() == Z6KEY_UP )
	{
		int index = currentItem();
		QString val = item(index)->getSubItemText(4,0,true);
		if ( val=="Yes" ) 
		{
		  if ( index > 0 ) moveItem(index-1,index);
		}
		else ZListBox::keyPressEvent(k);
	}
	else if ( k->key() == Z6KEY_DOWN )
	{
		int index = currentItem();
		QString val = item(index)->getSubItemText(4,0,true);
		if ( val=="Yes" ) 
		{
			if ( index < count() ) moveItem(index+1,index);
		}
		else ZListBox::keyPressEvent(k);
	}
	else
	{
      ZListBox::keyPressEvent(k);
	}

}






zMissed::zMissed(QString Idioma, int misscall, int missmsg)
:ZPopup()
{
	lang = Idioma;
	ProgDir1 = QString ( qApp->argv() [0] ) ;
	int i = ProgDir1.findRev ( "/" );
	ProgDir1.remove ( i+1, ProgDir1.length() - i );

	listboxx2 = new ZListBox ( QString ( "%R%I%M" ), this, 0);

	mcall = misscall;
	mmsg = missmsg;

	ZConfig IDini(lang, true);

	setSoftkeyTextLeft(IDini.readEntry(QString("COMMON"), QString("MENU_SELECT"), ""));
	setSoftkeyTextRight(IDini.readEntry(QString("COMMON"), QString("MENU_CANCEL"), ""));

	setTitleIcon("alert_pop");
	setTitle(IDini.readEntry(QString("NOTIFY"), QString("TITLE"), ""));

	insertChild(listboxx2, 0);
	connect(listboxx2, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(brClicked()));

	CreateDialog();

	if ( QCopChannel::isRegistered("UNS/Server") )
	{
		notificaciones2 = new QCopChannel("UNS/Server", this); 
		connect( notificaciones2, SIGNAL(received(const QCString &,const QByteArray &)), this, SLOT(NotiNueva(const QCString &,const QByteArray &)) ); 
	}



}

void zMissed::CreateDialog()
{
	ZConfig IDini(lang, true);
	QPixmap p1; RES_ICON_Reader iconReader;
	listboxx2->clear();

	if ( mcall==1 )
	{
		p1 = QPixmap(iconReader.getIcon("recnt_missed_call_small", false));
		ZListBoxItem* listitem = new ZListBoxItem ( listboxx2, QString ( "%I20%M" ) );
		listitem->setPixmap ( 0, p1 );
		listitem->appendSubItem ( 1, IDini.readEntry(QString("NOTIFY"), QString("MISSEDCALL"), ""), false, 0 );
		listboxx2->insertItem ( listitem, 0, true );
	}
	if ( mmsg==1 )
	{
		p1 = QPixmap(iconReader.getIcon("msg_msgemail_unread_small", false));
		ZListBoxItem* listitem2 = new ZListBoxItem ( listboxx2, QString ( "%I20%M" ) );
		listitem2->setPixmap ( 0, p1 );
		listitem2->appendSubItem ( 1, IDini.readEntry(QString("NOTIFY"), QString("MISSEDMSG"), ""), false, 0 );
		listboxx2->insertItem ( listitem2, 1, true );
	}

}

zMissed::~zMissed()
{
  
}

void zMissed::NotiNueva(const QCString &mensaje, const QByteArray &datos)
{
	QDataStream stream(datos, IO_ReadOnly);
	int key, type;
	stream >> key >> type; 

	if( mensaje == "addItem(int,...)")
	{
		if (key==0) mcall=1;
		if (key==3) mmsg=1;
	}
	if( mensaje == "removeItem(int,int)")
	{
		if (key==0) mcall=0;
		if (key==3) mmsg=0;
	}
	if ( (mcall==1) || (mmsg==1) ) 
	{
		CreateDialog();
	}

}
void zMissed::accept()
{
	brClicked();
}

void zMissed::keyPressEvent(QKeyEvent* k)
{
	if ( ( k->key() == Z6KEY_RED ) || ( k->key() == SLIDER_CLOSE ) )
	{
		reject();
	}
}

void zMissed::brClicked()
{
	ZConfig IDini(lang, true);
	QString app = listboxx2->item ( listboxx2->currentItem() )->getSubItemText ( 1, 0, true );
	QString val = IDini.readEntry(QString("NOTIFY"), QString("MISSEDCALL"), "");
	if ( app==val )
	{
		listboxx2->removeItem( listboxx2->currentItem() );
		QString run = "{ce59717d-fb23-4b7c-8800-25ce613f4042}";
		ZMsg msg( "/AM/Launcher", "launchApp" ); msg << (QUuid)run; msg.send();
        QByteArray out; QDataStream outSrm(out, IO_WriteOnly); outSrm << 0 << 0;
        QCopChannel::send("UNS/Server", "removeItem(int,int)", out);
	}
	else
	{
		system(("/ezxlocal/LinXtend/usr/bin/inbox &"));
		listboxx2->removeItem( listboxx2->currentItem() );
        QByteArray out; QDataStream outSrm(out, IO_WriteOnly); outSrm << 3 << 3;
        QCopChannel::send("UNS/Server", "removeItem(int,int)", out);
	}
	if ( listboxx2->count() == 0 )
	{
		screenx->clearNoti();
		reject();
	}
}

void zMissed::reject()
{
	QByteArray out; QDataStream outSrm(out, IO_WriteOnly); outSrm << 0 << 0;
	QCopChannel::send("UNS/Server", "removeItem(int,int)", out);
	QByteArray out2; QDataStream outSrm2(out2, IO_WriteOnly); outSrm2 << 3 << 3;
	QCopChannel::send("UNS/Server", "removeItem(int,int)", out2);
	screenx->clearNoti();
	this->hide();
}

