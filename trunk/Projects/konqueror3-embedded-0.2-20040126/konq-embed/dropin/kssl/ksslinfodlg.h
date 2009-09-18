#ifndef __ksslinfodlg_h__
#define __ksslinfodlg_h__

#include <qstring.h>

#include <ksslcertificate.h>
#include <kssl.h>

class QWidget;

class KSSLInfoDlg
{
public:
    KSSLInfoDlg( bool secureConnection, QWidget *parent, const char *, bool );

    void setSecurityInQuestion(bool);

    void setup( KSSLCertificate* /*cert*/,
		const QString &peername,
                const QString &issuer,
                const QString &ip,
                const QString &url,
                const QString &cipher,
                int usedbits,
                int bits,
                KSSLCertificate::KSSLValidation certstate,
                const QString &goodFrom = QString::null,
                const QString &goodUntil = QString::null);

    void exec();

private:
    bool m_ssl;
    QWidget *m_parent;
    QString m_moreInfo;
};

#endif
