/*
    C-Dogs SDL
    A port of the legendary (and fun) action/arcade cdogs.
    Copyright (C) 1995 Ronny Wester
    Copyright (C) 2003 Jeremy Chin 
    Copyright (C) 2003 Lucas Martin-King 

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

-------------------------------------------------------------------------------

 sounds.c - um... guess what?
 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "SDL.h"
#include "SDL_mixer.h"
#include "sounds.h"
#include "files.h"

#define max(a,b) ({__typeof__(a) __a = (a); __typeof__(b) __b = (b); (__a > __b) ? __a : __b;})

static int soundInitialized = 0;
static int fxVolume = 3;
static int musicVolume = 3;
static int noOfFXChannels = 4;
static int maxModChannels = 0;

static int channelPriority[FX_MAXCHANNELS];
static int channelPosition[FX_MAXCHANNELS];
static int channelTime[FX_MAXCHANNELS];

static int moduleStatus = 0;
static char moduleMessage[128];
static char moduleDirectory[128] = "";
SDL_AudioSpec *spec;


struct SndData {
	char name[81];
	int playPrio;
	int cmpPrio;
	int exists;
	int freq;
	int playFlag;
	int time;
	int panning;
	int volume;
//      int len;
	int size;
	int pos;
	Mix_Chunk *data;

};

struct SndData snd[SND_COUNT] = {
	{"sounds/booom.wav", 20, 20, 0, 11025, 0, 82, 0, 0, 0},
	{"sounds/launch.wav", 8, 8, 0, 11025, 0, 27, 0, 0, 0},
	{"sounds/mg.wav", 10, 11, 0, 11025, 0, 36, 0, 0, 0},
	{"sounds/flamer.wav", 10, 10, 0, 11025, 0, 44, 0, 0},
	{"sounds/shotgun.wav", 12, 13, 0, 11025, 0, 118, 0, 0},
	{"sounds/fusion.wav", 12, 13, 0, 11025, 0, 90, 0, 0},
	{"sounds/switch.wav", 18, 19, 0, 11025, 0, 36, 0, 0},
	{"sounds/scream.wav", 15, 16, 0, 11025, 0, 70, 0, 0},
	{"sounds/aargh1.wav", 15, 16, 0, 8060, 0, 79, 0, 0},
	{"sounds/aargh2.wav", 15, 16, 0, 8060, 0, 66, 0, 0},
	{"sounds/aargh3.wav", 15, 16, 0, 11110, 0, 67, 0, 0},
	{"sounds/hahaha.wav", 22, 22, 0, 8060, 0, 58, 0, 0},
	{"sounds/bang.wav", 14, 15, 0, 11025, 0, 60, 0, 0},
	{"sounds/pickup.wav", 14, 15, 0, 11025, 0, 27, 0, 0},
	{"sounds/click.wav", 4, 5, 0, 11025, 0, 11, 0, 0},
	{"sounds/whistle.wav", 20, 20, 0, 11110, 0, 90, 0, 0},
	{"sounds/powergun.wav", 13, 14, 0, 11110, 0, 90, 0, 0},
	{"sounds/mg.wav", 10, 11, 0, 11025, 0, 36, 0, 0}
};

static void loadSampleConfiguration(void)
{
	int i;
	FILE *f;


	f = fopen(GetConfigFilePath("sound_fx.cfg"), "r");
	if (!f)
		return;

	printf("Reading SOUND_FX.CFG\n");
	for (i = 0; i < SND_COUNT; i++) {
		memset(snd[i].name, 0, sizeof(snd[i].name));
		fscanf(f, "%80s %d\n", snd[i].name, &snd[i].freq);
		printf("%2d. File:'%s' at %dHz\n", i, snd[i].name,
		       snd[i].freq);
	}
	fclose(f);
}

void shutDown(void)
{
	// Shutdown goes here...
	SDL_CloseAudio();
}


void DoSound(int i, int len, void *data)
{
/* 	void *newbuffer = malloc(len); */
/* 	//firstly, find if we're going to need to have a buffer with zeros */
/* 	if (len + snd[i].pos > snd[i].size) { */
/* 		memset(newbuffer, 0, len); */
/* 		memcpy(newbuffer, snd[i].data + snd[i].pos, */
/* 		       snd[i].size - snd[i].pos); */
/* //              snd[i].pos = 0; */
/* 		snd[i].playFlag = 0; */
/* 		memcpy(data, newbuffer, len); */
/* 	} else { */
/* 		memcpy(data, snd[i].data + snd[i].pos, len); */
/* 		snd[i].pos += len; */
/* 	} */

