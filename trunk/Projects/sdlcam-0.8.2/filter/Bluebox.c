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

static int   status = 0;
       int   Type   	(void ) { return 4; }
       int   Toggle 	(void ) { return status = (status + 1) % 2; }
       int   Mode_set  	(int a) { return status = a % 2; }
       int   Mode_get  	(void ) { return status; }
const  char *Name	(void ) { return "bluebox"; }	
const  char *Display	(void ) { return "Bluebox"; }

int Apply (int mode, const SDL_v4l_image *BUF, SDL_v4l_image *DAT, unsigned int trash) {{{
	char MASK[8];
	unsigned char *buf = BUF->data;
	unsigned char *dat = DAT->data;
	unsigned int len = BUF->len;
	if (mode == 0) return 0;
	if (BUF->len!=DAT->len) return 0;
#ifdef MMX
	/* {{{ MMX */
	/* D = A and (A trashmask B) */
	MASK[0]=trash;
	MASK[1]=trash;
	MASK[2]=trash;
	MASK[3]=trash;
	MASK[4]=trash;
	MASK[5]=trash;
	MASK[6]=trash;
	MASK[7]=trash;
	if (len < 8) return 0;              // image size must be at least 8 bytes 
	asm volatile (
	"	push		%%ebx				\n"
	"	push		%%ecx				\n"
	"	push		%%esi				\n"
	"	push		%%edi				\n"
	"	mov          	%3, %%ecx			\n"
	"	mov	     	%2, %%ebx                       \n"
	"	mov	     	%1, %%edi                       \n"
	"	mov	     	%0, %%esi                       \n"
	"	shr          	$5, %%ecx			\n"
	".align 16                                              \n"
	".blue_MMX:                                             \n"
	"	movq    	  (%%esi), %%mm0                \n"
	"	movq    	 8(%%esi), %%mm1                \n"
	"	movq    	16(%%esi), %%mm2                \n"
	"	movq    	24(%%esi), %%mm3                \n"
	"	movq    	  (%%edi), %%mm4                \n"
	"	movq    	 8(%%edi), %%mm5		\n"
	"	movq    	16(%%edi), %%mm6                \n"
	"	movq    	24(%%edi), %%mm7                \n"
	/* tmp0=BUF-DAT */
	"	psubusb		%%mm4	 , %%mm0                \n"
	"	psubusb 	%%mm5	 , %%mm1                \n"
	"	psubusb 	%%mm6	 , %%mm2                \n"
	"	psubusb 	%%mm7	 , %%mm3                \n"
	/* tmp1=DAT-BUF */
	"	psubusb		  (%%esi), %%mm4	        \n"
	"	psubusb          8(%%esi), %%mm5                \n"
	"	psubusb         16(%%esi), %%mm6                \n"
	"	psubusb         24(%%esi), %%mm7                \n"
	/* tmp0=absdif(DAT,BUF) */
	"	por		%%mm4	 , %%mm0	        \n"
	"	por             %%mm5	 , %%mm1                \n"
	"	por             %%mm6	 , %%mm2		\n"
	"	por             %%mm7	 , %%mm3                \n"
	/* tmp0=(tmp0<trash)?0;0xff */
	"	pcmpgtb		(%%ebx)  , %%mm0                \n"
	"	pcmpgtb		(%%ebx)  , %%mm1                \n"
	"	pcmpgtb		(%%ebx)  , %%mm2                \n"
	"	pcmpgtb		(%%ebx)  , %%mm3                \n"
	"	pxor     %%mm7,  %%mm7                          \n"
	"	pcmpgtd	 %%mm7 , %%mm0                          \n"
	"	pcmpgtd	 %%mm7 , %%mm1                          \n"
	"	pcmpgtd	 %%mm7 , %%mm2                          \n"
	"	pcmpgtd  %%mm7 , %%mm3                          \n"
	/* tmp=tmp0 AND DAT */
	"	pand		  (%%edi), %%mm0                \n"
	"	pand		 8(%%edi), %%mm1                \n"
	"	pand		16(%%edi), %%mm2                \n"
	"	pand		24(%%edi), %%mm3		\n"
	/* DAT = tmp0 */
	"	movq    	%%mm0,   (%%edi)                \n"
	"	movq    	%%mm1,  8(%%edi)                \n"
	"	movq    	%%mm2, 16(%%edi)                \n"
	"	movq    	%%mm3, 24(%%edi)                \n"
	"	add          	$32, %%esi                      \n"
	"	add          	$32, %%edi                      \n"
	"	dec              %%ecx                          \n"
	"	jnz             .blue_MMX			\n"
	"	emms                  				\n"
	"	pop		%%edi				\n"
	"	pop		%%esi				\n"
	"	pop		%%ecx				\n"
	"	pop		%%ebx				\n"
		: "=m"  (buf)		// %0
		:"m"    (dat),		// %1
		 "g"	(MASK),		// %2
		 "m"	(len)		// %3
		);
	/* }}} */
#else
    len>>=2;
	while(len--)
	  {
	    int d[3];
		d[0]=(dat[0]>buf[0])?(dat[0]-buf[0]):(buf[0]-dat[0]);
		d[1]=(dat[1]>buf[1])?(dat[1]-buf[1]):(buf[1]-dat[1]);
		d[2]=(dat[2]>buf[2])?(dat[2]-buf[2]):(buf[2]-dat[2]);
		if(d[0]<trash && d[1]<trash &&d[2]<trash)
		  dat[0]=dat[1]=dat[2]=0;
		dat+=4;
		buf+=4;
	  }
#endif
	return 1;
	}}}

/*
 * vim600: fdm=marker
 */
