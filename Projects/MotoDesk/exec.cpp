#include <qfile.h>
#include <qstring.h>
#include <qtextstream.h>
#include <qstringlist.h>
#include <qvaluelist.h>
#include <qcstring.h>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "exec.h"

pid_t sysexec::exec( const QString & execPath)
{
    //zEnter();
    QValueList<QCString> args;
    QValueList<QCString> env;
    
    return doExec(execPath,
        ezxGid(),
        ezxUid(),
        args,
        env
        );
}

int sysexec::ezxUid()
{
    return 2000;
}

int sysexec::ezxGid()
{
    return 233;
}

int sysexec::DRMUid(  )
{
    return 518;
}

char ** sysexec::stringListToStr(const QValueList<QCString> & strList)
{
    //zEnter();
    int strCount = strList.count ();
    char ** ppA = new char * [strCount + 1];
    for (int i = 0; i < strCount; i ++)
    {
        ppA[i] = qstrdup (strList[i]);
    }

    ppA[strCount] = NULL;

    return ppA;
}

void sysexec::setEnv(const QValueList<QCString>& env)
{
    //zEnter();
    for ( QValueList<QCString>::ConstIterator it = env.begin(); it != env.end(); ++it ) 
    {
        ::putenv(qstrdup((*it)));
    }
}

void sysexec::setSameGidUid(int gid, int uid)
{
    //zEnter();
    // setuid, setgid
    ::setgid( gid );
    ::setuid( uid );
    
    // setreuid, setregid
    ::setregid( gid, gid );
    ::setreuid( uid, uid );
    
    // euid egid
    ::setegid( gid );
    ::seteuid( uid );
}

pid_t sysexec::doExec(const QString & execPath,
    int gid, 
    int uid, 
    const QValueList<QCString> & args, 
    const QValueList<QCString> & env
    )
{
    //zEnter();
    pid_t pid;
    pid = ::vfork();

    if ( 0 == pid )
    {
        int nv = getpriority( PRIO_PROCESS, 0 );
        nice( -nv );

        for ( int fd = 3; fd < 100; fd++ )
        {
            ::close( fd );
        }

        setSameGidUid(gid, uid);

        //set enviroments
        setEnv(env);

        //generate arguement
        QValueList<QCString> fullArgs(args);
        fullArgs.prepend(QFile::encodeName( execPath ));
        char **  argList = stringListToStr(fullArgs);

        if ( ::execv( QFile::encodeName( execPath )
              ,argList) < 0 )
        {
            perror( "::execl error:" );
        }

        ::_exit( -1 );
    }

    //zLeave();
    return pid;

}
