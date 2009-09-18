/*  This file is part of the KDE project
    Copyright (C) 2000,2001 Simon Hausmann <hausmann@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    As a special exception this program may be linked with Qt non-commercial 
    edition, the resulting executable be distributed, without including the 
    source code for the Qt non-commercial edition in the source distribution.

*/

#include <config.h>
#include <kapplication.h>
#include <stdlib.h>
#include <kcookieserver.h>
#include <authcache.h>
#include <khtml_part.h>
#include <klibloader.h>
#include <ecma/kjs_proxy.h>
#include <kiconloader.h>
#include <kstandarddirs.h>
#include <klocale.h>
#include <qtranslator.h>
#include <qmessagebox.h>
#include <qdir.h>
#include <qrect.h>
#include <kio/launcher.h>
#include <kio/uiserver.h>
#include <kssld.h>
#include <kconfig.h>
#include <mimehandler.h>

#ifdef _WS_QWS_
#include <qwsdisplay_qws.h>
#ifdef QT_NO_QWS_CURSOR
#include "popuphandler.h"
#endif
#endif

#ifdef _QT_QPE_
#include "konqplugininterface.h"
#include <qpe/qlibrary.h>
#endif

#if !defined(Q_WS_WIN)
#include <qfile.h>
#include <signal.h>

void setupSignalHandler()
{
    struct sigaction sa;
    memset( &sa, 0, sizeof( sa ) );
    sa.sa_handler = SIG_IGN;
    sigemptyset( &sa.sa_mask );
#if defined(SA_RESTART)
    sa.sa_flags = SA_RESTART;
#endif
    sigaction( SIGCHLD, &sa, 0 );
}

#endif // !Q_WS_WIN

#include "mainwindowbase.h"
#include "pics/file_broken.xpm"

// ### from ecma/kjs_proxy.cpp and khtml_factory.cpp
extern "C"
{
#if defined(ENABLE_JAVASCRIPT) && !defined(DLOPEN_JAVASCRIPT)
    KJSProxy *kjs_html_init( KHTMLPart * );
#endif
    void *init_libkhtml();
#define KONQE_INIT(f) extern void f()
#include "init.inc"
#undef KONQE_INIT
#define KONQE_INIT(f) f()
static void addons_init() {
#include "init.inc"
}
#undef KONQE_INIT
}

// ### from http_cache_cleaner.cpp
extern int http_cache_cleaner_main( int, char ** );

#ifdef NDEBUG
void messageHandler( QtMsgType , const char *)
{
	// eat this
}
#endif

