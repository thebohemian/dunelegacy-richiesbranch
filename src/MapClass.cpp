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

#include <MapClass.h>

#include <globals.h>

#include <Game.h>
#include <House.h>
#include <ScreenBorder.h>

#include <units/UnitClass.h>
#include <structures/StructureClass.h>

#include <stack>

MapClass::MapClass(int xSize, int ySize)
{
	sizeX = xSize;
	sizeY = ySize;

	lastSinglySelectedObject = NULL;

	cell = new TerrainClass*[xSize];
	for (int i = 0; i < xSize; i++) {
		cell[i] = new TerrainClass[ySize];
	}

	for (int i=0; i<xSize; i++) {
		for (int j=0; j<ySize; j++) {
			cell[i][j].location.x = i;
			cell[i][j].location.y = j;
		}
	}
}


MapClass::~MapClass()
{
	for(int i = 0; i < sizeX; i++) {
		delete[] cell[i];
	}

	delete[] cell;
}

void MapClass::load(Stream& stream)
{
	sizeX = stream.readSint32();
	sizeY = stream.readSint32();
	for (int i = 0; i < sizeX; i++) {
		for (int j = 0; j < sizeY; j++) {
			cell[i][j].load(stream);
			cell[i][j].location.x = i;
			cell[i][j].location.y = j;
		}
	}
}

void MapClass::save(Stream& stream) const
{
	stream.writeSint32(sizeX);
	stream.writeSint32(sizeY);

	for (int i = 0; i < sizeX; i++) {
		for (int j = 0; j < sizeY; j++) {
			cell[i][j].save(stream);
		}
	}
}

void MapClass::createSandRegions()
{
	std::stack<TerrainClass*> terrainQueue;
	std::vector<bool> visited(sizeX * sizeY);

	for(int i = 0; i < sizeX; i++) {
		for(int j = 0; j < sizeY; j++)	{
			cell[i][j].setSandRegion(NONE);
		}
	}

    int	region = 0;
	for(int i = 0; i < sizeX; i++) {
		for(int j = 0; j < sizeY; j++) {
			if(!cell[i][j].isRock() && !visited[j*sizeX+i]) {
				terrainQueue.push(&cell[i][j]);

				while(!terrainQueue.empty()) {
					TerrainClass* pTerrain = terrainQueue.top();
					terrainQueue.pop();

					pTerrain->setSandRegion(region);
					for(int angle = 0; angle < NUM_ANGLES; angle++) {
						Coord pos = getMapPos(angle, pTerrain->location);
						if(cellExists(pos) && !getCell(pos)->isRock() && !visited[pos.y*sizeX + pos.x]) {
							terrainQueue.push(getCell(pos));
							visited[pos.y*sizeX + pos.x] = true;
						}
					}
				}
				region++;
			}
		}
	}
}

void MapClass::damage(Uint32 damagerID, House* damagerOwner, const Coord& realPos, Uint32 bulletID, int damage, int damageRadius, bool air)
{
	Coord centerCell = Coord(realPos.x/BLOCKSIZE, realPos.y/BLOCKSIZE);

	for(int i = centerCell.x-2; i <= centerCell.x+2; i++) {
		for(int j = centerCell.y-2; j <= centerCell.y+2; j++) {
			if(cellExists(i, j)) {
				getCell(i, j)->damageCell(damagerID, damagerOwner, realPos, bulletID, damage, damageRadius, air);
			}
		}
	}

	if((bulletID != Bullet_Sonic) && cellExists(centerCell) && getCell(centerCell)->isSpiceBloom()) {
        getCell(centerCell)->triggerSpiceBloom(damagerOwner);
	}
}

bool MapClass::okayToPlaceStructure(int x, int y, int buildingSizeX, int buildingSizeY, bool tilesRequired, House* pHouse)
{
	bool withinBuildRange = false;

	for (int i = x; i < x + buildingSizeX; i++)
	{
		for (int j = y; j < y + buildingSizeY; j++)
		{
			if (!currentGameMap->cellExists(i,j) || !currentGameMap->getCell(i,j)->isRock() || (tilesRequired && !currentGameMap->getCell(i,j)->isConcrete()) || currentGameMap->getCell(i,j)->isMountain() || currentGameMap->getCell(i,j)->hasAGroundObject())
				return false;

			if ((pHouse == NULL) || isWithinBuildRange(i, j, pHouse))
				withinBuildRange = true;
		}
	}
	return withinBuildRange;
}


