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

#include <House.h>

#include <globals.h>
#include <sand.h>

#include <FileClasses/TextManager.h>

#include <players/AIPlayer.h>
#include <players/HumanPlayer.h>

#include <Game.h>
#include <MapClass.h>
#include <RadarView.h>
#include <SoundPlayer.h>

#include <structures/StructureClass.h>
#include <structures/BuilderClass.h>
#include <structures/RefineryClass.h>
#include <structures/ConstructionYardClass.h>
#include <units/Carryall.h>
#include <units/HarvesterClass.h>

#include <algorithm>


House::House(int newHouse, int newCredits, Uint8 team, int quota) : choam(this)
{
    House::init();

    houseID = ((newHouse >= 0) && (newHouse < NUM_HOUSES)) ? newHouse :  0;
    this->team = team;

	storedCredits = 0.0;
    startingCredits = newCredits;
    oldCredits = lround(storedCredits+startingCredits);

    this->quota = quota;

    destroyedValue = 0;
    numDestroyedUnits = 0;
    numDestroyedStructures = 0;
    harvestedSpice = 0.0;
    producedPower = 0;
    powerUsageTimer = 0;
}




House::House(Stream& stream) : choam(this)
{
    House::init();

    houseID = stream.readUint8();
	team = stream.readUint8();

	storedCredits = stream.readDouble();
	startingCredits = stream.readDouble();
    oldCredits = lround(storedCredits+startingCredits);
    quota = stream.readSint32();

    destroyedValue = stream.readUint32();
    numDestroyedUnits = stream.readUint32();
    numDestroyedStructures = stream.readUint32();
    harvestedSpice = stream.readDouble();
    producedPower = stream.readSint32();
    powerUsageTimer = stream.readSint32();

    choam.load(stream);

	if(stream.readBool() == true) {
		pLocalHouse = this;
	}

	Uint32 numPlayers = stream.readUint32();
	for(Uint32 i = 0; i < numPlayers; i++) {
        bool bAI = stream.readBool();
        if(bAI == true) {
            addPlayer(std::shared_ptr<Player>(new AIPlayer(stream, this)));
        } else {
            addPlayer(std::shared_ptr<Player>(new HumanPlayer(stream, this)));
        }
	}
}




void House::init()
{
    ai = false;

    numUnits = 0;
	numStructures = 0;
	for(int i=0;i<ItemID_max;i++) {
        numItem[i] = 0;
	}

	capacity = 0;
	powerRequirement = 0;
}

House::~House()
{
}




void House::save(Stream& stream) const
{
	stream.writeUint8(houseID);
	stream.writeUint8(team);

	stream.writeDouble(storedCredits);
	stream.writeDouble(startingCredits);
	stream.writeSint32(quota);

    stream.writeUint32(destroyedValue);
    stream.writeUint32(numDestroyedUnits);
    stream.writeUint32(numDestroyedStructures);
    stream.writeDouble(harvestedSpice);
    stream.writeSint32(producedPower);
    stream.writeSint32(powerUsageTimer);

    choam.save(stream);

	stream.writeBool(this == pLocalHouse);

	stream.writeUint32(players.size());
    std::list<std::shared_ptr<Player> >::const_iterator iter;
    for(iter = players.begin(); iter != players.end(); ++iter) {
        if(dynamic_cast<HumanPlayer*>(iter->get()) != NULL) {
            stream.writeBool(false);
        } else {
            stream.writeBool(true);
        }

        (*iter)->save(stream);
    }
}




void House::addPlayer(std::shared_ptr<Player> newPlayer) {
    if(dynamic_cast<HumanPlayer*>(newPlayer.get()) == NULL) {
        ai = true;
    }
    players.push_back(newPlayer);
}




void House::setProducedPower(int newPower)
{
	// calculate current power production
    bool bhasPowerBefore = hasPower();
	producedPower = newPower;
	bool bhasPowerAfterwards = hasPower();

    if(hasRadar() && (bhasPowerBefore != bhasPowerAfterwards)) {
        changeRadar(bhasPowerAfterwards);
	}
}




void House::addCredits(double newCredits, bool wasRefined)
{
	if (newCredits > 0.0) {
	    if(wasRefined == true) {
                harvestedSpice += newCredits;
	    }

		storedCredits += newCredits;
		if (this == pLocalHouse)	{
			if(((currentGame->winFlags & WINLOSEFLAGS_QUOTA) != 0) && (quota != 0))	{
				if(storedCredits >= quota) {
					win();
				}
			}
		}
	}
}




