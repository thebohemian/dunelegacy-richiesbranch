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

#include <TerrainClass.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>

#include <Game.h>
#include <MapClass.h>
#include <House.h>
#include <SoundPlayer.h>
#include <ScreenBorder.h>
#include <ConcatIterator.h>

#include <structures/StructureClass.h>
#include <units/InfantryClass.h>
#include <units/AirUnit.h>

#define DAMAGEPERCELL 5

TerrainClass::TerrainClass() {
	type = Terrain_Sand;

	for(int i = 0; i < MAX_PLAYERS; i++) {
		explored[i] = false;
		lastAccess[i] = 0;
	}

	fogColor = COLOR_BLACK;

	owner = INVALID;
	sandRegion = NONE;

	spice = 0.0;

	sprite = pGFXManager->getObjPic(ObjPic_Terrain);

	for(int i=0; i < NUM_ANGLES; i++) {
        tracksCounter[i] = 0;
	}

	location.x = 0;
	location.y = 0;

	destroyedStructureTile = DestroyedStructure_None;
}


TerrainClass::~TerrainClass() {
}

void TerrainClass::load(Stream& stream) {
	type = stream.readUint32();

	for(int i=0;i<MAX_PLAYERS;i++) {
        explored[i] = stream.readBool();
	}

    for(int i=0;i<MAX_PLAYERS;i++) {
        lastAccess[i] = stream.readUint32();
	}

	fogColor = stream.readUint32();

	owner = stream.readSint32();
	sandRegion = stream.readUint32();

	spice = stream.readDouble();

	Uint32 numDamage = stream.readUint32();
	for(Uint32 i=0; i<numDamage; i++) {
        DAMAGETYPE newDamage;
        newDamage.damageType = stream.readUint32();
        newDamage.tile = stream.readSint32();
        newDamage.realPos.x = stream.readSint32();
        newDamage.realPos.y = stream.readSint32();

        damage.push_back(newDamage);
	}

	Uint32 numDeadObjects = stream.readUint32();
	for(Uint32 i=0; i<numDeadObjects; i++) {
        DEADUNITTYPE newDeadUnit;
        newDeadUnit.type = stream.readUint8();
        newDeadUnit.house = stream.readUint8();
        newDeadUnit.onSand = stream.readBool();
        newDeadUnit.realPos.x = stream.readSint32();
        newDeadUnit.realPos.y = stream.readSint32();
        newDeadUnit.timer = stream.readSint16();

        deadUnits.push_back(newDeadUnit);
	}

	destroyedStructureTile = stream.readSint32();

    for(int i=0; i < NUM_ANGLES; i++) {
        tracksCounter[i] = stream.readSint16();
    }

	assignedAirUnitList = stream.readUint32List();
	assignedInfantryList = stream.readUint32List();
	assignedUndergroundUnitList = stream.readUint32List();
	assignedNonInfantryGroundObjectList = stream.readUint32List();
}

void TerrainClass::save(Stream& stream) const {
	stream.writeUint32(type);

    for(int i=0;i<MAX_PLAYERS;i++) {
        stream.writeBool(explored[i]);
	}

    for(int i=0;i<MAX_PLAYERS;i++) {
        stream.writeUint32(lastAccess[i]);
	}

	stream.writeUint32(fogColor);

	stream.writeUint32(owner);
	stream.writeUint32(sandRegion);

	stream.writeDouble(spice);

	stream.writeUint32(damage.size());
    for(std::vector<DAMAGETYPE>::const_iterator iter = damage.begin(); iter != damage.end(); ++iter) {
        stream.writeUint32(iter->damageType);
        stream.writeSint32(iter->tile);
        stream.writeSint32(iter->realPos.x);
        stream.writeSint32(iter->realPos.y);
    }

    stream.writeUint32(deadUnits.size());
    for(std::vector<DEADUNITTYPE>::const_iterator iter = deadUnits.begin(); iter != deadUnits.end(); ++iter) {
        stream.writeUint8(iter->type);
        stream.writeUint8(iter->house);
        stream.writeBool(iter->onSand);
        stream.writeSint32(iter->realPos.x);
        stream.writeSint32(iter->realPos.y);
        stream.writeSint16(iter->timer);
    }

    stream.writeSint32(destroyedStructureTile);

    for(int i=0; i < NUM_ANGLES; i++) {
        stream.writeSint16(tracksCounter[i]);
    }

	stream.writeUint32List(assignedAirUnitList);
	stream.writeUint32List(assignedInfantryList);
	stream.writeUint32List(assignedUndergroundUnitList);
	stream.writeUint32List(assignedNonInfantryGroundObjectList);
}

void TerrainClass::assignAirUnit(Uint32 newObjectID) {
	assignedAirUnitList.push_back(newObjectID);
}

void TerrainClass::assignNonInfantryGroundObject(Uint32 newObjectID) {
	assignedNonInfantryGroundObjectList.push_back(newObjectID);
}

int TerrainClass::assignInfantry(Uint32 newObjectID, Sint8 currentPosition) {
	Sint8 i = currentPosition;

	if(currentPosition == -1) {
		bool used[NUM_INFANTRY_PER_CELL];
		int pos;
		for (i = 0; i < NUM_INFANTRY_PER_CELL; i++)
			used[i] = false;


		std::list<Uint32>::const_iterator iter;
		for(iter = assignedInfantryList.begin(); iter != assignedInfantryList.end() ;++iter) {
			InfantryClass* infant = (InfantryClass*) currentGame->getObjectManager().getObject(*iter);
			if(infant == NULL) {
				continue;
			}

			pos = infant->getCellPosition();
			if ((pos >= 0) && (pos < NUM_INFANTRY_PER_CELL))
				used[pos] = true;
		}

		for (i = 0; i < NUM_INFANTRY_PER_CELL; i++) {
			if (used[i] == false) {
				break;
			}
		}

		if ((i < 0) || (i >= NUM_INFANTRY_PER_CELL))
			i = 0;
	}

	assignedInfantryList.push_back(newObjectID);
	return i;
}


