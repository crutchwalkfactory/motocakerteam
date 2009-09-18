/*
 * $Id: client.c,v 1.9 2001/12/11 16:28:25 alphix Exp $
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
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <sys/types.h>
#include <stdlib.h>

#include "ample.h"
#include "client.h"
#include "entries.h"
#include "helper.h"
#include "configuration.h"

static int bytestometa = BYTESBETWEENMETA;

static int 
copyamount(int readlimit) 
{

	if((MODE_ISSET(MODE_METADATA) && (bytestometa < NETBUFFSIZE)) && (readlimit < NETBUFFSIZE))
		return (readlimit > bytestometa ? bytestometa : readlimit);
	else if(MODE_ISSET(MODE_METADATA) && (bytestometa < NETBUFFSIZE))
		return bytestometa;
	else if(readlimit < NETBUFFSIZE)
		return readlimit;
	else
		return NETBUFFSIZE;
}


static int 
preparedata(char *buf, long end, FILE *from) 
{
	long current = ftell(from);
	int diff = (int)(end - current);
	int amount;
	
	if(current < 0)
		die("ftell");
	
	debug(4, "amount prepared to be sent: %d", copyamount(diff));
	amount = fread(buf, sizeof(char), copyamount(diff), from);

	if(ferror(from))
		die("read");

	if(feof(from) || current == end)
		return 0;
	
	return amount;
}


static void 
senddata(FILE *to, char *buf, int amount, int *metaflag) 
{
	if(fwrite(buf,sizeof(char),amount,to) != amount)
		die("error writing to client");
	
	if(MODE_ISSET(MODE_METADATA)) {
		bytestometa = bytestometa - amount;
		if(bytestometa == 0) {
			if(writemetadata(to,root,metaflag))
				bytestometa = BYTESBETWEENMETA;
			else
				die("error writing to client");
		}
	}
}


static void 
setoffsets(FILE *stream, long *end) 
{
	if(HASID3V1(root) && *end > root->id3v1len)
		*end -= root->id3v1len;
	else if(HASID3V1(root))
		die("file length < ID3v1 length");
	if(fseek(stream, root->id3v2len, SEEK_SET) != 0)
		die("setstartpos");
}

static void 
playfile(FILE *conn, mp3entry *entry) 
{
	FILE *file;
	char buf[NETBUFFSIZE];
	int amount;
	int metaflag = TRUE;
	long end;

	logmsg("playing file %s", entry->path);
	debug(1, "file info - ID3v1: %d, ID3v2: %d, title: %s", HASID3V1(entry), HASID3V2(entry), entry->title);

	if ((file = fopen(entry->path,"r")) == NULL)
		die("fopen failed");
	
	end = (long)entry->filesize;
	if(!MODE_ISSET(MODE_SINGLE))
		setoffsets(file, &end);

	while((amount = preparedata(buf, end, file)))
		senddata(conn, buf, amount, &metaflag);
	
	fclose(file);
}


static void 
playlist(FILE *conn) 
{
	while(root->next != NULL) {
		playfile(conn, root);
		root = root->next;
	}
}


int 
handleclient(int conn) 
{
	char *basedir;
	char *clientdir;
	mp3entry *mp3buf;
	FILE *stream;

	if ((stream = fdopen(conn, "r+")) == NULL || !readrequest(stream)) {
		debug(1, "error while reading client request");
		fclose(stream);
		return(EXIT_FAILURE);
	}
	
	debug(1, "requested path %s", conf.path);

	GETCWD(basedir);
	if(chdir(conf.path))
		die("incorrect dir");
	GETCWD(clientdir);
	
	debug(1, "doing security checks");
	if(strncmp(basedir, clientdir, strlen(basedir) - 1)) {
		fclose(stream);
		debug(1, "basedir %s, clientdir %s", basedir, clientdir);
		die("client tried to break out of base directory");
	}
	
	debug(1, "looking for mp3 files in subdirectory %s", conf.path);
	if(!MODE_ISSET(MODE_SINGLE)) {
		getfiles(".");
	} else if(validfile(conf.filename, clientdir, &mp3buf)) { 
		addentry(&root, mp3buf);
	}
	
	if(!root)
		die("no files");
	else
		debug(1, "%d MP3 file(s) found", countentries(root));

	if(!MODE_ISSET(MODE_INDEX) && !conf.order) {
		debug(1, "shuffling mp3 files");
		shuffleentries();
	}

	if(conf.debuglevel > 2) {
		debug(1, "listing mp3 files");
		dumpentries();
	}
	
	if(MODE_ISSET(MODE_INDEX)) {
		debug(1, "entering HTTP mode");
		fprintf(stream, HTTPSERVMSG, AMPLE_VERSION);
		fflush(stream);
		createhtml(stream,(clientdir + strlen(basedir)),(strlen(clientdir) + 1));
	} else if(MODE_ISSET(MODE_SINGLE)) {
		debug(1, "entering MP3-Single mode");
		fprintf(stream, SINGLESERVMSG, AMPLE_VERSION, root->filesize);
		fflush(stream);
		playfile(stream, root);
	} else if (MODE_ISSET(MODE_METADATA)) {
		debug(1, "entering MP3-Metadata mode");
		fprintf(stream, SHOUTSERVMSG, AMPLE_VERSION, conf.servername, BYTESBETWEENMETA);
		fflush(stream);
		playlist(stream);
	} else {
		debug(1, "entering MP3-Basic mode");
		fprintf(stream, BASICSERVMSG, AMPLE_VERSION);
		fflush(stream);
		playlist(stream);
	}

	fclose(stream);
	clearlist(root);
	return(EXIT_SUCCESS);
}

