/* Default path */
#ifndef DEF_PATH
#define DEF_PATH "/usr/local/shared/mp3"
#endif
/* Default logfile */
#ifndef DEF_LOGFILE
#define DEF_LOGFILE "/usr/local/var/log/ample"
#endif
/* Default configfile */
#ifndef DEF_CONFFILE
#define DEF_CONFFILE "/usr/local/etc/ample.conf"
#endif

#define DEF_PORT 1234           /* Port to listen to */
#define DEF_MAX_CLIENTS 5       /* Max simultaneous clients */
#define DEF_SERVERNAME "Ample"  /* Default server name */
#define LINELENGTH 400          /* Size of buffer used with fgets() */

/* Config file option types */
#define OPT_BOOL 1              /* Boolean option */
#define OPT_INT 2               /* Integer option */
#define OPT_STRING 3            /* String option */

#define CONN_TIMEOUT 60         /* The time a client has to complete the http request */


#define	USAGE1 "Usage: %s [OPTION(S)]... [PATH]\n\
\n\
AMPLE - An MP3 LEnder\n\
recursively indexes all MP3 files it finds below PATH and then listens\n\
to a TCP port for incoming connections. Once a connection is made AMPLE will\n\
start sending randomly chosen MP3's. If called from the command line or a\n\
script AMPLE will go into daemon mode (unlike when called from inetd or with\n\
the option -t).\n\
\n"

#if HAVE_GETOPT_H
#define USAGE2 "\
  -p, --port=NUMBER           which port to listen to, default %d\n\
  -o, --order                 play MP3 files in alphabetical order\n\
  -c, --clients=NUMBER        how many clients are allowed to be connected\n\
                              default %d\n\
  -n, --norecursive           don't index MP3 files in subdirs of the given dir\n\
  -f, --conffile=FILENAME     alternative file to read for config options\n\
  -h, --help                  display this help and exit\n\
  -d, --debug[=NUMBER]        debug messages will be printed\n\
                              higher number means more detail\n\
  -t, --trace                 no forking, no backgrounding\n\
                              helpful when debugging\n\
  -v, --version               output version information and exit\n\
\n\
Report bugs to <david@2gen.com>\n"
#else
#define USAGE2 "\
  -p=NUMBER                   which port to listen to, default %d\n\
  -o                          play MP3 files in alphabetical order\n\
  -c=NUMBER                   how many clients are allowed to be connected,\n\
                              default %d\n\
  -n                          don't index MP3 files in subdirs of the given dir\n\
  -f=FILENAME                 alternative file to read for config options\n\
  -h                          display this help and exit\n\
  -d[=NUMBER]                 debug messages will be printed,\n\
                              higher number means more detail\n\
  -t                          no forking, no backgrounding,\n\
                              helpful when debugging)\n\
  -v                          output version information and exit\n\
\n\
Report bugs to <david@2gen.com>\n"
#endif

/* Checks command line, config file and default options then set conf */
extern void checkopt(int argc, char * argv[]);

/* Reads and parses client http request */
extern bool readrequest (FILE *stream);

/* Struct used when parsing config files */
struct confoption {
	char *name;
	unsigned short int type;
	void *value;
};
