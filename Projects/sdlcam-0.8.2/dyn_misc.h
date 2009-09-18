#ifndef DYN_MISC_H
#define DYN_MISC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "SDL_v4l_image.h"
#include <stdlib.h>
#include "SDL_v4l_filters.h"

extern unsigned int  Level_R[256];
extern unsigned int  Level_G[256];
extern unsigned int  Level_B[256];
extern unsigned int  Level_A[256];
extern unsigned int  Level_med;
extern unsigned int  Level_avg;
extern unsigned char pallete_r[256]; // von range und brightness_contrast genutzt
extern unsigned char pallete_g[256];
extern unsigned char pallete_b[256];

const char *Dump_Path   (void);
const char *Dump_Prefix (void);
void Dump_Path_set   (const char *c);
void Dump_Prefix_set (const char *c);

void *xRealloc (void *p,size_t s);
void *xMalloc  (        size_t s);

int SDL_v4l_Buffer_Prepare (const SDL_v4l_image *SRC, SDL_v4l_image *DST);
void Get_Level(const SDL_v4l_image *SRC);

Filter_plugin *Find_Filter (const char *name);
int Call_Filter_bb  (const char *name, int mode, const SDL_v4l_image *SRC,                       SDL_v4l_image *DST);
int Call_Filter_b   (const char *name, int mode,                                                 SDL_v4l_image *DST);
int Call_Filter_bbb (const char *name, int mode, const SDL_v4l_image *A, const SDL_v4l_image *B, SDL_v4l_image *DST);
int Call_Filter_bbv (const char *name, int mode, const SDL_v4l_image *A, SDL_v4l_image *DST, unsigned int val);
int Toggle_Filter   (const char *name);
int Mode_Filter     (const char *name);

char *getNewFilename(const char *path, const char *prepend, const char *append);

#ifdef __cplusplus
	}
#endif


#endif

/*
 * vim600: fdm=marker
 */
