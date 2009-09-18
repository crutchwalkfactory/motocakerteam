#ifndef __kmessagebox_h__
#define __kmessagebox_h__

#include <qmessagebox.h>
#include <qvaluelist.h>
#include <qguardedptr.h>

#include <kstdguiitem.h>

// ### this is an ugly hack, to get the QInputDialog wrapper for
// khtml/ecma/kjs_window.cpp, in case QT_NO_INPUTDIALOG is
// defined (kjs_window.cpp includes kmessagebox)
#include <kinputdialog.h>

class MessageBoxQueuer : public QObject
{
    Q_OBJECT
public:
    static MessageBoxQueuer *self();

    static void queue( QWidget *parent, int type, const QString &text,
                       const QString &caption );

private:
    MessageBoxQueuer() {}

private slots:
    void dequeue();

private:
    struct Entry
    {
        QGuardedPtr<QWidget> parent;
        int type;
        QString text;
        QString caption;
    };

    QValueList<Entry> m_queue;
    bool m_lock;

    static MessageBoxQueuer *s_self;
};

class KMessageBox
{
public:

    enum
    {
        QuestionYesNo = 1,
        WarningYesNo = 2,
        WarningContinueCancel = 3,
        WarningYesNoCancel = 4,
        Information = 5,
        // Reserved for: SSLMessageBox = 6
        Sorry = 7,
        Error = 8,
        WarningContinueCancelList = 9
    };

    enum Answer { Ok = 1, Cancel = 2, Yes = 3, No = 4, Continue = 5 };

    // Needed for kdelibs 3.1 
    enum OptionsType { Notify = 1, AllowLink = 2 };
    
    static void error( QWidget *parent,
                       const QString &text,
                       const QString &caption = QString::null )
        { information( parent, text, caption ); }

    static void sorry( QWidget *parent,
                       const QString &text,
                       const QString &caption = QString::null )
        { information( parent, text, caption ); }

    static int questionYesNo( QWidget *parent,
                              const QString &text,
                              const QString &caption = QString::null,
                              const QString &buttonYes = QString::null,
                              const QString &buttonNo = QString::null,
                              const QString &dontaskmeagain = QString::null,
                              bool okButton = false )
        { return warningYesNoCancel( parent, text, caption, buttonYes, buttonNo, dontaskmeagain, false ); }

    // changed for kdelibs 3.1
    static void information( QWidget *parent,
                             const QString &text,
                             const QString &caption = QString::null,
                             const QString &dontShowAgainName = QString::null )
        { warningYesNoCancel( parent, text, caption, QString::null, QString::null, QString::null, false, true ); }

    // changed for kdelibs 3.1
    static int warningYesNo( QWidget *parent,
                             const QString &text,
                             const QString &caption = QString::null,
                             const QString &buttonYes = QString::null,
                             const QString &buttonNo = QString::null,
                             const QString & = QString::null,
			     int = Notify )
        { return questionYesNo( parent, text, caption, buttonYes, buttonNo ); }

    static int warningContinueCancel( QWidget *parent,
                             const QString &text,
                             const QString &caption = QString::null,
                             const QString &buttonContinue = QString::null,
                             const QString &buttonCancel = QString::null );

    static int warningYesNoCancel( QWidget *parent,
                             const QString &text,
                             const QString &caption = QString::null,
                             const QString &buttonYes = QString::null,
                             const QString &buttonNo = QString::null,
                             const QString &/*dontaskmeagain*/ = QString::null,
                             bool cancelButton = true,
                             bool okButton = false );

    // ### HACK!!! new for KDE3
    static int warningContinueCancelList(QWidget *parent, 
                             const QString &text,
                             const QStringList &strlist,
                             const QString &caption = QString::null,
                             const QWidget &buttonContinue = QWidget(),
                             const QString &dontAskAgainName = QString::null) 
        { return questionYesNo( parent, text, QString::null, QString::null, QString::null ); }
    
    static void queuedMessageBox( QWidget *parent,
                                  int type,
                                  const QString &text,
                                  const QString &caption )
        { MessageBoxQueuer::queue( parent, type, text, caption ); }

};


#endif
