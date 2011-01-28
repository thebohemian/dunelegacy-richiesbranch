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


#include <players/AIPlayer.h>


#include <globals.h>


#include <Game.h>
#include <GameInitSettings.h>
#include <MapClass.h>
#include <sand.h>
#include <House.h>

#include <structures/StructureClass.h>
#include <structures/BuilderClass.h>
#include <structures/StarPortClass.h>
#include <units/UnitClass.h>



AIPlayer::AIPlayer(House* associatedHouse, DIFFICULTYTYPE difficulty) : Player(associatedHouse) {
    AIPlayer::init();

	setDifficulty(difficulty);

	attackTimer = ((2-difficulty) * MILLI2CYCLES(2*60*1000)) + currentGame->RandomGen.rand(MILLI2CYCLES(8*60*1000), MILLI2CYCLES(11*60*1000));
	buildTimer = 0;
}

AIPlayer::AIPlayer(Stream& stream, House* associatedHouse) : Player(associatedHouse) {
    AIPlayer::init();

	setDifficulty((DIFFICULTYTYPE) stream.readUint8());
	attackTimer = stream.readSint32();
	buildTimer = stream.readSint32();

	Uint32 NumPlaceLocations = stream.readUint32();
	for(Uint32 i = 0; i < NumPlaceLocations; i++) {
        Sint32 x = stream.readSint32();
        Sint32 y = stream.readSint32();

		placeLocations.push_back(Coord(x,y));
	}
}

void AIPlayer::init() {
}


AIPlayer::~AIPlayer() {
}

void AIPlayer::save(Stream& stream) const {
    stream.writeUint8(difficulty);
	stream.writeSint32(attackTimer);
    stream.writeSint32(buildTimer);

	stream.writeUint32(placeLocations.size());
	std::list<Coord>::const_iterator iter;
	for(iter = placeLocations.begin(); iter != placeLocations.end(); ++iter) {
		stream.writeSint32(iter->x);
		stream.writeSint32(iter->y);
	}
}



