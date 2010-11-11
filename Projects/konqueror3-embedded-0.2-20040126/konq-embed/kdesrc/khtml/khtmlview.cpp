/* This file is part of the KDE project
 *
 * Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 *                     1999 Lars Knoll <knoll@kde.org>
 *                     1999 Antti Koivisto <koivisto@kde.org>
 *                     2000 Dirk Mueller <mueller@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#include "khtmlview.moc"

#include "khtmlview.h"

#include "khtml_part.h"
#include "khtml_events.h"

#include "html/html_documentimpl.h"
#include "html/html_inlineimpl.h"
#include "rendering/render_root.h"
#include "rendering/render_frames.h"
#include "rendering/render_replaced.h"
#include "xml/dom2_eventsimpl.h"
#include "css/cssstyleselector.h"
#include "misc/htmlhashes.h"
#include "misc/helper.h"
#include "khtml_settings.h"
#include "khtml_printsettings.h"

#include <kcursor.h>
#include <ksimpleconfig.h>
#include <kstringhandler.h>
#include <kstandarddirs.h>
#include <kprinter.h>
#include <klocale.h>

#include <qtooltip.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qstylesheet.h>
#include <qobjectlist.h>
#include <kapplication.h>

#include <kimageio.h>
#include <assert.h>
#include <kdebug.h>
#include <kurldrag.h>
#include <qobjectlist.h>
#include <qtimer.h>
#include <kdialogbase.h>

#define PAINT_BUFFER_HEIGHT 128

using namespace DOM;
using namespace khtml;
class KHTMLToolTip;

#ifndef QT_NO_TOOLTIP

class KHTMLToolTip : public QToolTip
{
public:
    KHTMLToolTip(KHTMLView *view,  KHTMLViewPrivate* vp) : QToolTip(view->viewport())
    {
        m_view = view;
        m_viewprivate = vp;
    };

protected:
    virtual void maybeTip(const QPoint &);

private:
    KHTMLView *m_view;
    KHTMLViewPrivate* m_viewprivate;
};

#endif

class KHTMLViewPrivate {
    friend class KHTMLToolTip;
public:
    KHTMLViewPrivate()
    {
        underMouse = 0;
        reset();
        tp=0;
        paintBuffer=0;
        vertPaintBuffer=0;
        formCompletions=0;
        prevScrollbarVisible = true;
	timerId = 0;
        repaintTimerId = 0;
        scrollTimerId = 0;
        complete = false;
	tooltip = 0;
        possibleTripleClick = false;
    }
    ~KHTMLViewPrivate()
    {
        delete formCompletions;
        delete tp; tp = 0;
        delete paintBuffer; paintBuffer =0;
        delete vertPaintBuffer;
        if (underMouse)
	    underMouse->deref();
	delete tooltip;
    }
    void reset()
    {
        if (underMouse)
	    underMouse->deref();
	underMouse = 0;
        linkPressed = false;
        useSlowRepaints = false;
        originalNode = 0;
	borderTouched = false;
#ifndef KHTML_NO_SCROLLBARS
        vmode = QScrollView::Auto;
        hmode = QScrollView::Auto;
#else
        vmode = QScrollView::AlwaysOff;
        hmode = QScrollView::AlwaysOff;
#endif
        scrollBarMoved = false;
        ignoreWheelEvents = false;
	borderX = 30;
	borderY = 30;
	clickX = -1;
	clickY = -1;
        prevMouseX = -1;
        prevMouseY = -1;
	clickCount = 0;
	isDoubleClick = false;
	scrollingSelf = false;
	timerId = 0;
        repaintTimerId = 0;
        scrollTimerId = 0;
        complete = false;
        firstRelayout = true;
        dirtyLayout = false;
        layoutSchedulingEnabled = true;
        updateRect = QRect();
        m_dialogsAllowed = true;
    }
    void newScrollTimer(QWidget *view, int tid)
    {
        kdDebug() << "newScrollTimer timer" << tid << endl;
        view->killTimer(scrollTimerId);
        scrollTimerId = tid;
    }
    enum ScrollDirection { ScrollLeft, ScrollRight, ScrollUp, ScrollDown };

    void adjustScroller(QWidget *view, ScrollDirection direction, ScrollDirection oppositedir)
    {
        static const struct { int msec, pixels; } timings [] = {
            {100,1}, {50,1}, {30,1}, {20,1}, {20,2}, {20,4}, {20,6}, {0,0}
        };
        if (!scrollTimerId ||
            (scrollDirection != direction &&
             scrollDirection != oppositedir)) {
            scrollTiming = 2;
            scrollBy = timings[scrollTiming].pixels;
            scrollDirection = direction;
            newScrollTimer(view, view->startTimer(timings[scrollTiming].msec));
        } else if (scrollDirection == direction &&
                   timings[scrollTiming+1].msec) {
            scrollBy = timings[++scrollTiming].pixels;
            newScrollTimer(view, view->startTimer(timings[scrollTiming].msec));
        } else if (scrollDirection == oppositedir) {
            if (scrollTiming) {
                scrollBy = timings[--scrollTiming].pixels;
                newScrollTimer(view, view->startTimer(timings[scrollTiming].msec));
            } else
                newScrollTimer(view, 0);
        }
    }

    QPainter *tp;
    QPixmap  *paintBuffer;
    QPixmap  *vertPaintBuffer;
    NodeImpl *underMouse;

    // the node that was selected when enter was pressed
    NodeImpl *originalNode;

    bool borderTouched:1;
    bool borderStart:1;
    bool scrollBarMoved:1;

    QScrollView::ScrollBarMode vmode;
    QScrollView::ScrollBarMode hmode;
    bool prevScrollbarVisible;
    bool linkPressed;
    bool useSlowRepaints;
    bool ignoreWheelEvents;

    int borderX, borderY;
    KSimpleConfig *formCompletions;

    int clickX, clickY, clickCount;
    bool isDoubleClick;

    int prevMouseX, prevMouseY;
    bool scrollingSelf;
    int timerId;

    int repaintTimerId;
    int scrollTimerId;
    int scrollTiming;
    int scrollBy;
    ScrollDirection scrollDirection;
    bool complete;
    bool firstRelayout;
    bool layoutSchedulingEnabled;
    bool possibleTripleClick;
    bool dirtyLayout;
    bool m_dialogsAllowed;
    QRect updateRect;
    KHTMLToolTip *tooltip;
    QPtrDict<QWidget> visibleWidgets;
};

#ifndef QT_NO_TOOLTIP

void KHTMLToolTip::maybeTip(const QPoint& p)
{
    DOM::NodeImpl *node = m_viewprivate->underMouse;
    QRect region;
    while ( node ) {
        if ( node->isElementNode() ) {
            QString s = static_cast<DOM::ElementImpl*>( node )->getAttribute( ATTR_TITLE ).string();
            region |= QRect( m_view->contentsToViewport( node->getRect().topLeft() ), node->getRect().size() );
            if ( !s.isEmpty() ) {
                tip( region, QStyleSheet::convertFromPlainText( s, QStyleSheetItem::WhiteSpaceNormal ) );
                break;
            }
        }
        node = node->parentNode();
    }
}
#endif

KHTMLView::KHTMLView( KHTMLPart *part, QWidget *parent, const char *name)
    : QScrollView( parent, name, WResizeNoErase | WRepaintNoErase | WPaintUnclipped )
{
    m_medium = "screen";

    m_part = part;
    d = new KHTMLViewPrivate;
    QScrollView::setVScrollBarMode(d->vmode);
    QScrollView::setHScrollBarMode(d->hmode);
    connect(kapp, SIGNAL(kdisplayPaletteChanged()), this, SLOT(slotPaletteChanged()));
    connect(this, SIGNAL(contentsMoving(int, int)), this, SLOT(slotScrollBarMoved()));

    // initialize QScrollview
    enableClipper(true);
    setResizePolicy(Manual);
    viewport()->setMouseTracking(true);
    viewport()->setBackgroundMode(NoBackground);

    KImageIO::registerFormats();

#ifndef QT_NO_TOOLTIP
    d->tooltip = new KHTMLToolTip( this, d );
#endif

    init();

    viewport()->show();
}

KHTMLView::~KHTMLView()
{
    closeChildDialogs();
    if (m_part)
    {
        //WABA: Is this Ok? Do I need to deref it as well?
        //Does this need to be done somewhere else?
        DOM::DocumentImpl *doc = m_part->xmlDocImpl();
        if (doc)
            doc->detach();
    }
    delete d; d = 0;
}

void KHTMLView::init()
{
    if(!d->paintBuffer) d->paintBuffer = new QPixmap(PAINT_BUFFER_HEIGHT, PAINT_BUFFER_HEIGHT);
    if(!d->vertPaintBuffer)
        d->vertPaintBuffer = new QPixmap(10, PAINT_BUFFER_HEIGHT);
    if(!d->tp) d->tp = new QPainter();

    setFocusPolicy(QWidget::StrongFocus);
    viewport()->setFocusPolicy( QWidget::WheelFocus );
    viewport()->setFocusProxy(this);

    _marginWidth = -1; // undefined
    _marginHeight = -1;
    _width = 0;
    _height = 0;

    setAcceptDrops(true);
    resizeContents(visibleWidth(), visibleHeight());
}

void KHTMLView::clear()
{
    // work around QScrollview's unbelievable bugginess
    setStaticBackground(true);

    d->reset();
    killTimers();
    emit cleared();

    QScrollView::setHScrollBarMode(d->hmode);
    if (d->vmode==Auto)
        QScrollView::setVScrollBarMode(d->prevScrollbarVisible?AlwaysOn:Auto);
    else
        QScrollView::setVScrollBarMode(d->vmode);
}

void KHTMLView::hideEvent(QHideEvent* e)
{
    QScrollView::hideEvent(e);
}

void KHTMLView::showEvent(QShowEvent* e)
{
    QScrollView::showEvent(e);
}

void KHTMLView::resizeEvent (QResizeEvent* e)
{
    QScrollView::resizeEvent(e);

    if ( m_part && m_part->xmlDocImpl() )
        m_part->xmlDocImpl()->dispatchWindowEvent( EventImpl::RESIZE_EVENT, false, false );
}

void KHTMLView::viewportResizeEvent (QResizeEvent* e)
{
    QScrollView::viewportResizeEvent(e);

    //int w = visibleWidth();
    //int h = visibleHeight();

    if (d->layoutSchedulingEnabled)
        layout();

    KApplication::sendPostedEvents(viewport(), QEvent::Paint);
}

// this is to get rid of a compiler virtual overload mismatch warning. do not remove
void KHTMLView::drawContents( QPainter*)
{
}

void KHTMLView::drawContents( QPainter *p, int ex, int ey, int ew, int eh )
{
    //kdDebug( 6000 ) << "drawContents x=" << ex << ",y=" << ey << ",w=" << ew << ",h=" << eh << endl;
    if(!m_part->xmlDocImpl() || !m_part->xmlDocImpl()->renderer()) {
        p->fillRect(ex, ey, ew, eh, palette().active().brush(QColorGroup::Base));
        return;
    }
    if (eh > PAINT_BUFFER_HEIGHT && ew <= 10) {
        if ( d->vertPaintBuffer->height() < visibleHeight() )
            d->vertPaintBuffer->resize(10, visibleHeight());
        d->tp->begin(d->vertPaintBuffer);
        d->tp->translate(-ex, -ey);
        d->tp->fillRect(ex, ey, ew, eh, palette().active().brush(QColorGroup::Base));
        m_part->xmlDocImpl()->renderer()->paint(d->tp, ex, ey, ew, eh, 0, 0);
        d->tp->end();
        p->drawPixmap(ex, ey, *d->vertPaintBuffer, 0, 0, ew, eh);
    }
    else {
        if ( d->paintBuffer->width() < visibleWidth() )
            d->paintBuffer->resize(visibleWidth(),PAINT_BUFFER_HEIGHT);

        int py=0;
        while (py < eh) {
            int ph = eh-py < PAINT_BUFFER_HEIGHT ? eh-py : PAINT_BUFFER_HEIGHT;
            d->tp->begin(d->paintBuffer);
            d->tp->translate(-ex, -ey-py);
            d->tp->fillRect(ex, ey+py, ew, ph, palette().active().brush(QColorGroup::Base));
            m_part->xmlDocImpl()->renderer()->paint(d->tp, ex, ey+py, ew, ph, 0, 0);
#ifdef BOX_DEBUG
            if (m_part->xmlDocImpl()->focusNode())
            {
                d->tp->setBrush(Qt::NoBrush);
                d->tp->drawRect(m_part->xmlDocImpl()->focusNode()->getRect());
            }
#endif
            d->tp->end();

            p->drawPixmap(ex, ey+py, *d->paintBuffer, 0, 0, ew, ph);
            py += PAINT_BUFFER_HEIGHT;
        }
    }

    khtml::DrawContentsEvent event( p, ex, ey, ew, eh );
    QApplication::sendEvent( m_part, &event );

}

void KHTMLView::setMarginWidth(int w)
{
    // make it update the rendering area when set
    _marginWidth = w;
}

void KHTMLView::setMarginHeight(int h)
{
    // make it update the rendering area when set
    _marginHeight = h;
}

void KHTMLView::layout()
{
    if( m_part->xmlDocImpl() ) {
        DOM::DocumentImpl *document = m_part->xmlDocImpl();

        khtml::RenderRoot* root = static_cast<khtml::RenderRoot *>(document->renderer());
        if ( !root ) return;

         if (document->isHTMLDocument()) {
             NodeImpl *body = static_cast<HTMLDocumentImpl*>(document)->body();
             if(body && body->renderer() && body->id() == ID_FRAMESET) {
                 QScrollView::setVScrollBarMode(AlwaysOff);
                 QScrollView::setHScrollBarMode(AlwaysOff);
                 body->renderer()->setLayouted(false);
//                  if (d->tooltip) {
//                      delete d->tooltip;
//                      d->tooltip = 0;
//                  }
             }
             else if (!d->tooltip)
                 d->tooltip = new KHTMLToolTip( this, d );
         }

        _height = visibleHeight();
        _width = visibleWidth();
        //QTime qt;
        //qt.start();
        root->setMinMaxKnown(false);
        root->setLayouted(false);
        root->layout();
        //kdDebug( 6000 ) << "TIME: layout() dt=" << qt.elapsed() << endl;
    }
    else
       _width = visibleWidth();
}

void KHTMLView::closeChildDialogs()
{
    QObjectList *dlgs = queryList("QDialog");
    for (QObject *dlg = dlgs->first(); dlg; dlg = dlgs->next())
    {
        KDialogBase* dlgbase = dynamic_cast<KDialogBase *>( dlg );
        if ( dlgbase ) {
            kdDebug(6000) << "closeChildDialogs: closing dialog " << dlgbase << endl;
            // close() ends up calling QButton::animateClick, which isn't immediate
            // we need something the exits the event loop immediately (#49068)
            dlgbase->cancel();
        }
        else
        {
            kdWarning() << "closeChildDialogs: not a KDialogBase! Don't use QDialogs in KDE! " << static_cast<QWidget*>(dlg) << endl;
            static_cast<QWidget*>(dlg)->hide();
        }
    }
    delete dlgs;
    d->m_dialogsAllowed = false;
}

bool KHTMLView::dialogsAllowed() {
    bool allowed = d->m_dialogsAllowed;
    KHTMLPart* p = m_part->parentPart();
    if (p && p->view())
        allowed &= p->view()->dialogsAllowed();
    return allowed;
}

void KHTMLView::closeEvent( QCloseEvent* ev )
{
    closeChildDialogs();
    QScrollView::closeEvent( ev );
}

//
// Event Handling
//
/////////////////

void KHTMLView::viewportMousePressEvent( QMouseEvent *_mouse )
{
    if(!m_part->xmlDocImpl()) return;
    if (d->possibleTripleClick)
    {
        viewportMouseDoubleClickEvent( _mouse ); // it handles triple clicks too
        return;
    }

    int xm, ym;
    viewportToContents(_mouse->x(), _mouse->y(), xm, ym);

    //kdDebug( 6000 ) << "\nmousePressEvent: x=" << xm << ", y=" << ym << endl;

    d->isDoubleClick = false;

    DOM::NodeImpl::MouseEvent mev( _mouse->stateAfter(), DOM::NodeImpl::MousePress );
    m_part->xmlDocImpl()->prepareMouseEvent( false, xm, ym, &mev );

    // Qt bug: sometimes Qt sends us events that should be sent
    // to the widget instead
    if (  mev.innerNode.handle() && mev.innerNode.handle()->renderer() &&
         mev.innerNode.handle()->renderer()->isWidget() )
        return;

    if (d->clickCount > 0 &&
        QPoint(d->clickX-xm,d->clickY-ym).manhattanLength() <= QApplication::startDragDistance())
	d->clickCount++;
    else {
	d->clickCount = 1;
	d->clickX = xm;
	d->clickY = ym;
    }

    bool swallowEvent = dispatchMouseEvent(EventImpl::MOUSEDOWN_EVENT,mev.innerNode.handle(),true,
                                           d->clickCount,_mouse,true,DOM::NodeImpl::MousePress);
    if (mev.innerNode.handle())
	mev.innerNode.handle()->setPressed();

    if (!swallowEvent) {
	khtml::MousePressEvent event( _mouse, xm, ym, mev.url, mev.target, mev.innerNode );
	QApplication::sendEvent( m_part, &event );

	emit m_part->nodeActivated(mev.innerNode);
    }
}

void KHTMLView::viewportMouseDoubleClickEvent( QMouseEvent *_mouse )
{
    if(!m_part->xmlDocImpl()) return;

    int xm, ym;
    viewportToContents(_mouse->x(), _mouse->y(), xm, ym);

    kdDebug( 6000 ) << "mouseDblClickEvent: x=" << xm << ", y=" << ym << endl;

    d->isDoubleClick = true;

    DOM::NodeImpl::MouseEvent mev( _mouse->stateAfter(), DOM::NodeImpl::MouseDblClick );
    m_part->xmlDocImpl()->prepareMouseEvent( false, xm, ym, &mev );

    // Qt bug: sometimes Qt sends us events that should be sent
    // to the widget instead
    if (  mev.innerNode.handle() && mev.innerNode.handle()->renderer() &&
         mev.innerNode.handle()->renderer()->isWidget() )
        return;

    // We do the same thing as viewportMousePressEvent() here, since the DOM does not treat
    // single and double-click events as separate (only the detail, i.e. number of clicks differs)
    if (d->clickCount > 0 && d->clickX == xm && d->clickY == ym) // ### support mouse threshold
	d->clickCount++;
    else {
	d->clickCount = 1;
	d->clickX = xm;
	d->clickY = ym;
    }
    kdDebug() << "KHTMLView::viewportMouseDoubleClickEvent clickCount=" << d->clickCount << endl;
    bool swallowEvent = dispatchMouseEvent(EventImpl::MOUSEDOWN_EVENT,mev.innerNode.handle(),true,
                                           d->clickCount,_mouse,true,DOM::NodeImpl::MouseDblClick);

    if (mev.innerNode.handle())
	mev.innerNode.handle()->setPressed();

    if (!swallowEvent) {
	khtml::MouseDoubleClickEvent event( _mouse, xm, ym, mev.url, mev.target, mev.innerNode, d->clickCount );
	QApplication::sendEvent( m_part, &event );
    }

    d->possibleTripleClick=true;
    QTimer::singleShot(QApplication::doubleClickInterval(),this,SLOT(tripleClickTimeout()));
}

void KHTMLView::tripleClickTimeout()
{
    d->possibleTripleClick = false;
    d->clickCount = 0;
}

void KHTMLView::viewportMouseMoveEvent( QMouseEvent * _mouse )
{

    if(!m_part->xmlDocImpl()) return;

    int xm, ym;
    viewportToContents(_mouse->x(), _mouse->y(), xm, ym);

    DOM::NodeImpl::MouseEvent mev( _mouse->stateAfter(), DOM::NodeImpl::MouseMove );
    m_part->xmlDocImpl()->prepareMouseEvent( false, xm, ym, &mev );

    // Qt bug: sometimes Qt sends us events that should be sent
    // to the widget instead
    if (  mev.innerNode.handle() && mev.innerNode.handle()->renderer() &&
         mev.innerNode.handle()->renderer()->isWidget() )
        return;

    bool swallowEvent = dispatchMouseEvent(EventImpl::MOUSEMOVE_EVENT,mev.innerNode.handle(),false,
                                           0,_mouse,true,DOM::NodeImpl::MouseMove);

    if (d->clickCount > 0 &&
        QPoint(d->clickX-xm,d->clickY-ym).manhattanLength() > QApplication::startDragDistance()) {
	d->clickCount = 0;  // moving the mouse outside the threshold invalidates the click
    }

    // execute the scheduled script. This is to make sure the mouseover events come after the mouseout events
    m_part->executeScheduledScript();

    khtml::RenderObject* r = mev.innerNode.handle() ? mev.innerNode.handle()->renderer() : 0;
    khtml::RenderStyle* style = (r && r->style()) ? r->style() : 0;
    QCursor c;
    switch ( style ? style->cursor() : CURSOR_AUTO) {
    case CURSOR_AUTO:
        if ( mev.url.length() && m_part->settings()->changeCursor() )
            c = m_part->urlCursor();

        if (r && r->isFrameSet() && !static_cast<RenderFrameSet*>(r)->noResize())
            c = QCursor(static_cast<RenderFrameSet*>(r)->cursorShape());

        break;
    case CURSOR_CROSS:
        c = KCursor::crossCursor();
        break;
    case CURSOR_POINTER:
        c = m_part->urlCursor();
        break;
    case CURSOR_PROGRESS:
        c = KCursor::workingCursor();
        break;
    case CURSOR_MOVE:
        c = KCursor::sizeAllCursor();
        break;
    case CURSOR_E_RESIZE:
    case CURSOR_W_RESIZE:
        c = KCursor::sizeHorCursor();
        break;
    case CURSOR_N_RESIZE:
    case CURSOR_S_RESIZE:
        c = KCursor::sizeVerCursor();
        break;
    case CURSOR_NE_RESIZE:
    case CURSOR_SW_RESIZE:
        c = KCursor::sizeBDiagCursor();
        break;
    case CURSOR_NW_RESIZE:
    case CURSOR_SE_RESIZE:
        c = KCursor::sizeFDiagCursor();
        break;
    case CURSOR_TEXT:
        c = KCursor::ibeamCursor();
        break;
    case CURSOR_WAIT:
        c = KCursor::waitCursor();
        break;
    case CURSOR_HELP:
        c = KCursor::whatsThisCursor();
        break;
    case CURSOR_DEFAULT:
        break;
    }

    if ( viewport()->cursor().handle() != c.handle() ) {
        if( c.handle() == KCursor::arrowCursor().handle()) {
            for (KHTMLPart* p = m_part; p; p = p->parentPart())
                p->view()->viewport()->unsetCursor();
        }
        else
            viewport()->setCursor( c );
    }

    d->prevMouseX = xm;
    d->prevMouseY = ym;

    if (!swallowEvent) {
        khtml::MouseMoveEvent event( _mouse, xm, ym, mev.url, mev.target, mev.innerNode );
        QApplication::sendEvent( m_part, &event );
    }
}

void KHTMLView::viewportMouseReleaseEvent( QMouseEvent * _mouse )
{
    if ( !m_part->xmlDocImpl() ) return;

    int xm, ym;
    viewportToContents(_mouse->x(), _mouse->y(), xm, ym);

    //kdDebug( 6000 ) << "\nmouseReleaseEvent: x=" << xm << ", y=" << ym << endl;

    DOM::NodeImpl::MouseEvent mev( _mouse->stateAfter(), DOM::NodeImpl::MouseRelease );
    m_part->xmlDocImpl()->prepareMouseEvent( false, xm, ym, &mev );

    // Qt bug: sometimes Qt sends us events that should be sent
    // to the widget instead
    if (  mev.innerNode.handle() && mev.innerNode.handle()->renderer() &&
         mev.innerNode.handle()->renderer()->isWidget() )
        return;

    bool swallowEvent = dispatchMouseEvent(EventImpl::MOUSEUP_EVENT,mev.innerNode.handle(),true,
                                           d->clickCount,_mouse,false,DOM::NodeImpl::MouseRelease);

    if (d->clickCount > 0 &&
        QPoint(d->clickX-xm,d->clickY-ym).manhattanLength() <= QApplication::startDragDistance())
	dispatchMouseEvent(EventImpl::CLICK_EVENT,mev.innerNode.handle(),true,
			   d->clickCount,_mouse,true,DOM::NodeImpl::MouseRelease);

    if (mev.innerNode.handle())
	mev.innerNode.handle()->setPressed(false);

    if (!swallowEvent) {
	khtml::MouseReleaseEvent event( _mouse, xm, ym, mev.url, mev.target, mev.innerNode );
	QApplication::sendEvent( m_part, &event );
    }
}

void KHTMLView::keyPressEvent( QKeyEvent *_ke )
{

    if (m_part->xmlDocImpl()) {
        if (m_part->xmlDocImpl()->focusNode())
            if (m_part->xmlDocImpl()->focusNode()->dispatchKeyEvent(_ke))
            {
                _ke->accept();
                return;
            }
        if (!_ke->text().isNull() && m_part->xmlDocImpl()->getHTMLEventListener(EventImpl::KHTML_KEYDOWN_EVENT))
            if (m_part->xmlDocImpl()->documentElement()->dispatchKeyEvent(_ke))
            {
                _ke->accept();
                return;
            }
    }
    int offs = (clipper()->height() < 30) ? clipper()->height() : 30;
    if (_ke->state() & Qt::ShiftButton)
      switch(_ke->key())
        {
        case Key_Space:
            if ( d->vmode == QScrollView::AlwaysOff )
                _ke->accept();
            else
                scrollBy( 0, -clipper()->height() - offs );
            break;

        case Key_Down:
        case Key_J:
            d->adjustScroller(this, KHTMLViewPrivate::ScrollDown, KHTMLViewPrivate::ScrollUp);
            break;

        case Key_Up:
        case Key_K:
            d->adjustScroller(this, KHTMLViewPrivate::ScrollUp, KHTMLViewPrivate::ScrollDown);
            break;

        case Key_Left:
        case Key_H:
            d->adjustScroller(this, KHTMLViewPrivate::ScrollLeft, KHTMLViewPrivate::ScrollRight);
            break;

        case Key_Right:
        case Key_L:
            d->adjustScroller(this, KHTMLViewPrivate::ScrollRight, KHTMLViewPrivate::ScrollLeft);
            break;
        }
    else
        switch ( _ke->key() )
        {
        case Key_Down:
        case Key_J:
            if ( d->vmode == QScrollView::AlwaysOff )
                _ke->accept();
            else {
                if (d->scrollTimerId)
                    d->newScrollTimer(this, 0);
                else
                    scrollBy( 0, 10 );
            }
            break;

        case Key_Space:
        case Key_Next:
            if ( d->vmode == QScrollView::AlwaysOff )
                _ke->accept();
            else
                scrollBy( 0, clipper()->height() - offs );
            break;

        case Key_Up:
        case Key_K:
            if ( d->vmode == QScrollView::AlwaysOff )
                _ke->accept();
            else {
                if (d->scrollTimerId)
                    d->newScrollTimer(this, 0);
                else
                    scrollBy( 0, -10 );
            }
            break;

        case Key_Prior:
            if ( d->vmode == QScrollView::AlwaysOff )
                _ke->accept();
            else
                scrollBy( 0, -clipper()->height() + offs );
            break;
        case Key_Right:
        case Key_L:
            if ( d->hmode == QScrollView::AlwaysOff )
                _ke->accept();
            else {
                if (d->scrollTimerId)
                    d->newScrollTimer(this, 0);
                else
                    scrollBy( 10, 0 );
            }
            break;
        case Key_Left:
        case Key_H:
            if ( d->hmode == QScrollView::AlwaysOff )
                _ke->accept();
            else {
                if (d->scrollTimerId)
                    d->newScrollTimer(this, 0);
                else
                    scrollBy( -10, 0 );
            }
            break;
        case Key_Enter:
        case Key_Return:
	    // ### FIXME:
	    // move this code to HTMLAnchorElementImpl::setPressed(false),
	    // or even better to HTMLAnchorElementImpl::event()
            if (m_part->xmlDocImpl()) {
		NodeImpl *n = m_part->xmlDocImpl()->focusNode();
		if (n)
		    n->setActive();
		d->originalNode = n;
	    }
            break;
        case Key_Home:
            if ( d->vmode == QScrollView::AlwaysOff )
                _ke->accept();
            else
                setContentsPos( 0, 0 );
            break;
        case Key_End:
            if ( d->vmode == QScrollView::AlwaysOff )
                _ke->accept();
            else
                setContentsPos( 0, contentsHeight() - visibleHeight() );
            break;
        case Key_Shift:
            // what are you doing here?
	    _ke->ignore();
            return;
        default:
            if (d->scrollTimerId)
                d->newScrollTimer(this, 0);
	    _ke->ignore();
            return;
        }
    _ke->accept();
}

void KHTMLView::keyReleaseEvent(QKeyEvent *_ke)
{
    if(m_part->xmlDocImpl() && m_part->xmlDocImpl()->focusNode()) {
        // Qt is damn buggy. we receive release events from our child
        // widgets. therefore, do not support keyrelease event on generic
        // nodes for now until we found  a workaround for the Qt bugs. (Dirk)
//         if (m_part->xmlDocImpl()->focusNode()->dispatchKeyEvent(_ke)) {
//             _ke->accept();
//             return;
//         }
//        QScrollView::keyReleaseEvent(_ke);
        Q_UNUSED(_ke);
    }
}

void KHTMLView::contentsContextMenuEvent ( QContextMenuEvent * /*ce*/ )
{
// ### what kind of c*** is that ?
#if 0
    if (!m_part->xmlDocImpl()) return;
    int xm = _ce->x();
    int ym = _ce->y();

    DOM::NodeImpl::MouseEvent mev( _ce->state(), DOM::NodeImpl::MouseMove ); // ### not a mouse event!
    m_part->xmlDocImpl()->prepareMouseEvent( xm, ym, &mev );

    NodeImpl *targetNode = mev.innerNode.handle();
    if (targetNode && targetNode->renderer() && targetNode->renderer()->isWidget()) {
        int absx = 0;
        int absy = 0;
        targetNode->renderer()->absolutePosition(absx,absy);
        QPoint pos(xm-absx,ym-absy);

        QWidget *w = static_cast<RenderWidget*>(targetNode->renderer())->widget();
        QContextMenuEvent cme(_ce->reason(),pos,_ce->globalPos(),_ce->state());
        setIgnoreEvents(true);
        QApplication::sendEvent(w,&cme);
        setIgnoreEvents(false);
    }
#endif
}

