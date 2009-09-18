#ifndef Z6MANAGER_H
#define Z6MANAGER_H

#include <qobject.h>
#include <unistd.h>
#include <stdlib.h>
#include <qfile.h>

#include <ZMessageDlg.h>
#include <ZNavTabWidget.h>
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


//using namespace std;

class Z6Man : public ZKbMainWidget
{
    Q_OBJECT
  public:
    Z6Man (const QString &Argument = QString::null, QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~Z6Man();
    QString getProgramDir();
    QString texto(const char*xString);
    QString getFileName(const QString &AFileName);
    QString ClaveCorrecta;
    void CreateWindow(QWidget* parent);
    QString ProgDir;
    QString Idioma;
    QWidget* dlg;
    ZNoticeDlg* znd;
	ZNumPickerDlg* swapspace;
    ZListBox* appListBox;
    ZListBox* runListBox;
    ZListBox* javaListBox;
    ZListBox* skinListBox;
    ZListBox* roboListBox;
	
	void Create_SoftKeyMain();
    void Create_SoftKey();
    void Create_SubMenuApp();
    void Create_MenuApp();
    void Create_MenuConfig();
    void Create_MenuAutorun();
    void Create_MenuTheme();
    void Create_MenuJava();

  signals:
    void picked(const QString &sFileName);
    void isFilePicked(bool);
    
  public slots:
    void Instalar(QString mpkg);
    void InstalarApp(QString mpkg, QString name);
    void InstalarSkin(QString mpkg, QString name);
    void about();
    void salir();
    void language();
    void LimpiarCR();
    void IngresoClave();
    void AsociarMPKGs();
    void CargarListaApps();
    void CargarListaJava();
    void CargarListaSkins();
    void CargarListaAutorun();
    void CargarListaRobo();
    void EntradaAutorun(QString entrada, int ubicacion, QString active);
    void AutorunAdd();
    void AutorunRemove();
    void JavaAdd();
    void JavaRemove();
    void EjecutarMPKG();
    void BuscarMPKG();
    void BorrarMPKG();
    void BorrarTema();
    void Cron();
    void Inetd();
    void Swap();
    void Antirrobo();
    void zNothief(QString numeros);
    void getIMEI();
	void runZNothief();
	void SendSMS(QString number, QString message);
    void ChangeFont(QString valor);
    void SelectFont(QString valor);
    void CambiarNombre();
    void CambiarIcono();


    void slotCurrentChanged(QWidget* AWidget);    

  //private slots:
    void appClicked(ZListBoxItem *);   
    void runClicked(ZListBoxItem *);   
    void itemClicked(ZListBoxItem *);

  private:
    ZScrollPanel *scrollPanel;
    ZNavTabWidget *tabWidget;
    ZSoftKey *softKey;
    ZOptionsMenu * menu;
    ZOptionsMenu * submenu;

};



#endif

