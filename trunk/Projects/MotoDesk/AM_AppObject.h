#ifndef AM_AppObject_H
#define AM_AppObject_H

#include <qmap.h>
#include <qstring.h>
#include <quuid.h>
#include <qpixmap.h>

typedef QMap< QString, QString > AppRecord;
typedef QMap< QString, QString >::Iterator AppRecordIt;
typedef QMap< QString, QString >::ConstIterator AppRecordConstIt;

class AM_LauncherInterface;
class AM_Folder;

class AM_AppObject
{
public:
    enum Type
    {
        InvalidType = 0,
        Folder = 1,
        Application = 2, 
        EmbeddedLink = 3,
        Shortcut = 4
    };

    enum Attribute
    {
        InvalidAttr = 0,
        Native = 1,
        PreloadedQt = 2,
        PreloadedJava = 3,
        InstalledQt = 4,
        InstalledJava = 5,
        System = 6, 
        Preload = 7,
        User = 8
    };

public:
    AM_AppObject();
    virtual ~AM_AppObject();

    virtual void invoke( const AM_LauncherInterface & ) {}
    virtual bool createObject( const AppRecord & );
    virtual bool getAppLockEnabled() const;
    virtual void setAppLockEnabled( bool );
    virtual int  getAppLockValue() const;
    virtual void setAppLockValue( int );
    
    AM_AppObject::Type getType() const;
    void setType( const AM_AppObject::Type t );

    AM_AppObject::Attribute getAttr() const;
    void setAttr( const AM_AppObject::Attribute a );

    QString getBigIcon() const;
    void setBigIcon( const QString &icon );

    QString getSmallIcon() const;
    void setSmallIcon( const QString & icon );

    QString getAnimationIcon() const;
    void setAnimationIcon( const QString & icon );
    
    void getBigIcon( QPixmap &icon ) const;
    void getSmallIcon( QPixmap &icon ) const;
    void getAnimationIcon( QPixmap &icon ) const;

    QString getUid() const;
    void setUid( const QString &id );

    QUuid getAppId() const;
    void setAppId( const QUuid &id );

    bool getFixedPosition() const;
    void setFixedPosition( bool b );
    bool getFixedAllPosition() const;
    void setFixedAllPosition( bool b );

    QString getDirectory() const;
    void setDirectory( const QString &d );

    QString getName() const;
    void setName( const QString & );
    
    bool getUserDefinedName() const;
    void setUserDefinedName( bool b );
    bool getUserDefinedIcon() const;
    void setUserDefinedIcon( bool b );

    int getDevice() const;
    void setDevice( const int d );
    
    AM_Folder * getParent() const;
    void setParent( const AM_Folder *p );

    bool getVisible() const;
    void setVisible( bool v );

    QString getArgs() const;
    void setArgs( const QString &a );
    
    QString getAutoRunArgs() const;
    void setAutoRunArgs( const QString &a );
     unsigned int  getSize(void);
	QString getVender(void);
	bool getModified(void)const;
	void setModified(bool b);
	QString getDefaultName(void);
    
    AM_AppObject( Type );
    AM_Folder *parent;
    Type type;
    Attribute attr;
    QString bgIcon;
    QString smIcon;
    QString aniIcon;
    QString uid;
    QUuid appId;
    bool fixedPos;
    bool fixedAllPos;
    QString dir;
    QString name;
    QString vender;
    bool visible;
    bool userDefinedName;
    bool userDefinedIcon;
    int dev;    

    QString autoRunArgs;
    QString args;       

	 QString getPath(void) const;

	 bool mModified;

    enum IconType
    {
        BigIcon = 0,
        SmallIcon = 1,
        AniIcon = 2
    };
    
    void getIcon( QPixmap &icon, IconType type = BigIcon ) const;
    void init();
};

#endif
