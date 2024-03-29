/*
	spritegn.h
	header file for sprite generation program

	$Id: spritegn.h,v 1.5 2007/03/14 21:03:42 sezero Exp $
*/

#ifndef __SPRITEGEN_H
#define __SPRITEGEN_H

// **********************************************************
// * This file must be identical in the spritegen directory *
// * and in the Quake directory, because it's used to       *
// * pass data from one to the other via .spr files.        *
// **********************************************************

//-------------------------------------------------------
// This program generates .spr sprite package files.
// The format of the files is as follows:
//
// dsprite_t file header structure
// <repeat dsprite_t.numframes times>
//   <if spritegroup, repeat dspritegroup_t.numframes times>
//     dspriteframe_t frame header structure
//     sprite bitmap
//   <else (single sprite frame)>
//     dspriteframe_t frame header structure
//     sprite bitmap
// <endrepeat>
//-------------------------------------------------------

#ifdef INCLUDELIBS

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "cmdlib.h"
#include "scriplib.h"
#include "dictlib.h"
#include "trilib.h"
#include "lbmlib.h"
#include "mathlib.h"

#endif

#define SPRITE_VERSION	1

// Little-endian "IDSP"
#define IDSPRITEHEADER	(('P'<<24)+('S'<<16)+('D'<<8)+'I')

// must match definition in genmodel.h
#ifndef SYNCTYPE_T
#define SYNCTYPE_T
typedef enum
{
	ST_SYNC = 0,
	ST_RAND
} synctype_t;
#endif

// TODO: shorten these?
typedef struct
{
	int			ident;
	int			version;
	int			type;
	float		boundingradius;
	int			width;
	int			height;
	int			numframes;
	float		beamlength;
	synctype_t	synctype;
} dsprite_t;

#define	SPR_VP_PARALLEL_UPRIGHT		0
#define	SPR_FACING_UPRIGHT		1
#define	SPR_VP_PARALLEL			2
#define	SPR_ORIENTED			3
#define	SPR_VP_PARALLEL_ORIENTED	4

typedef struct
{
	int			origin[2];
	int			width;
	int			height;
} dspriteframe_t;

typedef struct
{
	int			numframes;
} dspritegroup_t;

typedef struct
{
	float		interval;
} dspriteinterval_t;

typedef enum
{
	SPR_SINGLE = 0,
	SPR_GROUP
} spriteframetype_t;

typedef struct
{
	spriteframetype_t	type;
} dspriteframetype_t;

#endif	/* __SPRITEGEN_H */

