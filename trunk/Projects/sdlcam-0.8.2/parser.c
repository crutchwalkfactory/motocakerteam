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
#include <stdio.h>
#include <string.h>

#include "SDL_v4l_filters.h"
#include "SDL_v4l_filters_thomas.h"

/*
   Variable
   SRC		Quelle
//   DST		Ziel
   CAP  	Captued Ziel
   A0 - A9	Array for "TEMP" saving

   Funtion:
   AND		2 Bilder
   ANDN		2 Bilder
   OR		2 Bilder
   XOR		2 Bilder
   SUBB		2 Bilder
   SUBSB	2 Bilder
   SUBUSB	2 Bilder
   ADDB		2 Bilder
   ADDSB	2 Bilder
   ADDUSB	2 Bilder
   ABBDIFF	2 Bilder
   TRASH	1 Bild	 1 Zahl
   LOWPASS	1 Bild
   HIGHPASS	1 Bild
   GRAY1	1 Bild
   GRAY2	1 Bild
   BRIGHTCON	1 Bild	 1 Zahl	1 Zahl

   ANDi		1 Bild 	1 Zahl
   ANDNi	1 Bild 	1 Zahl
   ORi		1 Bild 	1 Zahl
   XORi		1 Bild 	1 Zahl
   SUBBi	1 Bild 	1 Zahl
   SUBSBi	1 Bild 	1 Zahl
   SUBUSBi	1 Bild 	1 Zahl
   ADDBi	1 Bild 	1 Zahl
   ADDSBi	1 Bild 	1 Zahl
   ADDUSBi	1 Bild 	1 Zahl

   */

/* {{{  Variable */
static int Variable(const char *var,unsigned int len) 
{
  if(strncmp(var,"SRC",len)==0) return  0;
//  if(strncmp(var,"DST",len)==0) return  1;
  if(strncmp(var,"CAP",len)==0) return  2;
  if(strncmp(var,"A0" ,len)==0) return  3;
  if(strncmp(var,"A1" ,len)==0) return  4;
  if(strncmp(var,"A2" ,len)==0) return  5;
  if(strncmp(var,"A3" ,len)==0) return  6;
  if(strncmp(var,"A4" ,len)==0) return  7;
  if(strncmp(var,"A5" ,len)==0) return  8;
  if(strncmp(var,"A6" ,len)==0) return  9;
  if(strncmp(var,"A7" ,len)==0) return 10;
  if(strncmp(var,"A8" ,len)==0) return 11;
  if(strncmp(var,"A9" ,len)==0) return 12;
  return -1;
}
/* }}} */

/* {{{ SDL_v4l_image */
static SDL_v4l_image *Var_Name(int i) 
{
  switch(i)
    {
      case  0: return source;
//      case  1: return "DST";
      case  2: return &background;
      case  3: return &(buffer[0]);
      case  4: return &(buffer[1]);
      case  5: return &(buffer[2]);
      case  6: return &(buffer[3]);
      case  7: return &(buffer[4]);
      case  8: return &(buffer[5]);
      case  9: return &(buffer[6]);
      case 10: return &(buffer[7]);
      case 11: return &(buffer[8]);
      case 12: return &(buffer[9]);
      default: return NULL;
    }
  return NULL;
}
/* }}} */

int Assign(char *cmd,char **ende);

/* {{{ Funtion */
static int Funtion(const char *var,unsigned int len,int *typ) 
{
  if(strncmp(var,"AND"		,len)==0) { *typ=0; return  0; }
  if(strncmp(var,"ANDN"		,len)==0) { *typ=0; return  1; }
  if(strncmp(var,"OR"		,len)==0) { *typ=0; return  2; }
  if(strncmp(var,"XOR"		,len)==0) { *typ=0; return  3; }
  if(strncmp(var,"SUBB"		,len)==0) { *typ=0; return  4; }
  if(strncmp(var,"SUBUSB"	,len)==0) { *typ=0; return  6; }
  if(strncmp(var,"ADDB"		,len)==0) { *typ=0; return  7; }
  if(strncmp(var,"ADDUSB"	,len)==0) { *typ=0; return  9; }
  if(strncmp(var,"ABSDIFF"	,len)==0) { *typ=0; return 10; }

  if(strncmp(var,"LOWPASS"	,len)==0) { *typ=2; return 12; }
  if(strncmp(var,"HIGHPASS"	,len)==0) { *typ=2; return 13; }
  if(strncmp(var,"GRAY1"	,len)==0) { *typ=2; return 14; }
  if(strncmp(var,"GRAY2"	,len)==0) { *typ=2; return 15; }
  if(strncmp(var,"BRIGHTCON"	,len)==0) { *typ=3; return 16; }

  if(strncmp(var,"TRASH"	,len)==0) { *typ=1; return 11; }
  if(strncmp(var,"ANDi"		,len)==0) { *typ=1; return 20; }
  if(strncmp(var,"ANDNi"	,len)==0) { *typ=1; return 21; }
  if(strncmp(var,"ORi"		,len)==0) { *typ=1; return 22; }
  if(strncmp(var,"XORi"		,len)==0) { *typ=1; return 23; }
  if(strncmp(var,"SUBBi"	,len)==0) { *typ=1; return 24; }
  if(strncmp(var,"SUBUSBi"	,len)==0) { *typ=1; return 26; }
  if(strncmp(var,"ADDBi"	,len)==0) { *typ=1; return 27; }
  if(strncmp(var,"ADDUSBi"	,len)==0) { *typ=1; return 29; }

  *typ=-1;
  return -1;
}
/* }}} */

