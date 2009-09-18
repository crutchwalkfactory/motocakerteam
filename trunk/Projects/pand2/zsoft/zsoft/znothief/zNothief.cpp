#include <iostream>
#include <fstream>
#include <string>
#include <TAPI_CLIENT_Func.h>
#include <TAPI_SIM_Client.h>
#include <TAPI_Result.h>
#include <TAPI_AppMsg.h>
#include <TAPI_AppMsgId.h>
#include <TAPI_General.h>
#include <TAPI_SMS_PduModeClient.h>
#include <TAPI_SMS_TextModeClient.h>
#include <TAPI_SMS_SettingClient.h>
#include <EZX_TypeDefs.h>
#include <TAPI_SETUP_General.h>
#include <qfile.h>
#include <qdatastream.h>
#include <qtextstream.h>


using namespace std ;


int main(int argc, char * argv[])
{
 INT32             sms_fd ;
 TAPI_APP_MSGID_T  msg_id[] = { TAPI_SMS_NOTIFY_SEND_CNF_MSGID } ;
 TAPI_RESULT_E     res ;
 TAPI_SMS_MO_SM_S  sms_mo ;
 UINT8             ref_num ;
 char  TARGET_TXT[200]="(zNothief por segnini75@hotmail.com)Alguien ha insertado una SIM en tu telefono, el IMSI detectado fue: ";
 char TARGET_NUM[TAPI_MAX_PHONE_NUMBER_LENGTH]="";
 char SERV[TAPI_MAX_PHONE_NUMBER_LENGTH]="";
 TAPI_PHONE_NUMBER_A servicio;
 TAPI_SIM_IMSI_EXT_S  imsi_actual;
 BOOLEAN setup = false;
 char IMSI[TAPI_MAX_PHONE_NUMBER_LENGTH]="";
 QString MYIMSI,MYTLF,ACIMSI;



strcpy(TARGET_NUM,argv[1]);

if(argc>1 && strcmp(argv[1],"setup")==0)
{//tenemos un argumento, asi que haremos setup
setup=true;
}

 
 sms_fd = TAPI_CLIENT_Init( msg_id, sizeof(msg_id)/sizeof(TAPI_APP_MSGID_T) ) ;
 if((sms_fd == TAPI_INVALID_SOCKETFD) || (sms_fd == 0)) {
  cout<<"failed to create sms fd..."<<endl ;
  return -1 ;
 }



    QFile f("/ezxlocal/download/appwrite/setup/znothief.cfg");
    if ( f.open(IO_ReadOnly) ) {    // file opened successfully
        QTextStream t( &f );        // use a text stream
        MYIMSI = t.readLine(); 
	MYTLF = t.readLine();       
           //printf( "%3d: %s\n", n++, (const char *)s );

        f.close();
	
	strcat(TARGET_NUM,MYTLF);

	
    }



memset(&imsi_actual, 0, sizeof(imsi_actual));

res = TAPI_SIM_GetImsiExt(&imsi_actual);
if ( res == TAPI_RESULT_SUCC ) cout<<"Tenemos el IMSI!."<<endl;
else cout<<"No tenemos el IMSI :(."<<endl ;
strcpy(IMSI,(char*)imsi_actual.mcc);
strcat(IMSI,(char*)imsi_actual.mnc);
strcat(IMSI,(char*)imsi_actual.subscribeId);
ACIMSI=QString(IMSI);
strcat(TARGET_TXT,IMSI+'\0');

if(setup==false)
{//if

    memset(&sms_mo, 0, sizeof(sms_mo));

    sms_mo.expirePeriod = TAPI_SMS_EXPIRE_PERIOD_DAY_EV;

    sms_mo.isNeedAck = FALSE ;
    sms_mo.msgRefNumber = 0 ;
    sms_mo.smBase.isReplyPathValid = TRUE;
   
   
    sms_mo.smBase.protocol = TAPI_SMS_PROTOCOL_TEXT_EV;
                                                                                   
            

    //sms_mo.smBase.encodeType = TAPI_SMS_TYPE_UCS2_TEXT_EV;
    sms_mo.smBase.encodeType = TAPI_SMS_TYPE_ASCII_TEXT_EV ;
   

res = TAPI_SMS_GetServiceCenter(servicio);
if ( res == TAPI_RESULT_SUCC ) cout<<"Tenemos el scService!."<<endl;
else cout<<"No tenemos el scService :(."<<endl ;
cout<<servicio;

strcpy(SERV,(char*)servicio);
cout<<SERV;

   
    strcpy((char*)sms_mo.smBase.scAddress, SERV);
    
    strcpy((char*)sms_mo.smBase.peerAddress, TARGET_NUM);
 
    if(sms_mo.smBase.encodeType == TAPI_SMS_TYPE_ASCII_TEXT_EV) {
     strcpy((char *)(sms_mo.smBase.asciiContent.msgBody), TARGET_TXT) ;
     sms_mo.smBase.asciiContent.msgLength = strlen(TARGET_TXT)+1 ;
    }
}//fi


if(setup==true)
{

  	QFile f("/ezxlocal/download/appwrite/setup/znothief.cfg");
        f.open(IO_WriteOnly);        
	QTextStream t( &f );    
       	t<<IMSI<<endl<<argv[2];
        f.close();


}//fi

                                                                             
if(setup==false)
{//if  
	
  // cout<<ACIMSI<<" == "<<MYIMSI<<endl;
 if(QString::compare(ACIMSI,MYIMSI)!=0)
 {
   cout<<IMSI;

   res = TAPI_SMS_SendSmBlockMode( &sms_mo, &ref_num );                               
                                   
   if ( res == TAPI_RESULT_SUCC ) cout<<"sms enviado con exito"<<endl ;
   else cout<<"fallo el envio del sms"<<endl ;

 }
}//fi   

    TAPI_CLIENT_Fini() ;

 return 0 ;
}
