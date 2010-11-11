//
// C++ Interface: ZMultiLineEditEx
//
// Description: 
//
//
// Author: root <root@andLinux>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ZMULTILINEEDITEX_H
#define ZMULTILINEEDITEX_H

#include <ZMultiLineEdit.h>
#include <ZMessageDlg.h>
#include <ZApplication.h>
#include "resources.h"

class ZMultiLineEditEx : public ZMultiLineEdit {
  Q_OBJECT
      
  public:
    ZMultiLineEditEx(QWidget *parent,bool EZXStyle,int lines = 1,const char *name=0 );
    ~ZMultiLineEditEx();    
      
    void MarkBegin();
    void MarkEnd();
    void atBegin();
    void atEnd();
    void BlockCopy();
    void BlockCut();
    void BlockPaste();
            
    bool isMarked;
    
  private:
    int m_iMarkStartX;
    int m_iMarkStartY;
      
  protected:
    virtual void keyPressEvent(QKeyEvent* e);    
    
};
      
#endif
