#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

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


#include <Definitions.h>

#include <SDL_mixer.h>

//! \enum MUSICTYPE
/*! Types of music available in the game*/
typedef enum {  MUSIC_ATTACK = 0,   /*!< Played when at least one of player's units was hit. */
                MUSIC_PEACE,        /*!< Played most of the time when the enemy is not attacking. */
                MUSIC_INTRO,        /*!< Background music for intro. */
                MUSIC_MENU,         /*!< The menu background music. */
                MUSIC_BRIEFING_A,   /*!< Atreides briefing background music. */
                MUSIC_BRIEFING_O,   /*!< Ordos briefing background music. */
                MUSIC_BRIEFING_H,   /*!< Harkonnen briefing background music. */
                MUSIC_WIN_A,        /*!< Atreides victory screen background music.. */
                MUSIC_WIN_O,        /*!< Ordos victory screen background music.. */
                MUSIC_WIN_H,        /*!< Harkonnen victory screen background music.. */
                MUSIC_LOSE,         /*!< Failure screen background music. */
                MUSIC_GAMESTAT,     /*!< Game statistics background music. */
                MUSIC_MAPCHOICE,    /*!< The map choice background music. */
                MUSIC_MEANWHILE,    /*!< Meanwhile cutscene background music. */
                MUSIC_FINALE_A,     /*!< Atreides finale cutscene background music. */
                MUSIC_FINALE_O,     /*!< Ordos finale cutscene background music. */
                MUSIC_FINALE_H,     /*!< Harkonnen finale cutscene background music. */
                MUSIC_NUM_MUSIC_TYPES,

                MUSIC_RANDOM = 0xFF /*!< Random music (attack music or peace music) */
			 }  MUSICTYPE;

class MusicPlayer
{
public:
    MusicPlayer() : musicOn(true), musicVolume(MIX_MAX_VOLUME/2), thisMusicID(INVALID), currentMusicType(MUSIC_RANDOM) { };
    virtual ~MusicPlayer() { };

	/*!
		change type of current music
		@param musicType type of music to be played
	*/
	virtual void changeMusic(MUSICTYPE musicType) = 0;

    /*!
		sets current music to MUSIC_PEACE if there's no
		other song being played
	*/
	virtual void musicCheck() = 0;

    /*!
        Toggle the music on and off
    */
	virtual void toggleSound() = 0;

    /*!
		turns music playing on or off
		@param value when true the function turns music on
	*/
	virtual void setMusic(bool value) = 0;

protected:
    //! whether music should be played
	bool	musicOn;

	//! music volume
	int musicVolume;

    //! id of currently played music
	int	thisMusicID;

    MUSICTYPE currentMusicType;
};

#endif // MUSICPLAYER_H
