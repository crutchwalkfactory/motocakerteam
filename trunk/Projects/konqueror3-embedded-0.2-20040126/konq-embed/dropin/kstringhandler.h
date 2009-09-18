#ifndef __kstringhandler_h__
#define __kstringhandler_h__

#include <qstring.h>

class KStringHandler
{
public:
    static QString csqueeze( const QString &s, int = 0 ) { return s; }

    // needed by kdelibs 3.1
    static QString rsqueeze( const QString &s, int = 0 ) { return s; }
};

#endif
/*
 * vim:et
 */
