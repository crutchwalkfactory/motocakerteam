/*
 * $Id: entries.c,v 1.21 2001/12/11 16:28:25 alphix Exp $
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
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#if HAVE_LIMITS_H
#include <limits.h>
#endif

#include "ample.h"
#include "entries.h"
#include "helper.h"

#define INITIAL_FILES 50
#define INITIAL_DIRS 10

mp3entry * root = NULL;


void 
addentry(mp3entry ** rootentry, mp3entry * toadd) 
{
	mp3entry * tmp = NULL;

	if(*rootentry == NULL) {
		toadd->next = toadd;
		*rootentry = toadd;
	} else {
		tmp = *rootentry;
		while(tmp->next != *rootentry)
			tmp = tmp->next;
		tmp->next = toadd;
		toadd->next = *rootentry;
	}
}


void 
removeentry(mp3entry **rootentry, mp3entry *toremove) 
{
	mp3entry *tmp = *rootentry;

	if(*rootentry == NULL)
		return;

	if(toremove->next == toremove) {
		if(*rootentry == toremove)
			*rootentry = NULL;
		else
			die("inconsistent linked list");

	} else {
		while(tmp->next != toremove)
			tmp = tmp->next;
		tmp->next = toremove->next;
		if(*rootentry == toremove)
			*rootentry = toremove->next;
	}
}


void 
clearlist(mp3entry *rootentry) 
{
	mp3entry *tmp;

	while(rootentry != NULL) {
		tmp = rootentry;
		removeentry(&rootentry, rootentry);
		free(tmp->path);
		free(tmp);
	}
}

	
int 
countentries(mp3entry *rootentry) 
{
	mp3entry *tmp = rootentry;
	int i = 0;

	if(tmp != NULL)
		do {
			i++;
			tmp = tmp->next;
		} while(tmp != rootentry);
	
	return i;
}


void 
shuffleentries(void) 
{
	int random;
	int i = countentries(root);
	mp3entry * oldroot = root;
	mp3entry * tmp;
	root = NULL;

	srand(time(NULL));
	while(i) {
		random = 1 + (int)((float)i*rand()/(RAND_MAX+1.0));
		tmp = oldroot;
		i--;

		while(random > 1) {
			tmp = tmp->next;
			random--;
		}

		removeentry(&oldroot, tmp);
		addentry(&root, tmp);
	}	
}


void 
dumpentries() 
{
	int i = 0;
	mp3entry * tmp = root;
	
	if(tmp == NULL)
		return;
	
	do {
		i++;
		debug(1, "%d: %s - ID3V1: %d - ID3V2: %d - TITLE: %s", i, tmp->path, HASID3V1(tmp), HASID3V2(tmp), tmp->title);
		tmp = tmp->next;
	} while (tmp != root);
}


void 
createhtml(FILE *st, char *dirname, int baselen) 
{
	int i = 0;
	mp3entry * tmp = root;
	
	if(tmp == NULL || st == NULL)
		return;
	
	fprintf(st, "<html><head><title>%s</title></head>\n", conf.servername);
	fprintf(st, "<body bgcolor=\"#b0c0ff\">\n");
	fprintf(st, "<center><p><h1>Ample</h1></p>\n");
	fprintf(st, "<p><h2>Tracks currently available in /%s</h2></p>\n", dirname);
	fprintf(st, "<table border=\"1\">\n");

	do {
		i++;
		fprintf(st, "<tr><td>%d</td><td><a href=\"%s\">%s</a></td></tr>\n",i,(tmp->path + baselen),(tmp->path + baselen));
		tmp = tmp->next;
	} while (tmp != root);
	fprintf(st, "</table></center>\n");
	fprintf(st, "<p align=\"right\"><font size=\"-1\"><em>For more information on Ample, see the ");
	fprintf(st, "<a href=\"http://ample.sourceforge.net\">project homepage</a></em></font></p>\n</body></html>\r\n");

}


static int 
dirsort(const void *arg1, const void *arg2) 
{
        return strcmp(*(char **)arg1, *(char **)arg2);
}


static int 
filesort(const void *arg1, const void *arg2) 
{
        return strcmp((*((mp3entry **)arg1))->path, (*((mp3entry **)arg2))->path);
}


static void 
resizearray(void ***array, int size, int *nmemb, int *maxsize) 
{
	if(*nmemb + 1 == *maxsize) {
		*maxsize = *maxsize * 2;
		*array = realloc((*array), *maxsize * size);
		if(*array == NULL)
			die("realloc");
	}
}


int 
validfile(char *filename, char *cwd, mp3entry **buf)
{
	struct stat statbuf;
	
	/* File/dir shouldn't begin with . */
	if(*(filename) == '.') {
		debug(2, "ignoring %s, starts with .",filename);
	}
	/* lstat must work */
	else if(lstat(filename,&statbuf)) {
		debug(2, "ignoring %s, lstat doesn't work",filename);
	}
	/* Don't follow symlinks */
	else if (S_ISLNK(statbuf.st_mode)) {
		debug(2, "ignoring %s, it's a symlink",filename);			
	}
	/* File must be a regular file */
	else if(!S_ISREG(statbuf.st_mode)) {
		debug(2, "ignoring %s, not a regular file",filename);
	}
	/* Filename must be at least as long as x.mp3 */
	else if(strlen(filename) < 5) {
		debug(2, "ignoring %s, too short name",filename);
	}
	/* Filename must end with .mp3 (case insensitive) */
	else if(strcasecmp((filename + strlen(filename) - 4),".mp3")) {
		debug(2, "ignoring %s, doesn't end with \".mp3\"",filename);
	}
	/* File size must be > 0 */
	else if(statbuf.st_size == 0) {
		debug(2, "ignoring %s, zero size",filename);
	} else {
		/* OK, it's valid */
		debug(2, "valid file: %s/%s",cwd,filename);
		*buf = (mp3entry *)malloc(sizeof(mp3entry));
		(*buf)->path = (char *)malloc(strlen(cwd) + strlen(filename) + 2);
		sprintf((*buf)->path, "%s/%s", cwd, filename);
		(*buf)->filesize = statbuf.st_size;
		checkid3tags(*buf);
		return TRUE;
	}

	return FALSE;
}


