#ifndef Z6MANAGER_H
#define Z6MANAGER_H

#include "ZFileDlg.h"

#include <qobject.h>
#include <unistd.h>
#include <stdlib.h>
#include <qfile.h>

#include <ZMessageDlg.h>
#include <ZKbMainWidget.h>
#include <ZListBox.h>
#include <ZScrollPanel.h>
#include <ZNumPickerDlg.h>
#include <ZSingleCaptureDlg.h>
#include <ZSingleSelectDlg.h>
#include <ZMultiSelectDlg.h>
#include <ZLabel.h>
#include <ZMessageDlg.h>
#include <ZNoticeDlg.h>
#include <ZSoftKey.h>
#include <ZNavTabWidget.h>


//using namespace std;

class Z6Man : public ZKbMainWidget
{
    Q_OBJECT
  public:
    Z6Man (const QString &Argument = QString::null, QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~Z6Man();
    QString getProgramDir();
    QString checkNorris();
    QString texto(const char*xString);
    QString getFileName(const QString &AFileName);
    QString ClaveCorrecta;
    void CreateWindow(QWidget* parent);
    QString ProgDir;
    QString Idioma;
    QWidget* dlg;
    ZSingleCaptureDlg* dlg_bt_settings;
    ZNumPickerDlg* dlg_bt_discover;
    ZNoticeDlg* znd;
    ZMultiSelectDlg* equalizer;
    ZSingleSelectDlg* energy1;
    ZListBox* appListBox;
    ZListBox* modListBox;

    void Create_SoftKey();
    void Create_MenuApp();

  signals:
    void picked(const QString &sFileName);
    void isFilePicked(bool);
    
  public slots:
    void about();
    void CargarListaApps();
    void BT_Discover();
    void Time_Alarm();
    void ID_Contacts();
    void SMS_Empty();
    void Sidebar();
    void Slider();
    void Equalize();
    void SelectFont();
	void Changing(QString fuente);   
    void JavaSize();
    void CameraMod();
    void Bootlogo();
    void Shortcut();
    void VR();
    void Slidertones();
    void Overclock();
    void Weblink();
    void CargarIdioma();
	void AMmod();
	void AMvib();
	void FontSize();
	void cambiar(QString archivo, QString valor);
	void Funlights();

	bool setFlexBit(unsigned short flexId, bool value);
	bool getFlexBit(unsigned short flexId, bool& value);

    void appClicked(ZListBoxItem *);   

  private:
    ZScrollPanel *scrollPanel;
    ZSoftKey *softKey;
    ZOptionsMenu * menu;
    ZNavTabWidget *tabWidget;

};



#endif

