#include <unistd.h>
#include <sys/types.h>
#include <qstring.h>
#include <qvaluelist.h>
#include <qcstring.h>


class sysexec
{
public:   
    int ezxUid();
    int ezxGid();
    int DRMUid();
	pid_t sysexec::doExec(const QString & execPath,int gid, int uid, 
    	const QValueList<QCString> & args,   const QValueList<QCString> & env );
    void setSameGidUid(int gid, int uid);   
    char ** stringListToStr(const QValueList<QCString> & strList);
    void setEnv(const QValueList<QCString>& env);
    pid_t exec( const QString & execPath);
};
