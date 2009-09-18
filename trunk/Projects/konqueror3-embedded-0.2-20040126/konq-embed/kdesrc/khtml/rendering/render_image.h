/*
 * This file is part of the DOM implementation for KDE.
 *
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
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
 * $Id: render_image.h,v 1.36.2.4 2004/01/10 18:54:02 mueller Exp $
 */
#ifndef RENDER_IMAGE_H
#define RENDER_IMAGE_H

#include "html/dtd.h"
#include "html/html_elementimpl.h"
#include "rendering/render_replaced.h"
#include "dom/dom_string.h"

#include <qmap.h>
#include <qpixmap.h>

namespace khtml {

class DocLoader;

class RenderImage : public RenderReplaced
{
public:
    RenderImage(DOM::HTMLElementImpl *_element);
    virtual ~RenderImage();

    virtual const char *renderName() const { return "RenderImage"; }

    virtual bool isRendered() const { return true; }

    virtual void paintObject( QPainter *p, int /*x*/, int /*y*/, int /*w*/, int /*h*/, int tx, int ty);

    virtual void layout();

    virtual void setPixmap( const QPixmap &, const QRect&, CachedImage *);

    // don't even think about making these methods virtual!
    QPixmap pixmap() const;
    DOM::HTMLElementImpl* element() const
    { return static_cast<DOM::HTMLElementImpl*>(RenderObject::element()); }

    bool complete() const;

    // hook to keep RendeObject::m_inline() up to date
    virtual void setStyle(RenderStyle *style);
    virtual void updateFromElement();

    virtual void notifyFinished(CachedObject *finishedObj);
    virtual bool nodeAtPoint(NodeInfo& info, int x, int y, int tx, int ty);

    virtual short calcReplacedWidth() const;
    virtual int   calcReplacedHeight() const;

private:
    /*
     * Cache for images that need resizing
     */
    QPixmap resizeCache;

    // text to display as long as the image isn't available
    DOM::DOMString alt;

    CachedImage *image;
    bool berrorPic : 1;
    bool loadEventSent : 1;
};


} //namespace

#endif
