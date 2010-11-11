#ifndef _REGISTRYMANAGER_H_
#define _REGISTRYMANAGER_H_

#include <qobject.h>
#include <qlist.h>
#include <qmap.h>
#include <quuid.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qdatastream.h>
#include <qvaluelist.h>
#include <qdatetime.h>
#include <qarray.h>
#include <qcopchannel_qws.h>


class QCopChannel;

enum ENUM_ALERT_STYLE{
	ALERT_STYLE_RING_ONLY_EV,
	ALERT_STYLE_RING_AND_VIBRATE_EV,
	ALERT_STYLE_VIBRATE_THEN_RING_EV,
	ALERT_STYLE_VIBRATE_ONLY_EV,
	ALERT_STYLE_SILENT_EV
};

enum ENUM_TIMER_TYPE
{
    BACKLIGHT_TIMEOUT_EV,
    KEYPAD_BACKLIGHT_TIMEOUT_EV,
    DISPLAY_TIMEOUT_EV,
    SCREENSAVER_TIMEOUT_EV
};


class STUFF_INSERT_Dialog
{
  public:
	void exec();

};

class IDLE_MediaPlayer_Client
{
  public:

	char getSongName();
	bool getPlayerState();
	void signalSongFullPath(QString);
	void sendStopCmd();
	void sendNextCmd();
	void sendPlayCmd();
	void sendPrevCmd();
	void sendQuitCmd();
	void sendPauseCmd();
	void signalPlayerStart();
	void signalPlayerEnd();
	char getSongFullPath();

};

class UTIL_KeyboardFilter
{
  public:
	void setKpBacklight(bool);

};

class MSGCTR_ENG_MsgListObj
{
  public:
	const int getMsgCount();
	const int getUnreadMsgCount();

};

class UTIL_KeyboardLockClient
{
  public:
	void lockKeyboard();
	void unlockKeyboard();
};

class SETUP_Utility
{
  public:
	int getAlertStyle(ENUM_ALERT_STYLE &);
	int setActiveAlertStyle(ENUM_ALERT_STYLE);
	void setAirplaneMode(bool);
	int isAirplaneModeOn();
	void setStatusLight(bool);
	void setTimeout(ENUM_TIMER_TYPE, int);
};

class AM_MainMenuInterface
{
  public:
    AM_MainMenuInterface();
    virtual ~AM_MainMenuInterface();

    int ShowMainMenu();
};

class MSGCTR_ENG_StatusApi: public QObject
{
  Q_OBJECT
  public:
    MSGCTR_ENG_StatusApi( QObject * parent = 0, const char * name = 0 );
    virtual ~MSGCTR_ENG_StatusApi();

	void gotoScreenOfComposingNewMsg(QString const&, QString const&);
	void gotoScreenOfUnreadMsgS();
	void gotoScreenOfInbox();
	int getCountOfUnreadSMS();
};

class MSGCTR_ENG_MsgList
{
  public:
	int getUnreadMsgCount();

};

class ALARMCLOCK_QuickSetting
{
  public:
	void setQuickAlarm(QTime const&);
	void cancelQuickAlarm();
};

class AM_ActionData : public QObject
{
  Q_OBJECT
  public:
    AM_ActionData();                         
    virtual ~AM_ActionData();                        
	void getValue(QString const&,QString&);
	void setValue(QString const&, QString const&);
	void setValue(QString const&, int);
	void setAction(QString const&);
	int invoke() const;

};

class ZUNS_NotifyDlg
{
//  Q_OBJECT
  public:
//	ZUNS_NotifyDlg(UNS_PRIO_T, QString const&, QString const&, QWidget*, char const*);
//	virtual ~ZUNS_NotifyDlg();

	void show();

};


class AM_LauncherInterface
{
  Q_OBJECT
  public:
    AM_LauncherInterface(QObject*, char const*);
    virtual ~AM_LauncherInterface();

	QUuid uid;
	void autoOpen(QString const&, bool);
	void launchApp(QUuid const&, bool);
	void doLaunchAppWithUidGid(QString const&, unsigned int, unsigned int, int, int);
	void launchAppWithUid(QString const&, QString const&, bool) ; 
	void launchAppWithUidGid(QString const&, uint, uint);
	void autoOpenWithAnotherApp(QUuid const&, QString const&, bool);

};

class AM_Global
{
  public:
    enum Device
    {
      InvalidDevice = -1,
      Flash1    = 0,
      Flash2    = 1,
      Card1   = 2,
      Card2   = 3,
      Preload   = 4
    };

    //installCompleteMessageBox()           // FUNC
    //installFailedMessageBox()
    //getInstCardDir()              // FUNC
    //getInstalledQtDir(AM_Global::Device)          // FUNC
    //getInstalledQtTmpDir(AM_Global::Device)               // FUNC
    //getInstalledThemeDir()
    //getNativeDir()                // FUNC
    //getPreloadDir()               // FUNC
    //getScreenLength()             // FUNC
    //getScreenWidth()              // FUNC
    //getSetupDir()
    //installCompleteMessageBox()           // FUNC
    //installFailedMessageBox()
    //errorOkMessageBox(QString const&)
    //memoryFullMessageBox()
    void uninstallCompleteMessageBox();
    void uninstallFailedMessageBox();

};

class AM_AppObject
{
  public:
    enum IconType
    {
      BigIcon = 0,    // Big icon
      SmallIcon = 1,  // Small icon
      AniIcon = 2     // Animation icon
    };
    enum Type
    {
      InvalidType = 0,
      Folder = 1,         // 'Folder' is a composite, which may contain other elements.
      Application = 2,    // 'Application', 'EmbeddedLink', 'Shortcut', etc. are all leaves
      EmbeddedLink = 3,   // Also known as 'Invisible net link'
      Shortcut = 4
    };

