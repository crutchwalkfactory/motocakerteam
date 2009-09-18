#include <config.h>

#if defined(ENABLE_JS_HOSTEXTEND)

#include "jsextension.h"
#include "jsextconnect.h"

#include <kjs/object.h>
#include <kjs/types.h>
#include <kjs/interpreter.h>

#include <qlist.h>

static QPtrList<void> js_extensions;

namespace KJS {

    void addJsExtension( const JsHostExtend ext )
    {
	js_extensions.append( (void *)ext );
    }

    void probeJsExtension(ExecState* exec, 
	    KParts::ReadOnlyPart *part, const KURL &url, 
	    const QString &mimetype, ObjectImp *imp )
    {
	QListIterator<void> it( js_extensions );

	KJS::Object hostobj(imp);
	for ( ; it.current(); ++it )
	    ( (JsHostExtend)(it.current()) ) (exec, part, url, mimetype, hostobj );
    }

};

#endif // ENABLE_JS_HOSTEXTEND
