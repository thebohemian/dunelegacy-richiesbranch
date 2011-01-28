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

#ifndef GAMEINILOADER_H
#define GAMEINILOADER_H

#include <FileClasses/INIFile.h>
#include <DataTypes.h>

#include <SDL.h>

#include <string>
#include <map>
#include <misc/memory.h>


// forward declarations
class Game;
class House;

class GameINILoader {
public:
	GameINILoader(Game* pGameClass, std::string mapname);
	~GameINILoader();

	static SDL_Surface* createMinimapImageOfMap(const INIFile& map);

private:
    void load();
    void loadMap();
    void loadHouses();
    void loadChoam();
    void loadUnits();
    void loadStructures();
    void loadReinforcements();
    void loadView();

    House* getOrCreateHouse(int house);
    HOUSETYPE getHouseID(std::string name);

    inline int getXPos(int pos) const { return (pos % logicalSizeX) - logicalOffsetX; };
    inline int getYPos(int pos) const { return (pos / logicalSizeX) - logicalOffsetY; };

    Game* pGameClass;
    std::string mapname;
    std::shared_ptr<INIFile> inifile;
    std::map<std::string, HOUSETYPE> housename2house;

    int version;

    int sizeX;
    int sizeY;
    int logicalSizeX;
    int logicalSizeY;
    int logicalOffsetX;
    int logicalOffsetY;
};

#endif //GAMEINILOADER_H
