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

#include <FileClasses/music/DirectoryPlayer.h>

#include <misc/FileSystem.h>
#include <misc/fnkdat.h>
#include <mmath.h>

DirectoryPlayer::DirectoryPlayer() : MusicPlayer() {
	// determine path to config file
	char tmp[FILENAME_MAX];
	fnkdat(NULL, tmp, FILENAME_MAX, FNKDAT_USER | FNKDAT_CREAT);
	std::string configfilepath(tmp);

	static const char* musicDirectoryNames[MUSIC_NUM_MUSIC_TYPES] = {   "/music/attack/",
                                                                        "/music/peace/",
                                                                        "/music/intro/",
                                                                        "/music/menu/",
                                                                        "/music/briefingA/",
                                                                        "/music/briefingO/",
                                                                        "/music/briefingH/",
                                                                        "/music/winA/",
                                                                        "/music/winO/",
                                                                        "/music/winH/",
                                                                        "/music/lose/",
                                                                        "/music/gamestats/",
                                                                        "/music/mapchoice/",
                                                                        "/music/meanwhile/",
                                                                        "/music/finaleA/",
                                                                        "/music/finaleO/",
                                                                        "/music/finaleH/"
                                                                    };

    for(int i=0;i<MUSIC_NUM_MUSIC_TYPES;i++) {
        MusicFileList[i] = getMusicFileNames(configfilepath + musicDirectoryNames[i]);
    }


	musicVolume = MIX_MAX_VOLUME/2;
    Mix_VolumeMusic(musicVolume);

	music = NULL;
}

DirectoryPlayer::~DirectoryPlayer() {
	if(music != NULL) {
		Mix_FreeMusic(music);
		music = NULL;
	}
}

void DirectoryPlayer::changeMusic(MUSICTYPE musicType)
{
	int musicNum = -1;
	std::string filename = "";

	if(currentMusicType == musicType) {
		return;
	}

	if(musicType >= 0 && musicType < MUSIC_NUM_MUSIC_TYPES) {
        musicNum = getRandomInt(0, MusicFileList[musicType].size()-1);
        filename = MusicFileList[musicType][musicNum];
        currentMusicType = musicType;
	} else {
	   // MUSIC_RANDOM
        int maxnum = MusicFileList[MUSIC_ATTACK].size() + MusicFileList[MUSIC_PEACE].size();

        if(maxnum > 0) {
            unsigned int randnum = getRandomInt(0, maxnum-1);

            if(randnum < MusicFileList[MUSIC_ATTACK].size()) {
                musicNum = randnum;
                filename = MusicFileList[MUSIC_ATTACK][musicNum];
                currentMusicType = MUSIC_ATTACK;
            } else {
                musicNum = randnum - MusicFileList[MUSIC_ATTACK].size();
                filename = MusicFileList[MUSIC_PEACE][musicNum];
                currentMusicType = MUSIC_PEACE;
            }
        }
	}

	if((musicOn == true) && (filename != "")) {

		Mix_HaltMusic();

		if(music != NULL) {
			Mix_FreeMusic(music);
			music = NULL;
		}

		music = Mix_LoadMUS(filename.c_str());
		if(music != NULL) {
			printf("Now playing %s!\n",filename.c_str());
			Mix_PlayMusic(music, -1);
		} else {
			printf("Unable to play %s!\n",filename.c_str());
		}
	}
}

void DirectoryPlayer::musicCheck() {
	if(musicOn) {
		if(!Mix_PlayingMusic()) {
			changeMusic(MUSIC_PEACE);
		}
	}
}

void DirectoryPlayer::setMusic(bool value) {
	musicOn = value;

	if(musicOn) {
		changeMusic(MUSIC_RANDOM);
	} else if(music != NULL) {
		Mix_HaltMusic();
	}
}


void DirectoryPlayer::toggleSound()
{
	if(musicOn == false) {
		musicOn = true;
		changeMusic(MUSIC_PEACE);
	} else {
		musicOn = false;
		if (music != NULL) {
			Mix_HaltMusic();
            Mix_FreeMusic(music);
            music = NULL;
		}
	}
}

std::vector<std::string> DirectoryPlayer::getMusicFileNames(std::string dir) {
	std::vector<std::string> Files;
	std::list<std::string> tmp;
	std::list<std::string>::const_iterator iter;

	tmp = GetFileNamesList(dir,"mp3",true);
	for(iter = tmp.begin(); iter != tmp.end(); ++iter) {
		Files.push_back(dir + *iter);
	}

	tmp = GetFileNamesList(dir,"ogg",true);
	for(iter = tmp.begin(); iter != tmp.end(); ++iter) {
		Files.push_back(dir + *iter);
	}

	tmp = GetFileNamesList(dir,"wav",true);
	for(iter = tmp.begin(); iter != tmp.end(); ++iter) {
		Files.push_back(dir + *iter);
	}

	tmp = GetFileNamesList(dir,"mid",true);
	for(iter = tmp.begin(); iter != tmp.end(); ++iter) {
		Files.push_back(dir + *iter);
	}

	return Files;
}
