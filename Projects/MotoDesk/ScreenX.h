#ifndef ZGUI_H
#define ZGUI_H

#include <qobject.h>
#include <unistd.h>
#include <stdlib.h>
#include "ZFileDlg.h"
#include "zCallDlg.h"

#include <ZMessageDlg.h>
#include <ZListBox.h>
#include <ZKbMainWidget.h>
#include <ZNavTabWidget.h>
#include <ZScrollPanel.h>
#include <qmap.h>
#include <ZOptionsMenu.h>
#include <ZImage.h>
#include <qtimer.h>
#include <ZLabel.h>

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

	QString GetSize(QString tam);
	QString getFilePath(QString &AFileName);
    QString getProgramDir();
    QString ProgDir;
    void CreateWindow(QWidget* parent);  
    void toLog(QString mes);
	void sendMes(int pid, QString mes);
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
	QPixmap Icon1, Icon2, Icon3, Icon4, Icon5, Icon6;
	QPixmap fizqi, fderi, basei, ianalog;
	QPixmap MPicon, MPprevi, MPnexti;
	QPixmap preClock;
	QMovie fondoi;
	//QPixmap h1,h2,h3,h4,d1,d2,d3;
    
  public slots:
    void Recargar();
    void RecargarIconos();
    void RecargarMenu();
	void CargarProgramas();
	void bloquear();
	void Desbloquear();
	void DesbloquearAhora();
	void ModoAvion();
	void CheckAvion();
	void Bluetooth();
	void mainmenu();
	void CargarMenu(QString lugar);
	void Slider();
	void Salir();
	void cancelcall();
	void Izquierda();
	void Derecha();
	void RunProg(int numero);

	void CamButton();
	void CreateSMS();
	void ScreenOptions();
	void Alarm();
	void Walltimertick();
	void Clocktick();
	void CheckPositions();
	void LeerDatos();
	void CheckBT();

	void readWallClock();

	void timerTick();
	void AnalogClock();
	void ShowClock();
	//void showNotesInfo();
	void SystemInfo();

	void showIMEI();
	void showFLEX();

	void chLanguage();

	void MDstatus(const QCString &mensaje, const QByteArray &datos);
	void Presionado(const QCString &mensaje, const QByteArray &datos);
	void NotiNueva(const QCString &mensaje, const QByteArray &datos);
	void nLCD(const QCString &mensaje, const QByteArray &datos);
	void aStatus(const QCString &mensaje, const QByteArray &datos);
	void clearNoti();

	void getTrackInfo();
	void getAlbumArt();

  private:
  	ZNavTabWidget *tabWidget;
    ZSoftKey *softKey;
    ZOptionsMenu * menu;
	ZScrollPanel *Panel;
	ZImage* AlbumArt;
	ZImage* Icono1;
	ZImage* Icono2;
	ZImage* Icono3;
	ZImage* Icono4;
	ZImage* Icono5;
	ZImage* Icono6;
	QTimer *reloj;
	ZImage* fizq;
	ZImage* fder;
	ZImage* base;
	ZImage* biconos;
	ZImage* analog;
	ZImage* agujas;
	ZLabel* eti1;
	ZLabel* clock;
	ZLabel* date;
	ZLabel* lNotes;
	zCalls* llamada;
	zProfile* alerta;
	ZImage* fondo;
	ZImage* hora1;
	ZImage* hora2;
	ZImage* hora3;
	ZImage* hora4;
	ZImage* dia1;
	ZImage* dia2;
	ZImage* dia3;
	ZImage* dia4;
	QTimer *showbar;
	ZLabel* systemInfo1;
	ZLabel* systemInfo2;
	ZLabel* systemInfo3;
	ZLabel* trackinfo;

  protected:
	virtual void keyPressEvent(QKeyEvent* k);
	virtual void keyReleaseEvent(QKeyEvent* k);
	virtual void paintEvent( QPaintEvent * pe);
	virtual void slotShutdown();	
	virtual void slotQuickQuit();
	virtual void slotRaise();

};

#endif
