/*
 * Unofficial EZX Native Software Development Kit
 * Copyright (C) 2007 Y.S Hsiao <ysakaed at gmail dot com>
 *
 *      2007-08-02 by Y.S Hsiao (intoxicated)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * Further, this software is distributed without any warranty that it is
 * free of the rightful claim of any third person regarding infringement
 * or the like.  Any license provided herein, whether implied or
 * otherwise, applies only to this software file.  Patent licenses, if
 * any, provided herein do not apply to combinations of this program with
 * other software, or any other product whatsoever.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#if !defined(__ZSMS_H__)
#define __ZSMS_H__

#include <qobject.h>
#include <qdatetime.h>

class ZSMS_OutgoingMsg;
class ZSMS_IncomingMsg;
class ZSMS_MsgRecordID;
class MSGAPI_INCOME_SMS_S;
class MSGAPI_MSG_RECORD_ID;
class MSGAPI_OUTGO_SMS_S;

class ZSMS : public QObject
{
    Q_OBJECT

public:

        ZSMS( QObject * parent = 0, const char * name = 0 );
        virtual ~ZSMS();
        int tmp[1024];
        void composeMsg(ZSMS_OutgoingMsg const&);                                     // compose window //
        void indOfIncomingMsg(ZSMS_IncomingMsg const&, ZSMS_MsgRecordID const&);
        void sendMsgWithResult(unsigned int*, ZSMS_OutgoingMsg const&);
        void enableInterceptCheck(bool);
        void rspnsOfSendMsgWithResult(unsigned int, int);
        void notifyInterceptCheckResult(ZSMS_MsgRecordID const&, bool);
        void gotoMessagingCenterHomeScreen();                                        // go to the message menu
        void sendMsg(ZSMS_OutgoingMsg const&);                                       // send out message
};

class ZSMS_OutgoingMsg
{
public:
        ZSMS_OutgoingMsg(QString const& , QString const& );  // Number, Content
        ZSMS_OutgoingMsg(ZSMS_OutgoingMsg const&);
        ~ZSMS_OutgoingMsg();
 	int tmp[1024];
        QString getAddress(void);
        QString getMsgBody(void);
        
};

class ZSMSPrivate : QObject
{
        Q_OBJECT
public:
        ZSMSPrivate(ZSMS*, QObject*, char const*);
        ~ZSMSPrivate();
        void vfnShowMsg(MSGAPI_INCOME_SMS_S const&, MSGAPI_MSG_RECORD_ID const&);
        void vfnIndOfIncomingMsg(MSGAPI_INCOME_SMS_S const&, MSGAPI_MSG_RECORD_ID const&);
        void vfnRspnsOfSaveToDraft(unsigned int, int, MSGAPI_MSG_RECORD_ID const&);
        void vfnRspnsOfSendMsgWithResult(unsigned int, int);
        void vfnRspnsOfGetLatestSmsInDraft(unsigned int, int, MSGAPI_OUTGO_SMS_S const&, MSGAPI_MSG_RECORD_ID const&);
};

class ZSMS_IncomingMsg
{
public:
        ZSMS_IncomingMsg(ZSMS_IncomingMsg const&);
        ZSMS_IncomingMsg();
        ~ZSMS_IncomingMsg();
        QString getAddress(void);
        QString getMsgBody(void);
        QDateTime getSentTime(void);
        bool isNull(void);
        bool isValid(void);
};

class ZSMS_MsgRecordID
{
public:
        ZSMS_MsgRecordID(ZSMS_MsgRecordID const&);
        ZSMS_MsgRecordID();
        ~ZSMS_MsgRecordID();
};

class ZSMSTypeConv
{
public:
        ZSMSTypeConv(void);
        ZSMS_IncomingMsg toZsmsIncomingMsg(MSGAPI_INCOME_SMS_S const&);
        ZSMS_MsgRecordID toZsmsMsgRecordID(MSGAPI_MSG_RECORD_ID const&);
        ZSMS_OutgoingMsg toZsmsOutgoingMsg(MSGAPI_OUTGO_SMS_S const&);
        MSGAPI_MSG_RECORD_ID toMsgapiMsgRecordID(ZSMS_MsgRecordID const&);
        MSGAPI_INCOME_SMS_S toMsgapiIncomeSms(ZSMS_IncomingMsg const&);
        ZSMS_OutgoingMsg toMsgapiOutgoSms(ZSMS_OutgoingMsg const&);

};


#endif  /* !defined(__ZSMS_H_) */
