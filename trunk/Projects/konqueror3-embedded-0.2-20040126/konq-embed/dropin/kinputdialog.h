#ifndef __kinputdialog_h__
#define __kinputdialog_h__

#include <qfeatures.h>

#include <qstring.h>

#ifdef QT_NO_INPUTDIALOG

class QInputDialog
{
public:
    static QString getText( const QString &caption, const QString &label,
                            const QString &text = QString::null );
    static QString getText( const QString &caption, const QString &label, int,
                            const QString &text = QString::null )
        { return getText(caption, label, text); }

};

#endif

#endif

