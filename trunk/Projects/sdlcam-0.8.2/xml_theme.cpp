/*
	SDLcam:  A program to view and apply effects in real-time to video
	Copyright (C) 2002, Raphael Assenat
	 
	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.
					 
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
								 
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/										

/*
Author: Thomas Luﬂnig <thomas.lussnig@bewegungsmelder.de>

Load XML Gui definition
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <libxml/tree.h>
#include <libxml/parser.h>

#include "SDL_v4l.h"
#include "interface.h"
#include "main.h"

/* {{{ for fast looking should be removed later
struct xmlDoc {
	void           *_private;	// application data //
	xmlElementType  type;       	// XML_DOCUMENT_NODE, must be second ! //
	char           *name;		// name/filename/URI of the document //
	struct _xmlNode *children;	// the document tree //
	struct _xmlNode *last;		// last child link //
	struct _xmlNode *parent;	// child->parent link //
	struct _xmlNode *next;		// next sibling link  //
	struct _xmlNode *prev;		// previous sibling link  //
	struct _xmlDoc  *doc;		// autoreference to itself //
	// End of common part //
	int             compression;	// level of zlib compression //
	int             standalone; 	// standalone document (no external refs) //
	struct _xmlDtd  *intSubset;	// the document internal subset //
	struct _xmlDtd  *extSubset;	// the document external subset //
	struct _xmlNs   *oldNs;		// Global namespace, the old way //
	const xmlChar   *version;	// the XML version string //
	const xmlChar   *encoding;   	// external initial encoding, if any //
	void            *ids;        	// Hash table for ID attributes if any //
	void            *refs;       	// Hash table for IDREFs attributes if any //
	const xmlChar   *URL;		// The URI for that document //
	int              charset;    	// encoding of the in-memory content actually an xmlCharEncoding //
	};
struct xmlNode {
	void           *_private;	// application data //
	xmlElementType   type;		// type number, must be second ! //
	const xmlChar   *name;      	// the name of the node, or the entity //
	struct _xmlNode *children;	// parent->childs link //
	struct _xmlNode *last;		// last child link //
	struct _xmlNode *parent;	// child->parent link //
	struct _xmlNode *next;		// next sibling link  //
	struct _xmlNode *prev;		// previous sibling link  //
	struct _xmlDoc  *doc;		// the containing document //
	// End of common part //
	xmlNs           *ns;        	// pointer to the associated namespace //
	xmlChar         *content;   	// the content //
	struct _xmlAttr *properties;	// properties list //
	xmlNs           *nsDef;     	// namespace definitions on this node //
	};
struct xmlAttr {
 	void           *_private;	// application data //
 	xmlElementType   type;      	// XML_ATTRIBUTE_NODE, must be second ! //
 	const xmlChar   *name;      	// the name of the property //
 	struct _xmlNode *children;	// the value of the property //
 	struct _xmlNode *last;		// NULL //
 	struct _xmlNode *parent;	// child->parent link //
 	struct _xmlAttr *next;		// next sibling link  //
 	struct _xmlAttr *prev;		// previous sibling link  //
 	struct _xmlDoc  *doc;		// the containing document //
 	xmlNs           *ns;        	// pointer to the associated namespace //
 	xmlAttributeType atype;     	// the attribute type if validating //
	};
typedef enum {
	XML_ELEMENT_NODE=		1,
	XML_ATTRIBUTE_NODE=		2,
	XML_TEXT_NODE=			3,
	XML_CDATA_SECTION_NODE=		4,
	XML_ENTITY_REF_NODE=		5,
	XML_ENTITY_NODE=		6,
	XML_PI_NODE=			7,
	XML_COMMENT_NODE=		8,
	XML_DOCUMENT_NODE=		9,
	XML_DOCUMENT_TYPE_NODE=		10,
	XML_DOCUMENT_FRAG_NODE=		11,
	XML_NOTATION_NODE=		12,
	XML_HTML_DOCUMENT_NODE=		13,
	XML_DTD_NODE=			14,
	XML_ELEMENT_DECL=		15,
	XML_ATTRIBUTE_DECL=		16,
	XML_ENTITY_DECL=		17,
	XML_NAMESPACE_DECL=		18,
	XML_XINCLUDE_START=		19,
	XML_XINCLUDE_END=		20
	#ifdef LIBXML_DOCB_ENABLED
	,XML_DOCB_DOCUMENT_NODE=	21
	#endif
	} xmlElementType;
 }}} */
