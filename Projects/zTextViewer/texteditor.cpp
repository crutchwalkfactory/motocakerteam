#include "texteditor.h"

//ezx
#include <ZApplication.h>
#include <qobject.h>
#include <zglobal.h>
#include <ZMessageDlg.h>
#include <ZSingleSelectDlg.h>
#include <ZListBox.h>
#include <ZConfig.h>

//qt
#include <qfile.h>
#include <qfileinfo.h>
#include <qtextstream.h>
#include <qtextcodec.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <qvbox.h>

#include <ZLabel.h>
#include <ZScrollView.h>
#include <ZMainWidget.h>    
#include "resources.h"

#define DELETE_IF_EXIST( value ) if( NULL != value ) { delete value; value = NULL; }		

#define UNINIT_VARIABLE -1

QString IDS_FILE, IDS_SAVE, IDS_SAVEAS, IDS_OPEN, IDS_NEW, IDS_NAVIGATION, IDS_EDIT, IDS_ABOUT, IDS_EXIT, IDS_PAGE_UP, IDS_PAGE_DOWN, IDS_AT_BEGIN, IDS_AT_END, IDS_COPY, IDS_CUT, IDS_PASTE, IDS_SELECT_ALL, IDS_UP, IDS_BLOCK_START, IDS_BLOCK_END, IDS_CANCEL, IDS_FUNCTIONS, IDS_CHOISE, IDS_SAVING, IDS_ENTER_FILE_NAME, IDS_FILE_CHANGED, IDS_REWRITE, IDS_CHANGED, IDS_FILE_SAVING_SUCCESS, IDS_FILE_SAVING_FAILED, IDS_ERROR, IDS_WARNING, IDS_CLOSE_APP, IDS_OPTIONS, IDS_LANGUAGE, IDS_TEXTSIZE, IDS_SIZE, IDS_SEARCH;

TextEditor::TextEditor ( QWidget * pParent, char const *pName )
    : ZKbMainWidget ( ( ZHeader::HEADER_TYPE ) 3, pParent, pName, WType_TopLevel )

{


  m_bSaveAs =  true;
  m_sFileName = QString::null;
  m_sFilePath = QString::null;
  m_sBuffer =   QString::null;
                        
  LoadSettings();
  
  CreateWindow ( pParent );
  show(); 
  

}

TextEditor::~TextEditor()
{
}

void TextEditor::CreateWindow ( QWidget* parent )
{
    
  setMainWidgetTitle ( APP_NAME );
  ZWidget *myWidget = new ZWidget ( this, NULL, 0);
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );
  myWidget->setFixedWidth(880);
  //----------------------------------------------------------------------    
  pAIA = new ZAppInfoArea(myWidget, NULL, 0);
  pAIA ->setInputArea();
  pAIA->setActArea(ZAppInfoArea::no_progress_meter);
  setAppInfoArea(pAIA);
  myVBoxLayout->addWidget ( pAIA, 0 );
  //----------------------------------------------------------------------
  
  m_pScrollPanel = new ZScrollPanel ( myWidget, NULL, 0, ZSkinService::clsZScrollPanel);
  m_pScrollPanel->setFixedWidth(880);
//  m_pScrollPanel->setFixedHeight(DISPLAY_HEIGHT - 35 );
 
  
  m_pTextEdit = new ZMultiLineEditEx ( m_pScrollPanel->viewport(), false, 1, "ZMultiLineEdit" );
  
  //m_pTextEdit->setCellHeight( DEFAULT_EDIT_CELL_HEIGHT.toInt() );
  m_pTextEdit->setFontPercent( FUENTE.toFloat() );
  //QFont f; f.setPixelSize(DEFAULT_EDIT_FONT_SIZE);
  //m_pTextEdit->setFont(f);
  m_pScrollPanel->addChild(m_pTextEdit, 0, 0);

  //our textedit handle center button
  QObject::connect( m_pTextEdit, SIGNAL( clicked() ), SLOT( slot_MarkBegin() ) );
  
  
  m_pSoftKey = new ZSoftKey ( "CST_2" , this , this );

  CreateMenu ( this );
  CreateSoftKeys(this);
  setContentWidget ( myWidget );

  QFile file;
  if ( qApp->argc() > 1 && file.exists ( qApp->argv() [1] ) )
  {
    m_sFileName = QString(qApp->argv()[1]).utf8();
    m_sFilePath = getFilePath(m_sFileName);
    load( m_sFileName );
  }
  else
  {
	if ( qApp->argc() > 1 && file.exists ( qApp->argv() [2] ) )
	{
	  m_sFileName = QString(qApp->argv()[2]).utf8();
	  m_sFilePath = getFilePath(m_sFileName);
	  load( m_sFileName );
	}
	else  {   slot_newFile();  }
  }
  setSoftKey ( m_pSoftKey );

	int y = 0;
	y = y + this->headerSize().height();
	y = y + m_pSoftKey->height();
	y = y + pAIA->height();
	m_pScrollPanel->setFixedHeight(320-y);
	m_pTextEdit->setFixedHeight(320-y);

}
    
