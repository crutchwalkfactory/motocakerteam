/****************************************************************************
** MainWidget meta object code from reading C++ file 'main.h'
**
** Created: Mon Aug 10 04:23:07 2009
**      by: The Qt MOC ($Id: qt/src/moc/moc.y   2.3.8   edited 2004-08-05 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 9
#elif Q_MOC_OUTPUT_REVISION != 9
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "../main.h"
#include <qmetaobject.h>
#include <qapplication.h>
#ifdef QWS
#include <qobjectdict.h>
#endif



const char *MainWidget::className() const
{
    return "MainWidget";
}

QMetaObject *MainWidget::metaObj = 0;

#ifdef QWS
static class MainWidget_metaObj_Unloader {
public:
    ~MainWidget_metaObj_Unloader()
    {
         if ( objectDict )
             objectDict->remove( "MainWidget" );
    }
} MainWidget_metaObj_unloader;
#endif

void MainWidget::initMetaObject()
{
    if ( metaObj )
	return;
    if ( qstrcmp(ZKbMainWidget::className(), "ZKbMainWidget") != 0 )
	badSuperclassWarning("MainWidget","ZKbMainWidget");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString MainWidget::tr(const char* s)
{
    return qApp->translate( "MainWidget", s, 0 );
}

QString MainWidget::tr(const char* s, const char * c)
{
    return qApp->translate( "MainWidget", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* MainWidget::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) ZKbMainWidget::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    typedef void (MainWidget::*m1_t0)();
    typedef void (QObject::*om1_t0)();
    typedef void (MainWidget::*m1_t1)();
    typedef void (QObject::*om1_t1)();
    typedef void (MainWidget::*m1_t2)(int);
    typedef void (QObject::*om1_t2)(int);
    m1_t0 v1_0 = &MainWidget::slotQuit;
    om1_t0 ov1_0 = (om1_t0)v1_0;
    m1_t1 v1_1 = &MainWidget::runEmulator;
    om1_t1 ov1_1 = (om1_t1)v1_1;
    m1_t2 v1_2 = &MainWidget::menuSelect;
    om1_t2 ov1_2 = (om1_t2)v1_2;
    QMetaData *slot_tbl = QMetaObject::new_metadata(3);
    QMetaData::Access *slot_tbl_access = QMetaObject::new_metaaccess(3);
    slot_tbl[0].name = "slotQuit()";
    slot_tbl[0].ptr = (QMember)ov1_0;
    slot_tbl_access[0] = QMetaData::Public;
    slot_tbl[1].name = "runEmulator()";
    slot_tbl[1].ptr = (QMember)ov1_1;
    slot_tbl_access[1] = QMetaData::Public;
    slot_tbl[2].name = "menuSelect(int)";
    slot_tbl[2].ptr = (QMember)ov1_2;
    slot_tbl_access[2] = QMetaData::Public;
    metaObj = QMetaObject::new_metaobject(
	"MainWidget", "ZKbMainWidget",
	slot_tbl, 3,
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
