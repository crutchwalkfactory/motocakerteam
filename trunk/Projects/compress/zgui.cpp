#include "zgui.h"
#include <ZConfig.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <qpixmap.h>
#include <qfileinfo.h>
#include <qfile.h>
#include <qdir.h>
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


QString Idioma;
QTextCodec* txtcodec = QTextCodec::codecForName("UTF-8");
RES_ICON_Reader iconReader;
int Cambio;
QString Archivo, nArchivo, aFolder;
QString modo="Archivo";
QString sPath;

ZGui::ZGui ( const QString &argument, QWidget* parent, const char* name, WFlags fl )
    : ZKbMainWidget ( ZHeader::FULL_TYPE, NULL, "ZMainWidget", 0 )
{
  Cambio=0;
  QString val; 
  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma = "ezxlocal/download/mystuff/.system/LinXtend/usr/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma).isFile() ) Idioma = "/ezxlocal/download/mystuff/.system/LinXtend/usr/languages/en-us";

  QString argum = argument;

	system(QString("echo \"ARCHIVO = %1\"").arg(argum));

  QFile f(argum);
  if ( f.exists() )
  {
	Archivo = argum;
	CreateWindow ( parent );
	( ( ZApplication* ) qApp )->showMainWidget ( this );
  }
  else
  {
	qApp->quit();
  }
	
}

ZGui::~ZGui()
{

}

