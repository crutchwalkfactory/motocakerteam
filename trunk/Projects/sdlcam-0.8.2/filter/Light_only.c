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
int Apply (int mode, const SDL_v4l_image *A, const SDL_v4l_image *B, SDL_v4l_image *DST)  {
	unsigned char *a = A->data;
	unsigned char *b = B->data;
	unsigned char *dst;
	unsigned int len = A->len;
	if(mode == 0) return 0;
	if (len != B->len) return 0;
	if (len < 8) return 0;              // image size must be at least 8 bytes 
	SDL_v4l_Buffer_Prepare (A, DST);
	dst = DST->data;
#ifdef MMX	
	/* {{{ */
	asm volatile (
	"	push		%%ecx			\n"
	"	push		%%esi			\n"
	"	push		%%edi			\n"
	"	push		%%ebx                   \n"
	"	mov          	%3, %%ecx		\n"
	"	mov	     	%2, %%ebx               \n"
	"	mov	     	%1, %%edi               \n"
	"	mov	     	%0, %%esi               \n"
	"	shr          	$5, %%ecx		\n"
	".align 16                                      \n"
	".light_only_MMX:                               \n"
	"	movq    	  (%%esi), %%mm0        \n"
	"	movq    	 8(%%esi), %%mm1        \n"
	"	movq    	16(%%esi), %%mm2        \n"
	"	movq    	24(%%esi), %%mm3        \n"
	"	movq		%%mm0    , %%mm4        \n"
	"	movq        %%mm1    , %%mm5		\n"
	"	movq        %%mm2    , %%mm6            \n"
	"	movq        %%mm3    , %%mm7            \n"
	"	pcmpgtb	   	  (%%edi), %%mm0        \n"
	"	pcmpgtb	   	 8(%%edi), %%mm1        \n"
	"	pcmpgtb	   	16(%%edi), %%mm2        \n"
	"	pcmpgtb	   	24(%%edi), %%mm3        \n"
	"	pand		%%mm0    , %%mm4        \n"
	"	pand		%%mm1    , %%mm5        \n"
	"	pand		%%mm2    , %%mm6        \n"
	"	pand		%%mm3    , %%mm7        \n"
	"	pandn		  (%%edi), %%mm0        \n"
        "	pandn       	 8(%%edi), %%mm1        \n"
	"	pandn       	16(%%edi), %%mm2        \n"
        "	pandn       	24(%%edi), %%mm3        \n"
	"	por		%%mm4    , %%mm0        \n"
	"	por		%%mm5    , %%mm1	\n"
	"	por		%%mm6    , %%mm2        \n"
	"	por		%%mm7    , %%mm3        \n"
	"	movq    	%%mm0    ,   (%%ebx)    \n"
	"	movq    	%%mm1    ,  8(%%ebx)    \n"
	"	movq    	%%mm2    , 16(%%ebx)    \n"
	"	movq    	%%mm3    , 24(%%ebx)    \n"
	"	add          	$32  , %%esi            \n"
	"	add          	$32  , %%edi            \n"
	"	add          	$32  , %%ebx            \n"
	"	dec              %%ecx                  \n"
	"	jnz             .light_only_MMX         \n"
	"	emms                                    \n"
	"	pop		%%ebx                   \n"
	"	pop		%%edi                   \n"
	"	pop		%%esi                   \n"
	"	pop		%%ecx			\n"
		: "=m"  (a)         // %0
		:"m"    (b),        // %1
	     "m"	(dst),		// %2
		 "m"	(len)		// %3
		);
	/* }}} */	 
#else		 
  while(len--)
    {
      if(*a>*b) *(dst++) = *a;
	  else	    *(dst++) = *b;
	  a++;
	  b++;
    }
#endif		 
  return 1;
  }

static int   status = 0;
       int   Type   	(void ) { return 2; }				// MANDATORY
       int   Toggle 	(void ) { return status = (status + 1) % 2; }	// RECOMENDED ( else always  ON    )
       int   Mode_set  	(int a) { return status = a % 2; }		// RECOMENDED ( else unknown STATE )
       int   Mode_get  	(void ) { return status; }			// RECOMENDED ( else unknown STATE )
const  char *Name	(void ) { return "light-only"; 	}		// MANDATORY
const  char *Display	(void ) { return "Light-Only";	}
/*
 * vim600: fdm=marker
 */
