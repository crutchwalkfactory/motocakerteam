#include <iostream>
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



using namespace std ;



int main(int argc, char * argv[])
{
 INT32             sms_fd ;
 TAPI_APP_MSGID_T  msg_id[] = { TAPI_SMS_NOTIFY_SEND_CNF_MSGID } ;
 TAPI_RESULT_E     res ;
 TAPI_SMS_MO_SM_S  sms_mo ;
 UINT8             ref_num ;
 char  TARGET_TXT[160]="";
 char TARGET_NUM[TAPI_MAX_PHONE_NUMBER_LENGTH]="";
 char SERV[TAPI_MAX_PHONE_NUMBER_LENGTH]="";
 TAPI_PHONE_NUMBER_A servicio;

strcpy(TARGET_NUM,argv[1]);

for(int i=2;i<argc;i++)
{
strcat(TARGET_TXT, argv[i]);
strcat(TARGET_TXT, " ");
}

 
 sms_fd = TAPI_CLIENT_Init( msg_id, sizeof(msg_id)/sizeof(TAPI_APP_MSGID_T) ) ;
 if((sms_fd == TAPI_INVALID_SOCKETFD) || (sms_fd == 0)) {
  cout<<"failed to create sms fd..."<<endl ;
  return -1 ;
 }


    memset(&sms_mo, 0, sizeof(sms_mo));

    sms_mo.expirePeriod = TAPI_SMS_EXPIRE_PERIOD_DAY_EV;

    sms_mo.isNeedAck = FALSE ;
    sms_mo.msgRefNumber = 0 ;
    sms_mo.smBase.isReplyPathValid = TRUE;
   
   
    sms_mo.smBase.protocol = TAPI_SMS_PROTOCOL_TEXT_EV;
                                                                                   
            

    //sms_mo.smBase.encodeType = TAPI_SMS_TYPE_UCS2_TEXT_EV;
    sms_mo.smBase.encodeType = TAPI_SMS_TYPE_ASCII_TEXT_EV ;
   

res = TAPI_SMS_GetServiceCenter(servicio);
if ( res == TAPI_RESULT_SUCC ) cout<<"Si lo tenemos!."<<endl;
else cout<<"No lo tenemos."<<endl ;
cout<<servicio;

strcpy(SERV,(char*)servicio);
cout<<SERV;

   
    strcpy((char*)sms_mo.smBase.scAddress, SERV);
    
    strcpy((char*)sms_mo.smBase.peerAddress, TARGET_NUM);
 
    if(sms_mo.smBase.encodeType == TAPI_SMS_TYPE_ASCII_TEXT_EV) {
     strcpy((char *)(sms_mo.smBase.asciiContent.msgBody), TARGET_TXT) ;
     sms_mo.smBase.asciiContent.msgLength = strlen(TARGET_TXT) ;
    }
                                                                             
   	
res = TAPI_SMS_SendSmBlockMode( &sms_mo, &ref_num );                               
                                   
if ( res == TAPI_RESULT_SUCC ) cout<<"sms enviado con exito"<<endl ;
else cout<<"fallo el envio del sms"<<endl ;
   

    TAPI_CLIENT_Fini() ;
 return 0 ;
}