bool KHTMLView::focusNextPrevChild( bool next )
{
    // Now try to find the next child
    if (m_part->xmlDocImpl()) {
        focusNextPrevNode(next);
        if (m_part->xmlDocImpl()->focusNode() != 0)
            return true; // focus node found
    }

    // If we get here, there is no next/previous child to go to, so pass up to the next/previous child in our parent
    if (m_part->parentPart() && m_part->parentPart()->view()) {
        return m_part->parentPart()->view()->focusNextPrevChild(next);
    }

    return QWidget::focusNextPrevChild(next);
}

void KHTMLView::doAutoScroll()
{
    QPoint pos = QCursor::pos();
    pos = viewport()->mapFromGlobal( pos );

    int xm, ym;
    viewportToContents(pos.x(), pos.y(), xm, ym);

    pos = QPoint(pos.x() - viewport()->x(), pos.y() - viewport()->y());
    if ( (pos.y() < 0) || (pos.y() > visibleHeight()) ||
         (pos.x() < 0) || (pos.x() > visibleWidth()) )
    {
        ensureVisible( xm, ym, 0, 5 );
    }
}

DOM::NodeImpl *KHTMLView::nodeUnderMouse() const
{
    return d->underMouse;
}

bool KHTMLView::scrollTo(const QRect &bounds)
{
    d->scrollingSelf = true; // so scroll events get ignored

    int x, y, xe, ye;
    x = bounds.left();
    y = bounds.top();
    xe = bounds.right();
    ye = bounds.bottom();

    //kdDebug(6000)<<"scrolling coords: x="<<x<<" y="<<y<<" width="<<xe-x<<" height="<<ye-y<<endl;

    int deltax;
    int deltay;

    int curHeight = visibleHeight();
    int curWidth = visibleWidth();

    if (ye-y>curHeight-d->borderY)
	ye  = y + curHeight - d->borderY;

    if (xe-x>curWidth-d->borderX)
	xe = x + curWidth - d->borderX;

    // is xpos of target left of the view's border?
    if (x < contentsX() + d->borderX )
            deltax = x - contentsX() - d->borderX;
    // is xpos of target right of the view's right border?
    else if (xe + d->borderX > contentsX() + curWidth)
            deltax = xe + d->borderX - ( contentsX() + curWidth );
    else
        deltax = 0;

    // is ypos of target above upper border?
    if (y < contentsY() + d->borderY)
            deltay = y - contentsY() - d->borderY;
    // is ypos of target below lower border?
    else if (ye + d->borderY > contentsY() + curHeight)
            deltay = ye + d->borderY - ( contentsY() + curHeight );
    else
        deltay = 0;

    int maxx = curWidth-d->borderX;
    int maxy = curHeight-d->borderY;

    int scrollX,scrollY;

    scrollX = deltax > 0 ? (deltax > maxx ? maxx : deltax) : deltax == 0 ? 0 : (deltax>-maxx ? deltax : -maxx);
    scrollY = deltay > 0 ? (deltay > maxy ? maxy : deltay) : deltay == 0 ? 0 : (deltay>-maxy ? deltay : -maxy);

    if (contentsX() + scrollX < 0)
	scrollX = -contentsX();
    else if (contentsWidth() - visibleWidth() - contentsX() < scrollX)
	scrollX = contentsWidth() - visibleWidth() - contentsX();

    if (contentsY() + scrollY < 0)
	scrollY = -contentsY();
    else if (contentsHeight() - visibleHeight() - contentsY() < scrollY)
	scrollY = contentsHeight() - visibleHeight() - contentsY();

    scrollBy(scrollX, scrollY);



    // generate abs(scroll.)
    if (scrollX<0)
        scrollX=-scrollX;
    if (scrollY<0)
        scrollY=-scrollY;

    d->scrollingSelf = false;

    if ( (scrollX!=maxx) && (scrollY!=maxy) )
	return true;
    else return false;

}