void House::returnCredits(double newCredits)
{
	if(newCredits > 0.0) {
		startingCredits += newCredits;
	}
}




double House::takeCredits(double amount)
{
	double	taken = 0.0;

	if(getCredits() >= 1) {
		if(storedCredits > amount) {
			taken = amount;
			storedCredits -= amount;
		} else {
			taken = storedCredits;
			storedCredits = 0.0;

			if(startingCredits > (amount - taken)) {
				startingCredits -= (amount - taken);
				taken = amount;
			} else {
				taken += startingCredits;
				startingCredits = 0.0;
			}
		}
	}

	return taken;	//the amount that was actually withdrawn
}




void House::printStat() const {
	fprintf(stderr,"House %s: (Number of Units: %d, Number of Structures: %d)\n",getHouseNameByNumber( (HOUSETYPE) getHouseID()).c_str(),numUnits,numStructures);
	fprintf(stderr,"Barracks: %d\t\tWORs: %d\n", numItem[Structure_Barracks],numItem[Structure_WOR]);
	fprintf(stderr,"Light Factories: %d\tHeavy Factories: %d\n",numItem[Structure_LightFactory],numItem[Structure_HeavyFactory]);
	fprintf(stderr,"IXs: %d\t\t\tPalaces: %d\n",numItem[Structure_IX],numItem[Structure_Palace]);
	fprintf(stderr,"Repair Yards: %d\t\tHigh-Tech Factories: %d\n",numItem[Structure_RepairYard],numItem[Structure_HighTechFactory]);
	fprintf(stderr,"Refineries: %d\t\tStarports: %d\n",numItem[Structure_Refinery],numItem[Structure_StarPort]);
	fprintf(stderr,"Walls: %d\t\tRocket Turrets: %d\n",numItem[Structure_Wall],numItem[Structure_RocketTurret]);
	fprintf(stderr,"Gun Turrets: %d\t\tConstruction Yards: %d\n",numItem[Structure_GunTurret],numItem[Structure_ConstructionYard]);
	fprintf(stderr,"Windtraps: %d\t\tRadars: %d\n",numItem[Structure_WindTrap],numItem[Structure_Radar]);
	fprintf(stderr,"Silos: %d\n",numItem[Structure_Silo]);
	fprintf(stderr,"Carryalls: %d\t\tFrigates: %d\n",numItem[Unit_Carryall],numItem[Unit_Frigate]);
	fprintf(stderr,"Devastators: %d\t\tDeviators: %d\n",numItem[Unit_Devastator],numItem[Unit_Deviator]);
	fprintf(stderr,"Soldiers: %d\t\tFremen: %d\n",numItem[Unit_Soldier],numItem[Unit_Fremen]);
	fprintf(stderr,"Trooper: %d\t\tDeviators: %d\n",numItem[Unit_Trooper],numItem[Unit_Sardaukar]);
	fprintf(stderr,"Saboteur: %d\t\tSandworms: %d\n",numItem[Unit_Saboteur],numItem[Unit_Sandworm]);
	fprintf(stderr,"Quads: %d\t\tTrikes: %d\n",numItem[Unit_Quad],numItem[Unit_Trike]);
	fprintf(stderr,"Raiders: %d\t\tTanks: %d\n",numItem[Unit_Raider],numItem[Unit_Tank]);
	fprintf(stderr,"Siege Tanks : %d\t\tSonic Tanks: %d\n",numItem[Unit_SiegeTank],numItem[Unit_SonicTank]);
	fprintf(stderr,"Harvesters: %d\t\tMCVs: %d\n",numItem[Unit_Harvester],numItem[Unit_MCV]);
	fprintf(stderr,"Ornithopters: %d\t\tRocket Launchers: %d\n",numItem[Unit_Ornithopter],numItem[Unit_Launcher]);
}




void House::checkSelectionLists()
{
    RobustList<StructureClass*>::const_iterator iter;
    for(iter = structureList.begin(); iter != structureList.end(); ++iter) {
		StructureClass* tempStructure = *iter;
        if(tempStructure->isABuilder() && (tempStructure->getOwner() == this)) {
			((BuilderClass*) tempStructure)->checkSelectionList();
        }
    }
}




