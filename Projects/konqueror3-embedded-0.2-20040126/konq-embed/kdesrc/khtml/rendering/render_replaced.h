/*
 * This file is part of the HTML widget for KDE.
 *
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
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
 * $Id: render_replaced.h,v 1.39.2.5 2003/10/25 13:36:02 ggarand Exp $
 */
#ifndef render_replaced_h
#define render_replaced_h

#include "render_flow.h"
#include <qobject.h>
class KHTMLView;
class QWidget;

namespace khtml {

class RenderReplaced : public RenderBox
{
public:
    RenderReplaced(DOM::NodeImpl* node);

    virtual const char *renderName() const { return "RenderReplaced"; }

    virtual bool isRendered() const { return true; }

    virtual short lineHeight( bool firstLine) const;
    virtual short baselinePosition( bool firstLine ) const;

    virtual void calcMinMaxWidth();

    virtual void paint( QPainter *, int x, int y, int w, int h,
                        int tx, int ty);
    virtual void paintObject(QPainter *p, int x, int y, int w, int h, int tx, int ty) = 0;

    virtual short intrinsicWidth() const { return m_intrinsicWidth; }
    virtual int intrinsicHeight() const { return m_intrinsicHeight; }

    void setIntrinsicWidth(int w) {  m_intrinsicWidth = w; }
    void setIntrinsicHeight(int h) { m_intrinsicHeight = h; }

    virtual void position(int x, int y, int from, int len, int width, bool reverse, bool firstLine, int);

protected:
    short m_intrinsicWidth;
    short m_intrinsicHeight;
};


class RenderWidget : public QObject, public RenderReplaced, public khtml::Shared<RenderWidget>
{
    Q_OBJECT
public:
    RenderWidget(DOM::NodeImpl* node);
    virtual ~RenderWidget();

    virtual void setStyle(RenderStyle *style);

    virtual void paintObject(QPainter *p, int x, int y, int w, int h, int tx, int ty);

    virtual bool isWidget() const { return true; };

    virtual void detach();
    virtual void layout( );

    virtual void updateFromElement();

    QWidget *widget() const { return m_widget; }
    KHTMLView* view() const { return m_view; }


public slots:
    void slotWidgetDestructed();

protected:
    virtual void handleFocusOut() {}
    bool event( QEvent *e );

    bool eventFilter(QObject* /*o*/, QEvent* e);
    void setQWidget(QWidget *widget);
    void resizeWidget( int w, int h );

    QWidget *m_widget;
    KHTMLView* m_view;
};


class RenderReplacedFlow : public RenderFlow
{
public:
    RenderReplacedFlow(DOM::NodeImpl* node);

    virtual const char *renderName() const { return "RenderReplacedFlow"; }

protected:
    virtual void calcMinMaxWidth();
    virtual short intrinsicWidth() const { return m_intrinsicWidth; }
    short calcObjectWidth( RenderObject *o, short width );
    void setIntrinsicWidth(int w) {  m_intrinsicWidth = w; }
    short m_intrinsicWidth;
};


}

#endif