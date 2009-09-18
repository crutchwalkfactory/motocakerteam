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
#ifndef _misc_h
#define _misc_h
#ifdef __cplusplus
extern "C" {
#endif

char *xStrdup (const char *t);

typedef struct { unsigned long high, low; } TimeStampCounter;
void getTimeStampCounter ( TimeStampCounter *t );
void Start_Timer (unsigned int idx);
void Ende_Timer  (unsigned int idx);
void Print_Timer (unsigned int idx);
unsigned long long Get_Timestamp(void);
unsigned long long Guess_Mhz 	(void);
unsigned long long Get_Mhz 	(void);
#ifdef __cplusplus
	}
#endif

#endif
