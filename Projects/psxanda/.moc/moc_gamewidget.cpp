/****************************************************************************
** GameWidget meta object code from reading C++ file 'gamewidget.h'
**
** Created: Mon Aug 10 04:23:09 2009
**      by: The Qt MOC ($Id: qt/src/moc/moc.y   2.3.8   edited 2004-08-05 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 9
#elif Q_MOC_OUTPUT_REVISION != 9
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "../gamewidget.h"
#include <qmetaobject.h>
#include <qapplication.h>
#ifdef QWS
#include <qobjectdict.h>
#endif



const char *GameWidget::className() const
{
    return "GameWidget";
}

QMetaObject *GameWidget::metaObj = 0;

#ifdef QWS
static class GameWidget_metaObj_Unloader {
public:
    ~GameWidget_metaObj_Unloader()
    {
         if ( objectDict )
             objectDict->remove( "GameWidget" );
    }
} GameWidget_metaObj_unloader;
#endif

void GameWidget::initMetaObject()
{
    if ( metaObj )
	return;
    if ( qstrcmp(QWidget::className(), "QWidget") != 0 )
	badSuperclassWarning("GameWidget","QWidget");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString GameWidget::tr(const char* s)
{
    return qApp->translate( "GameWidget", s, 0 );
}

QString GameWidget::tr(const char* s, const char * c)
{
    return qApp->translate( "GameWidget", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* GameWidget::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QWidget::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    QMetaData::Access *slot_tbl_access = 0;
    metaObj = QMetaObject::new_metaobject(
	"GameWidget", "QWidget",
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    metaObj->set_slot_access( slot_tbl_access );
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    return metaObj;
}