void House::update()
{
	if (oldCredits != getCredits()) {
		if((this == pLocalHouse) && (getCredits() > 0)) {
			soundPlayer->playSound(CreditsTick);
		}
		oldCredits = getCredits();
	}

	if(storedCredits > capacity) {
		storedCredits--;
		if(storedCredits < 0.0) {
		 storedCredits = 0.0;
		}

		if(this == pLocalHouse) {
			currentGame->AddToNewsTicker(pTextManager->getDuneText(DuneText_SpiceLost));
		}
	}

	powerUsageTimer--;
	if(powerUsageTimer <= 0) {
	    powerUsageTimer = MILLI2CYCLES(15*1000);
        takeCredits(((double)getPowerRequirement()) / 32.0);
	}

	choam.update();

    std::list<std::shared_ptr<Player> >::iterator iter;
    for(iter = players.begin(); iter != players.end(); ++iter) {
        (*iter)->update();
    }
}




void House::incrementUnits(int itemID)
{
    numUnits++;
    numItem[itemID]++;
}




void House::decrementUnits(int itemID)
{
	numUnits--;

	if(itemID == Unit_Harvester) {
        decrementHarvesters();
	} else {
        numItem[itemID]--;
	}

	if (!isAlive())
		lose();

}




void House::incrementStructures(int itemID)
{
	numStructures++;
	numItem[itemID]++;

    // change power requirements
	bool bhasPowerBefore = hasPower();
	int currentItemPower = currentGame->objectData.data[itemID].power;
	if(currentItemPower >= 0) {
        powerRequirement += currentGame->objectData.data[itemID].power;
	}
	bool bhasPowerAfterwards = hasPower();

    if(currentGame->gameState != LOADING) {
        if(hasRadar() && (bhasPowerBefore != bhasPowerAfterwards)) {
            changeRadar(bhasPowerAfterwards);
        }
    } else {
        // loading => don't show radar animation
        if(hasRadar()) {
            currentGame->radarView->setRadarMode(bhasPowerAfterwards);
        }
    }

	// change spice capacity
	capacity += currentGame->objectData.data[itemID].capacity;

    if(currentGame->gameState != LOADING) {
        // do not check selection lists if we are loading
        checkSelectionLists();
    }

    std::list<std::shared_ptr<Player> >::iterator iter;
    for(iter = players.begin(); iter != players.end(); ++iter) {
        (*iter)->onIncrementStructures(itemID);
    }
}




void House::decrementStructures(int itemID, const Coord& location)
{
	numStructures--;
    numItem[itemID]--;

	// change power requirements
	bool bhasPowerBefore = hasPower();
	int currentItemPower = currentGame->objectData.data[itemID].power;
	if(currentItemPower >= 0) {
        powerRequirement -= currentItemPower;
	}
	bool bhasPowerAfterwards = hasPower();

	if((currentGame->gameState != DEINITIALIZE) && (bhasPowerBefore != bhasPowerAfterwards)) {
        changeRadar(bhasPowerAfterwards);
	}

    // change spice capacity
	capacity -= currentGame->objectData.data[itemID].capacity;

    if(currentGame->gameState != LOADING) {
        // do not check selection lists if we are loading
        checkSelectionLists();
    }

	if (!isAlive())
		lose();

    std::list<std::shared_ptr<Player> >::iterator iter;
    for(iter = players.begin(); iter != players.end(); ++iter) {
        (*iter)->onDecrementStructures(itemID, location);
    }
}




void House::noteDamageLocation(ObjectClass* pObject, const Coord& location)
{
    std::list<std::shared_ptr<Player> >::iterator iter;
    for(iter = players.begin(); iter != players.end(); ++iter) {
        (*iter)->onDamage(pObject, location);
    }
}




/**
    This method informs this house that one of its units has killed an enemy unit or structure
    \param itemID   the ID of the enemy unit or structure
*/
void House::informHasKilled(Uint32 itemID)
{
    destroyedValue += std::max(currentGame->objectData.data[itemID].price/100, 1);
    if(isStructure(itemID)) {
        numDestroyedStructures++;
    } else {
        numDestroyedUnits++;
    }
}




void House::win()
{
	if (getTeam() == pLocalHouse->getTeam()) {
		currentGame->setGameWon();
		soundPlayer->playVoice(YourMissionIsComplete,houseID);
	} else {
		currentGame->setGameLost();
		soundPlayer->playVoice(YouHaveFailedYourMission,houseID);
	}
}




