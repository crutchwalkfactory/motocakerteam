// Implementation of the Tv GUI
// This is just like the Kiosk GUI, but the status message has to be shown on
// a floating QDockWindow, since the 

#include "mainwindow_tv.h"

#if defined(KONQ_GUI_TV)

#include "view.h"
#include "htmlview.h"

#include <qdialog.h>
#include <qvbox.h>

StatusDialog::StatusDialog(QWidget *parent) : 
	QDialog(parent, 0, 0, WType_Dialog | WStyle_Customize | WStyle_NoBorder)
{
    KConfig *cfg = KGlobal::config();
    cfg->setGroup("Tv");
    resize(cfg->readNumEntry("StatusPanelWidth", 600), 12 + QFontMetrics(font()).height());
    setFocusPolicy(QWidget::NoFocus);

    QVBox* statusBox = new QVBox(this);
    statusBox->setLineWidth(2);
    statusBox->setFrameStyle(QFrame::Panel | QFrame::Raised);
    statusBox->setFixedSize(this->size());
    statusBox->setMargin(3);
	
    label = new QLabel(statusBox);
}

StatusDialog::~StatusDialog()
{
}

void StatusDialog::setMessage(const QString &msg) 
{ 
    label->setText(msg); 
}

void StatusDialog::enterEvent(QEvent *)
{
    if (y() > 400) {
	move(x(), y() - 400);
    } else {
	move(x(), y() + 400);
    }
    
}

MainWindowTv::MainWindowTv() : MainWindowBase(0, 0, 
		WType_TopLevel | WDestructiveClose | 
		WStyle_Customize | WStyle_NoBorderEx)
{
}

BrowserView *MainWindowTv::createNewViewInternal(const KURL &url, 
	const KParts::URLArgs &args, bool withStatus)
{
    BrowserView *v = MainWindowBase::createNewViewInternal(url, args);
    v->document()->view()->setVScrollBarMode(QScrollView::AlwaysOff);
    v->document()->view()->setHScrollBarMode(QScrollView::AlwaysOff);

    
    return v;
}

void MainWindowTv::initGUI()
{
    m_findInput = 0;
    statusDialog = new StatusDialog(this);
    KConfig *cfg = KGlobal::config();
    cfg->setGroup("Tv");
    setGeometry(cfg->readRectEntry("SafeDisplayArea", QRect(32, 28, 640, 520)));
    QPoint p = cfg->readPointEntry("StatusPanelOffset", QPoint(20, 460));
    statusDialog->move(x() + p.x(), y() + p.y());
    
    // Disable original statusbar
    disconnect(m_statusBarAutoHideTimer, SIGNAL(timeout()),
             statusBar(), SLOT(hide()));
    statusBar()->hide();
    connect(m_statusBarAutoHideTimer, SIGNAL(timeout()),
             statusDialog, SLOT(hide()));
}

void MainWindowTv::popupMenu(const QString &, const QPoint &)
{
	// There're no popup menus for the tv GUI
}

void MainWindowTv::statusJustText(const QString &_msg, bool)
{
    QString msg = _msg;
    msg.remove(QRegExp("<img.*>"));
    statusDialog->setMessage(msg);
}

void MainWindowTv::statusMessage(const QString &_msg, bool visible)
{
    statusJustText(_msg, visible);
    if (visible && s_statusDelay >= 0) {
	statusDialog->show();
	if (s_statusDelay > 0)
	    m_statusBarAutoHideTimer->start(s_statusDelay, true);
    } else {
	statusDialog->hide();
    }
}

#include "mainwindow_tv.moc"

#endif

// vim:sw=4:
