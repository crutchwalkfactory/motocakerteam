#ifndef __kjavaapplet_h__
#define __kjavaapplet_h__

#include <qstring.h>
#include <kjavaappletcontext.h>

namespace KParts {
class LiveConnectExtension;
}

class KJavaApplet
{
public:
    KJavaApplet() {}

    void create() {};

    void setParameter( const QString &, const QString & ) {}
    
    // Needed by kdelibs 3.1
    KParts::LiveConnectExtension* getLiveConnectExtension() { return 0; }

    void setBaseURL( const QString & ) {}

    void setAppletClass( const QString & ) {}

    void setCodeBase( const QString & ) {}

    void setAppletName( const QString & ) {}

    void setJARFile( const QString & ) {}

    void setArchives( const QString & ) {}
    
    // Needed by kdelibs 3.1
    int  appletId() { return 0; }
    
    // Needed by kdelibs 3.1
    KJavaAppletContext* getContext() const { return 0; }

    // Needed by kdelibs 3 
    bool getMember(const QString & name, JType & type, QString & value) 
    { return false; }

    bool callMember(const QString &, const QStringList &, JType &, QString &) 
    { return false; }
};

#endif
