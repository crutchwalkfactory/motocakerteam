#ifndef __kjavaappletwidget_h__
#define __kjavaappletwidget_h__

#include <qwidget.h>

#include <kjavaapplet.h>

class KJavaAppletContext;

class KJavaAppletWidget : public QWidget
{
public:
    KJavaAppletWidget( KJavaAppletContext *, QWidget *parent, const char *name = 0 )
        : QWidget( parent, name )
    {}

    KJavaApplet *applet() const { return 0; }

    void showApplet() {}
};

#endif
