/*
	net.h
	quake's interface to the networking layer

	$Header: /cvsroot/uhexen2/hexenworld/Client/net.h,v 1.8 2007/12/21 15:05:23 sezero Exp $
*/

#ifndef __H2W_NET_H
#define __H2W_NET_H

#define	PORT_ANY	-1

typedef struct
{
	byte	ip[4];
	unsigned short	port;
	unsigned short	pad;
} netadr_t;

extern	netadr_t	net_local_adr;
extern	netadr_t	net_from;	// address of who sent the packet
extern	sizebuf_t	net_message;

extern	cvar_t	hostname;

extern	int		net_socket;

void		NET_Init (int port);
void		NET_Shutdown (void);
qboolean	NET_GetPacket (void);
void		NET_SendPacket (int length, void *data, netadr_t to);

qboolean	NET_CompareAdr (netadr_t a, netadr_t b);
const char	*NET_AdrToString (netadr_t a);
const char	*NET_BaseAdrToString (netadr_t a);
qboolean	NET_StringToAdr (const char *s, netadr_t *a);

//============================================================================

#define	OLD_AVG		0.99	// total = oldtotal*OLD_AVG + new*(1-OLD_AVG)

#define	MAX_LATENT	32

typedef struct
{
	qboolean	fatal_error;

	float	last_received;		// for timeouts

	// the statistics are cleared at each client begin, because
	// the server connecting process gives a bogus picture of the data

	float	frame_latency;		// rolling average
	float	frame_rate;

	int	drop_count;		// dropped packets, cleared each level
	int	good_count;		// cleared each level

	netadr_t	remote_address;

	// bandwidth estimator
	double	cleartime;		// if realtime > nc->cleartime, free to go
	double	rate;			// seconds / byte

	// sequencing variables
	int	incoming_sequence;
	int	incoming_acknowledged;
	int	incoming_reliable_acknowledged;	// single bit

	int	incoming_reliable_sequence;	// single bit, maintained local

	int	outgoing_sequence;
	int	reliable_sequence;		// single bit
	int	last_reliable_sequence;		// sequence number of last send

	// reliable staging and holding areas
	sizebuf_t	message;		// writing buffer to send to server
	byte		message_buf[MAX_MSGLEN];

	int	reliable_length;
	byte	reliable_buf[MAX_MSGLEN];	// unacked reliable message

	// time and size data to calculate bandwidth
	int	outgoing_size[MAX_LATENT];
	double	outgoing_time[MAX_LATENT];
} netchan_t;

extern	int	net_drop;		// packets dropped before this one

void Netchan_Init (void);
void Netchan_Transmit (netchan_t *chan, int length, byte *data);
void Netchan_OutOfBand (netadr_t adr, int length, byte *data);
void Netchan_OutOfBandPrint (netadr_t adr, const char *format, ...) __attribute__((format(printf,2,3)));
qboolean Netchan_Process (netchan_t *chan);
void Netchan_Setup (netchan_t *chan, netadr_t adr);

qboolean Netchan_CanPacket (netchan_t *chan);
qboolean Netchan_CanReliable (netchan_t *chan);

#endif	/* __H2W_NET_H */

