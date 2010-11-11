#ifndef _PP_MEMORY_APPLICATION_H
#define _PP_MEMORY_APPLICATION_H

#include <stdio.h>
#include <stdlib.h>
#include <ZApplication.h>
#include <qcopchannel_qws.h>
#include <qtimer.h>
#include <qfile.h>
#include <qdatastream.h>
#include <qtextstream.h>
#include <qmap.h>
#include <sys/vfs.h>

#define PP_CHANNEL "/phone/info/widget/channel"

class Daemon : public ZApplication 
{
    Q_OBJECT
  public:
    Daemon(int argc, char **argv);
    virtual ~Daemon();

	public:
		QString ProgDir;

	public slots:
		void Presionado(const QCString &,const QByteArray &);
//		void sendString(const QString &text, const QCString &MESSAGE);
//		void sendImageState(const QString &state, const QCString &MESSAGE);

	private:
//		QTimer *timerMemory;
	
	protected slots:
		//virtual void keyPressEvent(QKeyEvent* k);
		virtual void slotShutdown();	
		virtual void slotQuickQuit();
		virtual void slotRaise();
			
		
};

#endif
