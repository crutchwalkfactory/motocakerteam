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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "SDL_v4l.h"
#include "xml_theme.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "SDL_image.h"
#include "SDL_keysym.h"
#include "configuration.h"
#include "interface.h"
#include "SDL_ttf.h"
#include "webcam.h"
#include "SDL_v4l_filters.h"
#include "SDL_v4l_filters_thomas.h"
#include "dyn_misc.h"
#include "main.h"
#include "misc.h"
#include "capture.h"
#include "init.h"

int SIDE_BAR_X 	 =    0;
int SIDE_BAR_W 	 =  100;
int SIDE_BAR_Y 	 =    0;
int SIDE_BAR_H 	 =  350;
int BOTTOM_BAR_X =  100;
int BOTTOM_BAR_W =  325;
int BOTTOM_BAR_Y = -120;
int BOTTOM_BAR_H =  120;
int IMAGE_X 	 =  100;
int IMAGE_Y 	 =    0;

/* {{{ preset colors */
SDL_Color yellow = { 0xFF, 0xFF, 0x00, 0 };
SDL_Color white  = { 0xFF, 0xFF, 0xFF, 0 };
SDL_Color green  = { 0x00, 0xFF, 0x00, 0 };
SDL_Color red    = { 0xFF, 0x00, 0x00, 0 };
SDL_Color black  = { 0x00, 0x00, 0x00, 0 };
SDL_Color gray   = { 0xCC, 0xCC, 0xCC, 0 };
SDL_Color blue   = { 0x00, 0x00, 0xFF, 0 };
/* }}} */

#define CHANGE_STEP 3000
static int tab_nr = 1;
static int shutter_value = -1;
TTF_Font *font1;
TTF_Font *font2;
TTF_Font *font3;
int	captured_size = -1;
my_SDL_Key_Event *key_Root 	= NULL;
int line2_space=-12;
int mouse_img_x = 0;
int mouse_img_y = 0;
/* {{{ Gui Events */
my_SDL_Rect  white_m		= {{0,0,0,0}, 1, NULL  		, "white/"	, NULL, -1, NULL, NULL, NULL, -1};
my_SDL_Rect  red_p		= {{0,0,0,0}, 1, &white_m	, "red+"	, NULL, -1, NULL, NULL, NULL, -1};
my_SDL_Rect  red_m		= {{0,0,0,0}, 1, &red_p		, "red-"	, NULL, -1, NULL, NULL, NULL, -1};
my_SDL_Rect  blue_p		= {{0,0,0,0}, 1, &red_m		, "blue+"	, NULL, -1, NULL, NULL, NULL, -1};
my_SDL_Rect  blue_m		= {{0,0,0,0}, 1, &blue_p	, "blue-"	, NULL, -1, NULL, NULL, NULL, -1};
my_SDL_Rect  dump_m		= {{0,0,0,0}, 1, &blue_m	, "intervall"	, NULL, -1, NULL, NULL, NULL, -1};
my_SDL_Rect  interval_p		= {{0,0,0,0}, 1, &dump_m	, "intervall+"	, NULL, -1, NULL, NULL, NULL, -1};
my_SDL_Rect  interval_m		= {{0,0,0,0}, 1, &interval_p	, "intervall-"	, NULL, -1, NULL, NULL, NULL, -1};
my_SDL_Rect  qual_p		= {{0,0,0,0}, 1, &interval_m	, "qual+"	, NULL, -1, NULL, NULL, NULL, -1};
my_SDL_Rect  qual_m		= {{0,0,0,0}, 1, &qual_p	, "qual-"	, NULL, -1, NULL, NULL, NULL, -1};
my_SDL_Rect  radius_p		= {{0,0,0,0}, 1, &qual_m	, "radius+"	, NULL, -1, NULL, NULL, NULL, -1};
my_SDL_Rect  radius_m		= {{0,0,0,0}, 1, &radius_p	, "radius-"	, NULL, -1, NULL, NULL, NULL, -1};
my_SDL_Rect  effect_p		= {{0,0,0,0}, 1, &radius_m	, "effect+"	, NULL, -1, NULL, NULL, NULL, -1};
my_SDL_Rect  effect_m		= {{0,0,0,0}, 1, &effect_p	, "effect-"	, NULL, -1, NULL, NULL, NULL, -1};
my_SDL_Rect  shutter_p  	= {{0,0,0,0}, 1, &effect_m	, "shutter-"	, NULL, -1, NULL, NULL, NULL, -1};
my_SDL_Rect  shutter_m  	= {{0,0,0,0}, 1, &shutter_p	, "shutter-"	, NULL, -1, NULL, NULL, NULL, -1};
/* }}} */
my_SDL_Rect *event_root	= &shutter_m;

