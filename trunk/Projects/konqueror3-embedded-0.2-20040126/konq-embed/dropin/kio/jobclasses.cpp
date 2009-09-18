/*  This file is part of the KDE project
 
    This file is derived from kdelibs/kio/job.cpp.

    Copyright (C) 2000 Stephan Kulow <coolo@kde.org>
                       David Faure <faure@kde.org>
                       Waldo Bastian <bastian@kde.org>
		       Simon Hausmann <hausmann@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include "jobclasses.h"
#include "slavebase.h"
#include "scheduler.h"
#include "mimehandler.h"

#include <assert.h>

#include <qtimer.h>

#include <slave.h>

#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <global.h>

using namespace KIO;

#define KIO_ARGS QByteArray packedArgs; QDataStream stream( packedArgs, IO_WriteOnly ); stream

Job::Job( bool showProgressInfo ) : m_reenter( 0 ), m_destruct( false )
{
    m_showProgressInfo = showProgressInfo;
    m_error = 0;
    m_task = -1;
    m_percent = 0;
    // ###
}

Job::~Job()
{
}

bool Job::recurseExit()
{
    if ( --m_reenter > 0 || !m_destruct )
	return false;

    delete this;
    return true;
}

void Job::kill()
{
    // ###
    if ( m_reenter > 0 )
	m_destruct = true;
    else
	delete this;
}

QString Job::errorString() const
{
    return KIO::buildErrorString(m_error, m_errorText);
}

void Job::emitPercent( unsigned long size, unsigned long total )
{
  unsigned long ipercent = m_percent;

  if( total == 0 ) m_percent = 100;
  else m_percent = 100 * size / total;

  if( ( ipercent != m_percent ) || ( m_percent == 100 ) )
    emit percent( this, m_percent );
}

void Job::showErrorDialog( QWidget *parent )
{
    kdDebug() << k_funcinfo << " called" << endl;
    kdDebug() << errorString() << endl;

    if ( m_error != ERR_USER_CANCELED )
        KMessageBox::error( parent, errorString() );
}

SimpleJob::SimpleJob( const KURL &url, int command,
                      const QByteArray &packedArgs,
                      bool showProgressInfo )
    : Job( showProgressInfo )
{
    m_url = url;
    m_command = command;
    m_packedArgs = packedArgs;
    m_totalSize = 0;
    m_slave = 0;

    if ( m_url.isMalformed() )
    {
        m_error = ERR_MALFORMED_URL;
        m_errorText = m_url.url();
        QTimer::singleShot( 0, this, SLOT( slaveFinished() ) );
        return;
    }

    if ( !Scheduler::self()->doJob( this ) )
    {
        m_error = ERR_UNSUPPORTED_PROTOCOL;
        m_errorText = m_url.protocol();
        QTimer::singleShot( 0, this, SLOT( slaveFinished() ) );
        return;
    }
}

SimpleJob::~SimpleJob()
{
}

void SimpleJob::start( Slave *slave )
{
    m_slave = slave;

    assert( m_slave );

    QObject::connect( m_slave, SIGNAL( dataReq() ),
                      this, SLOT( dataReq() ) );
    QObject::connect( m_slave, SIGNAL( redirection( const KURL & ) ),
                      this, SLOT( slaveRedirection( const KURL & ) ) );
    QObject::connect( m_slave, SIGNAL( finished() ),
                      this, SLOT( slaveFinished() ) );
    QObject::connect( m_slave, SIGNAL( data( const QByteArray & ) ),
                      this, SLOT( receiveData( const QByteArray & ) ) );
    QObject::connect( m_slave, SIGNAL( infoMessage( const QString & ) ),
                      this, SLOT( slotInfoMessage( const QString & ) ) );
    QObject::connect( m_slave, SIGNAL( error( int, const QString & ) ),
                      this, SLOT( slotError( int, const QString & ) ) );

    QObject::connect( m_slave, SIGNAL( processedSize( KIO::filesize_t ) ),
		      this, SLOT( slotProcessedSize( KIO::filesize_t ) ) ) ;
    QObject::connect( m_slave, SIGNAL( totalSize( KIO::filesize_t ) ),
		      this, SLOT( slotTotalSize( KIO::filesize_t ) ) ) ;

    KIO_ARGS << m_url.host() << (Q_INT32)m_url.port() << m_url.user() << m_url.pass();
    m_slave->send( CMD_HOST, packedArgs );

    m_slave->send( m_command, m_packedArgs );
}

void SimpleJob::kill()
{
    Scheduler::self()->releaseJob( this, true );
    Job::kill();
}

void SimpleJob::receiveData( const QByteArray & )
{
}

void SimpleJob::slaveFinished()
{
    Scheduler::self()->releaseJob( this );

    recurseEnter();
    emit result( this );
    recurseExit();

    Job::kill();
}

void SimpleJob::slaveRedirection( const KURL & )
{
}

void SimpleJob::dataReq()
{
    // ###
}

void SimpleJob::slotInfoMessage( const QString &msg )
{
    emit infoMessage( this, msg );
}

void SimpleJob::slotError( int id, const QString &text )
{
    m_error = id;
    m_errorText = text;
    QTimer::singleShot( 0, this, SLOT( slaveFinished() ) );
}

void SimpleJob::slotTotalSize( KIO::filesize_t total_size )
{
  m_totalSize = total_size;
  emit totalSize( this, m_totalSize );
}

void SimpleJob::slotProcessedSize( KIO::filesize_t size )
{
  recurseEnter();
  emit processedSize( this, size );
  if ( recurseExit() )
    return;

  if( size > m_totalSize ) slotTotalSize( size );
  emitPercent( size, m_totalSize );
}

void SimpleJob::slotPercent( unsigned long p )
{
  emit percent( this, p );
}


QPtrList<TransferJob> *TransferJob::s_detachedJobs = 0;

TransferJob::TransferJob( const KURL &url, int command,
                          const QByteArray &packedArgs,
                          const QByteArray &staticData,
                          bool showProgressInfo )
    : SimpleJob( url, command, packedArgs, showProgressInfo )
{
    m_staticData = staticData;

    m_detached = false;
    m_cachedDataEmitted = false;
    m_finishAfterCacheEmit = false;
    m_filter = 0;
    m_firstdata = true;
}

TransferJob::~TransferJob()
{
    if ( s_detachedJobs )
        s_detachedJobs->removeRef( this );
    if ( m_filter )
	delete m_filter;
}

void TransferJob::setMetaData( const KIO::MetaData &data )
{
    m_outgoingMetaData = data;
}

void TransferJob::addMetaData( const QString &key, const QString &value )
{
    m_outgoingMetaData.insert( key, value );
}

void TransferJob::addMetaData( const QMap<QString,QString> &values )
{
    QMap<QString,QString>::ConstIterator it = values.begin();
    QMap<QString,QString>::ConstIterator end = values.end();
    for (; it != end; ++it )
        m_outgoingMetaData.insert( it.key(), it.data() );
}

void TransferJob::setIncomingMetaData( const MetaData &dat )
{
    m_incomingMetaData = dat;
}

MetaData TransferJob::metaData()
{
    return m_incomingMetaData;
}

MetaData TransferJob::outgoingMetaData()
{
    return m_outgoingMetaData;
}

QString TransferJob::queryMetaData( const QString &key )
{
    MetaData::ConstIterator it = m_incomingMetaData.find( key );
    if ( it == m_incomingMetaData.end() )
        return QString::null;
    return it.data();
}

void TransferJob::slotMimetype( const QString& type )
{
    m_mimetype = type;
    mimeRules( MimeHandler::Find( type ) );
    if ( !m_mimetype.isEmpty() )
	emit mimetype( this, m_mimetype );
}

void TransferJob::start( Slave *slave )
{
    m_slave = slave;

    assert( m_slave );

    QObject::connect( m_slave, SIGNAL( metaData( const KIO::MetaData & ) ),
                      this, SLOT( setIncomingMetaData( const KIO::MetaData & ) ) );

    QObject::connect( m_slave, SIGNAL( mimeType( const QString & ) ),
                      this, SLOT( slotMimetype( const QString & ) ) );

    m_mimetype = "";
    m_firstdata = true;

    KIO_ARGS << m_outgoingMetaData;

    m_slave->send( CMD_META_DATA, packedArgs );

    SimpleJob::start( slave );
}

void TransferJob::kill()
{
// hmmm, disabled this check for now. it breaks 'stop'
// (Simon)
//    if ( m_detached )
//        return;

    SimpleJob::kill();
}

void TransferJob::detach( const QByteArray &cachedData )
{
    assert( !m_detached );

    m_detached = true;

    if ( !s_detachedJobs )
#if QT_VERSION < 300
        s_detachedJobs = new QList<TransferJob>;
#else
        s_detachedJobs = new QPtrList<TransferJob>;
#endif

    assert( s_detachedJobs->findRef( this ) == -1 );

    s_detachedJobs->append( this );

    m_cachedData.clear();
    m_cachedData << cachedData;
}

void TransferJob::attach()
{
    assert( s_detachedJobs );
    assert( m_detached );
    assert( s_detachedJobs->findRef( this ) != -1 );

    m_detached = false;
    s_detachedJobs->removeRef( this );

    QTimer::singleShot( 0, this, SLOT( slotEmitCachedData() ) );
    m_cachedDataEmitted = false;
}

void TransferJob::slotEmitCachedData()
{
    // ### re-check this

    while ( m_cachedData.count() > 0 )
    {
        QByteArray stuff = *m_cachedData.begin();
        m_cachedData.remove( m_cachedData.begin() );

        // ### do we need to check for redirection and the like, like in receiveData() ??
	recurseEnter();
	emit data( this, stuff );
	if ( recurseExit() )
	    return;
    }

    m_cachedDataEmitted = true;

    if ( m_finishAfterCacheEmit ) // we are done
    {
	// peri, QTimer::singleShot( 0, this, SLOT( slaveFinished() ) );
	// it is not required, here we are it means, slaveFinsihed is called
	// code copied from the 'slaveFinished'
    	if ( m_redirectionURL.isEmpty() || m_redirectionURL.isMalformed() || m_error )
            SimpleJob::slaveFinished();
        else
        {
            kdDebug() << "performing redirection" << endl;

            m_staticData.truncate( 0 );

            m_url = m_redirectionURL;
            m_redirectionURL = KURL();

            KURL dummyURL;
            QString dummyString;

            switch ( m_command )
            {
                case CMD_GET:
                    {
                        kdDebug() << "CMD_GET redirection" << endl;
                        m_packedArgs.truncate( 0 );
                        QDataStream stream( m_packedArgs, IO_WriteOnly );
                        stream << m_url;
                    }
                    break;
                case CMD_SPECIAL:
                    {
                        kdDebug() << "HTTP_POST redirection" << endl;
                        int specialcmd;
                        QDataStream istream( m_packedArgs, IO_ReadOnly );
                        istream >> specialcmd;
                        assert( specialcmd == 1 );

                        addMetaData( "cache", "reload" );

                        m_packedArgs.truncate( 0 );
                        QDataStream stream( m_packedArgs, IO_WriteOnly );
                        stream << m_url;
                        m_command = CMD_GET;
                    }
                    break;
                default: assert( 0 );
            } 

            QTimer::singleShot( 0, this, SLOT( slotRedirectDelayed() ) );
        }
        return;
    } 
}

TransferJob *TransferJob::findDetachedJobForURL( const KURL &url )
{
    if ( !s_detachedJobs )
        return 0;

    QPtrListIterator<TransferJob> it( *s_detachedJobs );
    for ( ; it.current(); ++it )
        if ( it.current()->url() == url && !it.current()->isDestructing() )
            return it.current();

    return 0;
}

void TransferJob::dataReq()
{
    m_slave->send( MSG_DATA, m_staticData );
    m_staticData = QByteArray();
}

void TransferJob::slaveRedirection( const KURL &url )
{
    if ( m_redirectionList.contains( url ) > 5 )
    {
        m_error = ERR_CYCLIC_LINK;
        m_errorText = url.prettyURL();
    }
    else
    {
        kdDebug() << "redirection #1 : " << url.prettyURL() << endl;
        m_redirectionURL = url;
        m_redirectionList.append( url );
        emit redirection( this, url );
    }
}

void TransferJob::slaveFinished()
{
    m_slave->disconnect( this );

    // if we are detached then don't kill us.
    // also it could happen that the SST for the re-mission of the
    // cached data is pending and right before that we get called
    // here. so we need to sort that out here.
    if ( m_detached || m_cachedData.count() > 0 )
    {
	Scheduler::self()->releaseJob( this );
	m_finishAfterCacheEmit = true;
	return;
    }

    kdDebug() << "_finished" << endl;
    if ( m_redirectionURL.isEmpty() || m_redirectionURL.isMalformed() || m_error )
        SimpleJob::slaveFinished();
    else
    {
        kdDebug() << "performing redirection" << endl;

        m_staticData.truncate( 0 );

        m_url = m_redirectionURL;
        m_redirectionURL = KURL();

        KURL dummyURL;
        QString dummyString;

        switch ( m_command )
        {
            case CMD_GET:
            {
                kdDebug() << "CMD_GET redirection" << endl;
                m_packedArgs.truncate( 0 );
                QDataStream stream( m_packedArgs, IO_WriteOnly );
                stream << m_url;
            }
            break;
            case CMD_SPECIAL:
            {
                kdDebug() << "HTTP_POST redirection" << endl;
                int specialcmd;
                QDataStream istream( m_packedArgs, IO_ReadOnly );
                istream >> specialcmd;
                assert( specialcmd == 1 );

                addMetaData( "cache", "reload" );

                m_packedArgs.truncate( 0 );
                QDataStream stream( m_packedArgs, IO_WriteOnly );
                stream << m_url;
                m_command = CMD_GET;
            }
            break;
            default: assert( 0 );
        }

        QTimer::singleShot( 0, this, SLOT( slotRedirectDelayed() ) );
    }

}

void TransferJob::slotRedirectDelayed()
{
    kdDebug() << "slotRedirectDelayed()" << endl;
    // ### optimize this
    Scheduler::self()->releaseJob( this );
    Scheduler::self()->doJob( this );
}

void TransferJob::filteredData( const QByteArray &dat )
{
    if ( m_redirectionURL.isEmpty() || m_redirectionURL.isMalformed() || m_error )
    {
        if ( m_detached )
            m_cachedData << dat;
        else
        {
            // do we have data pending? if yes, then just append ;)
            if ( m_cachedData.count() > 0 && !m_cachedDataEmitted )
                m_cachedData << dat;
            else
		emit data( this, dat );
        }
    }
}

void TransferJob::receiveData( const QByteArray &dat )
{
    if ( m_firstdata )
    {
	m_firstdata = false;
	if ( m_mimetype.isEmpty() )
	{
	    // Try cheapest solution first
	    const MimeHandler* mime = MimeHandler::Find( m_url );
	    if ( !mime ) mime = MimeHandler::Find( dat );
	    if ( mime )
	    {
		mimeRules( mime );
		if ( m_mimetype.isEmpty() )
		    m_mimetype = mime->Preferred();
		recurseEnter();
		emit mimetype( this, m_mimetype );
		if ( recurseExit() )
		    return;
	    }
	}
    }
    recurseEnter();
    if ( m_filter )
	m_filter->dataInput( dat );
    else
	filteredData( dat );
    recurseExit();
}

void TransferJob::mimeRules( const MimeHandler* mime )
{
    if ( mime )
    {
	if ( mime->isFilter() )
	    AddFilter( mime->isFilter() );
	else if ( mime->isAlias() )
	    m_mimetype = mime->isAlias()->outputType();
    }
}

bool TransferJob::AddFilter( const MimeFilter* filter )
{
    if ( m_filter )
	return false;
    m_filter = filter->createFilter( m_mimetype, m_url );
    if ( m_filter )
    {
	m_mimetype = m_filter->outputType();
	if ( m_mimetype.isEmpty() )
	    m_mimetype = filter->outputType();
	QObject::connect( m_filter, SIGNAL( dataOutput( const QByteArray & ) ),
                          this, SLOT( filteredData( const QByteArray & ) ) );
	return true;
    }
    return false;
}

#include "jobclasses.moc"

/* vim: et sw=4
 */
