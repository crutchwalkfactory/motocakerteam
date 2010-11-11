//
// C++ Implementation: pe_options
//
// Description: 
//
//
// Author: root <root@andLinux>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ZSettingDlg.h"
#include <ZApplication.h>
#include <ZSoftKey.h>
#include <ZAppInfoArea.h>
#include <ZFormContainer.h>
#include <ZConfig.h>
#include <ZSeparator.h>
#include "ZDefs.h"
#include "resources.h"
#include "launcher.h"

extern ZLauncher *ZLauncher;
extern ZApplication *app;

bool chang;

ZAppSetting::ZAppSetting()
{
	
	setMainWidgetTitle(QString::fromUtf8(APP_NAME));
	
	ZFormContainer *form = new ZFormContainer(this, 0, ZSkinService::clsZFormContainer);
	this->setContentWidget(form);

	QString appConf="";
	ZConfig cfg(appConf);
	
/*	zcbFiltrProc = new ZMyCheckBox(QString::fromUtf8(LNG_PROCFILTER),this,"",!cfg.readBoolEntry(QString("Task"), QString("FiltrProc"), false));
	zcbTaskNoWin = new ZMyCheckBox(QString::fromUtf8(LNG_TASKNOWIN),this,"",cfg.readBoolEntry(QString("Task"), QString("ShowAppNoWindow"), false));
	zcbJavaIcon = new ZMyCheckBox(QString::fromUtf8(LNG_JAVAICON),this,"",cfg.readBoolEntry(QString("Task"), QString("ShowJavaIcon"), false));
	zcbNativIcon = new ZMyCheckBox(QString::fromUtf8(LNG_NATIVICON),this,"",cfg.readBoolEntry(QString("Task"), QString("ShowNativIcon"), false));
	zcbSendReasePhone = new ZMyCheckBox(QString::fromUtf8(LNG_SENDREASEPHONE),this,"",cfg.readBoolEntry(QString("General"), QString("SendReaisePhone"), false));
	zcbSendGoToIDLE = new ZMyCheckBox(QString::fromUtf8(LNG_SENDTOIDLE),this,"",cfg.readBoolEntry(QString("General"), QString("SendGoToIDLE"), false));
	zcbRunAppIcon = new ZMyCheckBox(QString::fromUtf8(LNG_RUNICON),this,"",cfg.readBoolEntry(QString("Task"), QString("ShowIconOnRunApp"), false));
	
	cfg.flush();
	
	chang = false;
	

	form->addChild(zcbFiltrProc);
	form->addChild(zcbTaskNoWin);
	ZSeparator *sep = new ZSeparator();
	form->addChild(sep);
	form->addChild(zcbJavaIcon);
	form->addChild(zcbNativIcon);
	form->addChild(zcbRunAppIcon);
	sep = new ZSeparator();
	form->addChild(sep);
	form->addChild(zcbSendReasePhone);
	form->addChild(zcbSendGoToIDLE);
*/
	ZSoftKey *softKey = this->getSoftKey();
	setMainWidgetTitle(QString::fromUtf8(APP_NAME));
	softKey->setText ( ZSoftKey::LEFT, QString::fromUtf8(IDS_OK), ( ZSoftKey::TEXT_PRIORITY ) 0 );
	softKey->setText ( ZSoftKey::RIGHT, QString::fromUtf8(IDS_CANCEL), ( ZSoftKey::TEXT_PRIORITY ) 0 );
	softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( reject() ) );
	softKey->setClickedSlot ( ZSoftKey::LEFT, this, SLOT ( accept() ) );
	
}

ZAppSetting::~ZAppSetting()
{
	delete zcbFiltrProc;
	delete zcbJavaIcon;
	delete zcbNativIcon;
	delete zcbSendReasePhone;
	delete zcbSendGoToIDLE;
}

void ZAppSetting::accept()
{

	QString appConf="";
	ZConfig cfg(appConf);
	cfg.writeEntry("Task", "FiltrProc", !zcbFiltrProc->GetChecked() );
	cfg.writeEntry("Task", "ShowAppNoWindow", zcbTaskNoWin->GetChecked() );
	cfg.writeEntry("Task", "ShowJavaIcon", zcbJavaIcon->GetChecked() );
	cfg.writeEntry("Task", "ShowNativIcon", zcbNativIcon->GetChecked() );
	cfg.writeEntry("Task", "ShowIconOnRunApp", zcbRunAppIcon->GetChecked() );
	cfg.writeEntry("General", "SendReaisePhone", zcbSendReasePhone->GetChecked() );  
	cfg.writeEntry("General", "SendGoToIDLE", zcbSendGoToIDLE->GetChecked() );
	cfg.flush();
	
	
	if (chang)
	{
		
/*		zTaskMan->cfg_SendReaisePhone = zcbSendReasePhone->GetChecked();
		zTaskMan->cfg_SendGoToIDLE = zcbSendGoToIDLE->GetChecked();
		zTaskMan->cfg_FiltrProc = !zcbFiltrProc->GetChecked();
		zTaskMan->cfg_ShowJavaIcon = zcbJavaIcon->GetChecked();
		zTaskMan->cfg_ShowNativIcon = zcbNativIcon->GetChecked();
		zTaskMan->cfg_ShowAppNoWindow = zcbTaskNoWin->GetChecked();
		zTaskMan->cfg_ShowIconOnRunApp = zcbRunAppIcon->GetChecked();
*/		
		
		MyBaseDlg::accept();
	} else
	{
		MyBaseDlg::reject();
	}
}

void ZAppSetting::reject()
{
	MyBaseDlg::reject();
} 

/**********************************************************************/
/**********************************************************************/
ZMyCheckBox::ZMyCheckBox(const QString & text, QWidget * parent, const char* name, bool value) : ZCheckBox ( text, parent, name ) {
    curState = value;
if (value)
	{
	    setState((ZBaseButton::ToggleState)2);
	}
	else
	{
	    setState((ZBaseButton::ToggleState)0);
	}
    connect (this,SIGNAL(toggled(bool)),this,SLOT(SetChecked(bool)));
}

ZMyCheckBox::~ZMyCheckBox()
{

}

void ZMyCheckBox::SetChecked(bool state)
{
    curState = state;
    chang = true;
}

bool ZMyCheckBox::GetChecked() const
{
     return curState;
}
