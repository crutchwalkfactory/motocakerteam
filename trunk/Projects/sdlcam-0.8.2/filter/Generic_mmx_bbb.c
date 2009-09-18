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

#define	MMX_pre_bbb 	"push		%%eax		\n"\
			"push		%%ebx		\n"\
			"push		%%ecx		\n"\
			"push		%%esi		\n"\
			"push		%%edi		\n"\
			"mov      	%3, %%ecx	\n"\
			"mov	     	%2, %%ebx	\n"\
			"mov	     	%1, %%edi	\n"\
			"mov	     	%0, %%esi	\n"\
			"shr        	$6, %%ecx	\n"\
			".align 16  			\n" 

#define	MMX_edi8(CMD)	"movq 	  (%%esi), %%mm0\n"\
			"movq 	 8(%%esi), %%mm1\n"\
			"movq 	16(%%esi), %%mm2\n"\
			"movq 	24(%%esi), %%mm3\n"\
			"movq 	32(%%esi), %%mm4\n"\
			"movq 	40(%%esi), %%mm5\n"\
			"movq 	48(%%esi), %%mm6\n"\
			"movq 	56(%%esi), %%mm7\n"\
			CMD" 	  (%%edi), %%mm0\n"\
			CMD" 	 8(%%edi), %%mm1\n"\
			CMD" 	16(%%edi), %%mm2\n"\
			CMD" 	24(%%edi), %%mm3\n"\
			CMD" 	32(%%edi), %%mm4\n"\
			CMD" 	40(%%edi), %%mm5\n"\
			CMD" 	48(%%edi), %%mm6\n"\
			CMD" 	56(%%edi), %%mm7\n"\
			"movq 	%%mm0,   (%%ebx)\n"\
			"movq 	%%mm1,  8(%%ebx)\n"\
			"movq 	%%mm2, 16(%%ebx)\n"\
			"movq 	%%mm3, 24(%%ebx)\n"\
			"movq 	%%mm4, 32(%%ebx)\n"\
			"movq 	%%mm5, 40(%%ebx)\n"\
			"movq 	%%mm6, 48(%%ebx)\n"\
			"movq 	%%mm7, 56(%%ebx)\n"\
			"add      $64, %%esi    \n"\
			"add      $64, %%edi    \n"\
			"add      $64, %%ebx    \n"\
			"dec           %%ecx    \n"

#define	MMX_post_bbb 	"emms                	\n"\
			"pop		%%edi	\n"\
			"pop		%%esi	\n"\
			"pop		%%ecx	\n"\
			"pop		%%ebx	\n"\
			"pop		%%eax	\n"


static int MMX_and         (const SDL_v4l_image *A, const SDL_v4l_image *B, SDL_v4l_image *DST)  {{{
	unsigned char *a  = A->data;
	unsigned char *b  = B->data;
	unsigned char *dst;
	unsigned char len = A->len;
	if (A->len != B->len) return 0;
 	SDL_v4l_Buffer_Prepare (A, DST);
    dst = DST->data;
#ifdef MMX
	if (len < 8) return 0;              // image size must be at least 8 bytes 
	asm volatile (
		MMX_pre_bbb
	".and_MMX:				\n"
		MMX_edi8("pand")
	"	jnz             .and_MMX	\n"
		MMX_post_bbb
	: "=m"  (a)         	// %0
	:  "m"  (b),        	// %1
	   "m"	(dst),		// %2
	    "m"	(len)		// %3
		);
#else
	len >>= 2;
	while (len--) *(dst++) = *(a++) & *(b++);
#endif
	return 1;
	}}}
static int MMX_andn        (const SDL_v4l_image *A, const SDL_v4l_image *B, SDL_v4l_image *DST) {{{
	unsigned char *a  = A->data;
	unsigned char *b  = B->data;
	unsigned char *dst;
	unsigned char len = A->len;
	if (A->len != B->len) return 0;
 	SDL_v4l_Buffer_Prepare (A, DST);
    dst = DST->data;
#ifdef MMX	
	if (len < 8) return 0;              // image size must be at least 8 bytes 
	asm volatile (
		MMX_pre_bbb
	".andn_MMX:				\n"
		MMX_edi8("pandn")
	"	jnz             .andn_MMX	\n"
		MMX_post_bbb
		: "=m"  (a)         // %0
		:"m"    (b),        // %1
	     "m"	(dst),		// %2
		 "m"	(len)		// %3
		);
#else
	len >>= 2;
	while (len--) *(dst++) = *(a++) & (~ *(b++));
#endif
    return 1;
	}}}
