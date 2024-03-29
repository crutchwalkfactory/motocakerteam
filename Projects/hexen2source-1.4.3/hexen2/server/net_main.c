/*
	net_main.c
	main networking module

	$Header: /cvsroot/uhexen2/hexen2/server/net_main.c,v 1.22 2007/11/11 13:17:42 sezero Exp $
*/

#include "quakedef.h"

qsocket_t	*net_activeSockets = NULL;
qsocket_t	*net_freeSockets = NULL;
int		net_numsockets = 0;

qboolean	serialAvailable = false;
qboolean	ipxAvailable = false;
qboolean	tcpipAvailable = false;

int		net_hostport;
int		DEFAULTnet_hostport = 26900;

char		my_ipx_address[NET_NAMELEN];
char		my_tcpip_address[NET_NAMELEN];

sizebuf_t	net_message;
//static byte	net_message_buffer[NET_MAXMESSAGE];
int		net_activeconnections		= 0;

int		messagesSent			= 0;
int		messagesReceived		= 0;
int		unreliableMessagesSent		= 0;
int		unreliableMessagesReceived	= 0;

static	cvar_t	net_messagetimeout = {"net_messagetimeout", "300", CVAR_NONE};
cvar_t	hostname = {"hostname", "UNNAMED", CVAR_NONE};

cvar_t	net_allowmultiple = {"net_allowmultiple", "0", CVAR_ARCHIVE};

#if NET_USE_SERIAL
static qboolean	configRestored = false;

void (*GetComPortConfig) (int portNumber, int *port, int *irq, int *baud, qboolean *useModem);
void (*SetComPortConfig) (int portNumber, int port, int irq, int baud, qboolean useModem);
void (*GetModemConfig) (int portNumber, char *dialType, char *clear, char *init, char *hangup);
void (*SetModemConfig) (int portNumber, const char *dialType, const char *clear, const char *init, const char *hangup);

cvar_t	config_com_port = {"_config_com_port", "0x3f8", CVAR_ARCHIVE};
cvar_t	config_com_irq = {"_config_com_irq", "4", CVAR_ARCHIVE};
cvar_t	config_com_baud = {"_config_com_baud", "57600", CVAR_ARCHIVE};
cvar_t	config_com_modem = {"_config_com_modem", "1", CVAR_ARCHIVE};
cvar_t	config_modem_dialtype = {"_config_modem_dialtype", "T", CVAR_ARCHIVE};
cvar_t	config_modem_clear = {"_config_modem_clear", "ATZ", CVAR_ARCHIVE};
cvar_t	config_modem_init = {"_config_modem_init", "", CVAR_ARCHIVE};
cvar_t	config_modem_hangup = {"_config_modem_hangup", "AT H", CVAR_ARCHIVE};
#endif	/* NET_USE_SERIAL */

// these two macros are to make the code more readable
#define sfunc	net_drivers[sock->driver]
#define dfunc	net_drivers[net_driverlevel]

int		net_driverlevel;

double		net_time;


double SetNetTime (void)
{
	net_time = Sys_DoubleTime();
	return net_time;
}


/*
===================
NET_NewQSocket

Called by drivers when a new communications endpoint is required
The sequence and buffer fields will be filled in properly
===================
*/
qsocket_t *NET_NewQSocket (void)
{
	qsocket_t	*sock;

	if (net_freeSockets == NULL)
		return NULL;

	if (net_activeconnections >= svs.maxclients)
		return NULL;

	// get one from free list
	sock = net_freeSockets;
	net_freeSockets = sock->next;

	// add it to active list
	sock->next = net_activeSockets;
	net_activeSockets = sock;

	sock->disconnected = false;
	sock->connecttime = net_time;
	strcpy (sock->address,"UNSET ADDRESS");
	sock->driver = net_driverlevel;
	sock->socket = 0;
	sock->driverdata = NULL;
	sock->canSend = true;
	sock->sendNext = false;
	sock->lastMessageTime = net_time;
	sock->ackSequence = 0;
	sock->sendSequence = 0;
	sock->unreliableSendSequence = 0;
	sock->sendMessageLength = 0;
	sock->receiveSequence = 0;
	sock->unreliableReceiveSequence = 0;
	sock->receiveMessageLength = 0;

	return sock;
}


