/**
 * This file is part of the KDE project.
 *
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 2000 Simon Hausmann <hausmann@kde.org>
 *           (C) 2000 Stefan Schimanski (1Stein@gmx.de)
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
 *
 * $Id: render_frames.cpp,v 1.144.2.6 2003/09/09 00:26:55 mueller Exp $
 */
//#define DEBUG_LAYOUT

#include "rendering/render_frames.h"
#include "rendering/render_root.h"
#include "html/html_baseimpl.h"
#include "html/html_objectimpl.h"
#include "html/htmltokenizer.h"
#include "misc/htmlattrs.h"
#include "xml/dom2_eventsimpl.h"
#include "xml/dom_docimpl.h"
#include "css/csshelper.h"
#include "misc/htmltags.h"
#include "khtmlview.h"
#include "khtml_part.h"

#include <kapplication.h>
#include <kmessagebox.h>
#include <kmimetype.h>
#include <klocale.h>
#include <kdebug.h>
#include <qtimer.h>
#include <qpainter.h>
#include <qcursor.h>

#include <assert.h>

using namespace khtml;
using namespace DOM;

RenderFrameSet::RenderFrameSet( HTMLFrameSetElementImpl *frameSet)
    : RenderBox(frameSet)
{
  // init RenderObject attributes
    setInline(false);

  for (int k = 0; k < 2; ++k) {
      m_gridLen[k] = -1;
      m_gridDelta[k] = 0;
      m_gridLayout[k] = 0;
  }

  m_resizing = m_clientresizing= false;

  m_cursor = Qt::ArrowCursor;

  m_hSplit = -1;
  m_vSplit = -1;

  m_hSplitVar = 0;
  m_vSplitVar = 0;
}

RenderFrameSet::~RenderFrameSet()
{
    for (int k = 0; k < 2; ++k) {
        if (m_gridLayout[k]) delete [] m_gridLayout[k];
        if (m_gridDelta[k]) delete [] m_gridDelta[k];
    }
  if (m_hSplitVar)
      delete [] m_hSplitVar;
  if (m_vSplitVar)
      delete [] m_vSplitVar;
}

bool RenderFrameSet::nodeAtPoint(NodeInfo& info, int _x, int _y, int _tx, int _ty)
{
    RenderBox::nodeAtPoint(info, _x, _y, _tx, _ty);

    bool inside = m_resizing || canResize(_x, _y);

    if ( inside && element() && !element()->noResize() && !info.readonly())
        info.setInnerNode(element());

    return inside || m_clientresizing;
}

