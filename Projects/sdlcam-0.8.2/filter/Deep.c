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

#include "../SDL_v4l_image.h"
#include "../dyn_misc.h"
#include <stdlib.h>
#define	MMX_val4(CMD)	CMD" %%mm4, %%mm0\n"\
		      	CMD" %%mm4, %%mm1\n"\
			CMD" %%mm4, %%mm2\n"\
			CMD" %%mm4, %%mm3\n"

int MMX_bv_and      (const SDL_v4l_image *SRC, SDL_v4l_image *DST, unsigned int val) {{{
	unsigned int len = SRC->len;
	unsigned char *src = SRC->data;
	unsigned char *dst;
	SDL_v4l_Buffer_Prepare (SRC, DST);
	dst = DST->data;
#ifdef MMX
	if (len < 64) return 0;              // image size must be at least 8 bytes 
	/* Nicht auf 8 Register setzen ist mit 4 schneller !!!
	   Mit 2 ist es auch wieder langsamer
	   */
	asm volatile (
	"	push		%%ecx		\n"
	"	push		%%esi		\n"
	"	push		%%edi		\n"
	"	mov		%3,%%ecx        \n"
	"	movd		%%ecx,%%mm0	\n"
	"	movd		%%ecx,%%mm4     \n"
	"	punpckldq	%%mm0,%%mm4	\n"
	"	mov         %2, %%ecx           \n"
	"	mov	     	%1, %%edi	\n"
	"	mov	     	%0, %%esi       \n"
	"	shr         $5, %%ecx		\n"
	"	pcmpeqb   	%%mm0  , %%mm0  \n"
	".align 16             			\n"
/* {{{ working loop */
	".andi_MMX:				\n"
	"	movq		  (%%esi), %%mm0	\n"
	"	movq		 8(%%esi), %%mm1	\n"
	"	movq		16(%%esi), %%mm2	\n"
	"	movq		24(%%esi), %%mm3	\n"
		MMX_val4("pand")
	"	movq    	%%mm0 ,   (%%edi)	\n"
	"	movq    	%%mm1 ,  8(%%edi)	\n"
	"	movq    	%%mm2 , 16(%%edi)	\n"
	"	movq    	%%mm3 , 24(%%edi)	\n"
	"	add          $32     , %%esi		\n"
	"	add          $32     , %%edi		\n"
	"	dec         %%ecx		\n"
	"	jnz             .andi_MMX 	\n"
/* }}} */
	"	emms                  		\n"
	"	pop		%%edi		\n"
	"	pop		%%esi		\n"
	"	pop		%%ecx		\n"
		: "=m"  (src)           // %0
		:"m"    (dst),          // %1
	     "m"	(len),			// %2
		 "m"	(val)			// %3
		);
#else
	len>>=2;
	while (len--) *(dst++) = *(src++) & val;
#endif
	return 1;
	}}}

static int   status = 0;

       int   Type   	(void ) { return 0; }				// MANDATORY
int   Toggle 	(void ) { return status = (status + 1) % 8; }	// RECOMENDED ( else always  ON    )

int   Mode_set  	(int a) { return status = a % 8; }		// RECOMENDED ( else unknown STATE )
       int   Mode_get  	(void ) { return status; }			// RECOMENDED ( else unknown STATE )
const  char *Name	(void ) { return "deep"; }			// MANDATORY
const  char *Display	(void ) {
	switch(status) {
	default : return "24 Bit";
	case  1 : return "21 Bit";
	case  2 : return "18 Bit";
	case  3 : return "15 Bit";
	case  4 : return "12 Bit";
	case  5 : return " 9 Bit";
	case  6 : return " 6 Bit";
	case  7 : return " 3 Bit";
		}
	return "Strange deep";
	}             	// RECOMENDED ( else display filter name )

int Apply(int mode, const SDL_v4l_image *SRC, SDL_v4l_image *DST) {
	switch(mode) {
	case 1  : return MMX_bv_and (SRC, DST, 0xFFFEFEFE); break;
	case 2  : return MMX_bv_and (SRC, DST, 0xFFFCFCFC); break;
	case 3  : return MMX_bv_and (SRC, DST, 0xFFF8F8F8); break;
	case 4  : return MMX_bv_and (SRC, DST, 0xFFF0F0F0); break;
	case 5  : return MMX_bv_and (SRC, DST, 0xFFE0E0E0); break;
	case 6  : return MMX_bv_and (SRC, DST, 0xFFC0C0C0); break;
	case 7  : return MMX_bv_and (SRC, DST, 0xFF808080); break;
		}
	return 0;
	}


/*
 * vim600: fdm=marker
 */
