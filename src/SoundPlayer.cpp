/*
 *  This file is part of Dune Legacy.
 *
 *  Dune Legacy is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Dune Legacy is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Dune Legacy.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <SoundPlayer.h>

#include <globals.h>

#include <ScreenBorder.h>
#include <Game.h>
#include <MapClass.h>
#include <House.h>


Mix_Chunk* curVoiceChunk = NULL;
int voiceChannel = 0;
bool PlayingVoiceATM = false;

void VoiceChunkFinishedCallback(int channel) {
	if(channel == voiceChannel) {
		PlayingVoiceATM = false;
	}
}

SoundPlayer::SoundPlayer() {
	sfxVolume = MIX_MAX_VOLUME/2;

	responseVolume = 100;
	voiceVolume = 128;

	Mix_Volume(-1, MIX_MAX_VOLUME);

	// init global variables
	curVoiceChunk = NULL;
	PlayingVoiceATM = false;

	voiceChannel = Mix_ReserveChannels(1);	//Reserve a channel for voice over
	Mix_ChannelFinished(VoiceChunkFinishedCallback);

	soundOn = true;
	//soundOn = false;
}

SoundPlayer::~SoundPlayer() {
}

void SoundPlayer::playSoundAt(Sound_enum soundID, const Coord& location)
{
	if(soundOn) {
        if(!currentGameMap->cellExists(location) ||
            !currentGameMap->getCell(location)->isExplored(pLocalHouse->getHouseID())) {
            return;
        }

		playSound(soundID, sfxVolume);
	}
}

void SoundPlayer::playSound(Sound_enum soundID, int volume)
{
	if(soundOn) {
		Mix_Chunk* tmp;

		if((tmp = pSFXManager->GetSound(soundID)) == NULL) {
			return;
		}

		int channel = Mix_PlayChannel(-1,tmp, 0);
		if (channel != -1)
			Mix_Volume(channel, (volume*sfxVolume)/MIX_MAX_VOLUME);
	}
}

void SoundPlayer::playVoice(Voice_enum id, int houseID) {
	if(soundOn) {
		Mix_Chunk* tmp;

		if((tmp = pSFXManager->GetVoice(id,houseID)) == NULL) {
			fprintf(stderr,"There is no voice with id %d!\n",id);
			exit(EXIT_FAILURE);
		}

		int channel = Mix_PlayChannel(-1, tmp, 0);
		Mix_Volume(channel,sfxVolume);
	}
}

void SoundPlayer::playSound(Mix_Chunk* sound) {
	if(soundOn) {
		Mix_PlayChannel(-1, sound, 0);
	}
}

void SoundPlayer::playSound(Sound_enum id) {
	if(soundOn) {
		Mix_Chunk* tmp;

		if((tmp = pSFXManager->GetSound(id)) == NULL) {
			fprintf(stderr,"There is no sound with id %d!\n",id);
			exit(EXIT_FAILURE);
		}

		Mix_PlayChannel(-1, tmp, 0);
	}
}
