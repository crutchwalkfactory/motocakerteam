//
// C++ Implementation: BaseDlg
//
// Description: 
//
//
// Author: root <root@andLinux>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "BaseDlg.h"
#include <ZApplication.h>
#include <ZSoftKey.h>

MyBaseDlg::MyBaseDlg()
  :ZKbMainWidget( ZHeader::FULL_TYPE, NULL, "ZMainWidget", WType_Modal | WType_TopLevel  )
{
  myInLoop = false;
  
  ZSoftKey *softKey = new ZSoftKey(NULL, this, this);
  softKey->setText(ZSoftKey::LEFT, "Aceptar", (ZSoftKey::TEXT_PRIORITY)0);
  softKey->setText(ZSoftKey::RIGHT, "Cancelar", (ZSoftKey::TEXT_PRIORITY)0);
  softKey->setClickedSlot(ZSoftKey::RIGHT, this, SLOT(reject()));
  softKey->setClickedSlot(ZSoftKey::LEFT, this, SLOT(accept()));
  setCSTWidget(softKey);

}

MyBaseDlg::~MyBaseDlg()
{

}

int MyBaseDlg::exec() {
  setResult(0);
  show();
  myInLoop = TRUE;
  qApp->enter_loop();
  return result();
}

void MyBaseDlg::done( int r )
{
  hide();
  if (myInLoop) {
    qApp->exit_loop();
  }
  setResult(r);
  if ( qApp->mainWidget() == this )
    qApp->quit();
}

void MyBaseDlg::accept() {
  done(Accepted);
}

void MyBaseDlg::reject() {
  done(Rejected);
}

