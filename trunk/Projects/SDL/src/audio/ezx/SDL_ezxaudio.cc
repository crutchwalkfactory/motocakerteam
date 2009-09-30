/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2006 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org

    This file written by Sergey Nizovtsev (snizovtsev@gmail.com)
*/
#include "SDL_config.h"

/* Output audio via libezxsound.so for Motorola Z6... */

#include "SDL_rwops.h"
#include "SDL_timer.h"
#include "SDL_audio.h"
#include "../SDL_audiomem.h"
#include "../SDL_audio_c.h"
#include "../SDL_audiodev_c.h"
#include "SDL_ezxaudio.h"

/* The tag name used by EZX audio */
#define EZXAudio_DRIVER_NAME         "ezx"

/* Shortcuts */
#define mixbuf			(thisdevice->hidden->mixbuf)
#define mixlen			(thisdevice->hidden->mixlen)
#define audio_dev		(thisdevice->hidden->audio_dev)

/* Audio driver functions */
static int EZXAudio_OpenAudio(_THIS, SDL_AudioSpec *spec);
static void EZXAudio_WaitAudio(_THIS);
static void EZXAudio_PlayAudio(_THIS);
static Uint8 *EZXAudio_GetAudioBuf(_THIS);
static void EZXAudio_CloseAudio(_THIS);

/* Audio driver bootstrap functions */
static int EZXAudio_Available(void)
{
	return(1);
}

static void EZXAudio_DeleteDevice(SDL_AudioDevice *device)
{
	SDL_free(device->hidden);
	SDL_free(device);
}

static SDL_AudioDevice *EZXAudio_CreateDevice(int devindex)
{
	SDL_AudioDevice *thisdevice;

	/* Initialize all variables that we clean on shutdown */
	thisdevice = (SDL_AudioDevice *)SDL_malloc(sizeof(SDL_AudioDevice));
	if ( thisdevice ) {
		SDL_memset(thisdevice, 0, (sizeof *thisdevice));
		thisdevice->hidden = (struct SDL_PrivateAudioData *)
				SDL_malloc((sizeof *thisdevice->hidden));
	}
	if ( (thisdevice == NULL) || (thisdevice->hidden == NULL) ) {
		SDL_OutOfMemory();
		if ( thisdevice ) {
			SDL_free(thisdevice);
		}
		return(0);
	}
	SDL_memset(thisdevice->hidden, 0, (sizeof *thisdevice->hidden));

	/* Set the function pointers */
	thisdevice->OpenAudio = EZXAudio_OpenAudio;
	thisdevice->WaitAudio = EZXAudio_WaitAudio;
	thisdevice->PlayAudio = EZXAudio_PlayAudio;
	thisdevice->GetAudioBuf = EZXAudio_GetAudioBuf;
	thisdevice->CloseAudio = EZXAudio_CloseAudio;

	thisdevice->free = EZXAudio_DeleteDevice;

	return thisdevice;
}

AudioBootStrap EZXAudio_bootstrap = {
	EZXAudio_DRIVER_NAME, "SDL EZX audio driver",
	EZXAudio_Available, EZXAudio_CreateDevice
};

/* This function waits until it is possible to write a full sound buffer */
static void EZXAudio_WaitAudio(_THIS)
{
	/* Not needed */
}

static void EZXAudio_PlayAudio(_THIS)
{
	int written = 0;
	if (mixlen > 0)
		written = audio_dev->write((short*)mixbuf, mixlen / 2);
}

static Uint8 *EZXAudio_GetAudioBuf(_THIS)
{
	return mixbuf;
}

static void EZXAudio_CloseAudio(_THIS)
{
	if ( mixbuf != NULL ) {
		SDL_FreeAudioMem(mixbuf);
		mixbuf = NULL;
	}
	audio_dev->flush();
	audio_dev->close();
	delete audio_dev;
}

static int EZXAudio_OpenAudio(_THIS, SDL_AudioSpec *spec)
{
	audio_dev = new AM_NORMAL_DEV_INTERFACE(used_by_flashclient);
	audio_dev->setAudioDevice(spec->freq, spec->channels, 2);
	audio_dev->setVolume(3);

	/* Allocate mixing buffer */
	mixlen = spec->size;
	mixbuf = (Uint8 *) SDL_AllocAudioMem(mixlen);
	if ( mixbuf == NULL ) {
		return(-1);
	}
	SDL_memset(mixbuf, spec->silence, spec->size);
	return(0);
}