/* {{{ Handle value changes */
static void Fps_p (void) {{{
  int temp=0;
  switch (v4l[v4l_akt]->Parameter ("fps")) {
  case  4: temp= 5; break;
  case  5: temp= 8; break;
  case  8: temp=10; break;
  case 10: temp=12; break;
  case 12: temp=15; break;
  case 15: temp=20; break;
  case 20: temp=24; break;
  case 24: temp=25; break;
  case 25: temp=30; break;
  default: return;
	}
  v4l[v4l_akt]->Parameter ("fps", temp);
  return;
}}}
static void Fps_m (void) {{{
  int temp=0;
  switch(v4l[v4l_akt]->Parameter ("fps")) {
  case  5: temp= 4; break;
  case  8: temp= 5; break;
  case 10: temp= 8; break;
  case 12: temp=10; break;
  case 15: temp=12; break;
  case 20: temp=15; break;
  case 24: temp=20; break;
  case 25: temp=24; break;
  case 30: temp=25; break;
  default: return;
  	}
  v4l[v4l_akt]->Parameter ("fps", temp);
  return;
}}}
static void Shutter_l (void) {{{
  if (shutter_value < 0)
      shutter_value = (65535 / v4l[v4l_akt]->Parameter ("fps")) - 1;
  else
      shutter_value = -shutter_value;
  v4l[v4l_akt]->Parameter ("shutter", shutter_value);
  return;
}}}
static void Shutter_p (void) {{{
  if (shutter_value + CHANGE_STEP > (65535 / v4l[v4l_akt]->Parameter ("fps")) - 1)
	shutter_value = (65535 / v4l[v4l_akt]->Parameter ("fps")) - 1;
  else 	shutter_value += CHANGE_STEP;
  v4l[v4l_akt]->Parameter ("shutter", shutter_value);
}}}
static void Shutter_m (void) {{{
  if (shutter_value - CHANGE_STEP < 0)
    	shutter_value = 0;
  else 	shutter_value -= CHANGE_STEP;
  v4l[v4l_akt]->Parameter ("shutter", shutter_value);
}}}
int Value_set (const char *name, const char *value) {{{
	int val;
	if (name == NULL) return -1000000;
	val = v4l[v4l_akt]->Parameter (name, atoi (value));
	if (val != -1000000) return val;
	return -1000000;
	}}}
int Value_get (const char *name) {{{
	int val;
	if (NULL == name) return -1000000;
	val = v4l[v4l_akt]->Parameter (name);
	if (val != -1000000) return val;
	if (0 == strcmp ("cur-fps"	, name)) return (cnt_fps>0) ? sum_fps / cnt_fps : -1;
	return  -1000000;
	}}}
void Set_Resolution  (const char *res) {{{
  	char *w;
  	if (res == NULL) return;
  	w = strchr (res, 'x');
  	if (w == NULL) return;
	v4l[v4l_akt]->Resolution(atoi (res), atoi (w + 1));
//  	V4L_resolution_set (atoi (res), atoi (w + 1));
	}}}
/* }}} */
static inputBox box;

int image_x = 0;
int image_y = 0;
static int bottom_bar_x = 0;
static int bottom_bar_y = 0;
static int side_bar_x = 0;
static int side_bar_y = 0;

static int Tab_offset_x (int tab) {{{
	if (tab>=0) return bottom_bar_x;
	else	    return side_bar_x;
	}}}
static int Tab_offset_y (int tab) {{{
	if (tab>=0) return bottom_bar_y;
	else	    return side_bar_y;
	}}}

int Show_Input_Box(int x,int y, const char *title, const char *value) {{{
	char BUF[64];
	int val = Value_get (value);
	if (val == -1000000) return 0;
	box.show = 1;
	box.x = x;
	box.y = y;
	if (box.title!=NULL) free (box.title);
	if (box.value!=NULL) free (box.value);
	if (box.name !=NULL) free (box.name );
	box.title = xStrdup(title);	
	box.name  = xStrdup(value);
	snprintf (BUF, 64, "%i", val);
	box.value = xStrdup (BUF);
	SDL_EnableUNICODE (1);
	return 1;
	}}}