bool MapClass::isWithinBuildRange(int x, int y, House* pHouse)
{
	bool withinBuildRange = false;

	for (int i = x - BUILDRANGE; i <= x + BUILDRANGE; i++)
		for (int j = y - BUILDRANGE; j <= y + BUILDRANGE; j++)
			if (cellExists(i, j) && (cell[i][j].getOwner() == pHouse->getHouseID()))
				withinBuildRange = true;

	return withinBuildRange;
}


bool MapClass::okayToBuildExclusive(int x, int y, int buildingSizeX, int buildingSizeY)
{
	for (int i = 0; i < buildingSizeX; i++)
	{
		for (int j = 0; j < buildingSizeY; j++)
		{
			if (((cellBlocked(x+i, y+j) == NOTBLOCKED) || ((i == 0) && (j == 0))))
			{
				if (!cell[x+i][y+j].isRock())
				{
					//printf("%d, %d is not rock\n", x+i, y+j);
					return false;
				}
			}
			else
			{
				//printf("%d, %d is blocked\n", x+i, y+j);
				return false;
			}
		}
	}

	return true;
}

/**
    This method figures out the direction of tile pos relative to tile source.
    \param  source  the starting point
    \param  pos     the destination
    \return one of RIGHT, RIGHTUP, UP, LEFTUP, LEFT, LEFTDOWN, DOWN, RIGHTDOWN or INVALID
*/
int MapClass::getPosAngle(const Coord& source, const Coord& pos) const
{
	if(pos.x > source.x) {
		if(pos.y > source.y) {
			return RIGHTDOWN;
        } else if(pos.y < source.y) {
			return RIGHTUP;
		} else {
			return RIGHT;
		}
	} else if(pos.x < source.x) {
		if(pos.y > source.y) {
			return LEFTDOWN;
		} else if(pos.y < source.y) {
			return LEFTUP;
		} else {
			return LEFT;
		}
	} else {
		if(pos.y > source.y) {
			return DOWN;
		} else if(pos.y < source.y) {
			return UP;
		} else {
			return INVALID;
		}
	}
}

BLOCKEDTYPE MapClass::cellBlocked(int xPos, int yPos)
{
        BLOCKEDTYPE blocked = COMBLOCKED;
        if (cellExists(xPos, yPos))
		{
			if (cell[xPos][yPos].getType() == Terrain_Mountain)
				blocked = MOUNTAIN;
			else if (cell[xPos][yPos].hasAnObject())
			{
				if (cell[xPos][yPos].getObject()->isInfantry())
					blocked = INFANTRY;
				else
					blocked = COMBLOCKED;
			}
			else
				blocked = NOTBLOCKED;
		}

        return blocked;
}

/**
    This method calculates the coordinate of one of the neighbour cells of source
    \param  angle   one of RIGHT, RIGHTUP, UP, LEFTUP, LEFT, LEFTDOWN, DOWN, RIGHTDOWN
    \param  source  the cell to calculate neigbour cell from
*/
Coord MapClass::getMapPos(int angle, const Coord& source) const
{
	switch (angle)
	{
		case (RIGHT):       return Coord(source.x + 1 , source.y);       break;
		case (RIGHTUP):     return Coord(source.x + 1 , source.y - 1);   break;
		case (UP):          return Coord(source.x     , source.y - 1);   break;
		case (LEFTUP):      return Coord(source.x - 1 , source.y - 1);   break;
		case (LEFT):        return Coord(source.x - 1 , source.y);       break;
		case (LEFTDOWN):    return Coord(source.x - 1 , source.y + 1);   break;
		case (DOWN):        return Coord(source.x     , source.y + 1);   break;
		case (RIGHTDOWN):   return Coord(source.x + 1 , source.y + 1);   break;
		default:            return Coord(source.x     , source.y);       break;
	}
}