void RenderFrameSet::layout( )
{
    KHTMLAssert( !layouted() );
    KHTMLAssert( minMaxKnown() );

    if ( !parent()->isFrameSet() ) {
        KHTMLView* view = root()->view();
        m_width = view->visibleWidth();
        m_height = view->visibleHeight();
    }

#ifdef DEBUG_LAYOUT
    kdDebug( 6040 ) << renderName() << "(FrameSet)::layout( ) width=" << width() << ", height=" << height() << endl;
#endif

    int remainingLen[2];
    remainingLen[1] = m_width - (element()->totalCols()-1)*element()->border();
    if(remainingLen[1]<0) remainingLen[1]=0;
    remainingLen[0] = m_height - (element()->totalRows()-1)*element()->border();
    if(remainingLen[0]<0) remainingLen[0]=0;

    int availableLen[2];
    availableLen[0] = remainingLen[0];
    availableLen[1] = remainingLen[1];

    if (m_gridLen[0] != element()->totalRows() || m_gridLen[1] != element()->totalCols()) {
        // number of rows or cols changed
        // need to zero out the deltas
        m_gridLen[0] = element()->totalRows();
        m_gridLen[1] = element()->totalCols();
        for (int k = 0; k < 2; ++k) {
            if (m_gridDelta[k]) delete [] m_gridDelta[k];
            m_gridDelta[k] = new int[m_gridLen[k]];
            if (m_gridLayout[k]) delete [] m_gridLayout[k];
            m_gridLayout[k] = new int[m_gridLen[k]];
            for (int i = 0; i < m_gridLen[k]; ++i)
                m_gridDelta[k][i] = 0;
        }
    }

    for (int k = 0; k < 2; ++k) {
        int totalRelative = 0;
        int totalFixed = 0;
        int totalPercent = 0;
        int countRelative = 0;
        int countPercent = 0;
        int gridLen = m_gridLen[k];
        int* gridDelta = m_gridDelta[k];
        khtml::Length* grid =  k ? element()->m_cols : element()->m_rows;
        int* gridLayout = m_gridLayout[k];

        if (grid) {
            // first distribute the available width for fixed rows, then handle the
            // percentage ones and distribute remaining over relative
            for(int i = 0; i< gridLen; ++i)
                if (grid[i].isFixed()) {
                    gridLayout[i] = kMin(grid[i].value() > 0 ? grid[i].value() : 0, remainingLen[k]);
                    remainingLen[k] -= gridLayout[i];
                    totalFixed += gridLayout[i];
                }
                else if(grid[i].isRelative()) {
                    totalRelative += grid[i].value() > 1 ? grid[i].value() : 1;
                    countRelative++;
                }

            for(int i = 0; i < gridLen; i++)
                if(grid[i].isPercent()) {
                    gridLayout[i] = kMin(kMax(grid[i].width(availableLen[k]), 0), remainingLen[k]);
                    remainingLen[k] -= gridLayout[i];
                    totalPercent += grid[i].value();
                    countPercent++;
                }

            assert(remainingLen[k] >= 0);

            if (countRelative) {
                int remaining = remainingLen[k];
                for (int i = 0; i < gridLen; ++i)
                    if (grid[i].isRelative()) {
                        gridLayout[i] = ((grid[i].value() > 1 ? grid[i].value() : 1) * remaining) / totalRelative;
                        remainingLen[k] -= gridLayout[i];
                    }
            }

            // distribute the rest
            if (remainingLen[k]) {
                LengthType distributeType = countPercent ? Percent : Fixed;
                int total = countPercent ? totalPercent : totalFixed;
                if (!total) total = 1;
                for (int i = 0; i < gridLen; ++i)
                    if (grid[i].type() == distributeType) {
                        int toAdd = (remainingLen[k] * grid[i].value()) / total;
                        gridLayout[i] += toAdd;
                    }
            }

            // now we have the final layout, distribute the delta over it
            bool worked = true;
            for (int i = 0; i < gridLen; ++i) {
                if (gridLayout[i] && gridLayout[i] + gridDelta[i] <= 0)
                    worked = false;
                gridLayout[i] += gridDelta[i];
            }
            // now the delta's broke something, undo it and reset deltas
            if (!worked)
                for (int i = 0; i < gridLen; ++i) {
                    gridLayout[i] -= gridDelta[i];
                    gridDelta[i] = 0;
                }
        }
        else
            gridLayout[0] = remainingLen[k];
    }

    positionFrames();

    RenderObject *child = firstChild();
    if ( !child )
      return;

    if(!m_hSplitVar && !m_vSplitVar)
    {
#ifdef DEBUG_LAYOUT
        kdDebug( 6031 ) << "calculationg fixed Splitters" << endl;
#endif
        if(!m_vSplitVar && element()->totalCols() > 1)
        {
            m_vSplitVar = new bool[element()->totalCols()];
            for(int i = 0; i < element()->totalCols(); i++) m_vSplitVar[i] = true;
        }
        if(!m_hSplitVar && element()->totalRows() > 1)
        {
            m_hSplitVar = new bool[element()->totalRows()];
            for(int i = 0; i < element()->totalRows(); i++) m_hSplitVar[i] = true;
        }

        for(int r = 0; r < element()->totalRows(); r++)
        {
            for(int c = 0; c < element()->totalCols(); c++)
            {
                bool fixed = false;

                if ( child->isFrameSet() )
                  fixed = static_cast<RenderFrameSet *>(child)->element()->noResize();
                else
                  fixed = static_cast<RenderFrame *>(child)->element()->noResize();

                if(fixed)
                {
#ifdef DEBUG_LAYOUT
                    kdDebug( 6031 ) << "found fixed cell " << r << "/" << c << "!" << endl;
#endif
                    if( element()->totalCols() > 1)
                    {
                        if(c>0) m_vSplitVar[c-1] = false;
                        m_vSplitVar[c] = false;
                    }
                    if( element()->totalRows() > 1)
                    {
                        if(r>0) m_hSplitVar[r-1] = false;
                        m_hSplitVar[r] = false;
                    }
                    child = child->nextSibling();
                    if(!child) goto end2;
                }
#ifdef DEBUG_LAYOUT
                else
                    kdDebug( 6031 ) << "not fixed: " << r << "/" << c << "!" << endl;
#endif
            }
        }

    }
    RenderContainer::layout();
 end2:
    setLayouted();
}