void Name_Event (const char *name) {{{
	int len = strlen (name);
	int inc,dec,val,mode;
	int input;
	if (name == NULL) return;
  	if (0 == strcmp (name, "white/"	)) {{{
  		int temp = v4l[v4l_akt]->Parameter ("white_m") + 1;
		v4l[v4l_akt]->Parameter ("white_m", temp%5);

		return;
 		}}}
  	if (0 == strcmp (name, "fps+")) { Fps_p(); return; }
  	if (0 == strcmp (name, "fps-")) { Fps_m(); return; }

	dec  = name[len-1]=='-';
	inc  = name[len-1]=='+';
	val  = name[len-1]=='#';
	mode = name[len-1]=='/';
	if (inc || dec || val || mode) {{{
		char *tmp = (char*) xStrdup (name);	
		input = v4l[v4l_akt]->Parameter (tmp);
		free (tmp);
		if (input!= -1000000) {
			if (inc ) { v4l[v4l_akt]->Parameter (name, input+2049); return; } // + 8*256 +1; lower byte for single step values higher for lage values :-)
			if (dec ) { v4l[v4l_akt]->Parameter (name, input-2049); return; } // - 8*256 -1;
			if (val ) { Show_Input_Box (Tab_offset_x (0) + 5, Tab_offset_y (0), name, name); return; }
			if (mode) { v4l[v4l_akt]->Parameter (name, -input); return; }
			}
		}}}

  if (0 == strcmp (name, "capture"	)) {{{
      if (configuration.dump_speed > 0)
        if(configuration.dump_count<=0)	configuration.dump_count = 100000;
        else				configuration.dump_count = 0;
       else 				configuration.dump_count = 1;
      configuration.dump_next = 0;
      return;
    }}}
  if (0 == strcmp (name, "debug"	)) {{{
  	configuration.debug = ! configuration.debug;
	if (configuration.debug) { printf("Debug messages enabled\n"); } else { printf("Debug Messages disabled\n"); }
	return;
  }}}
  if (0 == strcmp (name, "background"	)) {{{
  	background_capture=1;
	return;
  }}}
  if (0 == strcmp (name, "exit"	)) {{{
	return Exit();
  }}}
  if (0 == strcmp (name, "full"		)) { toggle_fullscreen ( ); return ;}
  if (0 == strcmp (name, "mini"		)) { toggle_mini_mode(); return; }
  if (0 == strcmp (name, "halfsize"	)) {{{
	Toggle_Filter ("halfsize");
	Toggle_Filter ("mmx_halfsize");
	return;
  }}}
/* {{{ resolution */  
  if (0 == strcmp (name, "sqcif"	)) {v4l[v4l_akt]->Resolution (128,  96); return;}
  if (0 == strcmp (name, "qsif" 	)) {v4l[v4l_akt]->Resolution (160, 120); return;}
  if (0 == strcmp (name, "qcif" 	)) {v4l[v4l_akt]->Resolution (176, 144); return;}
  if (0 == strcmp (name, "sif"		)) {v4l[v4l_akt]->Resolution (320, 240); return;}
  if (0 == strcmp (name, "cif"		)) {v4l[v4l_akt]->Resolution (352, 288); return;} 
  if (0 == strcmp (name, "vga"		)) {v4l[v4l_akt]->Resolution (640, 480); return;}
/* }}} */  
/* {{{ not module filter */
  if (0 == strcmp (name, "average"	)) { TOGGLE_FILTER  (average  		);return;}
  //if (0 == strcmp (name, "rgb2bgr"	)) { TOGGLE_FILTER  (rgb2bgr   		);return;}
  if (0 == strcmp (name, "invert"	)) { TOGGLE_FILTER  (invert   		);return;}
  if (0 == strcmp (name, "user"		)) { TOGGLE_FILTER  (user     		);return;}
  if (0 == strcmp (name, "reorder"	)) { TOGGLE_REORDER ( 			);return;}
/* }}} */
/* {{{ + / - */
    if (0 == strcmp (name, "red+"	)) {{{
  	int r = v4l[v4l_akt]->Parameter ("white_r") + CHANGE_STEP;
	v4l[v4l_akt]->Parameter ("white_r", (r>65535)?65535:r);
	return;
 	}}}
  if (0 == strcmp (name, "red-"	)) {{{
	int r = v4l[v4l_akt]->Parameter ("white_r") - CHANGE_STEP;
	v4l[v4l_akt]->Parameter ("white_r", (r<0)?0:r);
	return;
 	}}}
  if (0 == strcmp (name, "blue+")) {{{
  	int b = v4l[v4l_akt]->Parameter ("white_b") + CHANGE_STEP;
	v4l[v4l_akt]->Parameter ("white_b", (b>65535)?65535:b);
	return;
 	}}}
  if (0 == strcmp (name, "blue-")) {{{
	int b = v4l[v4l_akt]->Parameter ("white_b") - CHANGE_STEP;
	v4l[v4l_akt]->Parameter ("white_b", (b<0)?0:b);
	return;
 	}}}
  if (0 == strcmp (name, "intervall+"		)) {{{
	  configuration.dump_speed++;
	  return;
	}}}
  if (0 == strcmp (name, "intervall-"		)) {{{
	if(configuration.dump_speed>0)
		configuration.dump_speed--;
	return;
 	}}}
  if (0 == strcmp (name, "quall+"		)) {{{
  	if(configuration.dump_quality<100)
    		configuration.dump_quality++;
	  return;
	}}}
  if (0 == strcmp (name, "quall-"		)) {{{
  	if(configuration.dump_quality>  1)
    		configuration.dump_quality--;
	return;
 	}}}
  if (0 == strcmp (name, "radius+"	)) {{{
  	if(g_filter_stats.reorder_radius<50) g_filter_stats.reorder_radius++;
	return;
  	}}}
  if (0 == strcmp (name, "radius-"	)) {{{
  	if(g_filter_stats.reorder_radius>2) g_filter_stats.reorder_radius--;
	}}}
  if (0 == strcmp (name, "effect+"	)) {{{
  	if(g_filter_stats.reorder_effekt<1.0) g_filter_stats.reorder_effekt+=0.1;
	return;
	}}}
  if (0 == strcmp (name, "effect-"	)) {{{
  	if(g_filter_stats.reorder_effekt>-1.0) g_filter_stats.reorder_effekt-=0.1;
	return;
 	}}}
  if (0 == strcmp (name, "agc/"       )) { v4l[v4l_akt]->Parameter ("agc"       ,-v4l[v4l_akt]->Parameter ("agc"       )              ); }
  if (0 == strcmp (name, "agc+"       )) { v4l[v4l_akt]->Parameter ("agc"       , v4l[v4l_akt]->Parameter ("agc"       ) + CHANGE_STEP); }
  if (0 == strcmp (name, "agc-"       )) { v4l[v4l_akt]->Parameter ("agc"       , v4l[v4l_akt]->Parameter ("agc"       ) - CHANGE_STEP); }
  if (0 == strcmp (name, "contrast/"  )) { v4l[v4l_akt]->Parameter ("contrast"  ,-v4l[v4l_akt]->Parameter ("contrast"  )              ); }
  if (0 == strcmp (name, "contrast+"  )) { v4l[v4l_akt]->Parameter ("contrast"  , v4l[v4l_akt]->Parameter ("contrast"  ) + CHANGE_STEP); }
  if (0 == strcmp (name, "contrast-"  )) { v4l[v4l_akt]->Parameter ("contrast"  , v4l[v4l_akt]->Parameter ("contrast"  ) - CHANGE_STEP); }
  if (0 == strcmp (name, "brightness/")) { v4l[v4l_akt]->Parameter ("brightness",-v4l[v4l_akt]->Parameter ("brightness")              ); }
  if (0 == strcmp (name, "brightness+")) { v4l[v4l_akt]->Parameter ("brightness", v4l[v4l_akt]->Parameter ("brightness") + CHANGE_STEP); }
  if (0 == strcmp (name, "brightness-")) { v4l[v4l_akt]->Parameter ("brightness", v4l[v4l_akt]->Parameter ("brightness") - CHANGE_STEP); }
  if (0 == strcmp (name, "gamma/"     )) { v4l[v4l_akt]->Parameter ("gamma"     ,-v4l[v4l_akt]->Parameter ("gamma"     )              ); }
  if (0 == strcmp (name, "gamma+"     )) { v4l[v4l_akt]->Parameter ("gamma"     , v4l[v4l_akt]->Parameter ("gamma"     ) + CHANGE_STEP); }
  if (0 == strcmp (name, "gamma-"     )) { v4l[v4l_akt]->Parameter ("gamma"     , v4l[v4l_akt]->Parameter ("gamma"     ) - CHANGE_STEP); }

  /* }}} */
  if (0 == strcmp (name, "freeze"	)) { TOGGLE_FILTER (freeze   ); return; }
  if (0 == strcmp (name, "background"	)) { background_capture=1; return; }
  if (0 == strcmp (name, "full"		)) { toggle_fullscreen(); return; }
  if (0 == strcmp (name, "mini" 	)) { toggle_mini_mode (); return; }
  if (0 == strcmp (name, "schutter/" 	)) return Shutter_l();
  if (0 == strcmp (name, "schutter+" 	)) return Shutter_p();
  if (0 == strcmp (name, "schutter-" 	)) return Shutter_m();

  if (0 == strcmp (name, "dump"		)) {{{
  	if(configuration.debug>0) printf("Cycle Capture Mode\n");
  	configuration.dump_mode=(configuration.dump_mode+1)%3;
  	return;
  	}}}
  Toggle_Filter (name);
}}}

