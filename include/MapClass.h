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

#ifndef MAPCLASS_H
#define MAPCLASS_H

#include <TerrainClass.h>

class MapClass
{
public:
    /**
        Creates a map of size xSize x ySize. The map is initialized with all tiles of type Terrain_Sand.
    */
	MapClass(int xSize, int ySize);
	~MapClass();

	void load(Stream& stream);
	void save(Stream& stream) const;

	void createSandRegions();
	void damage(Uint32 damagerID, House* damagerOwner, const Coord& realPos, Uint32 bulletID, int damage, int damageRadius, bool air);
	Coord getMapPos(int angle, const Coord& source) const;
	void removeObjectFromMap(Uint32 ObjectID);
	void removeSpice(const Coord& coord);
	void selectObjects(int houseID, int x1, int y1, int x2, int y2, int realX, int realY, bool objectARGMode);
	void viewMap(int playerTeam, const Coord& location, int maxViewRange);
	void viewMap(int playerTeam, int x, int y, int maxViewRange);

	bool findSpice(Coord* destination, Coord* origin);
	bool perfectlyInACell(ObjectClass* thing);
	bool okayToPlaceStructure(int x, int y, int buildingSizeX, int buildingSizeY, bool tilesRequired, House* pHouse);
	bool isWithinBuildRange(int x, int y, House* pHouse);
	bool okayToBuildExclusive(int x, int y, int buildingSizeX, int buildingSizeY);
	int getPosAngle(const Coord& source, const Coord& pos) const;
	BLOCKEDTYPE cellBlocked(int xPos, int yPos);
	Coord findClosestEdgePoint(const Coord& origin, const Coord& buildingSize) const;
	Coord findDeploySpot(UnitClass* pUnit, const Coord origin, const Coord gatherPoint = Coord(INVALID_POS,INVALID_POS), const Coord buildingSize = Coord(0,0)) const;//building size is num squares
	ObjectClass* findObjectWidthID(int objectID, int lx, int ly);


	inline bool cellExists(int xPos, int yPos) const {
		return ((xPos >= 0) && (xPos < sizeX) && (yPos >= 0) && (yPos < sizeY));
	}

	inline bool cellExists(const Coord& pos) {
		return cellExists(pos.x, pos.y);
	}

	inline TerrainClass* getCell(int xPos, int yPos) const {
		if(cellExists(xPos,yPos))
			return &cell[xPos][yPos];
		else {
			fprintf(stderr,"getCell (%s - %d): cell[%d][%d] does not exist\n",__FILE__,__LINE__,xPos,yPos);
			fflush(stderr);
			return NULL;
		}
	}

	inline TerrainClass* getCell(const Coord& location) const   {
		return getCell(location.x, location.y);
	}

	Sint32	sizeX;          ///< number of tiles this map is wide
	Sint32  sizeY;          ///< number of tiles this map is high

	TerrainClass **cell;    ///< the 2d-array containing all the cells of the map
private:
	ObjectClass* lastSinglySelectedObject;      ///< The last selected object. If selected again all units of the same time are selected
};


#endif // MAPCLASS_H
