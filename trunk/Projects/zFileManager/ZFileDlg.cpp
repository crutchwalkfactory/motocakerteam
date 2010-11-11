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
#include "myam.h"
#include "ZFileDlg.h"
#include "ZFileBrowser.h"
#include "launcher.h"
#include <qpixmap.h>
#include <qlabel.h>
#include <ZApplication.h>
#include <ZSoftKey.h>
#include "resources.h"
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

QString ProgDir; QString Idioma; QString FStext;
int SelectProg=0;

extern ZLauncher * launcher;

//************************************************************************************************
//************************************** class ZFileOpenDialog ***********************************
//************************************************************************************************
ZFileOpenDialog::ZFileOpenDialog(bool onlyDirs)
  :MyBaseDlg()
{

  ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma = ProgDir + "/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma).isFile() ) Idioma = ProgDir + "/languages/en-us";
  ZConfig lang (Idioma,false); 

  this->setMainWidgetTitle( texto(lang.readEntry(QString("LANGUAGE"), "SELECTDEST", "")) );
  
  QWidget *myWidget = new QWidget ( this );
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );

  fb = new FileBrowser("*", onlyDirs, QString ( "%I%M" ), myWidget, 0);
  fb->setDir("/");
  QObject::connect(fb, SIGNAL(isFilePicked(bool)), SLOT(filePicked(bool)));

  myVBoxLayout->addWidget ( fb, 1 );
  
  fb->ProgramDir = ProgDir;
  setContentWidget ( myWidget );

  ZSoftKey *softKey = new ZSoftKey ( "CST_2A", this, this );
  //ZSoftKey *softKey = this->getSoftKey( true );
  
  softKey->setText ( ZSoftKey::RIGHT, texto(lang.readEntry(QString("LANGUAGE"), "PASTEHERE", "")), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  
  softKey->setTextForOptMenuHide ( texto(lang.readEntry(QString("LANGUAGE"), "FUNCTIONS", "")) );
  softKey->setTextForOptMenuShow ( texto(lang.readEntry(QString("LANGUAGE"), "CHOISE", "")), texto(lang.readEntry(QString("LANGUAGE"), "CANCEL", "")) );
  
  softKey->setText ( ZSoftKey::LEFT, texto(lang.readEntry(QString("LANGUAGE"), "FUNCTIONS", "")), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_paste() ) );
  //softKey->setClickedSlot ( ZSoftKey::LEFT, this, SLOT ( slot_paste() ) );
  
  QRect rect;
  menuPaste = new ZOptionsMenu ( rect, this, NULL , 0 , ZSkinService::clsZOptionsMenu );
  
  menuPaste->setItemSpacing ( 10 );
  softKey->setOptMenu ( ZSoftKey::LEFT, menuPaste );
  
  QPixmap* pm  = new QPixmap ( ProgDir + IDB_IMAGE_CANCEL );
  menuPaste->insertItem ( texto(lang.readEntry(QString("LANGUAGE"), "CANCEL", "")), NULL, pm , true , 0 , 0 );
  //pm->load ( ProgDir + IDB_IMAGE_PASTE );
  //menuPaste->insertItem ( texto(lang.readEntry(QString("LANGUAGE"), "PASTEHERE", "")), NULL, pm , true , 1 , 1 );
  pm->load ( ProgDir + IDB_IMAGE_MKDIR );
  menuPaste->insertItem ( texto(lang.readEntry(QString("LANGUAGE"), "MKDIR", "")), NULL, pm , true , 2 , 2 );

  
  menuPaste->connectItem ( 0, this, SLOT ( slot_cancel() ) );
  menuPaste->connectItem ( 1, this, SLOT ( slot_paste() ) );
  menuPaste->connectItem ( 2, this, SLOT ( slot_makedir()) );
  this->setCSTWidget( softKey );
    
}

ZFileOpenDialog::~ZFileOpenDialog()
{
  //launcher->CreateSoftKeys(NULL);
}

QString ZFileOpenDialog::texto(const char*xString)
{
	ZConfig IDini(Idioma, false);	
	QString	IDval = IDini.readEntry(QString("LANGUAGE"), QString("UNICODE"), "");
	if ( IDval == "1" ) { 
		QTextCodec* txtcodec = QTextCodec::codecForName("UTF-8");
		return txtcodec->toUnicode(xString); }
	else { return xString; }
}  

void ZFileOpenDialog::accept()
{
  fileName = fb->getFileName();
  basePath = fb->getFilePath();
  if (fileName != "") { MyBaseDlg::accept(); }
  else {
    ZMessageDlg * dlg = new ZMessageDlg ( "Error", QString("Filename is empty"), ZMessageDlg::just_ok, 0, this, "1", true, 0 );
    dlg->exec();
    delete dlg; dlg = NULL;
  }
}


void ZFileOpenDialog::selectItem(int idx)
{

}

void ZFileOpenDialog::slot_cancel()
{
  reject();
}

void ZFileOpenDialog::slot_paste()
{
  accept();
}



void ZFileOpenDialog::slot_makedir()
{
  ZConfig lang (Idioma,false);
  QString ASource = fb->getFilePath();
  if (ASource.isEmpty()) return;
  QString dirName;
  ZSingleCaptureDlg* dlgq = new ZSingleCaptureDlg ( texto(lang.readEntry(QString("LANGUAGE"), "MAKING", "")), 
		texto(lang.readEntry(QString("LANGUAGE"), "ENTER_DIRNAME", "")), ZSingleCaptureDlg::normal, this, NULL, true, 0, 0 );
  if ( dlgq->exec() ) {
    dirName = dlgq->getResultText();
    if ( dirName != "" ) {
      if ( ! fb->makedir(ASource, dirName) ) {
        ZMessageDlg* dlgz  = new ZMessageDlg ( texto(lang.readEntry(QString("LANGUAGE"), "ERROR", "")),
			texto(lang.readEntry(QString("LANGUAGE"), "MKDIR_ERROR", "")),  ZMessageDlg::just_ok , 0 , this , "about" , true , 0 );
        dlgz->exec();
        delete dlgz;
      } else {
        fb->setDir(ASource); 
      }
    }
  }    
  
}  

