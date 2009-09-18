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
#ifndef _interface_h
#define _interface_h

#ifdef __cplusplus
extern "C" {
#endif

extern int SIDE_BAR_X;		/* if <0 then right of image output */
extern int SIDE_BAR_W;		/* relavant if _X > 0 */
extern int SIDE_BAR_Y;
extern int SIDE_BAR_H;
extern int BOTTOM_BAR_X;
extern int BOTTOM_BAR_W;
extern int BOTTOM_BAR_Y;	/* if <0 under image output */
extern int BOTTOM_BAR_H;	/* relavant if _Y > 0 */
extern int IMAGE_X;
extern int IMAGE_Y;

extern int captured_size;
extern int mouse_img_x;
extern int mouse_img_y;
extern int image_x;
extern int image_y;

#include <SDL_video.h>
#include <SDL_keysym.h>
#include <SDL_ttf.h>
#include "SDL_v4l_image.h"

extern TTF_Font *font1;
extern TTF_Font *font2;
extern TTF_Font *font3;
extern SDL_Color yellow;
extern SDL_Color white ;
extern SDL_Color green ;
extern SDL_Color red   ;
extern SDL_Color black ;
extern SDL_Color gray  ;

typedef struct smy_SDL_Event {
  	SDL_Rect	l;		/* location 	*/
  	char		a;		/* active 	*/
  	struct smy_SDL_Event	*next  ;
  	char		*name;
  	char		*display;
  	int		tab;
  	TTF_Font 	*font;
  	SDL_Color	*fg;
  	SDL_Color	*bg;
  	char		type;		/* 0=Textlabel 1=Value(int) */
	} my_SDL_Rect;

typedef struct _SDL_Key_Event {
  	SDLKey	k;		/* key 		*/
  	int 		type;
  	char	*name;
  	struct _SDL_Key_Event	*next  ;
	} my_SDL_Key_Event;

typedef struct {
	SDL_Rect 	box;
	SDL_Rect 	close;
	SDL_Rect 	value_r;
	int		show;
	int		x;
	int		y;
	char*		title;
	char*		name;
	char*		value;
	} inputBox;

void Hotkey_clear	(void);
void Hotkey_add      	(const char *key, const char *name, int type);

void Hotspot_clear 	(void);
void Hotspot_add   	(const char *text, int tab, int x, int y, const char *action, TTF_Font *font, SDL_Color *fg, SDL_Color *bg, int type);

void interface_init  	(void);
void interface_click 	(int x, int y, SDL_Surface *window);
void interface_key   	(SDL_keysym *key);
int  interface_draw   	(SDL_Surface *target);
void interface_close 	(void);
int  show_image		(SDL_v4l_image *image, int flip_Y, SDL_Surface *target);
#ifdef __cplusplus
	}
#endif


#endif // _interface_h
