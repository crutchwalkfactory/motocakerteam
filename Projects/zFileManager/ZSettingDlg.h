//
// C++ Interface: pe_options
//
// Description: 
//
//
// Author: root <root@andLinux>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ZSETTINGDLG_H
#define ZSETTINGDLG_H

#include "BaseDlg.h"
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

class ZAppSetting : public MyBaseDlg {
  Q_OBJECT

  public:
    ZAppSetting();
    ~ZAppSetting();
    
    ZMyCheckBox *zcbFiltrProc;
    ZMyCheckBox *zcbTaskNoWin;
    
    ZMyCheckBox *zcbJavaIcon;
    ZMyCheckBox *zcbNativIcon;
    ZMyCheckBox *zcbRunAppIcon;
    
    ZMyCheckBox *zcbSendReasePhone;
    ZMyCheckBox *zcbSendGoToIDLE;
                 
    bool SingleRegistry;
    bool AppSecondLine;
    bool SkinSecondLine;
    bool JavaLockIcon;
    bool AppStorageIcon;   
  public slots:
    virtual void accept();
    virtual void reject();
};

#endif
