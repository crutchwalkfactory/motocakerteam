#include <iostream>
#include <string>
#include <TAPI_SIM_Client.h>
#include <TAPI_CLIENT_Func.h>
#include <TAPI_Result.h>
#include <TAPI_AppMsg.h>
#include <TAPI_AppMsgId.h>
#include <TAPI_General.h>
#include <TAPI_SETUP_General.h>
#include <EZX_TypeDefs.h>




using namespace std ;



int main(void)
{
 INT32             imsi_fd ;
 TAPI_APP_MSGID_T  msg_id[] = { TAPI_SMS_NOTIFY_SEND_CNF_MSGID } ;
 TAPI_RESULT_E     res ;
 TAPI_SIM_IMSI_EXT_S  imsi_actual;

 
 imsi_fd = TAPI_CLIENT_Init( msg_id, sizeof(msg_id)/sizeof(TAPI_APP_MSGID_T) ) ;
 if((imsi_fd == TAPI_INVALID_SOCKETFD) || (imsi_fd == 0)) {
  cout<<"No se pudo crear el imsi_fd"<<endl ;
  return -1 ;
 }


    memset(&imsi_actual, 0, sizeof(imsi_actual));



res = TAPI_SIM_GetImsiExt(&imsi_actual);
if ( res == TAPI_RESULT_SUCC ) cout<<"Si lo tenemos!."<<endl;
else cout<<"No lo tenemos."<<endl ;
   
    TAPI_CLIENT_Fini() ;

cout<<imsi_actual.mcc<<imsi_actual.mnc<<imsi_actual.subscribeId;
 return 0 ;
}