void KHTMLView::focusNextPrevNode(bool next)
{
    // Sets the focus node of the document to be the node after (or if next is false, before) the current focus node.
    // Only nodes that are selectable (i.e. for which isSelectable() returns true) are taken into account, and the order
    // used is that specified in the HTML spec (see DocumentImpl::nextFocusNode() and DocumentImpl::previousFocusNode()
    // for details).

    DocumentImpl *doc = m_part->xmlDocImpl();
    NodeImpl *oldFocusNode = doc->focusNode();
    NodeImpl *newFocusNode;

    // Find the next/previous node from the current one
    if (next)
        newFocusNode = doc->nextFocusNode(oldFocusNode);
    else
        newFocusNode = doc->previousFocusNode(oldFocusNode);

    // If there was previously no focus node and the user has scrolled the document, then instead of picking the first
    // focusable node in the document, use the first one that lies within the visible area (if possible).
    if (!oldFocusNode && newFocusNode && d->scrollBarMoved) {

      kdDebug(6000) << " searching for visible link" << endl;

        bool visible = false;
        NodeImpl *toFocus = newFocusNode;
        while (!visible && toFocus) {
            QRect focusNodeRect = toFocus->getRect();
            if ((focusNodeRect.left() > contentsX()) && (focusNodeRect.right() < contentsX() + visibleWidth()) &&
                (focusNodeRect.top() > contentsY()) && (focusNodeRect.bottom() < contentsY() + visibleHeight())) {
                // toFocus is visible in the contents area
                visible = true;
            }
            else {
                // toFocus is _not_ visible in the contents area, pick the next node
                if (next)
                    toFocus = doc->nextFocusNode(toFocus);
                else
                    toFocus = doc->previousFocusNode(toFocus);
            }
        }

        if (toFocus)
            newFocusNode = toFocus;
    }

    d->scrollBarMoved = false;

    if (!newFocusNode)
      {
        // No new focus node, scroll to bottom or top depending on next
        if (next)
            scrollTo(QRect(contentsX()+visibleWidth()/2,contentsHeight(),0,0));
        else
            scrollTo(QRect(contentsX()+visibleWidth()/2,0,0,0));
    }
    else
    // Scroll the view as necessary to ensure that the new focus node is visible
    {
      if (oldFocusNode)
	{
	  if (!scrollTo(newFocusNode->getRect()))
	    return;
	}
      else
	{
	  ensureVisible(contentsX(), next?0:contentsHeight());
	  //return;
	}
    }
    // Set focus node on the document
    m_part->xmlDocImpl()->setFocusNode(newFocusNode);
    emit m_part->nodeActivated(Node(newFocusNode));

#if 0
    if (newFocusNode) {

        // this does not belong here. it should run a query on the tree (Dirk)
        // I'll fix this very particular part of the code soon when I cleaned
        // up the positioning code
        // If the newly focussed node is a link, notify the part

        HTMLAnchorElementImpl *anchor = 0;
        if ((newFocusNode->id() == ID_A || newFocusNode->id() == ID_AREA))
            anchor = static_cast<HTMLAnchorElementImpl *>(newFocusNode);

        if (anchor && !anchor->areaHref().isNull())
            m_part->overURL(anchor->areaHref().string(), 0);
        else
            m_part->overURL(QString(), 0);
    }
#endif
}