void Hotkey_clear(void) {{{
	my_SDL_Key_Event *tmp;
	while (key_Root != NULL) {
		tmp = key_Root;
		key_Root = tmp->next;
		free (tmp->name);
		free (tmp);
		}
	}}}
void Hotkey_adding (SDLKey key,const char *name, int type) {{{
	my_SDL_Key_Event *tmp = (my_SDL_Key_Event *)xMalloc (sizeof (my_SDL_Key_Event));
	tmp->next = key_Root;
	tmp->k    = key;
	tmp->name = xStrdup(name);
	tmp->type= type;
	key_Root  = tmp;
	}}}
void Hotkey_add    (const char *key,const char *name,int type) {{{ // CHANGE TO ARRAY
	if (0==strcmp("SDLK_RETURN"	, key)) { Hotkey_adding (SDLK_RETURN	, name, type); return; }
	if (0==strcmp("SDLK_d"		, key)) { Hotkey_adding (SDLK_d		, name, type); return; }
	if (0==strcmp("SDLK_SPACE"	, key)) { Hotkey_adding (SDLK_SPACE	, name, type); return; }
	if (0==strcmp("SDLK_ESCAPE"	, key)) { Hotkey_adding (SDLK_ESCAPE	, name, type); return; }
	if (0==strcmp("SDLK_m"	       	, key)) { Hotkey_adding (SDLK_m		, name, type); return; }
	if (0==strcmp("SDLK_r"	       	, key)) { Hotkey_adding (SDLK_r		, name, type); return; }
	if (0==strcmp("SDLK_x"	       	, key)) { Hotkey_adding (SDLK_x		, name, type); return; }
	if (0==strcmp("SDLK_y"	       	, key)) { Hotkey_adding (SDLK_y		, name, type); return; }
	if (0==strcmp("SDLK_g"	       	, key)) { Hotkey_adding (SDLK_g		, name, type); return; }
	if (0==strcmp("SDLK_s"	       	, key)) { Hotkey_adding (SDLK_s		, name, type); return; }
	if (0==strcmp("SDLK_h"	       	, key)) { Hotkey_adding (SDLK_h		, name, type); return; }
	if (0==strcmp("SDLK_l"	       	, key)) { Hotkey_adding (SDLK_l		, name, type); return; }
	if (0==strcmp("SDLK_u"	       	, key)) { Hotkey_adding (SDLK_u		, name, type); return; }
	if (0==strcmp("SDLK_1"	       	, key)) { Hotkey_adding (SDLK_1		, name, type); return; }
	if (0==strcmp("SDLK_2"	       	, key)) { Hotkey_adding (SDLK_2		, name, type); return; }
	if (0==strcmp("SDLK_3"	       	, key)) { Hotkey_adding (SDLK_3		, name, type); return; }
	if (0==strcmp("SDLK_4"	       	, key)) { Hotkey_adding (SDLK_4		, name, type); return; }
	if (0==strcmp("SDLK_5"	       	, key)) { Hotkey_adding (SDLK_5		, name, type); return; }
	if (0==strcmp("SDLK_6"	       	, key)) { Hotkey_adding (SDLK_6		, name, type); return; }
	if (0==strcmp("SDLK_F1"	       	, key)) { Hotkey_adding (SDLK_F1	, name, type); return; }
	if (0==strcmp("SDLK_F2"	       	, key)) { Hotkey_adding (SDLK_F2	, name, type); return; }
	if (0==strcmp("SDLK_F3"	       	, key)) { Hotkey_adding (SDLK_F3	, name, type); return; }
	if (0==strcmp("SDLK_F4"	       	, key)) { Hotkey_adding (SDLK_F4	, name, type); return; }
	if (0==strcmp("SDLK_F5"	       	, key)) { Hotkey_adding (SDLK_F5	, name, type); return; }
	if (0==strcmp("SDLK_F6"	       	, key)) { Hotkey_adding (SDLK_F6	, name, type); return; }
	if (0==strcmp("SDLK_F7"	       	, key)) { Hotkey_adding (SDLK_F7	, name, type); return; }
	}}}
void Hotspot_add   (const char *text, int tab, int x, int y, const char *action, TTF_Font *font, SDL_Color *fg, SDL_Color *bg, int type) {{{
	my_SDL_Rect *tmp = (my_SDL_Rect *)xMalloc (sizeof (my_SDL_Rect));
	tmp->l.x	= x;
	tmp->l.y	= y;
	tmp->name	= (action == NULL) ? NULL : xStrdup (action);
	tmp->display	= (text   == NULL) ? NULL : xStrdup (text);
	tmp->tab	= tab;
	tmp->font	= font;
	tmp->fg		= fg;
	tmp->bg		= bg;
	tmp->type	= type;
	tmp->next	= event_root;
	event_root 	= tmp;
	}}}