void RenderFrameSet::positionFrames()
{
  int r;
  int c;

  RenderObject *child = firstChild();
  if ( !child )
    return;

  //  NodeImpl *child = _first;
  //  if(!child) return;

  int yPos = 0;

  for(r = 0; r < element()->totalRows(); r++)
  {
    int xPos = 0;
    for(c = 0; c < element()->totalCols(); c++)
    {
      child->setPos( xPos, yPos );
#ifdef DEBUG_LAYOUT
      kdDebug(6040) << "child frame at (" << xPos << "/" << yPos << ") size (" << m_gridLayout[1][c] << "/" << m_gridLayout[0][r] << ")" << endl;
#endif
      // has to be resized and itself resize its contents
      if ((m_gridLayout[1][c] != child->width()) || (m_gridLayout[0][r] != child->height())) {
          child->setWidth( m_gridLayout[1][c] );
          child->setHeight( m_gridLayout[0][r] );
          child->setLayouted(false);
	  child->layout();
      }

      xPos += m_gridLayout[1][c] + element()->border();
      child = child->nextSibling();

      if ( !child )
        return;

    }

    yPos += m_gridLayout[0][r] + element()->border();
  }

  // all the remaining frames are hidden to avoid ugly
  // spurious nonlayouted frames
  while ( child ) {
      child->setWidth( 0 );
      child->setHeight( 0 );
      child->setLayouted();

      child = child->nextSibling();
  }
}