//building size is num squares
Coord MapClass::findDeploySpot(UnitClass* pUnit, const Coord origin, const Coord gatherPoint, const Coord buildingSize) const
{
	double		closestDistance = 1000000000.0;
	Coord       closestPoint;
	Coord		size;

	bool	found = false;
    bool    foundClosest = false;

	int	counter = 0;
	int	depth = 0;
	int edge;

    if(pUnit->isAFlyingUnit()) {
        return origin;
    }

	int ranX = origin.x;
	int ranY = origin.y;

	do {
		edge = currentGame->RandomGen.rand(0, 3);
		switch(edge) {
            case 0:	//right edge
                ranX = origin.x + buildingSize.x + depth;
                ranY = currentGame->RandomGen.rand(origin.y - depth, origin.y + buildingSize.y + depth);
                break;
            case 1:	//top edge
                ranX = currentGame->RandomGen.rand(origin.x - depth, origin.x + buildingSize.x + depth);
                ranY = origin.y - depth - ((buildingSize.y == 0) ? 0 : 1);
                break;
            case 2:	//left edge
                ranX = origin.x - depth - ((buildingSize.x == 0) ? 0 : 1);
                ranY = currentGame->RandomGen.rand(origin.y - depth, origin.y + buildingSize.y + depth);
                break;
            case 3: //bottom edge
                ranX = currentGame->RandomGen.rand(origin.x - depth, origin.x + buildingSize.x + depth);
                ranY = origin.y + buildingSize.y + depth;
                break;
            default:
                break;
		}

		bool bOK2Deploy = pUnit->canPass(ranX, ranY);

		if(pUnit->isTracked() && cellExists(ranX, ranY) && getCell(ranX, ranY)->hasInfantry()) {
		    // we do not deploy on enemy infantry
		    bOK2Deploy = false;
		}

		if(bOK2Deploy) {
			if(gatherPoint.x == INVALID_POS || gatherPoint.y == INVALID_POS) {
				closestPoint.x = ranX;
				closestPoint.y = ranY;
				found = true;
			} else {
				Coord temp = Coord(ranX, ranY);
				if(blockDistance(temp, gatherPoint) < closestDistance) {
					closestDistance = blockDistance(temp, gatherPoint);
					closestPoint.x = ranX;
					closestPoint.y = ranY;
					foundClosest = true;
				}
			}
		}

		if(counter++ >= 100) {
		    //if hasn't found a spot on tempObject layer in 100 tries, goto next

			counter = 0;
			if(++depth > (std::max(currentGameMap->sizeX, currentGameMap->sizeY))) {
				closestPoint.x = INVALID_POS;
				closestPoint.y = INVALID_POS;
				found = true;
				fprintf(stderr, "Map full\n"); fflush(stderr);
			}
		}
	} while (!found && (!foundClosest || (counter > 0)));

	return closestPoint;
}

/**
    This method finds the tile which is at a map border and is at minimal distance to the structure
    specified by origin and buildingsSize. This method is especcially useful for Carryalls and Frigates
    that have to enter the map to deliver units.
    \param origin           the position of the structure in map coordinates
    \param buildingSize    the number of tiles occupied by the building (e.g. 3x2 for refinery)
*/
Coord MapClass::findClosestEdgePoint(const Coord& origin, const Coord& buildingSize) const
{
	int closestDistance = NONE;
	Coord closestPoint;

	if(origin.x < (sizeX - (origin.x + buildingSize.x))) {
		closestPoint.x = 0;
		closestDistance = origin.x;
	} else {
		closestPoint.x = sizeX - 1;
		closestDistance = sizeX - (origin.x + buildingSize.x);
	}
	closestPoint.y = origin.y;

	if(origin.y < closestDistance) {
		closestPoint.x = origin.x;
		closestPoint.y = 0;
		closestDistance = origin.y;
	}

	if((sizeY - (origin.y + buildingSize.y)) < closestDistance) {
		closestPoint.x = origin.x;
		closestPoint.y = sizeY - 1;
		closestDistance = origin.y;
	}

	return closestPoint;
}


void MapClass::removeObjectFromMap(Uint32 ObjectID)
{
	for(int y = 0; y < sizeY ; y++) {
		for(int x = 0 ; x < sizeX ; x++) {
			cell[x][y].unassignObject(ObjectID);
		}
	}
}