void TerrainClass::assignUndergroundUnit(Uint32 newObjectID) {
	assignedUndergroundUnitList.push_back(newObjectID);
}

void TerrainClass::blitGround(int xPos, int yPos) {
	SDL_Rect	source = { getTerrainTile()*BLOCKSIZE, 0, BLOCKSIZE, BLOCKSIZE };
	SDL_Rect    drawLocation = { xPos, yPos, BLOCKSIZE, BLOCKSIZE };

	if((hasANonInfantryGroundObject() == false) || (getNonInfantryGroundObject()->isAStructure() == false)) {

		//draw terrain
		if(destroyedStructureTile == DestroyedStructure_None || destroyedStructureTile == DestroyedStructure_Wall) {
            SDL_BlitSurface(sprite, &source, screen, &drawLocation);
		}

		if(destroyedStructureTile != DestroyedStructure_None) {
		    SDL_Surface* pDestroyedStructureSurface = pGFXManager->getObjPic(ObjPic_DestroyedStructure);
		    SDL_Rect source2 = { destroyedStructureTile*BLOCKSIZE, 0, BLOCKSIZE, BLOCKSIZE };
            SDL_BlitSurface(pDestroyedStructureSurface, &source2, screen, &drawLocation);
		}

		if(!isFogged(pLocalHouse->getHouseID())) {
		    // tracks
		    for(int i=0;i<NUM_ANGLES;i++) {
                if(tracksCounter[i] > 0) {
                    source.x = ((10-i)%8)*BLOCKSIZE;
                    SDL_BlitSurface(pGFXManager->getObjPic(ObjPic_Terrain_Tracks), &source, screen, &drawLocation);
                }
		    }

            // damage
		    for(std::vector<DAMAGETYPE>::const_iterator iter = damage.begin(); iter != damage.end(); ++iter) {
                source.x = iter->tile*BLOCKSIZE;
                drawLocation.x = screenborder->world2screenX(iter->realPos.x) - BLOCKSIZE/2;
                drawLocation.y = screenborder->world2screenY(iter->realPos.y) - BLOCKSIZE/2;
                if(iter->damageType == Terrain_RockDamage) {
                    SDL_BlitSurface(pGFXManager->getObjPic(ObjPic_RockDamage), &source, screen, &drawLocation);
                } else {
                    SDL_BlitSurface(pGFXManager->getObjPic(ObjPic_SandDamage), &source, screen, &drawLocation);
                }
		    }
		}
	}

}

void TerrainClass::blitStructures(int xPos, int yPos) {
	if (hasANonInfantryGroundObject() && getNonInfantryGroundObject()->isAStructure()) {
		//if got a structure, draw the structure, and dont draw any terrain because wont be seen
		bool	done = false;	//only draw it once
		StructureClass* structure = (StructureClass*) getNonInfantryGroundObject();

		for(int i = structure->getX(); (i < structure->getX() + structure->getStructureSizeX()) && !done;  i++) {
            for(int j = structure->getY(); (j < structure->getY() + structure->getStructureSizeY()) && !done;  j++) {
                if(screenborder->isTileInsideScreen(Coord(i,j))
                    && currentGameMap->cellExists(i, j) && (currentGameMap->getCell(i, j)->isExplored(pLocalHouse->getHouseID()) || debug))
                {

                    structure->setFogged(isFogged(pLocalHouse->getHouseID()));

                    if ((i == location.x) && (j == location.y)) {
                        //only this cell will draw it, so will be drawn only once
                        structure->blitToScreen();
                    }

                    done = true;
                }
            }
        }
	}
}

void TerrainClass::blitUndergroundUnits(int xPos, int yPos) {
	if(hasAnUndergroundUnit() && !isFogged(pLocalHouse->getHouseID())) {
	    UnitClass* current = getUndergroundUnit();

	    if(current->isVisible(pLocalHouse->getTeam())) {
		    if(location == current->getLocation()) {
                current->blitToScreen();
		    }
		}
	}
}

void TerrainClass::blitDeadUnits(int xPos, int yPos) {
	if(!isFogged(pLocalHouse->getHouseID())) {
	    for(std::vector<DEADUNITTYPE>::const_iterator iter = deadUnits.begin(); iter != deadUnits.end(); ++iter) {
	        SDL_Rect source = { 0, 0, BLOCKSIZE, BLOCKSIZE};
	        SDL_Surface* pSurface = NULL;
	        switch(iter->type) {
                case DeadUnit_Infantry: {
                    pSurface = pGFXManager->getObjPic(ObjPic_DeadInfantry, iter->house);
                    source.x = (iter->timer < 1000 && iter->onSand) ? BLOCKSIZE : 0;
                } break;

                case DeadUnit_Infantry_Squashed1: {
                    pSurface = pGFXManager->getObjPic(ObjPic_DeadInfantry, iter->house);
                    source.x = 4 * BLOCKSIZE;
                } break;

                case DeadUnit_Infantry_Squashed2: {
                    pSurface = pGFXManager->getObjPic(ObjPic_DeadInfantry, iter->house);
                    source.x = 5 * BLOCKSIZE;
                } break;

                case DeadUnit_Carrall: {
                    pSurface = pGFXManager->getObjPic(ObjPic_DeadAirUnit, iter->house);
                    if(iter->onSand) {
                        source.x = (iter->timer < 1000) ? 5*BLOCKSIZE : 4*BLOCKSIZE;
                    } else {
                        source.x = 3*BLOCKSIZE;
                    }
                } break;

                case DeadUnit_Ornithopter: {
                    pSurface = pGFXManager->getObjPic(ObjPic_DeadAirUnit, iter->house);
                    if(iter->onSand) {
                        source.x = (iter->timer < 1000) ? 2*BLOCKSIZE : BLOCKSIZE;
                    } else {
                        source.x = 0;
                    }
                } break;

                default: {
                    pSurface = NULL;
                } break;
	        }

	        if(pSurface != NULL) {
                SDL_Rect dest;
                dest.x = screenborder->world2screenX(iter->realPos.x) - BLOCKSIZE/2;
                dest.y = screenborder->world2screenY(iter->realPos.y) - BLOCKSIZE/2;
                dest.w = pSurface->w;
                dest.h = pSurface->h;
                SDL_BlitSurface(pSurface, &source, screen, &dest);
	        }
	    }
	}
}