void KHTMLView::setMediaType( const QString &medium )
{
    m_medium = medium;
}

QString KHTMLView::mediaType() const
{
    return m_medium;
}

void KHTMLView::setWidgetVisible(RenderWidget* w, bool vis)
{
    if (vis) {
        assert(w->widget());
        d->visibleWidgets.replace(w, w->widget());
    }
    else
        d->visibleWidgets.remove(w);
}

void KHTMLView::print()
{
#ifdef ENABLE_PRINTING // This code is not needed for kdenox for now
    if(!m_part->xmlDocImpl()) return;
    khtml::RenderRoot *root = static_cast<khtml::RenderRoot *>(m_part->xmlDocImpl()->renderer());
    if(!root) return;

    // this only works on Unix - we assume 72dpi
    KPrinter *printer = new KPrinter(QPrinter::PrinterResolution);
    printer->addDialogPage(new KHTMLPrintSettings());
    if(printer->setup(this)) {
        viewport()->setCursor( waitCursor ); // only viewport(), no QApplication::, otherwise we get the busy cursor in kdeprint's dialogs
        // set up KPrinter
        printer->setFullPage(false);
        printer->setCreator("KDE 3.0 HTML Library");
        QString docname = m_part->xmlDocImpl()->URL();
        if ( !docname.isEmpty() )
	    printer->setDocName(KStringHandler::csqueeze(docname, 80));

        QPainter *p = new QPainter;
        p->begin( printer );
        khtml::setPrintPainter( p );

        m_part->xmlDocImpl()->setPaintDevice( printer );
        QString oldMediaType = mediaType();
        setMediaType( "print" );
        // We ignore margin settings for html and body when printing
        // and use the default margins from the print-system
        // (In Qt 3.0.x the default margins are hardcoded in Qt)
        m_part->xmlDocImpl()->setPrintStyleSheet( printer->option("app-khtml-printfriendly") == "true" ?
                                                  "* { background-image: none !important;"
                                                  "    background-color: white !important;"
                                                  "    color: black !important; }"
						  "body { margin: 0px !important; }"
						  "html { margin: 0px !important; }" :
						  "body { margin: 0px !important; }"
						  "html { margin: 0px !important; }"
						  );

        QPaintDeviceMetrics metrics( printer );

        // this is a simple approximation... we layout the document
        // according to the width of the page, then just cut
        // pages without caring about the content. We should do better
        // in the future, but for the moment this is better than no
        // printing support
        kdDebug(6000) << "printing: physical page width = " << metrics.width()
                      << " height = " << metrics.height() << endl;
        root->setPrintingMode(true);
        root->setWidth(metrics.width());

        m_part->xmlDocImpl()->styleSelector()->computeFontSizes(&metrics, 100);
        m_part->xmlDocImpl()->updateStyleSelector();
        root->setPrintImages( printer->option("app-khtml-printimages") == "true");
        root->setLayouted( false );
        root->setMinMaxKnown( false );
        root->layout();

        bool printHeader = (printer->option("app-khtml-printheader") == "true");

        int headerHeight = 0;
        QFont headerFont("helvetica", 8);

        QString headerLeft = KGlobal::locale()->formatDate(QDate::currentDate(),true);
        QString headerMid = docname;
        QString headerRight;

        if (printHeader)
        {
           p->setFont(headerFont);
           headerHeight = (p->fontMetrics().lineSpacing() * 3) / 2;
        }

        // ok. now print the pages.
        kdDebug(6000) << "printing: html page width = " << root->docWidth()
                      << " height = " << root->docHeight() << endl;
        kdDebug(6000) << "printing: margins left = " << printer->margins().width()
                      << " top = " << printer->margins().height() << endl;
        kdDebug(6000) << "printing: paper width = " << metrics.width()
                      << " height = " << metrics.height() << endl;
        // if the width is too large to fit on the paper we just scale
        // the whole thing.
        int pageHeight = metrics.height();
        int pageWidth = metrics.width();
        p->setClipRect(0,0, pageWidth, pageHeight);

        pageHeight -= headerHeight;

        bool scalePage = false;
        double scale = 0.0;
#ifndef QT_NO_TRANSFORMATIONS
        if(root->docWidth() > metrics.width()) {
            scalePage = true;
            scale = ((double) metrics.width())/((double) root->docWidth());
            pageHeight = (int) (pageHeight/scale);
            pageWidth = (int) (pageWidth/scale);
            headerHeight = (int) (headerHeight/scale);
        }
#endif
        kdDebug(6000) << "printing: scaled html width = " << pageWidth
                      << " height = " << pageHeight << endl;

        // Squeeze header to make it it on the page.
        if (printHeader)
        {
            int available_width = metrics.width() - 10 -
                2 * QMAX(p->boundingRect(0, 0, metrics.width(), p->fontMetrics().lineSpacing(), Qt::AlignLeft, headerLeft).width(),
                         p->boundingRect(0, 0, metrics.width(), p->fontMetrics().lineSpacing(), Qt::AlignLeft, headerRight).width());
            if (available_width < 150)
               available_width = 150;
            int mid_width;
            int squeeze = 120;
            do {
                headerMid = KStringHandler::csqueeze(docname, squeeze);
                mid_width = p->boundingRect(0, 0, metrics.width(), p->fontMetrics().lineSpacing(), Qt::AlignLeft, headerMid).width();
                squeeze -= 10;
            } while (mid_width > available_width);
        }

        int top = 0;
        int page = 1;
        while(top < root->docHeight()) {
            if(top > 0) printer->newPage();
            if (printHeader)
            {
                int dy = p->fontMetrics().lineSpacing();
                p->setPen(Qt::black);
                p->setFont(headerFont);

                headerRight = QString("#%1").arg(page);

                p->drawText(0, 0, metrics.width(), dy, Qt::AlignLeft, headerLeft);
                p->drawText(0, 0, metrics.width(), dy, Qt::AlignHCenter, headerMid);
                p->drawText(0, 0, metrics.width(), dy, Qt::AlignRight, headerRight);
            }

#ifndef QT_NO_TRANSFORMATIONS
            if (scalePage)
                p->scale(scale, scale);
#endif
            p->translate(0, headerHeight-top);

            root->setTruncatedAt(top+pageHeight);

            root->paint(p, 0, top, pageWidth, pageHeight, 0, 0);
            if (top + pageHeight >= root->docHeight())
                break; // Stop if we have printed everything

            top = root->truncatedAt();
            p->resetXForm();
            page++;
        }

        p->end();
        delete p;

        // and now reset the layout to the usual one...
        root->setPrintingMode(false);
        khtml::setPrintPainter( 0 );
        setMediaType( oldMediaType );
        m_part->xmlDocImpl()->setPaintDevice( this );
        m_part->xmlDocImpl()->styleSelector()->computeFontSizes(m_part->xmlDocImpl()->paintDeviceMetrics(), m_part->zoomFactor());
        m_part->xmlDocImpl()->updateStyleSelector();
        viewport()->unsetCursor();
    }
    delete printer;
#endif
}

