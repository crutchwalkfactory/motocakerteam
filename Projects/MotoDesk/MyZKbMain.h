#define QT_NO_DRAGANDDROP
#define QT_NO_PROPERTIES
#include <ZApplication.h>
#include <ZKbMainWidget.h>
#include <ZSoftKey.h>
#include <ZFormContainer.h>
#include <ZMessageDlg.h>
#include <qpainter.h>
#include <qthread.h>
#include <qlabel.h>
#include <stdio.h>
#include <stdlib.h>
#include <qfile.h>
#include <qtextcodec.h>
#include <qlayout.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <qimage.h>
#include <qtimer.h>
#include <qstringlist.h>
#include <qlayout.h>
#include <qdir.h>
#include <pthread.h>
#include <dirent.h>
#include <ZLabel.h>
#include <ZScrollPanel.h>

QString ChineseAndEnglish(const char*ChineseString);

class MyZKbMain : public ZKbMainWidget
{	
	Q_OBJECT

  public:
	bool flag_loading;
	QImage myimg;
	QImage myimg2;

  public:
	MyZKbMain(QString path):ZKbMainWidget( (ZHeader::HEADER_TYPE)3, NULL , "ZMainWidget" , 0)
	{	
		folder=path;
		myimg.load("skin.png");
		flag_loading=false;
		setMainWidgetTitle("Prog");
		setFullScreenMode(true, true);
		
	}

	void exit();
	void Probabilidad(int valor);

  public :
	~MyZKbMain();



  private slots:
	
	virtual void keyPressEvent(QKeyEvent* k);
	virtual void paintEvent( QPaintEvent * pe);

  private:
	

	QString folder;	

	ZMessageDlg * dlg;
	ZMessageDlg * dlg_jugadas;
	ZMessageDlg * dlg_basta;
	ZMessageDlg * reboot;

	ZMessageDlg * dlg_juga_cinco;

};