void AIPlayer::update() {
	bool bConstructionYardChecked = false;
	if(buildTimer == 0) {

		RobustList<StructureClass*>::const_iterator iter;
		for(iter = structureList.begin(); iter != structureList.end(); ++iter) {
            StructureClass* tempStructure = *iter;

            //if this players structure, and its a heavy factory, build something
            if(tempStructure->getOwner() == getHouse()) {

                if((tempStructure->IsRepairing() == false) && (tempStructure->getHealth() < tempStructure->getMaxHealth())) {
                    tempStructure->DoRepair();
                }

                BuilderClass* builder = dynamic_cast<BuilderClass*>(tempStructure);
                if(builder != NULL) {

                    if((builder->getHealth() >= builder->getMaxHealth()) && (builder->IsUpgrading() == false) && (builder->CurrentUpgradeLevel() < builder->MaxUpgradeLevel())) {
                        builder->DoUpgrade();
                    }

                    switch (tempStructure->getItemID()) {

                        case Structure_Barracks:
                        case Structure_LightFactory:
                        case Structure_WOR: {
                            if((getHouse()->getCredits() > 100) && (builder->getNumItemsToBuild() < 1) && (builder->getNumSelections() > 0)) {
                                builder->buildRandom();
                            }
                        } break;

                        case Structure_ConstructionYard: {
                            if(bConstructionYardChecked == false) {
                                bConstructionYardChecked = true;
                                if(getHouse()->getCredits() > 100) {
                                    if((builder->getNumItemsToBuild() < 1) && (builder->getNumSelections() > 0)) {
                                        Uint32 itemID = NONE;
                                        if(getHouse()->getProducedPower() < 50 && builder->isAvailableToBuild(Structure_WindTrap)) {
                                            itemID = Structure_WindTrap;
                                        } else if(getHouse()->getNumItems(Structure_Refinery) < 2 && builder->isAvailableToBuild(Structure_Refinery)) {
                                            itemID = Structure_Refinery;
                                        } else if(getHouse()->getProducedPower() < 150 && builder->isAvailableToBuild(Structure_WindTrap)) {
                                            itemID = Structure_WindTrap;
                                        } else if((getHouse()->hasRadar() == false) && (currentGame->techLevel >= 2) && builder->isAvailableToBuild(Structure_Radar)) {
                                            itemID = Structure_Radar;
                                        } else if((getHouse()->getNumItems(Structure_WOR) <= 0) && (currentGame->techLevel >= 2) && builder->isAvailableToBuild(Structure_WOR)) {
                                            itemID = Structure_WOR;
                                        } else if((getHouse()->getNumItems(Structure_RocketTurret) < 2) && (currentGame->techLevel >= 6) && builder->isAvailableToBuild(Structure_RocketTurret)) {
                                            itemID = Structure_RocketTurret;
                                        } else if(getHouse()->getProducedPower() < 250 && builder->isAvailableToBuild(Structure_WindTrap)) {
                                            itemID = Structure_WindTrap;
                                        } else if((getHouse()->hasLightFactory() == false) && (currentGame->techLevel >= 3) && builder->isAvailableToBuild(Structure_LightFactory)) {
                                            itemID = Structure_LightFactory;
                                        } else if(getHouse()->getNumItems(Structure_Refinery) < 3 && builder->isAvailableToBuild(Structure_Refinery)) {
                                            itemID = Structure_Refinery;
                                        } else if((getHouse()->getNumItems(Structure_RocketTurret) < 4) && (currentGame->techLevel >= 6) && builder->isAvailableToBuild(Structure_RocketTurret)) {
                                            itemID = Structure_RocketTurret;
                                        } else if((getHouse()->getNumItems(Structure_HeavyFactory) <= 0) && (currentGame->techLevel >= 4) && builder->isAvailableToBuild(Structure_HeavyFactory)) {
                                            itemID = Structure_HeavyFactory;
                                        } else if(getHouse()->getProducedPower() < 350 && builder->isAvailableToBuild(Structure_WindTrap)) {
                                            itemID = Structure_WindTrap;
                                        } else if((getHouse()->getNumItems(Structure_HighTechFactory) <= 0) && (currentGame->techLevel >= 5) && builder->isAvailableToBuild(Structure_HighTechFactory)) {
                                            itemID = Structure_HighTechFactory;
                                        } else if((getHouse()->getNumItems(Structure_RocketTurret) < 6) && (currentGame->techLevel >= 6) && builder->isAvailableToBuild(Structure_RocketTurret)) {
                                            itemID = Structure_RocketTurret;
                                        } else if((getHouse()->getNumItems(Structure_RepairYard) <= 0) && (currentGame->techLevel >= 5) && builder->isAvailableToBuild(Structure_RepairYard)) {
                                            itemID = Structure_RepairYard;
                                        } else if(getHouse()->getProducedPower() < 450 && builder->isAvailableToBuild(Structure_WindTrap)) {
                                            itemID = Structure_WindTrap;
                                        } else if((getHouse()->getNumItems(Structure_StarPort) <= 0) && (currentGame->techLevel >= 6) && builder->isAvailableToBuild(Structure_StarPort)) {
                                            itemID = Structure_StarPort;
                                        } else if((getHouse()->getNumItems(Structure_IX) <= 0) && (currentGame->techLevel >= 7) && builder->isAvailableToBuild(Structure_IX)) {
                                            itemID = Structure_IX;
                                        } else if((getHouse()->getNumItems(Structure_Palace) <= 0) && (currentGame->techLevel >= 8) && builder->isAvailableToBuild(Structure_Palace)) {
                                            itemID = Structure_Palace;
                                        } else if((getHouse()->getNumItems(Structure_RocketTurret) < 10) && (currentGame->techLevel >= 6) && builder->isAvailableToBuild(Structure_RocketTurret)) {
                                            itemID = Structure_RocketTurret;
                                        } else if(getHouse()->getProducedPower() < 550 && builder->isAvailableToBuild(Structure_WindTrap)) {
                                            itemID = Structure_WindTrap;
                                        } else if((getHouse()->getNumItems(Structure_WOR) < 2) && (currentGame->techLevel >= 2) && builder->isAvailableToBuild(Structure_WOR)) {
                                            itemID = Structure_WOR;
                                        } else if((getHouse()->getNumItems(Structure_LightFactory) < 2) && (currentGame->techLevel >= 3) && builder->isAvailableToBuild(Structure_LightFactory)) {
                                            itemID = Structure_LightFactory;
                                        } else if((getHouse()->getNumItems(Structure_HeavyFactory) < 2) && (currentGame->techLevel >= 4) && builder->isAvailableToBuild(Structure_HeavyFactory)) {
                                            itemID = Structure_HeavyFactory;
                                        } else if((getHouse()->getNumItems(Structure_Palace) < 2) && (currentGame->techLevel >= 8) && builder->isAvailableToBuild(Structure_Palace)) {
                                            itemID = Structure_Palace;
                                        } else if(getHouse()->getProducedPower() < 650 && builder->isAvailableToBuild(Structure_WindTrap)) {
                                            itemID = Structure_WindTrap;
                                        } else if((getHouse()->getNumItems(Structure_RocketTurret) < 20) && (currentGame->techLevel >= 6) && builder->isAvailableToBuild(Structure_RocketTurret)) {
                                            itemID = Structure_RocketTurret;
                                        }

                                        if(itemID != NONE) {
                                            Coord	placeLocation, location;
                                            location = findPlaceLocation(itemID);

                                            if(location.x >= 0) {
                                                placeLocation = location;
                                                if(currentGame->getGameInitSettings().isConcreteRequired()) {
                                                    int i, j,
                                                        incI, incJ,
                                                        startI, startJ;

                                                    if(currentGameMap->isWithinBuildRange(location.x, location.y, getHouse())) {
                                                        startI = location.x, startJ = location.y, incI = 1, incJ = 1;
                                                    } else if(currentGameMap->isWithinBuildRange(location.x + getStructureSize(itemID).x - 1, location.y, getHouse())) {
                                                        startI = location.x + getStructureSize(itemID).x - 1, startJ = location.y, incI = -1, incJ = 1;
                                                    } else if(currentGameMap->isWithinBuildRange(location.x, location.y + getStructureSize(itemID).y - 1, getHouse())) {
                                                        startI = location.x, startJ = location.y + getStructureSize(itemID).y - 1, incI = 1, incJ = -1;
                                                    } else {
                                                        startI = location.x + getStructureSize(itemID).x - 1, startJ = location.y + getStructureSize(itemID).y - 1, incI = -1, incJ = -1;
                                                    }

                                                    for(i = startI; abs(i - startI) < getStructureSize(itemID).x; i += incI) {
                                                        for(j = startJ; abs(j - startJ) < getStructureSize(itemID).y; j += incJ) {
                                                            TerrainClass *tmpCell = currentGameMap->getCell(i, j);

                                                            if(tmpCell == NULL) {
                                                                fprintf(stderr,"%s - Line %d:getCell(%d,%d) returned NULL\n", __FILE__, __LINE__,i,j);
                                                                fflush(stderr);
                                                            }

                                                            if(tmpCell->getType() != Terrain_Slab) {
                                                                placeLocations.push_back(Coord(i,j));
                                                                builder->DoProduceItem(Structure_Slab1);
                                                            }
                                                        }
                                                    }
                                                }

                                                placeLocations.push_back(placeLocation);
                                                builder->DoProduceItem(itemID);
                                            }
                                        }
                                    }
                                }
                            }

                            if(builder->IsWaitingToPlace()) {
                                //find total region of possible placement and place in random ok position
                                int ItemID = builder->GetCurrentProducedItem();
                                Coord itemsize = getStructureSize(ItemID);

                                //see if there is already a spot to put it stored
                                if(!placeLocations.empty()) {
                                    Coord location = placeLocations.front();

                                    if(currentGameMap->okayToPlaceStructure(location.x, location.y, itemsize.x, itemsize.y, false, builder->getOwner())) {
                                        builder->getOwner()->placeStructure(builder->getObjectID(), ItemID, location.x, location.y);
                                        placeLocations.pop_front();
                                    } else if(ItemID == Structure_Slab1) {
                                        builder->DoCancelItem(Structure_Slab1);	//forget about concrete
                                        placeLocations.pop_front();
                                    } else {
                                        //cancel item
                                        builder->DoCancelItem(ItemID);
                                        placeLocations.pop_front();
                                    }
                                }
                            }
                        } break;

                        case Structure_HeavyFactory: {
                            if((getHouse()->getCredits() > 100) && (builder->getNumItemsToBuild() < 1) && (builder->getNumSelections() > 0)) {

                                if(getHouse()->getNumItems(Unit_Harvester) < 2*getHouse()->getNumItems(Structure_Refinery)) {
                                    builder->DoProduceItem(Unit_Harvester);
                                } else {
                                    builder->buildRandom();
                                }
                            }
                        } break;

                        case Structure_HighTechFactory: {
                            if((getHouse()->getCredits() > 100) && (builder->getNumItemsToBuild() < 1)) {

                                if(getHouse()->getNumItems(Unit_Carryall) < getHouse()->getNumItems(Unit_Harvester)) {
                                    builder->DoProduceItem(Unit_Carryall);
                                } else {
                                    builder->DoProduceItem(Unit_Ornithopter);
                                }
                            }
                        } break;

                        case Structure_StarPort: {
                            if(((StarPortClass*)builder)->okToOrder())	{
                                // order max 6 units
                                int num = 6;
                                while((num > 0) && (getHouse()->getCredits() > 2000)) {
                                    builder->buildRandom();
                                    num--;
                                }
                                ((StarPortClass*)builder)->DoPlaceOrder();
                            }
                        } break;

                        default: {
                            break;
                        }
                    }
                }
            }

		}

		buildTimer = ((2-difficulty) * MILLI2CYCLES(1000)) + currentGame->RandomGen.rand(300, 400) * (  getHouse()->getNumItems(Structure_HeavyFactory)
                                                                                                          + getHouse()->getNumItems(Structure_LightFactory)
                                                                                                          + getHouse()->getNumItems(Structure_WOR) );
	}


	if(attackTimer > 0) {
	    attackTimer--;
	} else {
        double speedCap = -1.0;

	    RobustList<UnitClass*>::const_iterator iter;
	    for(iter = unitList.begin(); iter != unitList.end(); ++iter) {
            UnitClass *tempUnit = *iter;

            if (tempUnit->isRespondable()
                && (tempUnit->getOwner() == getHouse())
                && !tempUnit->isAttacking()
                && (tempUnit->getAttackMode() == AREAGUARD)
                && (tempUnit->getItemID() != Unit_Harvester)
                && (tempUnit->getItemID() != Unit_MCV)
                && (tempUnit->getItemID() != Unit_Carryall)
                && (tempUnit->getItemID() != Unit_Saboteur)) {
                //find slowest speed of all units
                if ((speedCap < 0.0) || (tempUnit->getMaxSpeed() < speedCap))
                    speedCap = tempUnit->getMaxSpeed();
            }
        }

        Coord destination;
        UnitClass* leader = NULL;
	    for(iter = unitList.begin(); iter != unitList.end(); ++iter) {
            UnitClass *tempUnit = *iter;
            if (tempUnit->isRespondable()
                && (tempUnit->getOwner() == getHouse())
                && tempUnit->isActive()
                && !tempUnit->isAttacking()
                && (tempUnit->getAttackMode() == AREAGUARD)
                && (tempUnit->getItemID() != Unit_Harvester)
                && (tempUnit->getItemID() != Unit_MCV)
                && (tempUnit->getItemID() != Unit_Carryall)
                && (tempUnit->getItemID() != Unit_Saboteur)) {

                if(leader == NULL) {
                    leader = tempUnit;

                    //default destination
                    destination.x = leader->getX();
                    destination.y = leader->getY();

                    StructureClass* closestStructure = ObjectClass::findClosestTargetStructure(leader);
                    if(closestStructure) {
                        destination = closestStructure->getClosestPoint(leader->getLocation());
                    } else {
                        UnitClass* closestUnit = ObjectClass::findClosestTargetUnit(leader);
                        if(closestUnit) {
                            destination.x = closestUnit->getX();
                            destination.y = closestUnit->getY();
                        }
                    }
                }

                tempUnit->setSpeedCap(speedCap);
                tempUnit->DoMove2Pos(destination, false);
                tempUnit->setAttacking(true);
            }
        }

		//reset timer for next attack
		attackTimer = currentGame->RandomGen.rand(10000, 20000);
	}

	if(buildTimer > 0) {
		buildTimer--;
	}
}

