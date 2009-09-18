#ifndef __kurldrag_h__
#define __kurldrag_h__

#include <kurl.h>
#include <qdragobject.h>

class QWidget;
class QUriDrag;

class KURLDrag : public QUriDrag
{
    QMap<QString, QString> m;
public:

    QMap<QString, QString>& metaData() { return m; }

    static KURLDrag *newDrag( const KURL::List &, QWidget *, const char * = 0 )
        { return 0; }

    static bool decode( const QMimeSource *, KURL::List &)
    	{ return false; }
     
};

#endif
