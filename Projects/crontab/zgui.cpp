#include "zgui.h"
#include "ZFileDlg.h"
#include <ZConfig.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <qpixmap.h>
#include <string.h>
#include <ZOptionsMenu.h>
#include <ZApplication.h>
#include <ZListBox.h>
#include <ZSoftKey.h>
#include <qtextcodec.h>
#include <ezxres.h>
#include <qtextstream.h>
#include <ZSingleCaptureDlg.h>
#include <ZMessageDlg.h>
#include <ZDatePickerDlg.h>
#include <qdatetime.h>
#include <qfileinfo.h>

QString Idioma;
QTextCodec* txtcodec = QTextCodec::codecForName("UTF-8");
RES_ICON_Reader iconReader;
int Cambio;

ZGui::ZGui ( const QString &argument, QWidget* parent, const char* name, WFlags fl )
    : ZKbMainWidget ( ZHeader::FULL_TYPE, NULL, "ZMainWidget", 0 )
{
  Cambio=0;
  QString val; 
  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma = "ezxlocal/download/mystuff/.system/LinXtend/usr/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma).isFile() ) Idioma = "/ezxlocal/download/mystuff/.system/LinXtend/usr/languages/en-us";

  CreateWindow ( parent );
  ( ( ZApplication* ) qApp )->showMainWidget ( this );
	
}

ZGui::~ZGui()
{

}


void ZGui::CreateWindow ( QWidget* parent )
{
	ZConfig ini(Idioma, false);
	QString val;

	zllb1 = new ZListBox ( QString ( "%I%M" ), this, 0); zllb1->setFixedWidth ( 240 ); CargarMenu1();
	connect(zllb1, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(appClicked1()));

    setMainWidgetTitle ( "Cron" );

	setContentWidget ( zllb1 );

	softKey = new ZSoftKey ( NULL , this , this );
	Create_MenuApp();
	Create_SoftKey();
	setSoftKey ( softKey );

	zllb1->setFocus();

}

QString ZGui::getProgramDir()
{
  ProgDir = QString ( qApp->argv() [0] ) ;
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );
  return ProgDir;
}

QString ZGui::texto(const char*xString)
{
	ZConfig IDini(Idioma, false);	
	QString	IDval = IDini.readEntry(QString("SETTINGS"), QString("UNICODE"), "");
	if ( IDval == "1" ) { return txtcodec->toUnicode(xString); }
	else { return xString; }
}

void ZGui::CargarMenu1()
{
  QPixmap p1;
  QString line;
  QFile entrada("/ezxlocal/LinXtend/usr/bin/cron/user.txt");
  QTextStream stentrada(&entrada);
  if ( entrada.open(IO_ReadOnly | IO_Translate) )
  {
	while ( !stentrada.eof() )
	{
	  line = stentrada.readLine();
	  if ( line != "" )
	  {
		  ZListBoxItem* autoitem = new ZListBoxItem ( zllb1, QString ( "%I%M" ) );
		  p1=QPixmap(iconReader.getIcon("calen_schedule_tab", false));
		  QImage image; image = p1.convertToImage(); 
		  image = image.smoothScale(18, 17); p1.convertFromImage(image);
		  autoitem->setPixmap ( 0, p1 );
		  QString name = line;
		  int i = name.find("/",0); name.remove(0,i);
		  i= name.findRev("/"); if ( i!=name.length() ) name.remove(0,i+1);
		  autoitem->appendSubItem ( 1, name, false, NULL );
		  QString campos = line;
		  i = campos.findRev( QChar(' ') ); campos.remove(i,campos.length()-i);
		  autoitem->appendSubItem ( 1, campos, false, NULL );
		  zllb1->insertItem ( autoitem,-1,true );
      }
	}
  }

}

void ZGui::Create_MenuApp()
{
  QString IDval; ZConfig IDini(Idioma, false);
  QRect rect;   QPixmap* p1 = new QPixmap();
  menu = new ZOptionsMenu ( rect, softKey, NULL, 0);
  menu->setItemSpacing(10); int idx=0;

  p1 = new QPixmap(iconReader.getIcon("", false));
  IDval = IDini.readEntry(QString("CRONTAB"), QString("MENU_ADD"), "");
  menu->insertItem ( IDval, NULL, p1, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( addtask() ) );      
  ++idx;
  p1 = new QPixmap(iconReader.getIcon("", false));
  IDval = IDini.readEntry(QString("CRONTAB"), QString("MENU_REMOVE"), "");
  menu->insertItem ( IDval, NULL, p1, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( removetask() ) );      
  ++idx;
  menu->insertSeparator(idx, idx); 
  ++idx;
  p1 = new QPixmap(iconReader.getIcon("", false));
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_ABOUT"), "");
  menu->insertItem ( IDval, NULL, p1, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( about() ) );      
  ++idx;
  p1 = new QPixmap(iconReader.getIcon("", false));
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_EXIT"), "");
  menu->insertItem ( IDval, NULL, p1, true, idx, idx );
  menu->connectItem ( idx, qApp, SLOT ( quit() ) );
}