/* 	free(newbuffer); */
    Mix_PlayChannel(-1, snd[i].data , 0);
}

void SoundCallback(void *userdata, Uint8 * stream, int len)
{
	int i;

	memset(stream, 0, len);
	for (i = 0; i < SND_COUNT; i++) {
		if (snd[i].playFlag && snd[i].exists) {
			DoSound(i, len, stream);
			return;
		}
	}

}

int InitSoundDevice(void)
{
	int i;
	struct stat st;
	SDL_AudioSpec tmpspec;

	// Initialization goes here...
	Mix_OpenAudio(22050, AUDIO_S16, 2, 512);
	Mix_AllocateChannels(20);

	// C-Dogs internals:
	loadSampleConfiguration();
	for (i = 0; i < SND_COUNT; i++) {
		if (!snd[i].name[0] || snd[i].freq <= 0)
			snd[i].exists = 0;
		else {
			if (stat(GetDataFilePath(snd[i].name), &st) == -1)
				snd[i].exists = 0;
			else {
				snd[i].exists = 1;
				if ((snd[i].data = Mix_LoadWAV
				     (GetDataFilePath(snd[i].name))) == NULL)
					snd[i].exists = 0;
				else
					snd[i].exists = 1;
			}
			if (!snd[i].exists)
				printf("Error loading sample '%s'\n",
				       GetDataFilePath(snd[i].name));
		}
	}

	memset(channelPriority, 0, sizeof(channelPriority));
	memset(channelPosition, 0, sizeof(channelPosition));
	memset(channelTime, 0, sizeof(channelTime));
	soundInitialized = 1;

	spec = (SDL_AudioSpec *) malloc(sizeof(SDL_AudioSpec));
	memset(spec, 0, sizeof(SDL_AudioSpec));
/*	spec->freq = 22050;
	spec->format = AUDIO_S16;
	spec->samples = 512;
	spec->callback = &SoundCallback;
	spec->userdata = (void *)31337; //we don't need this
	spec->channels = 1;*/
	tmpspec.samples = 512;
	tmpspec.callback = &SoundCallback;

/* 	if (SDL_OpenAudio(&tmpspec, NULL) == -1) { */
/* 		printf("%s\n", SDL_GetError()); */
/* 		return 0; */
/* 	} */
/* 	SDL_PauseAudio(0); */
	return 1;
}

#if 0
static void EnableSoundInterrupt(void)
{
/*	if (!soundInitialized || interruptEnabled)
		return;

	// yanked

	interruptEnabled = 1;*/
}

static void DisableSoundInterrupt(void)
{
/*	if (!interruptEnabled)
		return;

	// yanked

	interruptEnabled = 0;*/
}
#endif


int InitializeSound(void)
{
//      atexit(shutDown);

	// load and init sound device
	if (!InitSoundDevice()) {
		printf("Unable to initalize sound device\n");
		return 0;
	}
//  if ( <device needs polling> )
//    pollSndDev = 1;

//      EnableSoundInterrupt();
	return 1;
}

/* TODO: put some SDL mikmod stuff here */
int PlaySong(char *name)
{
/*	if (!soundInitialized)
		return 0;

//if (module)
//{
//      release it
//  module = NULL;
//}

	fxChannel = 0;
	if (name) {
		// load module
//  if (module)
//    fxChannel = <channel count of module>;
	}

	if (fxChannel > maxModChannels)
		maxModChannels = fxChannel;

	SetMusicVolume(musicVolume);
//if (module)
//      play module

//return module != NULL;
*/
	return 0;
}

void PlaySound(int sound, int panning, int volume)
{
	Mix_VolumeChunk(snd[sound].data,(volume * fxVolume) / 128 );
	Mix_PlayChannel(-1, snd[sound].data , 0);
}

