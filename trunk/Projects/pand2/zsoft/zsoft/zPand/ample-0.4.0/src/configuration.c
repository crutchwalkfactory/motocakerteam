/*
 * $Id: configuration.c,v 1.10 2001/12/11 16:28:25 alphix Exp $
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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <libgen.h>
#include <ctype.h>
#if HAVE_GETOPT_H
#include <getopt.h>
#endif

#include "entries.h"
#include "ample.h"
#include "configuration.h"
#include "helper.h"


/* This function will deal with leading / and also
 * illegal characters according to RFC 1738:
 * ' ',',','<','>','"','#','%','{','}','|','\','^','~','[',']' and '`'
 * These should be (but aren't always) be encoded as %<two character hex code>
 * <hi> would become %3Chi%3E
 */
char *
decodeurl(char *url) 
{
	char *current = url;
	char *ret = (char *)malloc(strlen(url) + 3);
	char *pos = ret;
	int hexval;
	char hexbuf[3];

	hexbuf[2] = '\0';
	if(*current == '/') {
		sprintf(pos, "./");
		current++;
		pos = pos + 2;
	}

	while(*current != '\0') {
		if(*current != '%') {
			*pos = *current;
			pos++;
			current++;
		} else {
			if(((hexbuf[0] = *(current + 1)) == '\0') ||
			   ((hexbuf[1] = *(current + 2)) == '\0')) {
				strncpy(pos, current, 3);
				pos = pos + 2;
				break;
			} else if ((hexval = strtol(hexbuf, NULL, 16)) == 0) {
				strncpy(pos, current, 3);
				pos = pos + 3;
			} else {
				*pos = (char)hexval;
				pos++;
			}
			current = current + 3;
		}
	}
	
	*pos = '\0';
	debug(1, "decodeurl parses %s as %s",url,ret);
	return ret;
}

volatile int timeout = FALSE;

void 
conntimeout(int signal) 
{
	timeout = TRUE;
}

bool 
readrequest(FILE *stream)
{
	char line[LINELENGTH];
	int start,end;
	char *tmp;
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = conntimeout;
	sigaction(SIGALRM, &sa, NULL);
	alarm(CONN_TIMEOUT);

	while((fgets(line, LINELENGTH, stream)) != NULL && timeout != TRUE) {
		
		if(!strcmp(line, "\r\n")) {
			/* End of request from client */
			break;

		} else if(!strncasecmp(line, "GET", strlen("GET"))) {
			/* Found the GET request */
			debug(3, "GET request was %s", line);
			if(strlen(line) < 14)
				continue;
			
			/* If no spaces or just one space is found, continue */
			if(strchr(line, ' ') == NULL || strchr(line, ' ') == strrchr(line, ' '))
				continue;
			
			/* Set start and end indexes so that the URL is enclosed between them */
			for(start = strlen("GET"); isspace(line[start]); start++);
			for(end = (strrchr(line, ' ') - line); !isspace(line[end]); end--);
			line[end] = '\0';
			
			/* Decode the URL and set it in the config options */
			conf.path = decodeurl(&line[start]);
			
			/* Was index.html requested? */
			if((tmp = strrchr(conf.path, 'i')) != NULL && 
			   (!strcasecmp(tmp, "index.html") || !strcasecmp(tmp, "index.htm"))) {
				MODE_SET(MODE_INDEX);
				*tmp = '\0';
			}
			
			/* Was a single MP3 file requested? */
			if((tmp = strrchr(conf.path, '.')) == NULL)
				continue;
			if(!strcasecmp(tmp, ".mp3")) {
				MODE_SET(MODE_SINGLE);
				conf.filename = basename(strdup(conf.path));
				conf.path = dirname(conf.path);
			}
			
		} else if(!strncasecmp(line, "Icy-MetaData:", strlen("Icy-MetaData:"))) {
			/* Metadata was requested */
			start = strlen("Icy-MetaData:");
			if(line[start] == '\0' || !(strtol(&line[start], NULL, 10) > 0)) {
				debug(2, "metadata was requested, but request wasn't valid");
			} else if(MODE_ISSET(MODE_SINGLE)) {
				debug(2, "metadata was requested, but ignored since we are in single mode");
			} else {
				MODE_SET(MODE_METADATA);
				debug(2, "metadata was requested");
			}

		} else {
			debug(3, "client sent line %s", line);
		}
	}
	
	sa.sa_handler = SIG_IGN;
	sigaction(SIGALRM, &sa, NULL);

	if(timeout) {
		logmsg("connection timed out");
		return(FALSE);
	} else {
		alarm(0);
		return(TRUE);
	}
}

static void 
usage(bool status)
{
	if(status) {
		fprintf(stderr, "Try `%s --help' for more information.", conf.program_name);
		exit(1);
	} else {
		/* This is several printf's to please ISO C89 */
		printf(USAGE1, conf.program_name);
		printf(USAGE2, DEF_PORT, DEF_MAX_CLIENTS);
		exit(0);
	}
}

static void 
setdefopt(int argc, char * argv[])
{
	if(conf.port == 0)
		conf.port = DEF_PORT;
	if(conf.order == -1)
		conf.order = FALSE;
	if(conf.max_clients == 0)
		conf.max_clients = DEF_MAX_CLIENTS;
	if(conf.recursive == -1)
		conf.recursive = TRUE;
	if(conf.path == NULL)
		conf.path = strdup(DEF_PATH);
	if(conf.logfile == NULL)
		conf.logfile = strdup(DEF_LOGFILE);
	if(conf.servername == NULL)
		conf.servername = strdup(DEF_SERVERNAME);
}