void MapClass::selectObjects(int houseID, int x1, int y1, int x2, int y2, int realX, int realY, bool objectARGMode)
{
	ObjectClass	*lastCheckedObject = NULL,
				*lastSelectedObject = NULL;

	//if selection rectangle is checking only one cell and has shift selected we want to add/ remove that unit from the selected group of units
	if(!objectARGMode) {
		currentGame->unselectAll(currentGame->getSelectedList());
		currentGame->getSelectedList().clear();
	}

	if((x1 == x2) && (y1 == y2) && cellExists(x1, y1)) {

        if(cell[x1][y1].isExplored(houseID) || debug) {
            lastCheckedObject = cell[x1][y1].getObjectAt(realX, realY);
        } else {
		    lastCheckedObject = NULL;
		}

		if((lastCheckedObject != NULL) && (lastCheckedObject->getOwner()->getHouseID() == houseID)) {
			if((lastCheckedObject == lastSinglySelectedObject) && ( !lastCheckedObject->isAStructure())) {
                for(int i = screenborder->getTopLeftTile().x; i <= screenborder->getBottomRightTile().x; i++) {
                    for(int j = screenborder->getTopLeftTile().y; j <= screenborder->getBottomRightTile().y; j++) {
                        if(cellExists(i, j) && cell[i][j].hasAnObject()) {
                            cell[i][j].selectAllPlayersUnitsOfType(houseID, lastSinglySelectedObject->getItemID(), &lastCheckedObject, &lastSelectedObject);
                        }
                    }
				}
				lastSinglySelectedObject = NULL;

			} else if(!lastCheckedObject->isSelected())	{

				lastCheckedObject->setSelected(true);
				currentGame->getSelectedList().insert(lastCheckedObject->getObjectID());
				lastSelectedObject = lastCheckedObject;
				lastSinglySelectedObject = lastSelectedObject;

			} else if(objectARGMode) {
			    //holding down shift, unselect this unit
				lastCheckedObject->setSelected(false);
				currentGame->getSelectedList().erase(lastCheckedObject->getObjectID());
			}

		} else {
			lastSinglySelectedObject = NULL;
		}

	} else {
		lastSinglySelectedObject = NULL;
		for (int i = std::min(x1, x2); i <= std::max(x1, x2); i++)
		for (int j = std::min(y1, y2); j <= std::max(y1, y2); j++)
			if (cellExists(i, j) && cell[i][j].hasAnObject()
				&& cell[i][j].isExplored(houseID)
				&& !cell[i][j].isFogged(houseID) )
					cell[i][j].selectAllPlayersUnits(houseID, &lastCheckedObject, &lastSelectedObject);
	}

	//select an enemy unit if none of your units found
	if(currentGame->getSelectedList().empty() && (lastCheckedObject != NULL) && !lastCheckedObject->isSelected()) {
		lastCheckedObject->setSelected(true);
		lastSelectedObject = lastCheckedObject;
		currentGame->getSelectedList().insert(lastCheckedObject->getObjectID());
	} else if (lastSelectedObject != NULL) {
		lastSelectedObject->playSelectSound();	//we only want one unit responding
	}

/*
	if ((selectedList->getNumElements() == 1) && lastSelectedObject && lastSelectedObject->isAStructure() && ((StructureClass*)lastSelectedObject)->isABuilder())
		((BuilderClass*)lastSelectedObject)->checkSelectionList();*/
}


bool MapClass::findSpice(Coord* destination, Coord* origin)
{
	bool found = false;

	int	counter = 0,
		depth = 1,
		x = origin->x,
		y = origin->y,
		ranX,
		ranY;

	do
	{
		do
		{
			ranX = currentGame->RandomGen.rand(x-depth, x + depth);
			ranY = currentGame->RandomGen.rand(y-depth, y + depth);
		} while (((ranX >= (x+1 - depth)) && (ranX < (x + depth))) && ((ranY >= (y+1 - depth)) && (ranY < (y + depth))));

		if (cellExists(ranX,ranY) && !cell[ranX][ranY].hasAGroundObject() && cell[ranX][ranY].hasSpice())
		{
			found = true;
			destination->x = ranX;
			destination->y = ranY;
		}

		counter++;
		if (counter >= 100)		//if hasn't found a spot on tempObject layer in 100 tries, goto next
		{
			counter = 0;
			depth++;
		}
		if (depth > std::max(sizeX, sizeY))
			return false;	//there is possibly no spice left anywhere on map
	} while (!found);

	if ((depth > 1) && (cell[origin->x][origin->y].hasSpice()))
	{
		destination->x = origin->x;
		destination->y = origin->y;
	}

	return true;
}

