#ifndef __kurifilter_h__
#define __kurifilter_h__ 

#include <kurl.h>

class KURIFilterData
{
public:
    enum URITypes { 
	    NET_PROTOCOL=0, 
	    LOCAL_FILE, 
	    LOCAL_DIR, 
	    EXECUTABLE, 
	    HELP, 
	    SHELL, 
	    BLOCKED, 
	    ERROR, 
	    UNKNOWN 
    };

    KURIFilterData() {}
    void setData(const QString& url) { m_pURI = url; }
    void setData(KURL &uri) { m_pURI = uri; }
    URITypes uriType() const { return UNKNOWN; }
    KURL uri() const { return m_pURI; }

private:
    KURL m_pURI; 
};

class KURIFilter
{
public:
    // HACK!!! This could break stuff.  It works for now, since it is used as in
    // KURIFilter::self()->filterURI(...) which is probably optimized to false.
    static KURIFilter* self() { return 0; }

    bool filterURI(KURIFilterData& /*data*/) const { return false; }
};

#endif