    AM_AppObject();
    AM_AppObject ( AM_AppObject::Type );
    virtual ~AM_AppObject();

    virtual void invoke( const AM_LauncherInterface & ) {}
   
    
    QString getBigIcon() const;
    void getBigIcon ( QPixmap &icon) const;  
    void getAnimationIcon ( QPixmap& ) const; //work for java ???
    void getSmallIcon ( QPixmap& ) const;  // work for java ???
    void getIcon ( QPixmap& icon, AM_AppObject::IconType ) const;
    
    QString getDefaultName();
    
    QString getName() const;
    int setName ( QString const& );

    QString getPath() const;
    
    int getSize();

    QString getUid() const;
    void setUid( const QString &id );

    QUuid getAppId() const;
    void setAppId( const QUuid &id );
   

    AM_AppObject::Type getType();

    QString getVender();
};

class AM_AppLnk
{
  public:
    AM_AppLnk();
    virtual ~AM_AppLnk();

    QString getExecFullPath() const;
    int getGidByString ( QString const& );
    int getUidByString ( QString const& );
};


class AM_Folder : public AM_AppObject
{
  public:
                     AM_Folder(); 
                     virtual ~AM_Folder(); 
                     void add ( const AM_AppObject *a, int index = -1 ); 
                     void remove ( const AM_AppObject * ); 
                     void clear(); 
                     void getAppList ( QList<AM_AppObject> & ) const; 
                     bool isEmpty() const; 
                     int  getAppPosition ( const AM_AppObject *app ); 
    
};


typedef QMap< QString, AM_AppObject* > AppObjMap;
typedef AppObjMap::Iterator AppObjIterator;
typedef AppObjMap::ConstIterator AppObjConstIt;


class AM_RegistryManager : public QObject
{
  Q_OBJECT
  public:

    struct AM_AppRecord
    {
      // Structure like *Registry ini files (?)
      QString Type;
      QString Attribute;
      bool Daemon;
      bool MassStorageStatus;
      QString BigIcon;
      QString Icon;
      QString AniIcon;
      QUuid AppID;
      bool LockEnabled;
      bool FixedPosition;
      bool FixedAllPosition;
      QString Directory;
      QString Exec;
      QString Args;
      QString UserID;
      QString GroupID;
      bool Visible;
      bool Eraseable;
      QString Name;
      QString IMEI;
    };

    enum RegFileType
    {
        // System defined config file(read-only)
      SysMenuTree = 0,
      SysRegistry = 1,
        
        // User defined config file(read-write)
      UserMenuTree = 2,   // read-write copy of SysMenuTree
      UserRegistry = 3,   // for user defined folder
        
        // Config file on plug card(read-write), for installed Java/Qt on card
      CardRegistry = 4,
        
        // Customization config file, for rename & change icon
      UserConfig = 5,
        
        // Config file for application lock
      AppLockConfig = 6,
        
        // Installed Java/Qt on phone
      InstalledDB = 7,
        
        // MIME
      SysMime = 8,    // read-only
      UserMime = 9,   // read-write copy of SysMime
        
        // Installed java on card. useless now!!!
      PlugCardDB = 10,

        // store the application parent folder's uid and position on card
      PlugCardRegistry = 11
    };
        
    AM_RegistryManager ( QObject * parent=0, const char * name=0 );
    virtual ~AM_RegistryManager();

    static QUuid createUuid(); // Generate new correct Uuid
    int getAllAppList ( QList<AM_AppObject>& ) const;
    int getInstalledAppList ( QList<AM_AppLnk>&, bool ) const;

    bool addAppToRegistry ( const AM_AppRecord &record,
                            QString &strAppUid,
                            const QString &strFolderUid = QString::null,
                            const AM_Global::Device device = AM_Global::Flash2,
                            int pos = -1 );



    bool addAppToMainMenu ( const QString &appUid, const QString &folderUid = QString::null, int pos = -1 );
    bool removeAppFromMainMenu( const QString &appUid, bool removeItFromRegistry = false );
    
    int getAppNameByFullPath ( QString const&, QString& ) const;
    AM_AppObject * getAppObjById ( QUuid const& ) const;
    int getAppObject ( QString const& ) const;
    int getAutoRunApps ( QList<AM_AppLnk>& ) const;
    int loadMenuTree();
    int loadRegistry();
    
    static QString getConfigFileName( const RegFileType );
    
//    void slotAppInstalled ( AM_Global::Device device, const QString &appUid, const QString &folderUid, int pos );
    
  signals:
    void addAppToFolder ( QString const& appUid, QString const& dirUid, int pos = -1 );
    void folderChanged( const AM_Folder * );      // folder content has been changed

  public slots:
    void slotAddAppToFolder( const QString &appUid, const QString &folder, int pos );
    void slotAppInstalled( AM_Global::Device device, const QString &appUid, const QString &folderUid, int pos );
    void slotAppUnInstalled( const QString &appUid, const QString &folderUid );
};

//AM_RegistryManager_removeAppFromMainMenu
extern "C" 
     { 
           bool AM_RegistryManager_removeAppFromMainMenu ( const char * appUid, bool removeItFromRegistry = false ); 
           void AM_RegistryManager_getAppParentFolderUid ( const char * appUid, char * folderUid ); 
           bool AM_RegistryManager_changeFolder ( const char * appUid, const char * srcFolderUid, const char * destFolderUid, int pos = -1 ); 
   } 


#endif
