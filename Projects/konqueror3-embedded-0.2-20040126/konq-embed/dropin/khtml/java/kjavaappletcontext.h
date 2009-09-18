#ifndef __kjavaappletcontext_h__
#define __kjavaappletcontext_h__

#include <qobject.h>
#include <dcopobject.h>

typedef
enum { JError=0, JBoolean, JFunction, JNumber, JObject, JString, JVoid } JType;

class KJavaAppletContext : public QObject
{
public:
    KJavaAppletContext(DCOPObject * = NULL) {};

    int  contextId() const { return 0; }
    
    bool appletsLoaded() const { return false; }

};

#endif
