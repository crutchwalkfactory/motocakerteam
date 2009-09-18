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
#include <string.h>

#define RANGE_COUNT_MAX_LB 6 				/* COUNT = 2^4   */
#define RANGE_COUNT_MAX (1<<RANGE_COUNT_MAX_LB)

SDL_v4l_image buffer_time[RANGE_COUNT_MAX];
void Cleanup (void) {{{
  int i;
  for (i = 0; i < RANGE_COUNT_MAX; i++)
    if (buffer_time[i].data != NULL)
	  free (buffer_time[i].data);
}}}
int  Apply(int max_lb, SDL_v4l_image *image) {{{
	static int count=0;
	unsigned int i = 0;
	int update = 0;
	unsigned int SIZE;
	static short my_rand=17;
	unsigned int *buf[RANGE_COUNT_MAX];
	unsigned int *src = (unsigned int *)image->data;
	if (max_lb <= 0) return 0;
	if (max_lb > RANGE_COUNT_MAX_LB)
	  max_lb=RANGE_COUNT_MAX_LB;
	SIZE = 1<<max_lb;
	for (i=0; i< SIZE; i++)
	  {
	    update |= SDL_v4l_Buffer_Prepare (image,&(buffer_time[i]));
		buf[i]=(unsigned int *)buffer_time[i].data;
	  }
	memcpy(buf[count],src,image->len);
	i = image->len >> 2;
	src+=i;
	src--;
	SIZE--;
	while(i--)
	 {
	   unsigned int v;
	   if( ! (i%image->w) ) my_rand = rand();
	   my_rand = my_rand*117+137;
	   v=*src;
	   *(src--)=(buf[my_rand&SIZE])[i];
	 }
	count++;
	if(count==SIZE) count=0;
	return 1;
}}}

static int   status = 0;

       int   Type   	(void ) { return 1; }				// MANDATORY
       int   Toggle 	(void ) { return status = (status + 1) % (RANGE_COUNT_MAX_LB+1); }	// RECOMENDED ( else always  ON    )
       int   Mode_set  	(int a) { return status = a % (RANGE_COUNT_MAX_LB+1); }		// RECOMENDED ( else unknown STATE )
       int   Mode_get  	(void ) { return status; }			// RECOMENDED ( else unknown STATE )
const  char *Name	(void ) { return "time-rand"; }			// MANDATORY
const  char *Display	(void ) {return "Time-Random"; }             	// RECOMENDED ( else display filter name )

/*
 * vim600: fdm=marker
 */
