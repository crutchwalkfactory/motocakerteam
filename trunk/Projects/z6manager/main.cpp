
#include "z6manager.h"
//#include "GUI_Define.h"
#include <ZApplication.h>

Z6Man* z6man;
ZApplication* app;

int main ( int argc, char **argv )
{
  app = new ZApplication ( argc, argv );
  Z6Man *z6man = new Z6Man (argv[2], NULL, NULL );
  int ret = app->exec();
  delete z6man;
  z6man = NULL;
  delete app;
  app = NULL;
  return ret;
}