/* {{{ Draw Text and spezial input types */
int Display_Input_Box(SDL_Surface *sur) {{{
const	int	TITLE_h = 16;
	SDL_Rect rect;
	SDL_Rect rect_s;
	SDL_Surface *TEXT;
	int x, y;
	if (box.show != 1) return 0;
	if (box.x<0) box.x=box.x+sur->w;
	if (box.y<0) box.y=box.y+sur->h;
	x = box.x;
	y = box.y;
	//draw box
	box.box.x = x;
	box.box.y = y;
	box.box.w = 150;
	box.box.h = 1+TITLE_h+1+5+TITLE_h+5;;
	SDL_FillRect(sur, &box.box, 0x00cccccc);
	// Draw title Bar
	rect.x=x;
	rect.y=y;
	rect.w=150;
	rect.h=TITLE_h+2;
	SDL_FillRect(sur, &rect, 0x000000ff);
	// Draw Title
	TEXT = TTF_RenderText_Shaded(font1, box.title, white, blue);
	rect.x=x+5;
	rect.y=y+1;
	rect_s.x=0;
	rect_s.y=4;
	rect.w=rect_s.w=(TEXT->w<130)?TEXT->w:130;
	rect.h=rect_s.h=(TEXT->h<TITLE_h )?TEXT->h:TITLE_h;
	SDL_BlitSurface(TEXT, &rect_s, sur, &rect);
	SDL_FreeSurface(TEXT);
	{{{ // Draw X (image would be nicer
	box.close.x=x+135;
	box.close.y=y+1;
	rect_s.x=0;
	rect_s.y=4;
	box.close.w=rect_s.w=TITLE_h;
	box.close.h=rect_s.h=TITLE_h;
	TEXT = TTF_RenderText_Solid(font1, "X", white);
	SDL_BlitSurface(TEXT, &rect_s, sur, &box.close);
	SDL_FreeSurface(TEXT);
	}}}
	{{{ // Draw Input Field
	box.value_r.x=x+ 5;
	box.value_r.y=y+TITLE_h+2+5;
	box.value_r.w=140;
	box.value_r.h=TITLE_h;
	SDL_FillRect(sur, &box.value_r, 0x00ffffff);
	}}}
	if(strlen(box.value)>0) {{{ // draw Input Text
		TEXT = TTF_RenderText_Shaded(font1, box.value, black, white);
		rect.x=x+5;
		rect.y=y+TITLE_h+2+5+1;
		rect_s.x=0;
		rect_s.y=4;
		rect.w=rect_s.w=(TEXT->w<138)?TEXT->w:138;
		rect.h=rect_s.h=(TEXT->h<TITLE_h )?TEXT->h:TITLE_h;
		SDL_BlitSurface(TEXT, &rect_s, sur, &rect);
		SDL_FreeSurface(TEXT);
		}}}
	SDL_UpdateRect (sur, box.box.x, box.box.y, box.box.w, box.box.h);
	return 1;
	}}}
static my_SDL_Rect *Pos_Left (const my_SDL_Rect *a,my_SDL_Rect *b) {{{
  b->l.y = a->l.y;
  b->l.x = a->l.x + a->l.w + 10;
  b->tab = a->tab;
  return b;
}}}
static my_SDL_Rect *Pos_Under(const my_SDL_Rect *a,my_SDL_Rect *b) {{{
  b->l.y = a->l.y + a->l.h + line2_space;
  b->l.x = a->l.x;
  b->tab = a->tab;
  return b;
}}}
/* {{{ static int  TTF_Text       (SDL_Surface *target,TTF_Font *font, my_SDL_Rect *dstrect, const char *txt, SDL_Color fg, SDL_Color bg)
  *
  * Draw an text on the surface with givven color, and fill height and width for event handling
  *
  * Input:
  * SDL_Surface *	Where to draw the text 
  * TTF_Font	*	What font to use
  * SDL_Rect    *	what position (x,y)
  * const char  *	The text
  * const SDL_Color	foreground color
  * const SDL_Color	background color (if shading)
  *
  * Output:
  * int			If text was written
  * param[1]		(h,w)
  */ 
static int TTF_Text  ( SDL_Surface *target, TTF_Font *font, my_SDL_Rect *dstrect, const char *txt, SDL_Color fg, SDL_Color bg) {
	int w,h;
	SDL_Surface *TEXT;
	if(txt==NULL) return 0;
	TTF_SizeText (font, txt, &w, &h);
	dstrect->l.w = w;
	dstrect->l.h = h;
	if(target==NULL) return 1;	// Only get the size
	TEXT = TTF_RenderText_Shaded(font, txt, fg,bg);
	if(TEXT==NULL) return 0;
	dstrect->a = 1;

	dstrect->l.x += Tab_offset_x (dstrect->tab);
	dstrect->l.y += Tab_offset_y (dstrect->tab);
	SDL_BlitSurface(TEXT, NULL, target, &(dstrect->l));
	dstrect->l.x -= Tab_offset_x (dstrect->tab);
	dstrect->l.y -= Tab_offset_y (dstrect->tab);

	SDL_FreeSurface(TEXT);
	return 1;
	}
/* }}} */
static void TTF_Value      (SDL_Surface *target, TTF_Font *font, my_SDL_Rect *v      , int val        , SDL_Color *fg, SDL_Color *bg) {{{
	char tmp[9];
	SDL_Color Value  = { (val>>8),255-(val>>8), 0x00, 0 };
	sprintf(tmp, "%6d",val);
	if (fg == &red) TTF_Text(target, font, v, tmp, Value, *bg);
	else		TTF_Text(target, font, v, tmp, *fg  , *bg);
	}}}
