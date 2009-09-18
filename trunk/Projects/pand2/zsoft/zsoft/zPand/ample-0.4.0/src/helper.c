/*
 * $Id: helper.c,v 1.15 2001/12/16 01:35:44 alphix Exp $
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
#include <libgen.h>
#include <string.h>
#ifdef HAVE_SYSLOG_H
#include <syslog.h>
#endif
#include <stdarg.h>
#include <errno.h>

#include "ample.h"
#include "entries.h"
#include "helper.h"


static void 
printlogmsg(int type, char *fmt, va_list argp)
{
	char buffer[1000];
	int length;
	
	length = vsnprintf(buffer, 1000, fmt, argp);
	sprintf(&buffer[992], "<TRUNC>");

#ifdef HAVE_SYSLOG_H
	if(!conf.trace) {
		if(type == TYPE_LOG) {
			syslog(LOG_INFO, "%s\n", buffer);
		} else if(type == TYPE_DEBUG) {
			syslog(LOG_DEBUG, "%s\n", buffer);
		} else if(type == TYPE_DIE) {
			if(errno != 0)
				syslog(LOG_ERR, "died - %s, errno: %s\n", buffer, strerror(errno));
			else
				syslog(LOG_ERR, "died - %s\n", buffer);
			exit(EXIT_FAILURE);
		}
	} else {
#else
	if(TRUE) {
#endif
		if(type == TYPE_LOG) {
			printf("LOG[%d]: %s\n", getpid(), buffer);
		} else if(type == TYPE_DEBUG) {
			printf("DBG[%d]: %s\n", getpid(), buffer);
		} else if(type == TYPE_DIE) {
			if(errno != 0)
				printf("DIE[%d]: %s, errno: %s\n", getpid(), buffer, strerror(errno));
			else
				printf("DIE[%d]: %s\n", getpid(), buffer);
			exit(EXIT_FAILURE);
		}
		fflush(stdout);
	}
}


void 
logmsg(char *fmt, ...)
{
	va_list argp;

	va_start(argp, fmt);
	printlogmsg(TYPE_LOG, fmt, argp);
	va_end(argp);
}


void 
debug(int priority, char *fmt, ...) 
{
	va_list argp;

	if(priority > conf.debuglevel)
		return;

	va_start(argp, fmt);
	printlogmsg(TYPE_DEBUG, fmt, argp);
	va_end(argp);
}


void 
die(char *fmt, ...) 
{
	va_list argp;
	
	va_start(argp, fmt);
	printlogmsg(TYPE_DIE, fmt, argp);
	va_end(argp);
}


bool 
writemetadata(FILE *to, mp3entry *entry, int *metaflag) 
{
	int msglen;
	int padding;
	int towrite;
	int written;
	char *buf;

	if(*metaflag) {
		*metaflag = FALSE;

		msglen = strlen(entry->title) + 28;
		padding = 16 - msglen % 16;
		towrite = msglen + padding + 1;
		
		buf = malloc(towrite);
		memset(buf, 0, towrite);
		sprintf(buf, "%cStreamTitle='%s';StreamUrl='';", (msglen + padding)/16, entry->title);
	} else {
		towrite = 1;
		buf = malloc(towrite);
		memset(buf, 0, towrite);
	}

	written = fwrite(buf, sizeof(char), towrite, to);
	free(buf);
	if(written == towrite)
		return(TRUE);
	else
		return(FALSE);
}

static void 
stripspaces(char *buffer) 
{
	int i = 0;
	
	while(*(buffer + i) != '\0')
		i++;
	
	while(i >= 0) {
		if(*(buffer + i) == ' ')
			*(buffer + i) = '\0';
		else if(*(buffer + i) == '\0')
			i--;
		else
			break;
	}
}

static bool 
setid3v1title(FILE *file, mp3entry *entry) 
{
	char buffer[31];
	int offsets[3] = {-95,-65,-125};
	int i;
	char *result;

	result = (char *)malloc(3*30 + 2*3 + 1);
	*result = '\0';
	
	for(i=0;i<3;i++) {
		if(fseek(file, offsets[i], SEEK_END) != 0) {
			free(result);
			return FALSE;
		}
		
		
		fgets(buffer, 31, file);
		stripspaces(buffer);
		if(buffer[0] != '\0' && *result != '\0')
			strcat(result, " - ");
		strcat(result, buffer);
	}

	if(*result == '\0') {
		free(result);
		return(FALSE);
	} else {
		entry->title = (char *)realloc(result, strlen(result) + 1);
		return(TRUE);
	}
}


static bool 
setid3v2title(FILE *file, mp3entry *entry) 
{
	char *buffer;
	int minframesize;
	int size, readsize = 0, headerlen;
	char *title = NULL;
	char *artist = NULL;
	char header[10];
	unsigned short int version;
	
	/* 10 = headerlength */
	if(entry->id3v2len < 10)
		return(FALSE);

	/* Check version */
	fseek(file, 0, SEEK_SET);
	fread(header, sizeof(char), 10, file);
	version = (unsigned short int)header[3];
	
	/* Read all frames in the tag */
	size = entry->id3v2len - 10;
	buffer = malloc(size + 1);
	if(size != (int)fread(buffer, sizeof(char), size, file)) {
		free(buffer);
		return(FALSE);
	}
	*(buffer + size) = '\0';

	/* Set minimun frame size according to ID3v2 version */
	if(version > 2)
		minframesize = 12;
	else
		minframesize = 8;

	/* We must have at least minframesize bytes left for the remaining frames to be interesting */
	while(size - readsize > minframesize) {
		
                /* Read frame header and check length */
		if(version > 2) {
			memcpy(header, (buffer + readsize), 10);
			readsize += 10;
			headerlen = UNSYNC(header[4], header[5], header[6], header[7]);
		} else {
			memcpy(header, (buffer + readsize), 6);			
			readsize += 6;
			headerlen = (header[3] << 16) + (header[4] << 8) + (header[5]);
		}
		if(headerlen < 1)
			continue;

		/* Check for certain frame headers */
		if(!strncmp(header, "TPE1", strlen("TPE1")) || !strncmp(header, "TP1", strlen("TP1"))) {
			readsize++;
			headerlen--;
			if(headerlen > (size - readsize))
				headerlen = (size - readsize);
			artist = malloc(headerlen + 1);
			snprintf(artist, headerlen + 1, "%s", (buffer + readsize));
			readsize += headerlen;
		} else if(!strncmp(header, "TIT2", strlen("TIT2")) || !strncmp(header, "TT2", strlen("TT2"))) {
			readsize++;
			headerlen--;
			if(headerlen > (size - readsize))
				headerlen = (size - readsize);
			title = malloc(headerlen + 1);
			snprintf(title, headerlen + 1, "%s", (buffer + readsize));
			readsize += headerlen;
		}
	}

	/* Done, let's clean up */
	if(artist && title) {
		entry->title = malloc(strlen(artist) + strlen(title) + 4);
		snprintf(entry->title, strlen(artist) + strlen(title) + 4, "%s - %s", artist, title);
		free(artist);
		free(title);
	} else if(artist) {
		entry->title = artist;
	} else if(title) {
		entry->title = title;
	}
	
	free(buffer);
	return(entry->title != NULL);
}