static int Function_value(char *cmd,char **ende) {{{
  int ret;
  int sign=0;
  if(*cmd=='-')
    {
      sign=1;
      cmd++;
    }
  for (ret = 0; *cmd >= '0' && *cmd <= '9' && *cmd; cmd++)
    ret = ret * 10 + (*cmd - '0');
  *ende = cmd;
  if (sign) 
    ret=-ret;
  return ret;
}}}

static int Function_bb(int typ,char *cmd,char **ende,int dst) {{{
  int ARG1=Assign(cmd,ende);
  int ARG2=0;
  cmd=*ende;
  if(ARG1<0)
    return ARG1;
  cmd++;
  ARG2=Assign(cmd,ende);
  cmd=*ende;
  if(*cmd!=')')
    return -1;
  cmd++;
  *ende=cmd;
  switch (typ) 
    {
      case  0: Call_Filter_bbb ("generic-mmx-bbb", 1, Var_Name(ARG1), Var_Name(ARG2), Var_Name(dst)); return dst;
      case  1: Call_Filter_bbb ("generic-mmx-bbb", 2, Var_Name(ARG1), Var_Name(ARG2), Var_Name(dst)); return dst;
      case  3: Call_Filter_bbb ("generic-mmx-bbb", 3, Var_Name(ARG1), Var_Name(ARG2), Var_Name(dst)); return dst;
      case  2: Call_Filter_bbb ("generic-mmx-bbb", 4, Var_Name(ARG1), Var_Name(ARG2), Var_Name(dst)); return dst;
      case  4: Call_Filter_bbb ("generic-mmx-bbb", 5, Var_Name(ARG1), Var_Name(ARG2), Var_Name(dst)); return dst;
      case  7: Call_Filter_bbb ("generic-mmx-bbb", 6, Var_Name(ARG1), Var_Name(ARG2), Var_Name(dst)); return dst;
      case  6: Call_Filter_bbb ("generic-mmx-bbb", 7, Var_Name(ARG1), Var_Name(ARG2), Var_Name(dst)); return dst;
      case  9: Call_Filter_bbb ("generic-mmx-bbb", 8, Var_Name(ARG1), Var_Name(ARG2), Var_Name(dst)); return dst;
      case 10: Call_Filter_bbb ("abs-diff"       , 8, Var_Name(ARG1), Var_Name(ARG2), Var_Name(dst)); return dst;
      default: printf("?      (%i,%i,%i);\n",ARG1,ARG2,dst);
    }
  return -1;
}}}
static int Function_b(int typ,char *cmd,char **ende,int dst) {{{
  int ARG1=Assign(cmd,ende);
  cmd=*ende;
  if(ARG1<0)
    return ARG1;
  if(*cmd!=')')
    return -1;
  cmd++;
  *ende=cmd;
 switch (typ) 
    {
      case 12: Call_Filter_bb ("lowpass" , 1, Var_Name (ARG1), Var_Name (dst)); return dst;
      case 13: Call_Filter_bb ("highpass", 1, Var_Name (ARG1), Var_Name (dst)); return dst;
      case 14: Call_Filter_bb ("gray"	 , 1, Var_Name (ARG1), Var_Name (dst)); return dst;
      case 15: Call_Filter_bb ("gray"	 , 2, Var_Name (ARG1), Var_Name (dst)); return dst;
      default: printf("?      (%i);\n",ARG1);
    }
  return -1;
}}}
static int Function_bi(int typ,char *cmd,char **ende,int dst) {{{
  int ARG1=0;
  unsigned int ARG2=0;
  ARG1=Assign(cmd,ende);
  cmd=*ende;
  if(ARG1<0)
    return ARG1;
  if(*cmd!=',')
    return -1;
  cmd++;
  ARG2=Function_value(cmd,ende);
  *ende=cmd;
 if(*cmd!=')')
    return -1;
  cmd++;
  *ende=cmd;
  switch (typ) 
    {
      case 20: Call_Filter_bbv ("generic-mmx-bbb", 1, Var_Name(ARG1), Var_Name(dst), ARG2); return dst;
      case 21: Call_Filter_bbv ("generic-mmx-bbb", 2, Var_Name(ARG1), Var_Name(dst), ARG2); return dst;
      case 23: Call_Filter_bbv ("generic-mmx-bbb", 3, Var_Name(ARG1), Var_Name(dst), ARG2); return dst;
      case 22: Call_Filter_bbv ("generic-mmx-bbb", 4, Var_Name(ARG1), Var_Name(dst), ARG2); return dst;
      case 24: Call_Filter_bbv ("generic-mmx-bbb", 5, Var_Name(ARG1), Var_Name(dst), ARG2); return dst;
      case 27: Call_Filter_bbv ("generic-mmx-bbb", 6, Var_Name(ARG1), Var_Name(dst), ARG2); return dst;
      case 26: Call_Filter_bbv ("generic-mmx-bbb", 7, Var_Name(ARG1), Var_Name(dst), ARG2); return dst;
      case 29: Call_Filter_bbv ("generic-mmx-bbb", 8, Var_Name(ARG1), Var_Name(dst), ARG2); return dst;
      case 11: Call_Filter_bbv ("trashmask"	 , 1, Var_Name(ARG1), Var_Name(dst), ARG2); return dst;
      default: printf("?(%i,%i,%i);\n",ARG1,ARG2,dst); break;
    }
  return -1;
}}}
static int Function_bii(int typ,char *cmd,char **ende,int dst) {{{
  int ARG1=0;
  int ARG2=0;
  int ARG3=0;
  ARG1=Assign(cmd,ende);
  cmd=*ende;
  if(ARG1<0)
    return ARG1;
  if(*cmd!=',')
    return -1;
  cmd++;

  ARG2 = Function_value(cmd,ende);
  cmd=*ende;
  if(*cmd!=',')
    return -1;
  cmd++;

  ARG3 = Function_value(cmd,ende);
  cmd=*ende;
  if(*cmd!=')')
    return -1;
  cmd++;

  *ende=cmd;
  switch (typ) 
    {
      case 16: SDL_v4l_Filter_brightnes_contrast(Var_Name(ARG1),Var_Name(dst),ARG2,ARG3); return dst;
      default: printf("?(%i,%i,%i);\n",ARG1,ARG2,ARG3); break;
    }
  return -1;
}}}

