#ifndef ZMSG_H
#define ZMSG_H

#include <qcopchannel_qws.h>
#include <qdatastream.h>


class ZMsgPrivateData;

class ZMsg : public QDataStream
{

  public:

    ZMsg( const QCString& channel, const QCString& message, bool local = false );
    ~ZMsg();
    void send();

  private:
    ZMsgPrivateData *d;
  
};


#endif