/**
    This method fixes surounding thick spice tiles after spice gone to make things look smooth.
    \param coord    the coordinate to remove spice from
*/
void MapClass::removeSpice(const Coord& coord)
{
	if(cellExists(coord)) {	//this is the centre cell
		if (cell[coord.x][coord.y].getType() == Terrain_ThickSpice) {
			cell[coord.x][coord.y].setType(Terrain_Spice);
		} else {
			cell[coord.x][coord.y].setType(Terrain_Sand);

			//thickspice tiles cant handle non-(thick)spice tiles next to them, if there is after changes, make it non thick
			for(int i = coord.x-1; i <= coord.x+1; i++) {
                for(int j = coord.y-1; j <= coord.y+1; j++) {
                    if (cellExists(i, j) && (((i==coord.x) && (j!=coord.y)) || ((i!=coord.x) && (j==coord.y))) && cell[i][j].isThickSpice()) {
                        //only check cell, right, up, left and down of this one
                        cell[i][j].setType(Terrain_Spice);
                    }
                }
			}
		}
	}
}

void MapClass::viewMap(int playerTeam, const Coord& location, int maxViewRange)
{
	int			i;
    Coord   check;

//makes map viewable in an area like as shown below

//				       *****
//                   *********
//                  *****T*****
//                   *********
//                     *****

	check.x = location.x - maxViewRange;
	if (check.x < 0)
		check.x = 0;

	while ((check.x < sizeX) && ((check.x - location.x) <=  maxViewRange))
	{
		check.y = (location.y - lookDist[abs(check.x - location.x)]);
		if (check.y < 0) check.y = 0;

		while ((check.y < sizeY) && ((check.y - location.y) <= lookDist[abs(check.x - location.x)]))
		{
			if (distance_from(location, check) <= maxViewRange)
			for (i = 0; i < MAX_PLAYERS; i++)
				if (currentGame->house[i] && (currentGame->house[i]->getTeam() == playerTeam))
					cell[check.x][check.y].setExplored(i,currentGame->GetGameCycleCount());

			check.y++;
		}

		check.x++;
		check.y = location.y;
	}
}

void MapClass::viewMap(int playerTeam, int x, int y, int maxViewRange)
{
	viewMap(playerTeam, Coord(x,y), maxViewRange);
}


ObjectClass* MapClass::findObjectWidthID(int objectID, int lx, int ly)
{
	ObjectClass	*object = NULL;

	if (cellExists(lx, ly))
		object = getCell(lx, ly)->getObjectWithID(objectID);


	if(object == NULL) {
        //object wasn't found in expected cell
        //search surrounding cells

		for(int x=lx-5; x<lx+5 && !object; x++) {
			for(int y=ly-5; y<ly+5 && !object; y++) {
				if (cellExists(x, y)) {
					object = getCell(x, y)->getObjectWithID(objectID);
				}
			}
		}

		if(object == NULL) {
		    //object wasn't found in surrounding cells
            //search lists

            RobustList<UnitClass*>::const_iterator iter;
            for(iter = unitList.begin(); iter != unitList.end(); ++iter) {
				if((*iter)->hasObjectID(objectID)) {
                    object = *iter;
                    break;
				}
			}

			if(object == NULL) {
			    //object wasn't found in units
                RobustList<StructureClass*>::const_iterator iter;
                for(iter = structureList.begin(); iter != structureList.end(); ++iter) {
                    if((*iter)->hasObjectID(objectID)) {
                        object = *iter;
                        break;
                    }
                }
			}
		}
	}

	return object;
}
