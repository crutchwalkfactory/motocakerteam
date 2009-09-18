#ifndef __kstdguiitem_h
#define __kstdguiitem_h

#include <qstring.h>
#include <klocale.h>

#define KGuiItem QString
 
class KStdGuiItem 
{
public:
    enum StdItem
    {
        Ok=1, Cancel, Yes, No, Discard, Save, DontSave, SaveAs,
        Apply, Clear, Help, Defaults, Close, Back, Forward, Print,
        Continue
    };

    static KGuiItem cont() 
        { return i18n("Continue"); }
};

#endif

