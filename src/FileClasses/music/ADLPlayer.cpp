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

#include <FileClasses/music/ADLPlayer.h>

#include <globals.h>

#include <FileClasses/FileManager.h>
#include <FileClasses/adl/sound_adlib.h>

#include <mmath.h>

ADLPlayer::ADLPlayer() : MusicPlayer() {
    musicVolume = MIX_MAX_VOLUME/2;
    Mix_VolumeMusic(musicVolume);

    pSoundAdlibPC = NULL;

	currentMusicNum = 0;
}

ADLPlayer::~ADLPlayer() {
    setMusic(false);
}

void ADLPlayer::changeMusic(MUSICTYPE musicType)
{
	int musicNum = -1;
	std::string filename = "";

	if(currentMusicType == musicType) {
		return;
	}

    /* currently unused:
        DUNE0.ADL/4
        DUNE1.ADL/2 and DUNE10.ADL/2
        DUNE20.ADL/2
    */


	switch(musicType)
	{
		case MUSIC_ATTACK: {

            switch(getRandomInt(0, 5)) {
                case 0:     filename = "DUNE10.ADL";    musicNum = 7;   break;
                case 1:     filename = "DUNE11.ADL";    musicNum = 7;   break;
                case 2:     filename = "DUNE12.ADL";    musicNum = 7;   break;
                case 3:     filename = "DUNE13.ADL";    musicNum = 7;   break;
                case 4:     filename = "DUNE14.ADL";    musicNum = 7;   break;
                case 5:     filename = "DUNE15.ADL";    musicNum = 7;   break;
            }

		} break;

		case MUSIC_PEACE: {

            switch(getRandomInt(0, 7)) {
                case 0:     filename = "DUNE2.ADL";     musicNum = 6;   break;
                case 1:     filename = "DUNE3.ADL";     musicNum = 6;   break;
                case 2:     filename = "DUNE4.ADL";     musicNum = 6;   break;
                case 3:     filename = "DUNE5.ADL";     musicNum = 6;   break;
                case 4:     filename = "DUNE6.ADL";     musicNum = 6;   break;
                case 5:     filename = "DUNE9.ADL";     musicNum = 4;   break;
                case 6:     filename = "DUNE9.ADL";     musicNum = 5;   break;
                case 7:     filename = "DUNE18.ADL";    musicNum = 6;   break;
            }

		} break;

		case MUSIC_INTRO: {
		    filename = "DUNE0.ADL";
		    musicNum = 2;
        } break;

		case MUSIC_MENU: {
		    filename = "DUNE7.ADL";
		    musicNum = 6;
        } break;

		case MUSIC_BRIEFING_A: {
		    filename = "DUNE7.ADL";
		    musicNum = 3;
        } break;

		case MUSIC_BRIEFING_O: {
		    filename = "DUNE7.ADL";
		    musicNum = 4;
        } break;

		case MUSIC_BRIEFING_H: {
		    filename = "DUNE7.ADL";
		    musicNum = 2;
        } break;

		case MUSIC_WIN_A: {
		    filename = "DUNE8.ADL";
		    musicNum = 2;
		} break;

		case MUSIC_WIN_O: {
		    filename = "DUNE17.ADL";
		    musicNum = 4;
		} break;

		case MUSIC_WIN_H: {
		    filename = "DUNE8.ADL";
		    musicNum = 3;
		} break;

		case MUSIC_LOSE: {
		    filename = "DUNE1.ADL";
		    musicNum = 4;
		} break;

        case MUSIC_GAMESTAT: {
            switch(getRandomInt(0, 1)) {
                case 0:     filename = "DUNE10.ADL";     musicNum = 7;   break;
                case 1:     filename = "DUNE11.ADL";     musicNum = 7;   break;
            }
		} break;

        case MUSIC_MAPCHOICE: {
		    filename = "DUNE16.ADL";
		    musicNum = 7;
		} break;

		case MUSIC_MEANWHILE: {
		    filename = "DUNE16.ADL";
		    musicNum = 8;
		} break;

        case MUSIC_FINALE_A: {
		    filename = "DUNE19.ADL";
		    musicNum = 2;
		} break;

        case MUSIC_FINALE_O: {
		    filename = "DUNE19.ADL";
		    musicNum = 3;
		} break;

        case MUSIC_FINALE_H: {
		    filename = "DUNE19.ADL";
		    musicNum = 4;
		} break;

		case MUSIC_RANDOM:
		default: {

            switch(getRandomInt(0, 13)) {
                // attack
                case 0:     filename = "DUNE10.ADL";    musicNum = 7;   break;
                case 1:     filename = "DUNE11.ADL";    musicNum = 7;   break;
                case 2:     filename = "DUNE12.ADL";    musicNum = 7;   break;
                case 3:     filename = "DUNE13.ADL";    musicNum = 7;   break;
                case 4:     filename = "DUNE14.ADL";    musicNum = 7;   break;
                case 5:     filename = "DUNE15.ADL";    musicNum = 7;   break;

                // peace
                case 6:     filename = "DUNE2.ADL";     musicNum = 6;   break;
                case 7:     filename = "DUNE3.ADL";     musicNum = 6;   break;
                case 8:     filename = "DUNE4.ADL";     musicNum = 6;   break;
                case 9:     filename = "DUNE5.ADL";     musicNum = 6;   break;
                case 10:     filename = "DUNE6.ADL";     musicNum = 6;   break;
                case 11:     filename = "DUNE9.ADL";     musicNum = 4;   break;
                case 12:     filename = "DUNE9.ADL";     musicNum = 5;   break;
                case 13:     filename = "DUNE18.ADL";    musicNum = 6;   break;
            }

		} break;
	}

	currentMusicType = musicType;

	if((musicOn == true) && (filename != "")) {

        Mix_HookMusic(NULL, NULL);
	    delete pSoundAdlibPC;

	    SDL_RWops* rwop = pFileManager->OpenFile(filename);
	    if(rwop == NULL) {
            printf("Unable to load %s!\n",filename.c_str());
	    } else {
            pSoundAdlibPC = new SoundAdlibPC(rwop);

            SDL_RWclose(rwop);

            pSoundAdlibPC->playTrack(musicNum);

            Mix_HookMusic(pSoundAdlibPC->callback, pSoundAdlibPC);

            //printf("Now playing %s!\n",filename.c_str());
	    }
	}
}

void ADLPlayer::musicCheck() {
	if(musicOn) {
		if(pSoundAdlibPC->isPlaying() == false) {
			changeMusic(MUSIC_PEACE);
		}
	}
}

void ADLPlayer::setMusic(bool value) {
	musicOn = value;

	if(musicOn) {
		changeMusic(MUSIC_RANDOM);
	} else {
	    Mix_HookMusic(NULL, NULL);

	    delete pSoundAdlibPC;
	    pSoundAdlibPC = NULL;
	}
}


void ADLPlayer::toggleSound()
{
	if(musicOn == false) {
		musicOn = true;
		currentMusicType = MUSIC_RANDOM;
		changeMusic(MUSIC_PEACE);
	} else {
		setMusic(false);
	}
}