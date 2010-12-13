/*
	gl_warp.c
	sky and water polygons

	$Id: gl_warp.c,v 1.30 2008/02/07 09:27:22 sezero Exp $
*/

#include "quakedef.h"

extern	model_t	*loadmodel;

int		skytexturenum;

static GLuint	solidskytexture, alphaskytexture;
static float	speedscale;	// for top sky and bottom sky

static msurface_t	*warpface;

#define	SUBDIVIDE_SIZE	64

static void BoundPoly (int numverts, float *verts, vec3_t mins, vec3_t maxs)
{
	int		i, j;
	float	*v;

	mins[0] = mins[1] = mins[2] = 9999;
	maxs[0] = maxs[1] = maxs[2] = -9999;
	v = verts;
	for (i = 0; i < numverts; i++)
	{
		for (j = 0; j < 3; j++, v++)
		{
			if (*v < mins[j])
				mins[j] = *v;
			if (*v > maxs[j])
				maxs[j] = *v;
		}
	}
}

static void SubdividePolygon (int numverts, float *verts)
{
	int		i, j, k;
	vec3_t	mins, maxs;
	float	m;
	float	*v;
	vec3_t	front[64], back[64];
	int		f, b;
	float	dist[64];
	float	frac;
	glpoly_t	*poly;
	float	s, t;

	if (numverts > 60)
		Sys_Error ("numverts = %i", numverts);

	BoundPoly (numverts, verts, mins, maxs);

	for (i = 0; i < 3; i++)
	{
		m = (mins[i] + maxs[i]) * 0.5;
		m = SUBDIVIDE_SIZE * floor (m/SUBDIVIDE_SIZE + 0.5);
		if (maxs[i] - m < 8)
			continue;
		if (m - mins[i] < 8)
			continue;

		// cut it
		v = verts + i;
		for (j = 0; j < numverts; j++, v += 3)
			dist[j] = *v - m;

		// wrap cases
		dist[j] = dist[0];
		v -= i;
		VectorCopy (verts, v);

		f = b = 0;
		v = verts;
		for (j = 0; j < numverts; j++, v += 3)
		{
			if (dist[j] >= 0)
			{
				VectorCopy (v, front[f]);
				f++;
			}
			if (dist[j] <= 0)
			{
				VectorCopy (v, back[b]);
				b++;
			}
			if (dist[j] == 0 || dist[j+1] == 0)
				continue;
			if ( (dist[j] > 0) != (dist[j+1] > 0) )
			{
				// clip point
				frac = dist[j] / (dist[j] - dist[j+1]);
				for (k = 0; k < 3; k++)
					front[f][k] = back[b][k] = v[k] + frac*(v[3+k] - v[k]);
				f++;
				b++;
			}
		}

		SubdividePolygon (f, front[0]);
		SubdividePolygon (b, back[0]);
		return;
	}

	poly = (glpoly_t *) Hunk_AllocName (sizeof(glpoly_t) + (numverts-4) * VERTEXSIZE*sizeof(float), "subdv_poly");
	poly->next = warpface->polys;
	warpface->polys = poly;
	poly->numverts = numverts;
	for (i = 0; i < numverts; i++, verts += 3)
	{
		VectorCopy (verts, poly->verts[i]);
		s = DotProduct (verts, warpface->texinfo->vecs[0]);
		t = DotProduct (verts, warpface->texinfo->vecs[1]);
		poly->verts[i][3] = s;
		poly->verts[i][4] = t;
	}
}

/*
================
GL_SubdivideSurface

Breaks a polygon up along axial 64 unit
boundaries so that turbulent and sky warps
can be done reasonably.
================
*/
void GL_SubdivideSurface (msurface_t *fa)
{
	vec3_t		verts[64];
	int		numverts;
	int		i;
	int		lindex;
	float		*vec;

	warpface = fa;

	//
	// convert edges back to a normal polygon
	//
	numverts = 0;
	for (i = 0; i < fa->numedges; i++)
	{
		lindex = loadmodel->surfedges[fa->firstedge + i];

		if (lindex > 0)
			vec = loadmodel->vertexes[loadmodel->edges[lindex].v[0]].position;
		else
			vec = loadmodel->vertexes[loadmodel->edges[-lindex].v[1]].position;
		VectorCopy (vec, verts[numverts]);
		numverts++;
	}

	SubdividePolygon (numverts, verts[0]);
}