void TerrainClass::blitInfantry(int xPos, int yPos) {
	if(hasInfantry() && !isFogged(pLocalHouse->getHouseID())) {
		std::list<Uint32>::const_iterator iter;
		for(iter = assignedInfantryList.begin(); iter != assignedInfantryList.end() ;++iter) {
			InfantryClass* current = (InfantryClass*) currentGame->getObjectManager().getObject(*iter);

			if(current == NULL) {
				continue;
			}

			if(current->isVisible(pLocalHouse->getTeam())) {
			    if(location == current->getLocation()) {
                    current->blitToScreen();
			    }
			}
		}
	}
}

void TerrainClass::blitNonInfantryGroundUnits(int xPos, int yPos) {
	if(hasANonInfantryGroundObject() && !isFogged(pLocalHouse->getHouseID())) {
        std::list<Uint32>::const_iterator iter;
		for(iter = assignedNonInfantryGroundObjectList.begin(); iter != assignedNonInfantryGroundObjectList.end() ;++iter) {
			ObjectClass* current =  currentGame->getObjectManager().getObject(*iter);

            if(current->isAUnit() && current->isVisible(pLocalHouse->getTeam())) {
                if(location == current->getLocation()) {
                    current->blitToScreen();
                }
            }
		}
	}
}


void TerrainClass::blitAirUnits(int xPos, int yPos) {
	if(hasAnAirUnit()) {
		std::list<Uint32>::const_iterator iter;
		for(iter = assignedAirUnitList.begin(); iter != assignedAirUnitList.end() ;++iter) {
			AirUnit* airUnit = (AirUnit*) currentGame->getObjectManager().getObject(*iter);

			if(airUnit == NULL) {
				continue;
			}

			if(!isFogged(pLocalHouse->getHouseID()) || airUnit->getOwner() == pLocalHouse) {
                if(airUnit->isVisible(pLocalHouse->getTeam())) {
                    if(location == airUnit->getLocation()) {
                        airUnit->blitToScreen();
                    }
                }
			}
		}
	}
}

void TerrainClass::blitSelectionRects(int xPos, int yPos) {
    // draw underground selection rectangles
    if(hasAnUndergroundUnit() && !isFogged(pLocalHouse->getHouseID())) {
	    UnitClass* current = getUndergroundUnit();

	    if(current->isSelected() && current->isVisible(pLocalHouse->getTeam())) {
		    if(location == current->getLocation()) {
                current->drawSelectionBox();
		    }
		}
	}


    // draw infantry selection rectangles
    if(hasInfantry() && !isFogged(pLocalHouse->getHouseID())) {
		std::list<Uint32>::const_iterator iter;
		for(iter = assignedInfantryList.begin(); iter != assignedInfantryList.end() ;++iter) {
			InfantryClass* current = (InfantryClass*) currentGame->getObjectManager().getObject(*iter);

			if(current == NULL) {
				continue;
			}

			if(current->isSelected() && current->isVisible(pLocalHouse->getTeam())) {
			    if(location == current->getLocation()) {
                    current->drawSelectionBox();
			    }
			}
		}
	}

    // draw non infantry ground object selection rectangles
	if(hasANonInfantryGroundObject() && !isFogged(pLocalHouse->getHouseID())) {
	    std::list<Uint32>::const_iterator iter;
		for(iter = assignedNonInfantryGroundObjectList.begin(); iter != assignedNonInfantryGroundObjectList.end() ;++iter) {
            ObjectClass* current = currentGame->getObjectManager().getObject(*iter);

            if(current->isSelected() && current->isVisible(pLocalHouse->getTeam())) {
                if(location == current->getLocation()) {
                    current->drawSelectionBox();
                }
            }
		}
	}

    // draw air unit selection rectangles
	if(hasAnAirUnit() && !isFogged(pLocalHouse->getHouseID())) {
		std::list<Uint32>::const_iterator iter;
		for(iter = assignedAirUnitList.begin(); iter != assignedAirUnitList.end() ;++iter) {
			AirUnit* airUnit = (AirUnit*) currentGame->getObjectManager().getObject(*iter);

			if(airUnit == NULL) {
				continue;
			}

			if(airUnit->isSelected() && airUnit->isVisible(pLocalHouse->getTeam())) {
			    if(location == airUnit->getLocation()) {
                    airUnit->drawSelectionBox();
			    }
			}
		}
	}
}

void TerrainClass::update() {
    for(int i=0;i<NUM_ANGLES;i++) {
        if(tracksCounter[i] > 0) {
            tracksCounter[i]--;
        }
    }

    for(int i=0 ; i < (int)deadUnits.size() ; i++) {
        if(deadUnits[i].timer == 0) {
            deadUnits.erase(deadUnits.begin()+i);
            i--;
        } else {
            deadUnits[i].timer--;
        }
    }
}

void TerrainClass::clearTerrain() {
    damage.clear();
    deadUnits.clear();
}