void AIPlayer::onIncrementStructures(int itemID) {
}

void AIPlayer::onDecrementStructures(int itemID, const Coord& location) {
	/* // no good idea to rebuild everything
	//rebuild the structure if its the original gameType

	if (((currentGame->gameType == GAMETYPE_CAMPAIGN) || (currentGame->gameType == GAMETYPE_SKIRMISH)) && !structureList.empty()) {
		RobustList<StructureClass*>::const_iterator iter;
		for(iter = structureList.begin(); iter != structureList.end(); ++iter) {
			StructureClass* structure = *iter;
			if ((structure->getItemID() == Structure_ConstructionYard) && (structure->getOwner() == this)) {
				Coord *newLocation;

				if (currentGame->getGameInitSettings().isConcreteRequired()) {
					int i, j,
						incI, incJ,
						startI, startJ;

					if (currentGameMap->isWithinBuildRange(location->x, location->y, this))
						startI = location->x, startJ = location->y, incI = 1, incJ = 1;
					else if (currentGameMap->isWithinBuildRange(location->x + getStructureSize(itemID).x - 1, location->y, this))
						startI = location->x + getStructureSize(itemID).x - 1, startJ = location->y, incI = -1, incJ = 1;
					else if (currentGameMap->isWithinBuildRange(location->x, location->y + getStructureSize(itemID).y - 1, this))
						startI = location->x, startJ = location->y + getStructureSize(itemID).y - 1, incI = 1, incJ = -1;
					else
						startI = location->x + getStructureSize(itemID).x - 1, startJ = location->y + getStructureSize(itemID).y - 1, incI = -1, incJ = -1;

					for (i = startI; abs(i - startI) < getStructureSize(itemID).x; i += incI) {
						for (j = startJ; abs(j - startJ) < getStructureSize(itemID).y; j += incJ) {
							TerrainClass* tmpCell = currentGameMap->getCell(i, j);

							if(tmpCell == NULL) {
								fprintf(stderr,"%s - Line %d:getCell(%d,%d) returned NULL\n", __FILE__, __LINE__,i,j);
								fflush(stderr);
							}

							if(tmpCell->getType() != Terrain_Slab) {
								placeLocations.push_back(Coord(i,j));
								((ConstructionYardClass*)structure)->ProduceItem(Structure_Slab1);
							}
						}
					}
				}

				((ConstructionYardClass*)structure)->ProduceItem(itemID);
				placeLocations.push_back(*location);
				break;
			}
		}
	}*/
}