int Assign(char *cmd,char **ende) 
{
  const char *start=cmd;
  int DST=0;
  int SRC=0;
  int typ;
  unsigned int len;
  char *ENDE;
  if (ende == NULL) ende=&ENDE;
  if (cmd == NULL) return 0;
  while( ((*cmd>='0'&&*cmd<='9')||(*cmd>='A'&&*cmd<='Z'))&&*cmd) cmd++;
  (*ende)=cmd;
  len = cmd-start;
  DST = Variable (start, len);
  if(DST<0)     return DST;	/* error */
  if(*cmd==';') return DST;
  if(*cmd!='=') return DST;
  start=++cmd;
  while( ((*cmd>='0'&&*cmd<='9')||(*cmd>='A'&&*cmd<='Z'))&&*cmd) cmd++;
  (*ende)=cmd;	    
  len = cmd-start;
  SRC = Variable(start, len);
  if (SRC >= 0)
    {
      memcpy (Var_Name(DST)->data, Var_Name(SRC)->data, source->len);
      return DST;
    }
  len = cmd - start;
  SRC = Funtion (start, len, &typ);
  if (SRC < 0) 
    return SRC;
  if (*cmd!='(') 
    return -1;
  start = ++cmd;
  switch (typ) 
    {
      case 0: return Function_bb  (SRC, cmd, ende, DST);
      case 1: return Function_bi  (SRC, cmd, ende, DST);
      case 2: return Function_b   (SRC, cmd, ende, DST);
      case 3: return Function_bii (SRC, cmd, ende, DST);
      default:printf ("Unkown Funktion\n");    break;
    }
  return -1;
}
/* 
 * vim600: fdm=marker 
 */
