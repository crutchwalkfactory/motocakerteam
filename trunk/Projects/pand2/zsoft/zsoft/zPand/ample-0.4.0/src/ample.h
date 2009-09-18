#undef TRUE
#define TRUE 1
#undef FALSE
#define FALSE 0
#undef bool
/* NOTE: This is signed since it's used somewhat different than common 
 * in some conditions, < 0 means that it's undefined while > 0 means TRUE 
 * and 0 means FALSE
 */
#define bool signed short int

#define GETCWD(cwd)                                 \
do {                                                \
	errno = 0;                                  \
	cwd = getcwd(NULL,0);                       \
	if(errno == EACCES) {                       \
		die("no access to dir\n");          \
	} else if(errno == EINVAL && cwd == NULL) { \
		/* Auto-malloc not supported */     \
		errno = 0;                          \
		cwd = getcwd(NULL,PATH_MAX);        \
	}                                           \
	if(errno)                                   \
		/* That didn't help things */       \
		die("getcwd");                      \
} while(0)


struct config {
	unsigned short int  mode;
	int port;
	bool order;
	bool trace;
	bool recursive;
	int debuglevel;
	int max_clients;
	char * user;
	char * pass;
	char * path;
	char * filename;
	char * program_name;
	char * logfile;
	char * conffile;
	char * servername;
};

#define MODE_INDEX    0x01
#define MODE_METADATA 0x02
#define MODE_SINGLE   0x04

#define MODE_SET(x) do { conf.mode |= x; } while(0)
#define MODE_ISSET(x) (conf.mode & x)

extern struct config conf;