void KHTMLView::slotPaletteChanged()
{
    if(!m_part->xmlDocImpl()) return;
    DOM::DocumentImpl *document = m_part->xmlDocImpl();
    if (!document->isHTMLDocument()) return;
    khtml::RenderRoot *root = static_cast<khtml::RenderRoot *>(document->renderer());
    if(!root) return;
    root->style()->resetPalette();
    NodeImpl *body = static_cast<HTMLDocumentImpl*>(document)->body();
    if(!body) return;
    body->setChanged(true);
    body->recalcStyle( NodeImpl::Force );
}

void KHTMLView::paint(QPainter *p, const QRect &rc, int yOff, bool *more)
{
    if(!m_part->xmlDocImpl()) return;
    khtml::RenderRoot *root = static_cast<khtml::RenderRoot *>(m_part->xmlDocImpl()->renderer());
    if(!root) return;

    m_part->xmlDocImpl()->setPaintDevice(p->device());
    root->setPrintingMode(true);
    root->setWidth(rc.width());

    p->save();
    p->setClipRect(rc);
    p->translate(rc.left(), rc.top());
    double scale = ((double) rc.width()/(double) root->docWidth());
    int height = (int) ((double) rc.height() / scale);
#ifndef QT_NO_TRANSFORMATIONS
    p->scale(scale, scale);
#endif

    root->paint(p, 0, yOff, root->docWidth(), height, 0, 0);
    if (more)
        *more = yOff + height < root->docHeight();
    p->restore();

    root->setPrintingMode(false);
    m_part->xmlDocImpl()->setPaintDevice( this );
}


