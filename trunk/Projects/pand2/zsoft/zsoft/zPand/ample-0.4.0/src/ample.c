/*
 * Ample - An MP3 lender
 *
 * (c) Copyright - David Härdeman <david@2gen.com> - 2001
 *
 */

/*
 * $Id: ample.c,v 1.32 2001/12/16 01:35:44 alphix Exp $
 *
 * This file is part of Ample.
 *
 * Ample is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Ample is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ample; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <config.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <inttypes.h>

#ifdef HAVE_SYSLOG_H
#include <syslog.h>
#endif

#ifdef HAVE_LIBWRAP
#include <tcpd.h>
int allow_severity = LOG_INFO;
int deny_severity = LOG_WARNING;
#endif

#include "ample.h"
#include "client.h"
#include "entries.h"
#include "configuration.h"
#include "helper.h"


static volatile int num_clients = 0;
struct config conf;


static int 
openconn(struct sockaddr_in *address) 
{
	int i = 1;
	int sock;
	
	if ((sock = socket(PF_INET,SOCK_STREAM, 0)) < 0)
		die("socket");
	
	i = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));
	address->sin_family = AF_INET;
	address->sin_port = htons((uint16_t)conf.port);
	memset(&address->sin_addr, 0, sizeof(address->sin_addr));
	
	if(bind(sock, (struct sockaddr *)address, sizeof(struct sockaddr_in)))
		die("bind");
	
	if (listen(sock,5))
		die("listen");
	
	return sock;
}


void 
sigchild_handler(int signal) 
{
	int status;
	pid_t pid;

	num_clients--;
	pid = wait(&status);
	logmsg("child with pid %d exited, currently %d/%d client(s)", pid, num_clients, conf.max_clients);
}


void 
sighup_handler(int signal)
{
#ifndef HAVE_SYSLOG_H
	freopen("/dev/null", "r", stdin);
	freopen(conf.logfile, "a", stdout);
	freopen(conf.logfile, "a", stderr);
#endif
	return;
}


bool 
issocket(int fd)
{
	int v;
	socklen_t l;

	l = sizeof(int);
	return(getsockopt(fd, SOL_SOCKET, SO_TYPE, (void *)&v, &l) == 0);
}


void 
daemonize(int inetd) 
{
	int i;
	pid_t pid;
#ifndef HAVE_SYSLOG_H
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sighup_handler;
	sigaction(SIGHUP, &sa, NULL);
#else
	openlog("ample", LOG_CONS | LOG_PID, LOG_DAEMON);
	signal(SIGHUP, SIG_IGN);
#endif	

	umask(022);

	if(conf.trace || inetd)
		return;
	
	if((pid = fork()) > 0)
		exit(EXIT_SUCCESS);
	else if(pid < 0)
		die("fork");
	
	setsid();

	if((pid = fork()) > 0)
		exit(EXIT_SUCCESS);
	else if(pid < 0)
		die("fork");

#ifdef HAVE_SYSLOG_H
	fclose(stdin);
	fclose(stdout);
	fclose(stderr);

	for(i=0; i < 3; i++)
		close(i);
#else
	freopen("/dev/null", "r", stdin);
	freopen(conf.logfile, "a", stdout);
	freopen(conf.logfile, "a", stderr);
#endif	
}


bool 
libwrap_access(int fd)
{
#ifdef HAVE_LIBWRAP
	struct request_info request;

	request_init(&request,
		     RQ_DAEMON, "ample", 
		     RQ_FILE, fd, 
		     NULL);
	fromhost(&request);
	if (!hosts_access(&request)) {
		return FALSE;
	}
#endif
	return TRUE;
}


int 
main(int argc, char * argv[]) 
{
	struct sockaddr_in address;
	int conn,sock = 0;
	pid_t pid;
	socklen_t addrlen = (socklen_t)sizeof(struct sockaddr_in);
	char *cwd;
	struct sigaction sa;
	sigset_t chld;
	int inetd = issocket(0);

	/* Initial setup */
	checkopt(argc, argv);	
	daemonize(inetd);

	/* Make sure we got a valid MP3 dir */
	logmsg("Ample/%s started", AMPLE_VERSION);
	if(chdir(conf.path))
		die("incorrect root (%s) for mp3 files specified", conf.path);
	debug(1, "using %s as root for mp3 files", conf.path);
	GETCWD(cwd);

	/* Set up signal stuff */
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sigchild_handler;
	sigaction(SIGCHLD, &sa, NULL);
	sigemptyset(&chld);
	sigaddset(&chld, SIGCHLD);

	/* Open, and listen to, socket */
	if(!inetd) {
		debug(1, "opening socket, port %d",conf.port);
		sock = openconn(&address);
	}	
	
 restart:
	/* Accept connection and get name of peer */
	if(!inetd) {
		conn = accept(sock, (struct sockaddr *) &address, &addrlen);
		if(conn < 0 && errno == EINTR)
			goto restart;
		else if(conn < 0)
			die("accept");
	} else {
		conn = 0;
		getpeername(0, (struct sockaddr *) &address, &addrlen);
	}
	
	/* Connection is now set up, check if it's allowed */
	sigprocmask(SIG_BLOCK, &chld, NULL);
	num_clients++;
	if(!inetd && num_clients > conf.max_clients) {
		/* Too many clients, deny this one */
		logmsg("incoming connection from %s:%d denied, max client(s) exceeded", 
		    inet_ntoa(address.sin_addr), address.sin_port);
		num_clients--;
		sigprocmask(SIG_UNBLOCK, &chld, NULL);
		close(conn);
		goto restart;
	} else if(!libwrap_access(conn)) {
		logmsg("incoming connection from %s:%d denied by TCP wrappers", 
		    inet_ntoa(address.sin_addr), address.sin_port);
		num_clients--;
		sigprocmask(SIG_UNBLOCK, &chld, NULL);
		close(conn);
		goto restart;
	} else {
		logmsg("incoming connection from %s:%d accepted, currently %d/%d client(s)", 
		    inet_ntoa(address.sin_addr), address.sin_port, num_clients, conf.max_clients);
	}
	
	/* Connection is allowed, handle it */
	if(conf.trace || inetd) {
		debug(1, "no-fork/inetd mode: connection from %s:%d", inet_ntoa(address.sin_addr), address.sin_port);
		handleclient(conn);
		num_clients--;
		sigprocmask(SIG_UNBLOCK, &chld, NULL);
		/* We don't need to close(conn), handleclient does that, fall trough and exit */
	} else {
		if((pid = fork()) < 0) {
			die("fork");
		} else if(pid > 0) {
			debug(1, "connection from %s:%d handled by child with pid %d", 
			      inet_ntoa(address.sin_addr), address.sin_port, pid);
			sigprocmask(SIG_UNBLOCK, &chld, NULL);
			close(conn);
			goto restart;
		} else {
			close(sock);
			return(handleclient(conn));
		}
	}
	
	/* Time to say goodbye */
	if(!sock)
		close(sock);
	free(cwd);
	return(EXIT_SUCCESS);
}
