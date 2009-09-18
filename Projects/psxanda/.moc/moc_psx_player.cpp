/****************************************************************************
** PSX_GameWidget meta object code from reading C++ file 'psx_player.h'
**
** Created: Mon Aug 10 04:23:10 2009
**      by: The Qt MOC ($Id: qt/src/moc/moc.y   2.3.8   edited 2004-08-05 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 9
#elif Q_MOC_OUTPUT_REVISION != 9
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "../psx_player.h"
#include <qmetaobject.h>
#include <qapplication.h>
#ifdef QWS
#include <qobjectdict.h>
#endif



const char *PSX_GameWidget::className() const
{
    return "PSX_GameWidget";
}

QMetaObject *PSX_GameWidget::metaObj = 0;

#ifdef QWS
static class PSX_GameWidget_metaObj_Unloader {
public:
    ~PSX_GameWidget_metaObj_Unloader()
    {
         if ( objectDict )
             objectDict->remove( "PSX_GameWidget" );
    }
} PSX_GameWidget_metaObj_unloader;
#endif

void PSX_GameWidget::initMetaObject()
{
    if ( metaObj )
	return;
    if ( qstrcmp(GameWidget::className(), "GameWidget") != 0 )
	badSuperclassWarning("PSX_GameWidget","GameWidget");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString PSX_GameWidget::tr(const char* s)
{
    return qApp->translate( "PSX_GameWidget", s, 0 );
}

QString PSX_GameWidget::tr(const char* s, const char * c)
{
    return qApp->translate( "PSX_GameWidget", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* PSX_GameWidget::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) GameWidget::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    QMetaData::Access *slot_tbl_access = 0;
    metaObj = QMetaObject::new_metaobject(
	"PSX_GameWidget", "GameWidget",
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
