//
// C++ Implementation: ZFileDlg
//
// Description: 
//
//
// Author: root <root@andLinux>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "myPopup.h"
#include "resources.h"
#include "ZFileDlg.h"
#include <qpixmap.h>
#include <qlabel.h>
#include <ZApplication.h>
#include <ZSoftKey.h>
#include <ZConfig.h>
#include <string.h>
#include <ZSingleCaptureDlg.h>
#include <ZScrollView.h>
#include <ZMessageDlg.h>
#include <ZListBox.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <qfile.h>
#include <qdir.h>
#include <qlist.h>
#include <string.h>
#include <qtextstream.h>
#include <qvector.h>
#include <qtextcodec.h>
#include <ezxres.h>
#include "launcher.h"

QString lang;

extern ZLauncher * launcher;
extern ZSearch * search;

myListBox::myListBox(QString type, QWidget* parent, WFlags f, ZSkinService::WidgetClsID clsId)
:ZListBox( type, parent, 0 )
{
	


}


myListBox::~myListBox()
{

}

void myListBox::keyPressEvent(QKeyEvent* k)
{
  switch ( k->key() ) 
  {

	case Z6KEY_MUSIC:
	{ 
		launcher->prevPlay();
		break;
	}
	case Z6KEY_LEFT:
	{ 
		launcher->prevStop();
		break;
	}
	case Z6KEY_GREEN:
	{ 
		launcher->Settings();
		break;
	}
	case Z6KEY_C:
	{ 
		launcher->slot_delete();
		break;
	}
	case Z6KEY_SIDE_UP:
	{ 
		launcher->slot_clipboard();
		launcher->checkClip();
		launcher->checkPlayer();
		break;
	}
	case Z6KEY_SIDE_SELECT:
	{ 
		ZAdminFav *zfod = new ZAdminFav();
		if ( zfod->exec() )
		{
			QString fDestPath = zfod->getFileName(); 
			int i = fDestPath.findRev ( "//" ); fDestPath.remove ( i, 1 );
			if ( ! fDestPath.isEmpty() )
			{
				launcher->CargarBrowser(fDestPath);
			}
		}
		delete zfod;
		zfod = NULL;
		break;
	}
	case Z6KEY_SIDE_DOWN:
	{ 
		ZClipboard *zc = new ZClipboard();
		zc->exec();
		launcher->checkClip();
		launcher->checkPlayer();
		delete zc;
		zc = NULL;
		break;
	}
    case Z6KEY_RED:
    { 
		qApp->quit();
	    break; 
    }
    case Z6KEY_1:
    { 
		launcher->openText();
		break;
    }
    default:
      ZListBox::keyPressEvent(k);
  }  
  //launcher->verMiniaturas();
  


}











myIconView::myIconView( QWidget *parent, const char *name, WFlags, int initCount, int initStep, const ZSkinService::WidgetClsID clsId )
:ZIconView( parent, 0 )
{
	


}

myIconView::~myIconView()
{

}

void myIconView::keyPressEvent(QKeyEvent* k)
{
  switch ( k->key() ) 
  {

	case Z6KEY_GREEN:
	{ 
		launcher->Settings();
		break;
	}
	case Z6KEY_C:
	{ 
		launcher->slot_delete();
		break;
	}
	case Z6KEY_SIDE_UP:
	{ 
		launcher->slot_clipboard();
		launcher->checkClip();
		launcher->checkPlayer();
		break;
	}
	case Z6KEY_MUSIC:
	{ 
		launcher->prevPlay();
		break;
	}
	case Z6KEY_SIDE_SELECT:
	{ 
		ZAdminFav *zfod = new ZAdminFav();
		if ( zfod->exec() )
		{
			QString fDestPath = zfod->getFileName(); 
			int i = fDestPath.findRev ( "//" ); fDestPath.remove ( i, 1 );
			if ( ! fDestPath.isEmpty() )
			{
				launcher->CargarBrowser(fDestPath);
			}
		}
		delete zfod;
		zfod = NULL;
		break;
	}
	case Z6KEY_SIDE_DOWN:
	{ 
		ZClipboard *zc = new ZClipboard();
		zc->exec();
		launcher->checkClip();
		launcher->checkPlayer();
		delete zc;
		zc = NULL;
		break;
	}
    case Z6KEY_RED:
    { 
		qApp->quit();
	    break; 
    }
    case Z6KEY_1:
    { 
		launcher->openText();
		break;
    }
    default:
      ZIconView::keyPressEvent(k);
  }  
  //launcher->verMiniaturas();
 


}