bool RenderFrameSet::userResize( MouseEventImpl *evt )
{
    if (!layouted()) return false;

  bool res = false;
  int _x = evt->clientX();
  int _y = evt->clientY();

  if ( !m_resizing && evt->id() == EventImpl::MOUSEMOVE_EVENT || evt->id() == EventImpl::MOUSEDOWN_EVENT )
  {
#ifdef DEBUG_LAYOUT
    kdDebug( 6031 ) << "mouseEvent:check" << endl;
#endif

    m_hSplit = -1;
    m_vSplit = -1;
    //bool resizePossible = true;

    // check if we're over a horizontal or vertical boundary
    int pos = m_gridLayout[1][0] + xPos();
    for(int c = 1; c < element()->totalCols(); c++)
    {
      if(_x >= pos && _x <= pos+element()->border())
      {
        if(m_vSplitVar && m_vSplitVar[c-1] == true) m_vSplit = c-1;
#ifdef DEBUG_LAYOUT
        kdDebug( 6031 ) << "vsplit!" << endl;
#endif
        res = true;
        break;
      }
      pos += m_gridLayout[1][c] + element()->border();
    }

    pos = m_gridLayout[0][0] + yPos();
    for(int r = 1; r < element()->totalRows(); r++)
    {
      if( _y >= pos && _y <= pos+element()->border())
      {
        if(m_hSplitVar && m_hSplitVar[r-1] == true) m_hSplit = r-1;
#ifdef DEBUG_LAYOUT
        kdDebug( 6031 ) << "hsplitvar = " << m_hSplitVar << endl;
        kdDebug( 6031 ) << "hsplit!" << endl;
#endif
        res = true;
        break;
      }
      pos += m_gridLayout[0][r] + element()->border();
    }
#ifdef DEBUG_LAYOUT
    kdDebug( 6031 ) << m_hSplit << "/" << m_vSplit << endl;
#endif
  }


  m_cursor = Qt::ArrowCursor;
  if(m_hSplit != -1 && m_vSplit != -1)
      m_cursor = Qt::SizeAllCursor;
  else if( m_vSplit != -1 )
      m_cursor = Qt::SizeHorCursor;
  else if( m_hSplit != -1 )
      m_cursor = Qt::SizeVerCursor;

  if(!m_resizing && evt->id() == EventImpl::MOUSEDOWN_EVENT)
  {
      setResizing(true);
      KApplication::setOverrideCursor(QCursor(m_cursor));
      m_vSplitPos = _x;
      m_hSplitPos = _y;
      m_oldpos = -1;
  }

  // ### check the resize is not going out of bounds.
  if(m_resizing && evt->id() == EventImpl::MOUSEUP_EVENT)
  {
    setResizing(false);
    KApplication::restoreOverrideCursor();

    if(m_vSplit != -1 )
    {
#ifdef DEBUG_LAYOUT
      kdDebug( 6031 ) << "split xpos=" << _x << endl;
#endif
      int delta = m_vSplitPos - _x;
      m_gridDelta[1][m_vSplit] -= delta;
      m_gridDelta[1][m_vSplit+1] += delta;
    }
    if(m_hSplit != -1 )
    {
#ifdef DEBUG_LAYOUT
      kdDebug( 6031 ) << "split ypos=" << _y << endl;
#endif
      int delta = m_hSplitPos - _y;
      m_gridDelta[0][m_hSplit] -= delta;
      m_gridDelta[0][m_hSplit+1] += delta;
    }

    // this just schedules the relayout
    // important, otherwise the moving indicator is not correctly erased
    setLayouted(false);
  }

  if (m_resizing || evt->id() == EventImpl::MOUSEUP_EVENT) {
      QPainter paint( root()->view() );
      paint.setPen( Qt::gray );
      paint.setBrush( Qt::gray );
      paint.setRasterOp( Qt::XorROP );
      QRect r(xPos(), yPos(), width(), height());
      const int rBord = 3;
      int sw = element()->border();
      int p = m_resizing ? (m_vSplit > -1 ? _x : _y) : -1;
      if (m_vSplit > -1) {
          if ( m_oldpos >= 0 )
              paint.drawRect( m_oldpos + sw/2 - rBord , r.y(),
                              2*rBord, r.height() );
          if ( p >= 0 )
              paint.drawRect( p  + sw/2 - rBord, r.y(), 2*rBord, r.height() );
      } else {
          if ( m_oldpos >= 0 )
              paint.drawRect( r.x(), m_oldpos + sw/2 - rBord,
                              r.width(), 2*rBord );
          if ( p >= 0 )
              paint.drawRect( r.x(), p + sw/2 - rBord, r.width(), 2*rBord );
      }
      m_oldpos = p;
  }

  return res;
}

void RenderFrameSet::setResizing(bool e)
{
      m_resizing = e;
      for (RenderObject* p = parent(); p; p = p->parent())
          if (p->isFrameSet()) static_cast<RenderFrameSet*>(p)->m_clientresizing = m_resizing;
}

bool RenderFrameSet::canResize( int _x, int _y )
{
    // if we're not layouted, the gridLayout doesn't contain useful data
    if (!layouted() || !m_gridLayout[0] || !m_gridLayout[1] ) return false;

    // check if we're over a horizontal or vertical boundary
    int pos = m_gridLayout[1][0];
    for(int c = 1; c < element()->totalCols(); c++)
        if(_x >= pos && _x <= pos+element()->border())
            return true;

    pos = m_gridLayout[0][0];
    for(int r = 1; r < element()->totalRows(); r++)
        if( _y >= pos && _y <= pos+element()->border())
            return true;

    return false;
}

