/**
 * This file is part of the DOM implementation for KDE.
 *
 * Copyright (C) 1997 Martin Jones (mjones@kde.org)
 *           (C) 1997 Torben Weis (weis@kde.org)
 *           (C) 1998 Waldo Bastian (bastian@kde.org)
 *           (C) 1999 Lars Knoll (knoll@kde.org)
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
 */

#include "html/html_documentimpl.h"
#include "html/html_tableimpl.h"

#include "dom/dom_exception.h"
#include "dom/dom_node.h"

#include "misc/htmlhashes.h"
#include "khtmlview.h"
#include "khtml_part.h"

#include "css/cssstyleselector.h"
#include "css/cssproperties.h"
#include "css/cssvalues.h"
#include "css/csshelper.h"

#include "rendering/render_table.h"

#include <kdebug.h>
#include <kglobal.h>

using namespace khtml;
using namespace DOM;

HTMLTableElementImpl::HTMLTableElementImpl(DocumentPtr *doc)
  : HTMLElementImpl(doc)
{
    tCaption = 0;
    head = 0;
    foot = 0;
    firstBody = 0;

#if 0
    rules = None;
    frame = Void;
#endif
    padding = 1;

    m_noBorder = true;
    m_solid = false;

    // reset font color and sizes here, if we don't have strict parse mode.
    // this is 90% compatible to ie and mozilla, and the by way easiest solution...
    // only difference to 100% correct is that in strict mode <font> elements are propagated into tables.
    if ( getDocument()->parseMode() != DocumentImpl::Strict ) {
        addCSSProperty( CSS_PROP_FONT_SIZE, CSS_VAL_MEDIUM );
        addCSSProperty( CSS_PROP_COLOR, "-konq-text" );
        addCSSProperty( CSS_PROP_FONT_FAMILY, "konq_default" );
    }
}

HTMLTableElementImpl::~HTMLTableElementImpl()
{
}

NodeImpl::Id HTMLTableElementImpl::id() const
{
    return ID_TABLE;
}

NodeImpl* HTMLTableElementImpl::setCaption( HTMLTableCaptionElementImpl *c )
{
    int exceptioncode = 0;
    NodeImpl* r;
    if(tCaption) {
        replaceChild ( c, tCaption, exceptioncode );
        r = c;
    }
    else
        r = insertBefore( c, firstChild(), exceptioncode );
    tCaption = c;
    return r;
}

NodeImpl* HTMLTableElementImpl::setTHead( HTMLTableSectionElementImpl *s )
{
    int exceptioncode = 0;
    NodeImpl* r;
    if(head) {
        replaceChild( s, head, exceptioncode );
        r = s;
    }
    else if( foot )
        r = insertBefore( s, foot, exceptioncode );
    else if( firstBody )
        r = insertBefore( s, firstBody, exceptioncode );
    else
        r = appendChild( s, exceptioncode );

    head = s;
    return r;
}

NodeImpl* HTMLTableElementImpl::setTFoot( HTMLTableSectionElementImpl *s )
{
    int exceptioncode = 0;
    NodeImpl* r;
    if(foot) {
        replaceChild ( s, foot, exceptioncode );
        r = s;
    }
    else if( firstBody )
        r = insertBefore( s, firstBody, exceptioncode );
    else
        r = appendChild( s, exceptioncode );
    foot = s;
    return r;
}

NodeImpl* HTMLTableElementImpl::setTBody( HTMLTableSectionElementImpl *s )
{
    int exceptioncode = 0;
    NodeImpl* r;

    if(!firstBody)
        firstBody = s;

    if( foot )
        r = insertBefore( s, foot, exceptioncode );
    else
        r = appendChild( s, exceptioncode );

    return r;
}

HTMLElementImpl *HTMLTableElementImpl::createTHead(  )
{
    if(!head)
    {
        int exceptioncode = 0;
        head = new HTMLTableSectionElementImpl(docPtr(), ID_THEAD, true /* implicit */);
        if(foot)
            insertBefore( head, foot, exceptioncode );
        if(firstBody)
            insertBefore( head, firstBody, exceptioncode);
        else
            appendChild(head, exceptioncode);
    }
    return head;
}

