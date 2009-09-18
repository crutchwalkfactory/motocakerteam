/*
	SDLcam:  A program to view and apply effects in real-time to video
	Copyright (C) 2002, Raphael Assenat
	 
	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.
					 
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
								 
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

#include "ftp-upload.h"

static int ftp_write=-1;
static int ftp_read=-1 ;
static int ftp_pid=-1;


/**
  * init_ftp
  *
  * initialize an ftp connection via command line ftp and pipe
  * succesive calls after the first return 1
  * set's passive mode and binary so that theres no problem with fw
  *
  * Input:
  * const char*		Hostname where to connect
  * const char*		Username to login in with
  * const char*		Passwort to login with
  *
  * Return:
  * int			0=Failled to initialise 1=Success
  *
  * Todo:
  * check the response messages
  */
int init_ftp (const char *host,const char *user,const char *pass)
{
  int pipe_write[2];	// 0 Read 1 write
  int pipe_read[2];
  char buffer[512];
  unsigned int len;
  buffer[511] = 0;
  if (ftp_write != -1 && ftp_read != -1)
    return (1);
  if (pipe (pipe_write))
    return (0);
  if (pipe (pipe_read))
    {
      close (pipe_write[0]);
      close (pipe_write[1]);
     return (0);
    }
  
  switch ( (ftp_pid = fork ()))
    {
      case -1:
	close (pipe_write[0]);
	close (pipe_write[1]);
	close (pipe_read [0]);
	close (pipe_read [1]);
	return (0);
      case  0:
	dup2 (pipe_write[0], 0);
	dup2 (pipe_read [1], 1);
	dup2 (pipe_read [1], 2);
	execl ("/usr/bin/ftp","ftp","-n",NULL);
	printf ("Fehler: execlp () = %s\n", strerror (errno));
	ftp_write = -1;
	ftp_read  = -1;
	ftp_pid = -1;
	return (0);
      default:
	ftp_write = pipe_write[1];
	ftp_read  = pipe_read [0];
	len = snprintf (buffer, 511, "open %s\n", host);
	write (ftp_write, buffer, len);
	len = snprintf (buffer, 511, "user %s %s\n", user, pass);
	write (ftp_write, buffer, len);
	write (ftp_write, "passive\n", strlen("passive\n"));
	write (ftp_write, "binary\n" , strlen("binary\n" ));
	return (1);
    }
}

/**
  * ftp_send
  *
  * Upload an file via established ftp connection
  * if connection do not exist return 0
  *
  * Input:
  * const char*		local Filename
  * const char*		remote Filename
  *
  * Output:
  * int			0 failed 1 success
  *
  * Todo:
  * error code checking
  */
int ftp_send (const char *quelle, const char*ziel)
{
  char buffer[512];
  unsigned int len;
  if (ftp_write == -1 || ftp_read == -1)
    return (0);
  len=snprintf(buffer,511,"put %s %s\n", quelle, ziel);
  write (ftp_write, buffer, len);
  return (1);
}

/**
  * ftp_rename
  *
  * rename an remote file
  *
  * Input:
  * const char*		Old Filename
  * const char*		New filename
  *
  * Outout:
  * int			0 failed 1 success
  *
  * Todo:
  * error code checking
  */
int ftp_rename (const char *quelle, const char*ziel)
{
  char buffer[512];
  unsigned int len;
  if (ftp_write == -1 || ftp_read == -1)
    return (0);
  len=snprintf(buffer,511,"rename %s %s\n", quelle, ziel);
  write (ftp_write, buffer, len);
  return (1);
}


/**
  * ftp_quit
  *
  * disconnect an ftp connection
  *
  * Outout:
  * int			0 failed 1 success
  */
int ftp_quit (void)
{
  int status;
  if (ftp_write == -1 || ftp_read == -1)
    return (0);
  write (ftp_write, "quit\n\r",strlen("quit\n"));
  if (ftp_pid > 0)
    waitpid (ftp_pid, &status, 0);
  ftp_write = -1;
  ftp_read = -1;
  ftp_pid = -1;
  return (1);
}

/*
int main (int argc, char **argv)
{
  int status;
  if (init_ftp ("deliver.bewegungsmelder.de", "tlussnig", "as34rt"))
    {
      ftp_send   ("skate-scout.jpg", "tmp.jpg");
      ftp_renmae ("tmp.jpg", "skate-scout.jpg");
      ftp_quit ();
    }
}
*/