void TerrainClass::damageCell(Uint32 damagerID, House* damagerOwner, const Coord& realPos, int bulletType, int bulletDamage, int damageRadius, bool air) {
	TerrainClass* cell;

	if (bulletType == Bullet_Sandworm) {
		ConcatIterator<Uint32> iterator;
		iterator.addList(assignedInfantryList);
		iterator.addList(assignedNonInfantryGroundObjectList);

		ObjectClass* object;
		while(!iterator.IterationFinished()) {

			object = currentGame->getObjectManager().getObject(*iterator);
			if ((object->getX() == location.x) && (object->getY() == location.y)) {
				object->setVisible(VIS_ALL, false);
				object->handleDamage(bulletDamage, damagerID, damagerOwner);
			}
			++iterator;
		}
	} else {
		int			distance;
		double		damageProp;

		if (air == true) {
			// air damage
			if (hasAnAirUnit() && ((bulletType == Bullet_DRocket) || (bulletType == Bullet_Rocket) || (bulletType == Bullet_SmallRocket)))
			{
				AirUnit*	airUnit;

				std::list<Uint32>::const_iterator iter;
				for(iter = assignedAirUnitList.begin(); iter != assignedAirUnitList.end() ;++iter) {
					airUnit = (AirUnit*) currentGame->getObjectManager().getObject(*iter);

					if(airUnit == NULL)
						continue;

					Coord centerPoint = airUnit->getCenterPoint();
					distance = lround(distance_from(centerPoint, realPos));
					if (distance <= 0) {
						distance = 1;
					}

					if ((distance - airUnit->getRadius()) <= damageRadius) {
						if ((bulletType == Bullet_DRocket) && (currentGame->RandomGen.rand(0, 100) <= 5)) {
							((UnitClass*)airUnit)->deviate(damagerOwner);
						}

						damageProp = ((double)(damageRadius + airUnit->getRadius() - distance))/((double)distance);
						if (damageProp > 0)	{
							if (damageProp > 1.0) {
								damageProp = 1.0;
							}
							airUnit->handleDamage(lround((double)bulletDamage * damageProp), damagerID, damagerOwner);
						}
					}
				}
			}
		} else {
			// non air damage
			ConcatIterator<Uint32> iterator;
			iterator.addList(assignedNonInfantryGroundObjectList);
			iterator.addList(assignedInfantryList);
			iterator.addList(assignedUndergroundUnitList);

			ObjectClass* object;
			while(!iterator.IterationFinished()) {

				object = currentGame->getObjectManager().getObject(*iterator);

				Coord centerPoint = object->getClosestCenterPoint(location);
				distance = lround(distance_from(centerPoint, realPos));
				if (distance <= 0) {
					distance = 1;
				}

				if (distance - object->getRadius() <= damageRadius)	{
					if ((bulletType == Bullet_DRocket) && (object->isAUnit()) && (currentGame->RandomGen.rand(0, 100) <= 30)) {
						((UnitClass*)object)->deviate(damagerOwner);
					}

					damageProp = ((double)(damageRadius + object->getRadius() - distance))/(double)distance;
					if (damageProp > 0)	{
						if (damageProp > 1.0) {
							damageProp = 1.0;
						}

						if((bulletType == Bullet_Sonic) && object->isAStructure()) {
						    StructureClass* pStructure = (StructureClass*) object;

                            damageProp = damageProp/(((pStructure->getStructureSizeX()*pStructure->getStructureSizeY())+1)/2);
						}

						object->handleDamage(lround((double) bulletDamage * damageProp), damagerID, damagerOwner);


						if( (bulletType == Bullet_LargeRocket || bulletType == Bullet_Rocket || bulletType == Bullet_SmallRocket || bulletType == Bullet_SmallRocketHeavy)
                            && object->isAStructure() && (object->getHealth() < object->getMaxHealth()/2)) {
                            StructureClass* pStructure = dynamic_cast<StructureClass*>(object);

                            if(pStructure->getNumSmoke() < 5) {
                                pStructure->addSmoke(realPos, currentGame->GetGameCycleCount());
                            }
						}
					}
				}

				++iterator;
			}

			if (currentGameMap->cellExists(realPos.x/BLOCKSIZE, realPos.y/BLOCKSIZE))
			{
				cell = currentGameMap->getCell(realPos.x/BLOCKSIZE, realPos.y/BLOCKSIZE);
				if (((bulletType == Bullet_Rocket) || (bulletType == Bullet_SmallRocket) || (bulletType == Bullet_LargeRocket) || (bulletType == Unit_Devastator))
					&& (!hasAGroundObject() || !getGroundObject()->isAStructure())
					&& ((realPos.x <= (location.x*BLOCKSIZE + BLOCKSIZE/2))//if hasn't been assigned an object or the assigned object isnt a structure
						&& (realPos.y <= (location.y*BLOCKSIZE + BLOCKSIZE/2))))
				{
					if(!cell->hasAGroundObject() || !cell->getGroundObject()->isAStructure()) {
						if(((cell->getType() == Terrain_Rock) && (cell->getTerrainTile() == TerrainTile_RockFull))
							|| (cell->getType() == Terrain_Slab))
						{
							if(cell->getType() == Terrain_Slab) {
								cell->setType(Terrain_Rock);
								cell->setDestroyedStructureTile(Destroyed1x1Structure);
								cell->setOwner(NONE);
							}

							if(damage.size() < DAMAGEPERCELL) {
                                DAMAGETYPE newDamage;
                                newDamage.tile = (bulletType==Bullet_SmallRocket) ? RockDamage1 : RockDamage2;
                                newDamage.damageType = Terrain_RockDamage;
                                newDamage.realPos.x = realPos.x;
                                newDamage.realPos.y = realPos.y;

                                damage.push_back(newDamage);
							}
						} else if((cell->getType() == Terrain_Sand) || (cell->getType() == Terrain_Spice)) {
							if(damage.size() < DAMAGEPERCELL) {
                                DAMAGETYPE newDamage;
                                newDamage.tile = (bulletType==Bullet_SmallRocket) ? currentGame->RandomGen.rand(SandDamage1, SandDamage2) : SandDamage3;
                                newDamage.damageType = Terrain_SandDamage;
                                newDamage.realPos.x = realPos.x;
                                newDamage.realPos.y = realPos.y;

                                damage.push_back(newDamage);
							}
						}
					}
				}
			}
		}
	}
}