//************************************************************************************************
//************************************** class ZFileSaveDialog ***********************************
//************************************************************************************************
ZFileSaveDialog::ZFileSaveDialog(const QString &curPath, const QString &originName)
  :MyBaseDlg()
{
  QWidget *myWidget = new QWidget ( this );
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );
  
  // label
  QLabel *myLabel = new QLabel ( QString ( "Filename to save:" ), myWidget );
  myVBoxLayout->addWidget ( myLabel, 0 );
  myLabel->setIndent ( 5 );
  myLabel->setAutoResize ( true );
    
  // zlineedit
  zmle = new ZMultiLineEdit(myWidget, true, 1, "ZMultiLineEdit");
  zmle->setEdited(false);
  zmle->setFixedHeight(myWidget->sizeHint().height());
  zmle->setInsertionMethod ( ( ZMultiLineEdit::InsertionMethod ) 3 );
  zmle->setFontPercent ( 0.7 );
  zmle->setSizePolicy ( QSizePolicy ( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
  zmle->setCellHeight(22);
  zmle->setText(originName);
  
  myVBoxLayout->addWidget ( zmle, 1 );
  
  fb = new FileBrowser("*", false, QString ( "%I%M" ), myWidget, 0);
  fb->setDir(curPath);
  QObject::connect(fb, SIGNAL(isFilePicked(bool)), SLOT(filePicked(bool)));
  myVBoxLayout->addWidget ( fb, 1 );
  
  
  setContentWidget ( myWidget );
  
  ZSoftKey *softKey = new ZSoftKey ( NULL, this, this );
  softKey->setText ( ZSoftKey::LEFT, "Save", ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setText ( ZSoftKey::RIGHT, "Cancel", ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( reject() ) );
  softKey->setClickedSlot ( ZSoftKey::LEFT, this, SLOT ( accept() ) );
  setCSTWidget ( softKey );
}

ZFileSaveDialog::~ZFileSaveDialog()
{

}

QString ZFileSaveDialog::texto(const char*xString)
{
	ZConfig IDini(Idioma, false);	
	QString	IDval = IDini.readEntry(QString("LANGUAGE"), QString("UNICODE"), "");
	if ( IDval == "1" ) { 
		QTextCodec* txtcodec = QTextCodec::codecForName("UTF-8");
		return txtcodec->toUnicode(xString); }
	else { return xString; }
}  

void ZFileSaveDialog::keyPressEvent ( QKeyEvent* e )
{
  switch ( e->key() ) 
  {
    case Z6KEY_LEFT:
    case Z6KEY_RIGHT: 
    {
      zmle->setFocus();
      break;
    }
    default:
      MyBaseDlg::keyPressEvent ( e );
  }   

}

void ZFileSaveDialog::accept()
{
  // check multilineedit
  QString txt;
  txt = zmle->text();
  if ((txt != "")) // || (txt != QString::null))
  {
    fileName = txt;
    basePath = fb->getFilePath();
    MyBaseDlg::accept();
  }
  else
  {
    ZMessageDlg *dlg = new ZMessageDlg ( "Error", "Please enter a file name.", ZMessageDlg::just_ok, 0, this, "About", true, 0 );
    dlg->setIcon(QPixmap("Dialog_Question_Mark.gif"));
    dlg->exec();
    delete dlg;
    dlg = NULL;  
  }
}


//extern ZLauncher *launcher;
//************************************************************************************************
//**************************************** class ZIconSelect *************************************
//************************************************************************************************
ZIconSelect::ZIconSelect()
  :MyBaseDlg()
{
  QString ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma = ProgDir + "/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma).isFile() ) Idioma = ProgDir + "/languages/en-us";
  ZConfig lang (Idioma,false); 

  this->setMainWidgetTitle("zFileManager");
  
  QWidget *myWidget = new ZWidget ( this, NULL, 0);
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );
  myWidget->setFixedWidth(240);
  ZAppInfoArea* pAIA = new ZAppInfoArea(myWidget, NULL, 0);
  pAIA->setActArea(ZAppInfoArea::no_progress_meter);
  QPixmap pixmap; pixmap.load(ProgDir+"/img/settings.png");
  pAIA->appendIcon(ZAppInfoArea::activity,pixmap,"");
  pAIA->appendText(ZAppInfoArea::activity, lang.readEntry(QString("LANGUAGE"), "SELECTICON", ""), "name");
  setAppInfoArea(pAIA);
  myVBoxLayout->insertWidget (0, pAIA );

  browser = new ZListBox ( QString ( "%I%M" ), this, 0);
  browser->setFixedWidth ( 240 );
  QPixmap p1;

  ZListBoxItem* Item = new ZListBoxItem ( browser, QString ( "%I%M" ) );
  Item->appendSubItem ( 1, QString ( ".." ), false, NULL );
  browser->insertItem ( Item, 0,true );
  int ItemSize = browser->itemHeight(0)-2 ; if ( ItemSize>20 ) ItemSize=20;
  browser->clear();

  QString folder; QFile entrada("/tmp/selectedfolder"); QTextStream stentrada(&entrada);
  if ( entrada.open(IO_ReadOnly | IO_Translate) ) { folder = stentrada.readLine(); } entrada.close();

  ZConfig ini(ProgDir+"/settings.cfg", true);
  QString SIcon = ini.readEntry(QString("FM"), QString("IconSource"), "1");


  if ( QFileInfo(folder).isDir() ) 
  {
	if ( SIcon=="0" ) 
	{
		system(QString("ls %1/img/folders/ > /tmp/customfolders").arg(ProgDir));
		QStringList listaApps; QString line;
		QFile entrada("/tmp/customfolders"); QTextStream stentrada(&entrada);
		if ( entrada.open(IO_ReadOnly | IO_Translate) ) {
			while ( !stentrada.eof() ) {
			line = stentrada.readLine();
			ZListBoxItem* listitem1 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
			p1.load(ProgDir + "/img/folders/" + line );
			if ( p1.height() > ItemSize ) { QImage image; image = p1.convertToImage(); 
			image = image.smoothScale(ItemSize, ItemSize); p1.convertFromImage(image); }  
			listitem1->setPixmap ( 0, p1 );
			listitem1->appendSubItem ( 1, line, false, 0 );
			browser->insertItem ( listitem1,-1,true );
		  }
		}
		entrada.close();
	}
	else
	{
		system("rm /tmp/customfolders");
		QFile entrada0(ProgDir + "/iconstoextract.cfg"); QTextStream stentrada0(&entrada0); 
		if ( entrada0.open(IO_ReadOnly | IO_Translate) )
		{ while ( !stentrada0.eof() ) { 
			QString line = stentrada0.readLine(); 
			int i = line.find ( "FOLDER = " );
			if ( i==0 ) { line.remove(0,9);
			system(QString("echo \"%1\" >> /tmp/customfolders").arg(line)); }
		} } entrada0.close();

		RES_ICON_Reader iconReader;
		QStringList listaApps; QString line;
		QFile entrada("/tmp/customfolders"); QTextStream stentrada(&entrada);
		if ( entrada.open(IO_ReadOnly | IO_Translate) ) {
			while ( !stentrada.eof() ) {
			line = stentrada.readLine();
			ZListBoxItem* listitem1 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
			p1=QPixmap(iconReader.getIcon(line, false));	
			p1.load(line);
			if ( p1.height() > ItemSize ) { QImage image; image = p1.convertToImage(); 
			image = image.smoothScale(ItemSize, ItemSize); p1.convertFromImage(image); }  
			listitem1->setPixmap ( 0, p1 );
			listitem1->appendSubItem ( 1, line, false, 0 );
			browser->insertItem ( listitem1,-1,true );
		  }
		}
		entrada.close();
	}


  }

  if ( QFileInfo(folder).isFile() ) 
  {
	if ( SIcon=="0" ) 
	{
		system(QString("ls %1/img/files/ > /tmp/customfolders").arg(ProgDir));
		QStringList listaApps; QString line;
		QFile entrada("/tmp/customfolders"); QTextStream stentrada(&entrada);
		if ( entrada.open(IO_ReadOnly | IO_Translate) ) {
			while ( !stentrada.eof() ) {
			line = stentrada.readLine();
			ZListBoxItem* listitem1 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
			p1.load(ProgDir + "/img/files/" + line );
			if ( p1.height() > ItemSize ) { QImage image; image = p1.convertToImage(); 
			image = image.smoothScale(ItemSize, ItemSize); p1.convertFromImage(image); }  
			listitem1->setPixmap ( 0, p1 );
			listitem1->appendSubItem ( 1, line, false, 0 );
			browser->insertItem ( listitem1,-1,true );
		  }
		}
		entrada.close();
	}
	else
	{
		system("rm /tmp/customfolders");
		QFile entrada0(ProgDir + "/iconstoextract.cfg"); QTextStream stentrada0(&entrada0); 
		if ( entrada0.open(IO_ReadOnly | IO_Translate) )
		{ while ( !stentrada0.eof() ) { 
			QString line = stentrada0.readLine(); 
			int i = line.find ( "FILE = " );
			if ( i==0 ) { line.remove(0,7);
			system(QString("echo \"%1\" >> /tmp/customfolders").arg(line)); }
		} } entrada0.close();

		RES_ICON_Reader iconReader;
		QStringList listaApps; QString line;
		QFile entrada("/tmp/customfolders"); QTextStream stentrada(&entrada);
		if ( entrada.open(IO_ReadOnly | IO_Translate) ) {
			while ( !stentrada.eof() ) {
			line = stentrada.readLine();
			ZListBoxItem* listitem1 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
			p1=QPixmap(iconReader.getIcon(line, false));	
			p1.load(line);
			if ( p1.height() > ItemSize ) { QImage image; image = p1.convertToImage(); 
			image = image.smoothScale(ItemSize, ItemSize); p1.convertFromImage(image); }  
			listitem1->setPixmap ( 0, p1 );
			listitem1->appendSubItem ( 1, line, false, 0 );
			browser->insertItem ( listitem1,-1,true );
		  }
		}
		entrada.close();
	}


  }
  browser->enableMarquee(ZListBox::LISTITEM_REGION_C, true);
  connect(browser, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(brClicked()));

  myVBoxLayout->addWidget ( browser, 1 );
  setContentWidget ( browser );

  ZSoftKey *softKey = new ZSoftKey ( "CST_2A", this, this );

  softKey->setText ( ZSoftKey::RIGHT, texto(lang.readEntry(QString("LANGUAGE"), "CANCEL", "")), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  
  softKey->setTextForOptMenuHide ( texto(lang.readEntry(QString("LANGUAGE"), "FUNCTIONS", "")) );
  softKey->setTextForOptMenuShow ( texto(lang.readEntry(QString("LANGUAGE"), "CHOISE", "")), texto(lang.readEntry(QString("LANGUAGE"), "CANCEL", "")) );
  
  softKey->setText ( ZSoftKey::LEFT, texto(lang.readEntry(QString("LANGUAGE"), "FUNCTIONS", "")), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_cancel() ) );
  
  if ( QFileInfo(folder).isDir() ) 
  {
	QRect rect;
	menuPaste = new ZOptionsMenu ( rect, this, NULL , 0 , ZSkinService::clsZOptionsMenu );
	menuPaste->setItemSpacing ( 10 );
	softKey->setOptMenu ( ZSoftKey::LEFT, menuPaste );
	QPixmap* pm  = new QPixmap ( ProgDir + IDB_IMAGE_CANCEL );
	menuPaste->insertItem ( texto(lang.readEntry(QString("LANGUAGE"), "RESET", "")), NULL, pm , true , 0 , 0 );
	menuPaste->connectItem ( 0, this, SLOT ( accept() ) );
	this->setCSTWidget( softKey );
  }    
  if ( QFileInfo(folder).isFile() ) 
  {
	QRect rect;
	menuPaste = new ZOptionsMenu ( rect, this, NULL , 0 , ZSkinService::clsZOptionsMenu );
	menuPaste->setItemSpacing ( 10 );
	softKey->setOptMenu ( ZSoftKey::LEFT, menuPaste );
	QPixmap* pm  = new QPixmap ( ProgDir + IDB_IMAGE_CANCEL );
	menuPaste->insertItem ( texto(lang.readEntry(QString("LANGUAGE"), "CANCEL", "")), NULL, pm , true , 0 , 0 );
	menuPaste->connectItem ( 0, this, SLOT ( slot_cancel() ) );
	this->setCSTWidget( softKey );
  }

}

ZIconSelect::~ZIconSelect()
{
  //launcher->CreateSoftKeys(NULL);
}

QString ZIconSelect::texto(const char*xString)
{
	ZConfig IDini(Idioma, false);	
	QString	IDval = IDini.readEntry(QString("LANGUAGE"), QString("UNICODE"), "");
	if ( IDval == "1" ) { 
		QTextCodec* txtcodec = QTextCodec::codecForName("UTF-8");
		return txtcodec->toUnicode(xString); }
	else { return xString; }
}  

void ZIconSelect::accept()
{
  QString ProgDir =  QString ( qApp->argv() [0] );
  int j = ProgDir.findRev ( "/" );
  ProgDir.remove ( j+1, ProgDir.length() - j );
  ZConfig lang (Idioma,false);

	QString folder; QFile entrada("/tmp/selectedfolder"); QTextStream stentrada(&entrada);
	if ( entrada.open(IO_ReadOnly | IO_Translate) ) { folder = stentrada.readLine(); } entrada.close();

	int index = browser->currentItem(); if (index == -1) {return;}
	ZListBoxItem* listitem = browser->item ( index );
	QString app = listitem->getSubItemText ( 1, 0, true );
	//folder.remove(0,1);
	//qDebug(QString( "Restableciendo icono" ));  

	ZConfig Sini(ProgDir + "/settings.cfg", true);
	QString SIcons = Sini.readEntry(QString("FM"), QString("IconSource"), "");
	
	if ( SIcons == "0" )
	{
		ZConfig ini(ProgDir + "/foldericons.cfg", true);
		ini.removeEntry(QString("FOLDERICONS"), folder);
		ZMessageDlg* msg =  new ZMessageDlg ( QString(APP_NAME), texto(lang.readEntry(QString("LANGUAGE"), "APPLYSUBS", "")),
		ZMessageDlg::yes_no, 0, NULL, "ZMessageDlg", true, 0 );
		int i = msg->exec();
		delete msg;
		msg = NULL;
		if (i)
		{
		  system(QString("find %1 -type d > /tmp/folder2").arg(folder));
		  QFile entrada("/tmp/folder2"); QTextStream stentrada(&entrada);
		  if ( entrada.open(IO_ReadOnly | IO_Translate) )
		  { while ( !stentrada.eof() ) { 
			QString line = stentrada.readLine(); 
			if ( QFileInfo(line).isDir() ) { ini.removeEntry(QString("FOLDERICONS"), line); }
		  } } entrada.close(); system("rm /tmp/folder2");
		}
	}
	else
	{
		ZConfig ini(ProgDir + "/sfoldericons.cfg", true);
		ini.removeEntry(QString("FOLDERICONS"), folder);
		ZMessageDlg* msg =  new ZMessageDlg ( QString(APP_NAME), texto(lang.readEntry(QString("LANGUAGE"), "APPLYSUBS", "")),
		ZMessageDlg::yes_no, 0, NULL, "ZMessageDlg", true, 0 );
		int i = msg->exec();
		delete msg;
		msg = NULL;
		if (i)
		{
		  system(QString("find %1 -type d > /tmp/folder2").arg(folder));
		  QFile entrada("/tmp/folder2"); QTextStream stentrada(&entrada);
		  if ( entrada.open(IO_ReadOnly | IO_Translate) )
		  { while ( !stentrada.eof() ) { 
			QString line = stentrada.readLine(); 
			if ( QFileInfo(line).isDir() ) { ini.removeEntry(QString("FOLDERICONS"), line); }
		  } } entrada.close(); system("rm /tmp/folder2");
		}
	}


  MyBaseDlg::accept();

}


void ZIconSelect::selectItem(int idx)
{

}

void ZIconSelect::slot_cancel()
{
  reject();
}


void ZIconSelect::brClicked()
{

  QString ProgDir =  QString ( qApp->argv() [0] );
  int j = ProgDir.findRev ( "/" );
  ProgDir.remove ( j+1, ProgDir.length() - j );
  ZConfig lang (Idioma,false);

  int index = browser->currentItem();
  if (index == -1) {return;}
  ZListBoxItem* listitem = browser->item ( index );
  QString app = listitem->getSubItemText ( 1, 0, true );  

  QString folder; QFile entrada("/tmp/selectedfolder"); QTextStream stentrada(&entrada);
  if ( entrada.open(IO_ReadOnly | IO_Translate) ) { folder = stentrada.readLine(); } entrada.close();

  if ( QFileInfo(folder).isDir() )
  {

	ZConfig Sini(ProgDir + "/settings.cfg", true);
	QString SIcons = Sini.readEntry(QString("FM"), QString("IconSource"), "");
	
	if ( SIcons == "0" )
	{
		int encontro = folder.find( "//" , 0 );
		if ( encontro > -1 ) { folder.remove( encontro,1); }
		//qDebug(QString( "Cambiando icono de carpeta: " + app ));  

		ZConfig ini(ProgDir + "/foldericons.cfg", true);
		ini.writeEntry(QString("FOLDERICONS"), folder, app);

		ZMessageDlg* msg =  new ZMessageDlg ( QString(APP_NAME), texto(lang.readEntry(QString("LANGUAGE"), "APPLYSUBS", "")),
		ZMessageDlg::yes_no, 0, NULL, "ZMessageDlg", true, 0 );
		int i = msg->exec();
		delete msg;
		msg = NULL;
		if (i)
		{
		  system(QString("find %1 -type d > /tmp/folder2").arg(folder));
		  QFile entrada("/tmp/folder2"); QTextStream stentrada(&entrada);
		  if ( entrada.open(IO_ReadOnly | IO_Translate) )
		  { while ( !stentrada.eof() ) { 
			QString line = stentrada.readLine(); 
			if ( QFileInfo(line).isDir() ) { ini.writeEntry(QString("FOLDERICONS"), line, app); }
		  } } entrada.close(); system("rm /tmp/folder2");
		}
	}
	else
	{
		int encontro = folder.find( "//" , 0 );
		if ( encontro > -1 ) { folder.remove( encontro,1); }
		//qDebug(QString( "Cambiando icono de carpeta: " + app ));  

		ZConfig ini(ProgDir + "/sfoldericons.cfg", true);
		ini.writeEntry(QString("FOLDERICONS"), folder, app);

		ZMessageDlg* msg =  new ZMessageDlg ( QString(APP_NAME), texto(lang.readEntry(QString("LANGUAGE"), "APPLYSUBS", "")),
		ZMessageDlg::yes_no, 0, NULL, "ZMessageDlg", true, 0 );
		int i = msg->exec();
		delete msg;
		msg = NULL;
		if (i)
		{
		  system(QString("find %1 -type d > /tmp/folder2").arg(folder));
		  QFile entrada("/tmp/folder2"); QTextStream stentrada(&entrada);
		  if ( entrada.open(IO_ReadOnly | IO_Translate) )
		  { while ( !stentrada.eof() ) { 
			QString line = stentrada.readLine(); 
			if ( QFileInfo(line).isDir() ) { ini.writeEntry(QString("FOLDERICONS"), line, app); }
		  } } entrada.close(); system("rm /tmp/folder2");
		}
	}

  }

  if ( QFileInfo(folder).isFile() )
  {
	//qDebug(QString( "Cambiando icono de archivo: " + app ));  
	ZConfig Sini(ProgDir + "/foldericons.cfg", true);
	QString SIcons = Sini.readEntry(QString("FM"), QString("IconSource"), "");
	
	if ( SIcons == "0" )
	{
		int i = folder.findRev ( "." ); folder.remove ( 0, i+1 );
		ZConfig ini(ProgDir + "/fileicons.cfg", true);
		ini.writeEntry(QString("FILES"), folder, app);
	}
	else
	{
		int i = folder.findRev ( "." ); folder.remove ( 0, i+1 );
		ZConfig ini(ProgDir + "/sfileicons.cfg", true);
		ini.writeEntry(QString("FILES"), folder, app);
	}

  }
  MyBaseDlg::accept();


}


//extern ZLauncher *launcher;
//************************************************************************************************
//************************************** class ZOpenWith *****************************************
//************************************************************************************************
ZOpenWith::ZOpenWith()
  :MyBaseDlg()
{
  QString ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma = ProgDir + "/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma).isFile() ) Idioma = ProgDir + "/languages/en-us";
  ZConfig lang (Idioma,false); 

  this->setMainWidgetTitle("zFileManager");
  
  QWidget *myWidget = new ZWidget ( this, NULL, 0);
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );
  myWidget->setFixedWidth(240);
  ZAppInfoArea* pAIA = new ZAppInfoArea(myWidget, NULL, 0);
  pAIA->setActArea(ZAppInfoArea::no_progress_meter);
  QPixmap pixmap; pixmap.load(ProgDir+"/img/settings.png");
  pAIA->appendIcon(ZAppInfoArea::activity,pixmap,"");
  pAIA->appendText(ZAppInfoArea::activity, lang.readEntry(QString("LANGUAGE"), "OPENWITH", ""), "name");
  setAppInfoArea(pAIA);
  myVBoxLayout->insertWidget (0, pAIA );

  browser = new ZListBox ( QString ( "%I%M" ), this, 0);
  browser->setFixedWidth ( 240 );


  ZListBoxItem* Item = new ZListBoxItem ( browser, QString ( "%I%M" ) );
  Item->appendSubItem ( 1, QString ( ".." ), false, NULL );
  browser->insertItem ( Item, 0,true );
  int ItemSize = browser->itemHeight(0)-2 ; if ( ItemSize>20 ) ItemSize=20;
  browser->clear();


	system("cat /mmc/mmca1/.system/java/CardRegistry /ezxlocal/download/appwrite/am/UserRegistry /usr/data_resource/preloadapps/SysRegistry > /tmp/SysRegTemp");
	QPixmap p1;	
	QString name, exec, java, ico, appid, dir, type;
	RES_ICON_Reader iconReader;
	
	ZConfig confSYS ( "/tmp/SysRegTemp", false );
	QStringList grouplist;
	confSYS.getGroupsKeyList ( grouplist );
	for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
		name = confSYS.readEntry ( *it, "Name", "" );
		QString Nval = lang.readEntry( QString("TRANSLATE"), name, name ); 
		exec = confSYS.readEntry ( *it, "Exec", "" );
		dir = confSYS.readEntry ( *it, "Directory", "" );
		java = confSYS.readEntry ( *it, "JavaId", "" );
		ico = confSYS.readEntry ( *it, "BigIcon", "" );
		type = confSYS.readEntry ( *it, "Type", "" );
		int agregar=0;
		if ( (name!="")&&(ico!="")&&(exec!="") ) agregar=1; 
		else if ( (name!="")&&(ico!="")&&(type=="4") ) agregar=1;
		if ( agregar==1 )
		{
			ZListBoxItem* listitem1 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
			int i = dir.findRev("../.."); if ( i==0 ) { dir.remove(0,5); }
			i = ico.findRev("/"); if ( i > -1 ) { if ( ico[0] != QChar('/') ) { ico=dir+"/"+ico; } }
			QPixmap pixmap=QPixmap(iconReader.getIcon(ico, false));	pixmap.load(ico);		
			if ( pixmap.height() > 19 ) { QImage image; image = pixmap.convertToImage(); 
			image = image.smoothScale(19, 19); pixmap.convertFromImage(image); }     
			listitem1->setPixmap ( 0, pixmap );
			listitem1->appendSubItem ( 1, Nval, false, 0 );
			browser->insertItem ( listitem1, -1, true );
		}
	}	
	browser->sort(true);
	browser->setCurrentItem(0,true);


  browser->enableMarquee(ZListBox::LISTITEM_REGION_C, true);
  connect(browser, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(brClicked()));

  myVBoxLayout->addWidget ( browser, 1 );
  
  setContentWidget ( browser );
  
  ZSoftKey *softKey = new ZSoftKey ( "CST_2A", this, this );
  //ZSoftKey *softKey = this->getSoftKey( true );

  softKey->setText ( ZSoftKey::RIGHT, texto(lang.readEntry(QString("LANGUAGE"), "CANCEL", "")), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  
  softKey->setTextForOptMenuHide ( texto(lang.readEntry(QString("LANGUAGE"), "FUNCTIONS", "")) );
  softKey->setTextForOptMenuShow ( texto(lang.readEntry(QString("LANGUAGE"), "CHOISE", "")), texto(lang.readEntry(QString("LANGUAGE"), "CANCEL", "")) );
  
  softKey->setText ( ZSoftKey::LEFT, texto(lang.readEntry(QString("LANGUAGE"), "FUNCTIONS", "")), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_cancel() ) );
  
	QRect rect;
	menuPaste = new ZOptionsMenu ( rect, this, NULL , 0 , ZSkinService::clsZOptionsMenu );
	menuPaste->setItemSpacing ( 10 );
	softKey->setOptMenu ( ZSoftKey::LEFT, menuPaste );
	QPixmap* pm  = new QPixmap ( ProgDir + IDB_IMAGE_CANCEL );
	menuPaste->insertItem ( texto(lang.readEntry(QString("LANGUAGE"), "CANCEL", "")), NULL, pm , true , 0 , 0 );
	menuPaste->connectItem ( 0, this, SLOT ( slot_cancel() ) );
	this->setCSTWidget( softKey );

}


