//
// C++ Implementation: ZFileDlg
//
// Description: 
//
//
// Author: root <root@andLinux>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "ZFileDlg.h"
#include "zgui.h"
#include "myam.h"
#include <qpixmap.h>
#include <qlabel.h>
#include <ZApplication.h>
#include <ZSoftKey.h>
#include <ZSingleSelectDlg.h>
#include <ZScrollView.h>
#include <ZNumPickerDlg.h>

#include <qfile.h>
#include <qfileinfo.h>
#include <qtextstream.h>
#include <qtextcodec.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <qpixmap.h>
#include <ezxres.h>

QString ProgDir; QString Idioma2; QString FStext;
int SelectProg=0;
bool chang;
int current;


//************************************************************************************************
//*************************************** class ZSearch ******************************************
//************************************************************************************************
ZSearch::ZSearch(int nota)
  :MyBaseDlg()
{
  QString ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  /*ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma2 = "/ezxlocal/download/mystuff/.system/LinXtend/usr/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma2).isFile() ) Idioma2 = "/ezxlocal/download/mystuff/.system/LinXtend/usr/languages/en-us";*/

  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma2 = ProgDir + "/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma2).isFile() ) Idioma2 = ProgDir + "/languages/en-us";

  ZConfig lang (Idioma2,false); 

  this->setMainWidgetTitle( lang.readEntry(QString("NOTEPAD"), QString("NOTES"), "") );
  
  current = nota;

  QWidget *myWidget = new ZWidget ( this, NULL, 0);
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );
  myWidget->setFixedWidth(240);
  ZAppInfoArea* pAIA = new ZAppInfoArea(myWidget, NULL, 0);
  pAIA->setActArea(ZAppInfoArea::no_progress_meter);
  pAIA->appendText(ZAppInfoArea::activity, lang.readEntry(QString("LANGUAGE"), "SEARCH", ""), "name");
  setAppInfoArea(pAIA);
  myVBoxLayout->addWidget ( pAIA, 1, 1 );

    ZFormContainer* fc = new ZFormContainer(this);
    setContentWidget(fc);

    lineEdit = new ZMultiLineEdit(this, true, 4);
    lineEdit->setTitle(lang.readEntry(QString("NOTEPAD"), "NOTES", ""));
    lineEdit->setTitlePosition(ZMultiLineEdit::TitleTop);
    //lineEdit->setMaxLength(50);
	lineEdit->setText("");
    fc->addChild(lineEdit);

	
	if ( current == 0 )
	{
		marcado = new ZMyCheckBox(lang.readEntry(QString("NOTEPAD"), QString("FLAG_NOTE"), ""),this,"",false);
		fc->addChild(marcado);
	}
	else
	{
		ZConfig PREini("/ezxlocal/download/appwrite/setup/ezx_notepad.cfg", true);
		QString val = PREini.readEntry(QString("NOTES"), QString("Note%1text").arg(current), "");
		lineEdit->setText(val);
		val = PREini.readEntry(QString("NOTES"), QString("Note%1flag").arg(current), "");
		if ( val == "Yes" ) 
		{
			marcado = new ZMyCheckBox(lang.readEntry(QString("NOTEPAD"), QString("FLAG_NOTE"), ""),this,"",true);
			fc->addChild(marcado);
		}
		else
		{
			marcado = new ZMyCheckBox(lang.readEntry(QString("NOTEPAD"), QString("FLAG_NOTE"), ""),this,"",false);
			fc->addChild(marcado);
		}
	}

	connect(lineEdit, SIGNAL(textChanged()), SLOT(checktext()));

	softKey = new ZSoftKey ( "CST_2A", this, this );

	QRect rect; int idx = 0;
	menu1 = new ZOptionsMenu ( rect, softKey, NULL, 0);
	menu1->setItemSpacing(10);  
	menu1->insertItem ( lang.readEntry(QString("NOTEPAD"), QString("CANCEL"), ""), NULL, NULL, true, idx, idx );
	menu1->connectItem ( idx, this, SLOT ( slot_cancel() ) );

	softKey->setText ( ZSoftKey::LEFT, lang.readEntry(QString("NOTEPAD"), "MENU", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
	if ( current == 0 )
	{
		softKey->setText ( ZSoftKey::RIGHT, lang.readEntry(QString("NOTEPAD"), "CANCEL", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
		softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_cancel() ) );
	}
	else
	{
		softKey->setText ( ZSoftKey::RIGHT, lang.readEntry(QString("NOTEPAD"), "SAVE", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
		softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_save() ) );
	}

  softKey->setOptMenu ( ZSoftKey::LEFT, menu1 );

  this->setCSTWidget( softKey );

}


ZSearch::~ZSearch()
{
  
}

void ZSearch::checktext()
{
  ZConfig lang (Idioma2,false); 
  if ( lineEdit->text() == "" )
  {
	softKey->setText ( ZSoftKey::RIGHT, lang.readEntry(QString("NOTEPAD"), "CANCEL", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
	softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_cancel() ) );
  }
  else
  {
	softKey->setText ( ZSoftKey::RIGHT, lang.readEntry(QString("NOTEPAD"), "SAVE", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
	softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_save() ) );
  }

}


void ZSearch::slot_cancel()
{
  reject();
}


void ZSearch::slot_save()
{



	ZConfig Wini("/ezxlocal/download/appwrite/setup/ezx_notepad.cfg", true);
	int TP = Wini.readNumEntry(QString("NOTES"), QString("Count"), 0);  

	QDate fecha = QDate::currentDate();
	int d = fecha.day(); int m = fecha.month(); int y = fecha.year();
	QString d2; if (d<10) d2=QString("0%1").arg(d); else d2=QString("%1").arg(d);
	QString m2; if (m<10) m2=QString("0%1").arg(m); else m2=QString("%1").arg(m);

	QString tf;
	ZConfig Fini4(QString("/ezxlocal/download/appwrite/setup/ezx_system.cfg"), false);
	int DateFormat = Fini4.readNumEntry(QString("SYS_SYSTEM_SET"), QString("DateFormat"), 0);
	if ( DateFormat==0) { tf = QString("%1/%2/%3").arg(m2).arg(d2).arg(y); }
	else if ( DateFormat==1) { tf = QString("%1/%2/%3").arg(y).arg(m2).arg(d2); }
	else if ( DateFormat==2) { tf = QString("%1/%2/%3").arg(d2).arg(m2).arg(y); }

	if ( current == 0 )
	{
		Wini.writeEntry ( QString("NOTES"), QString("Note%1text").arg(TP+1), lineEdit->text() );
		Wini.writeEntry ( QString("NOTES"), QString("Note%1date").arg(TP+1), tf );
		if ( marcado->GetChecked() ) Wini.writeEntry ( QString("NOTES"), QString("Note%1flag").arg(TP+1), "Yes" );
		else Wini.writeEntry ( QString("NOTES"), QString("Note%1flag").arg(TP+1), "No" );
		TP = TP + 1;
		Wini.writeEntry(QString("NOTES"), QString("Count"), QString("%1").arg(TP) );
	}
	else
	{
		Wini.writeEntry ( QString("NOTES"), QString("Note%1text").arg(current), lineEdit->text() );
		Wini.writeEntry ( QString("NOTES"), QString("Note%1date").arg(current), tf );
		if ( marcado->GetChecked() ) Wini.writeEntry ( QString("NOTES"), QString("Note%1flag").arg(current), "Yes" );
		else Wini.writeEntry ( QString("NOTES"), QString("Note%1flag").arg(current), "No" );
	}
	reject();

}



//************************************************************************************************
//**************************************** class ZView *******************************************
//************************************************************************************************
ZView::ZView(int nota)
  :MyBaseDlg()
{
  QString ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  /*ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma2 = "/ezxlocal/download/mystuff/.system/LinXtend/usr/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma2).isFile() ) Idioma2 = "/ezxlocal/download/mystuff/.system/LinXtend/usr/languages/en-us";*/

  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma2 = ProgDir + "/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma2).isFile() ) Idioma2 = ProgDir + "/languages/en-us";

  ZConfig lang (Idioma2,false); 

  this->setMainWidgetTitle( lang.readEntry(QString("NOTEPAD"), QString("NOTES"), "") );
  
  current = nota;


    ZFormContainer* fc = new ZFormContainer(this);
    setContentWidget(fc);

    ZLabel* lineT = new ZLabel(this);
	lineT->setText( QString( lang.readEntry(QString("NOTEPAD"), "NOTES", "") + ":") );
    fc->addChild(lineT);

	QFont ft = lineT->font();
	ft.setPixelSize(14);
	ft.setBold(true);
	lineT->setFont(ft);

    lineEdit = new ZLabel(this);
	ZConfig PREini("/ezxlocal/download/appwrite/setup/ezx_notepad.cfg", true);
	QString val = PREini.readEntry(QString("NOTES"), QString("Note%1text").arg(current), "");
	lineEdit->setText(val);
    fc->addChild(lineEdit);

	QFont f = lineEdit->font();
	f.setPixelSize(14);
	lineEdit->setFont(f);

	lineEdit->setAutoResize(true);

	ZSeparator* sep = new ZSeparator();
	fc->addChild(sep);

    ZLabel* lineM1 = new ZLabel(this);
	lineM1->setText( QString( lang.readEntry(QString("NOTEPAD"), "LAST_UPDATE", "") + ":") );
    fc->addChild(lineM1);
	lineM1->setFont(ft);

    lineM2 = new ZLabel(this);
	lineM2->setText( PREini.readEntry(QString("NOTES"), QString("Note%1date").arg(current), "") );
    fc->addChild(lineM2);
	lineM2->setFont(f);
	
	softKey = new ZSoftKey ( "CST_2A", this, this );

	QRect rect; int idx = 0;

	ZOptionsMenu* menusend = new ZOptionsMenu ( rect, softKey, NULL, 0);
	menusend->setItemSpacing(10);  
	QString IDval = lang.readEntry(QString("NOTEPAD"), QString("SEND_MSG"), "");
	menusend->insertItem ( IDval, NULL, NULL, true, idx, idx );
	menusend->connectItem ( idx, this, SLOT ( slot_send() ) );

	menu1 = new ZOptionsMenu ( rect, softKey, NULL, 0);
	menu1->setItemSpacing(10);  
	menu1->insertItem ( lang.readEntry(QString("NOTEPAD"), QString("EDIT"), ""), NULL, NULL, true, idx, idx );
	menu1->connectItem ( idx, this, SLOT ( slot_edit() ) ); ++idx;
	menu1->insertItem ( lang.readEntry(QString("NOTEPAD"), QString("SEND_TO"), ""), menusend, NULL, true, idx, idx ); ++idx;
	menu1->insertItem ( lang.readEntry(QString("NOTEPAD"), QString("DELETE"), ""), NULL, NULL, true, idx, idx );
	menu1->connectItem ( idx, this, SLOT ( slot_delete() ) ); ++idx;

	softKey->setText ( ZSoftKey::LEFT, lang.readEntry(QString("NOTEPAD"), "MENU", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
	softKey->setText ( ZSoftKey::RIGHT, lang.readEntry(QString("NOTEPAD"), "GOBACK", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
	softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( reject() ) );

  softKey->setOptMenu ( ZSoftKey::LEFT, menu1 );

  this->setCSTWidget( softKey );

}


ZView::~ZView()
{
  
}

void ZView::slot_delete()
{
	int index = current;

	ZConfig Wini("/ezxlocal/download/appwrite/setup/ezx_notepad.cfg", true);
	int TP = Wini.readNumEntry(QString("NOTES"), QString("Count"), 0);  
	if ( TP > 0 )
	{
		int index = current;
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
		reject();
	}
}

void ZView::slot_edit()
{
	int index = current;
	ZSearch *zfod = new ZSearch(index);
	zfod->exec();
	delete zfod;
	zfod = NULL;

	reject();

}

void ZView::slot_send()
{
	ZConfig Wini("/ezxlocal/download/appwrite/setup/ezx_notepad.cfg", true);
	int index = current-1;
	QString val = Wini.readEntry( QString("NOTES"), QString("Note%1text").arg(index+1), "" );
	ZSMS pepe;
	ZSMS_OutgoingMsg mail(QString(""),QString(val));
	pepe.composeMsg(mail);
}


/**********************************************************************/
/**********************************************************************/
ZMyCheckBox::ZMyCheckBox(const QString & text, QWidget * parent, const char* name, bool value) : ZCheckBox ( text, parent, name ) {
    curState = value;
if (value)
	{
	    setState((ZBaseButton::ToggleState)2);
	}
	else
	{
	    setState((ZBaseButton::ToggleState)0);
	}
    connect (this,SIGNAL(toggled(bool)),this,SLOT(SetChecked(bool)));
}

ZMyCheckBox::~ZMyCheckBox()
{

}

void ZMyCheckBox::SetChecked(bool state)
{
    curState = state;
    chang = true;
}

bool ZMyCheckBox::GetChecked() const
{
     return curState;
}
