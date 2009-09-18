#ifndef HTMLPAGEINFO_H
#define HTMLPAGEINFO_H

/*
 * dummy HtmlPageInfo class to make khtml v3.1 happy
 *
 */

#include <qwidget.h>

class KHTMLInfoDlg 
{
public:
    KHTMLInfoDlg( QWidget* parent = 0, 
		  const char* name = 0, bool modal = FALSE, int fl = 0 ) {}
    ~KHTMLInfoDlg() {}
};

#endif