#ifndef NDEBUG
void RenderFrameSet::dump(QTextStream *stream, QString ind) const
{
  *stream << " totalrows=" << element()->totalRows();
  *stream << " totalcols=" << element()->totalCols();

  uint i;
  for (i = 0; i < (uint)element()->totalRows(); i++)
    *stream << " hSplitvar(" << i << ")=" << m_hSplitVar[i];

  for (i = 0; i < (uint)element()->totalCols(); i++)
    *stream << " vSplitvar(" << i << ")=" << m_vSplitVar[i];

  RenderBox::dump(stream,ind);
}
#endif

/**************************************************************************************/

RenderPart::RenderPart(DOM::HTMLElementImpl* node)
    : RenderWidget(node)
{
    // init RenderObject attributes
    setInline(false);
}

void RenderPart::setWidget( QWidget *widget )
{
#ifdef DEBUG_LAYOUT
    kdDebug(6031) << "RenderPart::setWidget()" << endl;
#endif
    setQWidget( widget );
    widget->setFocusPolicy(QWidget::WheelFocus);
    if(widget->inherits("KHTMLView"))
        connect( widget, SIGNAL( cleared() ), this, SLOT( slotViewCleared() ) );

    setLayouted( false );
    setMinMaxKnown( false );

    // make sure the scrollbars are set correctly for restore
    // ### find better fix
    slotViewCleared();
}

bool RenderPart::partLoadingErrorNotify(khtml::ChildFrame *, const KURL& , const QString& )
{
    return false;
}

short RenderPart::intrinsicWidth() const
{
    return 300;
}

int RenderPart::intrinsicHeight() const
{
    return 200;
}

void RenderPart::slotViewCleared()
{
}

/***************************************************************************************/

RenderFrame::RenderFrame( DOM::HTMLFrameElementImpl *frame )
    : RenderPart(frame)
{
    setInline( false );
}

void RenderFrame::slotViewCleared()
{
    if(m_widget->inherits("QScrollView")) {
#ifdef DEBUG_LAYOUT
        kdDebug(6031) << "frame is a scrollview!" << endl;
#endif
        QScrollView *view = static_cast<QScrollView *>(m_widget);
        if(!element()->frameBorder || !((static_cast<HTMLFrameSetElementImpl *>(element()->parentNode()))->frameBorder()))
            view->setFrameStyle(QFrame::NoFrame);
	    view->setVScrollBarMode(element()->scrolling );
	    view->setHScrollBarMode(element()->scrolling );
        if(view->inherits("KHTMLView")) {
#ifdef DEBUG_LAYOUT
            kdDebug(6031) << "frame is a KHTMLview!" << endl;
#endif
            KHTMLView *htmlView = static_cast<KHTMLView *>(view);
            if(element()->marginWidth != -1) htmlView->setMarginWidth(element()->marginWidth);
            if(element()->marginHeight != -1) htmlView->setMarginHeight(element()->marginHeight);
        }
    }
}

/****************************************************************************************/

RenderPartObject::RenderPartObject( DOM::HTMLElementImpl* element )
    : RenderPart( element )
{
    // init RenderObject attributes
    setInline(true);
}

