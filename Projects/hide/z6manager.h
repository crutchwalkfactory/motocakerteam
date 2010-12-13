#ifndef Z6MANAGER_H
#define Z6MANAGER_H

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

class Z6Man : public ZKbMainWidget
{
    Q_OBJECT
  public:
    Z6Man ( int argc, char **argv, QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~Z6Man();
    void CreateWindow(int argc, char **argv, QWidget* parent);
    QString texto(const char*xString);

  void Create_SoftKey();
  ZListBox* browser;
  ZListBox* browser2;

public slots:
  void CargarBrowser(QString direccion);
  void CargarBrowser2(QString direccion);
  void brClicked();   
  void brClicked2();   
  void selected();   


  signals:
    void picked(const QString &sFileName);
    void isFilePicked(bool);
    
  private:
    ZSoftKey *softKey;
    ZNavTabWidget *tabWidget;

};



#endif