void TerrainClass::selectAllPlayersUnits(int houseID, ObjectClass** lastCheckedObject, ObjectClass** lastSelectedObject) {
	ConcatIterator<Uint32> iterator;
	iterator.addList(assignedInfantryList);
	iterator.addList(assignedNonInfantryGroundObjectList);
	iterator.addList(assignedUndergroundUnitList);
	iterator.addList(assignedAirUnitList);

	while(!iterator.IterationFinished()) {
		*lastCheckedObject = currentGame->getObjectManager().getObject(*iterator);
		if (((*lastCheckedObject)->getOwner()->getHouseID() == houseID)
			&& !(*lastCheckedObject)->isSelected()
			&& (*lastCheckedObject)->isAUnit()
			&& ((*lastCheckedObject)->isRespondable()))	{

			(*lastCheckedObject)->setSelected(true);
			currentGame->getSelectedList().insert((*lastCheckedObject)->getObjectID());
			*lastSelectedObject = *lastCheckedObject;
		}
		++iterator;
	}
}


void TerrainClass::selectAllPlayersUnitsOfType(int houseID, int itemID, ObjectClass** lastCheckedObject, ObjectClass** lastSelectedObject) {
	ConcatIterator<Uint32> iterator;
	iterator.addList(assignedInfantryList);
	iterator.addList(assignedNonInfantryGroundObjectList);
	iterator.addList(assignedUndergroundUnitList);
	iterator.addList(assignedAirUnitList);

	while(!iterator.IterationFinished()) {
		*lastCheckedObject = currentGame->getObjectManager().getObject(*iterator);
		if (((*lastCheckedObject)->getOwner()->getHouseID() == houseID)
			&& !(*lastCheckedObject)->isSelected()
			&& ((*lastCheckedObject)->getItemID() == itemID)) {

			(*lastCheckedObject)->setSelected(true);
			currentGame->getSelectedList().insert((*lastCheckedObject)->getObjectID());
			*lastSelectedObject = *lastCheckedObject;
		}
		++iterator;
	}
}


void TerrainClass::unassignAirUnit(Uint32 ObjectID) {
	assignedAirUnitList.remove(ObjectID);
}


void TerrainClass::unassignNonInfantryGroundObject(Uint32 ObjectID) {
	assignedNonInfantryGroundObjectList.remove(ObjectID);
}

void TerrainClass::unassignUndergroundUnit(Uint32 ObjectID) {
	assignedUndergroundUnitList.remove(ObjectID);
}

void TerrainClass::unassignInfantry(Uint32 ObjectID, int currentPosition) {
	assignedInfantryList.remove(ObjectID);
}

void TerrainClass::unassignObject(Uint32 ObjectID) {
	unassignInfantry(ObjectID,-1);
	unassignUndergroundUnit(ObjectID);
	unassignNonInfantryGroundObject(ObjectID);
	unassignAirUnit(ObjectID);
}


void TerrainClass::setType(int newType) {
	type = newType;
	destroyedStructureTile = DestroyedStructure_None;

	if (type == Terrain_Spice) {
		spice = currentGame->RandomGen.rand(RANDOMSPICEMIN, RANDOMSPICEMAX);
	} else if (type == Terrain_ThickSpice) {
		spice = currentGame->RandomGen.rand(RANDOMTHICKSPICEMIN, RANDOMTHICKSPICEMAX);
	} else if (type == Terrain_Dunes) {
	} else {
		spice = 0;
		if (isRock()) {
			sandRegion = NONE;
			if (hasAnUndergroundUnit())	{
				ObjectClass* current;
				std::list<Uint32>::const_iterator iter;
				iter = assignedUndergroundUnitList.begin();

				do {
					current = currentGame->getObjectManager().getObject(*iter);
					++iter;

					if(current == NULL)
						continue;

					unassignUndergroundUnit(current->getObjectID());
					current->destroy();
				} while(iter != assignedUndergroundUnitList.end());
			}

			if(type == Terrain_Mountain) {
				if(hasANonInfantryGroundObject()) {
					ObjectClass* current;
					std::list<Uint32>::const_iterator iter;
					iter = assignedNonInfantryGroundObjectList.begin();

					do {
						current = currentGame->getObjectManager().getObject(*iter);
						++iter;

						if(current == NULL)
							continue;

						unassignNonInfantryGroundObject(current->getObjectID());
						current->destroy();
					} while(iter != assignedNonInfantryGroundObjectList.end());
				}
			}
		}
	}

	for (int i=location.x; i <= location.x+3; i++) {
		for (int j=location.y; j <= location.y+3; j++) {
			if (currentGameMap->cellExists(i, j)) {
				currentGameMap->getCell(i, j)->clearTerrain();
			}
		}
	}
}


void TerrainClass::squash() {
	if(hasInfantry()) {
		InfantryClass* current;
		std::list<Uint32>::const_iterator iter;
		iter = assignedInfantryList.begin();

		do {
			current = (InfantryClass*) currentGame->getObjectManager().getObject(*iter);
			++iter;

			if(current == NULL)
				continue;

			current->squash();
		} while(iter != assignedInfantryList.end());
	}
}


int TerrainClass::getInfantryTeam() {
	int team = NONE;
	if (hasInfantry())
		team = getInfantry()->getOwner()->getTeam();
	return team;
}


double TerrainClass::harvestSpice() {
	double oldSpice = spice;

	if ((spice - HARVESTSPEED) >= 0)
		spice -= HARVESTSPEED;
	else
		spice = 0;

	return (oldSpice - spice);
}


void TerrainClass::setSpice(double newSpice) {
	if(newSpice <= 0) {
		type = Terrain_Sand;
	} else if(newSpice >= RANDOMTHICKSPICEMIN) {
		type = Terrain_ThickSpice;
	} else {
		type = Terrain_Spice;
	}
	spice = newSpice;
}


AirUnit* TerrainClass::getAirUnit() {
	return dynamic_cast<AirUnit*>(currentGame->getObjectManager().getObject(assignedAirUnitList.front()));
}

ObjectClass* TerrainClass::getGroundObject() {
	if (hasANonInfantryGroundObject())
		return getNonInfantryGroundObject();
	else if (hasInfantry())
		return getInfantry();
	else
		return NULL;
}

