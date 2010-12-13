/*
	console.h
	the game console

	$Header: /cvsroot/uhexen2/hexenworld/Client/console.h,v 1.10 2007/03/15 13:36:58 sezero Exp $
*/

#ifndef __CONSOLE_H
#define __CONSOLE_H

#define		CON_TEXTSIZE	16384
typedef struct
{
	char	text[CON_TEXTSIZE];
	byte	text_attr[CON_TEXTSIZE];
	int		current;		// line where next message will be printed
	int		x;			// offset in current line for next print
	int		display;		// bottom of console displays this line
} console_t;

extern	console_t	con_main;
extern	console_t	con_chat;
extern	console_t	*con;			// point to either con_main or con_chat

extern	int		con_ormask;

extern	int con_totallines;
extern	qboolean con_initialized;
extern	byte *con_chars;
extern	int con_notifylines;	// scan lines to clear for notify lines

void Con_DrawCharacter (int cx, int line, int num);

void Con_CheckResize (void);
void Con_Init (void);
void Con_DrawConsole (int lines);

void Con_ShowList (int , const char **);
//void Con_Clear_f (void);
void Con_DrawNotify (void);
void Con_ClearNotify (void);
void Con_ToggleConsole_f (void);

void Con_NotifyBox (const char *text);	// during startup for sound / cd warnings

#endif	/* __CONSOLE_H */