void HTMLTableElementImpl::deleteTHead(  )
{
    if(head) {
        int exceptioncode = 0;
        HTMLElementImpl::removeChild(head, exceptioncode);
    }
    head = 0;
}

HTMLElementImpl *HTMLTableElementImpl::createTFoot(  )
{
    if(!foot)
    {
        int exceptioncode = 0;
        foot = new HTMLTableSectionElementImpl(docPtr(), ID_TFOOT, true /*implicit */);
        if(firstBody)
            insertBefore( foot, firstBody, exceptioncode );
        else
            appendChild(foot, exceptioncode);
    }
    return foot;
}

void HTMLTableElementImpl::deleteTFoot(  )
{
    if(foot) {
        int exceptioncode = 0;
        HTMLElementImpl::removeChild(foot, exceptioncode);
    }
    foot = 0;
}

HTMLElementImpl *HTMLTableElementImpl::createCaption(  )
{
    if(!tCaption)
    {
        int exceptioncode = 0;
        tCaption = new HTMLTableCaptionElementImpl(docPtr());
        insertBefore( tCaption, firstChild(), exceptioncode );
    }
    return tCaption;
}

void HTMLTableElementImpl::deleteCaption(  )
{
    if(tCaption) {
        int exceptioncode = 0;
        HTMLElementImpl::removeChild(tCaption, exceptioncode);
    }
    tCaption = 0;
}

HTMLElementImpl *HTMLTableElementImpl::insertRow( long index, int &exceptioncode )
{
    // The DOM requires that we create a tbody if the table is empty
    // (cf DOM2TS HTMLTableElement31 test)
    // (note: this is different from "if the table has no sections", since we can have
    // <TABLE><TR>)
    if(!firstBody && !head && !foot && !hasChildNodes())
        setTBody( new HTMLTableSectionElementImpl(docPtr(), ID_TBODY, true /* implicit */) );

    //kdDebug(6030) << k_funcinfo << index << endl;
    // IE treats index=-1 as default value meaning 'append after last'
    // This isn't in the DOM. So, not implemented yet.
    HTMLTableSectionElementImpl* section = 0L;
    HTMLTableSectionElementImpl* lastSection = 0L;
    NodeImpl *node = firstChild();
    bool append = index == -1;
    for ( ; node && (index>=0 || append) ; node = node->nextSibling() )
    {
        if ( node->id() == ID_THEAD || node->id() == ID_TFOOT || node->id() == ID_TBODY )
        {
            section = static_cast<HTMLTableSectionElementImpl *>(node);
            lastSection = section;
            //kdDebug(6030) << k_funcinfo << "section id=" << node->id() << " rows:" << section->numRows() << endl;
            if ( !append )
            {
                int rows = section->numRows();
                if ( rows > index )
                    break;
                else
                    index -= rows;
                //kdDebug(6030) << "       index is now " << index << endl;
            }
        }
        // Note: we now can have both TR and sections (THEAD/TBODY/TFOOT) as children of a TABLE
        else if ( node->id() == ID_TR )
        {
            section = 0L;
            //kdDebug(6030) << k_funcinfo << "row" << endl;
            if (!append && !index)
            {
                // Insert row right here, before "node"
                HTMLTableRowElementImpl* row = new HTMLTableRowElementImpl(docPtr());
                insertBefore(row, node, exceptioncode );
                return row;
            }
            if ( !append )
                index--;
            //kdDebug(6030) << "       index is now " << index << endl;
        }
        section = 0L;
    }
    // Index == 0 means "insert before first row in current section"
    // or "append after last row" (if there's no current section anymore)
    if ( !section && ( index == 0 || append ) )
    {
        section = lastSection;
        index = section ? section->numRows() : 0;
    }
    if ( section && ( index >= 0 || append ) ) {
        //kdDebug(6030) << "Inserting row into section " << section << " at index " << index << endl;
        return section->insertRow( index, exceptioncode );
    } else {
        // No more sections => index is too big
        exceptioncode = DOMException::INDEX_SIZE_ERR;
        return 0L;
    }
}