void NET_FreeQSocket(qsocket_t *sock)
{
	qsocket_t	*s;

	// remove it from active list
	if (sock == net_activeSockets)
		net_activeSockets = net_activeSockets->next;
	else
	{
		for (s = net_activeSockets; s; s = s->next)
		{
			if (s->next == sock)
			{
				s->next = sock->next;
				break;
			}
		}

		if (!s)
			Sys_Error ("%s: not active", __thisfunc__);
	}

	// add it to free list
	sock->next = net_freeSockets;
	net_freeSockets = sock;
	sock->disconnected = true;
}


/*
===================
NET_CheckNewConnections
===================
*/
qsocket_t *NET_CheckNewConnections (void)
{
	qsocket_t	*ret;

	SetNetTime();

	for (net_driverlevel = 0; net_driverlevel < net_numdrivers; net_driverlevel++)
	{
		if (net_drivers[net_driverlevel].initialized == false)
			continue;
		ret = dfunc.CheckNewConnections ();
		if (ret)
		{
			return ret;
		}
	}

	return NULL;
}

/*
===================
NET_Close
===================
*/
void NET_Close (qsocket_t *sock)
{
	if (!sock)
		return;

	if (sock->disconnected)
		return;

	SetNetTime();

	// call the driver_Close function
	sfunc.Close (sock);

	NET_FreeQSocket(sock);
}


/*
=================
NET_GetMessage

If there is a complete message, return it in net_message

returns 0 if no data is waiting
returns 1 if a message was received
returns -1 if connection is invalid
=================
*/
int	NET_GetMessage (qsocket_t *sock)
{
	int ret;

	if (!sock)
		return -1;

	if (sock->disconnected)
	{
		Con_Printf("%s: disconnected socket\n", __thisfunc__);
		return -1;
	}

	SetNetTime();

	ret = sfunc.QGetMessage(sock);

	// see if this connection has timed out
	if (ret == 0)
	{
		if (net_time - sock->lastMessageTime > net_messagetimeout.value)
		{
			NET_Close(sock);
			return -1;
		}
	}

	if (ret > 0)
	{
			sock->lastMessageTime = net_time;
			if (ret == 1)
				messagesReceived++;
			else if (ret == 2)
				unreliableMessagesReceived++;
	}

	return ret;
}


/*
==================
NET_SendMessage

Try to send a complete length+message unit over the reliable stream.
returns 0 if the message cannot be delivered reliably, but the connection
		is still considered valid
returns 1 if the message was sent properly
returns -1 if the connection died
==================
*/
int NET_SendMessage (qsocket_t *sock, sizebuf_t *data)
{
	int		r;

	if (!sock)
		return -1;

	if (sock->disconnected)
	{
		Con_Printf("%s: disconnected socket\n", __thisfunc__);
		return -1;
	}

	SetNetTime();
	r = sfunc.QSendMessage(sock, data);
	if (r == 1)
		messagesSent++;

	return r;
}


int NET_SendUnreliableMessage (qsocket_t *sock, sizebuf_t *data)
{
	int		r;

	if (!sock)
		return -1;

	if (sock->disconnected)
	{
		Con_Printf("%s: disconnected socket\n", __thisfunc__);
		return -1;
	}

	SetNetTime();
	r = sfunc.SendUnreliableMessage(sock, data);
	if (r == 1)
		unreliableMessagesSent++;

	return r;
}


/*
==================
NET_CanSendMessage

Returns true or false if the given qsocket can currently accept a
message to be transmitted.
==================
*/
qboolean NET_CanSendMessage (qsocket_t *sock)
{
	if (!sock)
		return false;

	if (sock->disconnected)
		return false;

	SetNetTime();

	return sfunc.CanSendMessage(sock);
}


int NET_SendToAll (sizebuf_t *data, double blocktime)
{
	double		start;
	int			i;
	int			count = 0;
	qboolean	msg_init[MAX_CLIENTS];	/* did we write the message to the client's connection	*/
	qboolean	msg_sent[MAX_CLIENTS];	/* did the msg arrive its destination (canSend state).	*/

	for (i = 0, host_client = svs.clients; i < svs.maxclients; i++, host_client++)
	{
		if (host_client->netconnection && host_client->active)
		{
			count++;
			msg_init[i] = false;
			msg_sent[i] = false;
		}
		else
		{
			msg_init[i] = true;
			msg_sent[i] = true;
		}
	}

	start = Sys_DoubleTime();
	while (count)
	{
		count = 0;
		for (i = 0, host_client = svs.clients; i < svs.maxclients; i++, host_client++)
		{
			if (! msg_init[i])
			{
				if (NET_CanSendMessage (host_client->netconnection))
				{
					msg_init[i] = true;
					NET_SendMessage(host_client->netconnection, data);
				}
				else
				{
					NET_GetMessage (host_client->netconnection);
				}
				count++;
				continue;
			}

			if (! msg_sent[i])
			{
				if (NET_CanSendMessage (host_client->netconnection))
				{
					msg_sent[i] = true;
				}
				else
				{
					NET_GetMessage (host_client->netconnection);
				}
				count++;
				continue;
			}
		}
		if ((Sys_DoubleTime() - start) > blocktime)
			break;
	}
	return count;
}


