#ifndef ZLAUNCHER_H
#define ZLAUNCHER_H

#include "ZFileDlg.h"
#include "ZAboutDlg.h"
#include "ZConfig.h"
#include "myPopup.h"

#include <ZKbMainWidget.h>
#include <ZSoftKey.h>
#include <ZOptionsMenu.h>
#include <ZMessageDlg.h>
#include <ZAppInfoArea.h>
#include <ZProgressDlg.h>
#include <ZSingleCaptureDlg.h>
#include <ZLineEdit.h>
#include <ZNumPickerDlg.h>
#include <ZFormContainer.h>
#include <ZListBox.h>
#include <ZIconView.h>

#include <qdir.h>
#include <sys/vfs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <qobject.h>

class ZLauncher: public ZKbMainWidget
{
	Q_OBJECT
public:    
  ZLauncher(const QString &Argument = QString::null, QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~ZLauncher();
	        
  void CreateWindow(QWidget* parent);
  void LoadSettings();
  bool PasteFiles(const QString &ASourse, const QString &ADest, bool remove = false);
  int  GetFolderSize ( const QString &path );
  int  GetFilesCountInFolder ( const QString &path );
  QStringList GetFilesInFolder(const QString &path);
  bool  CopyFile ( QString ASourceName, QString ADestName );
  bool  DeleteFile ( QString AFileName );
  ZAppInfoArea *pAIA;
  
  QString sPath;
  QString Idioma;
  QString Accion;
  QString getProgramDir();
  QString getIMEI();
  QString getFilePath(const QString &AFileName);
  QString getFileName(const QString &AFileName);

  QString RealName(const QString &AFileName);
  
  ZListBox* settings;
  myListBox* browser;
  ZIconView* Bbrowser;
 
  QString fileName;
  QString filePath;
 
  QString texto(const char*xString);



public slots:
  void checkClip();
  void checkPlayer();
  void slot_about();
  
  void CargarBrowser(QString direccion);
  void Atras();
  void slot_copy();
  void slot_clipboard();
  void slot_paste();
  void slot_delete();
  void slot_move();
  void slot_rename();
  void slot_properties();
  void slot_attrs();
  void slot_makedir();
  
  void CreateSubMenu();
  void CreateMenu();
  void CreateSoftKeys();

  void slot_personalizeName();

  void slot_personalize();

  void Open();   
  void OpenWith();   
  void openText();

  void AdminFav();
  void AgregarFav();   

  void Buscar();
  void Settings();

  void prevPlay();   
  void prevStop();   
  void brClicked();   

  void sendRarFile();
  void sendtoDevice();   
  void sendMenuLink();

  void applyRingtone();   
  void applyWallpaper();   


  void verMiniaturas();   

  void checkearMenu();

private:    

	ZSoftKey *softKey;    
	ZOptionsMenu * menuMain;
	ZOptionsMenu * menuControl;
	ZOptionsMenu * menuPersonalize;
	ZOptionsMenu * menuFavs;
	ZOptionsMenu * menuSend;
	ZOptionsMenu * menuApply;
	ZFormContainer *FPersonalize;

  QString ProgramDir;
  
  protected:
//    bool  eventFilter( QObject *, QEvent * );  
	virtual void keyPressEvent(QKeyEvent* k);


};


#endif //
