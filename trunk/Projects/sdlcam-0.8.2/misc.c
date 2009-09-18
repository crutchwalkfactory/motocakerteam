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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "dyn_misc.h"
#include "misc.h"

#define rdtsc(low,high) \
     __asm__ __volatile__("rdtsc" : "=a" (low), "=d" (high))



char *xStrdup (const char *t) {{{
  char *ret;
  ret = strdup (t);
  if (ret != NULL)
    return ret;
  fprintf (stderr, "strdup (%s) FAILD", t);
  exit (-1);
  return NULL; /* prevents warnings ;) */
}}}

TimeStampCounter start[256];
TimeStampCounter ende [256];
long long 	summe[256];
int		 count[256];

void getTimeStampCounter ( TimeStampCounter *t ) {{{
#ifdef TIMER	
	unsigned long h=t->high;
	unsigned long l=t->low ;
	//rdtsc(l,h);
	t->high = h;
	t->low  = l;
#else	
	t->low  = clock();
	t->high = 0;
#endif	
	}}}

void Start_Timer (unsigned int idx) {{{
	if(idx>255) return;
	getTimeStampCounter(&start[idx]);
	}}}
void Print_Timer (unsigned int idx) {{{
	if(idx>255) return;
	printf("Timer %3i Anzahl %4i Schnitt %18llu\n",idx,count[idx],(summe[idx]/count[idx]));
	}}}
void Ende_Timer (unsigned int idx) {{{
	unsigned long long zeit1,zeit2;
	if(idx>255) return;
	getTimeStampCounter(&ende [idx]);
	zeit1=            start[idx].high;
	zeit1=(zeit1<<32)+start[idx].low ;
	zeit2=            ende[idx].high ;
	zeit2=(zeit2<<32)+ende[idx].low  ;
	zeit2-=zeit1;
	summe[idx]+=zeit2;
	count[idx]++;
	Print_Timer (idx);
	}}}

unsigned long long Get_Timestamp(void) {{{
  TimeStampCounter my_start;
  unsigned long long zeit;
  getTimeStampCounter (&my_start);
  zeit =                my_start.high;
  zeit = (zeit << 32) + my_start.low ;
  return zeit;
}}}

unsigned long long Guess_Mhz (void) {{{
static unsigned long long ret=0;
	unsigned long long zeit1,zeit2;
	return 1100000000;
	if (ret>0)
	  return ret;
	zeit1 = Get_Timestamp ();
	sleep(10);				/* time stamp don't count :-( */
	zeit2 = Get_Timestamp ();
	zeit2 = zeit2 - zeit1;
	ret = zeit2;
	return ret;
}}}

unsigned long long Get_Mhz (void) {{ {
#ifdef TIMER
static unsigned long long ret=0;
  char *my_buffer;
  FILE *cpuinfo;
  if (ret>0)
    return ret;
  my_buffer = (char*) xMalloc (512);
  if (my_buffer == NULL)
    return Guess_Mhz ();
  if ( (cpuinfo = fopen ("/proc/cpuinfo", "r")) == NULL )
    return Guess_Mhz ();
  while ( 0 < fgets (my_buffer, 511, cpuinfo))
    {
      my_buffer[511]=0;
      if (0 == strncmp(my_buffer, "cpu MHz",7))
  	{
	  char *colon = strchr(my_buffer,':');
	  if (colon != NULL)
	    {
	      ret = (unsigned long long)(atof (colon+2)*1000.0);
	      ret *= 1000;
	      break;
	    }
	}
    }
  fclose (cpuinfo);
  free (my_buffer);
  return ret;
#else
 return 1000000;	/* for clock */
#endif  
}}}



/*
 * vim600: fdm=marker
 */
