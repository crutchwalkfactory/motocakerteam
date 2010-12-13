#ifndef ZGUI_H
#define ZGUI_H

#include <qobject.h>
#include <unistd.h>
#include <stdlib.h>
#include <qfile.h>

#include <ZNavTabWidget.h>
#include <ZKbMainWidget.h>
#include <ZListBox.h>
#include <ZSoftKey.h>
#include <ZScrollPanel.h>
#include <ZNoticeDlg.h>


//using namespace std;

class ZGui : public ZKbMainWidget
{
    Q_OBJECT
  public:
    ZGui (const QString &Argument = QString::null, QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~ZGui();
    void CreateWindow(QWidget* parent);
    QString texto(const char*xString);
	QString getFileName(const QString &AFileName);
	QString getFilePath(const QString &AFileName);
    QString getProgramDir();
    QString ProgDir;
    ZListBox* zllb1;
	void Create_MenuApp();
	void Create_MenuApp2();
    void Create_SoftKey();
    void Create_SoftKey2();

  public slots:
    void CargarMenu1();
    void unzip(QString destino);
	void browser();
    void destino(QString carpeta);
    void appClicked();
    void selected();
    void cancelar();
    void atras();

  private:
    ZSoftKey *softKey;
    ZOptionsMenu * menu;
    ZOptionsMenu * menu2;

};



#endif