void House::lose()
{
	char message[100];
	sprintf(message, pTextManager->getLocalized("House '%s' has been defeated.").c_str(), getHouseNameByNumber( (HOUSETYPE) getHouseID()).c_str());
	currentGame->AddToNewsTicker(message);

	if((getTeam() == pLocalHouse->getTeam()) && ((currentGame->winFlags & WINLOSEFLAGS_HUMAN_HAS_BUILDINGS) != 0)) {
	    // pLocalHouse is destroyed and this is a game finish condition
	    if((currentGame->loseFlags & WINLOSEFLAGS_HUMAN_HAS_BUILDINGS) != 0) {
	        // house has won
            currentGame->setGameWon();
            soundPlayer->playVoice(YourMissionIsComplete,houseID);
	    } else {
	        // house has lost
            currentGame->setGameLost();
            soundPlayer->playVoice(YouHaveFailedYourMission,houseID);
	    }

	} else if((currentGame->winFlags & WINLOSEFLAGS_AI_NO_BUILDINGS) != 0) {
		//if the only players left are on the thisPlayers team, pLocalHouse has won
		bool finished = true;

		for(int i=0; i<NUM_HOUSES; i++) {
			if ((currentGame->house[i] != NULL) && currentGame->house[i]->isAlive()
				&& (currentGame->house[i]->getTeam() != 0) && (currentGame->house[i]->getTeam() != pLocalHouse->getTeam()))
				finished = false;
		}

		if(finished) {
		    // all AI players are destroyed and this is a game finish condition
            if((currentGame->loseFlags & WINLOSEFLAGS_AI_NO_BUILDINGS) != 0) {
                // house has won
                currentGame->setGameWon();
                soundPlayer->playVoice(YouHaveFailedYourMission,houseID);
            } else {
                // house has lost
                currentGame->setGameLost();
                soundPlayer->playVoice(YourMissionIsComplete,houseID);
            }
		}
	}
}




void House::freeHarvester(int xPos, int yPos)
{
	if (currentGameMap->cellExists(xPos, yPos)
		&& currentGameMap->getCell(xPos, yPos)->hasAGroundObject()
		&& (currentGameMap->getCell(xPos, yPos)->getGroundObject()->getItemID() == Structure_Refinery))
	{
		RefineryClass* refinery = (RefineryClass*)currentGameMap->getCell(xPos, yPos)->getGroundObject();
		Coord closestPos = currentGameMap->findClosestEdgePoint(refinery->getLocation() + Coord(2,0), Coord(1,1));

		Carryall* carryall = (Carryall*)createUnit(Unit_Carryall);
		HarvesterClass* harvester = (HarvesterClass*)createUnit(Unit_Harvester);
		harvester->setAmountOfSpice(5.0);
		carryall->setOwned(false);
		carryall->giveCargo(harvester);
		carryall->deploy(closestPos);
		carryall->setDropOfferer(true);

		if (closestPos.x == 0)
			carryall->setAngle(RIGHT);
		else if (closestPos.x == currentGameMap->sizeX-1)
			carryall->setAngle(LEFT);
		else if (closestPos.y == 0)
			carryall->setAngle(DOWN);
		else if (closestPos.y == currentGameMap->sizeY-1)
			carryall->setAngle(UP);

		harvester->setTarget(refinery);
		harvester->setActive(false);
		carryall->setTarget(refinery);
	}
}




