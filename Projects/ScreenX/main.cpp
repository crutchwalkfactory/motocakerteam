#include "ScreenX.h"
#include <ZApplication.h>
#include <qpixmap.h>
#include <ZConfig.h>

ScreenX *screenx;
ZApplication* app;

int main ( int argc, char **argv )
{
  app = new ZApplication ( argc, argv );
  screenx = new ScreenX ( NULL, NULL );

	QString Fval1; ZConfig Fini1(QString("/ezxlocal/download/appwrite/setup/ezx_theme.cfg"), false);
	Fval1 = Fini1.readEntry(QString("THEME_GROUP_LOG"), QString("CurrentTheme"), "");
	QString Fval2; ZConfig Fini2(Fval1, false);
	Fval2 = Fini2.readEntry(QString("THEME_CONFIG_TABLE"), QString("WallPaper"), "");

	QPixmap f1; f1.load(Fval2);
	app->setAppFSBA( f1, 100, 0 );


  int ret = app->exec();
  delete screenx;
  screenx = NULL;
  delete app;
  app = NULL;
  return ret;
}