/* {{{ static void Ctrl_Value     (SDL_Surface *target, my_SDL_Rect *p, my_SDL_Rect *v, my_SDL_Rect *m, int val, int manual);
  *
  * Display and config field vor numerical values
  * + and - only display if manual=1
  *
  * Input:
  * SDL_Surface *	Where to display the output
  * int			X Location
  * int			Y Location
  * SDL_Rect *		storage for + Hotspace
  * SDL_Rect *		storage for value Hotscape
  * SDL_Rect *		storage for  -Hotspace
  * int			value to display
  * int			Manual/Automatic
  *
  * Return:
  * -			Noting
  * p, v, m		filled with x,y,w,h
  */
static void Ctrl_Value (SDL_Surface *target, my_SDL_Rect *p, my_SDL_Rect *v, my_SDL_Rect *m, int val, int manual) {
	char tmp[9];
	SDL_Color Value  = { (val>>8),255-(val>>8), 0x00, 0 };
	sprintf(tmp, "%6d",val);
	if(manual)	TTF_Text(target, font2, p	      , "+"	, red  , black);
	else		TTF_Text(NULL  , font2, p	      , "+"     , red  , black);	// Do not display /but need the size

			TTF_Text(NULL  , font2, Pos_Left(p,v) , "-54321", red  , black);	// 6 space between + -
			TTF_Text(target, font2, v	      , tmp     , Value, black);
	if(manual)	TTF_Text(target, font2, Pos_Left(v,m) , "-"	, green, black);
	}
/* }}} */
static void Config_Shutter (SDL_Surface *target, my_SDL_Rect *l) {{{
  my_SDL_Rect tmp = {{0,0,0,0},0,NULL,NULL};
  TTF_Text(target, font2, l, "Shutter Timming", (shutter_value==-1)?red:green, black);
  if (shutter_value < 0) TTF_Text  (target, font2, Pos_Under (l, &shutter_p), "Automatic" , white, black);
  else 			 Ctrl_Value(target,        Pos_Under (l, &shutter_p), &tmp, &shutter_m, shutter_value, 1);
  }}}
/* {{{ static void Config_White   (SDL_Surface *target, my_SDL_Rect *)
  *
  * Display an controlset for white space management
  *
  * Input:
  * SDL_Surface *	Where to display the output
  * int			X Location
  * int			Y Location
  *
  * Output:
  * -			Nothing only display and Modify
  *
  * Modify:
  * ,white_m		Label and Mode for whitespace
  * red_p, , red_m		Red value control set
  * blue_p, , blue_m	Blue value control set
  */
static void Config_White (SDL_Surface *target, my_SDL_Rect *l) {
	int mode = v4l[v4l_akt]->Parameter ("white_m");
	my_SDL_Rect tmp = {{0,0,0,0},0,NULL,NULL};
	const char *temp;
	switch(mode) {
		case  0: temp = "Automatic"  ;  break;
		case  1: temp = "Manual"     ;  break;
		case  2: temp = "Indoor"     ;  break;
		case  3: temp = "Outdoor"    ;  break;
		case  4: temp = "Fluorescent";  break;
		default: temp = "Unknown"    ;  break;
		}
	
	TTF_Text  (target, font2, l                       , "White Balance", (mode == 1)?white:red, black);
	TTF_Text  (target, font2, Pos_Under (l , &white_m), temp           , white                , black);
	Ctrl_Value(target       , Pos_Under (&white_m, &red_p  ), &tmp, &red_m , v4l[v4l_akt]->Parameter ("white_r"), mode==1);
	Ctrl_Value(target       , Pos_Under (&red_p  , &blue_p ), &tmp, &blue_m, v4l[v4l_akt]->Parameter ("white_b"), mode==1);
	}
/* }}} */
/* {{{ static void Config_Dump    (SDL_Surface *target, my_SDL_Rect *)
  *
  * Display Dump control set
  *
  * SDL_Surface *	Where to display the output
  * int			X Location
  * int			Y Location
  *
  * Output:
  * -			Only display and modify
  *
  * Modify:
  * , &dump_m	label and Mode for Dumping
  *
  * Todo:
  * Interval value for dumping every N seconds
  */
static void Config_Dump (SDL_Surface *target, my_SDL_Rect *l)
{
	my_SDL_Rect tmp = {{0,0,0,0},0,NULL,NULL};
	TTF_Text(target, font2, l, "     Capture", red, black);
	Ctrl_Value(target, Pos_Under (l, &interval_p), &tmp, &interval_m, configuration.dump_speed, 1);
	Pos_Under (&interval_p, &dump_m);
	switch (configuration.dump_mode) {
		case  0: TTF_Text (target, font2, &dump_m, "     JPEG", white, black); 
			 Ctrl_Value(target, Pos_Under (&dump_m, &qual_p), &tmp, &qual_m, configuration.dump_quality, 1);
			 break;
		case  1: TTF_Text (target, font2, &dump_m, "     PNG" , white, black); break;
		case  2: TTF_Text (target, font2, &dump_m, "     BMP" , white, black); break;
		default: break;
		}
}

