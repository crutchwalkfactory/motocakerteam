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
#ifndef FTP_UPLOAD_H
#define FTP_UPLOAD_H
#ifdef __cplusplus
extern "C" {
#endif

int init_ftp (const char *host,const char *user,const char *pass);
int ftp_send (const char *quelle, const char*ziel);
int ftp_rename (const char *quelle, const char*ziel);
int ftp_quit (void);
#ifdef __cplusplus
	}
#endif


#endif