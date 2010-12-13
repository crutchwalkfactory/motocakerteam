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
    QString getProgramDir();
    QString ProgDir;
    ZListBox* zllb1;
	void Create_MenuApp();
    void Create_SoftKey();

  public slots:
    void CargarMenu1();
    void appClicked1();
    void about();
    void addtask();
    void removetask();

  private:
    ZSoftKey *softKey;
    ZOptionsMenu * menu;


};



#endif

