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

/*
	Author: Thomas Luﬂnig <thomas.kussnig@bewegungsmelder.de>
	This filter double the image size.
	Use Noise or avarage for bether details.
*/

#include "../SDL_v4l_image.h"
#include "../dyn_misc.h"
#include <stdlib.h>

/* {{{ MMX helper macros */
#define	MMX_val4(CMD) 	"movq	  (%%esi), %%mm0        \n"\
			"movq	 8(%%esi), %%mm1	\n"\
			"movq	16(%%esi), %%mm2        \n"\
			"movq	24(%%esi), %%mm3        \n"\
			CMD" 	   %%mm4 , %%mm0	\n"\
			CMD" 	   %%mm4 , %%mm1	\n"\
			CMD" 	   %%mm4 , %%mm2	\n"\
			CMD" 	   %%mm4 , %%mm3	\n"\
			"movq      %%mm0 ,   (%%edi)    \n"\
			"movq      %%mm1 ,  8(%%edi)    \n"\
			"movq      %%mm2 , 16(%%edi)    \n"\
			"movq      %%mm3 , 24(%%edi)    \n"\
			"add       $32   , %%esi        \n"\
			"add       $32   , %%edi        \n"\
			"dec       %%ecx                \n"

#define	MMX_edi8(CMD) 	CMD" 	  (%%edi), %%mm0	\n"\
			CMD" 	 8(%%edi), %%mm1	\n"\
			CMD" 	16(%%edi), %%mm2	\n"\
			CMD" 	24(%%edi), %%mm3	\n"\
			CMD" 	32(%%edi), %%mm4	\n"\
			CMD" 	40(%%edi), %%mm5	\n"\
			CMD" 	48(%%edi), %%mm6	\n"\
			CMD" 	56(%%edi), %%mm7	\n"

#define MMX_bc_prepare  "push		%%ecx		\n"\
			"push		%%esi		\n"\
			"push		%%edi		\n"\
			"mov		%3   , %%ecx	\n"\
			"movd		%%ecx, %%mm0	\n"\
			"movd		%%ecx, %%mm4    \n"\
			"punpckldq	%%mm0, %%mm4    \n"\
			"mov         	%2   , %%ecx    \n"\
			"mov	     	%1   , %%edi	\n"\
			"mov	     	%0   , %%esi    \n"\
			"shr         	$5   , %%ecx    \n"\
			"pcmpeqb   	%%mm0, %%mm0    \n"\
			".align 16                      \n"

#define MMX_bc_post 	"emms                  		\n"\
			"pop		%%edi		\n"\
			"pop		%%esi		\n"\
			"pop		%%ecx		\n"


/* }}} */					  

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
	MMX_bc_prepare
/* {{{ working loop */
	".andi_MMX:                                     \n"
	MMX_val4("pand")
	"	jnz             .andi_MMX               \n"
/* }}} */
	MMX_bc_post
	: "=m"  (src)           // %0
	:  "m"  (dst),          // %1
           "m"	(len),		// %2
	   "m"	(val)		// %3
		);
#else
	len>>=2;
	while (len--) *(dst++) = *(src++) & val;
#endif
	return 1;
	}}}
int MMX_bv_andn     (const SDL_v4l_image *SRC, SDL_v4l_image *DST, unsigned int val) {{{
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
	MMX_bc_prepare
	".andni_MMX:					\n"
	MMX_val4("pandn")
	"	jnz             .andni_MMX 		\n"
	MMX_bc_post
		: "=m"  (src)           // %0
		:"m"    (dst),          // %1
	     "m"	(len),			// %2
		 "m"	(val)			// %3
		);
#else
	len>>=2;
	while (len--) *(dst++) = *(src++) & (~val);
#endif
	return 1;
	}}}
int MMX_bv_xor      (const SDL_v4l_image *SRC, SDL_v4l_image *DST, unsigned int val) {{{
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
	MMX_bc_prepare
	".xori_MMX:\n"
		MMX_val4("pxor")
	"	jnz             .xori_MMX 	\n"
	MMX_bc_post
		: "=m"  (src)           // %0
		:"m"    (dst),          // %1
	     "m"	(len),			// %2
		 "m"	(val)			// %3
		);
#else
	len>>=2;
	while (len--) *(dst++) = *(src++) ^ val;
#endif
	return 1;
	}}}
int MMX_bv_or       (const SDL_v4l_image *SRC, SDL_v4l_image *DST, unsigned int val) {{{
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
	MMX_bc_prepare
	".ori_MMX:	\n"
		MMX_val4("por")
	"	jnz             .ori_MMX \n"
	MMX_bc_post
		: "=m"  (src)           // %0
		:"m"    (dst),          // %1
	     "m"	(len),			// %2
		 "m"	(val)			// %3
		);
#else
	len >>= 2;
	while (len--) *(dst++) = *(src++) | val;
#endif
	return 1;
}}}
int MMX_bv_subb     (const SDL_v4l_image *SRC, SDL_v4l_image *DST, unsigned int val) {{{
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
	MMX_bc_prepare
	".subbi_MMX:				\n"
		MMX_val4("psubb")
	"	jnz             .subbi_MMX 	\n"
	MMX_bc_post
		: "=m"  (src)           // %0
		:"m"    (dst),          // %1
	     "m"	(len),			// %2
		 "m"	(val)			// %3
		);
#else
	while (len--) *(dst++) = *(src++) - val;
#endif
	return 1;
	}}}
