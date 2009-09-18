#ifndef __kfileitem_h__
#define __kfileitem_h__

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <global.h>
#include <kmimetype.h>
#include <qstylesheet.h>

// ### ;-)
#if 0 // This does not work here - ML
namespace KIO
{
    typedef struct stat UDSEntry;
};
#endif


class KFileItem
{
public:
    // needed by kdelibs 3.1
    enum { Unknown = (mode_t) - 1 };
    
    KFileItem( KIO::UDSEntry udsEntry, const KURL &, bool, bool )
        { m_entry = udsEntry; }

    // Needed by kdelibs 3.1
    KFileItem( const KURL &, const QString &, mode_t ) 
        { }
    
    //I don't care about files... at least for now. Check later XXX - ML
    //bool isDir() const { return S_ISDIR( m_entry.st_mode ); }
    bool isDir() const { return false; }
    bool isFile() const { return true; }

private:
    KIO::UDSEntry m_entry;
};

#endif
