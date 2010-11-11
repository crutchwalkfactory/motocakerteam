#include <ZKbMainWidget.h>
#include <ZApplication.h>
#include <qobject.h>
#include <unistd.h>
#include <stdlib.h>


#ifndef DAEMON_APP_H

class BotonVR: public ZApplication
{
  Q_OBJECT
  public:

	BotonVR(int argc, char ** argv);
	virtual ~BotonVR();
	void Presionado(const QCString &mensaje, const QByteArray &datos);

  protected slots:
	virtual void slotShutdown();
	virtual void slotQuickQuit();
	virtual void slotRaise();
};

#endif
