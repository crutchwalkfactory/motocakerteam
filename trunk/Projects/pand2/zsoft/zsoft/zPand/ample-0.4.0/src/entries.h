#ifndef PATH_MAX
#  if defined (_POSIX_PATH_MAX)
#    define PATH_MAX _POSIX_PATH_MAX
#  elif defined (MAXPATHLEN)
#    define PATH_MAX MAXPATHLEN
#  elif defined (_PC_PATH_MAX)
#    define PATH_MAX _PC_PATH_MAX
#  else
#    define PATH_MAX 1024
#  endif
#endif

/* Holds all needed info about a MP3 file */
typedef struct node {
	char * path;
	off_t filesize;
	int id3v1len;
	int id3v2len;
	char *title;
	struct node * next;
} mp3entry;

#define HASID3V2(entry) entry->id3v2len > 0
#define HASID3V1(entry) entry->id3v1len > 0

/* Current entry in our circular linked list */
extern mp3entry * root;

/* Adds an entry to the end of the list pointed to by rootentry */
extern void addentry(mp3entry ** rootentry, mp3entry * toadd);

/* Removes an entry from the list pointed to by rootentry */
extern void removeentry(mp3entry ** rootentry, mp3entry * toremove);

/* Clears and frees a tree of MP3 files */
extern void clearlist(mp3entry *rootentry);

/* Counts the entries in the list pointed to by rootentry */
extern int countentries(mp3entry * rootentry);

/* Shuffles the entries */
extern void shuffleentries(void);

/* Dumps all MP3's in list, useful for debugging */
void dumpentries();

/* Creates a HTML listing of a directory */
void createhtml(FILE *st, char *dirname, int baselen);

/* Recursively finds all MP3's below path and adds them to list */
extern void getfiles(char * path);

/* Checks if a filename is a valid MP3 file, if so it returns TRUE and fills in buf */
extern int validfile(char *filename, char *cwd, mp3entry **buf);

/* Checks if a dirname is a valid dir, if so it copies the name to buf */
extern int validdir(char *dirname, char *cwd, char **buf);
