/*
	cd_win.c
	Win32 cdaudio code

	$Id: cd_win.c,v 1.19 2008/03/31 15:30:25 sezero Exp $

	Quake is a trademark of Id Software, Inc., (c) 1996 Id Software, Inc. All
	rights reserved.
*/

#include "quakedef.h"
#include "winquake.h"

/*
 * You just can't set the volume of CD playback via MCI :
 * http://blogs.msdn.com/larryosterman/archive/2005/10/06/477874.aspx
 * OTOH, using the aux APIs to control the CD audio volume is broken.
 */
#undef	USE_AUX_API

static qboolean cdValid = false;
static qboolean	playing = false;
static qboolean	wasPlaying = false;
static qboolean	initialized = false;
static qboolean	enabled = false;
static qboolean playLooping = false;
static byte 	remap[100];
static byte	playTrack;
static byte	maxTrack;

static float	old_cdvolume;
static UINT		wDeviceID;
#if defined(USE_AUX_API)
static UINT		CD_ID;
static unsigned long	CD_OrigVolume;
static void CD_SetVolume(unsigned long Volume);
#endif	/* USE_AUX_API */


static void CDAudio_Eject(void)
{
	DWORD	dwReturn;

	dwReturn = mciSendCommand(wDeviceID, MCI_SET, MCI_SET_DOOR_OPEN, (DWORD_PTR)NULL);
	if (dwReturn)
		Con_DPrintf("MCI_SET_DOOR_OPEN failed (%lu)\n", (unsigned long)dwReturn);
}


static void CDAudio_CloseDoor(void)
{
	DWORD	dwReturn;

	dwReturn = mciSendCommand(wDeviceID, MCI_SET, MCI_SET_DOOR_CLOSED, (DWORD_PTR)NULL);
	if (dwReturn)
		Con_DPrintf("MCI_SET_DOOR_CLOSED failed (%lu)\n", (unsigned long)dwReturn);
}


static int CDAudio_GetAudioDiskInfo(void)
{
	DWORD				dwReturn;
	MCI_STATUS_PARMS	mciStatusParms;

	cdValid = false;

	mciStatusParms.dwItem = MCI_STATUS_READY;
	dwReturn = mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_WAIT, (DWORD_PTR) (LPVOID) &mciStatusParms);
	if (dwReturn)
	{
		Con_DPrintf("CDAudio: drive ready test - get status failed\n");
		return -1;
	}
	if (!mciStatusParms.dwReturn)
	{
		Con_DPrintf("CDAudio: drive not ready\n");
		return -1;
	}

	mciStatusParms.dwItem = MCI_STATUS_NUMBER_OF_TRACKS;
	dwReturn = mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_WAIT, (DWORD_PTR) (LPVOID) &mciStatusParms);
	if (dwReturn)
	{
		Con_DPrintf("CDAudio: get tracks - status failed\n");
		return -1;
	}
	if (mciStatusParms.dwReturn < 1)
	{
		Con_DPrintf("CDAudio: no music tracks\n");
		return -1;
	}

	cdValid = true;
	maxTrack = mciStatusParms.dwReturn;

	return 0;
}


void CDAudio_Play(byte track, qboolean looping)
{
	DWORD				dwReturn;
	MCI_PLAY_PARMS		mciPlayParms;
	MCI_STATUS_PARMS	mciStatusParms;

	if (!enabled)
		return;
	
	if (!cdValid)
	{
		CDAudio_GetAudioDiskInfo();
		if (!cdValid)
			return;
	}

	track = remap[track];

	if (track < 1 || track > maxTrack)
	{
		Con_DPrintf("CDAudio: Bad track number %u.\n", track);
		return;
	}

	// don't try to play a non-audio track
	mciStatusParms.dwItem = MCI_CDA_STATUS_TYPE_TRACK;
	mciStatusParms.dwTrack = track;
	dwReturn = mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK | MCI_WAIT, (DWORD_PTR) (LPVOID) &mciStatusParms);
	if (dwReturn)
	{
		Con_DPrintf("MCI_STATUS failed (%lu)\n", (unsigned long)dwReturn);
		return;
	}
	if (mciStatusParms.dwReturn != MCI_CDA_TRACK_AUDIO)
	{
		Con_Printf("CDAudio: track %i is not audio\n", track);
		return;
	}

	// get the length of the track to be played
	mciStatusParms.dwItem = MCI_STATUS_LENGTH;
	mciStatusParms.dwTrack = track;
	dwReturn = mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK | MCI_WAIT, (DWORD_PTR) (LPVOID) &mciStatusParms);
	if (dwReturn)
	{
		Con_DPrintf("MCI_STATUS failed (%lu)\n", (unsigned long)dwReturn);
		return;
	}

	if (playing)
	{
		if (playTrack == track)
			return;
		CDAudio_Stop();
	}

	mciPlayParms.dwFrom = MCI_MAKE_TMSF(track, 0, 0, 0);
	mciPlayParms.dwTo = (mciStatusParms.dwReturn << 8) | track;
	mciPlayParms.dwCallback = (DWORD_PTR)mainwindow;
	dwReturn = mciSendCommand(wDeviceID, MCI_PLAY, MCI_NOTIFY | MCI_FROM | MCI_TO, (DWORD_PTR)(LPVOID) &mciPlayParms);
	if (dwReturn)
	{
		Con_DPrintf("CDAudio: MCI_PLAY failed (%lu)\n", (unsigned long)dwReturn);
		return;
	}

	playLooping = looping;
	playTrack = track;
	playing = true;

	if (bgmvolume.value == 0.0)
		CDAudio_Pause ();
}


