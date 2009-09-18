/**
 * This file is part of the DOM implementation for KDE.
 *
 * (C) 1999 Lars Knoll (knoll@kde.org)
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
 * $Id: dom_element.cpp,v 1.30.2.7 2003/06/30 23:52:13 faure Exp $
 */

#include "dom/dom_exception.h"
#include "xml/dom_docimpl.h"
#include "xml/dom_elementimpl.h"
#include "html/html_formimpl.h"
#include <kdebug.h>

using namespace DOM;

Attr::Attr() : Node()
{
}

Attr::Attr(const Attr &other) : Node(other)
{
}

Attr::Attr( AttrImpl *_impl )
{
    impl= _impl;
    if (impl) impl->ref();
}

Attr &Attr::operator = (const Node &other)
{
    NodeImpl* ohandle = other.handle();
    if ( impl != ohandle ) {
        if (!ohandle || !ohandle->isAttributeNode()) {
            if (impl) impl->deref();
            impl = 0;
        } else {
            Node::operator =(other);
        }
    }
    return *this;
}

Attr &Attr::operator = (const Attr &other)
{
    Node::operator =(other);
    return *this;
}

Attr::~Attr()
{
}

DOMString Attr::name() const
{
    if (!impl) throw DOMException(DOMException::NOT_FOUND_ERR);
    return impl->getDocument()->attrName(
        static_cast<AttrImpl*>(impl)->attrImpl()->id());
}


bool Attr::specified() const
{
  if (impl) return ((AttrImpl *)impl)->specified();
  return 0;
}

Element Attr::ownerElement() const
{
  if (!impl) return 0;
  return static_cast<AttrImpl*>(impl)->ownerElement();
}

DOMString Attr::value() const
{
    if (!impl) throw DOMException(DOMException::NOT_FOUND_ERR);
    return impl->nodeValue();
}

void Attr::setValue( const DOMString &newValue )
{
  if (!impl)
    return;

  int exceptioncode = 0;
  ((AttrImpl *)impl)->setValue(newValue,exceptioncode);
  if (exceptioncode)
    throw DOMException(exceptioncode);
}

// ---------------------------------------------------------------------------

Element::Element() : Node()
{
}

Element::Element(const Element &other) : Node(other)
{
}

Element::Element(ElementImpl *impl) : Node(impl)
{
}

Element &Element::operator = (const Node &other)
{
    NodeImpl* ohandle = other.handle();
    if ( impl != ohandle ) {
        if (!ohandle || !ohandle->isElementNode()) {
            if (impl) impl->deref();
            impl = 0;
	} else {
            Node::operator =(other);
	}
    }
    return *this;
}

Element &Element::operator = (const Element &other)
{
    Node::operator =(other);
    return *this;
}

Element::~Element()
{
}

DOMString Element::tagName() const
{
    if (!impl) throw DOMException(DOMException::NOT_FOUND_ERR);
    return static_cast<ElementImpl*>(impl)->tagName();
}

DOMString Element::getAttribute( const DOMString &name )
{
    return getAttributeNS(DOMString(), name);
}

void Element::setAttribute( const DOMString &name, const DOMString &value )
{
    setAttributeNS(DOMString(), name, value);
}

void Element::removeAttribute( const DOMString &name )
{
    removeAttributeNS(DOMString(), name);
}

Attr Element::getAttributeNode( const DOMString &name )
{
    return getAttributeNodeNS(DOMString(), name);
}

Attr Element::setAttributeNode( const Attr &newAttr )
{
    return setAttributeNodeNS(newAttr);
}

Attr Element::removeAttributeNode( const Attr &oldAttr )
{
    if (!impl || oldAttr.isNull() || oldAttr.ownerElement().handle() != impl)
        throw DOMException(DOMException::NOT_FOUND_ERR);
    if (impl->getDocument() != oldAttr.handle()->getDocument())
        throw DOMException(DOMException::WRONG_DOCUMENT_ERR);

    int exceptioncode = 0;

    NodeImpl::Id id = static_cast<AttrImpl *>(oldAttr.handle())->attrImpl()->id();
    Attr r = static_cast<ElementImpl*>(impl)->attributes(true)->removeNamedItem(id, exceptioncode);
    if ( exceptioncode )
        throw DOMException( exceptioncode );
    return r;
}

NodeList Element::getElementsByTagName( const DOMString &name )
{
    if (!impl) return 0;
    return static_cast<ElementImpl*>(impl)->
        getElementsByTagNameNS(0, name.implementation());
}

NodeList Element::getElementsByTagNameNS( const DOMString &namespaceURI,
                                          const DOMString &localName )
{
    if (!impl) return 0;
    return static_cast<ElementImpl*>(impl)->
        getElementsByTagNameNS(namespaceURI.implementation(), localName.implementation());
}

DOMString Element::getAttributeNS( const DOMString &namespaceURI,
                                   const DOMString &localName)
{
    if (!impl) throw DOMException(DOMException::NOT_FOUND_ERR);
    NodeImpl::Id id = impl->getDocument()->attrId(namespaceURI.implementation(),
                                                  localName.implementation(), true,
                                                  0);
    if (!id) return DOMString();
    return static_cast<ElementImpl*>(impl)->getAttribute(id);
}

