
#include "kwin.h"

#ifdef _WS_QWS_
#include <qwsmanager_qws.h>
#endif

class HackWidget : public QWidget
{
public:
    using QWidget::topData;
};

KWin::Info KWin::info( WId id )
{
    KWin::Info res;

    QWidget *w = QWidget::find( id );
    if ( !w )
        return res;
 
#ifdef _WS_QWS_
#ifndef QT_NO_QWS_MANAGER
    QWSManager *manager = static_cast<HackWidget *>( w )->topData()->qwsManager;
    if ( manager )
    {
        res.geometry = manager->region().boundingRect();
        return res;
    }
#endif    
#endif

    // ###
    res.geometry = w->geometry(); 
    return res;
}

