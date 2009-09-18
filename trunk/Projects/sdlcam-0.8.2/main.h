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
#ifndef _main_h
#define _main_h
#ifdef __cplusplus
extern "C" {
#endif


extern unsigned long long cpu_hertz;
extern unsigned long long fps;
extern int sum_fps;
extern int cnt_fps;

void toggle_flip_x(void);
void toggle_flip_y(void);
void Exit 	   (void);
int Add_Input (const char *type, const char *device);

int main(int argc, char **argv);
#ifdef __cplusplus
	}
#endif


#endif
