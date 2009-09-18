#define ID3V2_ID "ID3"
#define ID3V2_HEADER_LEN 21
#define ID3V2_MAJOR_VERS 0x04
#define ID3V2_MINOR_VERS 0x00
#define ID3V2_HEADER_FLAGS 0x00

#define ID3V2_FRAME_ID "TIT2"
#define ID3V2_FRAME_FLAGS1 0x00
#define ID3V2_FRAME_FLAGS2 0x00
#define ID3V2_FRAME_ENC 0x00

#define SYNCBYTE1(value) ((value >> (3*7)) & 0x7F)
#define SYNCBYTE2(value) ((value >> (2*7)) & 0x7F)
#define SYNCBYTE3(value) ((value >> (1*7)) & 0x7F)
#define SYNCBYTE4(value) ((value >> (0*7)) & 0x7F)
#define UNSYNC(b1,b2,b3,b4) (((b1 & 0x7F) << (3*7)) + ((b2 & 0x7F) << (2*7)) + ((b3 & 0x7F) << (1*7)) + ((b4 & 0x7F) << (0*7)))

#define TYPE_LOG   0
#define TYPE_DEBUG 1
#define TYPE_DIE   2

extern void checkid3tags(mp3entry *entry);
extern bool writemetadata(FILE *to, mp3entry *entry, int *metaflag);
extern void logmsg(char *fmt, ...);
extern void debug(int priority, char *fmt, ...); 
extern void die(char *fmt, ...);
