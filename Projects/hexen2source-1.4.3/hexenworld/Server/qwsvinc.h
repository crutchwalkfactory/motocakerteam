/*
	qwsvinc.h
	primary header for server

	FIXME:	kill this in the future and make each C
		file include only the necessary headers.

	$Id: qwsvinc.h,v 1.11 2007/12/14 16:41:14 sezero Exp $
*/

#ifndef __HWSVINC_H
#define __HWSVINC_H

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
#include "info_str.h"
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

#include "progs.h"
#include "pr_strng.h"
#include "cl_effect.h"
#include "model.h"

#include "server.h"
#include "world.h"
#include "pmove.h"

#endif	/* __HWSVINC_H */