StructureClass* House::placeStructure(Uint32 builderID, int itemID, int xPos, int yPos, bool bForcePlacing)
{
	if(!currentGameMap->cellExists(xPos,yPos)) {
		return NULL;
	}

	if(builderID != NONE && itemID == Structure_Palace && getNumItems(Structure_Palace) > 0) {
        if(this == pLocalHouse) {
            currentGame->currentCursorMode = Game::CursorMode_Normal;
        }
        return NULL;
	}

	StructureClass* tempStructure = NULL;

	switch (itemID) {
		case (Structure_Slab1): {
			// Slabs are no normal buildings
			currentGameMap->getCell(xPos, yPos)->setType(Terrain_Slab);
			currentGameMap->getCell(xPos, yPos)->setOwner(getHouseID());
			currentGameMap->viewMap(getTeam(), xPos, yPos, currentGame->objectData.data[Structure_Slab1].viewrange);
	//		currentGameMap->getCell(xPos, yPos)->clearTerrain();
			ObjectClass* pObject = currentGame->getObjectManager().getObject(builderID);
			if (pObject != NULL) {
				if (pObject->isAStructure() && ((StructureClass*) pObject)->isABuilder()) {
					((BuilderClass*) pObject)->unSetWaitingToPlace();
					if (this == pLocalHouse) {
						currentGame->currentCursorMode = Game::CursorMode_Normal;
					}
				}
			} else if (this == pLocalHouse) {
				currentGame->currentCursorMode = Game::CursorMode_Normal;
			}
		} break;

		case (Structure_Slab4): {
			// Slabs are no normal buildings
			int i,j;
			for(i = xPos; i < xPos + 2; i++) {
				for(j = yPos; j < yPos + 2; j++) {
					if (currentGameMap->cellExists(i, j)) {
						TerrainClass* cell = currentGameMap->getCell(i, j);

						if (!cell->hasAGroundObject() && cell->isRock() && !cell->isMountain()) {
							cell->setType(Terrain_Slab);
							cell->setOwner(getHouseID());
							currentGameMap->viewMap(getTeam(), i, j, currentGame->objectData.data[Structure_Slab4].viewrange);
							//cell->clearTerrain();
						}
					}
				}
			}

			ObjectClass* pObject = currentGame->getObjectManager().getObject(builderID);
			if(pObject != NULL) {
				if (pObject->isAStructure() && ((StructureClass*) pObject)->isABuilder()) {
					((BuilderClass*) pObject)->unSetWaitingToPlace();

					if(this == pLocalHouse) {
						currentGame->currentCursorMode = Game::CursorMode_Normal;
					}
				}
			} else if (this == pLocalHouse) {
				currentGame->currentCursorMode = Game::CursorMode_Normal;
			}

		} break;

		default: {
			tempStructure = (StructureClass*) ObjectClass::createObject(itemID,this);
			if(tempStructure == NULL) {
				fprintf(stderr,"House::placeStructure(): Cannot create Object with itemID %d\n",itemID);
				fflush(stderr);
				exit(EXIT_FAILURE);
			}

            if(bForcePlacing == false) {
                // check if there is already something on this cell
                for(int i=0;i<tempStructure->getStructureSizeX();i++) {
                    for(int j=0;j<tempStructure->getStructureSizeY();j++) {
                        if((currentGameMap->cellExists(xPos+i, yPos+j) == false) || (currentGameMap->getCell(xPos+i, yPos+j)->hasAGroundObject() == true)) {
                            delete tempStructure;
                            return NULL;
                        }
                    }
                }
            }

			for(int i=0;i<tempStructure->getStructureSizeX();i++) {
				for(int j=0;j<tempStructure->getStructureSizeY();j++) {
					if(currentGameMap->cellExists(xPos+i, yPos+j)) {
						currentGameMap->getCell(xPos+i, yPos+j)->clearTerrain();
					}
				}
			}

			tempStructure->setLocation(xPos, yPos);

			if ((builderID != NONE) && (itemID != Structure_Wall)) {
				tempStructure->setJustPlaced();
			}

			// at the beginning of the game every refinery gets one harvester for free (brought by a carryall)
			if(((currentGame->gameState == START) || (builderID != NONE)) && (itemID == Structure_Refinery)) {
				freeHarvester(xPos, yPos);
			}

			// if this structure was built by a construction yard this construction yard must be informed
			BuilderClass* pBuilder = dynamic_cast<BuilderClass*>(currentGame->getObjectManager().getObject(builderID));
			if(pBuilder != NULL) {
                pBuilder->unSetWaitingToPlace();

                if(itemID == Structure_Palace) {
                    // cancel all other palaces
                    for(RobustList<StructureClass*>::iterator iter = structureList.begin(); iter != structureList.end(); ++iter) {
                        if((*iter)->getOwner() == this && (*iter)->getItemID() == Structure_ConstructionYard) {
                            ConstructionYardClass* pConstructionYard = (ConstructionYardClass*) *iter;

                            if(pBuilder != pConstructionYard) {
                                pConstructionYard->DoCancelItem(Structure_Palace, false);
                            }
                        }
                    }
                }

                if (this == pLocalHouse) {
                    currentGame->currentCursorMode = Game::CursorMode_Normal;
                }
			} else if (this == pLocalHouse) {
				currentGame->currentCursorMode = Game::CursorMode_Normal;
			}

			if(itemID == Structure_Radar && hasPower()) {
                changeRadar(true);
			}

			if(tempStructure->isABuilder()) {
                ((BuilderClass*) tempStructure)->checkSelectionList();
			}


		} break;
	}

	return tempStructure;
}