void RenderPartObject::updateWidget()
{
  QString url;
  QString serviceType;
  QStringList params;
  KHTMLPart *part = m_view->part();

  setMinMaxKnown(false);
  setLayouted(false);

  // ### this should be constant true - move iframe to somewhere else
  if (element()->id() == ID_OBJECT || element()->id() == ID_EMBED) {

      for (NodeImpl* child = element()->firstChild(); child; child=child->nextSibling()) {
          if ( child->id() == ID_PARAM ) {
              HTMLParamElementImpl *p = static_cast<HTMLParamElementImpl *>( child );

              QString aStr = p->name();
              aStr += QString::fromLatin1("=\"");
              aStr += p->value();
              aStr += QString::fromLatin1("\"");
              params.append(aStr);
          }
      }
      params.append( QString::fromLatin1("__KHTML__PLUGINEMBED=\"YES\"") );
      params.append( QString::fromLatin1("__KHTML__PLUGINBASEURL=\"%1\"").arg( part->url().url() ) );
  }

  if(element()->id() == ID_OBJECT) {

      // check for embed child object
      HTMLObjectElementImpl *o = static_cast<HTMLObjectElementImpl *>(element());
      HTMLEmbedElementImpl *embed = 0;
      for (NodeImpl *child = o->firstChild(); child; child = child->nextSibling())
          if ( child->id() == ID_EMBED ) {
              embed = static_cast<HTMLEmbedElementImpl *>( child );
              break;
          }

      params.append( QString::fromLatin1("__KHTML__CLASSID=\"%1\"").arg( o->classId ) );
      params.append( QString::fromLatin1("__KHTML__CODEBASE=\"%1\"").arg( o->getAttribute(ATTR_CODEBASE).string() ) );
      if (!o->getAttribute(ATTR_WIDTH).isEmpty())
          params.append( QString::fromLatin1("WIDTH=\"%1\"").arg( o->getAttribute(ATTR_WIDTH).string() ) );
      if (!o->getAttribute(ATTR_HEIGHT).isEmpty())
          params.append( QString::fromLatin1("HEIGHT=\"%1\"").arg( o->getAttribute(ATTR_HEIGHT).string() ) );

      if ( !embed )
      {
          url = o->url;
          serviceType = o->serviceType;
          if(serviceType.isEmpty() || serviceType.isNull()) {
              if(!o->classId.isEmpty()) {
                  // We have a clsid, means this is activex (Niko)
                  serviceType = "application/x-activex-handler";
                  url = "dummy"; // Not needed, but KHTMLPart aborts the request if empty
              }

              if(o->classId.contains(QString::fromLatin1("D27CDB6E-AE6D-11cf-96B8-444553540000"))) {
                  // It is ActiveX, but the nsplugin system handling
                  // should also work, that's why we don't override the
                  // serviceType with application/x-activex-handler
                  // but let the KTrader in khtmlpart::createPart() detect
                  // the user's preference: launch with activex viewer or
                  // with nspluginviewer (Niko)
                  serviceType = "application/x-shockwave-flash";
              }
              else if(o->classId.contains(QString::fromLatin1("CFCDAA03-8BE4-11cf-B84B-0020AFBBCCFA")))
                  serviceType = "audio/x-pn-realaudio-plugin";

              // TODO: add more plugins here
          }

          if(url.isEmpty()) {
              // look for a SRC attribute in the params
              NodeImpl *child = o->firstChild();
              while ( child ) {
                  if ( child->id() == ID_PARAM ) {
                      HTMLParamElementImpl *p = static_cast<HTMLParamElementImpl *>( child );

                      if ( ( p->name().lower()==QString::fromLatin1("src") ||
                             p->name().lower()==QString::fromLatin1("movie") ||
                             p->name().lower()==QString::fromLatin1("code") ) && !p->value().isNull() )
                      {
                          url = p->getDocument()->completeURL(khtml::parseURL(p->value()).string());
                          if (!p->getDocument()->isURLAllowed(url))
                              url = QString::null;
                          else
                              break;
                      }
                  }
                  child = child->nextSibling();
              }
          }


          if ( url.isEmpty() && serviceType.isEmpty() ) {
#ifdef DEBUG_LAYOUT
              kdDebug() << "RenderPartObject::close - empty url and serverType" << endl;
#endif
              return;
          }
          part->requestObject( this, url, serviceType, params );
      }
      else {
          // render embed object
          url = embed->url;
          serviceType = embed->serviceType;

          if ( url.isEmpty() && serviceType.isEmpty() ) {
#ifdef DEBUG_LAYOUT
              kdDebug() << "RenderPartObject::close - empty url and serverType" << endl;
#endif
              return;
          }
          part->requestObject( this, url, serviceType, params );
      }
      o->setLiveConnect(part->liveConnectExtension(this));
  }
  else if ( element()->id() == ID_EMBED ) {

      HTMLEmbedElementImpl *o = static_cast<HTMLEmbedElementImpl *>(element());
      url = o->url;
      serviceType = o->serviceType;

      if ( url.isEmpty() && serviceType.isEmpty() ) {
#ifdef DEBUG_LAYOUT
          kdDebug() << "RenderPartObject::close - empty url and serverType" << endl;
#endif
          return;
      }
      // add all attributes set on the embed object
      NamedAttrMapImpl* a = o->attributes();
      if (a) {
          for (unsigned long i = 0; i < a->length(); ++i) {
              AttributeImpl* it = a->attributeItem(i);
              params.append(o->getDocument()->attrName(it->id()).string() + "=\"" + it->value().string() + "\"");
          }
      }
      part->requestObject( this, url, serviceType, params );
  } else {
      assert(element()->id() == ID_IFRAME);
      HTMLIFrameElementImpl *o = static_cast<HTMLIFrameElementImpl *>(element());
      url = o->url.string();
      if( url.isEmpty()) return;
      KHTMLView *v = static_cast<KHTMLView *>(m_view);
      v->part()->requestFrame( this, url, o->name.string(), QStringList(), true );
  }
}

