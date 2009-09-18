/* This file is part of the KDE libraries
    Copyright (C) 1999 Lars Knoll (knoll@kde.org)
    $Id: kcharsets.cpp,v 1.9 2003/02/03 15:59:20 montanaro Exp $

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include <qglobal.h>

#if !defined(QT_NO_FONTDATABASE) && !defined(QT_NO_CODECS)
#include "../kdesrc/kdecore/kcharsets.cpp"
#elif defined(Q_WS_QWS)

#include "kcharsets.h"

#include "kentities.c"

#include <kapplication.h>
#include <kglobal.h>
#include <klocale.h>
#include <kconfig.h>

#include <qfontinfo.h>
#include <qfontmanager_qws.h>
#include <kdebug.h>

#include <qtextcodec.h>
#include <qmap.h>
#include <qcstring.h>
#include <qstrlist.h>

static const char * const languages[] = {
    I18N_NOOP( "other" ),
	I18N_NOOP( "Arabic" ),
	I18N_NOOP( "Baltic" ),
	I18N_NOOP( "Central European" ),
	I18N_NOOP( "Chinese Simplified" ),
	I18N_NOOP( "Chinese Traditional" ),
	I18N_NOOP( "Cyrillic" ),
	I18N_NOOP( "Greek" ),
	I18N_NOOP( "Hebrew" ),
	I18N_NOOP( "Japanese" ),
	I18N_NOOP( "Korean" ),
	I18N_NOOP( "Thai" ),
	I18N_NOOP( "Turkish" ),
	I18N_NOOP( "Western European" ),
	I18N_NOOP( "Tamil" ),
	I18N_NOOP( "Unicode" )
};


class KCharsetsPrivate
{
public:
    KCharsetsPrivate(KCharsets* _kc)
        : codecForNameDict(43, false) // case insensitive
    {
        kc = _kc;
        conf = new KConfig( "charsets", true, false );
    }
    ~KCharsetsPrivate()
    {
        delete conf;
    }
    QAsciiDict<QTextCodec> codecForNameDict;
    KCharsets* kc;
    KConfig* conf;

    void getAvailableCharsets();
};

// --------------------------------------------------------------------------

KCharsets::KCharsets()
{
    d = new KCharsetsPrivate(this);
}

KCharsets::~KCharsets()
{
    delete d;
}

QString KCharsets::languageForEncoding( const QString &encoding )
{
    d->conf->setGroup("LanguageForEncoding");

    int lang = d->conf->readNumEntry(encoding, 0 );
    return i18n( ::languages[lang] );
}

QString KCharsets::encodingForName( const QString &descriptiveName )
{
    int left = descriptiveName.find( "( " );
    return descriptiveName.mid( left + 2, descriptiveName.find( " )" )
      - left - 2 );
}

QStringList KCharsets::descriptiveEncodingNames()
{
  QStringList encodings = availableEncodingNames();
  QStringList::Iterator it;
  for( it = encodings.begin(); it != encodings.end(); ++it ) {
      QString lang = KGlobal::charsets()->languageForEncoding( *it );
      *it = lang + " ( " + *it + " )";
  }
  encodings.sort();
  return encodings;
}

QFont KCharsets::fontForChar( const QChar &c, const QFont &_f ) const
{
    QFontInfo fi(_f);

    // unicode can display any char...
    if (fi.charSet() == QFont::Unicode) return _f;

    // here comes the work...
    // we look at the range the char is in, and try charsets which can
    // map the char...
    QFont f = _f;
    int uc = c.unicode();

    if( uc < 0xff ) // 8859-1
        setQFont( f, QFont::Latin1 );
    else if ( uc > 0x0400 && uc < 0x0460 )
        setQFont( f, QFont::Latin5 );
    else if ( uc > 0x0600 && uc < 0x0660 )
        setQFont( f, QFont::Latin6 );
    else if ( uc > 0x0380 && uc < 0x03e0 )
        setQFont( f, QFont::Latin7 );
    else if ( uc >= 0x05d0 && uc < 0x05f0 )
        setQFont( f, QFont::Latin8 );
    else if ( hasUnicode( f ) ) // don't know, let's try Unicode
        setQFont( f, QFont::Unicode );

    return f;
}

QStringList KCharsets::availableFamilies( QFont::CharSet ch )
{
    QStringList families;
    QStrList chFamilies = (*d->availableCharsets)[ch];

    // list Unicode as available if nothing else found
    if ( ( (int)ch != QFont::Unicode ) && chFamilies.isEmpty() )
	chFamilies = (*d->availableCharsets)[QFont::Unicode];

    for ( unsigned i = 0; i < chFamilies.count(); ++i )
	families.append( QString::fromLatin1( chFamilies.at( i ) ) );

    return families;
}

void KCharsets::setQFont(QFont &f, QString charset) const
{
    QFont::CharSet cs = nameToID(charset);
    if ( cs == QFont::AnyCharSet ) {
	cs = charsetForEncoding(charset);
    }
    setQFont( f, cs );
}

void KCharsets::setQFont(QFont &f, QFont::CharSet charset) const
{
    if(QFontInfo(f).charSet() == charset)
        return;

    if(charset == QFont::AnyCharSet)
    {
        f.setCharSet(QFont::AnyCharSet);
        return;
    }

    d->getAvailableCharsets();

    QCString family = f.family().latin1();

    //kdDebug() << "KCharsets::setQFont family=" << family << endl;

    QStrList chFamilies = (*d->availableCharsets)[charset];
    if(chFamilies.contains(family)) {
	//kdDebug() << "KCharsets::setQFont: charsetAvailable in family" << endl;
        f.setCharSet(charset);
        return;
    }

    // ok... we don't have the charset in the specified family, let's
    // try to find a replacement.

    if(chFamilies.count() != 0) {
	//kdDebug() << "KCharsets::setQFont: using family " << chFamilies.first() << " in native charset " << charset << endl;
        f.setFamily(chFamilies.first());
        f.setCharSet(charset);
        return;
    }

    QStrList ucFamilies = (*d->availableCharsets)[QFont::Unicode];
    if(ucFamilies.contains(family)) {
	//kdDebug() << "KCharsets::setQFont: using unicode" << endl;
	// just setting the charset to unicode should work
        f.setCharSet(QFont::Unicode);
        return;
    }

    // Unicode and any family
    if(ucFamilies.count() != 0) {
	//kdDebug() << "KCharsets::setQFont: using family " << chFamilies.first() << " in unicode" << endl;
        f.setFamily(ucFamilies.first());
        f.setCharSet(QFont::Unicode);
        return;
    }

    // give up -- just use something
    f.setCharSet(QFont::AnyCharSet);
    return;
}

bool KCharsets::isAvailable(const QString &charset)
{
    QFont::CharSet cs = nameToID(charset);
    return cs == QFont::AnyCharSet ? false : isAvailable(cs);
}

bool KCharsets::isAvailable(QFont::CharSet charset)
{
    d->getAvailableCharsets();

    return d->availableCharsets->contains(charset);
}

QFont::CharSet KCharsets::charsetForLocale()
{
    return nameToID(KGlobal::locale()->charset());
}

bool KCharsets::hasUnicode(QString family) const
{
    d->getAvailableCharsets();
    if(!d->availableCharsets->contains(QFont::Unicode))
        return false;
    QStrList lst = (*d->availableCharsets)[QFont::Unicode];
    if(lst.contains(family.latin1()))
        return true;
    return false;
}

bool KCharsets::hasUnicode(QFont &font) const
{
    return hasUnicode(font.family());
}


QString KCharsets::xCharsetName(QFont::CharSet charSet) const
{
    switch( charSet )
    {
    case QFont::Unicode:
        return "iso10646-1";
    case QFont::ISO_8859_1:
        return "iso8859-1";
    case QFont::ISO_8859_2:
        return "iso8859-2";
    case QFont::ISO_8859_3:
        return "iso8859-3";
    case QFont::ISO_8859_4:
        return "iso8859-4";
    case QFont::ISO_8859_5:
        return "iso8859-5";
    case QFont::ISO_8859_6:
        return "iso8859-6";
    case QFont::ISO_8859_7:
        return "iso8859-7";
    case QFont::ISO_8859_8:
        return "iso8859-8";
    case QFont::ISO_8859_9:
        return "iso8859-9";
    case QFont::ISO_8859_10:
        return "iso8859-10";
    case QFont::ISO_8859_11: // most of them are actually named as tis620
        return "tis620-0";
    case QFont::ISO_8859_12:
        return "iso8859-12";
    case QFont::ISO_8859_13:
        return "iso8859-13";
    case QFont::ISO_8859_14:
        return "iso8859-14";
    case QFont::ISO_8859_15:
        return "iso8859-15";
    case QFont::KOI8R:
        return "koi8-r";
    case QFont::KOI8U:
        return "koi8-u";
    case QFont::Set_Ko:
        return "ksc5601.1987-0";
    case QFont::Set_Ja:
        return "jisx0208.1983-0";
    case QFont::TSCII:
        return "tscii-0";
    case QFont::Set_Th_TH:
	return "unknown";
	case QFont::Set_GBK:
        return "gbk-0";
	case QFont::Set_Zh:
	 return "gb2312.1980-0";
    case QFont::Set_Zh_TW:
        return "cns11643.1986-*";
    case QFont::Set_Big5:
	return "big5-0";
#if QT_VERSION >= 224
    case QFont::CP1251:
	return "microsoft-cp1251";
    case QFont::PT154:
	return "paratype-cp154";
#endif
    case QFont::AnyCharSet:
    default:
        break;
    }
    return "*-*";
}

QFont::CharSet KCharsets::nameToID(QString name) const
{
    name = name.lower();
    if(d->nameToIDMap.contains(name))
        return d->nameToIDMap[name]; // cache hit

    int i = 0;
    while(i < CHARSETS_COUNT)
    {
        if( name == charsetsStr[i] )
        {
            d->nameToIDMap.replace(name, charsetsIds[i]);
            return charsetsIds[i];
        }
        i++;
    }

    i = 0;
    while(i < CHARSETS_COUNT)
    {
        if( name == xNames[i] )
        {
            d->nameToIDMap.replace(name, charsetsIds[i]);
            return charsetsIds[i];
        }
        i++;
    }
    d->nameToIDMap.replace(name, QFont::AnyCharSet);
    return QFont::AnyCharSet;
}

QString KCharsets::name(const QFont &f)
{
    QFont::CharSet c = f.charSet();

    return name(c);
}

QString KCharsets::name(QFont::CharSet c)
{
    int i = 0;

    while(i < CHARSETS_COUNT)
    {
        if( c == charsetsIds[i] )
            return charsetsStr[i];
        i++;
    }
    return "any";
}


QFont::CharSet KCharsets::xNameToID(QString name) const
{
    name = name.lower();

    // fix this stone age problem
    if ( name == "iso10646" )
	name = xNames[0];

    // try longest names first, then shorter ones
    // to avoid that iso-8859-10 matches iso-8859-1
    int i = CHARSETS_COUNT-1; // avoid the "" entry
    while( i-- )
    {
	if( !QRegExp( xNames[i] ).match(name) ) {
	    return charsetsIds[i];
	}
    }

    return QFont::AnyCharSet;
}


QTextCodec *KCharsets::codecForName(const QString &n) const
{
    bool b;
    return codecForName( n, b );
}

QTextCodec *KCharsets::codecForName(const QString &n, bool &ok) const
{
    ok = true;

    QTextCodec* codec = 0;
    // dict lookup is case insensitive anyway
    if((codec = d->codecForNameDict[n.isEmpty() ? "->locale<-" : n.latin1()]))
        return codec; // cache hit, return

    QCString name = n.lower().latin1();

    if (n.isEmpty()) {
        QString lc = KGlobal::locale()->charset();
        if (lc.isEmpty())
            codec = QTextCodec::codecForName("iso8859-1");
        else
            codec = codecForName(lc);

        d->codecForNameDict.replace("->locale<-", codec);
        return codec;
    }

    codec = QTextCodec::codecForName(name);

    if(codec) {
        d->codecForNameDict.replace(name, codec);
        return codec;
    }

    // these codecs are built into Qt, but the name given for the codec is different,
    // so QTextCodec did not recognise it.
    d->conf->setGroup("builtin");

    QString cname = d->conf->readEntry(name.data());
    if(!cname.isEmpty() && !cname.isNull())
        codec = QTextCodec::codecForName(cname.latin1());

    if(codec)
    {
        d->codecForNameDict.replace(name, codec);
        return codec;
    }

    d->conf->setGroup("general");
    QString dir = d->conf->readEntry("i18ndir", QString::fromLatin1("/usr/share/i18n/charmaps"));
    dir += "/";

    // these are codecs not included in Qt. They can be build up if the corresponding charmap
    // is available in the charmap directory.
    d->conf->setGroup("aliases");

    cname = d->conf->readEntry(name.data());
    if(cname.isNull() || cname.isEmpty())
        cname = name;
    cname = cname.upper();

#ifndef QT_NO_CODECS
    codec = QTextCodec::loadCharmapFile(dir + cname);
#endif

    if(codec) {
        d->codecForNameDict.replace(name, codec);
        return codec;
    }

    // this also failed, the last resort is now to take some compatibility charmap

    d->conf->setGroup("conversionHints");
    cname = cname.lower();
    cname = d->conf->readEntry(cname);

    if(!cname.isEmpty() && !cname.isNull())
        codec = QTextCodec::codecForName(cname.latin1());

    if(codec) {
        d->codecForNameDict.replace(name, codec);
        return codec;
    }

    // could not assign a codec, let's return Latin1
    ok = false;
    return QTextCodec::codecForName("iso8859-1");
}


#endif // QT_NO_FONTDATABASE