void HTMLTableElementImpl::deleteRow( long index, int &exceptioncode )
{
    HTMLTableSectionElementImpl* section = 0L;
    NodeImpl *node = firstChild();
    bool lastRow = index == -1;
    HTMLTableSectionElementImpl* lastSection = 0L;
    for ( ; node ; node = node->nextSibling() )
    {
        if ( node->id() == ID_THEAD || node->id() == ID_TFOOT || node->id() == ID_TBODY )
        {
            section = static_cast<HTMLTableSectionElementImpl *>(node);
            lastSection = section;
            int rows = section->numRows();
            if ( !lastRow )
            {
                if ( rows > index )
                    break;
                else
                    index -= rows;
            }
        }
        section = 0L;
    }
    if ( lastRow )
        lastSection->deleteRow( -1, exceptioncode );
    else if ( section && index >= 0 && index < section->numRows() )
        section->deleteRow( index, exceptioncode );
    else
        exceptioncode = DOMException::INDEX_SIZE_ERR;
}

NodeImpl *HTMLTableElementImpl::addChild(NodeImpl *child)
{
#ifdef DEBUG_LAYOUT
    kdDebug( 6030 ) << nodeName().string() << "(Table)::addChild( " << child->nodeName().string() << " )" << endl;
#endif

    switch(child->id())
    {
    case ID_CAPTION:
        return setCaption(static_cast<HTMLTableCaptionElementImpl *>(child));
        break;
    case ID_COL:
    case ID_COLGROUP:
        if(head || foot || firstBody)
            return 0;
	{
	    int exceptioncode = 0;
	    return appendChild(child, exceptioncode);
	}
    case ID_THEAD:
        return setTHead(static_cast<HTMLTableSectionElementImpl *>(child));
        break;
    case ID_TFOOT:
        return setTFoot(static_cast<HTMLTableSectionElementImpl *>(child));
        break;
    case ID_TBODY:
        return setTBody(static_cast<HTMLTableSectionElementImpl *>(child));
        break;
    }
    return 0;
}

