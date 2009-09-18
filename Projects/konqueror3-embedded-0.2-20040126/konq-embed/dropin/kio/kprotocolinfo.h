#ifndef __kprotocolinfo_h__
#define __kprotocolinfo_h__

#include <kurl.h>
/* Needed by kdelibs 3.2 */

class KProtocolInfo 
{
public:
    static KURL::URIMode uriParseMode(const QString& protocol) { return KURL::Auto; }
};

#endif 