static int MMX_xor         (const SDL_v4l_image *A, const SDL_v4l_image *B, SDL_v4l_image *DST) {{{
	unsigned char *a  = A->data;
	unsigned char *b  = B->data;
	unsigned char *dst;
	unsigned char len = A->len;
	if (A->len != B->len) return 0;
 	SDL_v4l_Buffer_Prepare (A, DST);
    dst = DST->data;
#ifdef MMX	
	if (len < 8) return 0;              // image size must be at least 8 bytes 
	asm volatile (
		MMX_pre_bbb
	".xor_MMX:				\n"
		MMX_edi8("pxor")
	"	jnz             .xor_MMX	\n"
		MMX_post_bbb
		: "=m"  (a)         // %0
		:"m"    (b),        // %1
	     "m"	(dst),		// %2
		 "m"	(len)		// %3
		);
#else
	len >>= 2;
	while (len--) *(dst++) = *(a++) ^ *(b++);
#endif
    return 1;
	}}}
static int MMX_or          (const SDL_v4l_image *A, const SDL_v4l_image *B, SDL_v4l_image *DST) {{{
	unsigned char *a  = A->data;
	unsigned char *b  = B->data;
	unsigned char *dst;
	unsigned char len = A->len;
	if (A->len != B->len) return 0;
 	SDL_v4l_Buffer_Prepare (A, DST);
    dst = DST->data;
#ifdef MMX	
	if (len < 8) return 0;              // image size must be at least 8 bytes 
	asm volatile (
		MMX_pre_bbb
	".or_MMX:			\n"
		MMX_edi8("por")
	"	jnz             .or_MMX	\n"
		MMX_post_bbb
		: "=m"  (a)         // %0
		:"m"    (b),        // %1
	     "m"	(dst),		// %2
		 "m"	(len)		// %3
		);
#else
	len >>= 2;
	while (len--) *(dst++) = *(a++) | *(b++);
#endif
	return 1;
	}}}
static int MMX_subb        (const SDL_v4l_image *A, const SDL_v4l_image *B, SDL_v4l_image *DST) {{{
	unsigned char *a  = A->data;
	unsigned char *b  = B->data;
	unsigned char *dst;
	unsigned char len = A->len;
	if (A->len != B->len) return 0;
 	SDL_v4l_Buffer_Prepare (A, DST);
    dst = DST->data;
#ifdef MMX
	if (len < 8) return 0;              // image size must be at least 8 bytes 
	asm volatile (
		MMX_pre_bbb
	".subb_MMX:				\n"
		MMX_edi8("psubb")
	"	jnz             .subb_MMX	\n"
		MMX_post_bbb
		: "=m"  (a)         // %0
		:"m"    (b),        // %1
	     "m"	(dst),		// %2
		 "m"	(len)		// %3
		);
#else		 
	while(len--) *(dst++)=*(a++)-(*(b++));
#endif		 
	return 1;
	}}}
