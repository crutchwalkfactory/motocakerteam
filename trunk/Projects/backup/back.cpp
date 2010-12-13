#include <ZApplication.h>
#include <ZSingleSelectDlg.h>
#include <ZSingleCaptureDlg.h>
#include <ZMessageDlg.h>
#include <ZNoticeDlg.h>
#include <ZListBox.h>
#include <ZConfig.h>

#include <getopt.h>
#include <qtextcodec.h>
#include <string.h>
#include <qfileinfo.h>
#include <sys/un.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <iostream>
#include <linux/ioctl.h>
#include <fstream>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <stdarg.h>
#include <qtextcodec.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <qtextstream.h>

QTextCodec* txtcodec = QTextCodec::codecForName("UTF-8");
QString Idioma;
QString continuar;

int main(int argc, char **argv)
{
	ZApplication* a = new ZApplication(argc, argv);
	QStringList list;
	int checked = -1;
	int timeout = 0;
	continuar="NO";

	QString val; 
	ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
	Idioma = "ezxlocal/download/mystuff/.system/LinXtend/usr/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
	if ( ! QFileInfo(Idioma).isFile() ) Idioma = "/ezxlocal/download/mystuff/.system/LinXtend/usr/languages/en-us";
	ZConfig IDini(Idioma, false);

	val = IDini.readEntry(QString("BACKUP"), QString("BACKUP"), "");
	list += val;


	system(QString("find /mmc/mmca1/Backup -name \"*.backup\" > /tmp/mybackups"));
	QString archivo = QString("/tmp/mybackups"); QStringList copias, copias2;
	QString line=""; QFile entrada(archivo); QTextStream stentrada(&entrada);
	if ( entrada.open(IO_ReadOnly | IO_Translate) )
	{ while ( !stentrada.eof() ) { line = stentrada.readLine();
	if ( line != "" ) { copias2 += line; int i = line.findRev("/"); line.remove( 0, i+1 ); 
		int k = line.findRev(QChar('.')); line.remove( k, line.length() - k ); copias += line; } } 
	}
	entrada.close(); system("rm /tmp/mybackups");
	if ( copias.count() > 0 )
	{
		val = IDini.readEntry(QString("BACKUP"), QString("RESTORE"), "");
		list += val;
		val = IDini.readEntry(QString("BACKUP"), QString("DELETE"), "");
		list += val;
	}

	val = IDini.readEntry(QString("BACKUP"), QString("TEXT1"), "");
	ZSingleSelectDlg* dlg = new ZSingleSelectDlg("Backup & Restore", val, NULL, "ZSingleSelectDlg", true, 0, 0);

	ZConfig ICONini(QString("/usr/data_resource/preloadapps/SysRegistry"), false);
	val = ICONini.readEntry(QString("fcb2cf94-6f7a-4000-803c-41adc1998807"), QString("BigIcon"), "");
	if ( val[0] == QChar('/') ) { QPixmap p1; p1.load(val); dlg->setTitleIcon(p1); }
	else { dlg->setTitleIcon(val); }
	dlg->addItemsList(list);
	ZListBox* listbox = dlg->getListBox();
	if (checked >= 0) listbox->checkItem(checked, true);
	if (timeout) dlg->setAutoDismissTime(timeout*1000);

   	a->setMainWidget( dlg );
   	dlg->show();
   	a->exec();
	int result = dlg->result();
	if (result != 1) { result = 0; }
	else { system(QString("echo \"%1\"").arg(result)); }

	if (listbox->itemChecked(0))
	{
	  QString BackName;
	  val = IDini.readEntry(QString("BACKUP"), QString("ENTERNAME"), "");
	  ZSingleCaptureDlg* zscd = new ZSingleCaptureDlg( QString("Backup & Restore"), val, ZSingleCaptureDlg::normal, NULL, "ZSingleCaptureDlg", true, 0, 0);
	  ZLineEdit* zle = (ZLineEdit*)zscd->getLineEdit(); zle->setEdited(true); 
	  if (zscd->exec() == 1) { if ( zle->text() != "" ) { BackName=zle->text(); continuar="SI"; } else { continuar="NO"; } }
	  if ( continuar=="SI" ) {
		val = IDini.readEntry(QString("BACKUP"), QString("CONFIRM"), "");
		ZMessageDlg* msg =  new ZMessageDlg ( "Backup & Restore", QString(val).arg(BackName), ZMessageDlg::yes_no, 0, NULL, "ZMessageDlg", true, 0 );
		ZConfig ICONini(QString("/usr/data_resource/preloadapps/SysRegistry"), false);
		val = ICONini.readEntry(QString("fcb2cf94-6f7a-4000-803c-41adc1998807"), QString("BigIcon"), "");
		if ( val[0] == QChar('/') ) { QPixmap p1; p1.load(val); msg->setTitleIcon(p1); }
		else { msg->setTitleIcon(val); }
		int i = msg->exec(); delete msg; msg = NULL;
		if (i)
		{
		  system(QString("mkdir /mmc/mmca1/Backup"));
		  system(QString("mkdir /mmc/mmca1/Backup/pre%1").arg(BackName));
		  system(QString("cp -R /ezx_user/sysDatabase /mmc/mmca1/Backup/pre%1").arg(BackName));
		  system(QString("cp -R /ezxlocal/download/appwrite/setup/devdetail.wbxml /mmc/mmca1/Backup/pre%1").arg(BackName));
		  system(QString("cp -R /ezxlocal/.system/mms/mailfile /mmc/mmca1/Backup/pre%1").arg(BackName));
		  system(QString("cp -R /ezx_user/download/appwrite/bluetooth /mmc/mmca1/Backup/pre%1").arg(BackName));
		  system(QString("cp -R /ezxlocal/download/appwrite/NetProfile /mmc/mmca1/Backup/pre%1").arg(BackName));
		  system(QString("cp -R /ezx_user/download/appwrite/email /mmc/mmca1/Backup/pre%1").arg(BackName));
		  system(QString("cp -R /ezxlocal/download/appwrite/alarmclock /mmc/mmca1/Backup/pre%1").arg(BackName));
		  system(QString("cd /mmc/mmca1/Backup; /ezxlocal/LinXtend/bin/busybox2 tar czvf %1.backup pre%2").arg(BackName).arg(BackName));
		  system(QString("rm -r /mmc/mmca1/Backup/pre%1").arg(BackName));
		  val = IDini.readEntry(QString("BACKUP"), QString("DONE"), "");
		  ZMessageDlg* msg =  new ZMessageDlg ( QString("Backup & Restore"), val, ZMessageDlg::just_ok, 0, NULL, "ZMessageDlg", true, 0 );
		  ZConfig ICONini(QString("/usr/data_resource/preloadapps/SysRegistry"), false);
		  val = ICONini.readEntry(QString("fcb2cf94-6f7a-4000-803c-41adc1998807"), QString("BigIcon"), "");
		  if ( val[0] == QChar('/') ) { QPixmap p1; p1.load(val); msg->setTitleIcon(p1); }
		  else { msg->setTitleIcon(val); }
		  msg->exec(); delete msg; msg = NULL;
		}
	  }
	}
	if (listbox->itemChecked(1))
	{
	  system(QString("find /mmc/mmca1/Backup -name \"*.backup\" > /tmp/mybackups"));
	  QString archivo = QString("/tmp/mybackups"); QStringList copias, copias2;
	  QString line=""; QFile entrada(archivo); QTextStream stentrada(&entrada);
	  if ( entrada.open(IO_ReadOnly | IO_Translate) )
	  { while ( !stentrada.eof() ) { line = stentrada.readLine();
		if ( line != "" ) { copias2 += line; int i = line.findRev("/"); line.remove( 0, i+1 ); 
		int k = line.findRev(QChar('.')); line.remove( k, line.length() - k ); copias += line; } } 
	  }
	  entrada.close(); system("rm /tmp/mybackups");
	  val = IDini.readEntry(QString("BACKUP"), QString("TEXT2"), "");
	  ZSingleSelectDlg* dlg2 = new ZSingleSelectDlg("Backup & Restore", val, NULL, "ZSingleSelectDlg", true, 0, 0);
	  ZConfig ICONini(QString("/usr/data_resource/preloadapps/SysRegistry"), false);
	  val = ICONini.readEntry(QString("fcb2cf94-6f7a-4000-803c-41adc1998807"), QString("BigIcon"), "");
	  if ( val[0] == QChar('/') ) { QPixmap p1; p1.load(val); dlg2->setTitleIcon(p1); }
	  else { dlg2->setTitleIcon(val); }
	  dlg2->addItemsList(copias);
	  ZListBox* listbox2 = dlg2->getListBox();
	  if (checked >= 0) listbox2->checkItem(checked, true);
	  if (timeout) dlg2->setAutoDismissTime(timeout*1000);
	  a->setMainWidget( dlg2 );
   	  dlg2->show(); a->exec(); int result = dlg2->result();
	  if (result != 1) { result = 0; }
	  else
	  {
		int j = listbox2->currentItem();
		QString BackName = copias[j];
		val = IDini.readEntry(QString("BACKUP"), QString("CONFIRM2"), "");
		ZMessageDlg* msg =  new ZMessageDlg ( "Backup & Restore", QString(val).arg(BackName), ZMessageDlg::yes_no, 0, NULL, "ZMessageDlg", true, 0 );
		ZConfig ICONini(QString("/usr/data_resource/preloadapps/SysRegistry"), false);
		val = ICONini.readEntry(QString("fcb2cf94-6f7a-4000-803c-41adc1998807"), QString("BigIcon"), "");
		if ( val[0] == QChar('/') ) { QPixmap p1; p1.load(val); msg->setTitleIcon(p1); }
		else { msg->setTitleIcon(val); }
		int i = msg->exec(); delete msg; msg = NULL;
		if (i)
		{
		  system(QString("echo \"RESTORE : %1\"").arg(BackName));
		  system(QString("/ezxlocal/LinXtend/bin/busybox2 tar -xf \"/mmc/mmca1/Backup/%1.backup\" -C /mmc/mmca1/Backup").arg(BackName));
		  system(QString("cp -f /mmc/mmca1/Backup/pre%1/sysDatabase/main.db /ezx_user/sysDatabase/main.db").arg(BackName));
		  system(QString("cp -f /mmc/mmca1/Backup/pre%1/devdetail.wbxml /ezxlocal/download/appwrite/setup/devdetail.wbxml").arg(BackName));
		  system(QString("rm -f /ezxlocal/.system/mms/*"));
		  system(QString("cp -Rf /mmc/mmca1/Backup/pre%1/mailfile /ezxlocal/.system/mms").arg(BackName));
		  system(QString("cp -Rf /mmc/mmca1/Backup/pre%1/bluetooth /ezx_user/download/appwrite").arg(BackName));
		  system(QString("cp -Rf /mmc/mmca1/Backup/pre%1/NetProfile /ezx_user/download/appwrite").arg(BackName));
		  system(QString("cp -Rf /mmc/mmca1/Backup/pre%1/email /ezx_user/download/appwrite").arg(BackName));
		  system(QString("cp -Rf /mmc/mmca1/Backup/pre%1/alarmclock /ezx_user/download/appwrite").arg(BackName));
		  system(QString("chown -R ezx:ezx /ezxlocal/.system/mms/mailfile/*"));
		  int k = BackName.findRev("."); BackName.remove( k,4 );
		  system(QString("rm -r /mmc/mmca1/Backup/pre%1").arg(BackName));
		  val = IDini.readEntry(QString("BACKUP"), QString("DONE2"), "");
		  ZMessageDlg* msg =  new ZMessageDlg ( QString("Backup & Restore"), val, ZMessageDlg::just_ok, 0, NULL, "ZMessageDlg", true, 0 );
		  ZConfig ICONini(QString("/usr/data_resource/preloadapps/SysRegistry"), false);
		  val = ICONini.readEntry(QString("fcb2cf94-6f7a-4000-803c-41adc1998807"), QString("BigIcon"), "");
		  if ( val[0] == QChar('/') ) { QPixmap p1; p1.load(val); msg->setTitleIcon(p1); }
		  else { msg->setTitleIcon(val); }
		  msg->exec(); delete msg; msg = NULL;
		  system(QString("/sbin/reboot"));
		}
	  }
	}
	if (listbox->itemChecked(2))
	{
	  system(QString("find /mmc/mmca1/Backup -name \"*.backup\" > /tmp/mybackups"));
	  QString archivo = QString("/tmp/mybackups"); QStringList copias, copias2;
	  QString line=""; QFile entrada(archivo); QTextStream stentrada(&entrada);
	  if ( entrada.open(IO_ReadOnly | IO_Translate) )
	  { while ( !stentrada.eof() ) { line = stentrada.readLine();
		if ( line != "" ) { copias2 += line; int i = line.findRev("/"); line.remove( 0, i+1 );  
		int k = line.findRev(QChar('.')); line.remove( k, line.length() - k ); copias += line; } } 
	  }
	  entrada.close(); system("rm /tmp/mybackups");
	  val = IDini.readEntry(QString("BACKUP"), QString("TEXT3"), "");
	  ZSingleSelectDlg* dlg2 = new ZSingleSelectDlg("Backup & Restore", val, NULL, "ZSingleSelectDlg", true, 0, 0);
	  ZConfig ICONini(QString("/usr/data_resource/preloadapps/SysRegistry"), false);
	  val = ICONini.readEntry(QString("fcb2cf94-6f7a-4000-803c-41adc1998807"), QString("BigIcon"), "");
	  if ( val[0] == QChar('/') ) { QPixmap p1; p1.load(val); dlg2->setTitleIcon(p1); }
	  else { dlg2->setTitleIcon(val); }
	  dlg2->addItemsList(copias);
	  ZListBox* listbox2 = dlg2->getListBox();
	  if (checked >= 0) listbox2->checkItem(checked, true);
	  if (timeout) dlg2->setAutoDismissTime(timeout*1000);
	  a->setMainWidget( dlg2 );
   	  dlg2->show(); a->exec(); int result = dlg2->result();
	  if (result != 1) { result = 0; }
	  else
	  {
		int j = listbox2->currentItem();
		QString BackName = copias[j];
		val = IDini.readEntry(QString("BACKUP"), QString("CONFIRM3"), "");
		ZMessageDlg* msg =  new ZMessageDlg ( "Backup & Restore", QString(val).arg(BackName), ZMessageDlg::yes_no, 0, NULL, "ZMessageDlg", true, 0 );
		ZConfig ICONini(QString("/usr/data_resource/preloadapps/SysRegistry"), false);
		val = ICONini.readEntry(QString("fcb2cf94-6f7a-4000-803c-41adc1998807"), QString("BigIcon"), "");
		if ( val[0] == QChar('/') ) { QPixmap p1; p1.load(val); msg->setTitleIcon(p1); }
		else { msg->setTitleIcon(val); }
		int i = msg->exec(); delete msg; msg = NULL;
		if (i)
		{
		  system(QString("rm /mmc/mmca1/Backup/%1").arg(BackName));
		  val = IDini.readEntry(QString("BACKUP"), QString("DONE3"), "");
		  ZMessageDlg* msg =  new ZMessageDlg ( QString("Backup & Restore"), QString(val).arg(BackName),
								ZMessageDlg::just_ok, 0, NULL, "ZMessageDlg", true, 0 );
		  ZConfig ICONini(QString("/usr/data_resource/preloadapps/SysRegistry"), false);
		  val = ICONini.readEntry(QString("fcb2cf94-6f7a-4000-803c-41adc1998807"), QString("BigIcon"), "");
		  if ( val[0] == QChar('/') ) { QPixmap p1; p1.load(val); msg->setTitleIcon(p1); }
		  else { msg->setTitleIcon(val); }
		  msg->exec(); delete msg; msg = NULL;
		}
	  }
	}
}
