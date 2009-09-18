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
#ifndef _config_file_h
#define _config_file_h
#ifdef __cplusplus
extern "C" {
#endif


int find_and_parse_config_file (void);
char *xStrdup  (const char *t);
void *xMalloc  (        size_t s);
void *xRealloc (void *p,size_t s);
int Parse_Assignment (char *line, char **key, char **val);
#ifdef __cplusplus
	}
#endif

#endif