static int MMX_addb        (const SDL_v4l_image *A, const SDL_v4l_image *B, SDL_v4l_image *DST) {{{
	unsigned char *a  = A->data;
	unsigned char *b  = B->data;
	unsigned char *dst;
	unsigned char len = A->len;
	if (A->len != B->len) return 0;
 	SDL_v4l_Buffer_Prepare (A, DST);
    dst = DST->data;
#ifdef MMX	
	if (len < 8) return 0;              // image size must be at least 8 bytes 
	asm volatile (
		MMX_pre_bbb
	".addb_MMX:				\n"
		MMX_edi8("paddb")
	"	jnz             .addb_MMX	\n"
		MMX_post_bbb
		: "=m"  (a)         // %0
		:"m"    (b),        // %1
	     "m"	(dst),		// %2
		 "m"	(len)		// %3
		);
#else
	while(len--) *(dst++)=*(a++)+*(b++);
#endif		 
	return 1;
}}}
static int MMX_subusb      (const SDL_v4l_image *A, const SDL_v4l_image *B, SDL_v4l_image *DST) {{{
	unsigned char *a  = A->data;
	unsigned char *b  = B->data;
	unsigned char *dst;
	unsigned char len = A->len;
	if (A->len != B->len) return 0;
 	SDL_v4l_Buffer_Prepare (A, DST);
    dst = DST->data;
#ifdef MMX	
	if (len < 8) return 0;              // image size must be at least 8 bytes 
	asm volatile (
		MMX_pre_bbb	
	".subusb_MMX:				\n"
		MMX_edi8("psubusb")
	"	jnz             .subusb_MMX	\n"
		MMX_post_bbb
		: "=m"  (a)         // %0
		:"m"    (b),        // %1
	     "m"	(dst),		// %2
		 "m"	(len)		// %3
		);
#else
	while(len--) 
	  {
	    if(*a<*b) *(dst++)=0;
		else	  *(dst++)=*a-*b;
		a++;
		b++;
	  }	
#endif		 
	return 1;
	}}}
static int MMX_addusb      (const SDL_v4l_image *A, const SDL_v4l_image *B, SDL_v4l_image *DST) {{{
	unsigned char *a  = A->data;
	unsigned char *b  = B->data;
	unsigned char *dst;
	unsigned char len = A->len;
	if (A->len != B->len) return 0;
 	SDL_v4l_Buffer_Prepare (A, DST);
    dst = DST->data;
#ifdef MMX	
/* {{{ ASM */	
	if (len < 8) return 0;              // image size must be at least 8 bytes 
	asm volatile (
		MMX_pre_bbb
	".addusb_MMX:				\n"
		MMX_edi8("paddusb")
	"	jnz             .addusb_MMX	\n"
		MMX_post_bbb
		: "=m"  (a)         // %0
		:"m"    (b),        // %1
	     "m"	(dst),		// %2
		 "m"	(len)		// %3
		);
/* }}} */		 
#else
	while(len--)
	  {
	    if((unsigned int)*a+(unsigned int)*b > 255) *(dst++)=255;
		else										*(dst++)=*a+*b;
		a++;
		b++;
	  }
#endif		 
	return 1;
	}}}

static int   status = 0;
       int   Type   	(void ) { return 2; }				// MANDATORY
       int   Toggle 	(void ) { return status = (status + 1) % 9; }	// RECOMENDED ( else always  ON    )
       int   Mode_set  	(int a) { return status = a % 9; }		// RECOMENDED ( else unknown STATE )
       int   Mode_get  	(void ) { return status; }			// RECOMENDED ( else unknown STATE )
const  char *Name	(void ) { return "generic-mmx-bbb"; }		// MANDATORY
const  char *Display	(void ) {
	switch (status) {
	default : return "Gen_mmx_bbb"; 
	case  1 : return "AND_mmx_bbb";
	case  2 : return "ANDN_mmx_bbb";
	case  3 : return "XOR_mmx_bbb";
	case  4 : return "OR_mmx_bbb";
	case  5 : return "SUBB_mmx_bbb";
	case  6 : return "ADD_mmx_bbb";
	case  7 : return "SUBUSB_mmx_bbb";
	case  8 : return "ADDUSB_mmx_bbb";
	  }
	}
int Apply (int mode, const SDL_v4l_image *A, const SDL_v4l_image *B, SDL_v4l_image *DST) {
	switch (mode) {
	default : return 0;
	case  1 : return MMX_and	(A, B, DST);
	case  2 : return MMX_andn	(A, B, DST);
	case  3 : return MMX_xor	(A, B, DST);
	case  4 : return MMX_or		(A, B, DST);
	case  5 : return MMX_subb	(A, B, DST);
	case  6 : return MMX_addb	(A, B, DST);
	case  7 : return MMX_subusb	(A, B, DST);
	case  8 : return MMX_addusb	(A, B, DST);
	}
  return 0;
}

/*
 * vim600: fdm=marker
 */