ZOpenWith::~ZOpenWith()
{
  //launcher->CreateSoftKeys(NULL);
}

QString ZOpenWith::texto(const char*xString)
{
	ZConfig IDini(Idioma, false);	
	QString	IDval = IDini.readEntry(QString("LANGUAGE"), QString("UNICODE"), "");
	if ( IDval == "1" ) { 
		QTextCodec* txtcodec = QTextCodec::codecForName("UTF-8");
		return txtcodec->toUnicode(xString); }
	else { return xString; }
}  

void ZOpenWith::accept()
{
	brClicked();
}


void ZOpenWith::selectItem(int idx)
{

}

void ZOpenWith::slot_cancel()
{
  reject();
}


void ZOpenWith::brClicked()
{

  QString ProgDir =  QString ( qApp->argv() [0] );
  int j = ProgDir.findRev ( "/" );
  ProgDir.remove ( j+1, ProgDir.length() - j );
  ZConfig lang (Idioma,false);

  int index = browser->currentItem();
  if (index == -1) {return;}
  ZListBoxItem* listitem = browser->item ( index );
  QString app = listitem->getSubItemText ( 1, 0, true );

  QString selected; QFile entrada("/tmp/selectedfolder"); QTextStream stentrada(&entrada);
  if ( entrada.open(IO_ReadOnly | IO_Translate) ) { selected = stentrada.readLine(); } entrada.close();

  QString destino;
  system("cat /mmc/mmca1/.system/java/CardRegistry /ezxlocal/download/appwrite/am/UserRegistry /usr/data_resource/preloadapps/SysRegistry > /tmp/SysRegTemp");

	ZConfig confSYS ( "/tmp/SysRegTemp", false );
	QStringList grouplist;
	confSYS.getGroupsKeyList ( grouplist );
	for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
		QString name = confSYS.readEntry ( *it, "Name", "" );
		QString exec = confSYS.readEntry ( *it, "Exec", "" );
		QString dir = confSYS.readEntry ( *it, "Directory", "" );
		int i = dir.findRev ( "../../" ); dir.remove ( i, 5 );
		if ( (exec!="")&&(name!=""))
		{
		  if ( QString(name) == QString(app) ) {
			if ( dir[0]!= QChar('/') ) { dir=QString("/usr/SYSqtapp/%1").arg(dir); }
			destino = QString("%1/%2").arg(dir).arg(exec); }
			int encontro = destino.find( "//" , 0 );
			if ( encontro > -1 ) { destino.remove( encontro,1 ); }
		}
	}	
	  ZMessageDlg* msg =  new ZMessageDlg ( QString(APP_NAME), QString(texto(lang.readEntry(QString("LANGUAGE"), "ASOCIATEFILE", ""))).arg(app),
			ZMessageDlg::yes_no, 0, NULL, "ZMessageDlg", true, 0 );
	  int i = msg->exec();
	  delete msg;
	  msg = NULL;
	  if (i)
	  {
		QString sel = selected; int i = sel.findRev ( "." ); sel.remove ( 0, i+1 );
		ZConfig ini(ProgDir + "/mimetype.cfg", true);
		ini.writeEntry(QString("MIME"), QString("%1").arg(sel).lower(), destino );
	  }
	  system("rm /tmp/SysRegTemp");


	QString extension = selected;
	i = extension.findRev ( "." ); extension.remove ( 0, i +1 );
	QString eten = QString("%1").arg(extension).lower();
	QString tipo = "";

	if ((eten=="jpg")||(eten=="jpeg")||(eten=="gif")||(eten=="png")||(eten=="bmp")||(eten=="tif")||(eten=="tiff")) tipo="Image";
	else if ((eten=="aac")||(eten=="ac3")||(eten=="amr")||(eten=="mp3")||(eten=="wav")||(eten=="imy")||(eten=="wma")) tipo="Music";
	//else if ((eten=="3gp")||(eten=="rm")||(eten=="mp4")) tipo="Video";

	if ( (destino=="/usr/SYSqtapp/mediaplayer/mediaplayer") && (tipo=="Image") )
	{
		AM_ActionData ad; ad.setAction("ViewPictureInMediaFinder");
		ad.setValue( "FileName", QString("%1").arg(selected.utf8()) ); 
		ad.invoke();    
	}
	else if ( (destino=="/usr/SYSqtapp/mediaplayer/mediaplayer") && (tipo=="Music") )
	{
		/*AM_ActionData ad; ad.setAction("LaunchMusicChannel");
		ad.setValue( "FileName", QString("%1").arg(fName.utf8()) ); 
		ad.invoke();*/  
		system("busybox killall alertprocess");
		system("busybox killall mediaplayer");
		system(QString("%1 -d \"%2\" &").arg(destino.utf8()).arg(selected.utf8()));
	}
	else
	{
		system(QString("%1 -d \"%2\"").arg(destino.utf8()).arg(selected.utf8()));
	}

  MyBaseDlg::accept();


}

//extern ZLauncher *launcher;
//************************************************************************************************
//************************************** class ZFolderSelect ***********************************
//************************************************************************************************
ZFolderSelect::ZFolderSelect(bool onlyDirs, QString path)
  :MyBaseDlg()
{
  QString ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma = ProgDir + "/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma).isFile() ) Idioma = ProgDir + "/languages/en-us";
  ZConfig lang (Idioma,false); 

  this->setMainWidgetTitle("zFileManager");
  
  QWidget *myWidget = new ZWidget ( this, NULL, 0);
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );
  myWidget->setFixedWidth(240);
  ZAppInfoArea* pAIA = new ZAppInfoArea(myWidget, NULL, 0);
  pAIA->setActArea(ZAppInfoArea::no_progress_meter);
  QPixmap pixmap; pixmap.load(ProgDir+"/img/settings.png");
  pAIA->appendIcon(ZAppInfoArea::activity,pixmap,"");
  pAIA->appendText(ZAppInfoArea::activity, lang.readEntry(QString("LANGUAGE"), "HOME", ""), "name");
  setAppInfoArea(pAIA);
  myVBoxLayout->insertWidget (0, pAIA );

  fb = new FileBrowser("*", onlyDirs, QString ( "%I%M" ), myWidget, 0);
  fb->setDir(path);
  QObject::connect(fb, SIGNAL(isFilePicked(bool)), SLOT(filePicked(bool)));

  fb->ProgramDir = ProgDir;

  setContentWidget ( fb );
  
  ZSoftKey *softKey = new ZSoftKey ( "CST_2A", this, this );
  
  softKey->setText ( ZSoftKey::RIGHT, texto(lang.readEntry(QString("LANGUAGE"), "CANCEL", "")), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setText ( ZSoftKey::LEFT, texto(lang.readEntry(QString("LANGUAGE"), "CHOISE", "")), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_cancel() ) );
  softKey->setClickedSlot ( ZSoftKey::LEFT, this, SLOT ( accept() ) );
   
  this->setCSTWidget( softKey );

}

ZFolderSelect::~ZFolderSelect()
{
  //launcher->CreateSoftKeys(NULL);
}

QString ZFolderSelect::texto(const char*xString)
{
	ZConfig IDini(Idioma, false);	
	QString	IDval = IDini.readEntry(QString("LANGUAGE"), QString("UNICODE"), "");
	if ( IDval == "1" ) { 
		QTextCodec* txtcodec = QTextCodec::codecForName("UTF-8");
		return txtcodec->toUnicode(xString); }
	else { return xString; }
} 