static int 
getid3v2len(FILE *file) 
{
	char buf[6];
	int offset;
	
	/* Make sure file has a ID3 tag */
	if((fseek(file, 0, SEEK_SET) != 0) ||
	   (fread(buf, sizeof(char), 6, file) != 6) ||
	   (strncmp(buf, "ID3", strlen("ID3")) != 0))
		offset = 0;
	/* Now check what the ID3v2 size field says */
	else if(fread(buf, sizeof(char), 4, file) != 4)
		offset = 0;
	else
		offset = UNSYNC(buf[0], buf[1], buf[2], buf[3]) + 10;
	
	return(offset);
}


static int 
getid3v1len(FILE *file) 
{
	char buf[3];
	int offset;

	/* Check if we find "TAG" 128 bytes from EOF */
	if((fseek(file, -128, SEEK_END) != 0) ||
	   (fread(buf, sizeof(char), 3, file) != 3) ||
	   (strncmp(buf, "TAG", 3) != 0))
		offset = 0;
	else
		offset = 128;
	
	return offset;
}

void 
checkid3tags(mp3entry *entry) 
{
	FILE *file;
	char *copy;
	char *title;

	if((file = fopen(entry->path, "r")) == NULL)
		return;

	entry->id3v2len = getid3v2len(file);
	entry->id3v1len = getid3v1len(file);
	entry->title = NULL;

	if(HASID3V2(entry))
		setid3v2title(file, entry);

	if(HASID3V1(entry) && !entry->title)
		setid3v1title(file, entry);
	
	if(!entry->title) {
		copy = strdup(entry->path);
		title = basename(copy);
		entry->title = malloc(strlen(title) - 3);
		snprintf(entry->title, strlen(title) - 3, "%s", title);
		free(copy);
	}

	fclose(file);
}