void Print_Type(xmlElementType type) {{{
	switch(type) {
	case XML_ELEMENT_NODE 	: printf("Element\n");		break;
	case XML_DOCUMENT_NODE	: printf("Document\n");		break;
	case XML_TEXT_NODE	: printf("Text\n");		break;
	default		      	: printf("Undefined %i\n", type); break;
		}
	}}}

const char *Content  (const xmlNode *akt) {{{
	if(akt->children==NULL) return NULL;
	akt=akt->children;
	if(akt->type!=XML_TEXT_NODE) return NULL;
	return (char*) akt->content;
	}}}
const char *Attrib   (xmlNode *akt,const char *key) {{{
	xmlAttr *attr = akt->properties;
	while(attr!=NULL) {
		if(0==strcmp(key, (char*)attr->name)) {
			if(attr->children!=NULL || attr->children->type==XML_TEXT_NODE)
			  return (char*) attr->children->content;
			}	
		attr = attr->next;
		}
	return NULL;
	}}}
int Parse_Key        (xmlNode *akt) {{{
	do {
		while (akt!=NULL  && akt->type==XML_TEXT_NODE) akt=akt->next;
		if (akt == NULL ) return -1;
		if (0 == strcmp ("Key", (char*)akt->name)) 
			Hotkey_add ((char*)Attrib(akt,"id"), (char*)Content(akt),0);
		if (0 == strcmp ("Res", (char*)akt->name)) 
			Hotkey_add (Attrib(akt,"id"),Content(akt),4);
		akt = akt->next;
		} while(akt!=NULL);
	return 1;
	}}}
int Parse_Keybinding (xmlNode *akt) {{{
	Hotkey_clear ();
	while (akt!=NULL && akt->type==XML_TEXT_NODE) akt=akt->next;
	if(akt == NULL			) return -1;
	Parse_Key (akt->children);
	return 1;
	}}}
SDL_Color *Get_Color(const char *col,SDL_Color *def) {{{
	if (NULL == col) return def;
	if (0 == strcmp("red"   , col))	return &red;
	if (0 == strcmp("yellow", col))	return &yellow;
	if (0 == strcmp("white" , col))	return &white;
	if (0 == strcmp("green" , col))	return &green;
	return def;
	}}}

const char *Parse_Source (xmlNode *akt) {{{
static	char ID[19];
	int v4l_id   = Add_Input ( Attrib(akt,"type"),  Attrib(akt,"device"));
	if (v4l_id < 0) return NULL;
	const char *width  = Attrib(akt,"width");
	const char *height = Attrib(akt,"height");
	if (height != NULL && width != NULL) {
		v4l[v4l_id]->Resolution (atoi (width), atoi(height));
		}
	sprintf(ID, "%18i", v4l_id);
	return ID;
	}}}