void ZFolderSelect::accept()
{
  fileName = fb->getFileName();
  basePath = fb->getFilePath();
  if (fileName != "") { MyBaseDlg::accept(); }
  else {
    ZMessageDlg * dlg = new ZMessageDlg ( "Error", QString("Filename is empty"), ZMessageDlg::just_ok, 0, this, "1", true, 0 );
    dlg->exec();
    delete dlg; dlg = NULL;
	
  }
}


void ZFolderSelect::selectItem(int idx)
{

}

void ZFolderSelect::slot_cancel()
{
  reject();
}

void ZFolderSelect::slot_paste()
{
  accept();
}


void ZFolderSelect::slot_makedir()
{
  ZConfig lang ( Idioma, false );

  QString ASource = fb->getFilePath();
  if (ASource.isEmpty()) return;
  QString dirName;
  ZSingleCaptureDlg* dlgq = new ZSingleCaptureDlg ( texto(lang.readEntry(QString("LANGUAGE"), "MAKING", "")), 
		texto(lang.readEntry(QString("LANGUAGE"), "ENTER_DIRNAME", "")), ZSingleCaptureDlg::normal, this, NULL, true, 0, 0 );
  if ( dlgq->exec() ) {
    dirName = dlgq->getResultText();
    if ( dirName != "" ) {
      if ( ! fb->makedir(ASource, dirName) ) {
        ZMessageDlg* dlgz  = new ZMessageDlg ( texto(lang.readEntry(QString("LANGUAGE"), "ERROR", "")), 
			texto(lang.readEntry(QString("LANGUAGE"), "MKDIR_ERROR", "")),  ZMessageDlg::just_ok , 0 , this , "about" , true , 0 );
        dlgz->exec();
        delete dlgz;
      } else {
        fb->setDir(ASource); 
      }
    }
  }    
  
}  


//************************************************************************************************
//************************************** class ZAdminFav *****************************************
//************************************************************************************************
ZAdminFav::ZAdminFav()
  :MyBaseDlg()
{
  QString ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma = ProgDir + "/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma).isFile() ) Idioma = ProgDir + "/languages/en-us";
  ZConfig lang (Idioma,false); 

  this->setMainWidgetTitle("zFileManager");
  
  QWidget *myWidget = new ZWidget ( this, NULL, 0);
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );
  myWidget->setFixedWidth(240);
  ZAppInfoArea* pAIA = new ZAppInfoArea(myWidget, NULL, 0);
  pAIA->setActArea(ZAppInfoArea::no_progress_meter);
  QPixmap pixmap; pixmap.load(ProgDir+"/img/favsmall.png");
  pAIA->appendIcon(ZAppInfoArea::activity,pixmap,"");
  pAIA->appendText(ZAppInfoArea::activity, lang.readEntry(QString("LANGUAGE"), "FAVS", ""), "name");
  setAppInfoArea(pAIA);
  myVBoxLayout->insertWidget (0, pAIA );

  browser = new ZListBox ( QString ( "%I%M" ), this, 0);
  browser->setFixedWidth ( 240 );
  QPixmap p1;

  CargarFavs();

  connect(browser, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(brClicked()));

  myVBoxLayout->addWidget ( browser, 1 );
  
  setContentWidget ( browser );

  ZSoftKey *softKey = new ZSoftKey ( "CST_2A", this, this );
  softKey->setText ( ZSoftKey::RIGHT, texto(lang.readEntry(QString("LANGUAGE"), "GOBACK", "")), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setTextForOptMenuHide ( texto(lang.readEntry(QString("LANGUAGE"), "FUNCTIONS", "")) );
  softKey->setTextForOptMenuShow ( texto(lang.readEntry(QString("LANGUAGE"), "CHOISE", "")), texto(lang.readEntry(QString("LANGUAGE"), "CANCEL", "")) );
  softKey->setText ( ZSoftKey::LEFT, texto(lang.readEntry(QString("LANGUAGE"), "FUNCTIONS", "")), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_cancel() ) );

  QRect rect;
  menuMain = new ZOptionsMenu ( rect, this, NULL , 0 , ZSkinService::clsZOptionsMenu );
  menuMain->setItemSpacing ( 10 );
  softKey->setOptMenu ( ZSoftKey::LEFT, menuMain );
  
  menuMain->setItemSpacing ( 10 ); int idx = 0;
  QPixmap* pm  = new QPixmap(ProgDir+ IDB_IMAGE_DELETE);
  menuMain->insertItem ( texto(lang.readEntry(QString("LANGUAGE"), "FAVDELETE", "")), NULL, pm , true , idx , idx );
  menuMain->connectItem ( idx, this, SLOT ( BorrarFav() ) ); ++idx;
  pm->load(ProgDir+ IDB_IMAGE_EDIT);
  menuMain->insertItem ( texto(lang.readEntry(QString("LANGUAGE"), "FAVEDIT", "")), NULL , pm, true , idx , idx );
  menuMain->connectItem ( idx, this, SLOT ( EditarFav() ) ); ++idx;

  this->setCSTWidget( softKey );

}


ZAdminFav::~ZAdminFav()
{
  //launcher->CreateSoftKeys(NULL);
}

QString ZAdminFav::texto(const char*xString)
{
	ZConfig IDini(Idioma, false);	
	QString	IDval = IDini.readEntry(QString("LANGUAGE"), QString("UNICODE"), "");
	if ( IDval == "1" ) { 
		QTextCodec* txtcodec = QTextCodec::codecForName("UTF-8");
		return txtcodec->toUnicode(xString); }
	else { return xString; }
}  

void ZAdminFav::CargarFavs()
{
  QString ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  ZListBoxItem* Item = new ZListBoxItem ( browser, QString ( "%I%M" ) );
  Item->appendSubItem ( 1, QString ( ".." ), false, NULL );
  browser->insertItem ( Item, 0,true );
  int ItemSize = browser->itemHeight(0)-2 ; if ( ItemSize>20 ) ItemSize=20;
  browser->clear();

  RES_ICON_Reader iconReader;

  QString destino; ZConfig IC(ProgDir + "/settings.cfg", true);
  QString SIcon = IC.readEntry(QString("FM"), QString("IconSource"), "");
  if ( SIcon=="0" ) { destino="/foldericons.cfg"; } else { destino="/sfoldericons.cfg"; }

  ZConfig iniF(ProgDir + "/favs.cfg", true);
  ZConfig fini(ProgDir + destino, true);	
  QFile entrada(ProgDir + "/favs.cfg");
  QTextStream stentrada(&entrada); 
  if ( entrada.open(IO_ReadOnly | IO_Translate) )
  { while ( !stentrada.eof() ) { 
	QString line = stentrada.readLine(); 
	if ( line.length() > 0 ) {
	  if ( line[0] != QChar('[') ) {
		int i = line.findRev ( " = " );
		line.remove ( i, line.length() - i );
		QPixmap p1;
		ZListBoxItem* listitem1 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
		QString line2 = iniF.readEntry(QString("BOOKMARKS"), line, "" );
		QString	mval = fini.readEntry(QString("FOLDERICONS"), line2, "");
		if ( SIcon=="0" ) {
			if ( mval == "" ) { mval="normal.png"; }
			p1=QPixmap ( ProgDir + "/img/folders/" + mval );
		} else {
			if ( mval == "" ) { mval="fm_folder_thb"; }
			p1=QPixmap( iconReader.getIcon(mval, false) );
		}
		if ( p1.height() > 18 ) { QImage image; image = p1.convertToImage(); 
		image = image.smoothScale(18, 18); p1.convertFromImage(image); }  
		listitem1->setPixmap ( 0, p1 );
		listitem1->appendSubItem ( 1, line, false, 0 );
		//listitem1->appendSubItem ( 1, line2, false, 0 );
		browser->insertItem ( listitem1,-1,true );
	} }
  } } entrada.close();
  browser->enableMarquee(ZListBox::LISTITEM_REGION_C, true);

}

void ZAdminFav::EditarFav()
{
  QString ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  ZConfig lang (Idioma, false);
  int index = browser->currentItem();
  if (index == -1) {return;}
  ZListBoxItem* listitem = browser->item ( index );
  QString app = listitem->getSubItemText ( 1, 0, true );  
  QString oldapp = listitem->getSubItemText ( 1, 0, true );  

  ZConfig ini(ProgDir + "/favs.cfg", true);
  QString ruta = ini.readEntry(QString("BOOKMARKS"), app, "");

  ZSingleCaptureDlg* zscd = new ZSingleCaptureDlg( texto(lang.readEntry(QString("LANGUAGE"), "ADDFAV", "")), 
		texto(lang.readEntry(QString("LANGUAGE"), "FAVNAME", "")), ZSingleCaptureDlg::normal, NULL, "ZSingleCaptureDlg", true, 0, 0);
  ZLineEdit* zle = (ZLineEdit*)zscd->getLineEdit();  
  zle->setText(app);
  zle->setEdited(true);  
  if (zscd->exec() == 1) {
	if ( zle->text() != "" ) {
	  ZSingleCaptureDlg* zscd2 = new ZSingleCaptureDlg( texto(lang.readEntry(QString("LANGUAGE"), "ADDFAV", "")),
			texto(lang.readEntry(QString("LANGUAGE"), "FAVDIR", "")), ZSingleCaptureDlg::normal, NULL, "ZSingleCaptureDlg", true, 0, 0);
	  ZLineEdit* zle2 = (ZLineEdit*)zscd2->getLineEdit();  
	  zle2->setText(ruta);
	  zle2->setEdited(true);  
	  if (zscd2->exec() == 1) {
		if ( zle2->text() != "" ) {
		  QString fName = zle->text();
		  QString fName2 = zle2->text();
		  ini.writeEntry(QString("BOOKMARKS"), fName, fName2);
		  if ( zle->text() != app ) {
			  ini.removeEntry(QString("BOOKMARKS"), oldapp);
		  }
		  browser->clear();
		  CargarFavs();
		}
	  }
	}
  }

}

void ZAdminFav::BorrarFav()
{
  QString ProgDir =  QString ( qApp->argv() [0] );
  int k = ProgDir.findRev ( "/" );
  ProgDir.remove ( k+1, ProgDir.length() - k );

  ZConfig lang (Idioma, false);
  int index = browser->currentItem();
  if (index == -1) {return;}
  ZListBoxItem* listitem = browser->item ( index );
  QString app = listitem->getSubItemText ( 1, 0, true );  

  ZMessageDlg* msg =  new ZMessageDlg ( QString(APP_NAME), QString(texto(lang.readEntry(QString("LANGUAGE"), "DELETE_FAV", ""))).arg(app),
        ZMessageDlg::yes_no, 0, NULL, "ZMessageDlg", true, 0 );
  int i = msg->exec();
  delete msg;
  msg = NULL;
  if (i)
  {
	ZConfig ini(ProgDir + "/favs.cfg", true);
	ini.removeEntry(QString("BOOKMARKS"), app);
	browser->removeItem(index);
  }

}

void ZAdminFav::accept()
{
//	brClicked();
}


void ZAdminFav::selectItem(int idx)
{
  //qDebug(QString("********\nIdx: aboutToShow")); 

}

void ZAdminFav::slot_cancel()
{
  //qDebug(QString("**********************\ncancel"));
  reject();
}


void ZAdminFav::brClicked()
{
  QString ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  int index = browser->currentItem();
  if (index == -1) {return;}
  ZListBoxItem* listitem = browser->item ( index );
  QString app = listitem->getSubItemText ( 1, 0, true );  

  ZConfig iniF(ProgDir + "/favs.cfg", true);

  fileName = iniF.readEntry(QString("BOOKMARKS"), app, "" );
  basePath = fileName;
  //qDebug(QString("***************\n SELECCIONADO" + basePath));  
  MyBaseDlg::accept();

}


//************************************************************************************************
//************************************** class ZSettings *****************************************
//************************************************************************************************
ZSettings::ZSettings()
  :MyBaseDlg()
{
  QString ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma = ProgDir + "/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma).isFile() ) Idioma = ProgDir + "/languages/en-us";
  ZConfig lang (Idioma,false); 

  SelectProg=0;

  this->setMainWidgetTitle("zFileManager");
  
  QWidget *myWidget = new ZWidget ( this, NULL, 0);
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );
  myWidget->setFixedWidth(240);
  ZAppInfoArea* pAIA = new ZAppInfoArea(myWidget, NULL, 0);
  pAIA->setActArea(ZAppInfoArea::no_progress_meter);
  QPixmap pixmap; pixmap.load(ProgDir+"/img/settings.png");
  pAIA->appendIcon(ZAppInfoArea::activity,pixmap,"");
  pAIA->appendText(ZAppInfoArea::activity, lang.readEntry(QString("LANGUAGE"), "SETTINGS", ""), "name");
  setAppInfoArea(pAIA);
  myVBoxLayout->insertWidget (0, pAIA );

  browser = new ZListBox ( QString ( "%I%M" ), this, 0);
  browser->setFixedWidth ( 240 );
  QPixmap p1;

  CargarSettings();

  connect(browser, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(setClicked()));

  myVBoxLayout->addWidget ( browser, 1 );
  
  setContentWidget ( browser );

  ZSoftKey *softKey = new ZSoftKey ( "CST_2A", this, this );
  softKey->setText ( ZSoftKey::RIGHT, texto(lang.readEntry(QString("LANGUAGE"), "GOBACK", "")), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setText ( ZSoftKey::LEFT, texto(lang.readEntry(QString("LANGUAGE"), "CHOISE", "")), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( accept() ) );
  softKey->setClickedSlot ( ZSoftKey::LEFT, this, SLOT ( setClicked() ) );

  this->setCSTWidget( softKey );

}


