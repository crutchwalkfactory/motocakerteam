//
// C++ Interface: ZFileDlg
//
// Description: 
//
//
// Author: root <root@andLinux>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MYPOPUP_H
#define MYPOPUP_H

#include <ZListBox.h>

#include <qtextstream.h>
#include <qobject.h>
#include <stdio.h>
#include <stdlib.h>
#include <qfile.h>
#include <qdir.h>
#include <ZConfig.h>
#include <ZApplication.h>
#include <ZMessageDlg.h>
#include <ZNoticeDlg.h>
#include <ZPopup.h>
#include <ZIconView.h>
#include <ZPressButton.h>
#include <qmap.h>
#include <qlist.h>
#include <qstringlist.h>
#include <qdir.h>
#include <qfile.h>
#include <sys/vfs.h>


class myListBox : public ZListBox {
  Q_OBJECT

  public:
      myListBox( QString type, QWidget* parent=0, WFlags f = 0,
                  ZSkinService::WidgetClsID clsId = ZSkinService::clsZListBox1 );
	  virtual ~myListBox();

  protected:
	virtual void keyPressEvent(QKeyEvent* k);

};


#endif