void KHTMLView::useSlowRepaints()
{
    kdDebug(0) << "slow repaints requested" << endl;
    d->useSlowRepaints = true;
    setStaticBackground(true);
}


void KHTMLView::setVScrollBarMode ( ScrollBarMode mode )
{
#ifndef KHTML_NO_SCROLLBARS
    d->vmode = mode;
    QScrollView::setVScrollBarMode(mode);
#else
    Q_UNUSED( mode );
#endif
}

void KHTMLView::setHScrollBarMode ( ScrollBarMode mode )
{
#ifndef KHTML_NO_SCROLLBARS
    d->hmode = mode;
    QScrollView::setHScrollBarMode(mode);
#else
    Q_UNUSED( mode );
#endif
}

void KHTMLView::restoreScrollBar()
{
    int ow = visibleWidth();
    QScrollView::setVScrollBarMode(d->vmode);
    if (visibleWidth() != ow)
        layout();
    d->prevScrollbarVisible = verticalScrollBar()->isVisible();
}

QStringList KHTMLView::formCompletionItems(const QString &name) const
{
    if (!m_part->settings()->isFormCompletionEnabled())
        return QStringList();
    if (!d->formCompletions)
        d->formCompletions = new KSimpleConfig(locateLocal("data", "khtml/formcompletions"));
    return d->formCompletions->readListEntry(name);
}