void CDAudio_Stop(void)
{
	DWORD	dwReturn;

	if (!enabled)
		return;
	
	if (!playing)
		return;

	dwReturn = mciSendCommand(wDeviceID, MCI_STOP, 0, (DWORD_PTR)NULL);
	if (dwReturn)
		Con_DPrintf("MCI_STOP failed (%lu)", (unsigned long)dwReturn);

	wasPlaying = false;
	playing = false;
}


void CDAudio_Pause(void)
{
	DWORD				dwReturn;
	MCI_GENERIC_PARMS	mciGenericParms;

	if (!enabled)
		return;

	if (!playing)
		return;

	mciGenericParms.dwCallback = (DWORD_PTR)mainwindow;
	dwReturn = mciSendCommand(wDeviceID, MCI_PAUSE, 0, (DWORD_PTR)(LPVOID) &mciGenericParms);
	if (dwReturn)
		Con_DPrintf("MCI_PAUSE failed (%lu)", (unsigned long)dwReturn);

	wasPlaying = playing;
	playing = false;
}


void CDAudio_Resume(void)
{
	DWORD			dwReturn;
	MCI_PLAY_PARMS	mciPlayParms;

	if (!enabled)
		return;
	
	if (!cdValid)
		return;

	if (!wasPlaying)
		return;
	
	mciPlayParms.dwFrom = MCI_MAKE_TMSF(playTrack, 0, 0, 0);
	mciPlayParms.dwTo = MCI_MAKE_TMSF(playTrack + 1, 0, 0, 0);
	mciPlayParms.dwCallback = (DWORD_PTR)mainwindow;
	dwReturn = mciSendCommand(wDeviceID, MCI_PLAY, MCI_TO | MCI_NOTIFY, (DWORD_PTR)(LPVOID) &mciPlayParms);
	if (dwReturn)
	{
		Con_DPrintf("CDAudio: MCI_PLAY failed (%lu)\n", (unsigned long)dwReturn);
		return;
	}
	playing = true;
}


static void CD_f (void)
{
	const char	*command;
	int		ret;
	int		n;

	if (Cmd_Argc() < 2)
	{
		Con_Printf("commands:");
		Con_Printf("on, off, reset, remap, \n");
		Con_Printf("play, stop, loop, pause, resume\n");
		Con_Printf("eject, close, info\n");
		return;
	}

	command = Cmd_Argv (1);

	if (q_strcasecmp(command, "on") == 0)
	{
		enabled = true;
		return;
	}

	if (q_strcasecmp(command, "off") == 0)
	{
		if (playing)
			CDAudio_Stop();
		enabled = false;
		return;
	}

	if (q_strcasecmp(command, "reset") == 0)
	{
		enabled = true;
		if (playing)
			CDAudio_Stop();
		for (n = 0; n < 100; n++)
			remap[n] = n;
		CDAudio_GetAudioDiskInfo();
		return;
	}

	if (q_strcasecmp(command, "remap") == 0)
	{
		ret = Cmd_Argc() - 2;
		if (ret <= 0)
		{
			for (n = 1; n < 100; n++)
				if (remap[n] != n)
					Con_Printf("  %u -> %u\n", n, remap[n]);
			return;
		}
		for (n = 1; n <= ret; n++)
			remap[n] = atoi(Cmd_Argv (n+1));
		return;
	}

	if (q_strcasecmp(command, "close") == 0)
	{
		CDAudio_CloseDoor();
		return;
	}

	if (!cdValid)
	{
		CDAudio_GetAudioDiskInfo();
		if (!cdValid)
		{
			Con_Printf("No CD in player.\n");
			return;
		}
	}

	if (q_strcasecmp(command, "play") == 0)
	{
		CDAudio_Play((byte)atoi(Cmd_Argv (2)), false);
		return;
	}

	if (q_strcasecmp(command, "loop") == 0)
	{
		CDAudio_Play((byte)atoi(Cmd_Argv (2)), true);
		return;
	}

	if (q_strcasecmp(command, "stop") == 0)
	{
		CDAudio_Stop();
		return;
	}

	if (q_strcasecmp(command, "pause") == 0)
	{
		CDAudio_Pause();
		return;
	}

	if (q_strcasecmp(command, "resume") == 0)
	{
		CDAudio_Resume();
		return;
	}

	if (q_strcasecmp(command, "eject") == 0)
	{
		if (playing)
			CDAudio_Stop();
		CDAudio_Eject();
		cdValid = false;
		return;
	}

	if (q_strcasecmp(command, "info") == 0)
	{
		Con_Printf("%u tracks\n", maxTrack);
		if (playing)
			Con_Printf("Currently %s track %u\n", playLooping ? "looping" : "playing", playTrack);
		else if (wasPlaying)
			Con_Printf("Paused %s track %u\n", playLooping ? "looping" : "playing", playTrack);
		Con_Printf("Volume is %f\n", bgmvolume.value);
		return;
	}
}


