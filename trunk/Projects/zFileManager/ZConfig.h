/***********************************************************
 *
 * Reversed header for Motorola Z6
 *
 * Version: 0.1alpha
 *
 * Id: $Id: ZConfig.h 6 2008-01-15 10:24:31Z ivan $
 *
 * vim: sw=8:ts=8:si:noexpandtab
 */

#ifndef _ZCONFIG_H_
#define _ZCONFIG_H_

#include <qstring.h>
#include <qstringlist.h>

class ZConfig
{
public:
    ZConfig(QString const& file, bool); // I don't no, what is bool parametr
    virtual ~ZConfig();

    // No comments =)   
    bool changed() const;
    // May be flush on disk file?
    virtual int flush();

    // TODO ? int write(QString const&);
    // TODO - QString read();
    int parseLine(QString const&);

    int initSettingDirectory();
    QString getSettingDirectory();

    int setGroup(QString const&);
    // TODO - QStringList getGroupEntriesKeyList(QString const&);
    // TODO ? QStringList getGroupsKeyList(QStringList&);
    bool groupExists(QString const&);
    virtual int clearGroup(QString const&);

    bool readBoolEntry(QString const&, QString const&, bool);
    QString readEntryDirect(QString const&, QString const&, QString const&);
    QString readEntry(QString const&, QString const&, QString const&);
    QStringList readListEntry(QString const&, QString const&, QChar const&);
    int readNumEntry(QString const&, QString const&, int);
    unsigned int readUIntEntry(QString const&, QString const&, unsigned int);

    int writeEntry(QString const&, QString const&, bool);
    int writeEntry(QString const&, QString const&, char*);
    int writeEntry(QString const&, QString const&, double);
    int writeEntry(QString const&, QString const&, int);
    int writeEntry(QString const&, QString const&, QString const&);
    int writeEntry(QString const&, QString const&, QStringList const&, QChar const&);
    int writeEntry(QString const&, QString const&, unsigned int);

    virtual int removeEntry(QString const&, QString const&);

protected:

private:
    void* ConfigData;
};

#endif

