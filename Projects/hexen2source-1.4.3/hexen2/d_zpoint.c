/*
	d_zpoint.c
	software driver module for drawing z-buffered points

	$Id: d_zpoint.c,v 1.4 2007/03/14 21:03:06 sezero Exp $
*/

#include "quakedef.h"
#include "d_local.h"

/*
=====================
D_DrawZPoint
=====================
*/
void D_DrawZPoint (void)
{
	byte	*pdest;
	short	*pz;
	int		izi;

	pz = d_pzbuffer + (d_zwidth * r_zpointdesc.v) + r_zpointdesc.u;
	pdest = d_viewbuffer + d_scantable[r_zpointdesc.v] + r_zpointdesc.u;
	izi = (int)(r_zpointdesc.zi * 0x8000);

	if (*pz <= izi)
	{
		*pz = izi;
		*pdest = r_zpointdesc.color;
	}
}
