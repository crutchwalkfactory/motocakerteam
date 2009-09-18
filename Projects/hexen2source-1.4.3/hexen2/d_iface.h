/*
	d_iface.h
	interface header file for rasterization driver modules

	$Id: d_iface.h,v 1.16 2008/03/30 12:25:16 sezero Exp $
*/

#ifndef __D_IFACE_H
#define __D_IFACE_H

#define WARP_WIDTH		320
#define WARP_HEIGHT		200

#define MAX_SKIN_HEIGHT		480

typedef struct
{
	float	u, v;
	float	s, t;
	float	zi;
} emitpoint_t;

/* particle enums and types: note that hexen2 and
   hexenworld versions of these are different!! */
#include "particle.h"

typedef struct polyvert_s {
	float	u, v, zi, s, t;
} polyvert_t;

typedef struct polydesc_s {
	int			numverts;
	float		nearzi;
	msurface_t	*pcurrentface;
	polyvert_t	*pverts;
} polydesc_t;

// !!! if this is changed, it must be changed in d_ifacea.h too !!!
typedef struct finalvert_s {
	int		v[6];		// u, v, s, t, l, 1/z
	int		flags;
	float	reserved;
} finalvert_t;

// !!! if this is changed, it must be changed in d_ifacea.h too !!!
typedef struct
{
	void			*pskin;
	maliasskindesc_t	*pskindesc;
	int			skinwidth;
	int			skinheight;
	mtriangle_t		*ptriangles;
	finalvert_t		*pfinalverts;
	int			numtriangles;
	int			drawtype;
	int			seamfixupX16;
} affinetridesc_t;

// !!! if this is changed, it must be changed in d_ifacea.h too !!!
typedef struct {
	float	u, v, zi, color;
} screenpart_t;

typedef struct
{
	int		nump;
	emitpoint_t	*pverts;	// there's room for an extra element at [nump], 
					//  if the driver wants to duplicate element [0] at
					//  element [nump] to avoid dealing with wrapping
	mspriteframe_t	*pspriteframe;
	vec3_t		vup, vright, vpn;	// in worldspace
	float		nearzi;
} spritedesc_t;

typedef struct
{
	int		u, v;
	float		zi;
	int		color;
} zpointdesc_t;

extern cvar_t	r_drawflat;
extern int		d_spanpixcount;
extern int		r_framecount;	// sequence # of current frame since Quake
					//  started
extern qboolean	r_drawpolys;		// 1 if driver wants clipped polygons
					//  rather than a span list
extern qboolean	r_drawculledpolys;	// 1 if driver wants clipped polygons that
					//  have been culled by the edge list
extern qboolean	r_worldpolysbacktofront;	// 1 if driver wants polygons
						//  delivered back to front rather
						//  than front to back
extern qboolean	r_recursiveaffinetriangles;	// true if a driver wants to use
						//  recursive triangular subdivison
						//  and vertex drawing via
						//  D_PolysetDrawFinalVerts() past
						//  a certain distance (normally 
						//  only used by the software
						//  driver)
extern float	r_aliasuvscale;		// scale-up factor for screen u and v
					//  on Alias vertices passed to driver
extern int	r_pixbytes;
extern qboolean	r_dowarp;

extern int	d_con_indirect;		// if 0, Quake will draw console directly
					//  to vid.buffer; if 1, Quake will
					//  draw console via D_DrawRect. Must be
					//  defined by the driver (vid_*.c)

extern affinetridesc_t	r_affinetridesc;
extern spritedesc_t	r_spritedesc;
extern zpointdesc_t	r_zpointdesc;
extern polydesc_t	r_polydesc;

extern vec3_t	r_pright, r_pup, r_ppn;


void D_DrawPoly (void);
void D_DrawSprite (void);
void D_DrawSurfaces (qboolean Translucent);


__ASM_FUNCS_BEGIN

void D_PolysetDraw (void);
void D_PolysetDrawT (void);
void D_PolysetDrawT2 (void);
void D_PolysetDrawT3 (void);
void D_PolysetDrawT5 (void);