UnitClass* House::createUnit(int itemID)
{
	UnitClass* newUnit = NULL;

	newUnit = (UnitClass*) ObjectClass::createObject(itemID,this);

	if(newUnit == NULL) {
		fprintf(stderr,"House::createUnit(): Cannot create Object with itemID %d\n",itemID);
		fflush(stderr);
		exit(EXIT_FAILURE);
	}

	return newUnit;
}




UnitClass* House::placeUnit(int itemID, int xPos, int yPos)
{
	UnitClass* newUnit = NULL;
	if(currentGameMap->cellExists(xPos, yPos) == true) {
	    TerrainClass* pTile = currentGameMap->getCell(xPos,yPos);

	    if(itemID == Unit_Fremen || itemID == Unit_Saboteur || itemID == Unit_Sardaukar || itemID == Unit_Soldier || itemID == Unit_Trooper) {
            if((pTile->hasANonInfantryGroundObject() == true) || (pTile->infantryNotFull() == false)) {
                // infantry units can not placed on non-infantry units or structures (or the cell is already full of infantry units)
                return NULL;
            }
	    } else {
	        if(pTile->hasAGroundObject() == true) {
                // non-infantry units can not placed on a cell where already some other unit or structure is placed on
                return NULL;
	        }
	    }

        newUnit = createUnit(itemID);
	}

	if (newUnit) {
		Coord pos = Coord(xPos, yPos);
		if (newUnit->canPass(xPos, yPos)) {
			newUnit->deploy(pos);
		} else {
			newUnit->setVisible(VIS_ALL, false);
			newUnit->destroy();
			newUnit = NULL;
		}
	}

	return newUnit;
}




/**
    This method starts the animation for switching the radar on or off
    \param status   true if the radar is switched on, false if switched off
*/
void House::changeRadar(bool status)
{
	if(this == pLocalHouse) {
		soundPlayer->playSound(RadarNoise);

		if(status == true && pLocalHouse->hasRadar()) {
			//turn on
			currentGame->radarView->switchRadarMode(true);
			soundPlayer->playVoice(RadarActivated,this->getHouseID());
		} else {
			//turn off
			currentGame->radarView->switchRadarMode(false);
			soundPlayer->playVoice(RadarDeactivated,this->getHouseID());
		}
	}
}




/**
    This method returns the center of the base of this house.
    \return the coordinate of the center in tile coordinates
*/
Coord House::getCenterOfMainBase() const {
    Coord center;
    int numStructures = 0;

    RobustList<StructureClass*>::const_iterator iter;
    for(iter = structureList.begin(); iter != structureList.end(); ++iter) {
        StructureClass* tempStructure = *iter;

        if(tempStructure->getOwner() == this) {
            center += tempStructure->getLocation();
            numStructures++;
        }
    }

    center /= numStructures;

    return center;
}




void House::decrementHarvesters()
{
    numItem[Unit_Harvester]--;

    if (numItem[Unit_Harvester] <= 0) {
        numItem[Unit_Harvester] = 0;

        if(numItem[Structure_Refinery]) {
            Coord	closestPos;
            Coord	pos = Coord(0,0);
            double	closestDistance = INFINITY;
            StructureClass *closestRefinery = NULL;

            RobustList<StructureClass*>::const_iterator iter;
            for(iter = structureList.begin(); iter != structureList.end(); ++iter) {
                StructureClass* tempStructure = *iter;

                if((tempStructure->getItemID() == Structure_Refinery) && (tempStructure->getOwner() == this) && (tempStructure->getHealth() > 0.0)) {
                    pos = tempStructure->getLocation();

                    Coord closestPoint = tempStructure->getClosestPoint(pos);
                    double refineryDistance = blockDistance(pos, closestPoint);
                    if(!closestRefinery || (refineryDistance < closestDistance)) {
                            closestDistance = refineryDistance;
                            closestRefinery = tempStructure;
                            closestPos = pos;
                    }
                }
            }

            if(closestRefinery && (currentGame->gameState == BEGUN)) {
                freeHarvester(closestRefinery->getLocation().x, closestRefinery->getLocation().y);
            }
        }
    }
}