//=============================================================================

/*
====================
NET_Init
====================
*/

void NET_Init (void)
{
	int			i;
	int			controlSocket;
	qsocket_t	*s;

	i = COM_CheckParm ("-port");
	if (!i)
		i = COM_CheckParm ("-udpport");
	if (!i)
		i = COM_CheckParm ("-ipxport");

	if (i)
	{
		if (i < com_argc-1)
			DEFAULTnet_hostport = atoi (com_argv[i+1]);
		else
			Con_SafePrintf("%s: ignoring -port argument\n", __thisfunc__);
	}
	net_hostport = DEFAULTnet_hostport;

	net_numsockets = svs.maxclientslimit;

	SetNetTime();

	for (i = 0; i < net_numsockets; i++)
	{
		s = (qsocket_t *)Hunk_AllocName(sizeof(qsocket_t), "qsocket");
		s->next = net_freeSockets;
		net_freeSockets = s;
		s->disconnected = true;
	}

	// allocate space for network message buffer
//	SZ_Init (&net_message, net_message_buffer, sizeof(net_message_buffer));
	SZ_Init (&net_message, NULL, NET_MAXMESSAGE);

	Cvar_RegisterVariable (&net_messagetimeout);
	Cvar_RegisterVariable (&hostname);
	Cvar_RegisterVariable (&net_allowmultiple);
#if NET_USE_SERIAL
	Cvar_RegisterVariable (&config_com_port);
	Cvar_RegisterVariable (&config_com_irq);
	Cvar_RegisterVariable (&config_com_baud);
	Cvar_RegisterVariable (&config_com_modem);
	Cvar_RegisterVariable (&config_modem_dialtype);
	Cvar_RegisterVariable (&config_modem_clear);
	Cvar_RegisterVariable (&config_modem_init);
	Cvar_RegisterVariable (&config_modem_hangup);
#endif	/* NET_USE_SERIAL */

	// initialize all the drivers
	for (i = net_driverlevel = 0; net_driverlevel < net_numdrivers; net_driverlevel++)
	{
		controlSocket = net_drivers[net_driverlevel].Init();
		if (controlSocket == -1)
			continue;
		i++;
		net_drivers[net_driverlevel].initialized = true;
		net_drivers[net_driverlevel].controlSock = controlSocket;
		net_drivers[net_driverlevel].Listen (true);
	}

	if (i == 0)
		Sys_Error("Network not available!");

	if (*my_ipx_address)
		Con_DPrintf("IPX address %s\n", my_ipx_address);
	if (*my_tcpip_address)
		Con_DPrintf("TCP/IP address %s\n", my_tcpip_address);
}

/*
====================
NET_Shutdown
====================
*/

void NET_Shutdown (void)
{
	qsocket_t	*sock;

	SetNetTime();

	for (sock = net_activeSockets; sock; sock = sock->next)
		NET_Close(sock);

//
// shutdown the drivers
//
	for (net_driverlevel = 0; net_driverlevel < net_numdrivers; net_driverlevel++)
	{
		if (net_drivers[net_driverlevel].initialized == true)
		{
			net_drivers[net_driverlevel].Shutdown ();
			net_drivers[net_driverlevel].initialized = false;
		}
	}
}


static PollProcedure *pollProcedureList = NULL;

void NET_Poll(void)
{
	PollProcedure *pp;

#if NET_USE_SERIAL
	if (!configRestored)
	{
		if (serialAvailable)
		{
			qboolean	useModem;
			if (config_com_modem.value == 1.0)
				useModem = true;
			else
				useModem = false;
			SetComPortConfig (0, config_com_port.integer, config_com_irq.integer, config_com_baud.integer, useModem);
			SetModemConfig (0, config_modem_dialtype.string, config_modem_clear.string, config_modem_init.string, config_modem_hangup.string);
		}
		configRestored = true;
	}
#endif	/* NET_USE_SERIAL */

	SetNetTime();

	for (pp = pollProcedureList; pp; pp = pp->next)
	{
		if (pp->nextTime > net_time)
			break;
		pollProcedureList = pp->next;
		pp->procedure(pp->arg);
	}
}