void ZGui::Create_SoftKey()
{
  QString IDval; QString IDval2; ZConfig IDini(Idioma, false);
  softKey->setOptMenu ( ZSoftKey::LEFT, menu ); 
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_MENU"), "");
  softKey->setText ( ZSoftKey::LEFT, IDval, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_MENU"), "");
  softKey->setTextForOptMenuHide(IDval);
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_SELECT"), "");
  IDval2 = IDini.readEntry(QString("COMMON"), QString("MENU_CANCEL"), "");
  softKey->setTextForOptMenuShow(IDval, IDval2);
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_EXIT"), "");
  softKey->setText ( ZSoftKey::RIGHT, IDval, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, qApp, SLOT ( quit() ) );      

}

void ZGui::appClicked1()
{
	qApp->processEvents();
	int index = zllb1->currentItem();
	if (index == -1) {return;}
	int result = index+1;
	QString app = zllb1->item(index)->getSubItemText(1,0,true);

	QString line, TEXTO;
	QFile entrada("/ezxlocal/LinXtend/usr/bin/cron/user.txt");
	QTextStream stentrada(&entrada);
	if ( entrada.open(IO_ReadOnly | IO_Translate) )
	{
	  while ( !stentrada.eof() )
	  {
		line = stentrada.readLine();
		if ( line != "" )
		{
		  QString name = line;
		  int i = name.find("/",0); name.remove(0,i);
		  i= name.findRev("/"); if ( i!=name.length() ) name.remove(0,i+1);
		  if ( name == app )
		  {
			QString minuto = line; int j=0;
			i = minuto.find(" ",0); minuto.remove(i,minuto.length()-i); j=j+i+1;
			system(QString("echo \"%1\"").arg(minuto));
			QString hora = line; hora.remove(0,j); 
			system(QString("echo \"%1\"").arg(hora));
			i = hora.find(" ",0); hora.remove(i,hora.length()-i); j=j+i+1;
			QString dia1 = line; dia1.remove(0,j);
			system(QString("echo \"%1\"").arg(dia1));
			i = dia1.find(" ",0); dia1.remove(i,dia1.length()-i); j=j+i+1;
			QString mes = line; mes.remove(0,j);
			system(QString("echo \"%1\"").arg(mes));
			i = mes.find(" ",0); mes.remove(i,mes.length()-i); j=j+i+1;
			QString dia2 = line; dia2.remove(0,j);
			system(QString("echo \"%1\"").arg(dia2));
			i = dia2.find(" ",0); dia2.remove(i,dia2.length()-i);

			ZConfig IDini(Idioma, false);
			i= name.findRev("/"); if ( i!=name.length() ) name.remove(0,i+1);
			QString val = IDini.readEntry(QString("CRONTAB"), QString("NAME"), "");
			TEXTO = QString("%1: %2\n").arg(val).arg(name);
			val = IDini.readEntry(QString("CRONTAB"), QString("HOUR"), "");
			TEXTO += QString("%1: %2\n").arg(val).arg(hora);
			val = IDini.readEntry(QString("CRONTAB"), QString("MINUTE"), "");
			TEXTO += QString("%1: %2\n").arg(val).arg(minuto);
			val = IDini.readEntry(QString("CRONTAB"), QString("DAYMONTH"), "");
			TEXTO += QString("%1: %2\n").arg(val).arg(dia1);
			val = IDini.readEntry(QString("CRONTAB"), QString("MONTH"), "");
			TEXTO += QString("%1: %2\n").arg(val).arg(mes);
			val = IDini.readEntry(QString("CRONTAB"), QString("DAYWEEK"), "");
			TEXTO += QString("%1: %2\n").arg(val).arg(dia2);
			val = IDini.readEntry(QString("CRONTAB"), QString("DIR"), "");
			TEXTO += QString("%1: %2").arg(val).arg(app);
			ZMessageDlg* msg =  new ZMessageDlg ( "Cron", TEXTO, ZMessageDlg::just_ok, 0, NULL, "ZMessageDlg", true, 0 );
			msg->setTitleIcon("task_list_std");
			msg->exec();
			delete msg;
			msg = NULL;
			
		  }
		}
	  }
	}
	entrada.close();

}

void ZGui::addtask()
{
//	ZDateModule* dlg = new ZDateModule(this, NULL, 0, ZSkinService::clsZDateModule);
//	ZDatePickerDlg* msg = new ZDatePickerDlg(dlg, this, NULL, true, 0);
//	msg->exec(); delete msg; msg = NULL;
//	QDate fecha = msg->getResultDate();

  QString minuto, hora, dia, mes, dias, tarea;

  ZConfig IDini(Idioma, false);
  QString val = IDini.readEntry(QString("CRONTAB"), QString("MINUTE"), "");
  ZSingleCaptureDlg* zscd = new ZSingleCaptureDlg( QString("Cron"), val, ZSingleCaptureDlg::normal, NULL, "ZSingleCaptureDlg", true, 0, 0);
  ZLineEdit* zle = (ZLineEdit*)zscd->getLineEdit(); zle->setEdited(true);  
  if (zscd->exec() == 1) { if ( zle->text() != "" ) { minuto=zle->text(); } } else return;

  val = IDini.readEntry(QString("CRONTAB"), QString("HOUR"), "");
  zscd = new ZSingleCaptureDlg( QString("Cron"), val, ZSingleCaptureDlg::normal, NULL, "ZSingleCaptureDlg", true, 0, 0);
  zle = (ZLineEdit*)zscd->getLineEdit(); zle->setEdited(true);  
  if (zscd->exec() == 1) { if ( zle->text() != "" ) { hora=zle->text(); } } else return;

  val = IDini.readEntry(QString("CRONTAB"), QString("DAYMONTH"), "");
  zscd = new ZSingleCaptureDlg( QString("Cron"), val, ZSingleCaptureDlg::normal, NULL, "ZSingleCaptureDlg", true, 0, 0);
  zle = (ZLineEdit*)zscd->getLineEdit(); zle->setEdited(true);  
  if (zscd->exec() == 1) { if ( zle->text() != "" ) { dia=zle->text(); } } else return;

  val = IDini.readEntry(QString("CRONTAB"), QString("MONTH"), "");
  zscd = new ZSingleCaptureDlg( QString("Cron"), val, ZSingleCaptureDlg::normal, NULL, "ZSingleCaptureDlg", true, 0, 0);
  zle = (ZLineEdit*)zscd->getLineEdit(); zle->setEdited(true);  
  if (zscd->exec() == 1) { if ( zle->text() != "" ) { mes=zle->text(); } } else return;

  val = IDini.readEntry(QString("CRONTAB"), QString("DAYWEEK"), "");
  zscd = new ZSingleCaptureDlg( QString("Cron"), val, ZSingleCaptureDlg::normal, NULL, "ZSingleCaptureDlg", true, 0, 0);
  zle = (ZLineEdit*)zscd->getLineEdit(); zle->setEdited(true);  
  if (zscd->exec() == 1) { if ( zle->text() != "" ) { dias=zle->text(); } } else return;

  qApp->processEvents();

  ZFileOpenDialog *zopen  = new ZFileOpenDialog();
  zopen->exec();
  if ( zopen->result() == 1 )
  {
    QString archivo = zopen->getFileName();
    if (archivo != "")
	{
	  system(QString("echo \"%1 %2 %3 %4 %5 %6\" >> /ezxlocal/LinXtend/usr/bin/cron/user.txt").arg(minuto).arg(hora).arg(dia).arg(mes).arg(dias).arg(archivo));
	  zllb1->clear(); CargarMenu1();
	}
  }

}

void ZGui::removetask()
{
  ZConfig IDini(Idioma, false);
  QString val = IDini.readEntry(QString("CRONTAB"), QString("REMOVE"), "");
  ZMessageDlg* msg =  new ZMessageDlg ( "Cron", val, ZMessageDlg::yes_no, 0, NULL, "ZMessageDlg", true, 0 );
  int i = msg->exec();
  delete msg;
  msg = NULL;
  if (i)
  {
	qApp->processEvents();
	int index = zllb1->currentItem();
	if (index == -1) {return;}
	int result = index+1;
	QString app = zllb1->item(index)->getSubItemText(1,0,true);

	QString line, TEXTO; int numlinea=0;
	QFile entrada("/ezxlocal/LinXtend/usr/bin/cron/user.txt");
	QTextStream stentrada(&entrada);
	if ( entrada.open(IO_ReadOnly | IO_Translate) )
	{
	  while ( !stentrada.eof() )
	  {
		line = stentrada.readLine();
		if ( line != "" )
		{
		  QString name = line;
		  int i = name.find("/",0); name.remove(0,i);
		  i= name.findRev("/"); if ( i!=name.length() ) name.remove(0,i+1);
		  if ( name != app ) 
		  {
			if ( numlinea==0 ) { TEXTO += QString("%1").arg(line); numlinea=1; }
			else { TEXTO += QString("\n%1").arg(line); }
		  }
		}
	  }
	}
	entrada.close();
	system(QString("echo \"%1\" > /ezxlocal/LinXtend/usr/bin/cron/user.txt").arg(TEXTO));
	zllb1->clear(); CargarMenu1();
  }
}

void ZGui::about()
{
	menu->hide();
	qApp->processEvents();
	system("/ezxlocal/LinXtend/usr/bin/showMsg \"Cron Manager\" \"Motorola Z6\nCron Manager v0.1\n\n(C) 2009. CepiPerez\n\nwww.developermoto.com/es\nwww.modmymoto.com\" 2");

}