void AIPlayer::onDamage(ObjectClass* pObject, const Coord& location) {
    if(currentGame->getGameInitSettings().getMission() == 1) {
        // make the AI more stupid in the first mission
        return;
    }

	int itemID = pObject->getItemID();

	if((itemID == Unit_Harvester) || isStructure(itemID)) {
	    //scramble some free units to defend

	    RobustList<UnitClass*>::const_iterator iter;
	    for(iter = unitList.begin(); iter != unitList.end(); ++iter) {
		    UnitClass* tempUnit = *iter;
			if (tempUnit->isRespondable()
				&& (tempUnit->getOwner() == getHouse())
				&& !tempUnit->isAttacking()
				&& (tempUnit->getItemID() != Unit_Harvester)
				&& (tempUnit->getItemID() != Unit_MCV)
				&& (tempUnit->getItemID() != Unit_Carryall)
				&& (tempUnit->getItemID() != Unit_Frigate)
				&& (tempUnit->getItemID() != Unit_Saboteur)
				&& (tempUnit->getItemID() != Unit_Sandworm)) {

					tempUnit->DoMove2Pos(location, false);
					//tempUnit->setAttacking(true);
			}
		}

	} else if(pObject->isAUnit() && ((UnitClass*)pObject)->isAttacking()) {
	    //if one of attack force is shot, unrestrict the speeds of others as to catch up

	    RobustList<UnitClass*>::const_iterator iter;
	    for(iter = unitList.begin(); iter != unitList.end(); ++iter) {
			UnitClass* tempUnit = *iter;
			if (tempUnit->isRespondable()
				&& (tempUnit->getOwner() == getHouse())
				&& tempUnit->isAttacking()
				&& (tempUnit->getItemID() != Unit_Harvester)
				&& (tempUnit->getItemID() != Unit_MCV)
				&& (tempUnit->getItemID() != Unit_Carryall)
				&& (tempUnit->getItemID() != Unit_Saboteur)) {
				tempUnit->setSpeedCap(NONE);
			}
		}
	}

}




