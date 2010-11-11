#include <qcopchannel_qws.h>
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

	int ret = app->exec();
	delete screenx;
	screenx = NULL;
	delete app;
	app = NULL;
	return ret;
}