int MMX_bv_subusb   (const SDL_v4l_image *SRC, SDL_v4l_image *DST, unsigned int val) {{{
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
	MMX_bc_prepare
	".subusbi_MMX:	\n"
		MMX_val4("psubusb")
	"	jnz             .subusbi_MMX \n"
	MMX_bc_post
		: "=m"  (src)           // %0
		:"m"    (dst),          // %1
	     "m"	(len),			// %2
		 "m"	(val)			// %3
		);
#else
/* {{{ c version */
		 {	 
	unsigned char VAL[4];
	VAL[0]=(unsigned char) ((val>> 0)&0xff);
	VAL[1]=(unsigned char) ((val>> 8)&0xff);
	VAL[2]=(unsigned char) ((val>>16)&0xff);
	VAL[3]=(unsigned char) ((val>>24)&0xff);
	while (len--)
	  {
		  *dst++=(*src>VAL[0])?(*src-VAL[0]):0; src++;
		  *dst++=(*src>VAL[1])?(*src-VAL[1]):0; src++;
		  *dst++=(*src>VAL[2])?(*src-VAL[2]):0; src++;
		  *dst++=(*src>VAL[3])?(*src-VAL[3]):0; src++;
	  }	  
		 }
/* }}} */	
#endif
	return 1;
	}}}
int MMX_bv_addb     (const SDL_v4l_image *SRC, SDL_v4l_image *DST, unsigned int val) {{{
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
	MMX_bc_prepare
	".addbi_MMX:				\n"
		MMX_val4("paddb")
	"	jnz             .addbi_MMX 	\n"
	MMX_bc_post
		: "=m"  (src)           // %0
		:"m"    (dst),          // %1
	     "m"	(len),			// %2
		 "m"	(val)			// %3
		);
#else
/* {{{ c version */		 
	{
	unsigned char VAL[4];
	VAL[0]=(unsigned char) ((val>> 0)&0xff);
	VAL[1]=(unsigned char) ((val>> 8)&0xff);
	VAL[2]=(unsigned char) ((val>>16)&0xff);
	VAL[3]=(unsigned char) ((val>>24)&0xff);
	while (len--)
	  {
		  *dst++ = *(src++)-VAL[0];
		  *dst++ = *(src++)-VAL[1];
		  *dst++ = *(src++)-VAL[2];
		  *dst++ = *(src++)-VAL[3];
	  }	  
	}
/* }}} */
#endif
	return 1;
	}}}
int MMX_bv_addusb   (const SDL_v4l_image *SRC, SDL_v4l_image *DST, unsigned int val) {{{
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
	MMX_bc_prepare
	".addusbi_MMX:		\n"
		MMX_val4("paddusb")
	"	jnz             .addusbi_MMX \n"
	MMX_bc_post
		: "=m"  (src)           // %0
		:"m"    (dst),          // %1
	     "m"	(len),			// %2
		 "m"	(val)			// %3
		);
#else
/* {{{ c version */		 
		 { 
	unsigned char VAL[4];
	VAL[0]=(unsigned char) ((val>> 0)&0xff);
	VAL[1]=(unsigned char) ((val>> 8)&0xff);
	VAL[2]=(unsigned char) ((val>>16)&0xff);
	VAL[3]=(unsigned char) ((val>>24)&0xff);
	while (len--)
	  {
		  *dst++=(((unsigned int)*src+(unsigned int)VAL[0])<255)?(*src+VAL[0]):255; src++;
		  *dst++=(((unsigned int)*src+(unsigned int)VAL[1])<255)?(*src+VAL[1]):255; src++;
		  *dst++=(((unsigned int)*src+(unsigned int)VAL[2])<255)?(*src+VAL[2]):255; src++;
		  *dst++=(((unsigned int)*src+(unsigned int)VAL[3])<255)?(*src+VAL[3]):255; src++;
	  }	  
		 }
/* }}} */	
#endif
return 1;
	}}}

static int   status = 0;
       int   Type   	(void ) { return 3; }				// MANDATORY
       int   Toggle 	(void ) { return status = (status + 1) % 9; }	// RECOMENDED ( else always  ON    )
       int   Mode_set  	(int a) { return status = a % 9; }		// RECOMENDED ( else unknown STATE )
       int   Mode_get  	(void ) { return status; }			// RECOMENDED ( else unknown STATE )
const  char *Name	(void ) { return "generic-mmx-bbv"; }		// MANDATORY
const  char *Display	(void ) {
	switch (status) {
	default : return "Gen_mmx_bbv"; 
	case  1 : return "AND_mmx_bbv";
	case  2 : return "ANDN_mmx_bbv";
	case  3 : return "XOR_mmx_bbv";
	case  4 : return "OR_mmx_bbv";
	case  5 : return "SUBB_mmx_bbv";
	case  6 : return "ADD_mmx_bbv";
	case  7 : return "SUBUSB_mmx_bbv";
	case  8 : return "ADDUSB_mmx_bbv";
	  }
	}
int Apply (int mode, const SDL_v4l_image *A, SDL_v4l_image *DST, unsigned int val) {
	switch (mode) {
	default : return 0;
	case  1 : return MMX_bv_and	(A, DST, val);
	case  2 : return MMX_bv_andn	(A, DST, val);
	case  3 : return MMX_bv_xor	(A, DST, val);
	case  4 : return MMX_bv_or	(A, DST, val);
	case  5 : return MMX_bv_subb	(A, DST, val);
	case  6 : return MMX_bv_addb	(A, DST, val);
	case  7 : return MMX_bv_subusb	(A, DST, val);
	case  8 : return MMX_bv_addusb	(A, DST, val);
	}
  return 0;
}

/*
 * vim600: fdm=marker
 */
