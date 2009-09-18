#ifndef ZGUI_H
#define ZGUI_H

#include <qobject.h>
#include <unistd.h>
#include <stdlib.h>
#include "ZFileDlg.h"

#include <ZMessageDlg.h>
#include <ZListBox.h>
#include <ZKbMainWidget.h>
#include <ZNavTabWidget.h>
#include <ZScrollPanel.h>
#include <qmap.h>
#include <ZOptionsMenu.h>
#include <ZImage.h>

class BDRegistry
{
public:
    BDRegistry(){}
    ~BDRegistry(){}

	QString icon() const { return icon_; }
	QString name() const { return name_; }
	bool t() const { return t_; }
	void setIcon( QString salary ) { icon_ = salary; }
	void setName( QString salary ) { name_ = salary; }
	void setT( bool salary ) { t_ = salary; }

private:
	QString icon_;
	QString name_;
	bool t_;
};

class ScreenX : public ZKbMainWidget
{
    Q_OBJECT
  public:
    ScreenX (QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~ScreenX();
    ZListBox* zllb;
	ZIconView* illb;
    
	int pidPhone;  
	QString texto(const char*xString);
	QString getFilePath(QString &AFileName);
    QString getProgramDir();
    QString ProgDir;
    void CreateWindow(QWidget* parent);  
    void toLog(QString mes, bool r1 = false , bool r2 = false );
    void buildProcList();
    void nativIcon();
    void kvmToName();
    QString getJavaFolderInSD();
	bool procFilter(QString nameProc);    
	bool procFilterSh(QString nameProc);       
	bool addedProc(int pid);
	void pidToTop(int pid);
	void goToIdle();
	void sendMes(int pid, QString mes);
	void buildAppList();
	bool processIsRuning( pid_t pid );
	bool pidKill( int pid);
	void pidQuit( int pid );	
	QString readFileLine(const QString & fileName);
	void createProcMenu();
	void createAppMenu();
    void Create_SoftKey();
    void Create_MenuApp();
	
	bool cfg_SendReaisePhone;
	bool cfg_SendGoToIDLE;
	bool cfg_FiltrProc;    
	bool cfg_ShowJavaIcon;  
    bool cfg_ShowNativIcon; 
    bool cfg_ShowAppNoWindow;
    bool cfg_ShowIconOnRunApp;
    QString cfg_Lang;

	QString Prog1, Prog2, Prog3, Prog4, Prog5;
	QString Exec1, Exec2, Exec3, Exec4, Exec5;
	QPixmap Icon1, Icon2, Icon3, Icon4, Icon5;
    
  public slots:
    void Recargar();
	void CargarProgramas();
	void bloquear();
	void Desbloquear();
	void ModoAvion();
	void mainmenu();
	void CargarMenu(QString lugar);
	void Slider(int reason);
	void Salir(int reason);
	void Noticia();

	void CamButton();
	void CreateSMS();
	void ScreenOptions();
	void Alarm();
	void Settings();

        
  private:
  	ZNavTabWidget *tabWidget;
    ZSoftKey *softKey;
    ZOptionsMenu * menu;
	ZScrollPanel *Panel;
	ZImage* Icono1;
	ZImage* Icono2;
	ZImage* Icono3;
	ZImage* Icono4;
	ZImage* Icono5;
    
  protected:
	virtual void keyPressEvent(QKeyEvent* k);
};

#endif
