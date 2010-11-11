#include <qmap.h>
#include <qfile.h>
#include <qcopchannel_qws.h>
#include <qregexp.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qwmatrix.h>
#include <ezxres.h>
#include <qevent.h>
#include <qtextstream.h>
#include <qtextcodec.h>

#include <dirent.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/vfs.h>
#include <linux/power_ic.h>
#include <sys/ioctl.h>
#include "stdio.h"
#include <stdlib.h>
#include <unistd.h>
#include <ZApplication.h>

#include "vr.h"


#define Z6KEY_UP                0x1013

QCopChannel * ctecla;


BotonVR::BotonVR (int argc, char ** argv): ZApplication (argc, argv)
{ 
	printf ( "welcome" );
	if ( QCopChannel::isRegistered("/hardkey/bc") )
	{
		ctecla = new QCopChannel("/hardkey/bc", this); 
		connect( ctecla, SIGNAL(received(const QCString &,const QByteArray &)), this, SLOT(Presionado(const QCString &,const QByteArray &)) ); 
	} 
}

BotonVR::~BotonVR()
{
}

void BotonVR::Presionado(const QCString &mensaje, const QByteArray &datos)
{
	QDataStream stream(datos, IO_ReadOnly);
	if( mensaje == "keyMsg(int,int)")
	{
		int key, type;
		stream >> key >> type; 
		if( key == 4149 )
		{
			system("/ezxlocal/LinXtend/usr/bin/vr");
		} 
	}
}

void BotonVR::slotShutdown()
{
	processEvents();
}

void BotonVR::slotQuickQuit()
{
	processEvents();
}

void BotonVR::slotRaise()
{
	processEvents();
}

