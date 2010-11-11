#ifndef ZGUI_H
#define ZGUI_H

#include "myPopup.h"

#include <qobject.h>
#include <unistd.h>
#include <stdlib.h>

#include <ZMessageDlg.h>
#include <ZKbMainWidget.h>
#include <ZListBox.h>
#include <ZScrollPanel.h>
#include <ZAppInfoArea.h>
#include <ZOptionsMenu.h>

class ZGui : public ZKbMainWidget
{
    Q_OBJECT
  public:
    ZGui (QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~ZGui();
    QString getProgramDir();
    void CreateWindow(QWidget* parent);
	ZAppInfoArea *pAIA;
	myListBox* browser;
    void Create_MenuApp();
	int ordenando;   
    
  public slots:
	void slot_quit();   
	void brClicked();   
	void CargarBrowser();   
	void createnote();
	void editnote();
	void marcar();
	void deletenote();
	void sendtomsg();
	void checkNote();
	void Reordenar();

  private:
	ZSoftKey *softKey;    
	ZOptionsMenu * menu1;
	ZOptionsMenu * menu2;
	ZOptionsMenu * menusend;
    QString ProgDir;
    ZScrollPanel *scrollPanel;
	QPixmap tPix;

  protected:
	//virtual void keyPressEvent(QKeyEvent* k);


};

#endif

