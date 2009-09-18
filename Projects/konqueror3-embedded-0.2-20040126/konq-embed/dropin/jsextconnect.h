#ifndef __jsextplugger_h__
#define __jsextplugger_h__

#if defined(ENABLE_JS_HOSTEXTEND)

#include <kparts/part.h>
#include <kjs/object.h>
#include <kjs/interpreter.h>
#include <qstring.h>

namespace KJS {

    class Imp;

    void probeJsExtension(ExecState *exec, KParts::ReadOnlyPart *part, const KURL &url,
			   const QString &mimetype, KJS::ObjectImp *imp = 0 );

};

#endif // ENABLE_JS_HOSTEXTEND

#endif // __jsextplugger_h__
