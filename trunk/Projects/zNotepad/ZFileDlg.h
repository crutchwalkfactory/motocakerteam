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
#ifndef ZFILEDLG_H
#define ZFILEDLG_H

#include "BaseDlg.h"
#include <ZMultiLineEdit.h>
#include <ZMessageDlg.h>
#include <ZOptionsMenu.h>
#include <ZKbMainWidget.h>
#include <ZListBox.h>
#include <ZPressButton.h>
#include <ZCheckBox.h>


class ZMyCheckBox : public ZCheckBox {
  Q_OBJECT
  
  public:
    ZMyCheckBox( const QString & text, QWidget * parent, const char* name = 0, bool value = false);
    ~ZMyCheckBox();
    QString ParamName;
    QString ParamGroup;
    
    bool GetChecked() const;
    
  public slots:
    void SetChecked(bool state);
    
  private:
    bool curState; 
};


class ZSearch : public MyBaseDlg {
  Q_OBJECT

  public:
    ZSearch(int nota); 
    ~ZSearch();
	QString texto; 
    ZMyCheckBox *marcado;
   
  public slots:   
    void slot_cancel();
    void slot_save();
	void checktext();
    
  private:
    ZListBox* browser;
	ZMultiLineEdit* lineEdit;  
	ZSoftKey *softKey;
	ZOptionsMenu * menu1;

  protected:
	//virtual void keyPressEvent(QKeyEvent* e);
 
};


class ZView : public MyBaseDlg {
  Q_OBJECT

  public:
    ZView(int nota); 
    ~ZView();
	ZLabel* lineEdit;
	ZLabel* lineM2;
   
  public slots:   
    void slot_edit();
    void slot_delete();
    void slot_send();
    
  private:
	ZSoftKey *softKey;
	ZOptionsMenu * menu1;

  protected:
	//virtual void keyPressEvent(QKeyEvent* e);
 
};


#endif