InfantryClass* TerrainClass::getInfantry() {
	return dynamic_cast<InfantryClass*>(currentGame->getObjectManager().getObject(assignedInfantryList.front()));
}

std::list<Uint32> TerrainClass::getInfantryList() const {
	return assignedInfantryList;
}

ObjectClass* TerrainClass::getNonInfantryGroundObject() {
	return currentGame->getObjectManager().getObject(assignedNonInfantryGroundObjectList.front());
}

UnitClass* TerrainClass::getUndergroundUnit() {
	return dynamic_cast<UnitClass*>(currentGame->getObjectManager().getObject(assignedUndergroundUnitList.front()));
}


/*ObjectClass* TerrainClass::getInfantry(int i)
{
	int count;
	InfantryClass* infantry;
	assignedInfantry.reset();
	while (assignedInfantry.currentNotNull())
	{
		((InfantryClass*)assignedInfantry.getCurrent())->squash();
		assignedInfantry.nextLink();
	}
	return assignedInfantry.removeElement();
}*/


ObjectClass* TerrainClass::getObject() {
	ObjectClass* temp = NULL;
	if (hasAnAirUnit())
		temp = getAirUnit();
	else if (hasANonInfantryGroundObject())
		temp = getNonInfantryGroundObject();
	else if (hasInfantry())
		temp = getInfantry();
	else if (hasAnUndergroundUnit())
		temp = getUndergroundUnit();
	return temp;
}


ObjectClass* TerrainClass::getObjectAt(int x, int y) {
	ObjectClass* temp = NULL;
	if (hasAnAirUnit())
		temp = getAirUnit();
	else if (hasANonInfantryGroundObject())
		temp = getNonInfantryGroundObject();
	else if (hasInfantry())	{
		double closestDistance = NONE;
		Coord atPos, centerPoint;
		InfantryClass* infantry;
		atPos.x = x;
		atPos.y = y;

		std::list<Uint32>::const_iterator iter;
		for(iter = assignedInfantryList.begin(); iter != assignedInfantryList.end() ;++iter) {
			infantry = (InfantryClass*) currentGame->getObjectManager().getObject(*iter);
			if(infantry == NULL)
				continue;

			centerPoint = infantry->getCenterPoint();
			if ((closestDistance == NONE) || (distance_from(atPos, centerPoint) < closestDistance)) {
				closestDistance = distance_from(atPos, centerPoint);
				temp = infantry;
			}
		}
	}
	else if (hasAnUndergroundUnit())
		temp = getUndergroundUnit();

	return temp;
}


ObjectClass* TerrainClass::getObjectWithID(Uint32 objectID) {
	ConcatIterator<Uint32> iterator;
	iterator.addList(assignedInfantryList);
	iterator.addList(assignedNonInfantryGroundObjectList);
	iterator.addList(assignedUndergroundUnitList);
	iterator.addList(assignedAirUnitList);

	while(!iterator.IterationFinished()) {
		if(*iterator == objectID) {
			return currentGame->getObjectManager().getObject(*iterator);
		}
		++iterator;
	}

	return NULL;
}

void TerrainClass::triggerSpiceBloom(House* pTrigger) {
    if (isSpiceBloom()) {
        //a spice bloom
        setType(Terrain_ThickSpice);
        soundPlayer->playSoundAt(Sound_Bloom, getLocation());
        if(pTrigger == pLocalHouse) {
            soundPlayer->playVoice(BloomLocated, pLocalHouse->getHouseID());
        }

        for(int i = -5; i <= 5; i++) {
            for(int j = -5; j <= 5; j++) {
                if(currentGameMap->cellExists(location.x + i, location.y + j)) {
                    TerrainClass* cell = currentGameMap->getCell(location.x + i, location.y + j);

                    if(cell->isSand() && (distance_from(location.x, location.y, location.x + i, location.y + j) <= 5)) {
                        cell->setType(((i==0)&&(j==0)) ? Terrain_ThickSpice : Terrain_Spice);
                    }
                }
            }
        }
    }
}

void TerrainClass::triggerSpecialBloom(House* pTrigger) {
    if(isSpecialBloom()) {
        setType(Terrain_Sand);

        switch(currentGame->RandomGen.rand(0,3)) {
            case 0: {
                // the player gets an randomly choosen amount of credits between 150 and 400
                pTrigger->addCredits(currentGame->RandomGen.rand(150, 400),false);
            } break;

            case 1: {
                // The house gets a Trike for free. It spawns beside the special bloom.
                UnitClass* pNewUnit = pTrigger->createUnit(Unit_Trike);
                if(pNewUnit != NULL) {
                    Coord spot = currentGameMap->findDeploySpot(pNewUnit, location);
                    pNewUnit->deploy(spot);
                }
            } break;

            case 2: {
                // One of the AI players on the map (one that has at least one unit) gets a Trike for free. It spawns beside the special bloom.
                int numCandidates = 0;
                for(int i=0;i<MAX_PLAYERS;i++) {
                    if(currentGame->house[i] != NULL && currentGame->house[i]->getTeam() != pTrigger->getTeam() && currentGame->house[i]->getNumUnits() > 0) {
                        numCandidates++;
                    }
                }

                if(numCandidates == 0) {
                    break;
                }

                House* pEnemyHouse = NULL;
                for(int i=0;i<MAX_PLAYERS;i++) {
                    if(currentGame->house[i] != NULL && currentGame->house[i]->getTeam() != pTrigger->getTeam() && currentGame->house[i]->getNumUnits() > 0) {
                        numCandidates--;
                        if(numCandidates == 0) {
                            pEnemyHouse = currentGame->house[i];
                            break;
                        }
                    }
                }

                UnitClass* pNewUnit = pEnemyHouse->createUnit(Unit_Trike);
                if(pNewUnit != NULL) {
                    Coord spot = currentGameMap->findDeploySpot(pNewUnit, location);
                    pNewUnit->deploy(spot);
                }

            } break;

            case 3:
            default: {
                // One of the AI players on the map (one that has at least one unit) gets an Infantry unit (3 Soldiers) for free. The spawn beside the special bloom.
                int numCandidates = 0;
                for(int i=0;i<MAX_PLAYERS;i++) {
                    if(currentGame->house[i] != NULL && currentGame->house[i]->getTeam() != pTrigger->getTeam() && currentGame->house[i]->getNumUnits() > 0) {
                        numCandidates++;
                    }
                }

                if(numCandidates == 0) {
                    break;
                }

                House* pEnemyHouse = NULL;
                for(int i=0;i<MAX_PLAYERS;i++) {
                    if(currentGame->house[i] != NULL && currentGame->house[i]->getTeam() != pTrigger->getTeam() && currentGame->house[i]->getNumUnits() > 0) {
                        numCandidates--;
                        if(numCandidates == 0) {
                            pEnemyHouse = currentGame->house[i];
                            break;
                        }
                    }
                }

                for(int i=0;i<3;i++) {
                    UnitClass* pNewUnit = pEnemyHouse->createUnit(Unit_Soldier);
                    if(pNewUnit != NULL) {
                        Coord spot = currentGameMap->findDeploySpot(pNewUnit, location);
                        pNewUnit->deploy(spot);
                    }
                }
            } break;
        }
    }
}

