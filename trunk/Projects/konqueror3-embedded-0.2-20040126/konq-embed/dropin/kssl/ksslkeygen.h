#ifndef _KSSLKEYGEN_H
#define _KSSLKEYGEN_H

#include <qdialog.h>
#include <qstringlist.h>

class KSSLKeyGen {

public:
    KSSLKeyGen(QWidget*, const char*, bool) {};

    static QStringList supportedKeySizes() { return QStringList(); }

    void setKeySize(int idx) { };

    QDialog::DialogCode exec() { return QDialog::Rejected; };
};

#endif