// ugly..
void RenderPartObject::close()
{
    if ( element()->id() == ID_OBJECT )
        updateWidget();
    RenderPart::close();
}


bool RenderPartObject::partLoadingErrorNotify( khtml::ChildFrame *childFrame, const KURL& url, const QString& serviceType )
{
    KHTMLPart *part = static_cast<KHTMLView *>(m_view)->part();
    //kdDebug() << "RenderPartObject::partLoadingErrorNotify serviceType=" << serviceType << endl;
    // Check if we just tried with e.g. nsplugin
    // and fallback to the activexhandler if there is a classid
    // and a codebase, where we may download the ocx if it's missing
    if( serviceType != "application/x-activex-handler" && element()->id()==ID_OBJECT ) {

        // check for embed child object
        HTMLObjectElementImpl *o = static_cast<HTMLObjectElementImpl *>(element());
        HTMLEmbedElementImpl *embed = 0;
        NodeImpl *child = o->firstChild();
        while ( child ) {
            if ( child->id() == ID_EMBED )
                embed = static_cast<HTMLEmbedElementImpl *>( child );

            child = child->nextSibling();
        }
        if( embed && !o->classId.isEmpty() &&
            !( static_cast<ElementImpl *>(o)->getAttribute(ATTR_CODEBASE).string() ).isEmpty() )
        {
            KParts::URLArgs args;
            args.serviceType = "application/x-activex-handler";
            if (part->requestObject( childFrame, url, args ))
                return true; // success
        }
    }
    // Dissociate ourselves from the current event loop (to prevent crashes
    // due to the message box staying up)
    QTimer::singleShot( 0, this, SLOT( slotPartLoadingErrorNotify() ) );
#if 0
    Tokenizer *tokenizer = static_cast<DOM::DocumentImpl *>(part->document().handle())->tokenizer();
    if (tokenizer) tokenizer->setOnHold( true );
    slotPartLoadingErrorNotify();
    if (tokenizer) tokenizer->setOnHold( false );
#endif
    return false;
}

