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

#ifndef TEXTMANAGER_H
#define TEXTMANAGER_H

#include "BriefingText.h"
#include "IndexedTextFile.h"

#include <string>
#include <map>
#include <misc/memory.h>

#include <algorithm>

typedef enum {
    DuneText_Score = 21,
    DuneText_Time = 22,
    DuneText_You_have_attained_the_rank = 23,
    DuneText_UnitsDestroyedBy = 24,
    DuneText_BuildingsDestroyedBy = 25,
    DuneText_SpiceHarvestedBy = 26,

    DuneText_ConstructionComplete = 51,
    DuneText_SaboteurApproaching = 79,
    DuneText_FrigateArrived = 80,
    DuneText_MissileApproaching = 81,

    DuneText_IsPercentFull = 121,
    DuneText_IsPercentFullAndHarvesting = 122,
    DuneText_IsPercentFullAndHeadingBack = 123,
    DuneText_IsPercentFullAndAwaitingPickup = 124,
    DuneText_IsEmpty = 125,
    DuneText_IsEmptyAndHarvesting = 126,
    DuneText_IsEmptyAndHeadingBack = 127,
    DuneText_IsEmptyAndAwaitingPickup = 128,

    DuneText_CannotPlaceHere = 134,
    DuneText_CannotPlaceFoundationHere = 135,
    DuneText_SpiceLost = 145,

    DuneText_Carryall = 195,
    DuneText_Ornithopter = 197,
    DuneText_Soldier = 203,
    DuneText_Trooper = 205,
    DuneText_Saboteur = 207,
    DuneText_Launcher = 209,
    DuneText_Deviator = 211,
    DuneText_Tank = 213,
    DuneText_SiegeTank = 215,
    DuneText_Devastator = 217,
    DuneText_SonicTank = 219,
    DuneText_Trike = 221,
    DuneText_Raider = 223,
    DuneText_Quad = 225,
    DuneText_Harvester = 227,
    DuneText_MCV = 229,
    DuneText_Sandworm = 231,

    DuneText_Slab1 = 233,
    DuneText_Slab4 = 235,
    DuneText_Palace = 237,
    DuneText_LightFactory = 239,
    DuneText_HeavyFactory = 241,
    DuneText_HighTechFactory = 243,
    DuneText_IX = 245,
    DuneText_WOR = 247,
    DuneText_ConstructionYard = 249,
    DuneText_WindTrap = 251,
    DuneText_Barracks = 253,
    DuneText_StarPort = 255,
    DuneText_Refinery = 257,
    DuneText_RepairYard = 259,
    DuneText_Wall = 261,
    DuneText_GunTurret = 263,
    DuneText_RocketTurret = 265,
    DuneText_Silo = 267,
    DuneText_Radar = 269,


    DuneText_SandFlea = 271,
    DuneText_SandSnake = 272,
    DuneText_DesertMongoose = 273,
    DuneText_SandWarrior = 274,
    DuneText_DuneTrooper = 275,
    DuneText_SquadLeader = 276,
    DuneText_OutpostCommander = 277,
    DuneText_BaseCommander = 278,
    DuneText_Warlord = 279,
    DuneText_ChiefWarlord = 280,
    DuneText_RulerOfArrakis = 281,
    DuneText_Emperor = 282,

    /* Index 287 to 320: Meanwhile texts => see Meanwhile.h  */

    DuneText_You = 329,
    DuneText_Enemy = 330
} DuneTexts;

class TextManager {
public:
    /**
        Default constructor
    */
	TextManager();

	/**
        Destructor
	*/
	~TextManager();

    /**
        This method returns a text from the original Dune II data file DUNE.<extension>
        \param  index   the index of the string in the file (see enum DuneTexts)
        \return the requested string
    */
	std::string getDuneText(int index) {
	    if((index >= DuneText_RulerOfArrakis) && pDuneText->getNumStrings() == 335) {
	        // Dune II 1.0 has 2
	        index = std::max((int) DuneText_RulerOfArrakis, index - 2);
	    }

        return pDuneText->getString(index);
	}

    /**
        This method returns a briefing text for the specified mission.
        \param  mission     the mission number (0 = house description, 1 - 9 missions)
        \param  texttype    one of MISSION_DESCRIPTION, MISSION_WIN, MISSION_LOSE, MISSION_ADVICE
        \param  house       the house to get the text from (only HOUSE_ATREIDES, HOUSE_ORDOS and HOUSE_HARKONNEN)
        \return the briefing text
    */
	std::string	GetBriefingText(unsigned int mission, unsigned int texttype, int house);

    /**
        This method returns a localized version of unlocalizedString
        \param  unlocalizedString   the string in english
        \return the localized version of unlocalizedString
    */
	const std::string& getLocalized(const std::string& unlocalizedString) const {
        std::map<std::string, std::string>::const_iterator iter = localizedString.find(unlocalizedString);
        if(iter != localizedString.end()) {
            return iter->second;
        } else {
            return unlocalizedString;
        }
	}

private:
    /**
        Init French localization
    */
    void Init_Localization_French();

    /**
        Init German localization
    */
    void Init_Localization_German();

    std::shared_ptr<BriefingText> BriefingStrings[3];           ///< The briefing texts for all three houses
    std::shared_ptr<IndexedTextFile> pDuneText;                 ///< The DUNE.<EXTENSION> texts

    std::map<std::string, std::string> localizedString;         ///< The mapping between English text and localized text
};

#endif //TEXTMANAGER_H