int Parse_Elements    (xmlNode *akt) {{{
	akt = akt->children;
	while (akt != NULL) {
		if (akt->type==XML_ELEMENT_NODE) {
			TTF_Font  *font;
			SDL_Color *fg   = Get_Color(Attrib(akt,"fg"),&red  );
			SDL_Color *bg   = Get_Color(Attrib(akt,"bg"),&black);
			int	   tab  = atoi(Attrib(akt,"tab" ));
			int	   left = atoi(Attrib(akt,"left"));
			int	   top  = atoi(Attrib(akt,"top" ));

			switch(atoi(Attrib(akt,"font"))) {
			default : font = font2; break;
			case 1  : font = font1; break;
			case 2  : font = font2; break;
			case 3  : font = font3; break;
				}
			if (0 == strcmp ("Element"   , (char*)akt->name)) Hotspot_add(Content(akt), tab, left, top, Attrib(akt,"action"), font, fg, bg, 0);
			if (0 == strcmp ("Value"     , (char*)akt->name)) Hotspot_add(Content(akt), tab, left, top, Attrib(akt,"value" ), font, fg, bg, 1);
			if (0 == strcmp ("Filter"    , (char*)akt->name)) Hotspot_add(NULL	  , tab, left, top, Attrib(akt,"name"  ), font, fg, bg, 2);
			if (0 == strcmp ("Tab"       , (char*)akt->name)) Hotspot_add(Content(akt), tab, left, top, Attrib(akt,"action"), font, fg, bg, 3);
			if (0 == strcmp ("Resolution", (char*)akt->name)) Hotspot_add(Content(akt), tab, left, top, Attrib(akt,"size"  ), font, fg, bg, 4);
			if (0 == strcmp ("Spezial"   , (char*)akt->name)) Hotspot_add(NULL        , tab, left, top, Attrib(akt,"name"  ), font, fg, bg, 5);
			if (0 == strcmp ("Source"     , (char*)akt->name)) {{{
				const char *ID = Parse_Source (akt);
				if (ID != NULL) Hotspot_add(Content(akt), tab, left, top, ID, font, fg, bg, 6);
				}}}
		}
		akt=akt->next;
		}
	return 1;
	}}}
int Parse_Blocks      (xmlNode *akt) {{{
	akt = akt->children;
	while (akt != NULL) {
		if (akt->type == XML_ELEMENT_NODE) {{{
			if (0 == strcmp ("SideBar"  , (char*)akt->name)) {{{
				SIDE_BAR_X = atoi (Attrib (akt, "x"));
				SIDE_BAR_Y = atoi (Attrib (akt, "y"));
				SIDE_BAR_W = atoi (Attrib (akt, "w"));
				SIDE_BAR_H = atoi (Attrib (akt, "h"));
				}}}
			if (0 == strcmp ("BottomBar", (char*)akt->name)) {{{
				BOTTOM_BAR_X = atoi (Attrib (akt, "x"));
				BOTTOM_BAR_Y = atoi (Attrib (akt, "y"));
				BOTTOM_BAR_W = atoi (Attrib (akt, "w"));
				BOTTOM_BAR_H = atoi (Attrib (akt, "h"));
				}}}
			if (0 == strcmp ("Image"    , (char*)akt->name)) {{{
				IMAGE_X = atoi (Attrib (akt, "x"));
				IMAGE_Y = atoi (Attrib (akt, "y"));
				}}}
			}}}
		akt=akt->next;
		}
	return 1;
	}}}
int Parse_GUILayout   (xmlNode *akt) {{{
	akt = akt->children;
	while (akt != NULL) {
		if (akt->type == XML_ELEMENT_NODE) {
			if (0 == strcmp ("Elements" , (char*)akt->name)) Parse_Elements (akt);
			if (0 == strcmp ("GUIBlocks", (char*)akt->name)) Parse_Blocks   (akt);
			}
		akt=akt->next;
		}
	return 1;
	}}}
int Parse_Config_File (xmlNode *akt) {{{
	if (akt == NULL			) return -1;
	if (akt->type != XML_ELEMENT_NODE  ) return -2;
	if (0 != strcmp ("SDLcam", (char*)akt->name)) return -3;
	akt=akt->children;
	while(akt!=NULL) {
		if (0 == strcmp ("KeyBindings"	, (char*)akt->name)) Parse_Keybinding (akt);
		if (0 == strcmp ("GUILayout"	, (char*)akt->name)) Parse_GUILayout  (akt);
		akt=akt->next;
		}
//	Print_Type(akt->type);
//	printf("Name %s\n",akt->name    );
//	Parse_Keybinding(akt->children);
	return 1;
	}}}
void XML_Theme(const char *filename) {{{
	xmlDoc *doc = xmlParseFile (filename);
	Parse_Config_File (doc->children);
	xmlFreeDoc (doc);
	}}}

/*
 * vim600: fdm=marker
 */