LONG CDAudio_MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (lParam != wDeviceID)
		return 1;

	switch (wParam)
	{
		case MCI_NOTIFY_SUCCESSFUL:
			if (playing)
			{
				playing = false;
				if (playLooping)
					CDAudio_Play(playTrack, true);
			}
			break;

		case MCI_NOTIFY_ABORTED:
		case MCI_NOTIFY_SUPERSEDED:
			break;

		case MCI_NOTIFY_FAILURE:
			Con_DPrintf("MCI_NOTIFY_FAILURE\n");
			CDAudio_Stop ();
			cdValid = false;
			break;

		default:
			Con_DPrintf("Unexpected MM_MCINOTIFY type (%i)\n", wParam);
			return 1;
	}

	return 0;
}


static void CDAudio_SetVolume (cvar_t *var)
{
	if (var->value < 0.0)
		Cvar_SetValue (var->name, 0.0);
	else if (var->value > 1.0)
		Cvar_SetValue (var->name, 1.0);
	old_cdvolume = var->value;

#if defined(USE_AUX_API)
	CD_SetVolume (var->value * 0xffff);
#endif	/* USE_AUX_API */
	if (old_cdvolume == 0.0)
		CDAudio_Pause ();
	else
		CDAudio_Resume();
}

void CDAudio_Update(void)
{
	if (!enabled)
		return;

	if (old_cdvolume != bgmvolume.value)
		CDAudio_SetVolume (&bgmvolume);
}


#if defined(USE_AUX_API)
static void CD_FindCDAux(void)
{
	UINT		NumDevs, counter;
	MMRESULT		Result;
	AUXCAPS			Caps;

	CD_ID = -1;
	if (!COM_CheckParm("-usecdvolume"))
		return;
	NumDevs = auxGetNumDevs();
	for (counter = 0; counter < NumDevs; counter++)
	{
		Result = auxGetDevCaps(counter,&Caps,sizeof(Caps));
		if (!Result) // valid
		{
			if (Caps.wTechnology == AUXCAPS_CDAUDIO)
			{
				CD_ID = counter;
				auxGetVolume(CD_ID,&CD_OrigVolume);
				return;
			}
		}
	}
}

static void CD_SetVolume(unsigned long Volume)
{
	if (CD_ID != -1) 
		auxSetVolume(CD_ID,(Volume<<16)+Volume);
}
#endif	/* USE_AUX_API */

int CDAudio_Init(void)
{
	DWORD	dwReturn;
	MCI_OPEN_PARMS	mciOpenParms;
	MCI_SET_PARMS	mciSetParms;
	int	n;

	if (safemode || COM_CheckParm("-nocdaudio"))
		return -1;

	mciOpenParms.lpstrDeviceType = "cdaudio";
	dwReturn = mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_SHAREABLE, (DWORD_PTR) (LPVOID) &mciOpenParms);
	if (dwReturn)
	{
		Con_Printf("%s: MCI_OPEN failed (%lu)\n", __thisfunc__, (unsigned long)dwReturn);
		return -1;
	}
	wDeviceID = mciOpenParms.wDeviceID;

	// Set the time format to track/minute/second/frame (TMSF).
	mciSetParms.dwTimeFormat = MCI_FORMAT_TMSF;
	dwReturn = mciSendCommand(wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD_PTR)(LPVOID) &mciSetParms);
	if (dwReturn)
	{
		Con_Printf("MCI_SET_TIME_FORMAT failed (%lu)\n", (unsigned long)dwReturn);
		mciSendCommand(wDeviceID, MCI_CLOSE, 0, (DWORD_PTR)NULL);
		return -1;
	}

	for (n = 0; n < 100; n++)
		remap[n] = n;
	initialized = true;
	enabled = true;
	old_cdvolume = bgmvolume.value;

	if (CDAudio_GetAudioDiskInfo())
	{
		Con_Printf("%s: No CD in player.\n", __thisfunc__);
		cdValid = false;
		//enabled = false;
	}

	Cmd_AddCommand ("cd", CD_f);

#if defined(USE_AUX_API)
	CD_FindCDAux();
#endif	/* USE_AUX_API */

	Con_Printf("CD Audio Initialized\n");

	return 0;
}


void CDAudio_Shutdown(void)
{
	if (!initialized)
		return;
	CDAudio_Stop();
	if (mciSendCommand(wDeviceID, MCI_CLOSE, MCI_WAIT, (DWORD_PTR)NULL))
		Con_DPrintf("%s: MCI_CLOSE failed\n", __thisfunc__);
}