bool TerrainClass::isFogged(int houseID) {
	if(debug)
		return false;

	if(currentGame->getGameInitSettings().hasFogOfWar() == false) {
		return false;
	} else if((currentGame->GetGameCycleCount() - lastAccess[houseID]) >= MILLI2CYCLES(10*1000)) {
		return true;
	} else {
		return false;
	}
}

Uint32 TerrainClass::getRadarColor(House* pHouse, bool radar) {
	int color;

	if (isExplored(pHouse->getHouseID()) || debug) {
		if(isFogged(pHouse->getHouseID()) && radar) {
			return fogColor;
		} else {
			ObjectClass* tempObject = NULL;
			if (hasAnObject()) {
				tempObject = getObject();
			}

			if(tempObject != NULL) {
				switch (tempObject->getOwner()->getHouseID()) {
					case HOUSE_ATREIDES:    color = COLOR_ATREIDES;   break;
					case HOUSE_ORDOS:       color = COLOR_ORDOS;      break;
					case HOUSE_HARKONNEN:   color = COLOR_HARKONNEN;  break;
					case HOUSE_SARDAUKAR:   color = COLOR_SARDAUKAR;  break;
					case HOUSE_FREMEN:      color = COLOR_FREMEN;     break;
					case HOUSE_MERCENARY:   color = COLOR_MERCENARY;  break;
					default:                color = COLOR_BLACK;      break;
				}

				if (tempObject->getItemID() == Unit_Sandworm)
					color = COLOR_WHITE;

				// units and structures of the enemy are not visible if no radar
				if(!radar && (tempObject->getOwner()->getTeam() != pHouse->getTeam()))
					color = COLOR_BLACK;
			} else {
				if(!radar && !debug)
					return COLOR_BLACK;

				switch (getType()) {
					case Terrain_Dunes:
						color = COLOR_DESERTSAND;
						break;
					case Terrain_Mountain:
						color = COLOR_MOUNTAIN;
						break;
					case Terrain_Rock:
						color = COLOR_DARKGREY;
						break;
					case Terrain_Sand:
						color = COLOR_DESERTSAND;
						break;
					case Terrain_Spice:
						color = COLOR_SPICE;
						break;
					case Terrain_ThickSpice:
						color = COLOR_THICKSPICE;
						break;

					case Terrain_SpiceBloom:
					case Terrain_SpecialBloom:
						color = COLOR_RED;
						break;

                    case Terrain_Slab:
					default:
						color = COLOR_DARKGREY;
				};
			}
			fogColor = color;
		}
	} else {
		color = COLOR_BLACK;
	}
	return color;
}