ZSettings::~ZSettings()
{
  //launcher->CreateSoftKeys(NULL);
}

QString ZSettings::texto(const char*xString)
{
	ZConfig IDini(Idioma, false);	
	QString	IDval = IDini.readEntry(QString("LANGUAGE"), QString("UNICODE"), "");
	if ( IDval == "1" ) { 
		QTextCodec* txtcodec = QTextCodec::codecForName("UTF-8");
		return txtcodec->toUnicode(xString); }
	else { return xString; }
}  

void ZSettings::setClicked()
{
  QString ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

	qApp->processEvents();
	int index = browser->currentItem();
	if (index == -1) {return;}

	if (SelectProg==0)
	{
		int result = index+1;
		if ( result == 1 ) { MostrarOcultos(); return; }
		else if ( result == 2 ) { CambiarVista(); return; }
		else if ( result == 3 ) { Miniaturas(); return; }
		else if ( result == 4 ) { FontSize(); return; }
		else if ( result == 5 ) { EditarHome(); return; }
		else if ( result == 6 ) { TextEditor(); return; }
	}
	else
	{
		QString destino1=""; QString destino2="";
		ZListBoxItem* listitem = browser->item ( index );
		QString app = listitem->getSubItemText ( 1, 0, true );
	
		ZConfig confSYS ( "/tmp/SysRegTemp", false );
		QStringList grouplist;
		confSYS.getGroupsKeyList ( grouplist );
		for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
			QString name = confSYS.readEntry ( *it, "Name", "" );
			QString exec = confSYS.readEntry ( *it, "Exec", "" );
			QString dir = confSYS.readEntry ( *it, "Directory", "" );
			int i = dir.findRev ( "../../" ); dir.remove ( i, 5 );
			if ( (exec!="")&&(name!=""))
			{
			  if ( QString(name) == QString(app) ) {
				  system(QString("echo \"-%1- -%2-\"").arg(name).arg(app));
				if ( dir[0]!= QChar('/') ) { dir=QString("/usr/SYSqtapp/%1").arg(dir); }
				destino1 = name;
				destino2 = QString("%1/%2").arg(dir).arg(exec); }
				int encontro = destino2.find( "//" , 0 );
				if ( encontro > -1 ) { destino2.remove( encontro,1 ); }
			}
		}	
		if (destino1!="") 
		{
			ZConfig ini(ProgDir + "/settings.cfg", true);
			ini.writeEntry(QString("FM"), QString("TextName"), destino1 );
			ini.writeEntry(QString("FM"), QString("TextExec"), destino2 );
		}
		SelectProg=0;
		CargarSettings();
		browser->setCurrentItem(5,true);
	}

}

void ZSettings::CargarSettings()
{
  QString ProgramDir =  QString ( qApp->argv() [0] );
  int i = ProgramDir.findRev ( "/" );
  ProgramDir.remove ( i+1, ProgramDir.length() - i );

  ZConfig lang(Idioma,false);
  browser->clear();
  QPixmap pixmap;
  ZConfig ini(ProgramDir + "/settings.cfg", true);
  qApp->processEvents();


  ZListBoxItem* listitem1 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
  pixmap.load(ProgramDir + "/img/hidden.png");
  listitem1->setPixmap ( 0, pixmap );
  listitem1->appendSubItem ( 1, texto(lang.readEntry(QString("LANGUAGE"), "SHOWHIDDEN", "")), false, NULL );
  QString val = ini.readEntry(QString("FM"), QString("ShowHidden"), "");
  if ( val == "1" ) { listitem1->appendSubItem ( 1, texto(lang.readEntry(QString("LANGUAGE"), "ENABLED", "")), false, NULL ); }
  else { listitem1->appendSubItem ( 1, texto(lang.readEntry(QString("LANGUAGE"), "DISABLED", "")), false, NULL ); }
  browser->insertItem ( listitem1,-1,true );

  ZListBoxItem* listitem2 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
  pixmap.load(ProgramDir + "/img/view.png");
  listitem2->setPixmap ( 0, pixmap );
  listitem2->appendSubItem ( 1, texto(lang.readEntry(QString("LANGUAGE"), "VIEW", "")), false, NULL );
  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
  if ( val == "small" ) { listitem2->appendSubItem ( 1, texto(lang.readEntry(QString("LANGUAGE"), "VIEWLIST", "")), false, NULL ); }
  if ( val == "list" ) { listitem2->appendSubItem ( 1, texto(lang.readEntry(QString("LANGUAGE"), "VIEWDETAIL", "")), false, NULL ); }
  if ( val == "big" ) { listitem2->appendSubItem ( 1, texto(lang.readEntry(QString("LANGUAGE"), "VIEWBIG", "")), false, NULL ); }
  browser->insertItem ( listitem2,-1,true );

  ZListBoxItem* listitem6 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
  pixmap.load(ProgramDir + "/img/pic.png");
  listitem6->setPixmap ( 0, pixmap );
  listitem6->appendSubItem ( 1, texto(lang.readEntry(QString("LANGUAGE"), "SHOWPREVIEW", "")), false, NULL );
  val = ini.readEntry(QString("FM"), QString("ShowPreview"), "No");
  if ( val == "Yes" ) { listitem6->appendSubItem ( 1, texto(lang.readEntry(QString("LANGUAGE"), "ENABLED", "")), false, NULL ); }
  else { listitem6->appendSubItem ( 1, texto(lang.readEntry(QString("LANGUAGE"), "DISABLED", "")), false, NULL ); }
  browser->insertItem ( listitem6,-1,true );

  ZListBoxItem* listitem3 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
  pixmap.load(ProgramDir + "/img/font.png");
  listitem3->setPixmap ( 0, pixmap );
  listitem3->appendSubItem ( 1, texto(lang.readEntry(QString("LANGUAGE"), "FONT_SIZE", "")), false, NULL );
  int ival = ini.readNumEntry(QString("FM"), QString("FontSize"), 10);
  listitem3->appendSubItem ( 1, QString("%1").arg(ival), false, NULL );
  browser->insertItem ( listitem3,-1,true );

  ZListBoxItem* listitem5 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
  pixmap.load(ProgramDir + "/img/folder.png");
  listitem5->setPixmap ( 0, pixmap );
  listitem5->appendSubItem ( 1, texto(lang.readEntry(QString("LANGUAGE"), "HOME", "")), false, NULL );
  val = ini.readEntry(QString("FM"), QString("Start"), "");
  if ( val=="0" ) val = lang.readEntry(QString("LANGUAGE"), "LASTFOLDER", "");
  listitem5->appendSubItem ( 1, QString(val), false, NULL );
  browser->insertItem ( listitem5,-1,true );

  ZListBoxItem* listitem4 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
  pixmap.load(ProgramDir + "/img/text.png");
  listitem4->setPixmap ( 0, pixmap );
  listitem4->appendSubItem ( 1, texto(lang.readEntry(QString("LANGUAGE"), "TEXTEDIT", "")), false, NULL );
  val = ini.readEntry(QString("FM"), QString("TextName"), "");
  listitem4->appendSubItem ( 1, QString(val), false, NULL );
  browser->insertItem ( listitem4,-1,true );

}

QString ZSettings::getFileName(const QString &AFileName)
{
  QString str = AFileName;
  int i = str.findRev ( "/" );
  str.remove ( 0, i);
  if (str.left(1) = "/") str.remove ( 0, 1);
  return str;    
}

void ZSettings::TextEditor()
{
	SelectProg=1;
	browser->clear();

	system("cat /mmc/mmca1/.system/java/CardRegistry /ezxlocal/download/appwrite/am/UserRegistry /usr/data_resource/preloadapps/SysRegistry > /tmp/SysRegTemp");

	RES_ICON_Reader iconReader;
	QString name, exec, dir, ico, java, type; QPixmap p1;

	ZConfig confSYS ( "/tmp/SysRegTemp", false );
	QStringList grouplist;
	confSYS.getGroupsKeyList ( grouplist );
	for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
		name = confSYS.readEntry ( *it, "Name", "" );
		QString Nval = name; //lang.readEntry( QString("TRANSLATE"), name, name ); 
		exec = confSYS.readEntry ( *it, "Exec", "" );
		dir = confSYS.readEntry ( *it, "Directory", "" );
		java = confSYS.readEntry ( *it, "JavaId", "" );
		ico = confSYS.readEntry ( *it, "BigIcon", "" );
		type = confSYS.readEntry ( *it, "Type", "" );
		int agregar=0;
		if ( (name!="")&&(ico!="")&&(exec!="") ) agregar=1; 
		else if ( (name!="")&&(ico!="")&&(type=="4") ) agregar=1;
		if ( agregar==1 )
		{
			ZListBoxItem* listitem1 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
			int i = dir.findRev("../.."); if ( i==0 ) { dir.remove(0,5); }
			i = ico.findRev("/"); if ( i > -1 ) { if ( ico[0] != QChar('/') ) { ico=dir+"/"+ico; } }
			QPixmap pixmap=QPixmap(iconReader.getIcon(ico, false));	pixmap.load(ico);		
			if ( pixmap.height() > 19 ) { QImage image; image = pixmap.convertToImage(); 
			image = image.smoothScale(19, 19); pixmap.convertFromImage(image); }     
			listitem1->setPixmap ( 0, pixmap );
			listitem1->appendSubItem ( 1, Nval, false, 0 );
			browser->insertItem ( listitem1, -1, true );
		}
	}	

  browser->enableMarquee(ZListBox::LISTITEM_REGION_C, true);
  browser->sort(true);
  browser->setCurrentItem(0,true);

}