void KHTMLView::clearCompletionHistory(const QString& name)
{
    if (!d->formCompletions)
    {
        d->formCompletions = new KSimpleConfig(locateLocal("data", "khtml/formcompletions"));
    }
    d->formCompletions->writeEntry(name, "");
    d->formCompletions->sync();
}

void KHTMLView::addFormCompletionItem(const QString &name, const QString &value)
{
    if (!m_part->settings()->isFormCompletionEnabled())
        return;
    // don't store values that are all numbers or just numbers with
    // dashes or spaces as those are likely credit card numbers or
    // something similar
    bool cc_number(true);
    for (unsigned int i = 0; i < value.length(); ++i)
    {
      QChar c(value[i]);
      if (!c.isNumber() && c != '-' && !c.isSpace())
      {
        cc_number = false;
        break;
      }
    }
    if (cc_number)
      return;
    QStringList items = formCompletionItems(name);
    if (!items.contains(value))
        items.prepend(value);
    while ((int)items.count() > m_part->settings()->maxFormCompletionItems())
        items.remove(items.fromLast());
    d->formCompletions->writeEntry(name, items);
}

bool KHTMLView::dispatchMouseEvent(int eventId, DOM::NodeImpl *targetNode, bool cancelable,
				   int detail,QMouseEvent *_mouse, bool setUnder,
				   int mouseEventType)
{
    if (d->underMouse)
	d->underMouse->deref();
    d->underMouse = targetNode;
    if (d->underMouse)
	d->underMouse->ref();

    int exceptioncode = 0;
    int mx, my;
    viewportToContents(_mouse->x(), _mouse->y(), mx, my);
    // clientX and clientY are in viewport coordinates
    // At least the JS code wants event.[xy]/event.client[XY] to be in viewport coords.
    // [that's not the same as _mouse->[xy](), since we use the clipper]
    int clientX = mx - contentsX();
    int clientY = my - contentsY();
    int screenX = _mouse->globalX();
    int screenY = _mouse->globalY();
    int button = -1;
    switch (_mouse->button()) {
	case LeftButton:
	    button = 0;
	    break;
	case MidButton:
	    button = 1;
	    break;
	case RightButton:
	    button = 2;
	    break;
	default:
	    break;
    }
    bool ctrlKey = (_mouse->state() & ControlButton);
    bool altKey = (_mouse->state() & AltButton);
    bool shiftKey = (_mouse->state() & ShiftButton);
    bool metaKey = (_mouse->state() & MetaButton);

    // mouseout/mouseover
    if (setUnder && (d->prevMouseX != mx || d->prevMouseY != my)) {

        // ### this code sucks. we should save the oldUnder instead of calculating
        // it again. calculating is expensive! (Dirk)
        NodeImpl *oldUnder = 0;
	if (d->prevMouseX >= 0 && d->prevMouseY >= 0) {
	    NodeImpl::MouseEvent mev( _mouse->stateAfter(), static_cast<NodeImpl::MouseEventType>(mouseEventType));
	    m_part->xmlDocImpl()->prepareMouseEvent( true, d->prevMouseX, d->prevMouseY, &mev );
	    oldUnder = mev.innerNode.handle();
	}
	if (oldUnder != targetNode) {
	    // send mouseout event to the old node
	    if (oldUnder){
		oldUnder->ref();
		MouseEventImpl *me = new MouseEventImpl(EventImpl::MOUSEOUT_EVENT,
							true,true,m_part->xmlDocImpl()->defaultView(),
							0,screenX,screenY,clientX,clientY,
							ctrlKey,altKey,shiftKey,metaKey,
							button,targetNode);
		me->ref();
		oldUnder->dispatchEvent(me,exceptioncode,true);
		me->deref();
	    }

	    // send mouseover event to the new node
	    if (targetNode) {
		MouseEventImpl *me = new MouseEventImpl(EventImpl::MOUSEOVER_EVENT,
							true,true,m_part->xmlDocImpl()->defaultView(),
							0,screenX,screenY,clientX,clientY,
							ctrlKey,altKey,shiftKey,metaKey,
							button,oldUnder);

		me->ref();
		targetNode->dispatchEvent(me,exceptioncode,true);
		me->deref();
	    }

            if (oldUnder)
                oldUnder->deref();
        }
    }

    bool swallowEvent = false;

    if (targetNode) {
	// send the actual event
	MouseEventImpl *me = new MouseEventImpl(static_cast<EventImpl::EventId>(eventId),
						true,cancelable,m_part->xmlDocImpl()->defaultView(),
						detail,screenX,screenY,clientX,clientY,
						ctrlKey,altKey,shiftKey,metaKey,
						button,0);
	me->ref();
	targetNode->dispatchEvent(me,exceptioncode,true);
        if (me->defaultHandled() || me->defaultPrevented())
            swallowEvent = true;
	me->deref();

        if( eventId == EventImpl::MOUSEDOWN_EVENT ) {
            if (targetNode->isSelectable())
                m_part->xmlDocImpl()->setFocusNode(targetNode);
            else
                m_part->xmlDocImpl()->setFocusNode(0);
        }
    }

    return swallowEvent;
}

void KHTMLView::setIgnoreWheelEvents( bool e )
{
    d->ignoreWheelEvents = e;
}

#ifndef QT_NO_WHEELEVENT

