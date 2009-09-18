#ifndef __kapplication_h__
#define __kapplication_h__

#include <qapplication.h>

#include <stdlib.h>

#include <kinstance.h>

#define kapp KApplication::self()

class DCOPClient;

#ifdef _QT_QPE_
#include <qpeapplication.h>
#define QApplication QPEApplication
#endif

#include <kurl.h>

#if defined( Q_WS_QWS ) || !defined(KDE_VERSION_STRING) 
#define KDE_VERSION_STRING "2.9"
#endif

// nasty hack: khtmlview needs QPrinter. In case Qt is compiled without
// QPrinter (makes no sense on a PDA!) we provide a very dummy QPrinter
// replacement and include it here, as khtmlview.cpp certainly includes
// kapp.h
#ifdef QT_NO_PRINTER
#include <kdummyprinter.h>
#endif

class KApplication : public QApplication, public KInstance
{
    Q_OBJECT

#undef QApplication

public:
    KApplication( int argc, char **argv, const char *name );
    virtual ~KApplication();

    static KApplication *self() { return s_self; }
    static KApplication *kApplication() { return self(); } // for render_form.cpp

    // dummy for tcpslavebase.cpp
    static bool startServiceByDesktopPath( const QString &, const QStringList & ) { return false; }

    // Needed by KHTML 3.1 
    bool authorizeURLAction(const QString &, const KURL &, const KURL &) { 
	    return true; 
    }
    
    // ### extension
    static void setService( const QString &desktopPath, void *entryPoint );

    static bool startServiceByDesktopPath( const QString &path );

    static int startServiceByDesktopName( const QString&, const QStringList& =QStringList(),
                                          QString* =0, QCString* =0, int* = 0, const QCString& = "", bool = false )
        {
            return 0;
        }


    DCOPClient *dcopClient() const { return m_dcopClient; }

    void invokeBrowser( const QString & ) {}
    void invokeMailer( const QString & ) {}

    static int random() { return rand(); } // ### ;-)
    
    // Needed by kdelibs >= 3.1.2
    static QString randomString(int length);

signals:
    // required by KHTMLView
    void kdisplayPaletteChanged();

private:
    static KApplication *s_self;

    DCOPClient *m_dcopClient;

    static QString *s_serviceName;
    static void *s_serviceEntryPoint;
};

// ### FIXME: checkAccess copyright by Kalle!

bool checkAccess( const QString &pathname, int mode);

#endif
