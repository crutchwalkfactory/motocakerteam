#include <stdio.h>
#include <stdlib.h>
#include <qevent.h>
#include <dlfcn.h>
#include <unistd.h>
#include <qstring.h>
#include <ZConfig.h>
#include <qpixmap.h>
#include <qfile.h>
#include <linux/power_ic.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>
#include <qapplication.h>
#include <qwidgetlist.h>
#include <qfileinfo.h>


typedef bool (*func_t)(void *th, QObject* o, QEvent* e);

func_t notificar()
{
   static func_t func;
   if (! func) {
     void *handle;
     handle = dlopen("/usr/lib/libezxappbase.so", RTLD_LAZY);
     if (! handle) {
       abort();
     }
     func = (func_t)dlsym(handle, "_ZN12ZApplication6notifyEP7QObjectP6QEvent"); 
     dlclose(handle);
   }
   return func;
}

class ZApplication
{
  public:
	 bool notify(QObject*, QEvent*);

  protected:

};    

bool ZApplication::notify(QObject* obj, QEvent* ev)
{


	if ( QFileInfo("/ezxlocal/LinXtend/.vibrate").isFile() )
	{
		QKeyEvent *k = (QKeyEvent*)ev;
		if ( (ev->type()==QEvent::KeyPress) && (!k->isAutoRepeat()) )
		{
			QWidgetList *t = QApplication::topLevelWidgets();
	 		QWidgetListIt it( *t );
	 		QWidget * ventana;
	 		bool isvibrate = false;
	 		while ( (ventana=it.current()) != 0 )
			{ 
	 			QString wClassName = QString( ventana->className() );
	 			if ( wClassName == "AM_Mainmenu" || wClassName == "AM_Secondmenu" || wClassName == "MyMenu" ) isvibrate = true;
				++it; 
			}
			if ( isvibrate )
			{
				int power_ic = open("/dev/" POWER_IC_DEV_NAME, O_RDWR);
				ioctl(power_ic, POWER_IC_IOCTL_PERIPH_SET_VIBRATOR_ON,1);

				usleep(40000);   // this is the vibration time!

				ioctl(power_ic, POWER_IC_IOCTL_PERIPH_SET_VIBRATOR_ON,0);
				close(power_ic);
			}
		}
	}

	bool of = notificar()(this, obj, ev);
	return of; 
}
