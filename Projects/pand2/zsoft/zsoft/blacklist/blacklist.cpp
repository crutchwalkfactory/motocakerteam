#include <sys/un.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <linux/ioctl.h>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <stdarg.h>
#include <qtextcodec.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <qtextstream.h>
#include "ZConfig.h"

using namespace std;


//libezxtapi-xscalr-r
extern "C" int UTIL_GetPowerOnMode(void);
extern "C" int UTIL_GetServiceName(char *buf);
extern "C" int UTIL_SetServiceName(char *name);
extern "C" int TAPI_VOICE_MakeCall(char *n1,int *size);
extern "C" int TAPI_VOICE_CancelCall(int *i);
extern "C" int TAPI_CLIENT_Init(int * pi,int j);
extern "C" void TAPI_CLIENT_Fini(void);
extern "C" int TAPI_CLIENT_GetSyncSocketFd(void);
extern "C" int TAPI_CLIENT_ReadAndMallocMsg(int i,char * pmsg8byte);//return 0 when successed;
//extern "C" int TAPI_VOICE_DropCurrentCall(int tapihandle);
extern "C" int TAPI_VOICE_DropAllCall();
extern "C" int TAPI_CALL_GetAllCallSessions(char * buf);
extern "C" int TAPI_CSD_GetCurrentConnectedDuration(char * buf);
extern "C" int TAPI_FIXDIALING_GetStatus(int tapihandle,char * buf);
extern "C" int TAPI_VOICE_RejectCall(int tapihandle, int x);

//libezxappbase-xscale-r
extern "C" int UTIL_GetPhoneInCall(void);
extern "C" int UTIL_GetIncomingCallStatus(void);
extern "C" int UTIL_GetPhoneCallIconIndex(void);
extern "C" int UTIL_GetPhoneApplicationStatus(void);
extern "C" int UTIL_GetPhoneSystemStatus(void);
extern "C" void UTIL_SetUSBChargingStatus(int b);



struct callsession
{
	int numofcall;
	int connectid;
	int connecttype;
	int connectstatus;
	char callno[];
};

bool _gquit = false;
void signalfunc(int s)
{
	_gquit = true;
}

int main(int argc, char* argv[])
{

	QString ProgDir = QString("%1").arg(argv[0]) ;
	int i = ProgDir.findRev ( "/" );
	ProgDir.remove ( i+1, ProgDir.length() - i );
	
	ZConfig blacklist ( QString("%1/zPhoneFirewall.cfg").arg(ProgDir), false );
	QStringList list; QString ltype;
    //QString ltype = blacklist.readEntry ( QString("zPhoneFirewall"), QString("Firewall"), "" );
	//if ( ltype = "Blacklist" ) { list = blacklist.readListEntry ( QString("Blacklist"), QString("Num"), QChar(';') ); }
	//if ( ltype = "Whitelist" ) { list = blacklist.readListEntry ( QString("Whitelist"), QString("Num"), QChar(';') ); }
	int encontro=-1;
	QString aux; QString answer;
	system("touch /mmc/mmca1/llamadas");

	signal( SIGHUP,  signalfunc );
	signal( SIGINT,  signalfunc );
	signal( SIGQUIT, signalfunc );
	signal( SIGTERM, signalfunc );	
	{		
		char buf[2048];
		bool printlog = false;
		int vare = 0,varf = 0;
		int tapihandle = TAPI_CLIENT_Init(&vare,varf);
		int r = 0;int r2 = 0;
		enum callstate{idle = 0,incomingcall = 1,connecting = 3,connected = 4,droped = 6};
		enum calltype{gsm = 2,gprs = 3};
		callstate cs = idle;


		while(!_gquit)
		{
			r = TAPI_CALL_GetAllCallSessions(buf);
			

			if(r != 0)continue;
			callsession *pcs = (callsession*)buf;
			if(pcs->numofcall >= 1)
			{
				if(cs != pcs->connectstatus)
				{
					if(printlog)cout << "connect status:" << pcs->connectstatus << " connectid:" << pcs->connectid << " connecttype:" << pcs->connecttype << endl;
					if(pcs->connecttype == gsm)
					{
						switch(pcs->connectstatus)
						{
							case incomingcall:
								if(printlog)cout << "incomingcall:" << pcs->callno << endl;
								
								ltype = blacklist.readEntry ( QString("zPhoneFirewall"), QString("Firewall"), "" );
								if ( ltype == "Blacklist" )
								{
									list = blacklist.readListEntry ( QString("Blacklist"), QString("Num"), QChar(';') );
									answer = "YES";
									for (int j=0;j<list.count();++j)
									{
									  aux=pcs->callno;
									  encontro=aux.find(list[j],0);
									  if(encontro >= 0)
									  { 
										answer = "NO";
									  }
									}
									if ( answer == "NO" )
									{
										TAPI_VOICE_RejectCall( pcs->numofcall, 0 );
										//TAPI_VOICE_DropAllCall();
										system(QString("echo \"%1\" >> /mmc/mmca1/llamadas").arg(pcs->callno));
									}
								}

								if ( ltype == "Whitelist" )
								{
									list = blacklist.readListEntry ( QString("Whitelist"), QString("Num"), QChar(';') );
									answer = "NO";
									for (int j=0;j<list.count();++j)
									{
									  aux=pcs->callno;
									  encontro=aux.find(list[j],0);
									  if(encontro >= 0)
									  { 
										answer = "YES";
									  }
									}
									if ( answer == "NO" )
									{
										//
										TAPI_VOICE_DropAllCall();
										system(QString("echo \"%1\" >> /mmc/mmca1/llamadas").arg(pcs->callno));
									}
								}

								cs = incomingcall;
								break;
							case connecting:
								if(printlog)cout << "connecting:" << pcs->callno << endl;
								cs = connecting;
								break;
							case connected:
								if(printlog)cout << "connected:" << pcs->callno << endl;

								cs = connected;
								break;
							case droped:
								if(printlog)cout << "droped:" << pcs->callno << endl;

								cs = droped;
								break;
							default:
								cs = (callstate)pcs->connectstatus;
						}
					}
				}
			}
			else
			{

				cs = idle;
			}
			usleep(400000);
		}
		TAPI_CLIENT_Fini();
		if(printlog)cout << "shut down..." << endl;		
	}

  return 0;
}