// create popup menu
void TextEditor::CreateMenu ( QWidget * pParent )
{
  QRect rect;
  m_pMainMenu = new ZOptionsMenu ( rect, pParent, NULL , 0 , ZSkinService::clsZOptionsMenu);
  m_pFileMenu = new ZOptionsMenu ( rect, pParent, NULL , 0 , ZSkinService::clsZOptionsMenu);
  m_pEditMenu = new ZOptionsMenu ( rect, pParent, NULL , 0 , ZSkinService::clsZOptionsMenu);
  m_pOptionsMenu = new ZOptionsMenu ( rect, pParent, NULL , 0 , ZSkinService::clsZOptionsMenu);
  m_pNavigateMenu = new ZOptionsMenu ( rect, pParent, NULL , 0 , ZSkinService::clsZOptionsMenu);
  
  //***********************************************************************
  m_pMainMenu->setItemSpacing ( 10 );
  
  
  
  QPixmap* pm  = new QPixmap(m_sProgramDir+ IDB_IMAGE_EXIT);
  
  //***********************************************************************
  m_pFileMenu->setItemSpacing ( 10 );
  pm->load(m_sProgramDir+ IDB_IMAGE_NEW);
  m_pFileMenu->insertItem ( QString( IDS_NEW ), NULL, pm , true , 0 , 0 );
  pm->load(m_sProgramDir+ IDB_IMAGE_OPEN);
  m_pFileMenu->insertItem ( QString( IDS_OPEN ), NULL, pm , true , 1 , 1 );
  pm->load(m_sProgramDir+ IDB_IMAGE_SAVE);
  m_pFileMenu->insertItem ( QString( IDS_SAVE ), NULL, pm , true , 2 , 2 );
  pm->load(m_sProgramDir+ IDB_IMAGE_SAVEAS);
  m_pFileMenu->insertItem ( QString( IDS_SAVEAS ), NULL, pm , true , 3 , 3 );
  
  //------------------------------------------------------------------------
  m_pFileMenu->connectItem ( 2, pParent, SLOT ( slot_saveFile() ) );
  m_pFileMenu->connectItem ( 3, pParent, SLOT ( slot_saveFileAs() ) );
  m_pFileMenu->connectItem ( 1, pParent, SLOT ( slot_openFile() ) );
  m_pFileMenu->connectItem ( 0, pParent, SLOT ( slot_newFile() ) );


  //***********************************************************************
  m_pOptionsMenu->setItemSpacing ( 10 );
  //pm->load(m_sProgramDir+ IDB_IMAGE_LANGUAGE);
  //m_pOptionsMenu->insertItem ( QString( IDS_LANGUAGE ), NULL, pm , true , 0 , 0 );
  pm->load(m_sProgramDir+ IDB_IMAGE_TEXTSIZE);
  m_pOptionsMenu->insertItem ( QString( IDS_TEXTSIZE ), NULL, pm , true , 1 , 1 );
  //
  //------------------------------------------------------------------------
  //m_pOptionsMenu->connectItem ( 0, pParent, SLOT ( slot_language() ) );
  m_pOptionsMenu->connectItem ( 1, pParent, SLOT ( slot_textSize() ) );

  //***********************************************************************
  m_pNavigateMenu->setItemSpacing ( 10 );
  pm->load(m_sProgramDir+ IDB_IMAGE_ARROW_UP);
  m_pNavigateMenu->insertItem ( QString( IDS_PAGE_UP ), NULL , pm, true , 0 , 0 );
  pm->load(m_sProgramDir+ IDB_IMAGE_ARROW_DOWN);
  m_pNavigateMenu->insertItem ( QString( IDS_PAGE_DOWN ), NULL, pm, true , 1 , 1 );
  pm->load(m_sProgramDir+ IDB_IMAGE_HOME);
  m_pNavigateMenu->insertItem ( QString( IDS_AT_BEGIN ), NULL, pm , true , 2 , 2 );
  pm->load(m_sProgramDir+ IDB_IMAGE_END);
  m_pNavigateMenu->insertItem ( QString( IDS_AT_END ), NULL, pm , true , 3 , 3 );
  //------------------------------------------------------------------------
  m_pNavigateMenu->connectItem ( 0, pParent, SLOT ( slot_PageUp() ) );
  m_pNavigateMenu->connectItem ( 1, pParent, SLOT ( slot_PageDown() ) );
  m_pNavigateMenu->connectItem ( 2, pParent, SLOT ( slot_Home() ) );
  m_pNavigateMenu->connectItem ( 3, pParent, SLOT ( slot_End() ) );

  //***********************************************************************
  m_pEditMenu->setItemSpacing ( 10 );
  pm->load(m_sProgramDir+ IDB_IMAGE_COPY);
  m_pEditMenu->insertItem ( QString( IDS_COPY ), NULL , pm, true , 0 , 0 );
  pm->load(m_sProgramDir+ IDB_IMAGE_CUT);
  m_pEditMenu->insertItem ( QString( IDS_CUT ), NULL, pm , true , 1 , 1 );
  pm->load(m_sProgramDir+ IDB_IMAGE_PASTE);
  m_pEditMenu->insertItem ( QString( IDS_PASTE ), NULL, pm , true , 2 , 2 );
  pm->load(m_sProgramDir+ IDB_IMAGE_SELALL);
  m_pEditMenu->insertItem ( QString( IDS_SELECT_ALL ), NULL, pm , true , 3 , 3 );
  //m_pEditMenu->insertSeparator ( 4, 4 );
  //pm->load(m_sProgramDir+ IDB_IMAGE_SEARCH);
  //m_pEditMenu->insertItem ( QString( IDS_SEARCH ), NULL, pm , true , 5 , 5 );

//  pm->load(m_sProgramDir+ IDB_IMAGE_BLOCKBEGIN);
//  m_pEditMenu->insertItem ( QString( IDS_BLOCK_START ), NULL, pm , true , 4 , 4 );
//  m_pEditMenu->insertSeparator ( 4, 4 );
//  pm->load(m_sProgramDir+ IDB_IMAGE_BLOCKEND);
//  m_pEditMenu->insertItem ( QString( IDS_BLOCK_END ), NULL, pm , true , 5 , 5 );
  //------------------------------------------------------------------------
  m_pEditMenu->connectItem ( 0, pParent, SLOT ( slot_Copy() ) );
  m_pEditMenu->connectItem ( 1, pParent, SLOT ( slot_Cut() ) );
  m_pEditMenu->connectItem ( 2, pParent, SLOT ( slot_Paste() ) );
  m_pEditMenu->connectItem ( 3, pParent, SLOT ( slot_selectAll() ) );
//  m_pEditMenu->connectItem ( 4, pParent, SLOT ( slot_MarkBegin() ) );
//  m_pEditMenu->connectItem ( 5, pParent, SLOT ( slot_MarkEnd() ) );
  //m_pEditMenu->connectItem ( 5, pParent, SLOT ( slot_search() ) );
  
  pm->load(m_sProgramDir+ IDB_IMAGE_FILE);
  m_pMainMenu->insertItem ( QString( IDS_FILE ), m_pFileMenu, pm , true , 0 , 0 );
  pm->load(m_sProgramDir+ IDB_IMAGE_NAVIGATE);
  m_pMainMenu->insertItem ( QString( IDS_NAVIGATION ), m_pNavigateMenu, pm , true , 1 , 1 );  
  pm->load(m_sProgramDir+ IDB_IMAGE_EDIT);
  m_pMainMenu->insertItem ( QString( IDS_EDIT  ) , m_pEditMenu, pm , true , 2 , 2 );  
  pm->load(m_sProgramDir+ IDB_IMAGE_OPTIONS);
  m_pMainMenu->insertItem ( QString( IDS_OPTIONS ), m_pOptionsMenu, pm , true , 3 , 3 );  
  pm->load(m_sProgramDir+ IDB_IMAGE_ABOUT);
  m_pMainMenu->insertItem ( QString( IDS_ABOUT ), NULL, pm , true , 4 , 4 );  
  pm->load(m_sProgramDir+ IDB_IMAGE_EXIT);
  m_pMainMenu->insertItem ( QString( IDS_EXIT ), NULL , pm, true , 5 , 5 );
  //------------------------------------------------------------------------
  m_pMainMenu->connectItem ( 5, qApp, SLOT ( quit() ) );
  m_pMainMenu->connectItem ( 4, pParent, SLOT ( slot_about() ) );  
}


