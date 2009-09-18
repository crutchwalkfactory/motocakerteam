#include <config.h>

#ifndef ENABLE_JS_HOSTEXTEND
#error You MUST configure with --enable-jshostext to use JS Debug Output
#endif

#include <jsextension.h>

#include <kjs/object.h>
#include <kjs/interpreter.h>
#include <kjs/function.h>

#include <stdio.h>

using namespace KJS;

#if 1 // ANSI Supported?
static const char header[] = "\033[1;37m\033[43m\033[L"; // bold white on red
static const char footer[] = "\033[0m\033[L"; // reset attr & erase to EOL
#else
static const char header[] =
"--------------------------------------------------- Javascript DebugOut ---\n";
#define footer header
#endif

class JsDebug : public ObjectImp
{
public:
    JsDebug() : ObjectImp() {}
    virtual bool implementsCall() const { return true; }
    virtual Value call(ExecState *exec, Object &thisObj, const List &args );
};

Value JsDebug::call(ExecState *exec, Object & /*thisObject*/, const List &args)
{
    QString out( header );
    for ( int i = 0; i < args.size(); i++ )
    {
	out.append( args[i].toString(exec).qstring() );
	out.append( "\n" );
    }
    out.append( footer );
    fputs( out.latin1(), stderr );
    return Completion( ReturnValue, Undefined() );
}

// One debug output is plenty enough for all interpreters
// As an added bonus it won't leak any more memory ;)
static JsDebug *s_debug = 0;

static void JsDebugExtension(ExecState *exec, KParts::ReadOnlyPart *part, 
	const KURL &, const QString &, KJS::Object &hostobj )
{
    if ( !part->inherits( "KHTMLPart" ) )
	return;

    if ( !s_debug )
    {
	s_debug = new JsDebug;
	s_debug->ref();
    }

    hostobj.put(exec, "DebugOut", Object(s_debug));
}

extern "C" void konqe_register_libjsdebug()
{
    KJS::addJsExtension( &JsDebugExtension );
}
