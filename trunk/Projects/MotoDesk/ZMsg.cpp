#include <qbuffer.h>
#include <qdatastream.h>
#include <qfile.h>
#include <unistd.h>
#include <errno.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include "ZMsg.h"

class ZMsgPrivateData 
{
public:
    QCString ch;
    QCString msg;
    bool localMsg;
};
 

#define 	APP_CHANNEL_PREF   "EZX/Application/"

ZMsg::ZMsg( const QCString& channel, const QCString& message,bool local ) :
    QDataStream(new QBuffer)
{
    d = new ZMsgPrivateData;

    d->ch = channel;
    d->msg = message;
    d->localMsg = local;

    device()->open(IO_WriteOnly);
}

void ZMsg::send()
{
    QByteArray data = ((QBuffer*)device())->buffer();

    int pref = strlen(APP_CHANNEL_PREF);
    if ( strncmp(d->ch.data(),APP_CHANNEL_PREF,pref)==0 )
    {
        QString msgfn("/tmp/qcop-msg-");
        msgfn += d->ch.mid(pref);
        QFile msgfile(msgfn);

        if ( msgfile.open(IO_WriteOnly | IO_Append) )
        {
            if(flock(msgfile.handle(),  LOCK_NB | LOCK_EX ) != 0)
            {
                msgfile.close();
                return;
            }

            struct stat buf;
            time_t t;
            if (!fstat(msgfile.handle(), &buf) &&  (time(&t) != (time_t)-1) )
            {
                if (buf.st_mtime + 60 < t)
                {
                    ftruncate(msgfile.handle(), 0);
                    msgfile.reset();
                }
            }

            if ( !QCopChannel::isRegistered(d->ch) )
            {
                {
                    QDataStream ds(&msgfile);
                    ds << d->ch << d->msg << data;
                    flock(msgfile.handle(), LOCK_UN);
                    msgfile.close();
                }

                return;
            }
            flock(msgfile.handle(), LOCK_UN);
            msgfile.close();
            msgfile.remove();
        }
    }
    if (d->localMsg)
    {
        QCopChannel::sendLocally(d->ch,d->msg,data);
    }
    else
    {
        QCopChannel::send(d->ch,d->msg,data);
    }
}

ZMsg::~ZMsg()
{
    if (d) {
       delete d;
       d = NULL;
    }
 
    if (device())
    {
        delete device();
    }
}

