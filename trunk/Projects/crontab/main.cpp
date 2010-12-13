//
// C++ Implementation: TestMain
//
// Description:
//
//
// Author: BeZ <bezols@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "zgui.h"
//#include "GUI_Define.h"
#include <ZApplication.h>

ZGui* zgui;
ZApplication* app;

int main ( int argc, char **argv )
{
  app = new ZApplication ( argc, argv );
  ZGui *zgui = new ZGui ( argv[1], NULL, NULL );
  int ret = app->exec();
  delete zgui;
  zgui = NULL;
  delete app;
  app = NULL;
  return ret;
}
