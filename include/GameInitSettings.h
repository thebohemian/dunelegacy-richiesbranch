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

#ifndef GAMEINITINFOCLASS_H
#define GAMEINITINFOCLASS_H

#include "Definitions.h"
#include "DataTypes.h"
#include <misc/Stream.h>

#include <string>


class GameInitSettings
{
public:

    class PlayerInfo {
    public:
        PlayerInfo(std::string newPlayerName, std::string newPlayerClass, std::string newPlayerClassParameter = "")
         : PlayerName(newPlayerName), PlayerClass(newPlayerClass), PlayerClassParameter(newPlayerClassParameter) {
        }

        PlayerInfo(Stream& stream) {
            PlayerName = stream.readString();
            PlayerClass = stream.readString();
            PlayerClassParameter = stream.readString();
        }

        void save(Stream& stream) const {
            stream.writeString(PlayerName);
            stream.writeString(PlayerClass);
            stream.writeString(PlayerClassParameter);
        }

        std::string PlayerName;
        std::string PlayerClass;
        std::string PlayerClassParameter;
    };

    class HouseInfo {
    public:
        HouseInfo(HOUSETYPE newHouseID, int newTeam)
         : HouseID(newHouseID), team(newTeam) {
        }

        HouseInfo(Stream& stream) {
            HouseID = (HOUSETYPE) stream.readSint32();
            team = stream.readSint32();

            Uint32 numPlayerInfo = stream.readUint32();
            for(Uint32 i=0;i<numPlayerInfo;i++) {
                playerInfoList.push_back(PlayerInfo(stream));
            }
        }

        void save(Stream& stream) const {
            stream.writeSint32(HouseID);
            stream.writeSint32(team);

            stream.writeUint32(playerInfoList.size());
            PlayerInfoList::const_iterator iter;
            for(iter = playerInfoList.begin(); iter != playerInfoList.end(); ++iter) {
                iter->save(stream);
            }
        }

        inline void addPlayerInfo(const PlayerInfo& newPlayerInfo) { playerInfoList.push_back(newPlayerInfo); };

        typedef std::vector<PlayerInfo> PlayerInfoList;

        HOUSETYPE       HouseID;
        int             team;
        PlayerInfoList  playerInfoList;
    };

    typedef std::vector<HouseInfo> HouseInfoList;


    /**
        Default constructor.
        The constructed GameInitSettings object is empty
    */
	GameInitSettings();

	/**
        Constructor for specifying the start of a campaign
        \param  newHouseID          the house to play the campaign with
        \param  bConcreteRequired   are buildings built on non concrete tiles damaged?
        \param  bFogOfWar           is there a fog of war unrevealing tiles after some time?
	*/
	GameInitSettings(HOUSETYPE newHouseID, bool bConcreteRequired, bool bFogOfWar);

	/**
        Constructor for continuing a campaign at the specified mission
        \param  prevGameInitInfoClass   the init settings of the previous mission in the campaign
        \param  nextMission             the number of the mission to continue the campaign
	*/
	GameInitSettings(const GameInitSettings& prevGameInitInfoClass, int nextMission);

	/**
        Constructor for specifying the start of a skirmish mission in the campaing
        \param  newHouseID          the house specifying from which campaign the mission is from
        \param  newMission          the number of the mission (1 - 22)
        \param  bConcreteRequired   are buildings built on non concrete tiles damaged?
        \param  bFogOfWar           is there a fog of war unrevealing tiles after some time?
	*/
	GameInitSettings(HOUSETYPE newHouseID, int newMission, bool bConcreteRequired, bool bFogOfWar);

	/**
        Constructor for specifying the start of a custom map
        \param  mapfile             the path to the map file
        \param  bConcreteRequired   are buildings built on non concrete tiles damaged?
        \param  bFogOfWar           is there a fog of war unrevealing tiles after some time?
	*/
	GameInitSettings(std::string mapfile, bool bConcreteRequired, bool bFogOfWar);

	/**
        Constructor for specifying the loading of a savegame. If the given filename contains no valid savegame
        an exception is thrown.
        \param  savegame    the name of the savegame
	*/
	GameInitSettings(std::string savegame);

	/**
        Load the game init info from a stream
        \param  stream  the stream to load from
	*/
	GameInitSettings(Stream& stream);

	~GameInitSettings();

	void save(Stream& stream) const;

	inline GAMETYPE getGameType() const { return gameType; };
	inline HOUSETYPE getHouseID() const { return HouseID; };
	inline int getMission() const { return mission; };
	inline std::string getFilename() const { return filename; };
	inline Uint32 getRandomSeed() const { return randomSeed; };

	inline bool isConcreteRequired() const { return concreteRequired; };
	inline bool hasFogOfWar() const { return fogOfWar; };

	inline void addHouseInfo(const HouseInfo& newHouseInfo) { houseInfoList.push_back(newHouseInfo); };
	inline const HouseInfoList& getHouseInfoList() const { return houseInfoList; };

	inline void setHouseID(HOUSETYPE houseID) { HouseID = houseID; };

private:
    static std::string getScenarioFilename(HOUSETYPE newHouse, int mission);

    /**
        This method checks if it is possible to load a savegame and if the magic number is correct. If there is an error an exception is thrown.
        \param savegame the name of the file to check
    */
    static void checkSaveGame(std::string savegame);


    GAMETYPE		gameType;

	HOUSETYPE		HouseID;
    int				mission;

	std::string		filename;

	Uint32          randomSeed;

    bool		    concreteRequired;
	bool            fogOfWar;

    HouseInfoList   houseInfoList;
};

#endif // GAMEINITINFOCLASS_H