static void 
getconfopt(char *tmp, void *value, int type)
{
	int i;
	char *end;
	
	while(isspace(*tmp))
		tmp++;
	
	if(*tmp != '=')
		return;
	else
		tmp++;
	
	while(isspace(*tmp))
		tmp++;

	switch(type) {
	case OPT_INT:
		if((i = (int)strtol(tmp, NULL, 10)) > 0)
			*((int *)value) = i;
		break;
	case OPT_BOOL:
		if(!strncasecmp("yes",tmp,strlen("yes")) || !strncasecmp("true",tmp,strlen("true")))
			*((bool *)value) = TRUE;
		else if(!strncasecmp("no",tmp,strlen("no")) || !strncasecmp("false",tmp,strlen("false")))
			*((bool *)value) = FALSE;
		break;
	case OPT_STRING:
		end = tmp;
		while(*end != '\n' && *end != '\0' && *end != '#' && !isspace(*end))
			end++;
		if(end == tmp)
			break;
		*((char **)value) = malloc(end - tmp + 1);
		if(*((char **)value) == NULL)
			die("malloc");
		snprintf(*((char **)value), (end - tmp + 1), "%.*s", (end - tmp), tmp);
		break;
	default:
		debug(2, "incorrect type passed to getconfopt()");
		break;
	}
}

static void 
setconfopt(argc, argv) {

	char line[LINELENGTH];
	FILE *file;
	int i,j;
	const struct confoption options[] = {
		{"port", OPT_INT, &conf.port},
		{"order", OPT_BOOL, &conf.order},
		{"clients", OPT_INT, &conf.max_clients},
		{"recursive", OPT_BOOL, &conf.recursive},
		{"logfile", OPT_STRING, &conf.logfile},
		{"mp3path", OPT_STRING, &conf.path},
		{"servername", OPT_STRING, &conf.servername},
		{NULL, 0, NULL}
	};
	
	if(conf.conffile == NULL) {
		file = fopen(DEF_CONFFILE, "r");
		debug(2, "using %s as configfile", DEF_CONFFILE);
	} else {
		file = fopen(conf.conffile, "r");
		debug(2, "using %s as configfile", DEF_CONFFILE);
	}
	
	if (file == NULL) {
		debug(1, "opening config file failed, reason %s", strerror(errno));
		return;
	}

	while((fgets(line, LINELENGTH, file)) != NULL) {
		debug(9, "config line is %s", line);
		if(line[0] == '#')
			continue;
		
		for(i = 0; isspace(line[i]); i++);
		
		for(j = 0; options[j].name != NULL; j++) {
			if(!strncasecmp(&line[i], options[j].name, strlen(options[j].name))) {
				i += strlen(options[j].name);
				getconfopt(&line[i], options[j].value, options[j].type);
				break;
			}
		}
	}
}

static void 
setcmdopt(int argc, char * argv[])
{
	int c,i;

#ifdef HAVE_GETOPT_H
	const struct option longopts[] =
		{
			{"port", required_argument, NULL, 'p'},
			{"order", no_argument, NULL, 'o'},
			{"clients", required_argument, NULL, 'c'},
			{"norecursive", no_argument, NULL, 'n'},
			{"conffile", required_argument, NULL, 'f'},
			{"help", no_argument, NULL, 'h'},
			{"debug", optional_argument, NULL, 'd'},
			{"trace", no_argument, NULL, 't'},
			{"version", no_argument, NULL, 'v'},
			{NULL, 0, NULL, 0}
		};

	while((c = getopt_long(argc, argv, "p:oc:nf:hd::tv", longopts, &i)) != -1) {
#else
	while((c = getopt(argc, argv, "p:oc:nf:hd::tv")) != -1) {
#endif
		switch(c) {
		case 'p':
			if(strtol(optarg, NULL, 10) > 0)
				conf.port = (int)strtol(optarg, NULL, 10);
			break;
		case 'o':
			conf.order = TRUE;
			break;
		case 'c':
			if(strtol(optarg, NULL, 10) > 0)
				conf.max_clients = strtol(optarg, NULL, 10);
			break;
		case 'n':
			conf.recursive = FALSE;
			break;
		case 'f':
			conf.conffile = strdup(optarg);
			break;
		case 'h':
			usage(FALSE);
			break;
		case 'd':
			if(optarg == NULL || strtol(optarg, NULL, 10) < 0)
				conf.debuglevel = 1;
			else
				conf.debuglevel = strtol(optarg, NULL, 10);
			break;
		case 't':
			conf.trace = TRUE;
			break;
		case 'v':
			printf("Ample version %s\n",AMPLE_VERSION);
			exit(0);
		default:
			usage(TRUE);
		}
	}
	
	if(optind < argc)
		conf.path = strdup(argv[optind]);
}
	
void 
checkopt(int argc, char * argv[])
{
	memset(&conf, 0, sizeof(struct config));
	conf.order = -1;
	conf.recursive = -1;
	conf.program_name = argv[0];

	setcmdopt(argc, argv);
	setconfopt(argc, argv);
	setdefopt(argc, argv);
}
