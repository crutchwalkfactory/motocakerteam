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
       int   Type   	(void ) { return 3; }
       int   Toggle 	(void ) { return status = (status + 1) % 2; }
       int   Mode_set  	(int a) { return status = a % 2; }
       int   Mode_get  	(void ) { return status; }
const  char *Name	(void ) { return "trashmask"; }	
const  char *Display	(void ) { return "Trashmask"; }

int Apply (int mode, const SDL_v4l_image *SRC, SDL_v4l_image *DST, unsigned int trash) {{{
	char MASK[8];
	unsigned char *src = SRC->data;
	unsigned int len = SRC->len;
	unsigned char *dst;
	if (mode  == 0) return 0;
	if (len    < 8) return 0;				// below 8 it is to short
	if (trash == 0) return 1;
	SDL_v4l_Buffer_Prepare (SRC, DST);
    	dst = DST->data;
#ifdef MMX	
	/* {{{ ASM */
	MASK[0]=trash;
	MASK[1]=trash;
	MASK[2]=trash;
	MASK[3]=trash;
	MASK[4]=trash;
	MASK[5]=trash;
	MASK[6]=trash;
	MASK[7]=trash;
	asm volatile (
	"	push		%%ecx			\n"
	"	push		%%esi			\n"
	"	push		%%edi			\n"
	"	push		%%ebx                   \n"
	"	mov        	%3, %%ecx		\n"
	"	mov	     	%2, %%edi               \n"
	"	mov	     	%1, %%ebx               \n"
	"	mov	     	%0, %%esi               \n"
	"	shr        	$5, %%ecx		\n"
	".align 16                                      \n"
	".trashmask_MMX:                                \n"
	"	movq    	  (%%esi), %%mm0        \n"
	"	movq    	 8(%%esi), %%mm1        \n"
	"	movq    	16(%%esi), %%mm2        \n"
	"	movq    	24(%%esi), %%mm3        \n"
	"	pcmpgtb		(%%edi)  , %%mm0        \n"
	"	pcmpgtb		(%%edi)  , %%mm1	\n"
	"	pcmpgtb		(%%edi)  , %%mm2	\n"
	"	pcmpgtb		(%%edi)  , %%mm3	\n"
		/* make sure if one channel is above trash all are masked */
	"	pxor     %%mm7,  %%mm7			\n"
	"	pcmpgtd	 %%mm7 , %%mm0                  \n"
	"	pcmpgtd	 %%mm7 , %%mm1                  \n"
	"	pcmpgtd	 %%mm7 , %%mm2                  \n"
	"	pcmpgtd  %%mm7 , %%mm3                  \n"
	"	movq    	%%mm0,   (%%ebx)        \n"
	"	movq    	%%mm1,  8(%%ebx)        \n"
	"	movq    	%%mm2, 16(%%ebx)        \n"
	"	movq    	%%mm3, 24(%%ebx)	\n"
	"	add          	$32, %%esi              \n"
	"	add          	$32, %%ebx              \n"
	"	dec              %%ecx                  \n"
	"	jnz             .trashmask_MMX          \n"
	"	emms                                    \n"
	"	pop		%%ebx                   \n"
	"	pop		%%edi                   \n"
	"	pop		%%esi			\n"
	"	pop		%%ecx			\n"
		: "=m"  (src)		// %0
		: "m"	(dst),		// %1 
		 "g"    (MASK),         // %2
		 "m"	(len)		// %3
		);
	/* }}} */
#else	
  len >>=2;
  while(len--)
	{
	  if(src[0]<trash && src[1]<trash && src[2]<trash && src[3]<trash) 
	    dst[0]=dst[1]=dst[2]=dst[3]=0xff;
	  else
		dst[0]=dst[1]=dst[2]=dst[3]=0x0;
	  src+=4;
	  dst+=4;
	}
#endif		 
	return 1;
	}}}

/*
 * vim600: fdm=marker
 */
