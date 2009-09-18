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
/*
 * Flip the Image horizontal
 *
 * Input:
 * SDL_v4l_image *		Source/Dest
 *
 * Output:
 * int					0/0 succes
 *
 * Modify:
 * param[0]
 */
int Apply(int mode, SDL_v4l_image *image)
{
  unsigned int *data = (unsigned int *)image->data;
  unsigned int idx = 0;
  unsigned int  temp;
  unsigned int w	   = image->w;
  unsigned int w2	   = w >> 1;
  unsigned int x;
  unsigned int y = image->h;
  if (mode == 0) return 0;
  if(image->len&3) return 0;
  do
    {
      x = w2;
      do
        {
	  temp = data[idx + x];
	  data[idx + x] = data[idx + w - x];
	  data[idx + w - x] = temp;
	} while (--x);
      idx += w;
    } while (--y);
  return 1;
}

static int   status = 0;

       int   Type   	(void ) { return 1; }				// MANDATORY
       int   Toggle 	(void ) { return status = (status + 1) % 2; }	// RECOMENDED ( else always  ON    )
       int   Mode_set  	(int a) { return status = a % 2; }		// RECOMENDED ( else unknown STATE )
       int   Mode_get  	(void ) { return status; }			// RECOMENDED ( else unknown STATE )
const  char *Name	(void ) { return "flip-x"; }			// MANDATORY
const  char *Display	(void ) {return "Flip-X"; }             	// RECOMENDED ( else display filter name )

/*
 * vim600: fdm=marker
 */
