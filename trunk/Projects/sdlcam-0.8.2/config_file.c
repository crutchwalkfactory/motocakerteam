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
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include <fcntl.h>
#include <wait.h>
#include <errno.h>
#include <string.h>

#include "SDL_v4l_filters.h"
#include "configuration.h"
#include "config_file.h"
#include "misc.h"
#include "dyn_misc.h"

/**
  * <LINE>	 ::= <ASSIGNMENT>?<SPACE>+<COMMENT>
  * <ASSIGNMENT> ::= <KEY><SPACE>=<SPACE><VALUE>
  * <KEY> 	 ::= (!isspace())+
  * <SPACE>	 ::= (isspace())*
  * <COMMENT	 :=  [^\n\r]*
  * <VALUE>	 ::= <NONWHITE>|<ESC_STRING>
  * <NONWHITE>	 ::= (!isspace())+
  * <ESC_STRING> ::= "[^\"]*"
  
 */
/* {{{ int Parse_Assignment (char *line, char **key, char **val)
 *
 * INPUT : an '\0' Terminated Line and 2 Point to String
 * OUTPUT: return 0/1 an Valid Assignment Found
 *	    key pointer to the start of KEY ( start of line first white is now '\0' )
 *	    val pointer to start of VALUE ( '\0' terminated )
 *	    val = NULL if no valid asignment found
 */
int Parse_Assignment (char *line, char **key, char **val) 
{
  int   esc=0;
  char *akt=line;
  *key=line;
  if (isspace(*akt) || *akt==0 )	
    return 1;		// Not Valid Line
  akt++;
  while (!isspace(*akt) && *akt!='=' && *akt!=0 ) akt++;	// Find the first SPACE or =
  if (*akt == 0)
    return -4;
  if (*akt!='=') 
    {
      *akt=0;
      akt++;
      while (*akt!='=' && *akt!=0 ) 
	akt++;		// Find =
      if (*akt!='=')		
	return -3;
    }
  *akt=0;
  akt++;
  while (isspace (*akt) && *akt != 0 ) 
    akt++;			// Skip the SPACE
  if (*akt==0)			
    return -2;
  if (*akt=='"') 
    { 
      esc=1; 
      akt++; 
    }
  *val=akt;
  while ((!isspace (*akt) || esc) && ( !esc || *akt!='"' ) && *akt!=0 ) 
    akt++;
  if (esc && *akt != '"')
    return (-1);
  *akt=0;
  return (0);
}
/* }}} */

