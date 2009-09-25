/*
	quakeinc.h
	primary header for client and server

	FIXME:	kill this in the future and make each C
		file include only the necessary headers.

	$Id: quakeinc.h,v 1.21 2008/03/31 12:01:27 sezero Exp $
*/

#ifndef __QUAKEINC_H
#define __QUAKEINC_H

/* include the system stdc headers:			*/
#include "q_stdinc.h"

/* include the compiler specific stuff	*/
#include "compiler.h"
/* include the OS/arch definitions, etc	*/
#include "arch_def.h"

/* make sure to include our compile time options first	*/
#include "h2option.h"

/* include the quake headers				*/

#include "q_endian.h"
#include "link_ops.h"
#include "sizebuf.h"
#include "msg_io.h"
#include "printsys.h"
#include "common.h"
#include "quakefs.h"
#include "bspfile.h"
#include "sys.h"
#include "zone.h"
#include "mathlib.h"
#include "cvar.h"

#include "protocol.h"
#include "net.h"

#include "cmd.h"
#include "crc.h"

#include "host.h"

#if !defined(SERVERONLY)
#include "console.h"
#include "vid.h"
#include "wad.h"
#include "draw.h"
#include "render.h"
#include "view.h"
#include "screen.h"
#include "sbar.h"
#include "q_sound.h"
#include "cdaudio.h"
#include "mididef.h"
#endif	/* !SERVERONLY */

#include "progs.h"
#include "pr_strng.h"
#include "cl_effect.h"

#if !defined(SERVERONLY)
#include "client.h"
#endif	/* SERVERONLY */
#include "server.h"

#if defined(GLQUAKE)
#include "glheader.h"
#include "gl_model.h"
#include "glquake.h"
#elif defined(SERVERONLY)
#include "server/model.h"
#else	/* sw client */
#include "model.h"
#include "d_iface.h"
#endif
#if !defined(SERVERONLY)
#include "r_part.h"
#endif	/* SERVERONLY */

#include "world.h"

#if !defined(SERVERONLY)
#include "input.h"
#include "keys.h"
#include "menu.h"
#endif	/* !SERVERONLY */

#endif	/* __QUAKEINC_H */