void TextEditor::CreateSoftKeys(QWidget * pParent)
{

	m_pSoftKey->setTextForOptMenuHide ( QString( IDS_FUNCTIONS ) );
	m_pSoftKey->setTextForOptMenuShow ( QString( IDS_CHOISE ), QString( IDS_CANCEL ) );
  
	m_pSoftKey->setText ( ZSoftKey::LEFT, QString( IDS_FUNCTIONS ), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  m_pSoftKey->setText ( ZSoftKey::RIGHT, QString( IDS_CHOISE ), ( ZSoftKey::TEXT_PRIORITY ) 0 );
	m_pSoftKey->setOptMenu ( ZSoftKey::RIGHT, m_pEditMenu );
  m_pSoftKey->setOptMenu ( ZSoftKey::LEFT, m_pMainMenu );
	

}


void TextEditor::load ( const QString& FileName )
{
	QString extension = FileName;
	int i = extension.findRev ( "." ); extension.remove ( 0, i +1 );
	QString eten = QString("%1").arg(extension).lower();

	QString filetoopen = FileName;

	if ((eten=="docx"))
	{
		system("rm -r /tmp/word");
		system(QString("/ezxlocal/LinXtend/bin/unzipmod \"%1\" word/document.xml -d /tmp").arg(FileName));
		system("sed -e 's/<w:br[^>]*>/saltodelineabycepi/g' /tmp/word/document.xml > /tmp/document");
		system("sed -i 's/<\\\/w:p>/saltodelineabycepi/g' /tmp/document");
		system("sed -i 's/<[^>]*>//g' /tmp/document");
		system("rm -r /tmp/word");
		filetoopen = "/tmp/document";
	}
	else if (eten=="odt")
	{
		system("rm -r /tmp/word");
		system(QString("/ezxlocal/LinXtend/bin/unzipmod \"%1\" content.xml -d /tmp/word").arg(FileName));
		system("sed -e 's/<text:p[^>]*>/saltodelineabycepi/g' /tmp/word/content.xml > /tmp/document");
		system("sed -i 's/<[^>]*>//g' /tmp/document");
		system("rm -r /tmp/word");
		filetoopen = "/tmp/document";
	}


	QFile file ( filetoopen );
	if (  file.open ( IO_ReadOnly ) ) { qDebug("**** FAIL: file is a readonly"); }
	else { qDebug("**** PASS: file is not a readonly"); }
	m_pTextEdit->clear();
	m_pTextEdit->blockSignals(true);
	m_pTextEdit->setAutoUpdate ( FALSE );
	QTextStream stream ( &file );
	stream.setEncoding ( QTextStream::UnicodeUTF8 );

	if ((eten=="docx")||(eten=="odt"))
	{
		QString textox = stream.read();
		textox.replace( QRegExp("saltodelineabycepi"), QString("\n") );


		QString autorun = QString(getProgramDir() + "/replace.cfg");
		ZConfig rex ( autorun, false );
		QString line=""; QFile entrada(autorun); QTextStream stentrada(&entrada);
		if ( entrada.open(IO_ReadOnly | IO_Translate) )
		{
			while ( !stentrada.eof() )
			{
				line = stentrada.readLine();
				if ( (line!="") & (line!="[Replace]") )
				{
					int encontro = line.find( "=" , 0 );
					if ( encontro > -1 ) { line.remove( encontro, line.length() - encontro ); }
					encontro = line.find( " " , 0 );
					if ( encontro > -1 ) { line.remove( encontro, line.length() - encontro ); }
					QString remp1 = rex.readEntry("Replace", line, "");
					system(QString("echo \"REEMPLAZANDO -%1- POR -%2- \"").arg(line).arg(remp1));
					textox.replace( QRegExp(QString(line)), QString(remp1) );
				}
			}
		}
		entrada.close();

		//textox.replace( QRegExp("&apos;"), "'" );
		m_pTextEdit->setText(textox);
		pAIA->removeArea(ZAppInfoArea::activity);
		pAIA->appendText(ZAppInfoArea::activity, getFileName(m_sFileName), "name");
		m_sFileName = "";
	}
	else
	{
		m_pTextEdit->setText ( stream.read() );
		pAIA->removeArea(ZAppInfoArea::activity);
		pAIA->appendText(ZAppInfoArea::activity, getFileName(m_sFileName), "name");
	}

	file.close();
	m_pTextEdit->blockSignals(false);
	m_pTextEdit->setAutoUpdate ( TRUE );
	m_pTextEdit->setEdited ( false );

}


void TextEditor::slot_language()
{
  QString val; ZConfig lang(getProgramDir() + "%/settings.cfg", true);
  val = lang.readEntry(QString("General"), QString("LANGUAGE"), "");

  ZSingleSelectDlg* eq1 =  new ZSingleSelectDlg(IDS_LANGUAGE, "", NULL, "ZSingleSelectDlg", true, 0, 0);
  system(QString("ls %1/languages/ > %2/ltemp").arg(getProgramDir()).arg(getProgramDir()));	

  QString card = QString("%1/ltemp").arg(getProgramDir());
  QString line="";
  int cantl=0;  
  QStringList list;
  QFile entrada(card);
  QTextStream stentrada(&entrada);
  QString aux="";
  cantl=0;
  if ( entrada.open(IO_ReadOnly | IO_Translate) )
  {
	while ( !stentrada.eof() )
    {           
          line = stentrada.readLine();
		  ZConfig IDini(QString("%1/languages/%2").arg(getProgramDir()).arg(line), false);
		  QString val = IDini.readEntry(QString("TEXTVIEWER"), QString("LANG_NAME"), "");
          cantl++;
          aux=val;
		  list +=val;
    }
  }
  entrada.close();

  eq1->addItemsList(list);
  ZListBox* listboxx = eq1->getListBox();
  int checked = -1;
  if (checked >= 0)
 	listboxx->checkItem(checked, true);
	eq1->exec();
	int result = eq1->result();
	if (result != 1) {
		result = 0;
	} else {
		result = eq1->getCheckedItemIndex();

		QString card2 = QString("%1/ltemp").arg(getProgramDir());
		QString line2="";
		int cantl2=0;  
		QStringList list2;
		QFile entrada2(card2);
		QTextStream stentrada2(&entrada2);
		QString aux2="";
		cantl2=0;
		if ( entrada2.open(IO_ReadOnly | IO_Translate) )
		{
		while ( !stentrada2.eof() )
		{           
		  line2 = stentrada2.readLine();
		  if ( result == cantl2 )
		  {
			Idioma = getProgramDir() + "/languages/" + line2;
			
			ZConfig ini( QString("%1/settings.cfg").arg(getProgramDir()), true );
			ini.writeEntry(QString("General"), QString("LANGUAGE"), line2);
			qApp->processEvents();

			LoadSettings();
			
			CreateMenu(this);
			CreateSoftKeys(this);

		  }
		  cantl2++;
		  aux2=line2;
		  list2 +=aux2;
		}
		entrada.close();		
	}
  }
  system(QString("rm %1/ltemp").arg(getProgramDir()));
}

void TextEditor::slot_textSize()
{
    ZConfig IDini(QString("%1/settings.cfg").arg(getProgramDir()), true);
	int val = IDini.readNumEntry(QString("General"), QString("FONT_SIZE"), 10);
	textDialog = new ZNumPickerDlg(10, this, "ZNumPickerDlg", 0);
	ZNumModule* module = textDialog->getNumModule();
	module->setMaxValue(24);
	module->setMinValue(6);
	module->setValue(val);
	ZLabel* label = new ZLabel(IDS_SIZE, module, NULL,0, (ZSkinService::WidgetClsID)4);
		module->setLabel(label, (ZNumModule::RelativeAlignment)0);
	textDialog->setTitle(IDS_TEXTSIZE);
	if (textDialog->exec())
	{	
		int r = textDialog->getNumValue();
		ZConfig ini(QString("%1/settings.cfg").arg(getProgramDir()), true);
		ini.writeEntry(QString("General"), QString("FONT_SIZE"), r);
		LoadSettings();
		m_pTextEdit->setFontPercent( FUENTE.toFloat() );
		qApp->processEvents();
	}
}

void TextEditor::slot_about()
{
	QString sHeader = QString( IDS_ABOUT );
	QString sBody = QString( APP_ABOUT_TEXT );
  
  ZAboutDialog* aboutDlg = new ZAboutDialog();
  aboutDlg->exec();
  delete aboutDlg;
}



void TextEditor::slot_openFile()
{
	if ( m_pTextEdit->edited() )
	{
		QString sHeader = QString( IDS_SAVING );
		QString sBody = QString( IDS_FILE_CHANGED );
		ZMessageDlg * dlg = new ZMessageDlg ( sHeader, sBody, ( ZMessageDlg::MessageDlgType ) 1, 0, this, "1", true, 0 );
		if ( dlg->exec() ) {	slot_saveFile(); }
		delete dlg;
	}
  // open file dialog
  QString fName = "";
  ZFileOpenDialog *zfod  = new ZFileOpenDialog();
  zfod->exec();
  if ( zfod->result() == 1 )
  {
    fName = zfod->getFileName();
    m_sFileName = fName;
    m_sFilePath = getFilePath(fName);
    qDebug(QString("*** Load file: %1").arg(fName));
    if (fName != "")  {  load(fName);   }   
  }
  
  delete zfod;
  zfod = NULL;
   
}


void TextEditor::slot_newFile()
{
  if ( m_pTextEdit->edited() )
  {
    QString sHeader = QString( IDS_SAVING );
    QString sBody = QString( IDS_FILE_CHANGED );
    
    ZMessageDlg * pDlg = new ZMessageDlg ( sHeader, sBody, ( ZMessageDlg::MessageDlgType ) 1, 0, this, "1", true, 0 );
    if ( pDlg->exec() )
    {  
      if ( m_sFileName.isEmpty())
      {
      slot_saveFile(); 
      }
      else
      {
        saveAs(m_sFileName);
      }
    }
    delete pDlg;
  }

  m_sFileName = QString::null;
  m_sFilePath = QString::null;
  m_pTextEdit->clear();
  m_pTextEdit->setEdited ( false );
}


void TextEditor::slot_saveFile()
{
  if ( m_sFileName.isEmpty() )
  {
    slot_saveFileAs();
  }
  else
  {
    m_bSaveAs = false;
    saveAs ( m_sFileName );
  }
}

void TextEditor::slot_saveFileAs()
{
   QString fName = getFileName(m_sFileName);
  
  // get current path, if file was opened
  // fill editbox with origin filename
  qDebug(QString("*** FilePath: %1\n*** FileName: %2").arg(m_sFilePath).arg(fName));
  
/*  ZFileSaveDialog *zfsd  = new ZFileSaveDialog(m_sFilePath, fName);
  zfsd->exec();
  if ( zfsd->result() == 1 )
  {
    fName = zfsd->getFileName();
    qDebug(QString("*** Save file: %1").arg(fName));
    if (fName != "")  {  saveAs(fName);   }   
  }
  
  delete zfsd;
  zfsd = NULL;  */

  ZSearch *zfsd  = new ZSearch(m_sFilePath, fName);
  zfsd->exec();
  if ( zfsd->result() == 1 )
  {
    fName = zfsd->getFilePath() + "/" + zfsd->getFileName();
    qDebug(QString("*** Save file: %1").arg(fName));
    if (fName != "")  {  saveAs(fName);   }   
  }
  
  delete zfsd;
  zfsd = NULL; 


}

void TextEditor::saveAs ( const QString& sFileName )
{
  QFile file ( sFileName );
  
  if ( file.exists() && m_bSaveAs )
  {
    QString sHeader = QString( IDS_SAVING );
    QString sBody = QString( IDS_REWRITE ).arg ( sFileName );

    ZMessageDlg * pConfirmation = new ZMessageDlg ( sHeader, sBody, ( ZMessageDlg::MessageDlgType ) 1, 0, this, "2", true, 0 );
    if ( pConfirmation->exec() )
    {
      ZMessageDlg * pMessage = new ZMessageDlg ( sHeader, QString( IDS_CHANGED ), ( ZMessageDlg::MessageDlgType ) 1, 0, this, /*where do we use it?*/"saveas", true, 0 );
      pMessage->show();
      delete pMessage;
    }
    delete pConfirmation;
  }

  if ( !file.open ( IO_WriteOnly ) )
  {
    QString sHeader = QString( IDS_ERROR );
    QString sBody = QString( IDS_FILE_SAVING_FAILED );
    
    ZMessageDlg * pWarning = new ZMessageDlg ( sHeader, sBody, ( ZMessageDlg::MessageDlgType ) 2, 0, this, "saveas", true, 0 );
    pWarning->show();
    delete pWarning;
  }

  QTextStream stream ( &file );
  stream.setEncoding ( QTextStream::UnicodeUTF8 );
  stream << m_pTextEdit->text();
  file.close();
  
  m_pTextEdit->setEdited ( false );
  m_bSaveAs = true;
  
  QString sHeader = QString( IDS_SAVING );
  QString sBody = QString( IDS_FILE_SAVING_SUCCESS );
  ZMessageDlg * pMessage = new ZMessageDlg ( sHeader, sBody, ( ZMessageDlg::MessageDlgType ) 2, 0, this, "saveas", true, 0 );
  pMessage->exec();
  delete pMessage;

}

void TextEditor::slot_PageUp()
{
  m_pTextEdit->pageUp();
}

void TextEditor::slot_PageDown()
{
  m_pTextEdit->pageDown();
}

void TextEditor::slot_Home()
{
  m_pTextEdit->atBegin();
}

void TextEditor::slot_End()
{
  m_pTextEdit->atEnd();
}

void TextEditor::slot_selectAll()
{
  m_pTextEdit->selectAll();
}

void TextEditor::slot_MarkBegin()
{	
  m_pTextEdit->MarkBegin();
}

void TextEditor::slot_MarkEnd()
{
  m_pTextEdit->MarkEnd();
}

void TextEditor::slot_Copy()
{
  m_pTextEdit->BlockCopy();
}

void TextEditor::slot_Paste()
{
  m_pTextEdit->BlockPaste();
}

void TextEditor::slot_Cut()
{
  m_pTextEdit->BlockCut();
}


QString TextEditor::getProgramDir()
{
  m_sProgramDir = QString ( qApp->argv() [0] ) ;
  int i = m_sProgramDir.findRev ( "/" );
  m_sProgramDir.remove ( i+1, m_sProgramDir.length() - i );
  return m_sProgramDir;
}

QString TextEditor::getFilePath(const QString &AFileName)
{
  QString str = AFileName;
  int i = str.findRev ( "/" );
  str.remove ( i+1, str.length() - i );
  return str;  
}

QString TextEditor::getFileName(const QString &AFileName)
{
  QString str = AFileName;
  int i = str.findRev ( "/" );
  str.remove ( 0, i);
  if (str.left(1) = "/") str.remove ( 0, 1);
  return str;    
}

void TextEditor::LoadSettings()
{
  ZConfig cfg(getProgramDir() + "/settings.cfg", true);
  DEFAULT_EDIT_FONT_SIZE = cfg.readNumEntry(QString("General"), QString("FONT_SIZE"), 10);
  DEFAULT_EDIT_FONT_SIZE = DEFAULT_EDIT_FONT_SIZE - 2;
  if ( DEFAULT_EDIT_FONT_SIZE < 10 ) { FUENTE = QString("0") + QString("%1").arg(DEFAULT_EDIT_FONT_SIZE); }
  else { FUENTE = QString("%1").arg(DEFAULT_EDIT_FONT_SIZE); }
  FUENTE.insert(1, QChar('.') );

  DEFAULT_EDIT_CELL_HEIGHT = cfg.readEntry(QString("General"), QString("CELL_HEIGHT"), QString("22"));

  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  QString Idioma = getProgramDir() + "/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma).isFile() ) Idioma = getProgramDir() + "/languages/en-us";

  ZConfig cfg2(Idioma, true);

  IDS_FILE = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_FILE"), QString("File"));
  IDS_SAVE = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_SAVE"), QString("Save"));
  IDS_SAVEAS = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_SAVEAS"), QString("Save as..."));
  IDS_OPEN = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_OPEN"), QString("Open..."));
  IDS_NEW = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_NEW"), QString("New"));
  IDS_NAVIGATION = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_NAVIGATION"), QString("Navigation"));
  IDS_EDIT = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_EDIT"), QString("Edit"));
  IDS_ABOUT = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_ABOUT"), QString("About..."));
  IDS_EXIT = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_EXIT"), QString("Exit"));
  IDS_PAGE_UP = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_PAGE_UP"), QString("Page up"));
  IDS_PAGE_DOWN = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_PAGE_DOWN"), QString("Page down"));
  IDS_AT_BEGIN = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_AT_BEGIN"), QString("At begin"));
  IDS_AT_END = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_AT_END"), QString("At end"));
  IDS_COPY = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_COPY"), QString("Copy"));
  IDS_CUT = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_CUT"), QString("Cut"));
  IDS_PASTE = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_PASTE"), QString("Paste"));
  IDS_SELECT_ALL = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_SELECT_ALL"), QString("Select all"));
  IDS_UP = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_UP"), QString("Up"));
  IDS_BLOCK_START = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_BLOCK_START"), QString("Block start"));
  IDS_BLOCK_END = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_BLOCK_END"), QString("Block end"));
  IDS_CANCEL = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_CANCEL"), QString("Cancel"));
  IDS_FUNCTIONS = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_FUNCTIONS"), QString("Options"));
  IDS_CHOISE = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_CHOISE"), QString("Select"));
  IDS_SAVING = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_SAVING"), QString("Saving..."));
  IDS_ENTER_FILE_NAME = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_ENTER_FILE_NAME"), QString("Enter filename"));
  IDS_FILE_CHANGED = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_FILE_CHANGED"), QString("File has been changed. Save changes?"));
  IDS_REWRITE = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_REWRITE"), QString("Overwrite file %1?"));
  IDS_CHANGED = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_CHANGED"), QString("Changed"));
  IDS_FILE_SAVING_SUCCESS = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_FILE_SAVING_SUCCESS"), QString("File saved successfully"));
  IDS_FILE_SAVING_FAILED = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_FILE_SAVING_FAILED"), QString("Could not save file"));
  IDS_ERROR = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_ERROR"), QString("Error"));
  IDS_WARNING = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_WARNING"), QString("Warning"));
  IDS_CLOSE_APP = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_CLOSE_APP"), QString("Close app?"));
  IDS_OPTIONS = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_OPTIONS"), QString("Options"));
  IDS_LANGUAGE = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_LANGUAGE"), QString("Language"));
  IDS_TEXTSIZE = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_TEXTSIZE"), QString("Text size"));
  IDS_SIZE = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_SIZE"), QString("Very small"));
  IDS_SEARCH = cfg2.readEntry(QString("TEXTVIEWER"), QString("IDS_SEARCH"), QString("Search"));

}



void TextEditor::slot_search()
{
  ZSearch *zfod = new ZSearch();
  zfod->exec();
  delete zfod;
  zfod = NULL;
}