void AIPlayer::setDifficulty(DIFFICULTYTYPE newDifficulty) {
	if((newDifficulty >= EASY) && (newDifficulty <= HARD)) {
		difficulty = newDifficulty;
	}
}

void AIPlayer::setAttackTimer(int newAttackTimer) {
	if (newAttackTimer >= 0)
		attackTimer = newAttackTimer;
}

Coord AIPlayer::findPlaceLocation(Uint32 itemID) {
    int structureSizeX = getStructureSize(itemID).x;
    int structureSizeY = getStructureSize(itemID).y;

	Coord bestLocation(INVALID_POS,INVALID_POS);
	int	count;
	int minX = currentGameMap->sizeX;
	int maxX = -1;
    int minY = currentGameMap->sizeY;
    int maxY = -1;

    RobustList<StructureClass*>::const_iterator iter;
    for(iter = structureList.begin(); iter != structureList.end(); ++iter) {
		StructureClass* structure = *iter;
		if (structure->getOwner() == getHouse()) {
			if (structure->getX() < minX)
				minX = structure->getX();
			if (structure->getX() > maxX)
				maxX = structure->getX();
			if (structure->getY() < minY)
				minY = structure->getY();
			if (structure->getY() > maxY)
				maxY = structure->getY();
		}
	}

	minX -= structureSizeX + 1;
	maxX += 2;
	minY -= structureSizeY + 1;
	maxY += 2;
	if (minX < 0) minX = 0;
	if (maxX >= currentGameMap->sizeX) maxX = currentGameMap->sizeX - structureSizeX;
	if (minY < 0) minY = 0;
	if (maxY >= currentGameMap->sizeY) maxY = currentGameMap->sizeY - structureSizeY;

    double bestrating = 0.0;
	count = 0;
	do {
	    Coord pos = Coord(currentGame->RandomGen.rand(minX, maxX), currentGame->RandomGen.rand(minY, maxY));
		count++;

		if(currentGameMap->okayToPlaceStructure(pos.x, pos.y, structureSizeX, structureSizeY, false, getHouse())) {
            double rating;

		    switch(itemID) {
                case Structure_Refinery: {
                    // place near spice
                    Coord spicePos;
                    if(currentGameMap->findSpice(&spicePos, &pos)) {
                        rating = 10000000 - blockDistance(pos, spicePos);
                    } else {
                        rating = 10000000;
                    }
                } break;

                case Structure_Barracks:
                case Structure_ConstructionYard:
                case Structure_HeavyFactory:
                case Structure_LightFactory:
                case Structure_RepairYard:
                case Structure_StarPort:
                case Structure_WOR: {
                    // place near sand

                    double nearestSand = 10000000;

                    for(int y = 0 ; y < currentGameMap->sizeY; y++) {
                        for(int x = 0; x < currentGameMap->sizeX; x++) {
                            int type = currentGameMap->getCell(x,y)->getType();

                            if(type != Terrain_Rock || type != Terrain_Slab || type != Terrain_Mountain) {
                                double tmp = blockDistance(pos, Coord(x,y));
                                if(tmp < nearestSand) {
                                    nearestSand = tmp;
                                }
                            }
                        }
                    }

                    rating = 10000000 - nearestSand;
                } break;

                case Structure_Wall:
                case Structure_GunTurret:
                case Structure_RocketTurret: {
                    // place towards enemy
                    double nearestEnemy = 10000000;

                    RobustList<StructureClass*>::const_iterator iter2;
                    for(iter2 = structureList.begin(); iter2 != structureList.end(); ++iter2) {
                        StructureClass* pStructure = *iter2;
                        if(pStructure->getOwner()->getTeam() != getHouse()->getTeam()) {

                            double tmp = blockDistance(pos, pStructure->getLocation());
                            if(tmp < nearestEnemy) {
                                nearestEnemy = tmp;
                            }
                        }
                    }

                    rating = 10000000 - nearestEnemy;
                } break;

                case Structure_HighTechFactory:
                case Structure_IX:
                case Structure_Palace:
                case Structure_Radar:
                case Structure_Silo:
                case Structure_WindTrap:
                default: {
                    // place at a save place
                    double nearestEnemy = 10000000;

                    RobustList<StructureClass*>::const_iterator iter2;
                    for(iter2 = structureList.begin(); iter2 != structureList.end(); ++iter2) {
                        StructureClass* pStructure = *iter2;
                        if(pStructure->getOwner()->getTeam() != getHouse()->getTeam()) {

                            double tmp = blockDistance(pos, pStructure->getLocation());
                            if(tmp < nearestEnemy) {
                                nearestEnemy = tmp;
                            }
                        }
                    }

                    rating = nearestEnemy;
                } break;
		    }

		    if(rating > bestrating) {
                bestLocation = pos;
                bestrating = rating;
		    }
		}

	} while(count <= 100);

	return bestLocation;
}