void ZSettings::MostrarOcultos()
{
  QString ProgramDir =  QString ( qApp->argv() [0] );
  int i = ProgramDir.findRev ( "/" );
  ProgramDir.remove ( i+1, ProgramDir.length() - i );

    ZConfig IDini(Idioma, false);
	QString IDval = IDini.readEntry(QString("LANGUAGE"), QString("SHOWHIDDEN"), "");
	ZSingleSelectDlg* eq1 =  new ZSingleSelectDlg(texto(IDval), QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
	QStringList list;
	IDval = IDini.readEntry(QString("LANGUAGE"), QString("ENABLED"), "");
	list = texto(IDval); 
	IDval = IDini.readEntry(QString("LANGUAGE"), QString("DISABLED"), "");
	list += texto(IDval);
	eq1->addItemsList(list);
	ZListBox* listboxx = eq1->getListBox();
	ZConfig Fini(QString("%1/settings.cfg").arg(ProgramDir), true);
	QString Fval = Fini.readEntry(QString("FM"), QString("ShowHidden"), "");
	if ( Fval != "0" ) { listboxx->checkItem(0, true); }
	else { listboxx->checkItem(1, true); }
	eq1->exec();
	int result = eq1->result();
	if (result != 1) {
		result = 0;
	} else {
		if (listboxx->itemChecked(1)) { Fini.writeEntry(QString("FM"), QString("ShowHidden"), "0"); }
		if (listboxx->itemChecked(0)) { Fini.writeEntry(QString("FM"), QString("ShowHidden"), "1"); }
		CargarSettings();
		browser->setCurrentItem(0,true);
	}
}

void ZSettings::Miniaturas()
{
  QString ProgramDir =  QString ( qApp->argv() [0] );
  int i = ProgramDir.findRev ( "/" );
  ProgramDir.remove ( i+1, ProgramDir.length() - i );

    ZConfig IDini(Idioma, false);
	QString IDval = IDini.readEntry(QString("LANGUAGE"), QString("SHOWPREVIEW"), "");
	ZSingleSelectDlg* eq1 =  new ZSingleSelectDlg(texto(IDval), QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
	QStringList list;
	IDval = IDini.readEntry(QString("LANGUAGE"), QString("ENABLED"), "");
	list = texto(IDval); 
	IDval = IDini.readEntry(QString("LANGUAGE"), QString("DISABLED"), "");
	list += texto(IDval);
	eq1->addItemsList(list);
	ZListBox* listboxx = eq1->getListBox();
	ZConfig Fini(QString("%1/settings.cfg").arg(ProgramDir), true);
	QString Fval = Fini.readEntry(QString("FM"), QString("ShowPreview"), "No");
	if ( Fval != "No" ) { listboxx->checkItem(0, true); }
	else { listboxx->checkItem(1, true); }
	eq1->exec();
	int result = eq1->result();
	if (result != 1) {
		result = 0;
	} else {
		if (listboxx->itemChecked(1)) { Fini.writeEntry(QString("FM"), QString("ShowPreview"), "No"); }
		if (listboxx->itemChecked(0)) { Fini.writeEntry(QString("FM"), QString("ShowPreview"), "Yes"); }
		CargarSettings();
		browser->setCurrentItem(2,true);
	}
}

void ZSettings::CambiarVista()
{
  QString ProgramDir =  QString ( qApp->argv() [0] );
  int i = ProgramDir.findRev ( "/" );
  ProgramDir.remove ( i+1, ProgramDir.length() - i );

    ZConfig IDini(Idioma, false);
	QString IDval = IDini.readEntry(QString("LANGUAGE"), QString("VIEW"), "");
	ZSingleSelectDlg* eq1 =  new ZSingleSelectDlg(texto(IDval), QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
	QStringList list;
	IDval = IDini.readEntry(QString("LANGUAGE"), QString("VIEWLIST"), "");
	list = texto(IDval); 
	IDval = IDini.readEntry(QString("LANGUAGE"), QString("VIEWDETAIL"), "");
	list += texto(IDval);
	IDval = IDini.readEntry(QString("LANGUAGE"), QString("VIEWBIG"), "");
	list += texto(IDval);
	eq1->addItemsList(list);
	ZListBox* listboxx = eq1->getListBox();
	ZConfig Fini(QString("%1/settings.cfg").arg(ProgramDir), true);
	QString Fval = Fini.readEntry(QString("FM"), QString("IconStyle"), "");
	if ( Fval == "small" ) { listboxx->checkItem(0, true); }
	if ( Fval == "list" ) { listboxx->checkItem(1, true); }
	if ( Fval == "big" ) { listboxx->checkItem(2, true); }
	eq1->exec();
	int result = eq1->result();
	if (result != 1) {
		result = 0;
	} else {
		if (listboxx->itemChecked(0)) { VerLista(); CargarSettings(); browser->setCurrentItem(1,true); }
		if (listboxx->itemChecked(1)) { VerDetalle(); CargarSettings(); browser->setCurrentItem(1,true); }
		if (listboxx->itemChecked(2)) { VerGrandes(); CargarSettings(); browser->setCurrentItem(1,true); }
	}
}

void ZSettings::VerLista()
{
  QString ProgramDir =  QString ( qApp->argv() [0] );
  int i = ProgramDir.findRev ( "/" );
  ProgramDir.remove ( i+1, ProgramDir.length() - i );
  ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), true);
  ini.writeEntry(QString("FM"), QString("IconStyle"), "small");
}


void ZSettings::VerDetalle()
{
  QString ProgramDir =  QString ( qApp->argv() [0] );
  int i = ProgramDir.findRev ( "/" );
  ProgramDir.remove ( i+1, ProgramDir.length() - i );
  ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), true);
  ini.writeEntry(QString("FM"), QString("IconStyle"), "list");
}

void ZSettings::VerGrandes()
{
  QString ProgramDir =  QString ( qApp->argv() [0] );
  int i = ProgramDir.findRev ( "/" );
  ProgramDir.remove ( i+1, ProgramDir.length() - i );
  ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), true);
  ini.writeEntry(QString("FM"), QString("IconStyle"), "big");
}

void ZSettings::EditarHome()
{
	QString ProgramDir =  QString ( qApp->argv() [0] );
	int i = ProgramDir.findRev ( "/" );
	ProgramDir.remove ( i+1, ProgramDir.length() - i );

    ZConfig IDini(Idioma, false);
	QString IDval = IDini.readEntry(QString("LANGUAGE"), QString("HOME"), "");
	ZSingleSelectDlg* eq1 =  new ZSingleSelectDlg(texto(IDval), QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
	QStringList list;
	IDval = IDini.readEntry(QString("LANGUAGE"), QString("LASTFOLDER"), "");
	list = texto(IDval); 
	IDval = IDini.readEntry(QString("LANGUAGE"), QString("HOMESELECT"), "");
	list += texto(IDval);
	eq1->addItemsList(list);
	ZListBox* listboxx = eq1->getListBox();
	ZConfig Fini(QString("%1/settings.cfg").arg(ProgramDir), true);
	QString Fval = Fini.readEntry(QString("FM"), QString("Start"), "");
	if ( Fval == "0" ) { listboxx->checkItem(0, true); }
	else { listboxx->checkItem(1, true); }
	eq1->exec();
	int result = eq1->result();
	if (result != 1) {
		result = 0;
	} else {
		if (listboxx->itemChecked(0))
		{
			Fini.writeEntry(QString("FM"), QString("Start"), "0");
			CargarSettings(); browser->setCurrentItem(4,true); 
		}
		else if (listboxx->itemChecked(1))
		{
			ZFolderSelect *zfod = new ZFolderSelect(true);
			if (zfod->exec() )
			{
				QString fDestPath = zfod->getFileName(); 
				int i = fDestPath.findRev ( "//" ); fDestPath.remove ( i, 1 );
				if ( ! fDestPath.isEmpty())
				{
					ZConfig ini(ProgramDir + "/settings.cfg", true);
					ini.writeEntry(QString("FM"), QString("Start"), fDestPath);
					browser->clear();
					CargarSettings();
					browser->setCurrentItem(4,true);
				}
			}
			delete zfod;
		}
	}


}

void ZSettings::FontSize()
{
	QString ProgramDir =  QString ( qApp->argv() [0] );
	int i = ProgramDir.findRev ( "/" );
	ProgramDir.remove ( i+1, ProgramDir.length() - i );

	ZConfig ini(Idioma, false);
    ZConfig IDini(ProgramDir + "/settings.cfg", true);
	int val = IDini.readNumEntry(QString("FM"), QString("FontSize"), 10);
	ZNumPickerDlg* textDialog = new ZNumPickerDlg(10, this, "ZNumPickerDlg", 0);
	ZNumModule* module = textDialog->getNumModule();
	module->setMaxValue(24);
	module->setMinValue(10);
	module->setValue(val);
	ZLabel* label = new ZLabel( "", module, NULL,0, (ZSkinService::WidgetClsID)4);
	module->setLabel(label, (ZNumModule::RelativeAlignment)0);
	textDialog->setTitle(ini.readEntry(QString("LANGUAGE"), QString("FONT_SIZE"), ""));
	if (textDialog->exec())
	{	
		int r = textDialog->getNumValue();
		IDini.writeEntry(QString("FM"), QString("FontSize"), r);
		qApp->processEvents();
	}
	CargarSettings();
	browser->setCurrentItem(3,true);
}

void ZSettings::accept()
{
  if (SelectProg==0)
  {
	MyBaseDlg::accept();
  }
  else 
  {
	SelectProg=0;
	CargarSettings();
	browser->setCurrentItem(5,true);
  }
}


void ZSettings::selectItem(int idx)
{
  //qDebug(QString("********\nIdx: aboutToShow")); 

}

void ZSettings::slot_cancel()
{
  reject();
}




//************************************************************************************************
//************************************** class ZClipboard ****************************************
//************************************************************************************************
ZClipboard::ZClipboard()
  :MyBaseDlg()
{
  QString ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma = ProgDir + "/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma).isFile() ) Idioma = ProgDir + "/languages/en-us";
  ZConfig lang (Idioma,false); 

  this->setMainWidgetTitle("zFileManager");
  
  QWidget *myWidget = new ZWidget ( this, NULL, 0);
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );
  myWidget->setFixedWidth(240);
  pAIA = new ZAppInfoArea(myWidget, NULL, 0);
  pAIA->setActArea(ZAppInfoArea::no_progress_meter);
  QPixmap pixmap; pixmap.load(ProgDir+"/img/paste.png");
  pAIA->appendIcon(ZAppInfoArea::activity,pixmap,"");
  pAIA->appendText(ZAppInfoArea::activity, lang.readEntry(QString("LANGUAGE"), "CLIPBOARD", ""), "name");
  setAppInfoArea(pAIA);
  myVBoxLayout->insertWidget (0, pAIA );

  browser = new ZListBox ( QString ( "%I%M" ), this, 0);
  browser->setFixedWidth ( 240 );
  QPixmap p1;

  CargarFavs();

  connect(browser, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(brClicked()));

  //fc->addChild(browser);

  setContentWidget ( browser );

  ZSoftKey *softKey = new ZSoftKey ( "CST_2A", this, this );
  softKey->setText ( ZSoftKey::RIGHT, texto(lang.readEntry(QString("LANGUAGE"), "GOBACK", "")), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setTextForOptMenuHide ( texto(lang.readEntry(QString("LANGUAGE"), "FUNCTIONS", "")) );
  softKey->setTextForOptMenuShow ( texto(lang.readEntry(QString("LANGUAGE"), "CHOISE", "")), texto(lang.readEntry(QString("LANGUAGE"), "CANCEL", "")) );
  softKey->setText ( ZSoftKey::LEFT, texto(lang.readEntry(QString("LANGUAGE"), "FUNCTIONS", "")), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_cancel() ) );

  QRect rect;
  menuMain = new ZOptionsMenu ( rect, this, NULL , 0 , ZSkinService::clsZOptionsMenu );
  menuMain->setItemSpacing ( 10 );
  softKey->setOptMenu ( ZSoftKey::LEFT, menuMain );
  
  menuMain->setItemSpacing ( 10 ); int idx = 0;
  QPixmap* pm  = new QPixmap(ProgDir+ IDB_IMAGE_DELETE);
  menuMain->insertItem ( texto(lang.readEntry(QString("LANGUAGE"), "DELETE_SELECTED", "")), NULL, pm , true , idx , idx );
  menuMain->connectItem ( idx, this, SLOT ( Borrar() ) ); ++idx;
  pm->load(ProgDir+ IDB_IMAGE_DELETE);
  menuMain->insertItem ( texto(lang.readEntry(QString("LANGUAGE"), "DELETE_ALL", "")), NULL , pm, true , idx , idx );
  menuMain->connectItem ( idx, this, SLOT ( Vaciar() ) ); ++idx;

  this->setCSTWidget( softKey );

}


ZClipboard::~ZClipboard()
{
  //launcher->CreateSoftKeys(NULL);
}

QString ZClipboard::texto(const char*xString)
{
	ZConfig IDini(Idioma, false);	
	QString	IDval = IDini.readEntry(QString("LANGUAGE"), QString("UNICODE"), "");
	if ( IDval == "1" ) { 
		QTextCodec* txtcodec = QTextCodec::codecForName("UTF-8");
		return txtcodec->toUnicode(xString); }
	else { return xString; }
}  

void ZClipboard::CargarFavs()
{
  QString ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  ZListBoxItem* Item = new ZListBoxItem ( browser, QString ( "%I%M" ) );
  Item->appendSubItem ( 1, QString ( ".." ), false, NULL );
  browser->insertItem ( Item, 0,true );
  int ItemSize = browser->itemHeight(0)-2 ; if ( ItemSize>20 ) ItemSize=20;
  browser->clear();

  RES_ICON_Reader iconReader;

  ZConfig SFIini(ProgDir + "/sfoldericons.cfg", false);	
  ZConfig SAIini(ProgDir + "/sfileicons.cfg", false);	

  QString destino; ZConfig IC(ProgDir + "/settings.cfg", true);
  destino="/sfoldericons.cfg";

  QFile entrada("/tmp/clipboard");
  QTextStream stentrada(&entrada); 
  if ( entrada.open (IO_ReadOnly | IO_Translate) )
  { while ( !stentrada.eof() ) { 
	QString line = stentrada.readLine(); 
	if ( line.length() > 0 )
	{
		QPixmap p1; QString mval;
		ZListBoxItem* listitem1 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
		QFileInfo* fInfo = new QFileInfo ( line );
		if ( fInfo->isDir() ) {		
			mval = SFIini.readEntry(QString("FOLDERICONS"), line, "");
			if ( mval == "" ) { mval="fm_folder_thb"; }
			p1=QPixmap( iconReader.getIcon(mval, false) );
			if ( p1.height() > 18 ) { QImage image; image = p1.convertToImage(); 
			image = image.smoothScale(18, 18); p1.convertFromImage(image); }  
			listitem1->setPixmap ( 0, p1 );
		} else if ( fInfo->isFile() ) {		
		    QString Eval = line;
			int i = Eval.findRev ( "." ); Eval.remove ( 0, i +1 );
			mval = SAIini.readEntry(QString("FILES"), QString("%1").arg(Eval).lower(), "");
			if ( mval == "" ) { mval="msg_unsupport_type_thb"; }
			p1=QPixmap( iconReader.getIcon(mval, false) );
			if ( p1.height() > 18 ) { QImage image; image = p1.convertToImage(); 
			image = image.smoothScale(18, 18); p1.convertFromImage(image); }  
			listitem1->setPixmap ( 0, p1 );
		} else return;
		//int i = line.findRev ( "/" ); line.remove ( 0, i); if (line.left(1) = "/") line.remove ( 0, 1);
		listitem1->appendSubItem ( 1, line, false, 0 );
		browser->insertItem ( listitem1,-1,true );
	}
  } } entrada.close();
  browser->enableMarquee(ZListBox::LISTITEM_REGION_C, true);

}