void HTMLTableElementImpl::parseAttribute(AttributeImpl *attr)
{
    // ### to CSS!!
    switch(attr->id())
    {
    case ATTR_WIDTH:
        if (!attr->value().isEmpty())
            addCSSLength( CSS_PROP_WIDTH, attr->value() );
        else
            removeCSSProperty(CSS_PROP_WIDTH);
        break;
    case ATTR_HEIGHT:
        if (!attr->value().isEmpty())
            addCSSLength(CSS_PROP_HEIGHT, attr->value());
        else
            removeCSSProperty(CSS_PROP_HEIGHT);
        break;
    case ATTR_BORDER:
    {
        int border;
        // ### this needs more work, as the border value is not only
        //     the border of the box, but also between the cells
        if(!attr->val())
            border = 0;
        else if(attr->val()->l == 0)
            border = 1;
        else
            border = attr->val()->toInt();
#ifdef DEBUG_DRAW_BORDER
        border=1;
#endif
        m_noBorder = !border;
        DOMString v = QString::number( border );
        addCSSLength(CSS_PROP_BORDER_WIDTH, v );
#if 0
        // wanted by HTML4 specs
        if(!border)
            frame = Void, rules = None;
        else
            frame = Box, rules = All;
#endif
        break;
    }
    case ATTR_BGCOLOR:
        if (!attr->value().isEmpty())
            addCSSProperty(CSS_PROP_BACKGROUND_COLOR, attr->value());
        else
            removeCSSProperty(CSS_PROP_BACKGROUND_COLOR);
        break;
    case ATTR_BORDERCOLOR:
        if(!attr->value().isEmpty()) {
            addCSSProperty(CSS_PROP_BORDER_COLOR, attr->value());
            addCSSProperty(CSS_PROP_BORDER_TOP_STYLE, CSS_VAL_SOLID);
            addCSSProperty(CSS_PROP_BORDER_BOTTOM_STYLE, CSS_VAL_SOLID);
            addCSSProperty(CSS_PROP_BORDER_LEFT_STYLE, CSS_VAL_SOLID);
            addCSSProperty(CSS_PROP_BORDER_RIGHT_STYLE, CSS_VAL_SOLID);
            m_solid = true;
        }
        break;
    case ATTR_BACKGROUND:
    {
        if (!attr->value().isEmpty()) {
            QString url = khtml::parseURL( attr->value() ).string();
            url = getDocument()->completeURL( url );
            addCSSProperty(CSS_PROP_BACKGROUND_IMAGE, "url('"+url+"')" );
        }
        else
            removeCSSProperty(CSS_PROP_BACKGROUND_IMAGE);
        break;
    }
    case ATTR_FRAME:
#if 0
        if ( strcasecmp( attr->value(), "void" ) == 0 )
            frame = Void;
        else if ( strcasecmp( attr->value(), "border" ) == 0 )
            frame = Box;
        else if ( strcasecmp( attr->value(), "box" ) == 0 )
            frame = Box;
        else if ( strcasecmp( attr->value(), "hsides" ) == 0 )
            frame = Hsides;
        else if ( strcasecmp( attr->value(), "vsides" ) == 0 )
            frame = Vsides;
        else if ( strcasecmp( attr->value(), "above" ) == 0 )
            frame = Above;
        else if ( strcasecmp( attr->value(), "below" ) == 0 )
            frame = Below;
        else if ( strcasecmp( attr->value(), "lhs" ) == 0 )
            frame = Lhs;
        else if ( strcasecmp( attr->value(), "rhs" ) == 0 )
            frame = Rhs;
#endif
        break;
    case ATTR_RULES:
#if 0
        if ( strcasecmp( attr->value(), "none" ) == 0 )
            rules = None;
        else if ( strcasecmp( attr->value(), "groups" ) == 0 )
            rules = Groups;
        else if ( strcasecmp( attr->value(), "rows" ) == 0 )
            rules = Rows;
        else if ( strcasecmp( attr->value(), "cols" ) == 0 )
            rules = Cols;
        else if ( strcasecmp( attr->value(), "all" ) == 0 )
            rules = All;
#endif
        break;
   case ATTR_CELLSPACING:
        if (!attr->value().isEmpty())
            addCSSLength(CSS_PROP_BORDER_SPACING, attr->value(), true);
        else
            removeCSSProperty(CSS_PROP_BORDER_SPACING);
        break;
    case ATTR_CELLPADDING:
        if (!attr->value().isEmpty()) {
            addCSSLength(CSS_PROP_PADDING_TOP, attr->value(), true);
            addCSSLength(CSS_PROP_PADDING_LEFT, attr->value(), true);
            addCSSLength(CSS_PROP_PADDING_BOTTOM, attr->value(), true);
            addCSSLength(CSS_PROP_PADDING_RIGHT, attr->value(), true);
	    padding = kMax( 0, attr->value().toInt() );
        }
        else {
            removeCSSProperty(CSS_PROP_PADDING_TOP);
            removeCSSProperty(CSS_PROP_PADDING_LEFT);
            removeCSSProperty(CSS_PROP_PADDING_BOTTOM);
            removeCSSProperty(CSS_PROP_PADDING_RIGHT);
	    padding = 1;
        }
        break;
    case ATTR_COLS:
    {
        // ###
#if 0
        int c;
        c = attr->val()->toInt();
        addColumns(c-totalCols);
        break;
#endif
    }
    case ATTR_ALIGN:
        if (!attr->value().isEmpty())
            addCSSProperty(CSS_PROP_FLOAT, attr->value());
        else
            removeCSSProperty(CSS_PROP_FLOAT);
        break;
    case ATTR_VALIGN:
        if (!attr->value().isEmpty())
            addCSSProperty(CSS_PROP_VERTICAL_ALIGN, attr->value());
        else
            removeCSSProperty(CSS_PROP_VERTICAL_ALIGN);
        break;
    case ATTR_NOSAVE:
	break;
    default:
        HTMLElementImpl::parseAttribute(attr);
    }
}

void HTMLTableElementImpl::attach()
{
    if (!m_noBorder) {
        int v = m_solid ? CSS_VAL_SOLID : CSS_VAL_OUTSET;
        addCSSProperty(CSS_PROP_BORDER_TOP_STYLE, v);
        addCSSProperty(CSS_PROP_BORDER_BOTTOM_STYLE, v);
        addCSSProperty(CSS_PROP_BORDER_LEFT_STYLE, v);
        addCSSProperty(CSS_PROP_BORDER_RIGHT_STYLE, v);
    }

    HTMLElementImpl::attach();
    if ( m_render && m_render->isTable() )
	static_cast<RenderTable *>(m_render)->setCellPadding( padding );
}

