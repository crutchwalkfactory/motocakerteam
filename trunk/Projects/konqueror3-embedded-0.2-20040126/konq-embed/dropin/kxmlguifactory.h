#ifndef __kxmlguifactory_h__
#define __kxmlguifactory_h__

class KInstance;
class KXMLGUIClient;

class KXMLGUIFactory
{
public:

    static QString readConfigFile( const QString &, KInstance * = 0 ) { return QString::null; }

    // Needed by kdelibs >= 3.1.2
    void removeClient( KXMLGUIClient *client ) {}
    
    // Needed by kdelibs >= 3.1.2
    void addClient( KXMLGUIClient *client ) {}

};

#endif