/* }}} */
static void Config_Reorder (SDL_Surface *target, my_SDL_Rect *l) {{{
	char *next;
	my_SDL_Rect tmp = {{0,0,0,0},0,NULL,NULL};
	char radius[15];
	char effekt[15];
	switch (GET_FILTER(reorder_typ)) {
	case 0 : next = "Reorder  "; break;
	case 1 : next = "Glass    "; break;
	case 2 : next = "Rotate   "; break;
	default: next = "Reorder ?"; break;
	  }
	snprintf(radius,15,"%5i",g_filter_stats.reorder_radius);
	snprintf(effekt,15,"%3.3f",g_filter_stats.reorder_effekt);

	TTF_Text(target, font2, 		    l	       , next , (GET_FILTER(reorder_typ))?red:green, black);  

	TTF_Text(target, font2, Pos_Under(l	   , &radius_p), "+"     , red  , black);
	TTF_Text(NULL  , font2, Pos_Left (&radius_p, &tmp     ), "-54321", red  , black);	// 6 space between + -
				Pos_Left (&tmp     , &radius_m);
	TTF_Text(target, font2, 		     &tmp      , radius  , white, black);
	TTF_Text(target, font2,                      &radius_m , "-"     , red  , black);

	TTF_Text(target, font2, Pos_Under(&radius_p, &effect_p), "+"     , red  , black);
	TTF_Text(NULL  , font2, Pos_Left (&effect_p, &tmp     ), "-54321", red  , black);	// 6 space between + -
				Pos_Left (&tmp     , &effect_m);
	TTF_Text(target, font2,           	     &tmp      , effekt  , white, black);
	TTF_Text(target, font2,           	     &effect_m , "-"     , red  , black);
}}}
/* }}} */

void interface_init  (void) {{{ // Init GUI (load SDLcam.xml / fonts)
	/* Initialise the font */
	if (0 > TTF_Init ()) {{{
		fprintf (stderr, "TTF_Init() failed\n");
		return Exit ();
		}}}
	atexit (TTF_Quit);
	if (NULL == (font1 = TTF_OpenFont (configuration.FONT1, 12))) {{{
		fprintf (stderr, "Font1 (%s)failed\n",configuration.FONT1);
		return Exit ();
		}}}
	if (NULL == (font2 = TTF_OpenFont (configuration.FONT2, 12))) {{{
		fprintf (stderr, "Font2 (%s)failed\n",configuration.FONT2);
		return Exit ();
		}}}
	line2_space=-(12*2/3);
	if (NULL == (font3 = TTF_OpenFont (configuration.FONT3, 20))) {{{
		fprintf (stderr, "Font3 (%s)failed\n",configuration.FONT3);
		return Exit ();
		}}}
	TTF_SetFontStyle(font1,TTF_STYLE_NORMAL);
	TTF_SetFontStyle(font2,TTF_STYLE_NORMAL);
	TTF_SetFontStyle(font3,TTF_STYLE_NORMAL);
	Hotkey_clear ();

	FILE *tmp_file;

	tmp_file = fopen("SDLcam.xml", "r");
	if (tmp_file != NULL) {
		fclose(tmp_file);
		XML_Theme ("SDLcam.xml");
		return;
		}
	tmp_file = fopen(PKGDATADIR "/SDLcam.xml", "r");
	if (tmp_file != NULL) {
		fclose(tmp_file);
		XML_Theme (PKGDATADIR "/SDLcam.xml");
		return;
		}
	fprintf(stderr, "Could not find/open SDLcam.xml. Aborting\n");
	exit(-1);
}}}

void Tab_draw       (SDL_Surface *sur, int tab) {{{
	my_SDL_Rect *akt;
	for (akt = event_root; akt != NULL; akt=akt->next) {
	    if (akt->tab != tab) continue;
	    switch(akt->type) {
		default: break;
		case 0 : TTF_Text (sur, akt->font, akt, akt->display		  , *akt->fg, *akt->bg); break;
		case 1 : TTF_Value(sur, akt->font, akt, abs(Value_get (akt->name)),  akt->fg,  akt->bg); break;					// Value
		case 2 : TTF_Text (sur, akt->font, akt, Display_Filter(akt->name) , (Mode_Filter(akt->name))?red:*akt->fg, *akt->bg); break; 	// Filter
		case 3 : TTF_Text (sur, akt->font, akt, akt->display		  , (atoi(akt->name)==tab_nr)?gray:*akt->fg, *akt->bg); break;	// Tab
		case 4 : {
				int res_w,res_h;
				v4l[v4l_akt]->Resolution (&res_w, &res_h);
			 	TTF_Text (sur, akt->font, akt, akt->display	  , (atoi(akt->name)==res_w)?red:*akt->fg, *akt->bg); 
			 }
			break;
			 
		case 5 : if (0 == strcmp("white"  , akt->name)) { Config_White   (sur, akt); break; }
			 if (0 == strcmp("shutter", akt->name)) { Config_Shutter (sur, akt); break; }
			 if (0 == strcmp("dump"	  , akt->name)) { Config_Dump    (sur, akt); break; }
			 if (0 == strcmp("reorder", akt->name)) { Config_Reorder (sur, akt); break; }

		case 6 : TTF_Text (sur, akt->font, akt, akt->display		  , (atoi(akt->name)==v4l_akt)?gray:*akt->fg, *akt->bg); break;	// Tab
		}
	    }
	}}}
int  Sidebar_draw   (SDL_Surface *sur) {{{
	my_SDL_Rect opt = {{4,4,0,0},1,NULL,NULL};
	SDL_Rect fill;
	/* {{{ Clear the old sidebar */
	fill.x = side_bar_x;
	fill.y = side_bar_y;
	fill.w = SIDE_BAR_W;
	fill.h = SIDE_BAR_H;
	SDL_FillRect(sur, &fill, 0);
	/* }}} */
	if(captured_size>0) {{{
		char  temp_s[19];
		opt.l.x = side_bar_x + 4;
		opt.l.y = side_bar_y + SIDE_BAR_H - 80;
		snprintf(temp_s,19,"%18i",captured_size);
		TTF_Text(sur, font2, &opt, "File size:", yellow, black);
		TTF_Text(sur, font2, Pos_Under (&opt, &opt), temp_s, yellow, black);
		}}}
	Tab_draw       (sur, -1);
	SDL_UpdateRect (sur, side_bar_x, side_bar_y, SIDE_BAR_W, SIDE_BAR_H    );
	return 1;
	}}}
