#include <linux/power_ic.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "demonio.h"

QCopChannel * ctecla;
QString msg;
int start;

extern "C" int UTIL_GetIncomingCallStatus(void);

Daemon::Daemon(int argc, char **argv) : ZApplication(argc, argv) 
{
	printf ( "Incoming Calls daemon by CepiPerez \n" );
	if ( QCopChannel::isRegistered("UNS/Server") )
	{
		ctecla = new QCopChannel("UNS/Server", this); 
		connect( ctecla, SIGNAL(received(const QCString &,const QByteArray &)), this, SLOT(Presionado(const QCString &,const QByteArray &)) ); 
		msg=""; start=1;
	} else {
		printf ( "UNS Server error\n" );
	} 


}

Daemon::~Daemon() 
{
}

void Daemon::slotShutdown()
{
	processEvents();
}

void Daemon::slotQuickQuit()
{
	processEvents();
}

void Daemon::slotRaise()
{
	processEvents();
}


void Daemon::Presionado(const QCString &mensaje, const QByteArray &datos)
{

	ZConfig Vini("/ezxlocal/download/appwrite/setup/ezx_am.cfg", true);
	int flashlight = Vini.readNumEntry(QString("CALLS"), QString("Flashlight"), 0);

	if ( mensaje == "startPopupWD()" )
	{
		while ( UTIL_GetIncomingCallStatus() )
		{
			if ( start==1 ) start=0; else start=1;

			printf ( "IncomingCall start!!!\n" );
			int power_ic = open("/dev/" POWER_IC_DEV_NAME, O_RDWR);
			LIGHTS_FL_UPDATE_T upd;
			upd.nRegion = LIGHTS_FL_REGION_DISPLAY_BL;
			upd.nApp = LIGHTS_FL_APP_CTL_DEFAULT;
			upd.nColor = LIGHTS_FL_COLOR_WHITE;
			if ( start==1 ) upd.nColor = LIGHTS_FL_COLOR_BLACK;
			else upd.nColor = LIGHTS_FL_COLOR_WHITE;
			ioctl(power_ic, POWER_IC_IOCTL_LIGHTS_FL_UPDATE, &upd);

			LIGHTS_FL_UPDATE_T upd3;
			upd3.nRegion = LIGHTS_FL_REGION_KEYPAD_NUM_BL;
			upd3.nApp = LIGHTS_FL_APP_CTL_DEFAULT;
			if ( start==1 ) upd3.nColor = LIGHTS_FL_COLOR_WHITE;
			else upd3.nColor = LIGHTS_FL_COLOR_BLACK;
			ioctl(power_ic, POWER_IC_IOCTL_LIGHTS_FL_UPDATE, &upd3);

			LIGHTS_FL_UPDATE_T upd4;
			upd4.nRegion = LIGHTS_FL_REGION_KEYPAD_NAV_BL;
			upd4.nApp = LIGHTS_FL_APP_CTL_DEFAULT;
			if ( start==1 ) upd3.nColor = LIGHTS_FL_COLOR_WHITE;
			else upd4.nColor = LIGHTS_FL_COLOR_BLACK;
			ioctl(power_ic, POWER_IC_IOCTL_LIGHTS_FL_UPDATE, &upd4);

			if ( flashlight == 1 )
			{
				LIGHTS_FL_UPDATE_T upd5;
				upd5.nRegion = LIGHTS_FL_REGION_CAMERA_FLASH;
				upd5.nApp = LIGHTS_FL_APP_CTL_DEFAULT;
				if ( start==1 ) upd5.nColor = LIGHTS_FL_CAMERA_TORCH;
				else  upd5.nColor = LIGHTS_FL_COLOR_BLACK;
				ioctl(power_ic, POWER_IC_IOCTL_LIGHTS_FL_UPDATE, &upd5);
			}

			LIGHTS_FL_UPDATE_T upd2;
			upd2.nRegion = LIGHTS_FL_REGION_SOL_LED;
			upd2.nApp = LIGHTS_FL_APP_CTL_DEFAULT;
			if ( start==1 ) upd2.nColor = LIGHTS_FL_COLOR_WHITE;
			else upd2.nColor = LIGHTS_FL_COLOR_BLACK;
			ioctl(power_ic, POWER_IC_IOCTL_LIGHTS_FL_UPDATE, &upd2);

			usleep(500000);
		}
	}
	else
	{
		printf ( "IncomingCall end!!!\n" );
		int power_ic = open("/dev/" POWER_IC_DEV_NAME, O_RDWR);
		LIGHTS_FL_UPDATE_T upd;
		upd.nRegion = LIGHTS_FL_REGION_DISPLAY_BL;
		upd.nApp = LIGHTS_FL_APP_CTL_DEFAULT;
		upd.nColor = LIGHTS_FL_COLOR_BLACK;
		ioctl(power_ic, POWER_IC_IOCTL_LIGHTS_FL_UPDATE, &upd);

		LIGHTS_FL_UPDATE_T upd3;
		upd3.nRegion = LIGHTS_FL_REGION_KEYPAD_NUM_BL;
		upd3.nApp = LIGHTS_FL_APP_CTL_DEFAULT;
		upd3.nColor = LIGHTS_FL_COLOR_BLACK;
		ioctl(power_ic, POWER_IC_IOCTL_LIGHTS_FL_UPDATE, &upd3);

		LIGHTS_FL_UPDATE_T upd4;
		upd4.nRegion = LIGHTS_FL_REGION_KEYPAD_NAV_BL;
		upd4.nApp = LIGHTS_FL_APP_CTL_DEFAULT;
		upd4.nColor = LIGHTS_FL_COLOR_BLACK;
		ioctl(power_ic, POWER_IC_IOCTL_LIGHTS_FL_UPDATE, &upd4);

		LIGHTS_FL_UPDATE_T upd5;
		upd5.nRegion = LIGHTS_FL_REGION_CAMERA_FLASH;
		upd5.nApp = LIGHTS_FL_APP_CTL_DEFAULT;
		upd5.nColor = LIGHTS_FL_COLOR_BLACK;
		ioctl(power_ic, POWER_IC_IOCTL_LIGHTS_FL_UPDATE, &upd5);

		LIGHTS_FL_UPDATE_T upd2;
		upd2.nRegion = LIGHTS_FL_REGION_SOL_LED;
		upd2.nApp = LIGHTS_FL_APP_CTL_DEFAULT;
		upd2.nColor = LIGHTS_FL_COLOR_BLACK;
		ioctl(power_ic, POWER_IC_IOCTL_LIGHTS_FL_UPDATE, &upd2);

	}

}


