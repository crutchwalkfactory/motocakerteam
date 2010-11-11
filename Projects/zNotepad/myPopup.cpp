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
#include "zgui.h"

#define Z6KEY_SIDE_SELECT       0x1005
#define Z6KEY_UP                0x1013
#define Z6KEY_DOWN              0x1015

QString lang;

extern ZGui * zgui;

myListBox::myListBox(QString type, QWidget* parent, WFlags f, ZSkinService::WidgetClsID clsId)
:ZListBox( type, parent, 0 )
{
	


}


myListBox::~myListBox()
{

}

void myListBox::keyPressEvent(QKeyEvent* k)
{
  
	if ( k->key() == Z6KEY_SIDE_SELECT )
	{ 
		zgui->marcar();
	}

	else if ( k->key() == Z6KEY_UP )
	{
		int index = currentItem();
		QString val = item(index)->getSubItemText(4,0,true);
		if ( val=="Yes" ) 
		{
		  if ( index > 0 ) moveItem(index-1,index);
		}
		else ZListBox::keyPressEvent(k);
	}

	else if ( k->key() == Z6KEY_DOWN )
	{
		int index = currentItem();
		QString val = item(index)->getSubItemText(4,0,true);
		if ( val=="Yes" ) 
		{
			if ( index < count() ) moveItem(index+1,index);
		}
		else ZListBox::keyPressEvent(k);
	}

    else { ZListBox::keyPressEvent(k); }

}












