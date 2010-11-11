#ifndef ZMENU_H
#define ZMENU_H

#include "GUI_Define.h"
#include "BaseDlg.h"
#include "myam.h"

#include <ZMultiLineEdit.h>
#include <ZMessageDlg.h>
#include <ZListBox.h>
#include <ZIconView.h>
#include <ZImage.h>
#include <ZConfig.h>
#include <ZApplication.h>
#include <ZMessageDlg.h>
#include <ZNoticeDlg.h>
#include <ZSingleSelectDlg.h>
#include <ZOptionsMenu.h>

#include <qtextstream.h>
#include <qobject.h>
#include <qfile.h>
#include <qdir.h>
#include <qmap.h>
#include <qlist.h>
#include <qstringlist.h>
#include <qdir.h>
#include <qfile.h>

#include <sys/vfs.h>
#include <stdio.h>
#include <stdlib.h>


#define Z6KEY_LEFT              0x1012
#define Z6KEY_UP                0x1013
#define Z6KEY_RIGHT             0x1014
#define Z6KEY_DOWN              0x1015
#define Z6KEY_CENTER            0x1004


class MyMenu : public MyBaseDlg {
  Q_OBJECT

  public:
    MyMenu(QString location); 
    ~MyMenu();
    QString getFileName(const QString &AFileName);
	QString getFilePath(QString &AFileName);
	QString texto(const char*xString);    
	QString resultado() { return fileName; } 
    AM_RegistryManager* rman;
    ZListBox* zllb;
    ZListBox* zllb2;
	QString Prog1, Prog2, Prog3, Prog4, Prog5, Prog6, Prog7, Prog8;
	QString Exec1, Exec2, Exec3, Exec4, Exec5, Exec6, Exec7, Exec8;
	QString Acc1, Acc2, Acc3, Acc4, Acc5, Acc6, Acc7, Acc8;
	QPixmap Icon1, Icon2, Icon3, Icon4, Icon5, Icon6, Icon7, Icon8;
	QPixmap fizqi, fderi;
	 
  public slots:   
    void slot_cancel();
	void mClicked();
    void timerTick();

    void ChangeLang();
    void ChangeIcon();
    void RecargarIconos();
	void CargarProgramas();
	void CargarData(QString carpeta);
  
  private:
    QString fileName;
    QString basePath;      
	ZScrollPanel *Panel;
	ZImage* IconoG;
	ZImage* preIcono;
	ZLabel* eti1;
	ZImage* Icono1;
	ZImage* Icono2;
	ZImage* Icono3;
	ZImage* Icono4;
	ZImage* Icono5;
	ZImage* Icono6;
	ZImage* Icono7;
	ZImage* Icono8;
	ZImage* fizq;
	ZImage* fder;
    ZSoftKey *softKey;
	QTimer *timer;

  protected:
	virtual void keyPressEvent(QKeyEvent* k);
	//virtual void keyReleaseEvent(QKeyEvent* k);
    virtual void accept(); 

};


#endif