// --------------------------------------------------------------------------

void HTMLTablePartElementImpl::parseAttribute(AttributeImpl *attr)
{
    switch(attr->id())
    {
    case ATTR_BGCOLOR:
        if (attr->val())
            addCSSProperty(CSS_PROP_BACKGROUND_COLOR, attr->value() );
        else
            removeCSSProperty(CSS_PROP_BACKGROUND_COLOR);
        break;
    case ATTR_BACKGROUND:
    {
        if (attr->val()) {
            QString url = khtml::parseURL( attr->value() ).string();
            url = getDocument()->completeURL( url );
            addCSSProperty(CSS_PROP_BACKGROUND_IMAGE,  "url('"+url+"')" );
        }
        else
            removeCSSProperty(CSS_PROP_BACKGROUND_IMAGE);
        break;
    }
    case ATTR_BORDERCOLOR:
    {
        if(!attr->value().isEmpty()) {
            addCSSProperty(CSS_PROP_BORDER_COLOR, attr->value());
            addCSSProperty(CSS_PROP_BORDER_TOP_STYLE, CSS_VAL_SOLID);
            addCSSProperty(CSS_PROP_BORDER_BOTTOM_STYLE, CSS_VAL_SOLID);
            addCSSProperty(CSS_PROP_BORDER_LEFT_STYLE, CSS_VAL_SOLID);
            addCSSProperty(CSS_PROP_BORDER_RIGHT_STYLE, CSS_VAL_SOLID);
            m_solid = true;
        }
        break;
    }
    case ATTR_VALIGN:
    {
        if (!attr->value().isEmpty())
            addCSSProperty(CSS_PROP_VERTICAL_ALIGN, attr->value());
        else
            removeCSSProperty(CSS_PROP_VERTICAL_ALIGN);
        break;
    }
    case ATTR_HEIGHT:
        if (!attr->value().isEmpty())
            addCSSLength(CSS_PROP_HEIGHT, attr->value());
        else
            removeCSSProperty(CSS_PROP_HEIGHT);
        break;
    case ATTR_NOSAVE:
	break;
    default:
        HTMLElementImpl::parseAttribute(attr);
    }
}

// -------------------------------------------------------------------------

HTMLTableSectionElementImpl::HTMLTableSectionElementImpl(DocumentPtr *doc,
                                                         ushort tagid, bool implicit)
    : HTMLTablePartElementImpl(doc)
{
    _id = tagid;
    m_implicit = implicit;
}

HTMLTableSectionElementImpl::~HTMLTableSectionElementImpl()
{
}

NodeImpl::Id HTMLTableSectionElementImpl::id() const
{
    return _id;
}


// these functions are rather slow, since we need to get the row at
// the index... but they aren't used during usual HTML parsing anyway
HTMLElementImpl *HTMLTableSectionElementImpl::insertRow( long index, int& exceptioncode )
{
    HTMLTableRowElementImpl *r = 0L;
    NodeListImpl *children = childNodes();
    int numRows = children ? (int)children->length() : 0;
    //kdDebug(6030) << k_funcinfo << "index=" << index << " numRows=" << numRows << endl;
    if ( index < -1 || index > numRows ) {
        exceptioncode = DOMException::INDEX_SIZE_ERR; // per the DOM
    }
    else
    {
        r = new HTMLTableRowElementImpl(docPtr());
        if ( numRows == index || index == -1 )
            appendChild(r, exceptioncode);
        else {
            NodeImpl *n;
            if(index < 1)
                n = firstChild();
            else
                n = children->item(index);
            insertBefore(r, n, exceptioncode );
        }
    }
    delete children;
    return r;
}