void ZClipboard::Borrar()
{
  QString ProgDir =  QString ( qApp->argv() [0] );
  int k = ProgDir.findRev ( "/" );
  ProgDir.remove ( k+1, ProgDir.length() - k );
  QString name = browser->item( browser->currentItem() )->getSubItemText ( 1, 0, true );

  QString TEXTO="";
  QFile entrada("/tmp/clipboard");
  QTextStream stentrada(&entrada); 
  if ( entrada.open (IO_ReadOnly | IO_Translate) )
  { 
	while ( !stentrada.eof() )
	{ 
		QString line = stentrada.readLine(); 
		if ( line.length() > 0 )
		{
			QString temp = line;
			if ( temp != name ) 
			{
				TEXTO += line; TEXTO += "\n";
			} 
		}
	}
  }
  entrada.close();
  system("rm /tmp/clipboard");
  if ( TEXTO != "" ) system(QString("echo \"%1\" > /tmp/clipboard").arg(TEXTO));
  browser->removeItem(browser->currentItem());



}

void ZClipboard::Vaciar()
{

  system("rm /tmp/clipboard");
  browser->clear();

}

void ZClipboard::accept()
{
//	brClicked();
}



void ZClipboard::slot_cancel()
{
  reject();
}


void ZClipboard::brClicked()
{

	QString ProgDir =  QString ( qApp->argv() [0] );
	int i = ProgDir.findRev ( "/" );
	ProgDir.remove ( i+1, ProgDir.length() - i );
	QString name = browser->item( browser->currentItem() )->getSubItemText ( 1, 0, true );

  	ZConfig ini(ProgDir + "/settings.cfg", true);
	QString FOLDER = ini.readEntry( QString("FM"), QString("LastFolder"),"");
	QString CopyPath = getFilePath(name);

	  if ( QFileInfo(name).isFile() ) 
	  {
		  PasteFiles(name, FOLDER, false);
	  }
	  else
	  {
		system(QString("find \"%1\" > /tmp/clipboard2").arg(name));
		QFile entrada2("/tmp/clipboard2"); QTextStream stentrada2(&entrada2);
		if ( entrada2.open(IO_ReadOnly | IO_Translate) ) {
		  while ( !stentrada2.eof() ) {           
			QString line2 = stentrada2.readLine();
			QString destino = line2;
			int k = destino.find ( CopyPath, 0 ); 
			destino.remove( k, CopyPath.length() );
			destino = FOLDER + "/" + destino;
			
			if ( QFileInfo(line2).isFile() ) { 
				system(QString("echo \"ARCHIVO: %1 a %2\n\"").arg(line2).arg(destino));
				PasteFiles(line2, getFilePath(destino), false); }
			else {
				QString carpeta = FOLDER + "/" + getFileName(line2);
				//int i = carpeta.find ( CopyPath, 0 );
				//carpeta.remove( i, CopyPath.length() );
				system(QString("echo \"CARPETA: %1\n\"").arg(carpeta));
				QDir dd; if ( ! dd.exists(carpeta) ) { dd.mkdir(carpeta); qApp->processEvents(); }
			}
			//QString line2 = stentrada2.readLine();
		  }
		} entrada2.close();
	  }

	launcher->CargarBrowser(FOLDER);
	reject();
}

QString ZClipboard::getFileName(const QString &AFileName)
{
  QString str = AFileName;
  int i = str.findRev ( "/" );
  str.remove ( 0, i);
  if (str.left(1) = "/") str.remove ( 0, 1);
  return str;    
}

QString ZClipboard::getFilePath(const QString &AFileName)
{
  QString str = AFileName;
  int i = str.findRev ( "/" );
  str.remove ( i+1, str.length() - i );
  return str;  
}

bool ZClipboard::PasteFiles(const QString &ASourse, const QString &ADest, bool remove)
{
  if (ASourse.isEmpty() ) return false;
  if (ADest.isEmpty() ) return false;
  
  ZConfig lang(Idioma, false);
  
  struct statfs stat;
  unsigned long freeSize = 0;  
  int TargetSize = 0;
  
  QFileInfo* fInfo = new QFileInfo ( ASourse );
  QString a = getFileName(ASourse);
  
  QString destName = ADest;
  //destName.prepend(a);


// check free space
  if ( statfs ( destName, &stat ) < 0 ) {
    ZMessageDlg* dlgw  = new ZMessageDlg ( lang.readEntry("LANGUAGE", "COPYING", ""), 
						 lang.readEntry("LANGUAGE", "ERROR_GET_FREE_SPACE", ""), ZMessageDlg::just_ok, 0 , this , "dlgw" , true , 0 );
    dlgw->exec();
    delete dlgw;
  } else {
    
    destName.append("/" + a);
       
    freeSize = stat.f_bsize*stat.f_bfree;
    QFileInfo* fInfoSource = new QFileInfo ( ASourse );
    if ( fInfoSource->isDir() ) {
      TargetSize = GetFolderSize ( ASourse );
      int cntFiles = GetFilesCountInFolder( ASourse );
      if ( ( freeSize > TargetSize ) && ( TargetSize >= 0 ) ) {
		
        QStringList fullFiles;
        QStringList::Iterator it;
        fullFiles = GetFilesInFolder( ASourse );        
        if (fullFiles.count() > 0)
        {
          it = fullFiles.begin();
                                
          while ( it != fullFiles.end() ) {
            QString b = *it;
            int k = b.findRev(ASourse);
            b.remove ( k, ASourse.length()-k);
            QDir dd;
            QString ss;
            ss = ADest + b;
            int ii = ss.findRev ( "/" );
            ss.remove ( ii+1, ss.length() - ii );
			ss = destName;
            if ( ! dd.exists(ss)) {
              dd.mkdir(ss);
            }
			if ( QFileInfo(destName).isDir() ) { CopyFile ( *it, destName + "/" + b); }
            else { CopyFile ( *it, ADest + b); }
            ++it;
          }
        }
        else { QString b;
          int k = b.findRev(ASourse);
          b.remove ( k, ASourse.length()-k);
		  CopyFile ( ASourse, ADest + b);
          //system(QString("cp %1 %2").arg(ASourse.utf8()).arg(QString(ADest + b).utf8()));}
        }                        
        if ( remove ) {
			QFile f;
			f.remove ( ASourse );
        }                
      }
    } else {
      if ( fInfoSource->isFile() ) {
        TargetSize = fInfoSource->size(); 
      }
    }
    
    if ( ( freeSize > TargetSize ) && ( TargetSize >= 0 ) ) 
    {
      CopyFile ( ASourse, ADest + "/" + a);
      if ( remove ) {
        QFile f;
        f.remove ( ASourse );
      }
    } else {
      ZMessageDlg* dlgw  = new ZMessageDlg ( lang.readEntry("LANGUAGE", "COPYING", ""), 
							lang.readEntry("LANGUAGE", "ERROR_NON_FREE_SPACE", "") , ZMessageDlg::just_ok, 0 , this , "dlgw" , true , 0 );
      dlgw->exec();
      delete dlgw;
    }    
  }
}

bool  ZClipboard::CopyFile ( QString ASourceName, QString ADestName )
{

  if ( QFileInfo(ADestName).isFile() )
  {

  }
  else
  {
	  ZConfig lang(Idioma, false);
	  ZProgressDlg* zp = new ZProgressDlg ( " ", " ", 1, NULL, NULL, true, ( ZSkinService::WidgetClsID ) 4 );
	  ZMeter* zm = zp->getMeter();
	  zm->setMaximumHeight(200);
	  QFileInfo* fInfo = new QFileInfo ( ASourceName );
	  if ( fInfo->isDir() ) { // copy directory
		zp->setTitle ( lang.readEntry("LANGUAGE", "COPYING", "") );
	  } else if ( fInfo->isFile() ) { //copy file
		long size = fInfo->size();
		long size2 = fInfo->size();
		if ( size == 0 ) {
		  delete zp;
		  return FALSE;
		} else {

		  zp->reset();
		  zp->setTitle ( lang.readEntry("LANGUAGE", "COPYING", "") );
		  zp->setInstructText(QString("%1\n").arg(ASourceName));
		                    //zp->setProgressTotalStep ( 0);
		  zm->setTotalSteps(10);
		  zm->setMaximumWidth(230);
		  zm->setFixedWidth(230);

		  QFile fDest ( ADestName );
		  QFile fSource ( ASourceName );
		  int blockSize = 1024;
		  unsigned long i = 0;
		  double u = 0;
		                    
		  zp->show();
		  qApp->processEvents();
		  char* p=NULL;
		  if ( fSource.open ( IO_ReadOnly ) ) {
		    p=new char[blockSize];
		    fDest.open ( IO_WriteOnly );
		    do {
		                                    
		      size = fSource.readBlock ( p, blockSize );
		      if ( size == -1 ) {
		        delete zp;
		        return FALSE;
		      }
		      if (fDest.writeBlock(p, size) == -1) {
		        delete zp;
		        return FALSE;
		      }
		      u = 100 * (double)i / size2 / 10;
		      zm->setProgress((int)u);
		      zp->setInstructText(QString("%1\n").arg(ASourceName) + QString( lang.readEntry("LANGUAGE", "PROGRESS_PERCENT", "") ).arg((int)u*10));
		      qApp->processEvents();
		      i += size;
		    } while (size == blockSize);
		    delete[] p;
		  }
		  
		  qApp->processEvents();
		}
	  }
	  delete zp;
	  return TRUE;
  }
}

int ZClipboard::GetFolderSize (const QString &path )
{
  //recursive
  QDir dir ( path );
  QStringList::Iterator it;
  int size = 0;
  QStringList files = dir.entryList ( "*", QDir::Files );
  it = files.begin();
  while ( it != files.end() ) {
    size += QFileInfo ( path, *it ).size();
    ++it;
  }
  QStringList dirs = dir.entryList ( QDir::Dirs );
  it = dirs.begin();
  while ( it != dirs.end() ) {
    if ( *it != "." && *it != ".." )
      size += GetFolderSize ( path + "/" + *it );
    ++it;
  }
  return size; 
}

int ZClipboard::GetFilesCountInFolder ( const QString &path )
{
  //recursive
  QDir dir ( path );
  QStringList::Iterator it;
  int cnt = 0;
  QStringList files = dir.entryList ( "*", QDir::Files );
  it = files.begin();
  while ( it != files.end() ) {
    cnt++;
    ++it;
  }
  QStringList dirs = dir.entryList ( QDir::Dirs );
  it = dirs.begin();
  while ( it != dirs.end() ) {
    if ( *it != "." && *it != ".." )
      cnt += GetFolderSize ( path + "/" + *it );
    ++it;
  }
  return cnt;  
}

QStringList ZClipboard::GetFilesInFolder(const QString &path)
{
  //recursive
  QDir dir ( path );
  QStringList::Iterator it;
  QStringList fullFiles;
  QStringList files = dir.entryList ( "*", QDir::Files );
  it = files.begin();
  while ( it != files.end() ) {
    fullFiles += QFileInfo ( path, *it ).filePath();
    ++it;
  }
  QStringList dirs = dir.entryList ( QDir::Dirs );
  it = dirs.begin();
  while ( it != dirs.end() ) {
    if ( *it != "." && *it != ".." )
      fullFiles += GetFilesInFolder ( path + "/" + *it );
    ++it;
  }
  return fullFiles;  
}






//************************************************************************************************
//*************************************** class ZSearch ******************************************
//************************************************************************************************
ZSearch::ZSearch()
  :MyBaseDlg()
{
  QString ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma = ProgDir + "/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma).isFile() ) Idioma = ProgDir + "/languages/en-us";
  ZConfig lang (Idioma,false); 

  this->setMainWidgetTitle("zFileManager");
  
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

    lineEdit = new ZLineEdit(this);
    lineEdit->setTitle(lang.readEntry(QString("LANGUAGE"), "SEARCH", ""));
    lineEdit->setTitlePosition(ZLineEdit::TitleTop);
    lineEdit->setMaxLength(50);
	lineEdit->setText("");
    fc->addChild(lineEdit);

 	buttonDir = new myButton(this, "ZPressButton");
	buttonDir->setTitle(lang.readEntry(QString("LANGUAGE"), "SEARCH_IN", ""));
	ZConfig IC(ProgDir + "/settings.cfg", true);

	QString str = IC.readEntry(QString("FM"), "LastFolder", "/");
	int j = str.findRev ( "/" ); str.remove ( 0, j);
	if (str.left(1) = "/") str.remove ( 0, 1);

    buttonDir->setText(str); 
	FStext=IC.readEntry(QString("FM"), "LastFolder", "/");
	buttonDir->setPixmap(QPixmap(ProgDir+"/img/folder.png"));
    buttonDir->setAvailableWidth(240); 
    fc->addChild(buttonDir);
	

  ZSoftKey *softKey = new ZSoftKey ( "CST_2A", this, this );
  softKey->setText ( ZSoftKey::RIGHT, texto(lang.readEntry(QString("LANGUAGE"), "GOBACK", "")), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setText ( ZSoftKey::LEFT, texto(lang.readEntry(QString("LANGUAGE"), "SEARCH", "")), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_cancel() ) );
  softKey->setClickedSlot ( ZSoftKey::LEFT, this, SLOT ( slot_search() ) );


  this->setCSTWidget( softKey );

}


