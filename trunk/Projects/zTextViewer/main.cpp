#include <ZApplication.h>
#include "texteditor.h"
#include "qfile.h"
#include "BuildConfig.h"

TextEditor * pTextEditor;

int main(int argc, char** argv)
{
	ZApplication * pApp = new ZApplication( argc, argv );

	pTextEditor = new TextEditor( NULL, NULL );
	
  pApp->setMainWidget(pTextEditor); 
	pTextEditor->show();
	
	int iRet = pApp->exec();

	delete pTextEditor;
	delete pApp;

	return iRet;
}