void HTMLTableSectionElementImpl::deleteRow( long index, int &exceptioncode )
{
    NodeListImpl *children = childNodes();
    int numRows = children ? (int)children->length() : 0;
    if ( index == -1 ) index = numRows - 1;
    if( index >= 0 && index < numRows )
        HTMLElementImpl::removeChild(children->item(index), exceptioncode);
    else
        exceptioncode = DOMException::INDEX_SIZE_ERR;
    delete children;
}


int HTMLTableSectionElementImpl::numRows() const
{
    int rows = 0;
    const NodeImpl *n = firstChild();
    while (n) {
        if (n->id() == ID_TR)
            rows++;
        n = n->nextSibling();
    }

    return rows;
}

// -------------------------------------------------------------------------

HTMLTableRowElementImpl::HTMLTableRowElementImpl(DocumentPtr *doc)
  : HTMLTablePartElementImpl(doc)
{
}

HTMLTableRowElementImpl::~HTMLTableRowElementImpl()
{
}

NodeImpl::Id HTMLTableRowElementImpl::id() const
{
    return ID_TR;
}

long HTMLTableRowElementImpl::rowIndex() const
{
    // some complex traversal stuff here to take into account that some rows may be in different sections
    int rIndex = 0;
    const NodeImpl *n = this;
    do {
        while (!n->previousSibling() && !(n->isElementNode() && n->id() == ID_TABLE))
            n = n->parentNode();
        if (n->isElementNode() && n->id() == ID_TABLE)
            n = 0;
        if (n) {
            n = n->previousSibling();
            while (!(n->isElementNode() && n->id() == ID_TR) && n->lastChild())
                n = n->lastChild();
        }

        if (n && n->isElementNode() && n->id() == ID_TR)
            rIndex++;
    }
    while (n && n->isElementNode() && n->id() == ID_TR);

    return rIndex;
}

long HTMLTableRowElementImpl::sectionRowIndex() const
{
    int rIndex = 0;
    const NodeImpl *n = this;
    do {
        n = n->previousSibling();
        if (n && n->isElementNode() && n->id() == ID_TR)
            rIndex++;
    }
    while (n);

    return rIndex;
}

HTMLElementImpl *HTMLTableRowElementImpl::insertCell( long index, int &exceptioncode )
{
    HTMLTableCellElementImpl *c = 0L;
    NodeListImpl *children = childNodes();
    int numCells = children ? children->length() : 0;
    if ( index < -1 || index > numCells )
        exceptioncode = DOMException::INDEX_SIZE_ERR; // per the DOM
    else
    {
        c = new HTMLTableCellElementImpl(docPtr(), ID_TD);
        if(numCells == index || index == -1)
            appendChild(c, exceptioncode);
        else {
            NodeImpl *n;
            if(index < 1)
                n = firstChild();
            else
                n = children->item(index);
            insertBefore(c, n, exceptioncode);
        }
    }
    delete children;
    return c;
}

void HTMLTableRowElementImpl::deleteCell( long index, int &exceptioncode )
{
    NodeListImpl *children = childNodes();
    int numCells = children ? children->length() : 0;
    if ( index == -1 ) index = numCells-1;
    if( index >= 0 && index < numCells )
        HTMLElementImpl::removeChild(children->item(index), exceptioncode);
    else
        exceptioncode = DOMException::INDEX_SIZE_ERR;
    delete children;
}

// -------------------------------------------------------------------------

HTMLTableCellElementImpl::HTMLTableCellElementImpl(DocumentPtr *doc, int tag)
  : HTMLTablePartElementImpl(doc)
{
  _col = -1;
  _row = -1;
  cSpan = rSpan = 1;
  m_nowrap = false;
  _id = tag;
  rowHeight = 0;
}

HTMLTableCellElementImpl::~HTMLTableCellElementImpl()
{
}