/*
** Original version by Thomas Lussnig <thomas@bewegungsmelder.de>
*/
int find_and_parse_config_file (void)
{
  char buf[512];
  FILE *fp = fopen ("SDLcam.cfg","r");
  int line=0;
  if (fp == NULL)
    {
      snprintf(buf,511,"%s/%s",PKGDATADIR,"SDLcam.cfg");
      fp = fopen (buf, "r");
      if (fp == NULL)
	return (0);
    }
  while (NULL != fgets (buf, sizeof (buf), fp))
    {
      char *key,*val;
      int  found;
      line++;
      if ((found = Parse_Assignment (buf,&key,&val))) {
	if (found < 0)
	  fprintf (stderr,"Error (%i) on line %d in configure file: %s\n",found,line,buf);
	continue;
      	}
      if (strcasecmp (key, "DUMP_PATH") == 0) { Dump_Path_set (val); continue; }
      if (strcasecmp (key, "FONT1"   ) == 0) { if (NULL != configuration.FONT1    ) free(configuration.FONT1    ); configuration.FONT1     = xStrdup (val); continue;}
      if (strcasecmp (key, "FONT2"   ) == 0) { if (NULL != configuration.FONT2    ) free(configuration.FONT2    ); configuration.FONT2     = xStrdup (val); continue;}
      if (strcasecmp (key, "FONT3"   ) == 0) { if (NULL != configuration.FONT3    ) free(configuration.FONT3    ); configuration.FONT3     = xStrdup (val); continue;}
/* {{{ ftp block */      
      if (strcasecmp (key, "FTP_HOST") == 0) { if (NULL != configuration.ftp_host ) free(configuration.ftp_host ); configuration.ftp_host  = xStrdup (val); continue;}
      if (strcasecmp (key, "FTP_USER") == 0) { if (NULL != configuration.ftp_user ) free(configuration.ftp_user ); configuration.ftp_user  = xStrdup (val); continue;}
      if (strcasecmp (key, "FTP_PASS") == 0) { if (NULL != configuration.ftp_pass ) free(configuration.ftp_pass ); configuration.ftp_pass  = xStrdup (val); continue;}
      if (strcasecmp (key, "FTP_TEMP") == 0) { if (NULL != configuration.ftp_temp ) free(configuration.ftp_temp ); configuration.ftp_temp  = xStrdup (val); continue;}
      if (strcasecmp (key, "FTP_FILE") == 0) { if (NULL != configuration.ftp_file ) free(configuration.ftp_file ); configuration.ftp_file  = xStrdup (val); continue;}
/* }}} */
      if (strcasecmp (key, "DEBUG"     		) == 0) { configuration.debug        = atoi (val); continue; }
/* {{{ dump control */      
      if (strcasecmp (key, "DUMP_MODE" 		) == 0) { configuration.dump_mode    = atoi (val); continue; }
      if (strcasecmp (key, "DUMP_QUALITY" 	) == 0) { configuration.dump_quality = atoi (val); continue; }
      if (strcasecmp (key, "DUMP_SPEED" 	) == 0) { configuration.dump_speed = atoi (val); continue; }
      if (strcasecmp (key, "DUMP_COUNT" 	) == 0) { configuration.dump_count = atoi (val); continue; }
/* }}} */

/* {{{ v4l block */
      if (strcasecmp (key, "V4L_FPS"   		) == 0) { configuration.v4l_fps      = atoi (val); continue; }
/* }}} */      
/* {{{ filter block */      
      if (strcasecmp (key, "FILTER_INVERT" 	) == 0) { if (atoi (val)) TOGGLE_FILTER (invert	 ); continue; }
      if (strcasecmp (key, "FILTER_NOISE" 	) == 0) { if (atoi (val)) TOGGLE_FILTER (average ); continue; }
      if (strcasecmp (key, "FILTER_USER") == 0) {{{
	  if (NULL != g_filter_stats.userdefined ) 
	    free (g_filter_stats.userdefined); 
	  g_filter_stats.userdefined  = xStrdup (val); 
	  continue;
	}}}
      if (strcasecmp (key, "FILTER_GLAS" 	) == 0) { if (atoi (val)) TOGGLE_FILTER (reorder_typ); continue; }
      if (strcasecmp (key, "FILTER_reorder_typ"	  ) == 0) { g_filter_stats.reorder_typ    = atoi (val); continue; }
      if (strcasecmp (key, "FILTER_reorder_radius") == 0) { g_filter_stats.reorder_radius = atoi (val); continue; }
      if (strcasecmp (key, "FILTER_reorder_effekt") == 0) { g_filter_stats.reorder_effekt = atof (val); continue; }
      if (strcasecmp (key, "AVERAGE_MAX_LB"	) == 0) { g_filter_stats.average_max_lb = atoi (val); continue; }
      if (0 == strncasecmp("FILTER_", key, 7)) {
	char *filter = strchr(key, '_');
	Mode_Filter_set (filter+1, atoi (val));
      	}
/* }}} */
/* {{{ logo block */      
      if (strcasecmp (key, "LOGO_PATH"   ) == 0) 
        { 
	  if (NULL != configuration.powered_by_image_path) 
	    free(configuration.powered_by_image_path); 
	  configuration.powered_by_image_path = xStrdup(val); 
	  continue; 
	}
      if (strcasecmp (key, "LOGO_POS"    ) == 0) 
        {
      	  configuration.powered_by_blitpos = atoi (val);
	  if (  !((configuration.powered_by_blitpos>=0)&&(configuration.powered_by_blitpos<=5)) )
	    {
	      fprintf (stderr, "Warning: LOGO_POS value in config file (line %d)is out of range ( !(>=0 || <=5) )\n",line);
	      configuration.powered_by_blitpos=0;
	    }
	  continue;
        }
/* }}} */      
    }
  fclose(fp);
  return (0);
}
/*
 * vim600: fdm=marker
 */

