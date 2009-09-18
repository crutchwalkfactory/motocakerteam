#ifndef __klineeditdlg_h__
#define __klineeditdlg_h__

#include <qinputdialog.h>

class KLineEditDlg
{
public:
  static QString getText(const QString &_caption, const QString &_text,
                         const QString& _value=QString::null,
                         bool *ok=0, QWidget *parent=0,
			 QValidator *validator=0) {
    return QInputDialog::getText(_caption, _text, QLineEdit::Normal, _value, ok, parent);
  }
};

#endif