//=========================================================


// speed up sin calculations - Ed
static float	turbsin[] =
{
#include "gl_warp_sin.h"
};
#define TURBSCALE (256.0 / (2 * M_PI))

/*
=============
EmitWaterPolys

Does a water warp on the pre-fragmented glpoly_t chain
=============
*/
void EmitWaterPolys (msurface_t *fa)
{
	glpoly_t	*p;
	float		*v;
	int			i;
	float		s, t, os, ot;
	vec3_t		nv;	// waterripple

	if (gl_waterripple.value < 0)
		gl_waterripple.value = 0;
	else if (gl_waterripple.value > 10)
		gl_waterripple.value = 10;

	for (p = fa->polys ; p ; p = p->next)
	{
		glBegin_fp (GL_POLYGON);
		for (i = 0, v = p->verts[0]; i < p->numverts; i++, v += VERTEXSIZE)
		{
			os = v[3];
			ot = v[4];

			nv[0] = v[0];
			nv[1] = v[1];
			nv[2] = v[2] + gl_waterripple.value*sin(v[0]*0.05 + realtime)*sin(v[2]*0.05 + realtime);

			s = os + turbsin[(int)((ot*0.125 + realtime) * TURBSCALE) & 255];
			s *= (1.0/64);

			t = ot + turbsin[(int)((os*0.125 + realtime) * TURBSCALE) & 255];
			t *= (1.0/64);

			glTexCoord2f_fp (s, t);
			//glVertex3fv_fp (v);
			glVertex3fv_fp (nv);
		}
		glEnd_fp ();
	}
}


/*
=============
EmitSkyPolys
=============
*/
static void EmitSkyPolysMulti (msurface_t *fa)
{
	glpoly_t	*p;
	float		*v;
	int		i;
	float	s, ss, t, tt;
	vec3_t		dir;
	float		length;

	glTexEnvf_fp(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	GL_Bind (solidskytexture);

	glActiveTextureARB_fp (GL_TEXTURE1_ARB);
	glTexEnvf_fp(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glEnable_fp(GL_TEXTURE_2D);
	GL_Bind (alphaskytexture);

	for (p = fa->polys ; p ; p = p->next)
	{
		glBegin_fp (GL_POLYGON);
		for (i = 0, v = p->verts[0]; i < p->numverts; i++, v += VERTEXSIZE)
		{
			VectorSubtract (v, r_origin, dir);
			dir[2] *= 3;	// flatten the sphere

			length = dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2];
			length = sqrt (length);
			length = 6*63/length;

			dir[0] *= length;
			dir[1] *= length;

			s = (realtime*8 + dir[0]) * (1.0/128);
			t = (realtime*8 + dir[1]) * (1.0/128);

			ss = (realtime*16 + dir[0]) * (1.0/128);
			tt = (realtime*16 + dir[1]) * (1.0/128);

			glMultiTexCoord2fARB_fp (GL_TEXTURE0_ARB, s, t);
			glMultiTexCoord2fARB_fp (GL_TEXTURE1_ARB, ss, tt);
			glVertex3fv_fp (v);
		}
		glEnd_fp ();
	}

	glDisable_fp(GL_TEXTURE_2D);
	glActiveTextureARB_fp (GL_TEXTURE0_ARB);
}

static void EmitSkyPolys (msurface_t *fa)
{
	glpoly_t	*p;
	float		*v;
	int		i;
	float		s, t;
	vec3_t		dir;
	float		length;

	for (p = fa->polys ; p ; p = p->next)
	{
		glBegin_fp (GL_POLYGON);
		for (i = 0, v = p->verts[0]; i < p->numverts; i++, v += VERTEXSIZE)
		{
			VectorSubtract (v, r_origin, dir);
			dir[2] *= 3;	// flatten the sphere

			length = dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2];
			length = sqrt (length);
			length = 6*63/length;

			dir[0] *= length;
			dir[1] *= length;

			s = (speedscale + dir[0]) * (1.0/128);
			t = (speedscale + dir[1]) * (1.0/128);

			glTexCoord2f_fp (s, t);
			glVertex3fv_fp (v);
		}
		glEnd_fp ();
	}
}

