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

#ifndef DATATYPES_H
#define DATATYPES_H

#include <Definitions.h>

// Libraries
#include <SDL.h>
#include <string>


class Coord {
public:
	Coord() {
		x = y = 0;
	}

	Coord(int x,int y) {
		this->x = x;
		this->y = y;
	}

	inline bool operator==(const Coord& c) const {
        return (x == c.x && y == c.y);
	}

	inline bool operator!=(const Coord& c) const {
        return !operator==(c);
	}

	inline Coord& operator+=(const Coord& c) {
        x += c.x;
        y += c.y;
        return *this;
	}

    inline Coord operator+(const Coord& c) const {
        Coord ret = *this;
        ret += c;
        return ret;
	}

    inline Coord& operator-=(const Coord& c) {
        x -= c.x;
        y -= c.y;
        return *this;
	}

    inline Coord operator-(const Coord& c) const {
        Coord ret = *this;
        ret -= c;
        return ret;
	}

    inline Coord& operator*=(int c) {
        x *= c;
        y *= c;
        return *this;
	}

    inline Coord operator*(int c) const {
        Coord ret = *this;
        ret *= c;
        return ret;
	}

    inline Coord& operator/=(int c) {
        x /= c;
        y /= c;
        return *this;
	}

    inline Coord operator/(int c) const {
        Coord ret = *this;
        ret /= c;
        return ret;
	}

	int	x,y;
};

typedef enum {
    ATTACKMODE_INVALID = -1,
    GUARD = 0,      ///< The unit will attack enemy units but will not move or follow enemy units.
    AREAGUARD=1,    ///< Area Guard is the most common command for pre-placed AI units. They will scan for targets in a relatively large radius, and return to their original position after their target was either destroyed or left the immediate area.
    STOP = 2,
    AMBUSH = 3,     ///< Ambush means a unit will remain in position until sighted by the enemy, and then proceed to attack any enemy units it might find on the map.
    HUNT=4,         ///< Hunt makes a unit start from its position towards enemy units, even if the player has not sighted the AI (normally the AI will not attack until there has been a contact between the player's and the AI's units). Also works for human units, they'll go towards any enemy units on the map just as the mission starts.
    CAPTURE =5,     ///< Capture is only used for infantry units when ordered to capture a building
    ATTACKMODE_MAX
} ATTACKMODE;

typedef enum {NOTBLOCKED, COMBLOCKED, MOUNTAIN, INFANTRY} BLOCKEDTYPE;
typedef enum {EASY = 0, MEDIUM = 1, HARD = 2} DIFFICULTYTYPE;
typedef enum {START, LOADING, BEGUN, DEINITIALIZE} GAMESTATETYPE;

typedef enum {
    GAMETYPE_INVALID            = -1,
    GAMETYPE_LOAD_SAVEGAME      = 0,
    GAMETYPE_CAMPAIGN           = 1,
    GAMETYPE_CUSTOM             = 2,
    GAMETYPE_SKIRMISH           = 3,
    GAMETYPE_CUSTOM_MULTIPLAYER = 4
} GAMETYPE;


class SettingsClass
{
public:
	class SettingsClass_GeneralClass {
	public:
		bool		PlayIntro;
		bool		ConcreteRequired;
		bool        FogOfWar;
		std::string	PlayerName;
		int			Language;
		std::string	LanguageExt;

		void setLanguage(int newLanguage) {
			switch(newLanguage) {
				case LNG_ENG:
					Language = LNG_ENG;
					LanguageExt = "ENG";
					break;
				case LNG_FRE:
					Language = LNG_FRE;
					LanguageExt = "FRE";
					break;
				case LNG_GER:
					Language = LNG_GER;
					LanguageExt = "GER";
					break;
				default:
					return;
			}
		}

		std::string getLanguageString() {
			switch(Language) {
				case LNG_ENG:
					return "en";
				case LNG_FRE:
					return "fr";
				case LNG_GER:
					return "de";
				default:
					return "";
			}
		}
	} General;

	class SettingsClass_VideoClass {
	public:
		bool	DoubleBuffering;
		bool	Fullscreen;
		int		Width;
		int		Height;
        bool	FrameLimit;
	} Video;

	class SettingsClass_AudioClass {
	public:
		std::string MusicType;
	} Audio;

	DIFFICULTYTYPE	playerDifficulty;
};

typedef enum
{
    HOUSE_UNUSED = -2,
    HOUSE_INVALID = -1,
	HOUSE_ATREIDES = 0,		//house definitions
	HOUSE_ORDOS,
	HOUSE_HARKONNEN,
	HOUSE_SARDAUKAR,
	HOUSE_FREMEN,
	HOUSE_MERCENARY,
	NUM_HOUSES
} HOUSETYPE;

typedef enum {
    RIGHT,
    RIGHTUP,
    UP,
    LEFTUP,
    LEFT,
    LEFTDOWN,
    DOWN,
    RIGHTDOWN,
    NUM_ANGLES
} ANGLETYPE;

#endif //DATATYPES_H