int  Bottombar_draw (SDL_Surface *sur) {{{
	SDL_Rect fill;
	/* {{{ Clear the bottom bar */
	fill.x=bottom_bar_x;
	fill.y=bottom_bar_y;
	fill.w=BOTTOM_BAR_W;
	fill.h=BOTTOM_BAR_H;
	SDL_FillRect(sur, &fill, 0);
	/* }}} */
	Tab_draw       (sur, 0	);
	Tab_draw       (sur, tab_nr );
	SDL_UpdateRect (sur, bottom_bar_x, bottom_bar_y, BOTTOM_BAR_W, BOTTOM_BAR_H);
	return 1;
	}}}
int  interface_draw (SDL_Surface *sur) {{{ // Display the compleat control set (gui)
	my_SDL_Rect *akt;
	/* All Hotspots deactivate on display they become activated again */
	for (akt = event_root; akt != NULL; akt=akt->next) akt->a = 0;

	bottom_bar_x = (BOTTOM_BAR_X >= 0) ? BOTTOM_BAR_X : sur->w + BOTTOM_BAR_X;
	bottom_bar_y = (BOTTOM_BAR_Y >= 0) ? BOTTOM_BAR_Y : sur->h + BOTTOM_BAR_Y;

	side_bar_x   = (SIDE_BAR_X   >= 0) ? SIDE_BAR_X   : sur->w + SIDE_BAR_X;
	side_bar_y   = (SIDE_BAR_Y   >= 0) ? SIDE_BAR_Y   : sur->h + SIDE_BAR_Y;
	Sidebar_draw   (sur);
	Bottombar_draw (sur);

	sum_fps = 0;
	cnt_fps = 0;
	Display_Input_Box (sur);
	return 0;
	}}}

void interface_close (void) {{{ // cleanup all GUI Stuff
  	TTF_CloseFont (font1);
  	TTF_CloseFont (font2);
  	TTF_CloseFont (font3);
  	return;
	}}}

/* {{{ Event Handling */
static inline int In_Box (const SDL_Rect *my_box, int x, int y) {{{
	return (x >= my_box->x && x <= (my_box->x + my_box->w) && 
		y >= my_box->y && y <= (my_box->y + my_box->h));
	}}}
void interface_click (int x, int y, SDL_Surface *sur) {{{  // Handle an Mouse event
	my_SDL_Rect *akt;
	if (box.show==1 && In_Box (&box.box, x, y)) {
		if(In_Box (&box.close, x, y)) {
			box.show = 0;
			SDL_EnableUNICODE (0);
			return;
			}
		return;
		}
	for (akt = event_root; akt != NULL; akt=akt->next)
	  if (akt->a == 1 && akt->name!=NULL) {
	    if (In_Box(&akt->l, x - Tab_offset_x (akt->tab), y - Tab_offset_y (akt->tab))) {
		    
		if (configuration.debug)
			printf("Click %s (%i)\n",akt->name, akt->type);

			
			switch (akt->type) {
			default : Name_Event (akt->name); return;
			case  1 : Show_Input_Box (Tab_offset_x (0) + 5, Tab_offset_y (0), akt->name, akt->name); return;
			case  3 : tab_nr = atoi(akt->name); return;
			case  4 : return Set_Resolution (akt->name);
			case  6 : v4l_akt = atoi(akt->name); return;
				}
	    		}
	  	}
	mouse_img_x = x - image_x;
	mouse_img_y = y - image_y;
	}}}
void interface_key   (SDL_keysym *key) {{{ // keyboard event handling
	my_SDL_Key_Event *akt;
	if (box.show==1) {{{
		unsigned char c = (unsigned char)key->unicode;
		int len;
		if(key->sym == SDLK_ESCAPE) {
			box.show = 0;
			SDL_EnableUNICODE (0);
			return;
			}
		len = strlen (box.value);
		if (c == '\n' || c == '\r') {
			box.show = 0;
			Value_set (box.name, box.value);
			SDL_EnableUNICODE (0);
			}
		if (c == '\b') {
			if (len==0) return;
			box.value = (char*)xRealloc (box.value, len);
			box.value[len-1]=0;
			return;
			}
		if (len > 10) return;
		if ( (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) {
			box.value= (char*) xRealloc (box.value, len+2);
			box.value[len  ]=c;
			box.value[len+1]=0;
			}
		return;
		}}}
	for (akt = key_Root; akt != NULL; akt=akt->next)
	  if (akt->k == key->sym && akt->name != NULL) {
	    switch (akt->type) {
	      	case 0 : return Name_Event (akt->name);
	      	case 4 : return Set_Resolution (akt->name);
              	case 6 : v4l_akt = atoi(akt->name); return;
	      	default: return;
		}
    	    }
}}}
/* }}} */

int show_image(SDL_v4l_image *image, int flip_Y, SDL_Surface *target) {{{
       size_t y, i=0;
       unsigned int *buf = (unsigned int *)image->data;
       unsigned int pos_x = image_x * target->format->BytesPerPixel;
       unsigned int pitch = image->w * 4;
       /* Lock the Surface for direct access to the pixels */
       if ( SDL_MUSTLOCK (target) )
               if ( SDL_LockSurface (target) < 0 ) {{{
                       fprintf(stderr, "Can't lock Surface: %s\n", SDL_GetError());
                       return -1;
               }}}

       i=0;
       for (y=0; y<image->h; y++)
               memcpy(((char*)target->pixels)+(y*target->pitch)+pos_x, &buf[y*image->w], pitch);

       if ( SDL_MUSTLOCK (target) )
               SDL_UnlockSurface (target);

       return 0;
}}}

#ifdef __cplusplus
	}
#endif


/*
 * vim600: fdm=marker
 */