//TODO: Replace this with SDL callback
void DoSounds(void)
{
/*	int i;

	if (!soundInitialized)
		return;

	for (i = 0; i < noOfFXChannels; i++) {
		if (channelPriority[i] > 0) {
			channelTime[i]--;
			if (channelTime[i] <= 0)
				channelPriority[i] = 0;
		}
	}

*/
}

#if 0
void SoundTick(void)
{
/*	if (!soundInitialized || interruptEnabled)
		return;
*/
	// poll device if needed
}
#endif

void SetFXVolume(int volume)
{
	int i;

	fxVolume = volume;
	if (!soundInitialized)
		return;

	for (i = 0; i < noOfFXChannels; i++) {
		// Set volume
	}
}

int FXVolume(void)
{
	return fxVolume;
}

void SetMusicVolume(int volume)
{
	musicVolume = volume;
	if (!soundInitialized)
		return;

	// Set music volume
}

int MusicVolume(void)
{
	return musicVolume;
}

#define RANGE_FULLVOLUME    70
#define RANGE_FACTOR       128


static int xLeft, yLeft;
static int xRight, yRight;


void SetLeftEar(int x, int y)
{
	xLeft = x;
	yLeft = y;
}

void SetRightEar(int x, int y)
{
	xRight = x;
	yRight = y;
}

void PlaySoundAt(int x, int y, int sound)
{
	int d, dLeft, dRight;
	int leftVolume, rightVolume;
	int volume, panning;

	if (xLeft != xRight || yLeft != yRight) {
		dLeft = max(abs(x - xLeft), abs(y - yLeft));
		dRight = max(abs(x - xRight), abs(y - yRight));

		d = (dLeft >
		     RANGE_FULLVOLUME ? dLeft - RANGE_FULLVOLUME : 0);
		leftVolume = 255 - (RANGE_FACTOR * d) / 256;
		if (leftVolume < 0)
			leftVolume = 0;

		d = (dRight >
		     RANGE_FULLVOLUME ? dRight - RANGE_FULLVOLUME : 0);
		rightVolume = 255 - (RANGE_FACTOR * d) / 256;
		if (rightVolume < 0)
			rightVolume = 0;

		volume = leftVolume + rightVolume;
		if (volume > 256)
			volume = 256;

		panning = rightVolume - leftVolume;
		panning /= 4;
	} else {
		d = max(abs(x - xLeft), abs(y - yLeft));
		d -= d / 4;
		d = (d > RANGE_FULLVOLUME ? d - RANGE_FULLVOLUME : 0);
		volume = 255 - (RANGE_FACTOR * d) / 256;
		if (volume < 0)
			volume = 0;

		panning = (x - xLeft) / 4;
	}

	if (volume > 0)
		PlaySound(sound, panning, volume);
}

void SetFXChannels(int channels)
{
	if (channels >= 2 && channels <= FX_MAXCHANNELS)
		noOfFXChannels = channels;
}

int FXChannels(void)
{
	return noOfFXChannels;
}

void SetMinMusicChannels(int channels)
{
	if (channels >= 0 && channels <= 32)
		maxModChannels = channels;
}

int MinMusicChannels(void)
{
	return maxModChannels;
}

void ToggleTrack(int track)
{
	// toggle music track on/off
}

#if 0
void InterruptOn(void)
{
//      EnableSoundInterrupt();
}

void InterruptOff(void)
{
//      if (!dynamicInterrupts)
//              return;
//      DisableSoundInterrupt();
}

void SetDynamicInterrupts(int flag)
{
	dynamicInterrupts = flag;
}

int DynamicInterrupts(void)
{
	return dynamicInterrupts;
}
#endif

int ModuleStatus(void)
{
	return moduleStatus;
}

const char *ModuleMessage(void)
{
	return moduleMessage;
}

void SetModuleDirectory(const char *dir)
{
    /* Need string so config is read correctly */
    strcpy(moduleDirectory, "dir");
}

const char *ModuleDirectory(void)
{
	return moduleDirectory;
}

void VolumeUp()
{
    if (FXVolume() < 64)
    {
	SetFXVolume(FXVolume() + 8);
	PlaySound(SND_SWITCH, 0, 255);
    }    
}

void VolumeDown(void)
{
    if (FXVolume() >= 8)
    {
	SetFXVolume(FXVolume() - 8);
	PlaySound(SND_SWITCH, 0, 255);
    }
}