void Element::setAttributeNS( const DOMString &namespaceURI,
                              const DOMString &qualifiedName,
                              const DOMString &value)
{
    if (!impl) throw DOMException(DOMException::NOT_FOUND_ERR);

    int colonpos = qualifiedName.find(':');
    DOMString localName = qualifiedName;
    if (colonpos >= 0) {
        localName.remove(0, colonpos+1);
        // ### extract and set new prefix
    }
    int exceptioncode = 0;
    NodeImpl::Id id = impl->getDocument()->attrId(namespaceURI.implementation(),
                                                  localName.implementation(), false /* allocate */,
                                                  &exceptioncode);
    if (exceptioncode)
        throw DOMException(exceptioncode);
    static_cast<ElementImpl*>(impl)->setAttribute(id, value.implementation(), exceptioncode);
    if (exceptioncode)
        throw DOMException(exceptioncode);
}

void Element::removeAttributeNS( const DOMString &namespaceURI,
                                 const DOMString &localName )
{
    if (!impl) throw DOMException(DOMException::NOT_FOUND_ERR);
    NodeImpl::Id id = impl->getDocument()->attrId(namespaceURI.implementation(),
                                                  localName.implementation(), true,
                                                  0);
    if (!id) return;

    int exceptioncode = 0;
    ((ElementImpl *)impl)->removeAttribute(id, exceptioncode);
    if ( exceptioncode )
        throw DOMException( exceptioncode );
}

Attr Element::getAttributeNodeNS( const DOMString &namespaceURI,
                                  const DOMString &localName )
{
    if (!impl) throw DOMException(DOMException::NOT_FOUND_ERR);
    NodeImpl::Id id = impl->getDocument()->attrId(namespaceURI.implementation(),
                                                  localName.implementation(), true,
                                                  0);
    if (!id) return 0;

    ElementImpl* e = static_cast<ElementImpl*>(impl);
    if (!e->attributes()) return 0; // exception ?

    return e->attributes()->getNamedItem(id);
}

Attr Element::setAttributeNodeNS( const Attr &newAttr )
{
    if (!impl || newAttr.isNull())
        throw DOMException(DOMException::NOT_FOUND_ERR);

    int exceptioncode = 0;
    Attr r = static_cast<ElementImpl*>(impl)->attributes(false)->setNamedItem(newAttr.handle(), exceptioncode);
    if ( exceptioncode )
        throw DOMException( exceptioncode );
    static_cast<AttrImpl *>(newAttr.handle())->setOwnerElement( static_cast<ElementImpl*>(impl) );
    return r;
}


bool Element::hasAttribute( const DOMString& name )
{
    return hasAttributeNS(DOMString(), name);
}

bool Element::hasAttributeNS( const DOMString &namespaceURI,
                              const DOMString &localName )
{
    if (!impl || !static_cast<ElementImpl*>(impl)->attributes()) return false; // ### throw ?
    NodeImpl::Id id = impl->getDocument()->attrId(namespaceURI.implementation(),
                                                  localName.implementation(), true,
                                                  0);
    if (!id) return false;

    if (!static_cast<ElementImpl*>(impl)->attributes(true /*readonly*/)) return false;
    return static_cast<ElementImpl*>(impl)->attributes(true)->getAttributeItem(id) != 0;
}

bool Element::isHTMLElement() const
{
    if(!impl) return false;
    return ((ElementImpl *)impl)->isHTMLElement();
}

Element Element::form() const
{
    if (!impl || !impl->isGenericFormElement()) return 0;
    return static_cast<HTMLGenericFormElementImpl*>(impl)->form();
}

CSSStyleDeclaration Element::style()
{
    if (impl) return ((ElementImpl *)impl)->styleRules();
    return 0;
}

bool Element::khtmlValidAttrName(const DOMString &name)
{
    // Check if name is valid
    // http://www.w3.org/TR/2000/REC-xml-20001006#NT-Name
    DOMStringImpl* _name = name.implementation();
    QChar ch = _name->s[0];
    if ( !ch.isLetter() && ch != '_' && ch != ':' )
        return false; // first char isn't valid
    for ( uint i = 0; i < _name->l; ++i )
    {
        ch = _name->s[i];
        if ( !ch.isLetter() && !ch.isDigit() && ch != '.'
             && ch != '-' && ch != '_' && ch != ':'
             && ch.category() != QChar::Mark_SpacingCombining
             /* no idea what "extender is" */ )
            return false;
    }
    return true;
}

bool Element::khtmlValidPrefix(const DOMString &name)
{
    // Null prefix is ok. If not null, reuse code from khtmlValidAttrName
    return !name.implementation() || khtmlValidAttrName(name);
}

bool Element::khtmlValidQualifiedName(const DOMString &name)
{
    return khtmlValidAttrName(name);
}

bool Element::khtmlMalformedQualifiedName(const DOMString &name)
{
    // #### Not clearly defined in the DOM spec...
    // But we know for sure that a null qualified name is malformed
    return name.isNull();
}

bool Element::khtmlMalformedPrefix(const DOMString &/*name*/)
{
    // ####
    return false;
}