ZSearch::~ZSearch()
{
  
}

void ZSearch::slot_cancel()
{
  reject();
}


void ZSearch::slot_search()
{
  QString buscar = lineEdit->text();
  if ( buscar != "" )
  {
	ZConfig lang (Idioma,false);
	QString Ival1 = lang.readEntry(QString("LANGUAGE"), QString("SEARCHING"), "");
	ZMessageDlg* msg0 =  new ZMessageDlg ( "zFileManager", Ival1, ZMessageDlg::NONE, 200, NULL, "ZMessageDlg", true, 0 );
	QPixmap pixmap; pixmap.load(ProgDir+"/img/search.png");
	msg0->setTitleIcon(pixmap);
	msg0->exec();

	system(QString("find %1 -name \"*%2*\" > /tmp/searchresults").arg(FStext).arg(buscar));
	QString line;
	QFile entrada("/tmp/searchresults");
	QTextStream stentrada(&entrada); 

	delete msg0;
	msg0 = NULL;

	if ( entrada.open (IO_ReadOnly | IO_Translate) )
	{ while ( !stentrada.eof() ) { 
		line += stentrada.readLine(); 
	} } entrada.close();

	if (line!="")
	{
	  ZSearchRes *zf = new ZSearchRes();
	  if ( zf->exec() )
	  delete zf;
	  zf = NULL;
	  reject();
	}
	else
	{
		QString IDval = lang.readEntry(QString("LANGUAGE"), "SEARCH_NORES", "");
		ZNoticeDlg* znd = new ZNoticeDlg(ZNoticeDlg::Success, 3000, QString(""), IDval, this, "z", true, 0);
		znd->setTitle(lang.readEntry(QString("LANGUAGE"), "SEARCH", ""));
		QPixmap pixmap; pixmap.load(ProgDir+"/img/search.png");
		znd->setTitleIcon(pixmap);
		znd->show(); znd = NULL;
	}
  }

}

QString ZSearch::texto(const char*xString)
{
	ZConfig IDini(Idioma, false);	
	QString	IDval = IDini.readEntry(QString("LANGUAGE"), QString("UNICODE"), "");
	if ( IDval == "1" ) { 
		QTextCodec* txtcodec = QTextCodec::codecForName("UTF-8");
		return txtcodec->toUnicode(xString); }
	else { return xString; }
}  




myButton::myButton(QWidget* parent, char const* name, ZSkinService::WidgetClsID clsId)
:ZPressButton( parent, NULL )
{
	
}

myButton::~myButton()
{

}

void myButton::keyPressEvent(QKeyEvent* k)
{
	if ( k->key() == Z6KEY_CENTER )
	{
	  ZFolderSelect2 *zf = new ZFolderSelect2(true, FStext);
	  if ( zf->exec() )
	  {
		QString fDestPath = zf->getFileName(); 
		int i = fDestPath.findRev ( "//" ); fDestPath.remove ( i, 1 );
		 if ( ! fDestPath.isEmpty())
		{
			setText(fDestPath); 
			FStext=fDestPath;
		}
	  }
	  delete zf;
	  zf = NULL;
	}
	else
	{
      ZPressButton::keyPressEvent(k);
	}

}


//************************************************************************************************
//************************************** class ZFolderSelect2 ***********************************
//************************************************************************************************
ZFolderSelect2::ZFolderSelect2(bool onlyDirs, QString path)
  :MyBaseDlg()
{
  QString ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma = ProgDir + "/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma).isFile() ) Idioma = ProgDir + "/languages/en-us";
  ZConfig lang (Idioma,false); 

  this->setMainWidgetTitle("zFileManager");
  
  QWidget *myWidget = new ZWidget ( this, NULL, 0);
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );
  myWidget->setFixedWidth(240);
  ZAppInfoArea* pAIA = new ZAppInfoArea(myWidget, NULL, 0);
  pAIA->setActArea(ZAppInfoArea::no_progress_meter);
  QPixmap pixmap; pixmap.load(ProgDir+"/img/folder.png");
  pAIA->appendIcon(ZAppInfoArea::activity,pixmap,"");
  pAIA->appendText(ZAppInfoArea::activity, lang.readEntry(QString("LANGUAGE"), "SEARCH_IN", ""), "name");
  setAppInfoArea(pAIA);
  myVBoxLayout->insertWidget (0, pAIA );

  fb = new FileBrowser("*", onlyDirs, QString ( "%I%M" ), myWidget, 0);
  fb->setDir(path);
  QObject::connect(fb, SIGNAL(isFilePicked(bool)), SLOT(filePicked(bool)));

  
  fb->ProgramDir = ProgDir;
  setContentWidget ( fb );
  
  ZSoftKey *softKey = new ZSoftKey ( "CST_2A", this, this );
  
  softKey->setText ( ZSoftKey::RIGHT, texto(lang.readEntry(QString("LANGUAGE"), "CANCEL", "")), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setText ( ZSoftKey::LEFT, texto(lang.readEntry(QString("LANGUAGE"), "CHOISE", "")), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_cancel() ) );
  softKey->setClickedSlot ( ZSoftKey::LEFT, this, SLOT ( accept() ) );
   
  this->setCSTWidget( softKey );

}

ZFolderSelect2::~ZFolderSelect2()
{
  //launcher->CreateSoftKeys(NULL);
}

QString ZFolderSelect2::texto(const char*xString)
{
	ZConfig IDini(Idioma, false);	
	QString	IDval = IDini.readEntry(QString("LANGUAGE"), QString("UNICODE"), "");
	if ( IDval == "1" ) { 
		QTextCodec* txtcodec = QTextCodec::codecForName("UTF-8");
		return txtcodec->toUnicode(xString); }
	else { return xString; }
} 

void ZFolderSelect2::accept()
{
  fileName = fb->getFileName();
  basePath = fb->getFilePath();
  if (fileName != "") { MyBaseDlg::accept(); }
  else {
    ZMessageDlg * dlg = new ZMessageDlg ( "Error", QString("Filename is empty"), ZMessageDlg::just_ok, 0, this, "1", true, 0 );
    dlg->exec();
    delete dlg; dlg = NULL;
	
  }
}


void ZFolderSelect2::selectItem(int idx)
{

}

void ZFolderSelect2::slot_cancel()
{
  reject();
}

void ZFolderSelect2::slot_paste()
{
  accept();
}





//************************************************************************************************
//************************************** class ZSearchRes ****************************************
//************************************************************************************************
ZSearchRes::ZSearchRes()
  :MyBaseDlg()
{
  QString ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma = ProgDir + "/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma).isFile() ) Idioma = ProgDir + "/languages/en-us";
  ZConfig lang (Idioma,false); 

  this->setMainWidgetTitle("zFileManager");
  
  QWidget *myWidget = new ZWidget ( this, NULL, 0);
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );
  myWidget->setFixedWidth(240);
  ZAppInfoArea* pAIA = new ZAppInfoArea(myWidget, NULL, 0);
  pAIA->setActArea(ZAppInfoArea::no_progress_meter);
  QPixmap pixmap; pixmap.load(ProgDir+"/img/search.png");
  pAIA->appendIcon(ZAppInfoArea::activity,pixmap,"");
  pAIA->appendText(ZAppInfoArea::activity, lang.readEntry(QString("LANGUAGE"), "SEARCH_RESULT", ""), "name");
  setAppInfoArea(pAIA);
  myVBoxLayout->insertWidget (0, pAIA );

  browser = new ZListBox ( QString ( "%I%M" ), this, 0);
  browser->setFixedWidth ( 240 );
  QPixmap p1;

  CargarFavs();

  connect(browser, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(brClicked()));

  myVBoxLayout->addWidget ( browser, 1 );
  
  setContentWidget ( browser );

  ZSoftKey *softKey = new ZSoftKey ( "CST_2A", this, this );
  softKey->setText ( ZSoftKey::RIGHT, texto(lang.readEntry(QString("LANGUAGE"), "GOBACK", "")), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setText ( ZSoftKey::LEFT, texto(lang.readEntry(QString("LANGUAGE"), "OPEN", "")), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_cancel() ) );
  softKey->setClickedSlot ( ZSoftKey::LEFT, this, SLOT ( brClicked() ) );

  this->setCSTWidget( softKey );

}


ZSearchRes::~ZSearchRes()
{

}

QString ZSearchRes::texto(const char*xString)
{
	ZConfig IDini(Idioma, false);	
	QString	IDval = IDini.readEntry(QString("LANGUAGE"), QString("UNICODE"), "");
	if ( IDval == "1" ) { 
		QTextCodec* txtcodec = QTextCodec::codecForName("UTF-8");
		return txtcodec->toUnicode(xString); }
	else { return xString; }
}  

void ZSearchRes::CargarFavs()
{
  QString ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  ZListBoxItem* Item = new ZListBoxItem ( browser, QString ( "%I%M" ) );
  Item->appendSubItem ( 1, QString ( ".." ), false, NULL );
  browser->insertItem ( Item, 0,true );
  int ItemSize = browser->itemHeight(0)-2 ; if ( ItemSize>20 ) ItemSize=20;
  browser->clear();

  RES_ICON_Reader iconReader;

  ZConfig SFIini(ProgDir + "/sfoldericons.cfg", false);	
  ZConfig SAIini(ProgDir + "/sfileicons.cfg", false);	

  QString destino; ZConfig IC(ProgDir + "/settings.cfg", true);
  destino="/sfoldericons.cfg";

  QFile entrada("/tmp/searchresults");
  QTextStream stentrada(&entrada); 
  if ( entrada.open (IO_ReadOnly | IO_Translate) )
  { while ( !stentrada.eof() ) { 
	QString line = stentrada.readLine(); 
	if ( line.length() > 0 )
	{
		QPixmap p1; QString mval;
		ZListBoxItem* listitem1 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
		QFileInfo* fInfo = new QFileInfo ( line );
		if ( fInfo->isDir() ) {		
			mval = SFIini.readEntry(QString("FOLDERICONS"), line, "");
			if ( mval == "" ) { mval="fm_folder_thb"; }
			p1=QPixmap( iconReader.getIcon(mval, false) );
			if ( p1.height() > 18 ) { QImage image; image = p1.convertToImage(); 
			image = image.smoothScale(18, 18); p1.convertFromImage(image); }  
			listitem1->setPixmap ( 0, p1 );
		} else if ( fInfo->isFile() ) {		
		    QString Eval = line;
			int i = Eval.findRev ( "." ); Eval.remove ( 0, i +1 );
			mval = SAIini.readEntry(QString("FILES"), QString("%1").arg(Eval).lower(), "");
			if ( mval == "" ) { mval="msg_unsupport_type_thb"; }
			p1=QPixmap( iconReader.getIcon(mval, false) );
			if ( p1.height() > 18 ) { QImage image; image = p1.convertToImage(); 
			image = image.smoothScale(18, 18); p1.convertFromImage(image); }  
			listitem1->setPixmap ( 0, p1 );
		} else return;
		//int i = line.findRev ( "/" ); line.remove ( 0, i); if (line.left(1) = "/") line.remove ( 0, 1);
		listitem1->appendSubItem ( 1, line, false, 0 );
		browser->insertItem ( listitem1,-1,true );
	}
  } } entrada.close();
  browser->enableMarquee(ZListBox::LISTITEM_REGION_C, true);

}


void ZSearchRes::accept()
{
//	brClicked();
}



void ZSearchRes::slot_cancel()
{
  reject();
}


void ZSearchRes::brClicked()
{

	QString ProgDir =  QString ( qApp->argv() [0] );
	int i = ProgDir.findRev ( "/" );
	ProgDir.remove ( i+1, ProgDir.length() - i );
	QString name = browser->item( browser->currentItem() )->getSubItemText ( 1, 0, true );

	QString FOLDER = name;
	if ( QFileInfo(name).isFile() ) FOLDER = getFilePath(name);

	launcher->CargarBrowser(FOLDER);

	reject();
}

QString ZSearchRes::getFileName(const QString &AFileName)
{
  QString str = AFileName;
  int i = str.findRev ( "/" );
  str.remove ( 0, i);
  if (str.left(1) = "/") str.remove ( 0, 1);
  return str;    
}

QString ZSearchRes::getFilePath(const QString &AFileName)
{
  QString str = AFileName;
  int i = str.findRev ( "/" );
  str.remove ( i, str.length() - i );
  return str;  
}


void  ZSearchRes::keyPressEvent(QKeyEvent* k)
{
    if (k->key() == Z6KEY_RED )
    {
        qApp->quit();
    }

}
