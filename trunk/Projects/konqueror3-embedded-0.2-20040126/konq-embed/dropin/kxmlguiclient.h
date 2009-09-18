#ifndef __kxmlguiclient_h__
#define __kxmlguiclient_h__

#include <qstring.h>
#if QT_VERSION < 300
#include <qlist.h>
#else
#include <qptrlist.h>
#endif
#include <qdom.h>

#include <kaction.h>
#include <assert.h>

class KXMLGUIFactory;

class KXMLGUIClient
{
public:
    KXMLGUIClient() {}
    virtual ~KXMLGUIClient() {}

    void setXMLFile( const QString & ) {}

    KActionCollection *actionCollection() { return &m_collection; }

#if QT_VERSION < 300    
    void plugActionList( const QString &, const QList<KAction> & ) {}
#else    
    void plugActionList( const QString &, const QPtrList<KAction> & ) {}
#endif    

    void unplugActionList( const QString & ) {}

    void setInstance( KInstance * ) {}

    void setXML( const QString & ) {}

    // Needed to compile kdelibs >= 3.1.2
    KXMLGUIFactory *factory() const { return 0; }
    
    // Needed to compile kdelibs >= 3.1.2
    void insertChildClient( KXMLGUIClient *child ) { assert(false); }
    
    // Needed to compile kdelibs >= 3.1.2
    void removeChildClient( KXMLGUIClient *child ) { assert(false); }
    
private:
    KActionCollection m_collection;
};

#endif
