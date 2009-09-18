#include "SDL_v4l_filters.h"
#include "SDL_v4l_image.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>

#include "misc.h"
#ifdef __cplusplus
extern "C" {
#endif


unsigned int  Level_R[256];
unsigned int  Level_G[256];
unsigned int  Level_B[256];
unsigned int  Level_A[256];	// stand for average

unsigned int  Level_med;	// Durchnitshelligkeit
unsigned int  Level_avg;	// Durchnitliche Anzahl
unsigned char pallete_r[256]; // von range und brightness_contrast genutzt
unsigned char pallete_g[256];
unsigned char pallete_b[256];

char *dump_path   = NULL;
char *dump_prefix = NULL;
const char *Dump_Path   (void) { return dump_path; }
const char *Dump_Prefix (void) { return dump_prefix; }

void Dump_Path_set   (const char *c) { if (NULL != dump_path  ) free (dump_path  ); dump_path   = xStrdup (c); }
void Dump_Prefix_set (const char *c) { if (NULL != dump_prefix) free (dump_prefix); dump_prefix = xStrdup (c); }

void *xMalloc (size_t s) {{{
  void *ret = malloc (s);
  if (ret != NULL)
    return ret;
  printf ("malloc (%i) FAILD", s);
 // exit (-1);
  return NULL; /* prevents warnings ;) */
}}}
void *xRealloc (void *p,size_t s) {{{
  void *ret = realloc (p,s);
  if (ret != NULL)
    return ret;
  printf ("realloc (%i) FAILD", s);
  exit (-1);
  return NULL; /* prevents warnings ;) */
}}}
int SDL_v4l_Buffer_Prepare (const SDL_v4l_image *SRC, SDL_v4l_image *DST) {{{
/*    
 * Make sure that Height,Withd,Len are equal 
 * and that there is the needed space for data
 *
 * if not free the old space and alloc new one
 */     
  int ret = 0;
  if (SRC == DST)
    return 0;
  if (SRC->len != DST->len || DST->data == NULL)
    {
	if (DST->data != NULL) free (DST->data);
      DST->data = (unsigned char*)xMalloc (SRC->len);
      ret = 1;
    }
  DST->len   = SRC->len  ;
  DST->w     = SRC->w    ;
  DST->h     = SRC->h    ;
  DST->pitch = SRC->pitch;
  return ret;
}}}
void Get_Level(const SDL_v4l_image *SRC) {{{
  const unsigned char *img = SRC->data;
  unsigned int len = SRC->len;
  /* Gather statistical information*/	
  memset(Level_R, 0, sizeof(Level_R));
  memset(Level_G, 0, sizeof(Level_G));
  memset(Level_B, 0, sizeof(Level_B));
  memset(Level_A, 0, sizeof(Level_A));
  len >>= 2;
  while(len--) {{{
	unsigned int avg;
	avg  = *img;
	Level_R[*img++]++;
	avg += *img;
	Level_G[*img++]++;
	avg += *img;
	Level_B[*img++]++;
	Level_A[avg/3]++;
	img++;
    }}}
 /* Calculate the average value */
  Level_med = 0;
  Level_avg = 0;  
  len=255;
  do {
	  Level_avg += Level_R[len];
	  Level_avg += Level_G[len];
	  Level_avg += Level_B[len];
	  Level_med += (Level_R[len]+Level_G[len]+Level_B[len])*len;
    } while(--len);
  if (Level_avg != 0) // Occur on an BLACK Image (static pic with dark)
    Level_med /= Level_avg;
  Level_avg /= (256 * 3);	// = W*H / 256
}}}

Filter_plugin *Find_Filter (const char *name) {{{
	Filter_plugin *akt = g_filter_stats.plugin;
	if (name == NULL) return NULL;
	while (akt != NULL && 0 != strcasecmp(name, akt->name)) akt = akt->next;
	return akt;
}}}
int Call_Filter_bb  (const char *name, int mode, const SDL_v4l_image *SRC,                       SDL_v4l_image *DST){{{
  Filter_plugin *filter = Find_Filter (name);
  if (NULL == filter   ) return 0;			/* no such filter found  		*/
  if (filter->type != 0) return 0;			/* filter has wrong type 		*/
  if (mode == -1) mode = filter->mode;		/* Use the "toggeld" mode		*/
  if (mode ==  0) return 0;					/* This filter is off			*/
  return filter->Apply.BB (mode, SRC, DST);	/* Apply the selected filter	*/
}}}
int Call_Filter_b   (const char *name, int mode,                                               SDL_v4l_image *DST){{{
  Filter_plugin *filter = Find_Filter (name);
  if (NULL == filter   ) return 0;			/* no such filter found  		*/
  if (filter->type != 1) return 0;			/* filter has wrong type 		*/
  if (mode == -1) mode = filter->mode;		/* Use the "toggeld" mode		*/
  if (mode ==  0) return 0;					/* This filter is off			*/
  return filter->Apply.B (mode, DST);
}}}
int Call_Filter_bbb (const char *name, int mode, const SDL_v4l_image *A, const SDL_v4l_image *B, SDL_v4l_image *DST){{{
  Filter_plugin *filter = Find_Filter (name);
  if (NULL == filter   ) return 0;			/* no such filter found  		*/
  if (filter->type != 2) return 0;			/* filter has wrong type 		*/
  if (mode == -1) mode = filter->mode;		/* Use the "toggeld" mode		*/
  if (mode ==  0) return 0;					/* This filter is off			*/
  return filter->Apply.BBB (mode, A, B, DST);
}}}
int Call_Filter_bbv (const char *name, int mode, const SDL_v4l_image *A, SDL_v4l_image *DST, unsigned int val){{{
  Filter_plugin *filter = Find_Filter (name);
  if (NULL == filter   ) return 0;			/* no such filter found  		*/
  if (filter->type < 3 || filter->type > 4) return 0;			/* filter has wrong type 		*/
  if (mode == -1) mode = filter->mode;		/* Use the "toggeld" mode		*/
  if (mode ==  0) return 0;					/* This filter is off			*/
  return filter->Apply.BBv (mode, A, DST, val);
}}}
int Toggle_Filter   (const char *name) {{{
  Filter_plugin *filter = Find_Filter (name);
  if (NULL == filter   ) return 0;			/* no such filter found  		*/
  if (filter->Toggle != NULL) return filter->mode = filter->Toggle ();
  return filter->mode = !filter->mode;
}}}
int Mode_Filter     (const char *name) {{{
  Filter_plugin *filter = Find_Filter (name);
  if (name == NULL) return 0;
  if (filter != NULL) return filter->mode;
  if(strcmp("average"	 ,name)==0) return GET_FILTER (average        );
  if(strcmp("invert"	 ,name)==0) return GET_FILTER (invert         );
  if(strcmp("userfilter" ,name)==0) return GET_FILTER (user           );
  if(strcmp("rgb-bgr"	 ,name)==0) return GET_FILTER (rgb2bgr        );
  return 0;
}}}

int displayErrno (const char *filename) 
{
	switch (errno) 
	{
		case EACCES : fprintf (stderr, "Dump to %s failed. Access denied!\n"			, filename); return 0;
		case ENOTDIR: fprintf (stderr, "Dump to %s failed. Part of Direcory is no directory!\n" , filename); return 1;
		case EROFS  : fprintf (stderr, "Dump to %s failed. Read only filesystem!\n"		, filename); return 1;
		case EFAULT : fprintf (stderr, "Dump to %s failed. Invalid pathname!\n"			, filename); return 1;
		case ELOOP  : fprintf (stderr, "Dump to %s failed. Symlink loop!\n"			, filename); return 0;
		case ENOMEM : fprintf (stderr, "Dump to %s failed. Kernel memory problem!\n"		, filename); return 0;
		case EMFILE : fprintf (stderr, "Dump to %s failed. To many open file's (bug-report)\n"	, filename); return 1;
		case ENFILE : fprintf (stderr, "Dump to %s failed. To many open file's!\n"		, filename); return 1;
	}
	return 0;
}

char *getNewFilename(const char *path, const char *prepend, const char *append) {{{
	/* Build a filename like this: path/prepend00005append */
	char found=0;
	/* do not try previus used filenames from the same session */
	static	int capture_number=0;
	FILE *tmp=NULL;
	char *filename;
	filename = (char*)xMalloc (strlen(path)+strlen(prepend)+5+strlen(append)+1);
	if (filename ==NULL)
	  return NULL;			// Malloc error

	while(!found)
	{
		/* safe that it can't get too big */
		sprintf(filename, "%s/%s%05d%s", path,prepend,capture_number,append);
		tmp = fopen(filename, "r");
		// The fopen function may also fail and set errno for any of the errors specified for the routine open(2).
		if (tmp==NULL) 
		{
			/* Good, the file does not exist. But do we have write permission in
			 * this path??? */
			tmp = fopen(filename, "w+");
			if (tmp==NULL) { 
				displayErrno (filename);
				return NULL; 
				} // No, we dont

			/* We have write permission! */
			fclose(tmp);
			unlink(filename); // clean up
						
	        	found=1;
		}
		else
		{
			if(displayErrno (filename)) return NULL;
			fclose(tmp);
			capture_number++;
			if (100000 <= capture_number)
			{
				/* if imposible to get an file name return */  
				printf("No filename found\n");
				free(filename);
				return NULL;
			}
		}
	}
	++capture_number;	// Do not try this nummbr again in this session
	return filename;
	}}}
#ifdef __cplusplus
	}
#endif

/*
 * vim600: fdm=marker
 */
