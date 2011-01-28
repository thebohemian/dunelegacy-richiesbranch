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

#include <GameInitSettings.h>

#include <misc/FileStream.h>
#include <misc/string_util.h>

#include <stdlib.h>
#include <string.h>
#include <stdexcept>

GameInitSettings::GameInitSettings()
 : gameType(GAMETYPE_INVALID), HouseID(HOUSE_INVALID), mission(0), concreteRequired(false), fogOfWar(false) {
	randomSeed = rand();
}

GameInitSettings::GameInitSettings(HOUSETYPE newHouseID, bool bConcreteRequired, bool bFogOfWar)
 : gameType(GAMETYPE_CAMPAIGN), HouseID(newHouseID), mission(1), concreteRequired(bConcreteRequired), fogOfWar(bFogOfWar) {
    filename = getScenarioFilename(HouseID, mission);
    randomSeed = rand();
}

GameInitSettings::GameInitSettings(const GameInitSettings& prevGameInitInfoClass, int nextMission) {
    *this = prevGameInitInfoClass;
    mission = nextMission;
    filename = getScenarioFilename(HouseID, mission);
    randomSeed = rand();
}

GameInitSettings::GameInitSettings(HOUSETYPE newHouseID, int newMission, bool bConcreteRequired, bool bFogOfWar)
 : gameType(GAMETYPE_SKIRMISH), HouseID(newHouseID), mission(newMission), concreteRequired(bConcreteRequired), fogOfWar(bFogOfWar) {
    filename = getScenarioFilename(HouseID, mission);
    randomSeed = rand();
}

GameInitSettings::GameInitSettings(std::string mapfile, bool bConcreteRequired, bool bFogOfWar)
 : gameType(GAMETYPE_CUSTOM), HouseID(HOUSE_INVALID), mission(0), filename(mapfile), concreteRequired(bConcreteRequired), fogOfWar(bFogOfWar) {
    randomSeed = rand();
}

GameInitSettings::GameInitSettings(std::string savegame)
 : gameType(GAMETYPE_LOAD_SAVEGAME), HouseID(HOUSE_INVALID), mission(0), concreteRequired(false), fogOfWar(false) {
    checkSaveGame(savegame);
    filename = savegame;
}

GameInitSettings::GameInitSettings(Stream& stream) {
	gameType = (GAMETYPE) stream.readSint8();
	HouseID = (HOUSETYPE) stream.readSint8();

	filename = stream.readString();

	mission = stream.readUint8();
	randomSeed = stream.readUint32();

	concreteRequired = stream.readBool();
	fogOfWar = stream.readBool();

	Uint32 numHouseInfo = stream.readUint32();
	for(Uint32 i=0;i<numHouseInfo;i++) {
        houseInfoList.push_back(HouseInfo(stream));
	}
}

GameInitSettings::~GameInitSettings() {
}

void GameInitSettings::save(Stream& stream) const {
	stream.writeSint8(gameType);
	stream.writeSint8(HouseID);

	stream.writeString(filename);

	stream.writeUint8(mission);
	stream.writeUint32(randomSeed);

	stream.writeBool(concreteRequired);
	stream.writeBool(fogOfWar);

	stream.writeUint32(houseInfoList.size());
	HouseInfoList::const_iterator iter;
	for(iter = houseInfoList.begin(); iter != houseInfoList.end(); ++iter) {
        iter->save(stream);
	}
}



std::string GameInitSettings::getScenarioFilename(HOUSETYPE newHouse, int mission) {
	std::string name = "SCEN?0??.INI";

	if( (mission < 0) || (mission > 22)) {
	    throw std::invalid_argument("GameInitSettings::getScenarioFilename(): There is no mission number " + stringify(mission) + ".");
	}

	switch(newHouse) {
		case HOUSE_ATREIDES:
		{
			name[4] = 'A';
		} break;

		case HOUSE_ORDOS:
		{
			name[4] = 'O';
		} break;

		case HOUSE_HARKONNEN:
		{
			name[4] = 'H';
		} break;

		default:
		{
            throw std::invalid_argument("GameInitSettings::getScenarioFilename(): There are only missions for Atreides, Ordos and Harkonnen");
		} break;
	};

	name[6] = '0' + (mission / 10);
	name[7] = '0' + (mission % 10);

	return name;
}


void GameInitSettings::checkSaveGame(std::string savegame) {
    FileStream fs;

	if(fs.open(savegame.c_str(), "rb") == false) {
		throw std::runtime_error("Cannot open savegame. Make sure you have read access to this savegame!");
	}

    Uint32 magicNum;
    Uint32 savegameVersion;
    std::string duneVersion;
	try {
        magicNum = fs.readUint32();
        savegameVersion = fs.readUint32();
        duneVersion = fs.readString();
	} catch (std::exception& e) {
	    throw std::runtime_error("Cannot load this savegame,\n because it seems to be truncated!");
	}

    if(magicNum != SAVEMAGIC) {
        throw std::runtime_error("Cannot load this savegame,\n because it has a wrong magic number!");
    }

    if(savegameVersion < SAVEGAMEVERSION) {
        throw std::runtime_error("Cannot load this savegame,\n because it was created with an older version:\n" + duneVersion);
    }

    if(savegameVersion > SAVEGAMEVERSION) {
        throw std::runtime_error("Cannot load this savegame,\n because it was created with a newer version:\n" + duneVersion);
    }

	fs.close();
}
