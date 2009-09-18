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
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_NLS
#include <libintl.h>
#endif

#include "init.h"
#include "misc.h"
#include "dyn_misc.h"
#include "configuration.h"

ProgCFG configuration;

int create_default_config()
{
	char *tmp;
	configuration.v4l_fps = DEFAULT_CAPTURE_RATE;
	configuration.dump_mode = 0;
	configuration.dump_quality = 85;

	/* set dump path */
	tmp = getenv ("HOME");
	configuration.debug = 0;
	Dump_Prefix_set("SDLcam_pic");
	Dump_Path_set ((tmp==NULL)?"":tmp);

	/* Find ressource files */
	configuration.FONT1 = find_file("LucidaTypewriterRegular.ttf");
	configuration.FONT2 = find_file("LucidaSansRegular.ttf");
	configuration.FONT3 = find_file("LucidaSansRegular.ttf");

	configuration.ftp_host = NULL;
	configuration.ftp_user = NULL;
	configuration.ftp_pass = NULL;
	configuration.ftp_temp = NULL;
	configuration.ftp_file = NULL;
        configuration.dump_speed = 0;
	configuration.dump_count=0;
	configuration.dump_next=0;

	configuration.start_in_minimode=0;
	configuration.start_in_fullscreen=0;

	return 0;
}

int parse_args(int argc, char **argv)
{
	/* Parse the args and alter configuration if necessary */
	int arg=0;
	char processed=0;

//	print_progname();

	for (arg=0; arg<argc; arg++)
	{
		processed=0;

		if (!strcmp(ARG_HELP, argv[arg])) {
			print_commandLineOptions();
			return -1;
		}
		if (!strcmp(ARG_WARRENTY, argv[arg])) {
			warrenty();
			return -1;
		}
		if (!strcmp(ARG_FULLSCREEN, argv[arg])) {
			configuration.start_in_fullscreen=1;
			processed=1;
		}
		if (!strcmp(ARG_MINIMODE, argv[arg])) {
			configuration.start_in_minimode=1;
			processed=1;
		}
		if (!strcmp(ARG_FPS, argv[arg])){
			arg++;
			if (arg>=argc) { return -1; }
			configuration.v4l_fps = strtol(argv[arg], NULL, 0);
			processed=1;
		}
		if (!strcmp(ARG_DUMPPATH, argv[arg])){
			arg++;
			if (arg>=argc) { return -1; }
			Dump_Path_set (argv[arg]);
			processed=1;
		}
		if (!strcmp(ARG_DUMPMODE, argv[arg])){
			arg++;
			if (arg>=argc) { return -1; }
			configuration.dump_mode = strtol(argv[arg], NULL, 0);
			if (configuration.dump_mode<0||configuration.dump_mode>2)
			{
				printf(_("Error: Invalid mode value\n"));
				return -1;
			}
			processed=1;
		}
		if (!strcmp(ARG_DEBUG, argv[arg])){
			configuration.debug = 1;		
			processed=1;
		}
		if (!strcmp(ARG_LOGO, argv[arg])){
			int blit_pos;
			arg++;
			if (arg>=argc) { return -1; }
			configuration.powered_by_image_path = strdup(argv[arg]);
			arg++;
			if (arg>=argc) { return -1; }
			blit_pos = strtol(argv[arg], NULL, 0);

			if (  !((blit_pos>=0) && (blit_pos<=5))  )
			{
				printf(_("Error: Bad position value\n"));
			}
			configuration.powered_by_blitpos=blit_pos;
			processed=1;
		}

		if (!processed)
		{
			printf(_("Unknown arg: '%s'\n"), argv[arg]);
			return -1;
		}
	}

	return 0;
}

void print_commandLineOptions()
{
	printf(_("%-10s  List command line options\n")			, ARG_HELP);
	//printf("%-10s  Choose video device\n")			, ARG_DEVICE);
	printf(_("%-10s  Set capture width\n")				, ARG_WIDTH);
	printf(_("%-10s  Set capture height\n")				, ARG_HEIGHT);
	printf(_("%-10s  Set cam FPS\n")				, ARG_FPS);
	printf(_("%-10s  Set the directory where snapshots will go\n")	, ARG_DUMPPATH);
	printf(_("%-10s  0=jpeg(default), 1=png, 2=bmp\n")		, ARG_DUMPMODE);
	printf(_("%-10s  [filename] [pos]  0=dont show,  1=upper left, 2=lower right\n"),ARG_LOGO);
	printf(_("                              3 = lower left, 4 = lower right, 5=center\n"));
	printf(_("%-10s  Start in fullscreen mode\n")			, ARG_FULLSCREEN);
	printf(_("%-10s  Start in mini mode\n")				, ARG_MINIMODE);
	printf(_("%-10s  Start with debug messages enabled\n")		, ARG_DEBUG);
	printf(_("%-10s  Show warrenty\n")				, ARG_WARRENTY);
	printf("\n");
}

void print_progname()
{
	printf(_("\nSDLcam version %s, Copyright (C) 2001-2002 Raphael Assenat\n"), PACKAGE_VERSION); /* PROG_VERSION is defined in Makefile */
	printf(_("SDLcam comes with ABSOLUTELY NO WARRANTY; for details\n"));
	printf(_("type `SDLcam %s'.  This is free software, and you are welcome\n"),ARG_WARRENTY);
	printf(_("to redistribute it under certain conditions; read LICENSE\n"));
	printf(_("	for details.\n\n"));
	printf(_("Try ./SDLcam --help for a list of command line options\n\n"));
}

char *find_file (const char *filename)
{
	/* Search for the file filename in standard install locations */
	FILE *tmp=NULL;
	char *file;

	/* Current directory */

	tmp = fopen(filename, "r");
	if (tmp!=NULL) 
	  { 
	    fclose(tmp); 
	    return strdup (filename); 
	  }
	/* Install directory */
	file = (char*)xMalloc(strlen(PKGDATADIR)+strlen(filename)+2);
	sprintf(file, "%s/%s", PKGDATADIR, filename);

	tmp = fopen(file, "r");

	if (tmp!=NULL)
	  {

	    fclose(tmp);
	    return file;
	  }

	  perror("open");

	free(file);
	return NULL;
}

void warrenty(void)
{
	printf(
	"NO WARRANTY\n\n"
	"11. BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW. EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM \"AS IS\" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.\n\n"

	"12. IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.\n\n");
}



