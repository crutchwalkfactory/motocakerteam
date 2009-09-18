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
#ifndef _webcam_h
#define _webcam_h

class cWebcam:public cV4L {
	public:
			 cWebcam(const char *name,int mmap = 1);
	virtual 	~cWebcam(void) {};
        virtual int       Parameter  (const char *name, int val);  // Set an free parameter
        virtual int       Parameter  (const char *name)         ;  // Get an free parameter
	};

#ifdef __cplusplus
extern "C" {
#endif

/* Initialization*/
/* Set the compression preference. 0 = Not compressed, 3 = Maximum compression */
/* Reset all internal webcam settings to factory default */

/* Write current settings in the EEPROM */
/* Restore settings from the EEPROM */

/* The on and off time in milliseconds */
// int set_webcam_led_cycle(int ms_on, int ms_off);

/* Between 0 and 65535. Any negative number will enable automatic shutter speed */
/* Between 0 and 65535. give a negative number to enable agc. */
/* 0: Auto  1: Manual  2: Indoor  3: Outdoor  4: Fluorescent */
/* Any rate between 5 and 30 */

#ifdef __cplusplus
	}
#endif


#endif
