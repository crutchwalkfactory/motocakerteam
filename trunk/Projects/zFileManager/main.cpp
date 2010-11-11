#include <ZApplication.h>
#include "launcher.h"
#include "qfile.h"

ZLauncher * launcher;

int main(int argc, char** argv)
{
	ZApplication * pApp = new ZApplication( argc, argv );
	launcher = new ZLauncher( argv[1], NULL, NULL );
	pApp->setMainWidget(launcher); 
	launcher->show();
	int iRet = pApp->exec();
	delete launcher;
	delete pApp;
	return iRet;
}