int TerrainClass::getTerrainTile() const {
    switch(type) {
        case Terrain_Slab: {
            return TerrainTile_Slab;
        } break;

        case Terrain_Sand: {
            return TerrainTile_Sand;
        } break;

        case Terrain_Rock: {
            //determine which surounding tiles are rock
            bool up = (currentGameMap->cellExists(location.x,location.y-1) == false) || (currentGameMap->getCell(location.x, location.y-1)->isRock() == true);
            bool right = (currentGameMap->cellExists(location.x+1,location.y) == false) || (currentGameMap->getCell(location.x+1, location.y)->isRock() == true);
            bool down = (currentGameMap->cellExists(location.x,location.y+1) == false) || (currentGameMap->getCell(location.x, location.y+1)->isRock() == true);
            bool left = (currentGameMap->cellExists(location.x-1,location.y) == false) || (currentGameMap->getCell(location.x-1, location.y)->isRock() == true);

            return TerrainTile_Rock + (up | (right << 1) | (down << 2) | (left << 3));
        } break;

        case Terrain_Dunes: {
            //determine which surounding tiles are dunes
            bool up = (currentGameMap->cellExists(location.x,location.y-1) == false) || (currentGameMap->getCell(location.x, location.y-1)->getType() == Terrain_Dunes);
            bool right = (currentGameMap->cellExists(location.x+1,location.y) == false) || (currentGameMap->getCell(location.x+1, location.y)->getType() == Terrain_Dunes);
            bool down = (currentGameMap->cellExists(location.x,location.y+1) == false) || (currentGameMap->getCell(location.x, location.y+1)->getType() == Terrain_Dunes);
            bool left = (currentGameMap->cellExists(location.x-1,location.y) == false) || (currentGameMap->getCell(location.x-1, location.y)->getType() == Terrain_Dunes);

            return TerrainTile_Dunes + (up | (right << 1) | (down << 2) | (left << 3));
        } break;

        case Terrain_Mountain: {
            //determine which surounding tiles are mountains
            bool up = (currentGameMap->cellExists(location.x,location.y-1) == false) || (currentGameMap->getCell(location.x, location.y-1)->isMountain() == true);
            bool right = (currentGameMap->cellExists(location.x+1,location.y) == false) || (currentGameMap->getCell(location.x+1, location.y)->isMountain() == true);
            bool down = (currentGameMap->cellExists(location.x,location.y+1) == false) || (currentGameMap->getCell(location.x, location.y+1)->isMountain() == true);
            bool left = (currentGameMap->cellExists(location.x-1,location.y) == false) || (currentGameMap->getCell(location.x-1, location.y)->isMountain() == true);

            return TerrainTile_Mountain + (up | (right << 1) | (down << 2) | (left << 3));
        } break;

        case Terrain_Spice: {
            //determine which surounding tiles are spice
            bool up = (currentGameMap->cellExists(location.x,location.y-1) == false) || (currentGameMap->getCell(location.x, location.y-1)->isSpice() == true);
            bool right = (currentGameMap->cellExists(location.x+1,location.y) == false) || (currentGameMap->getCell(location.x+1, location.y)->isSpice() == true);
            bool down = (currentGameMap->cellExists(location.x,location.y+1) == false) || (currentGameMap->getCell(location.x, location.y+1)->isSpice() == true);
            bool left = (currentGameMap->cellExists(location.x-1,location.y) == false) || (currentGameMap->getCell(location.x-1, location.y)->isSpice() == true);

            return TerrainTile_Spice + (up | (right << 1) | (down << 2) | (left << 3));
        } break;

        case Terrain_ThickSpice: {
            //determine which surounding tiles are thick spice
            bool up = (currentGameMap->cellExists(location.x,location.y-1) == false) || (currentGameMap->getCell(location.x, location.y-1)->getType() == Terrain_ThickSpice);
            bool right = (currentGameMap->cellExists(location.x+1,location.y) == false) || (currentGameMap->getCell(location.x+1, location.y)->getType() == Terrain_ThickSpice);
            bool down = (currentGameMap->cellExists(location.x,location.y+1) == false) || (currentGameMap->getCell(location.x, location.y+1)->getType() == Terrain_ThickSpice);
            bool left = (currentGameMap->cellExists(location.x-1,location.y) == false) || (currentGameMap->getCell(location.x-1, location.y)->getType() == Terrain_ThickSpice);

            return TerrainTile_ThickSpice + (up | (right << 1) | (down << 2) | (left << 3));
        } break;

        case Terrain_SpiceBloom: {
            return TerrainTile_SpiceBloom;
        } break;

        case Terrain_SpecialBloom: {
            return TerrainTile_SpecialBloom;
        } break;

        default: {
            throw std::runtime_error("TerrainClass::getTerrainTile(): Invalid terrain type");
        } break;
    }
}

int TerrainClass::getHideTile(int houseID) const {

    // are all surounding tiles explored?
    if( ((currentGameMap->cellExists(location.x,location.y-1) == false) || (currentGameMap->getCell(location.x, location.y-1)->isExplored(houseID) == true))
        && ((currentGameMap->cellExists(location.x+1,location.y) == false) || (currentGameMap->getCell(location.x+1, location.y)->isExplored(houseID) == true))
        && ((currentGameMap->cellExists(location.x,location.y+1) == false) || (currentGameMap->getCell(location.x, location.y+1)->isExplored(houseID) == true))
        && ((currentGameMap->cellExists(location.x-1,location.y) == false) || (currentGameMap->getCell(location.x-1, location.y)->isExplored(houseID) == true))) {
        return 0;
    }

    //determine what tiles are unexplored
    bool up = (currentGameMap->cellExists(location.x,location.y-1) == false) || (currentGameMap->getCell(location.x, location.y-1)->isExplored(houseID) == false);
    bool right = (currentGameMap->cellExists(location.x+1,location.y) == false) || (currentGameMap->getCell(location.x+1, location.y)->isExplored(houseID) == false);
    bool down = (currentGameMap->cellExists(location.x,location.y+1) == false) || (currentGameMap->getCell(location.x, location.y+1)->isExplored(houseID) == false);
    bool left = (currentGameMap->cellExists(location.x-1,location.y) == false) || (currentGameMap->getCell(location.x-1, location.y)->isExplored(houseID) == false);

    return (up | (right << 1) | (down << 2) | (left << 3));
}

int TerrainClass::getFogTile(int houseID) const {

    // are all surounding tiles fogged?
    if( ((currentGameMap->cellExists(location.x,location.y-1) == false) || (currentGameMap->getCell(location.x, location.y-1)->isFogged(houseID) == false))
        && ((currentGameMap->cellExists(location.x+1,location.y) == false) || (currentGameMap->getCell(location.x+1, location.y)->isFogged(houseID) == false))
        && ((currentGameMap->cellExists(location.x,location.y+1) == false) || (currentGameMap->getCell(location.x, location.y+1)->isFogged(houseID) == false))
        && ((currentGameMap->cellExists(location.x-1,location.y) == false) || (currentGameMap->getCell(location.x-1, location.y)->isFogged(houseID) == false))) {
        return 0;
    }

    //determine what tiles are fogged
    bool up = (currentGameMap->cellExists(location.x,location.y-1) == false) || (currentGameMap->getCell(location.x, location.y-1)->isFogged(houseID) == true);
    bool right = (currentGameMap->cellExists(location.x+1,location.y) == false) || (currentGameMap->getCell(location.x+1, location.y)->isFogged(houseID) == true);
    bool down = (currentGameMap->cellExists(location.x,location.y+1) == false) || (currentGameMap->getCell(location.x, location.y+1)->isFogged(houseID) == true);
    bool left = (currentGameMap->cellExists(location.x-1,location.y) == false) || (currentGameMap->getCell(location.x-1, location.y)->isFogged(houseID) == true);

    return (up | (right << 1) | (down << 2) | (left << 3));
}