void ZGui::CreateWindow ( QWidget* parent )
{
	ZConfig ini(Idioma, false);
	QString val;

	nArchivo = getFileName(Archivo);
	aFolder = getFilePath(Archivo);
    setMainWidgetTitle ( nArchivo );

	zllb1 = new ZListBox ( QString ( "%I%M" ), this, 0); zllb1->setFixedWidth ( 240 ); 
	CargarMenu1();
	connect(zllb1, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(appClicked()));

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

QString ZGui::getFileName(const QString &AFileName)
{
  QString str = AFileName;
  int i = str.findRev ( "/" );
  str.remove ( 0, i);
  if (str.left(1) = "/") str.remove ( 0, 1);
  return str;    
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
  zllb1->clear();
  QString ext = nArchivo;
  int i = ext.findRev ( "." ); ext.remove ( 0, i +1 );
  if ( (ext.lower()=="tar") | (ext.lower()=="gz") )
  { system(QString("/usr/bin/busybox tar -tf \"%1\" > /tmp/compress").arg(Archivo)); }
  if ( ext.lower()=="rar" )
  { system(QString("/ezxlocal/LinXtend/usr/bin/unrarmod lb \"%1\" > /tmp/compress").arg(Archivo)); }
  if ( ext.lower()=="zip" )
  { system(QString("/ezxlocal/LinXtend/usr/bin/unzipmod -l \"%1\" | awk '{print $4}' > /tmp/compress").arg(Archivo)); }

  QPixmap p1;
  QString line;
  QFile entrada("/tmp/compress");
  QTextStream stentrada(&entrada);
  if ( entrada.open(IO_ReadOnly | IO_Translate) )
  {
	while ( !stentrada.eof() )
	{
	  line = stentrada.readLine();
	  if ( (line != "") && (line != "Name") && (line != "----"))
	  {
		  ZListBoxItem* autoitem = new ZListBoxItem ( zllb1, QString ( "%I%M" ) );
		  int final = line.length()-1;
		  if ( line[final]==QChar('/') ) { p1=QPixmap(iconReader.getIcon("fm_folder_small", false)); }
		  else { p1=QPixmap(iconReader.getIcon("msg_receipt_request_small", false)); }
		  if ( p1.height() > 19 ) { QImage image; image = p1.convertToImage(); 
		  image = image.smoothScale(19, 19); p1.convertFromImage(image); } 
		  autoitem->setPixmap ( 0, p1 );
		  QString name = line;
		  //int i = name.find("/",0); name.remove(0,i);
		  //i= name.findRev("/"); if ( i!=name.length() ) name.remove(0,i+1);
		  autoitem->appendSubItem ( 1, name, false, NULL );
		  zllb1->insertItem ( autoitem,-1,true );
      }
	}
	zllb1->sort(true);
  }

}

void ZGui::Create_MenuApp()
{
  QString IDval; ZConfig IDini(Idioma, false);
  QRect rect;   QPixmap* p1 = new QPixmap();
  menu = new ZOptionsMenu ( rect, softKey, NULL, 0);
  menu->setItemSpacing(10); int idx=0;

  p1 = new QPixmap(iconReader.getIcon("", false));
  IDval = IDini.readEntry(QString("COMPRESS"), QString("UNZIP"), "");
  menu->insertItem ( IDval, NULL, p1, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( browser() ) );      
  ++idx;
  p1 = new QPixmap(iconReader.getIcon("", false));
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_EXIT"), "");
  menu->insertItem ( IDval, NULL, p1, true, idx, idx );
  menu->connectItem ( idx, qApp, SLOT ( quit() ) );
}

void ZGui::Create_MenuApp2()
{
  QString IDval; ZConfig IDini(Idioma, false);
  QRect rect;   QPixmap* p1 = new QPixmap();
  menu2 = new ZOptionsMenu ( rect, softKey, NULL, 0);
  menu2->setItemSpacing(10); 
  p1 = new QPixmap(iconReader.getIcon("", false));
  IDval = IDini.readEntry(QString("COMPRESS"), QString("HERE"), "");
  menu2->insertItem ( IDval, NULL, p1, true, 0, 0 );
  menu2->connectItem ( 0, this, SLOT ( selected() ) );      
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_CANCEL"), "");
  menu2->insertItem ( IDval, NULL, p1, true, 1, 1 );
  menu2->connectItem ( 1, this, SLOT ( cancelar() ) );      
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

void ZGui::Create_SoftKey2()
{
  QString IDval; QString IDval2; ZConfig IDini(Idioma, false);
  softKey->setOptMenu ( ZSoftKey::LEFT, menu2 ); 
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_MENU"), "");
  softKey->setText ( ZSoftKey::LEFT, IDval, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_MENU"), "");
  softKey->setTextForOptMenuHide(IDval);
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_SELECT"), "");
  IDval2 = IDini.readEntry(QString("COMMON"), QString("MENU_CANCEL"), "");
  softKey->setTextForOptMenuShow(IDval, IDval2);
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_GOBACK"), "");
  softKey->setText ( ZSoftKey::RIGHT, IDval, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( atras() ) );      
}

QString ZGui::getFilePath(const QString &AFileName)
{
  QString str = AFileName;
  int i = str.findRev ( "/" );
  str.remove ( i+1, str.length() - i );
  return str;  
}


void ZGui::unzip(QString destino)
{
  qApp->processEvents();
  QString val; ZConfig IDini(Idioma, false);
  QString carpeta = getFilePath(Archivo);
  val = IDini.readEntry(QString("COMPRESS"), QString("DEST"), "");
  //system(QString("/ezxlocal/LinXtend/usr/bin/showFilePickerExt \"%1\" \"%2\" \"onlydirs\" > /tmp/selectedfolder").arg(val).arg(carpeta));
  //QFile entrada("/tmp/selectedfolder");
  //QTextStream stentrada(&entrada); QString line;
  //if ( entrada.open(IO_ReadOnly | IO_Translate) ) { line = stentrada.readLine(); }
  //system("rm /tmp/selectedfolder");
  int i = destino.length();
  if ( (destino!="0") && (destino[i]!=QChar('.')) ) {
	QString ext = nArchivo;
	int x = ext.findRev ( "." ); ext.remove ( 0, x +1 );
	x = destino.findRev ( "//" ); destino.remove ( x, 1 );
	if ( (ext.lower()=="tar") | (ext.lower()=="gz") )
	{ system(QString("/usr/bin/busybox tar -xf \"%1\" -C \"%2\"").arg(Archivo).arg(destino)); }
	if ( ext.lower()=="rar" )
	{ system(QString("/ezxlocal/LinXtend/usr/bin/unrarmod x \"%1\" \"%2\"").arg(Archivo).arg(destino)); }
	if ( ext.lower()=="zip" )
	{ system(QString("/ezxlocal/LinXtend/usr/bin/unzipmod -d \"%1\" \"%2\"").arg(destino).arg(Archivo)); }
	cancelar();
  }
}

void ZGui::browser()
{
	qApp->processEvents();
	destino(aFolder);

	Create_MenuApp2();
	softKey->setOptMenu ( ZSoftKey::LEFT, menu2 ); 
	QString IDval; QString IDval2; ZConfig IDini(Idioma, false);
	IDval = IDini.readEntry(QString("COMMON"), QString("MENU_GOBACK"), "");
	softKey->setText ( ZSoftKey::RIGHT, IDval, ( ZSoftKey::TEXT_PRIORITY ) 0 );
	softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( atras() ) );      

	QString val = IDini.readEntry(QString("COMPRESS"), QString("DEST"), "");
    setMainWidgetTitle ( val );

}

void ZGui::destino(QString carpeta)
{

  zllb1->clear();
  modo="Browser";
  QDir dir ( carpeta, "*" );
  sPath = carpeta;
  dir.setMatchAllDirs ( true );
  dir.setFilter ( QDir::Dirs | QDir::Hidden );
  if ( !dir.isReadable() )
	return;
  QStringList entries = dir.entryList();
  entries.sort();
  QStringList::ConstIterator it = entries.begin();

  QPixmap pixmap;
  if ( (sPath != "") && (sPath != "/") && (sPath != "//") )
  {
	pixmap=QPixmap( iconReader.getIcon("gen_back_to_arrw_thb", false) );
	if ( pixmap.height() > 19 ) { QImage image; image = pixmap.convertToImage(); 
	image = image.smoothScale( 19, 19 ); pixmap.convertFromImage(image); }  
	ZListBoxItem* Item;
	Item = new ZListBoxItem ( zllb1, QString ( "%I%M" ) );
	Item->setPixmap ( 0, pixmap ); Item->appendSubItem ( 1, "..", false, NULL );
	zllb1->insertItem ( Item,-1,true );
  }

  while ( it != entries.end() ) {
	if ( ( *it != "." ) && ( *it != ".." ) )
	{
		pixmap=QPixmap( iconReader.getIcon("fm_folder_small", false) );
		if ( pixmap.height() > 19 ) { QImage image; image = pixmap.convertToImage(); 
		image = image.smoothScale( 19, 19 ); pixmap.convertFromImage(image); }  

		ZListBoxItem* Item;
		Item = new ZListBoxItem ( zllb1, QString ( "%I%M" ) );
	    
		Item->setPixmap ( 0, pixmap );
		Item->appendSubItem ( 1, *it, false, NULL );
		zllb1->insertItem ( Item,-1,true );
	}
	++it;
  }

}

void ZGui::selected()
{
	qApp->processEvents();
	QString app = zllb1->item ( zllb1->currentItem() )->getSubItemText ( 1, 0, true );

	ZConfig IDini(Idioma, false);
	QString val = IDini.readEntry(QString("COMPRESS"), QString("PROCESSING"), "");
	ZMessageDlg* msg0 =  new ZMessageDlg ( QString("zCompress"),
	QString(val).arg(nArchivo), ZMessageDlg::NONE, 200, NULL, "ZMessageDlg", true, 0 );
	msg0->exec();
	unzip(sPath);
	delete msg0;
	msg0 = NULL;
	val = IDini.readEntry(QString("COMPRESS"), QString("DONE"), "");
	ZMessageDlg* msg =  new ZMessageDlg ( QString("zCompress"), QString(val).arg(nArchivo), 
						ZMessageDlg::just_ok, 0, NULL, "ZMessageDlg", true, 0 );
	msg->exec(); delete msg; msg = NULL;
	qApp->quit();

}


void ZGui::appClicked()
{
  if ( modo=="Browser" )
  {
	QString app = zllb1->item ( zllb1->currentItem() )->getSubItemText ( 1, 0, true );

	if ( app == ".." ) { 
		QString nPath = sPath;
		int i = nPath.findRev ( "/" );
		nPath.remove ( i, nPath.length() - i );
		destino( nPath );
	}
	else 
	{
		int i = sPath.findRev ( "//" ); sPath.remove ( i, 1 );
		QString fName = sPath + "/" + app;
		if ( QFileInfo(fName).isDir() ) { destino( fName ); return; }
	}
  }
}

void ZGui::cancelar()
{
	qApp->processEvents();
	modo="Archivo";
	CargarMenu1();
	Create_MenuApp();
	softKey->setOptMenu ( ZSoftKey::LEFT, menu ); 
	QString IDval; QString IDval2; ZConfig IDini(Idioma, false);
	IDval = IDini.readEntry(QString("COMMON"), QString("MENU_EXIT"), "");
	softKey->setText ( ZSoftKey::RIGHT, IDval, ( ZSoftKey::TEXT_PRIORITY ) 0 );
	softKey->setClickedSlot ( ZSoftKey::RIGHT, qApp, SLOT ( quit() ) );      
    setMainWidgetTitle ( getFileName(Archivo) );

}

void ZGui::atras()
{
	QString nPath = sPath;
	int i = nPath.length()-1;
	if ( nPath[i]==QChar('/') ) { nPath.remove ( i, 1 ); }
	i = nPath.findRev ( "/" );
	nPath.remove ( i, nPath.length() - i );
	system(QString("echo \"CARPETA ANTERIOR = %1\"").arg(sPath));
	system(QString("echo \"CARPETA ACTUAL = %1\"").arg(nPath));
	if ( nPath!="" ) { destino( "/" ); }
}