int main( int argc, char **argv )
{
#if defined(Q_WS_WIN)
    if ( getenv( "KDEDIR" ) == 0 )
    {
	QCString kdedir;
	kdedir.sprintf( "KDEDIR=%s", QDir::currentDirPath().local8Bit().data() );
	_putenv( kdedir.data() );
    }
#else
    setupSignalHandler();
#endif

#ifdef NDEBUG
    qInstallMsgHandler( messageHandler );
#endif
    KApplication app( argc, argv, "konq-embed" );

    if ( locate( "data", "khtml/css/html4.css" ).isEmpty() )
    {
	QMessageBox::critical( 0, i18n( "Fatal Error" ),
			       i18n( "Cannot find data file html4.css .\n"
				     "Make sure the KDEDIR environment variable is set correctly\n"
				     "or you execute this binary in the correct path. In the\n"
				     "directory KDEDIR points to to or in the directory the\n"
				     "executable is located there has to be the file:\n"
				     "share/apps/khtml/css/html4.css\n\n"
				     "Exiting..." ) );
	return 1;
    }

#if defined(ENABLE_FONTSUBS)
    typedef QMap<QString,QString> EntryMap;
    KConfig *cfg = KGlobal::config();
    EntryMap fsubs=cfg->entryMap( "Font Substitutions" );
    EntryMap::Iterator fsub;
    for ( fsub=fsubs.begin(); fsub != fsubs.end(); fsub++ )
        QFont::insertSubstitution( fsub.key(), fsub.data() );
#endif
#if defined(PARSE_CMDLINE)
    // ugly: we parse the arguments twice
    for ( int i = 1; i < argc; i++ )
    {
        if ( argv[i] == NULL)
            continue; /* avoid SIGSEGV :( */
	QString arg( argv[i] );
	if ( arg.startsWith( "--sysfont:" ) )
	{
	    QFont newfont;
	    QStringList strlist( QStringList::split(':', arg, TRUE) );
	    for ( unsigned int j = 1; j < strlist.count(); j++ )
	    {
		QString argn=strlist[j];
		if ( argn != "" )
		{
		    int parn = argn.toInt();
		    switch ( j )
		    {
			case 1:
			    newfont.setFamily( argn );
			    break;
			case 2:
			    if ( parn )
				newfont.setPointSize( parn );
			    break;
			case 3:
			    if ( parn )
				newfont.setPixelSize( parn );
			    break;
			case 4:
			    if ( parn )
				newfont.setWeight( parn );
			    break;
		    }
		}
	    }
	    app.setFont( newfont, TRUE );
	}
	else if ( arg.startsWith( "--statushide:" ) )
	{
	    QStringList strlist( QStringList::split(':', arg, TRUE) );
	    bool ok;
	    int newdelay = strlist[1].toInt( &ok );
	    if ( ok )
		MainWindowBase::s_statusDelay=newdelay;
	}
	else if ( arg.startsWith( "--windowed" ) )
	{
	    MainWindowBase::s_winStyle=MainWindowBase::STYLE_WINDOWED;
	    QStringList strlist( QStringList::split(':', arg, TRUE) );
	    if ( strlist.count() == 5 )
		MainWindowBase::s_initGeometry = new QRect(
		    strlist[1].toInt(), strlist[2].toInt(),
		    strlist[3].toInt(), strlist[4].toInt() );
	}
	else if ( arg == "--maximized" )
	    MainWindowBase::s_winStyle=MainWindowBase::STYLE_MAXIMIZED;
	else if ( arg == "--fullscreen" )
	    MainWindowBase::s_winStyle=MainWindowBase::STYLE_FULLSCREEN;
	else if ( arg == "--small-images" )
	    MainWindowBase::s_bigPixmaps=FALSE;
	else if ( arg == "--big-images" )
	    MainWindowBase::s_bigPixmaps=TRUE;
	else if ( arg == "--warn-close" )
	    MainWindowBase::s_warnClose=TRUE;
#if !defined(Q_WS_WIN)
	else if ( arg.startsWith( "--pidfile:" ) )
	{
	    if ( arg.length() > 10 )
	    {
		QFile f( arg.mid( 10 ) );
		if ( f.open( IO_WriteOnly | IO_Truncate ) )
		{
		    char pid[24]; // it is enough even for 64-bit machines
		    sprintf( pid, "%ld\n", (long)::getpid() );
		    f.writeBlock( pid, qstrlen( pid ) );
		    f.close();
		}
	    }
	}
#endif
    }
#endif // PARSE_CMDLINE

#if !defined(Q_WS_WIN)
    KApplication::setService( "http_cache_cleaner.desktop", (void *)&http_cache_cleaner_main );
#endif

#ifndef QT_NO_TRANSLATION
    char *lang = getenv( "LANG" );
    if ( lang )
    {
        QString qmFile = QString::fromLatin1( "konqueror_%1.qm" ).arg( QString::fromLatin1( lang ) );

        // locate the message file in $prefix/share/locale/
        qmFile = locate( "locale", qmFile );

        QTranslator *translator = new QTranslator( &app );
        if ( translator->load( qmFile ) )
            app.installTranslator( translator );
        else
            delete translator;
    }
#endif

    // this call is not explicitly necessary, but we do it at this early
    // stage because that makes the launcher process more lightweight
    KIO::Launcher::startLauncherProcess();
    addons_init();
    KIO::MimeHandler::Initialize();

#ifdef _QT_QPE_
    QString path = QPEApplication::qpeDir() + "/plugins/konqplugins";
    QDir dir( path, "lib*.so" );
    QStringList list = dir.entryList();
    QStringList::ConstIterator it = list.begin();
    for (; it != list.end(); ++it )
    {
        KonqPluginInterface *iface = 0;
        // ### known leak
        QLibrary *lib = new QLibrary( path + "/" + *it );    
        if ( lib->queryInterface( IID_KonqPluginInterface, 
                                  reinterpret_cast<QUnknownInterface **>( &iface ) ) == QS_OK )
        {
            iface->setup();
            // iface->release(); ... whatever.. we don't ever release the stuff anyway
        }
    }
#endif

#if defined(_WS_QWS_) && defined(QT_NO_QWS_CURSOR)
    (void)new PopupHandler( &app, "popup-handler" );
#endif

    KAuthCacheServer authCache;
    authCache.setObjId( "authcache" );

    KCookieServer cookieEater("kcookiejar");
    cookieEater.setObjId( "kcookiejar" );

#if defined(HAVE_SSL)
    KSSLD kssld( "kssld" );
    kssld.setObjId( "kssld" );
    UIServer uiserver;
    uiserver.setObjId( "kio_uiserver" );
#endif

#if defined(ENABLE_JAVASCRIPT) && !defined(DLOPEN_JAVASCRIPT)
    KLibLoader::self()->library( "kjs_html", KLibLoader::CreateStaticWrapper )->registerSymbol( "kjs_html_init", (void *)&kjs_html_init );
#endif
//    KLibLoader::self()->library( "libkhtml" )->registerSymbol( "init_libkhtml", (void *)&init_libkhtml );

    KIconLoader::self()->registerIcon( "file_broken", QPixmap( ( const char ** )file_broken_xpm ) );

    QObject::connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );

    MainWindowBase *toplevel = MainWindowBase::instantiate();

    bool openedMainWindow = false;

#if defined(_QT_QPE_)
    app.showMainDocumentWidget( toplevel );

    openedMainWindow = ( argc == 3 ) && !strcmp( "-qcop", argv[ 1 ] ) && 
	               ( toplevel->currentView() != 0 );
#else
    toplevel->show();

    app.setMainWidget( toplevel );

    for ( int i = 1; i < argc; i++ )
    {
       if ( argv[i] && *argv[i] && ( *argv[i] != '-' ) )
       {
           KURL u = QString::fromLatin1( QCString( argv[ i ] ) );
           if ( !u.isMalformed() && !u.isEmpty() )
           {
               toplevel->createNewView( u );
               openedMainWindow = true;
           }
       }
    }
#endif

    if ( !openedMainWindow )
    {
        QString startDocumentPath = locate( "data", "konq-embed/start.html" );
        if ( !startDocumentPath.isEmpty() )
            toplevel->createNewView( startDocumentPath.prepend( "file:" ) );
        else
            toplevel->createNewView( KURL() );
    }

    int res = app.exec();

    KIO::Launcher::self()->stopLauncherProcess();

    return res;
}