/*
===============
EmitBothSkyLayers

Does a sky warp on the pre-fragmented glpoly_t chain
This will be called for brushmodels, the world
will have them chained together.
===============
*/
void EmitBothSkyLayers (msurface_t *fa)
{
	// note: 3dfx doesn't like GL_DECAL
	if (!is_3dfx && gl_multitexture.integer && gl_mtexable)
	{
		EmitSkyPolysMulti (fa);
		return;
	}

	GL_Bind(solidskytexture);
	speedscale = realtime*8;
	speedscale -= (int)speedscale & ~127;

	EmitSkyPolys (fa);

	glEnable_fp (GL_BLEND);
	glTexEnvf_fp(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glColor4f_fp(1.0f, 1.0f, 1.0f, r_skyalpha.value);
	GL_Bind (alphaskytexture);
	speedscale = realtime*16;
	speedscale -= (int)speedscale & ~127;

	EmitSkyPolys (fa);

	glDisable_fp (GL_BLEND);
	glTexEnvf_fp(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

#ifndef QUAKE2
/*
=================
R_DrawSkyChain
=================
*/
void R_DrawSkyChain (msurface_t *s)
{
	msurface_t	*fa;

	// note: 3dfx doesn't like GL_DECAL
	if (!is_3dfx && gl_multitexture.integer && gl_mtexable)
	{
		for (fa = s ; fa ; fa = fa->texturechain)
			EmitSkyPolysMulti (fa);
		return;
	}

	GL_Bind(solidskytexture);
	speedscale = realtime*8;
	speedscale -= (int)speedscale & ~127;

	for (fa = s ; fa ; fa = fa->texturechain)
		EmitSkyPolys (fa);

	glEnable_fp (GL_BLEND);
	glTexEnvf_fp(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glColor4f_fp(1.0f, 1.0f, 1.0f, r_skyalpha.value);
	GL_Bind (alphaskytexture);
	speedscale = realtime*16;
	speedscale -= (int)speedscale & ~127;

	for (fa = s ; fa ; fa = fa->texturechain)
		EmitSkyPolys (fa);

	glDisable_fp (GL_BLEND);
	glTexEnvf_fp(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

#endif


/*
=================================================================

Quake 2 environment sky

=================================================================
*/

#ifdef QUAKE2
static GLuint	sky_tex[6];
/*
=================================================================

PCX Loading

=================================================================
*/

typedef struct
{
	char	manufacturer;
	char	version;
	char	encoding;
	char	bits_per_pixel;
	unsigned short	xmin,ymin,xmax,ymax;
	unsigned short	hres,vres;
	unsigned char	palette[48];
	char	reserved;
	char	color_planes;
	unsigned short	bytes_per_line;
	unsigned short	palette_type;
	char	filler[58];
	unsigned int	data;	// unbounded
} pcx_t;

static byte	*pcx_rgb;

/*
============
LoadPCX
============
*/
void LoadPCX (FILE *f)
{
	pcx_t	*pcx, pcxbuf;
	byte	palette[768];
	byte	*pix;
	int		x, y;
	int		dataByte, runLength;
	int		count;

//
// parse the PCX file
//
	fread (&pcxbuf, 1, sizeof(pcxbuf), f);

	pcx = &pcxbuf;

	if (pcx->manufacturer != 0x0a
		|| pcx->version != 5
		|| pcx->encoding != 1
		|| pcx->bits_per_pixel != 8
		|| pcx->xmax >= 320
		|| pcx->ymax >= 256)
	{
		Con_Printf ("Bad pcx file\n");
		return;
	}

	// seek to palette
	fseek (f, -768, SEEK_END);
	fread (palette, 1, 768, f);

	fseek (f, sizeof(pcxbuf) - 4, SEEK_SET);

	count = (pcx->xmax + 1) * (pcx->ymax + 1);
	pcx_rgb = Hunk_AllocName(count * 4, "pcxfile_data");

	for (y = 0 ; y <= pcx->ymax ; y++)
	{
		pix = pcx_rgb + 4*y*(pcx->xmax + 1);
		for (x = 0 ; x <= pcx->ymax ; )
		{
			dataByte = fgetc(f);

			if ((dataByte & 0xC0) == 0xC0)
			{
				runLength = dataByte & 0x3F;
				dataByte = fgetc(f);
			}
			else
				runLength = 1;

			while (runLength-- > 0)
			{
				pix[0] = palette[dataByte*3];
				pix[1] = palette[dataByte*3+1];
				pix[2] = palette[dataByte*3+2];
				pix[3] = 255;
				pix += 4;
				x++;
			}
		}
	}
}

/*
=========================================================

TARGA LOADING

=========================================================
*/

typedef struct _TargaHeader
{
	unsigned char	id_length, colormap_type, image_type;
	unsigned short	colormap_index, colormap_length;
	unsigned char	colormap_size;
	unsigned short	x_origin, y_origin, width, height;
	unsigned char	pixel_size, attributes;
} TargaHeader;

static TargaHeader	targa_header;
static byte		*targa_rgba;

int fgetLittleShort (FILE *f)
{
	byte	b1, b2;

	b1 = fgetc(f);
	b2 = fgetc(f);

	return (short)(b1 + b2*256);
}

int fgetLittleLong (FILE *f)
{
	byte	b1, b2, b3, b4;

	b1 = fgetc(f);
	b2 = fgetc(f);
	b3 = fgetc(f);
	b4 = fgetc(f);

	return b1 + (b2<<8) + (b3<<16) + (b4<<24);
}


/*
=============
LoadTGA
=============
*/
void LoadTGA (FILE *fin)
{
	int		columns, rows, numPixels;
	byte	*pixbuf;
	int		row, column;

	targa_header.id_length = fgetc(fin);
	targa_header.colormap_type = fgetc(fin);
	targa_header.image_type = fgetc(fin);

	targa_header.colormap_index = fgetLittleShort(fin);
	targa_header.colormap_length = fgetLittleShort(fin);
	targa_header.colormap_size = fgetc(fin);
	targa_header.x_origin = fgetLittleShort(fin);
	targa_header.y_origin = fgetLittleShort(fin);
	targa_header.width = fgetLittleShort(fin);
	targa_header.height = fgetLittleShort(fin);
	targa_header.pixel_size = fgetc(fin);
	targa_header.attributes = fgetc(fin);

	if (targa_header.image_type != 2 && targa_header.image_type != 10)
		Sys_Error ("%s: Only type 2 and 10 targa RGB images supported", __thisfunc__);

	if ((targa_header.pixel_size != 32 && targa_header.pixel_size != 24) ||
			targa_header.colormap_type !=0)
		Sys_Error ("%s: Only 32 or 24 bit images supported (no colormaps)", __thisfunc__);

	columns = targa_header.width;
	rows = targa_header.height;
	numPixels = columns * rows;

	targa_rgba = Hunk_AllocName(numPixels * 4, "tgafile_data");

	if (targa_header.id_length != 0)	// skip TARGA image comment
		fseek(fin, targa_header.id_length, SEEK_CUR);

	if (targa_header.image_type == 2)
	{
	// Uncompressed, RGB images
		for (row = rows-1; row >= 0; row--)
		{
			pixbuf = targa_rgba + row*columns*4;
			for (column = 0; column < columns; column++)
			{
				unsigned char	red, green, blue, alphabyte;
				switch (targa_header.pixel_size)
				{
				case 24:
					blue = getc(fin);
					green = getc(fin);
					red = getc(fin);
					*pixbuf++ = red;
					*pixbuf++ = green;
					*pixbuf++ = blue;
					*pixbuf++ = 255;
					break;
				case 32:
					blue = getc(fin);
					green = getc(fin);
					red = getc(fin);
					alphabyte = getc(fin);
					*pixbuf++ = red;
					*pixbuf++ = green;
					*pixbuf++ = blue;
					*pixbuf++ = alphabyte;
					break;
				}
			}
		}
	}
	else if (targa_header.image_type == 10)
	{
	// Runlength encoded RGB images
		unsigned char	red, green, blue, alphabyte;
		unsigned char	packetHeader, packetSize, j;
		for (row = rows-1; row >= 0; row--)
		{
			pixbuf = targa_rgba + row*columns*4;
			for (column = 0 ; column < columns ; )
			{
				packetHeader = getc(fin);
				packetSize = 1 + (packetHeader & 0x7f);
				if (packetHeader & 0x80)
				{	// run-length packet
					switch (targa_header.pixel_size)
					{
					case 24:
						blue = getc(fin);
						green = getc(fin);
						red = getc(fin);
						alphabyte = 255;
						break;
					case 32:
						blue = getc(fin);
						green = getc(fin);
						red = getc(fin);
						alphabyte = getc(fin);
						break;
					}

					for (j = 0; j < packetSize; j++)
					{
						*pixbuf++ = red;
						*pixbuf++ = green;
						*pixbuf++ = blue;
						*pixbuf++ = alphabyte;
						column++;
						if (column == columns)
						{	// run spans across rows
							column = 0;
							if (row > 0)
								row--;
							else
								goto breakOut;
							pixbuf = targa_rgba + row*columns*4;
						}
					}
				}
				else
				{	// non run-length packet
					for (j = 0; j < packetSize; j++)
					{
						switch (targa_header.pixel_size)
						{
						case 24:
							blue = getc(fin);
							green = getc(fin);
							red = getc(fin);
							*pixbuf++ = red;
							*pixbuf++ = green;
							*pixbuf++ = blue;
							*pixbuf++ = 255;
							break;
						case 32:
							blue = getc(fin);
							green = getc(fin);
							red = getc(fin);
							alphabyte = getc(fin);
							*pixbuf++ = red;
							*pixbuf++ = green;
							*pixbuf++ = blue;
							*pixbuf++ = alphabyte;
							break;
						}
						column++;
						if (column == columns)
						{	// pixel packet run spans across rows
							column = 0;
							if (row > 0)
								row--;
							else
								goto breakOut;
							pixbuf = targa_rgba + row*columns*4;
						}
					}
				}
			}
			breakOut:;
		}
	}

	fclose(fin);
}


/*
==================
R_LoadSkys
==================
*/

static char	*suf[6] = {"rt", "bk", "lf", "ft", "up", "dn"};

void R_LoadSkys (void)
{
	int	i, mark;
	FILE	*f;
	char	name[64], texname[20];

	for (i = 0; i < 6; i++)
	{
		q_snprintf (name, sizeof(name), "gfx/env/bkgtst%s.tga", suf[i]);
		FS_OpenFile (name, &f, false);
		if (!f)
		{
			Con_Printf ("Couldn't load %s\n", name);
			continue;
		}

		mark = Hunk_LowMark();
		LoadTGA (f);
	//	LoadPCX (f);

		q_snprintf(texname, sizeof(texname), "skybox%i", i);
		sky_tex[i] = GL_LoadTexture(texname, 256, 256, targa_rgba, false, false, 0, true);
		Hunk_FreeToLowMark(mark);

		glTexParameterf_fp(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter_max);
		glTexParameterf_fp(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter_max);
	}
}


static vec3_t	skyclip[6] =
{
	{  1,  1,  0 },
	{  1, -1,  0 },
	{  0, -1,  1 },
	{  0,  1,  1 },
	{  1,  0,  1 },
	{ -1,  0,  1 }
};

//int	c_sky;

// 1 = s, 2 = t, 3 = 2048
static int	st_to_vec[6][3] =
{
	{  3, -1,  2 },
	{ -3,  1,  2 },

	{  1,  3,  2 },
	{ -1, -3,  2 },

	{ -2, -1,  3 },	// 0 degrees yaw, look straight up
	{  2, -1, -3 }	// look straight down

//	{ -1,  2,  3 },
//	{  1,  2, -3 }
};

// s = [0]/[2], t = [1]/[2]
static int	vec_to_st[6][3] =
{
	{ -2,  3,  1 },
	{  2,  3, -1 },

	{  1,  3,  2 },
	{ -1,  3, -2 },

	{ -2, -1,  3 },
	{ -2,  1, -3 }

//	{ -1,  2,  3 },
//	{  1,  2, -3 }
};

static float	skymins[2][6], skymaxs[2][6];

static void DrawSkyPolygon (int nump, vec3_t vecs)
{
	int		i, j;
	vec3_t	v, av;
	float	s, t, dv;
	int		axis;
	float	*vp;

//	c_sky++;
#if 0
	glBegin_fp (GL_POLYGON);
	for (i = 0; i < nump; i++, vecs += 3)
	{
		VectorAdd(vecs, r_origin, v);
		glVertex3fv_fp (v);
	}
	glEnd_fp();
	return;
#endif
	// decide which face it maps to
	VectorClear (v);
	for (i = 0, vp = vecs; i < nump; i++, vp += 3)
	{
		VectorAdd (vp, v, v);
	}
	av[0] = fabs(v[0]);
	av[1] = fabs(v[1]);
	av[2] = fabs(v[2]);
	if (av[0] > av[1] && av[0] > av[2])
	{
		if (v[0] < 0)
			axis = 1;
		else
			axis = 0;
	}
	else if (av[1] > av[2] && av[1] > av[0])
	{
		if (v[1] < 0)
			axis = 3;
		else
			axis = 2;
	}
	else
	{
		if (v[2] < 0)
			axis = 5;
		else
			axis = 4;
	}

	// project new texture coords
	for (i = 0; i < nump; i++, vecs += 3)
	{
		j = vec_to_st[axis][2];
		if (j > 0)
			dv = vecs[j - 1];
		else
			dv = -vecs[-j - 1];

		j = vec_to_st[axis][0];
		if (j < 0)
			s = -vecs[-j -1] / dv;
		else
			s = vecs[j-1] / dv;

		j = vec_to_st[axis][1];
		if (j < 0)
			t = -vecs[-j -1] / dv;
		else
			t = vecs[j-1] / dv;

		if (s < skymins[0][axis])
			skymins[0][axis] = s;
		if (t < skymins[1][axis])
			skymins[1][axis] = t;
		if (s > skymaxs[0][axis])
			skymaxs[0][axis] = s;
		if (t > skymaxs[1][axis])
			skymaxs[1][axis] = t;
	}
}

#define	MAX_CLIP_VERTS	64
static void ClipSkyPolygon (int nump, vec3_t vecs, int stage)
{
	float	*norm;
	float	*v;
	qboolean	front, back;
	float	d, e;
	float	dists[MAX_CLIP_VERTS];
	int		sides[MAX_CLIP_VERTS];
	vec3_t	newv[2][MAX_CLIP_VERTS];
	int		newc[2];
	int		i, j;

	if (nump > MAX_CLIP_VERTS-2)
		Sys_Error ("%s: MAX_CLIP_VERTS", __thisfunc__);
	if (stage == 6)
	{	// fully clipped, so draw it
		DrawSkyPolygon (nump, vecs);
		return;
	}

	front = back = false;
	norm = skyclip[stage];
	for (i = 0, v = vecs; i < nump; i++, v += 3)
	{
		d = DotProduct (v, norm);
		if (d > ON_EPSILON)
		{
			front = true;
			sides[i] = SIDE_FRONT;
		}
		else if (d < ON_EPSILON)
		{
			back = true;
			sides[i] = SIDE_BACK;
		}
		else
			sides[i] = SIDE_ON;
		dists[i] = d;
	}

	if (!front || !back)
	{	// not clipped
		ClipSkyPolygon (nump, vecs, stage+1);
		return;
	}

	// clip it
	sides[i] = sides[0];
	dists[i] = dists[0];
	VectorCopy (vecs, (vecs+(i*3)) );
	newc[0] = newc[1] = 0;

	for (i = 0, v = vecs; i < nump; i++, v += 3)
	{
		switch (sides[i])
		{
		case SIDE_FRONT:
			VectorCopy (v, newv[0][newc[0]]);
			newc[0]++;
			break;
		case SIDE_BACK:
			VectorCopy (v, newv[1][newc[1]]);
			newc[1]++;
			break;
		case SIDE_ON:
			VectorCopy (v, newv[0][newc[0]]);
			newc[0]++;
			VectorCopy (v, newv[1][newc[1]]);
			newc[1]++;
			break;
		}

		if (sides[i] == SIDE_ON || sides[i+1] == SIDE_ON || sides[i+1] == sides[i])
			continue;

		d = dists[i] / (dists[i] - dists[i+1]);
		for (j = 0; j < 3; j++)
		{
			e = v[j] + d*(v[j+3] - v[j]);
			newv[0][newc[0]][j] = e;
			newv[1][newc[1]][j] = e;
		}
		newc[0]++;
		newc[1]++;
	}

	// continue
	ClipSkyPolygon (newc[0], newv[0][0], stage+1);
	ClipSkyPolygon (newc[1], newv[1][0], stage+1);
}

/*
=================
R_DrawSkyChain
=================
*/
void R_DrawSkyChain (msurface_t *s)
{
	msurface_t	*fa;
	int		i;
	vec3_t	verts[MAX_CLIP_VERTS];
	glpoly_t	*p;

//	c_sky = 0;
	GL_Bind(solidskytexture);

	// calculate vertex values for sky box
	for (fa = s ; fa ; fa = fa->texturechain)
	{
		for (p = fa->polys ; p ; p = p->next)
		{
			for (i = 0; i < p->numverts; i++)
			{
				VectorSubtract (p->verts[i], r_origin, verts[i]);
			}
			ClipSkyPolygon (p->numverts, verts[0], 0);
		}
	}
}


/*
==============
R_ClearSkyBox
==============
*/
void R_ClearSkyBox (void)
{
	int		i;

	for (i = 0; i < 6; i++)
	{
		skymins[0][i] = skymins[1][i] = 9999;
		skymaxs[0][i] = skymaxs[1][i] = -9999;
	}
}


static void MakeSkyVec (float s, float t, int axis)
{
	vec3_t	v, b;
	int		j, k;

	b[0] = s*2048;
	b[1] = t*2048;
	b[2] = 2048;

	for (j = 0; j < 3; j++)
	{
		k = st_to_vec[axis][j];
		if (k < 0)
			v[j] = -b[-k - 1];
		else
			v[j] = b[k - 1];
		v[j] += r_origin[j];
	}

	// avoid bilerp seam
	s = (s + 1)*0.5;
	t = (t + 1)*0.5;

	if (s < 1.0/512)
		s = 1.0/512;
	else if (s > 511.0/512)
		s = 511.0/512;
	if (t < 1.0/512)
		t = 1.0/512;
	else if (t > 511.0/512)
		t = 511.0/512;

	t = 1.0 - t;
	glTexCoord2f_fp (s, t);
	glVertex3fv_fp (v);
}

/*
==============
R_DrawSkyBox
==============
*/
static int	skytexorder[6] = {0, 2, 1, 3, 4, 5};

void R_DrawSkyBox (void)
{
	int		i;

#if 0
	glEnable_fp (GL_BLEND);
	glTexEnvf_fp(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glColor4f_fp (1,1,1,0.5);
	glDisable_fp (GL_DEPTH_TEST);
#endif
	for (i = 0; i < 6; i++)
	{
		if ( (skymins[0][i] >= skymaxs[0][i]) || (skymins[1][i] >= skymaxs[1][i]) )
			continue;

		GL_Bind(sky_tex[skytexorder[i]]);
#if 0
		skymins[0][i] = -1;
		skymins[1][i] = -1;
		skymaxs[0][i] = 1;
		skymaxs[1][i] = 1;
#endif
		glBegin_fp (GL_QUADS);
		MakeSkyVec (skymins[0][i], skymins[1][i], i);
		MakeSkyVec (skymins[0][i], skymaxs[1][i], i);
		MakeSkyVec (skymaxs[0][i], skymaxs[1][i], i);
		MakeSkyVec (skymaxs[0][i], skymins[1][i], i);
		glEnd_fp ();
	}
#if 0
	glDisable_fp (GL_BLEND);
	glTexEnvf_fp(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glColor4f_fp (1,1,1,0.5);
	glEnable_fp (GL_DEPTH_TEST);
#endif
}


#endif	// end of Quake2 sky

//===============================================================


/*
=============
R_InitSky

A sky texture is 256*128, with the right side being a masked overlay
==============
*/
void R_InitSky (texture_t *mt)
{
	int		i, j, p;
	byte		*src;
	unsigned int	trans[128*128];
	unsigned int	transpix;
	int		r, g, b;
	unsigned int	*rgba;

	src = (byte *)mt + mt->offsets[0];

	// make an average value for the back to avoid
	// a fringe on the top level

	r = g = b = 0;
	for (i = 0; i < 128; i++)
	{
		for (j = 0; j < 128; j++)
		{
			p = src[i*256 + j + 128];
			rgba = &d_8to24table[p];
			trans[(i*128) + j] = *rgba;
			r += ((byte *)rgba)[0];
			g += ((byte *)rgba)[1];
			b += ((byte *)rgba)[2];
		}
	}

	((byte *)&transpix)[0] = r / (128*128);
	((byte *)&transpix)[1] = g / (128*128);
	((byte *)&transpix)[2] = b / (128*128);
	((byte *)&transpix)[3] = 0;

	solidskytexture = GL_LoadTexture("upsky", 128, 128, (byte *)trans, false, false, 0, true);

	for (i = 0; i < 128; i++)
	{
		for (j = 0; j < 128; j++)
		{
			p = src[i*256 + j];
			if (p == 0)
				trans[(i*128) + j] = transpix;
			else
				trans[(i*128) + j] = d_8to24table[p];
		}
	}

	alphaskytexture = GL_LoadTexture("lowsky", 128, 128, (byte *)trans, false, true, 0, true);
}
