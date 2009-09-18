#ifndef __jsextension_h__
#define __jsextension_h__

#if defined(ENABLE_JS_HOSTEXTEND)

#include <kjs/object.h>
#include <kjs/interpreter.h>
#include <kparts/part.h>
#include <qstring.h>

typedef void (*JsHostExtend)(KJS::ExecState *exec, KParts::ReadOnlyPart *part,
	const KURL &url, const QString &mimetype, KJS::Object &hostobj );

namespace KJS {

    void addJsExtension( const JsHostExtend ext );

};

#endif // ENABLE_JS_HOSTEXTEND

#endif // __jsextension_h__