void KHTMLView::viewportWheelEvent(QWheelEvent* e)
{
    if ( ( e->state() & ShiftButton ) == ShiftButton )
    {
        emit zoomView( e->delta() );
        e->accept();
    }
    else if ( d->ignoreWheelEvents && !verticalScrollBar()->isVisible()
                && m_part->parentPart() ) {
        if ( m_part->parentPart()->view() )
            m_part->parentPart()->view()->wheelEvent( e );
        e->ignore();
    }
    else if ( d->vmode == QScrollView::AlwaysOff ) {
        e->accept();
    }
    else {
        d->scrollBarMoved = true;
        QScrollView::viewportWheelEvent( e );

        QMouseEvent *tempEvent = new QMouseEvent( QEvent::MouseMove, QPoint(-1,-1), QPoint(-1,-1), Qt::NoButton, e->state() );
        emit viewportMouseMoveEvent ( tempEvent );
        delete tempEvent;
    }

}
#endif

void KHTMLView::dragEnterEvent( QDragEnterEvent* ev )
{
    // Handle drops onto frames (#16820)
    // Drops on the main html part is handled by Konqueror (and shouldn't do anything
    // in e.g. kmail, so not handled here).
    if ( m_part->parentPart() )
    {
        // Duplicated from KonqView::eventFilter
        if ( QUriDrag::canDecode( ev ) )
        {
            KURL::List lstDragURLs;
            bool ok = KURLDrag::decode( ev, lstDragURLs );
            QObjectList *children = this->queryList( "QWidget" );

            if ( ok &&
                 !lstDragURLs.first().url().contains( "javascript:", false ) && // ### this looks like a hack to me
                 ev->source() != this &&
                 children &&
                 children->findRef( ev->source() ) == -1 )
                ev->acceptAction();

            delete children;
        }
    }
    QScrollView::dragEnterEvent( ev );
}

void KHTMLView::dropEvent( QDropEvent *ev )
{
    // Handle drops onto frames (#16820)
    // Drops on the main html part is handled by Konqueror (and shouldn't do anything
    // in e.g. kmail, so not handled here).
    if ( m_part->parentPart() )
    {
        KURL::List lstDragURLs;
        bool ok = KURLDrag::decode( ev, lstDragURLs );

        KHTMLPart* part = m_part->parentPart();
        while ( part && part->parentPart() )
            part = part->parentPart();
        KParts::BrowserExtension *ext = part->browserExtension();
        if ( ok && ext && lstDragURLs.first().isValid() )
            emit ext->openURLRequest( lstDragURLs.first() );
    }
    QScrollView::dropEvent( ev );
}

void KHTMLView::focusOutEvent( QFocusEvent *e )
{
    if(m_part) m_part->stopAutoScroll();
    QScrollView::focusOutEvent( e );
}

void KHTMLView::slotScrollBarMoved()
{
    if (!d->scrollingSelf)
        d->scrollBarMoved = true;
}

void KHTMLView::timerEvent ( QTimerEvent *e )
{
//    kdDebug() << "timer event " << e->timerId() << endl;
    if (e->timerId() == d->scrollTimerId) {
        switch (d->scrollDirection) {
            case KHTMLViewPrivate::ScrollDown:
                if (contentsY() + visibleHeight () >= contentsHeight())
                    d->newScrollTimer(this, 0);
                else
                    scrollBy( 0, d->scrollBy );
                break;
            case KHTMLViewPrivate::ScrollUp:
                if (contentsY() <= 0)
                    d->newScrollTimer(this, 0);
                else
                    scrollBy( 0, -d->scrollBy );
                break;
            case KHTMLViewPrivate::ScrollRight:
                if (contentsX() + visibleWidth () >= contentsWidth())
                    d->newScrollTimer(this, 0);
                else
                    scrollBy( d->scrollBy, 0 );
                break;
            case KHTMLViewPrivate::ScrollLeft:
                if (contentsX() <= 0)
                    d->newScrollTimer(this, 0);
                else
                    scrollBy( -d->scrollBy, 0 );
                break;
        }
        return;
    }
    if (e->timerId()==d->timerId)
    {
        d->firstRelayout = false;
        killTimer(d->timerId);

        d->dirtyLayout = true;
        d->layoutSchedulingEnabled=false;
        layout();
        d->layoutSchedulingEnabled=true;

        d->timerId = 0;


        //scheduleRepaint(contentsX(),contentsY(),visibleWidth(),visibleHeight());
	d->updateRect = QRect(contentsX(),contentsY(),visibleWidth(),visibleHeight());
    }

    if( m_part->xmlDocImpl() ) {
	DOM::DocumentImpl *document = m_part->xmlDocImpl();
	khtml::RenderRoot* root = static_cast<khtml::RenderRoot *>(document->renderer());

	if ( !root->layouted() ) {
	    killTimer(d->repaintTimerId);
	    d->repaintTimerId = 0;
	    scheduleRelayout();
	    return;
	}
    }

    setStaticBackground(d->useSlowRepaints);

//        kdDebug() << "scheduled repaint "<< d->repaintTimerId  << endl;
    killTimer(d->repaintTimerId);
    updateContents( d->updateRect );

    if (d->dirtyLayout && !d->visibleWidgets.isEmpty()) {
        d->dirtyLayout = false;

        QRect visibleRect(contentsX(), contentsY(), visibleWidth(), visibleHeight());
        QPtrList<RenderWidget> toRemove;
        QWidget* w;
        for (QPtrDictIterator<QWidget> it(d->visibleWidgets); it.current(); ++it) {
            int xp = 0, yp = 0;
            w = it.current();
            RenderWidget* rw = static_cast<RenderWidget*>( it.currentKey() );
            if (!rw->absolutePosition(xp, yp) ||
                !visibleRect.intersects(QRect(xp, yp, w->width(), w->height())))
                toRemove.append(rw);
        }
        for (RenderWidget* r = toRemove.first(); r; r = toRemove.next())
            if ( (w = d->visibleWidgets.take(r) ) )
                addChild(w, 0, -500000);
    }

    d->repaintTimerId = 0;
}

void KHTMLView::scheduleRelayout()
{
    if (!d->layoutSchedulingEnabled || d->timerId)
        return;

    d->timerId = startTimer( m_part->xmlDocImpl() && m_part->xmlDocImpl()->parsing()
                             ? 1000 : 0 );
}

void KHTMLView::scheduleRepaint(int x, int y, int w, int h)
{

    //kdDebug() << "scheduleRepaint(" << x << "," << y << "," << w << "," << h << ")" << endl;


    bool parsing = false;
    if( m_part->xmlDocImpl() ) {
        parsing = m_part->xmlDocImpl()->parsing();
    }

//     kdDebug() << "parsing " << parsing << endl;
//     kdDebug() << "complete " << d->complete << endl;

    int time;

    // if complete...
    if (d->complete)
        // ...repaint immediatly
        time = 0;
    else
    {
        if (parsing)
            // not complete and still parsing
            time = 300;
        else
            // not complete, not parsing, extend the timer if it exists
            // otherwise, repaint immediatly
            time = d->repaintTimerId ? 400 : 0;
    }

    if (d->repaintTimerId) {
        killTimer(d->repaintTimerId);
        d->updateRect = d->updateRect.unite(QRect(x,y,w,h));
    } else
        d->updateRect = QRect(x,y,w,h);

    d->repaintTimerId = startTimer( time );

//     kdDebug() << "starting timer " << time << endl;
}

void KHTMLView::complete()
{
//     kdDebug() << "KHTMLView::complete()" << endl;

    d->complete = true;

    // is there a relayout pending?
    if (d->timerId)
    {
//         kdDebug() << "requesting relayout now" << endl;
        // do it now
        killTimer(d->timerId);
        d->timerId = startTimer( 0 );
    }

    // is there a repaint pending?
    if (d->repaintTimerId)
    {
//         kdDebug() << "requesting repaint now" << endl;
        // do it now
        killTimer(d->repaintTimerId);
        d->repaintTimerId = startTimer( 1 );
    }
}