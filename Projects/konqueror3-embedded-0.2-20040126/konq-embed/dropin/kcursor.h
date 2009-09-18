#ifndef __kcursor_h__
#define __kcursor_h__

#include <qnamespace.h>
#include <qcursor.h>

// ### compat
#include <kglobalsettings.h>

class QWidget;

class KCursor
{
public:

    static QCursor handCursor()
        { return QCursor( Qt::PointingHandCursor ); }

    // Needed for kdelibs 3.1 codebase, check if there is a better cursor pixmap
    static QCursor workingCursor() 
        { return Qt::arrowCursor; }

    static QCursor arrowCursor()
        { return Qt::arrowCursor; }

    static QCursor upArrowCursor()
        { return Qt::upArrowCursor; }

    static QCursor crossCursor()
        { return Qt::crossCursor; }

    static QCursor sizeAllCursor()
        { return Qt::sizeAllCursor; }

    static QCursor sizeHorCursor()
        { return Qt::sizeHorCursor; }

    static QCursor sizeVerCursor()
        { return Qt::sizeVerCursor; }

    static QCursor sizeBDiagCursor()
        { return Qt::sizeBDiagCursor; }

    static QCursor sizeFDiagCursor()
        { return Qt::sizeFDiagCursor; }

    static QCursor ibeamCursor()
        { return Qt::ibeamCursor; }

    static QCursor waitCursor()
        { return Qt::waitCursor; }

    static QCursor whatsThisCursor()
        { return Qt::whatsThisCursor; }

    static void setAutoHideCursor( QWidget *, bool ) {}
};

#endif