int 
validdir(char *dirname, char *cwd, char **buf)
{
	struct stat statbuf;

	/* File/dir shouldn't begin with . */
	if(*dirname == '.') {
		debug(2, "ignoring %s, starts with .", dirname);
	}
	/* lstat must work */
	else if(lstat(dirname, &statbuf)) {
		debug(2, "ignoring %s, lstat doesn't work", dirname);
	}
	/* Don't follow symlinks */
	else if (S_ISLNK(statbuf.st_mode)) {
		debug(2, "ignoring %s, it's a symlink", dirname);			
	}
	/* Is it a directory? If so add it to the dir array */
	else if(S_ISDIR(statbuf.st_mode)) {
		debug(2, "valid dir: %s/%s", cwd, dirname);
		*buf = (char *)malloc(strlen(cwd) + strlen(dirname) + 2);
		sprintf(*buf, "%s/%s", cwd, dirname);
		
		return TRUE;
	}

	return FALSE;
}


void 
getfiles(char *path) 
{
	DIR *dir;
	struct dirent *dirent;
	char *filename;
	char *cwd;

	mp3entry **filearray;
	mp3entry *filebuf;
	int filemax = INITIAL_FILES;
	int filec = 0;

	char **dirarray;
	char *dirbuf;
	int dirmax = INITIAL_DIRS;
	int dirc = 0;

	chdir(path);
	GETCWD(cwd);
	if (!(dir = opendir(".")))
		die("opendir");
	
	if (!(filearray = (mp3entry **) malloc (filemax * sizeof(mp3entry *))))
		die("malloc");
	
	if (!(dirarray = (char **) malloc (dirmax * sizeof(char *))))
		die("malloc");
	
	while((dirent = readdir(dir))) {
		filename = dirent->d_name;

		if(validfile(filename, cwd, &filebuf)) { 
			resizearray((void ***)&filearray, sizeof(mp3entry *), &filec, &filemax);
			filearray[filec] = filebuf;
			filec++;
		} else if(conf.recursive && validdir(filename, cwd, &dirbuf)) {
			resizearray((void ***)&dirarray, sizeof(char *), &dirc, &dirmax);
			dirarray[dirc] = dirbuf;
			dirc++;
		} else {
			debug(2, "\"%s\" is not a valid file or directory", filename);
		}
		
		errno = 0;
	}
	
	if (errno)
		die("readdir");

	resizearray((void ***)&filearray, sizeof(mp3entry *), &filec, &filemax);
	filearray[filec] = NULL;
	resizearray((void ***)&dirarray, sizeof(char *), &dirc, &dirmax);
	dirarray[dirc] = NULL;
	
	if(conf.order) {
		debug(1, "sorting mp3 files");
		qsort(filearray, filec, sizeof(mp3entry *), filesort);

		debug(1, "sorting dirs");
		qsort(dirarray, dirc, sizeof(char *), dirsort);
	}

	filec = 0;
	while(filearray[filec] != NULL) {
		addentry(&root, filearray[filec]);
		filec++;
	}

	dirc = 0;
	while(dirarray[dirc] != NULL) {
		debug(1, "-> directory %s, processing", dirarray[dirc]);
		getfiles(dirarray[dirc]);
		if(chdir(cwd))
			die("chdir");
		debug(1, "<- directory %s, done", dirarray[dirc]);
		free(dirarray[dirc]);
		dirc++;
	}

	free(filearray);
	free(dirarray);
	free(cwd);
	closedir(dir);
}