void HTMLTableCellElementImpl::parseAttribute(AttributeImpl *attr)
{
    switch(attr->id())
    {
    case ATTR_BORDER:
        // euhm? not supported by other browsers as far as I can see (Dirk)
        //addCSSLength(CSS_PROP_BORDER_WIDTH, attr->value());
        break;
    case ATTR_ROWSPAN:
        // ###
        rSpan = attr->val() ? attr->val()->toInt() : 1;
        // limit this to something not causing an overflow with short int
        if(rSpan < 1 || rSpan > 1024) rSpan = 1;
        break;
    case ATTR_COLSPAN:
        // ###
        cSpan = attr->val() ? attr->val()->toInt() : 1;
        // limit this to something not causing an overflow with short int
        if(cSpan < 1 || cSpan > 1024) cSpan = 1;
        break;
    case ATTR_NOWRAP:
        m_nowrap = (attr->val() != 0);
        break;
    case ATTR_WIDTH:
        if (!attr->value().isEmpty())
            addCSSLength( CSS_PROP_WIDTH, attr->value() );
        else
            removeCSSProperty(CSS_PROP_WIDTH);
        break;
    case ATTR_NOSAVE:
	break;
    default:
        HTMLTablePartElementImpl::parseAttribute(attr);
    }
}

void HTMLTableCellElementImpl::attach()
{
    HTMLElementImpl* p = static_cast<HTMLElementImpl*>(parentNode());
    while(p && p->id() != ID_TABLE)
        p = static_cast<HTMLElementImpl*>(p->parentNode());

    if(p) {
        HTMLTableElementImpl* table = static_cast<HTMLTableElementImpl*>(p);
        if (table->m_noBorder) {
            addCSSProperty(CSS_PROP_BORDER_WIDTH, "0");
        }
        else {
            addCSSProperty(CSS_PROP_BORDER_WIDTH, "1px");
            int v = (table->m_solid || m_solid) ? CSS_VAL_SOLID : CSS_VAL_INSET;
            addCSSProperty(CSS_PROP_BORDER_TOP_STYLE, v);
            addCSSProperty(CSS_PROP_BORDER_BOTTOM_STYLE, v);
            addCSSProperty(CSS_PROP_BORDER_LEFT_STYLE, v);
            addCSSProperty(CSS_PROP_BORDER_RIGHT_STYLE, v);

            if (!m_solid)
                addCSSProperty(CSS_PROP_BORDER_COLOR, "inherit");
        }
    }

    HTMLTablePartElementImpl::attach();
}

// -------------------------------------------------------------------------

HTMLTableColElementImpl::HTMLTableColElementImpl(DocumentPtr *doc, ushort i)
    : HTMLTablePartElementImpl(doc)
{
    _id = i;
    _span = (_id == ID_COLGROUP ? 0 : 1);
}

HTMLTableColElementImpl::~HTMLTableColElementImpl()
{
}

NodeImpl::Id HTMLTableColElementImpl::id() const
{
    return _id;
}


void HTMLTableColElementImpl::parseAttribute(AttributeImpl *attr)
{
    switch(attr->id())
    {
    case ATTR_SPAN:
        _span = attr->val() ? attr->val()->toInt() : 1;
        break;
    case ATTR_WIDTH:
        if (!attr->value().isEmpty())
            addCSSLength(CSS_PROP_WIDTH, attr->value(), false, true );
        else
            removeCSSProperty(CSS_PROP_WIDTH);
        break;
    case ATTR_VALIGN:
        if (!attr->value().isEmpty())
            addCSSProperty(CSS_PROP_VERTICAL_ALIGN, attr->value());
        else
            removeCSSProperty(CSS_PROP_VERTICAL_ALIGN);
        break;
    default:
        HTMLTablePartElementImpl::parseAttribute(attr);
    }

}

// -------------------------------------------------------------------------

HTMLTableCaptionElementImpl::HTMLTableCaptionElementImpl(DocumentPtr *doc)
  : HTMLTablePartElementImpl(doc)
{
}

HTMLTableCaptionElementImpl::~HTMLTableCaptionElementImpl()
{
}

NodeImpl::Id HTMLTableCaptionElementImpl::id() const
{
    return ID_CAPTION;
}


void HTMLTableCaptionElementImpl::parseAttribute(AttributeImpl *attr)
{
    switch(attr->id())
    {
    case ATTR_ALIGN:
        if (!attr->value().isEmpty())
            addCSSProperty(CSS_PROP_CAPTION_SIDE, attr->value());
        else
            removeCSSProperty(CSS_PROP_CAPTION_SIDE);
        break;
    default:
        HTMLElementImpl::parseAttribute(attr);
    }

}