void D_PolysetDrawFinalVerts (finalvert_t *p1, finalvert_t *p2, finalvert_t *p3);
void D_PolysetDrawFinalVertsT (finalvert_t *p1, finalvert_t *p2, finalvert_t *p3);
void D_PolysetDrawFinalVertsT2 (finalvert_t *p1, finalvert_t *p2, finalvert_t *p3);
void D_PolysetDrawFinalVertsT3 (finalvert_t *p1, finalvert_t *p2, finalvert_t *p3);
void D_PolysetDrawFinalVertsT5 (finalvert_t *p1, finalvert_t *p2, finalvert_t *p3);

#if id386
void D_DrawNonSubdiv (void);
void D_PolysetCalcGradients (int skinwidth);
void D_PolysetCalcGradientsT (int skinwidth);
void D_PolysetCalcGradientsT2 (int skinwidth);
void D_PolysetCalcGradientsT3 (int skinwidth);
void D_PolysetCalcGradientsT5 (int skinwidth);
void D_PolysetRecursiveTriangle (int *p1, int *p2, int *p3);
void D_PolysetScanLeftEdge (int height);
void D_PolysetScanLeftEdgeT (int height);
void D_PolysetScanLeftEdgeT2 (int height);
void D_PolysetScanLeftEdgeT3 (int height);
void D_PolysetScanLeftEdgeT5 (int height);
void D_DrawParticle1x1b (particle_t *pparticle);
#endif

void D_DrawParticle (particle_t *pparticle);

__ASM_FUNCS_END


void D_BeginDirectRect (int x, int y, byte *pbitmap, int width, int height);
void D_EndDirectRect (int x, int y, int width, int height);
void D_EnableBackBufferAccess (void);
void D_DisableBackBufferAccess (void);

void D_DrawZPoint (void);
void D_Init (void);
void D_ViewChanged (void);
void D_SetupFrame (void);
void D_TurnZOn (void);
void D_WarpScreen (void);

void D_FillRect (vrect_t *vrect, int color);
void D_DrawRect (void);
void D_UpdateRects (vrect_t *prect);

void D_StartParticles (void);
void D_EndParticles (void);

// currently for internal use only, and should be a do-nothing function in
// hardware drivers
// FIXME: this should go away
void D_PolysetUpdateTables (void);

// these are currently for internal use only, and should not be used by drivers
extern byte				*r_skysource;

// !!! must be kept the same as in quakeasm.h !!!
#define TRANSPARENT_COLOR	0xFF

extern void *acolormap;	// FIXME: should go away

//=======================================================================//

// callbacks to Quake

typedef struct
{
	pixel_t		*surfdat;	// destination for generated surface
	int		rowbytes;	// destination logical width in bytes
	msurface_t	*surf;		// description for surface to generate
	fixed8_t	lightadj[MAXLIGHTMAPS];
						// adjust for lightmap levels for dynamic lighting
	texture_t	*texture;	// corrected for animating textures
	int		surfmip;	// mipmapped ratio of surface texels / world pixels
	int		surfwidth;	// in mipmapped texels
	int		surfheight;	// in mipmapped texels
} drawsurf_t;

extern drawsurf_t	r_drawsurf;

void R_DrawSurface (void);
void R_GenTile (msurface_t *psurf, void *pdest);


// !!! if this is changed, it must be changed in d_ifacea.h too !!!
#define TURB_TEX_SIZE	64		// base turbulent texture size

// !!! if this is changed, it must be changed in d_ifacea.h too !!!
#define	CYCLE		128		// turbulent cycle size

#define TILE_SIZE	128		// size of textures generated by R_GenTiledSurf

#define SKYSHIFT	7
#define	SKYSIZE		(1 << SKYSHIFT)
#define SKYMASK		(SKYSIZE - 1)

extern float	skyspeed, skyspeed2;
extern float	skytime;

extern int	c_surf;
extern vrect_t	scr_vrect;

extern byte	*r_warpbuffer;

#endif	/* __D_IFACE_H */