void RenderPartObject::slotPartLoadingErrorNotify()
{
    // First we need to find out the servicetype - again - this code is too duplicated !
    HTMLEmbedElementImpl *embed = 0;
    QString serviceType;
    if( element()->id()==ID_OBJECT ) {

        // check for embed child object
        HTMLObjectElementImpl *o = static_cast<HTMLObjectElementImpl *>(element());
        serviceType = o->serviceType;
        NodeImpl *child = o->firstChild();
        while ( child ) {
            if ( child->id() == ID_EMBED )
                embed = static_cast<HTMLEmbedElementImpl *>( child );

            child = child->nextSibling();
        }

    } else if( element()->id()==ID_EMBED ) {
        embed = static_cast<HTMLEmbedElementImpl *>(element());
    }
    if ( embed )
	serviceType = embed->serviceType;

    KHTMLPart *part = static_cast<KHTMLView *>(m_view)->part();
    KParts::BrowserExtension *ext = part->browserExtension();
    if( embed && !embed->pluginPage.isEmpty() && ext ) {
        // Prepare the mimetype to show in the question (comment if available, name as fallback)
        QString mimeName = serviceType;
        KMimeType::Ptr mime = KMimeType::mimeType(serviceType);
        if ( mime->name() != KMimeType::defaultMimeType() )
            mimeName = mime->comment();

        // Check if we already asked the user, for this page
        if (!mimeName.isEmpty() && part->docImpl() && !part->pluginPageQuestionAsked( serviceType ) )
        {
            part->setPluginPageQuestionAsked( serviceType );
            // Prepare the URL to show in the question (host only if http, to make it short)
            KURL pluginPageURL( embed->pluginPage );
            QString shortURL = pluginPageURL.protocol() == "http" ? pluginPageURL.host() : pluginPageURL.prettyURL();
            int res = KMessageBox::questionYesNo( m_view,
                                                  i18n("No plugin found for '%1'.\nDo you want to download one from %2?").arg(mimeName).arg(shortURL),
                                                  i18n("Missing Plugin"), QString::null, QString::null, QString("plugin-")+serviceType);
            if ( res == KMessageBox::Yes )
            {
                // Display vendor download page
                ext->createNewWindow( pluginPageURL );
                return;
            }
        }
    }

    // didn't work, render alternative content.
 //   if ( element() && element()->id() == ID_OBJECT )
   //     static_cast<HTMLObjectElementImpl*>( element() )->renderAlternative();
}

void RenderPartObject::layout( )
{
    KHTMLAssert( !layouted() );
    KHTMLAssert( minMaxKnown() );

    calcWidth();
    calcHeight();

    RenderPart::layout();

    setLayouted();
}

void RenderPartObject::slotViewCleared()
{
  if(m_widget->inherits("QScrollView") ) {
#ifdef DEBUG_LAYOUT
      kdDebug(6031) << "iframe is a scrollview!" << endl;
#endif
      QScrollView *view = static_cast<QScrollView *>(m_widget);
      int frameStyle = QFrame::NoFrame;
      QScrollView::ScrollBarMode scroll = QScrollView::Auto;
      int marginw = 0;
      int marginh = 0;
      if ( element()->id() == ID_IFRAME) {
	  HTMLIFrameElementImpl *frame = static_cast<HTMLIFrameElementImpl *>(element());
	  if(frame->frameBorder)
	      frameStyle = QFrame::Box;
	  scroll = frame->scrolling;
	  marginw = frame->marginWidth;
	  marginh = frame->marginHeight;
      }
      view->setFrameStyle(frameStyle);
      view->setVScrollBarMode(scroll );
      view->setHScrollBarMode(scroll );
      if(view->inherits("KHTMLView")) {
#ifdef DEBUG_LAYOUT
          kdDebug(6031) << "frame is a KHTMLview!" << endl;
#endif
          KHTMLView *htmlView = static_cast<KHTMLView *>(view);
          htmlView->setIgnoreWheelEvents( element()->id() == ID_IFRAME );
          if(marginw != -1) htmlView->setMarginWidth(marginw);
          if(marginh != -1) htmlView->setMarginHeight(marginh);
        }
  }
}

#include "render_frames.moc"