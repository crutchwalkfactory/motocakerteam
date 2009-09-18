#ifndef __klocale_h__
#define __klocale_h__

#include <qobject.h>
#include <qstringlist.h>
#include <qdatetime.h>
#include <qtextcodec.h>

// ### compat to make html_formimpl.cpp compile
#include <kglobal.h>

inline QString i18n( const char *s ) { return QObject::tr( s ); }
inline QString i18n( const char *, const char *s ) { return QObject::tr( s ); }
QString i18n( const char *singular, const char *plural, unsigned long n );

#define I18N_NOOP( x ) x

// dummy
class KLocale
{
public:
    KLocale() {}
    ~KLocale() {}

    // ###
    QStringList languageList() const
        {
            QStringList res;
            res << QString::fromLatin1( "C" );
            return res;
        }

    // ###
    QString language() const { return QString::fromLatin1( "C" ); }

    // ###
    QString languages() { return language(); }

    // ###
    QString charset() const { return QString::fromLatin1( "iso-8859-1" ); }

    // ###
    QTextCodec * codecForEncoding() const { return QTextCodec::codecForMib(4); }

    const char* encoding() const { return codecForEncoding()->name(); }

    // Added for kdelibs 3.1
    void removeCatalogue( const QString & ) {}

    static void setMainCatalogue( const char * ) {}

    //QString formatDate( const QDate &date ) const { return date.toString(); }

    //QString formatTime( const QTime &time ) const { return time.toString(); }

    QString formatDateTime( const QDateTime &dt, bool = false, bool = false ) const
        { return dt.date().toString() + " " + dt.time().toString(); }

    // ###
    QString formatNumber( double num, int precision = 6 ) const
        { return QString::number( num, 'g', precision ); }
};

#endif
