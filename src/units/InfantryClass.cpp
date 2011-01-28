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

#include <units/InfantryClass.h>

#include <globals.h>

#include <House.h>
#include <Game.h>
#include <MapClass.h>
#include <SoundPlayer.h>

#include <structures/StructureClass.h>
#include <structures/RefineryClass.h>
#include <structures/RepairYardClass.h>
#include <units/HarvesterClass.h>

InfantryClass::InfantryClass(House* newOwner) : GroundUnit(newOwner)
{
    InfantryClass::init();

    setHealth(getMaxHealth());

    cellPosition = INVALID_POS;
    oldCellPosition = INVALID_POS;
}

InfantryClass::InfantryClass(Stream& stream) : GroundUnit(stream)
{
    InfantryClass::init();

	cellPosition = stream.readSint8();
	oldCellPosition = stream.readSint8();
}

void InfantryClass::init()
{
	infantry = true;

	walkFrame = 0;
}

InfantryClass::~InfantryClass()
{
}


void InfantryClass::save(Stream& stream) const
{
	GroundUnit::save(stream);

	stream.writeSint8(cellPosition);
	stream.writeSint8(oldCellPosition);
}

void InfantryClass::HandleCaptureClick(int xPos, int yPos) {
	if(respondable && ((getItemID() == Unit_Fremen) || (getItemID() == Unit_Sardaukar) || (getItemID() == Unit_Soldier) || (getItemID() == Unit_Trooper))) {
		if (currentGameMap->cellExists(xPos, yPos)) {
			if (currentGameMap->cell[xPos][yPos].hasAnObject()) {
				// capture structure
				ObjectClass* tempTarget = currentGameMap->cell[xPos][yPos].getObject();

				currentGame->GetCommandManager().addCommand(Command(CMD_INFANTRY_CAPTURE,objectID,tempTarget->getObjectID()));
			}
		}
	}

}

void InfantryClass::DoCaptureObject(Uint32 TargetStructureID) {
	StructureClass* pStructure = dynamic_cast<StructureClass*>(currentGame->getObjectManager().getObject(TargetStructureID));

	if((pStructure == NULL) || (pStructure->canBeCaptured() == false) || (pStructure->getOwner()->getTeam() == getOwner()->getTeam())) {
	    // does not exist anymore, cannot be captured or is a friendly building
        return;
	}

	DoAttackObject(pStructure);
	DoSetAttackMode(CAPTURE);
}

void InfantryClass::assignToMap(const Coord& pos)
{
	if(currentGameMap->cellExists(pos)) {
		oldCellPosition = cellPosition;
		cellPosition = currentGameMap->cell[pos.x][pos.y].assignInfantry(getObjectID());
	}
}

void InfantryClass::blitToScreen()
{
	SDL_Rect dest, source;
    dest.x = getDrawnX();
    dest.y = getDrawnY();
    dest.w = source.w = imageW;
    dest.h = source.h = imageH;

    int temp = drawnAngle;
    if (temp == UP)
        temp = 1;
    else if (temp == DOWN)
        temp = 3;
    else if (temp == LEFTUP || temp == LEFTDOWN || temp == LEFT)
        temp = 2;
    else	//RIGHT
        temp = 0;

    source.x = temp*imageW;

    if (walkFrame/10 == 3)
        source.y = imageH;
    else
        source.y = walkFrame/10*imageH;

    SDL_BlitSurface(graphic, &source, screen, &dest);
}

bool InfantryClass::canPass(int xPos, int yPos) const
{
	bool passable = false;
	if (currentGameMap->cellExists(xPos, yPos))
	{
		TerrainClass* cell = currentGameMap->getCell(xPos, yPos);
		if (!cell->hasAGroundObject())
		{
			if (cell->getType() != Terrain_Mountain)
				passable = true;
			else
			{
				/* if this unit is infantry so can climb, and cell can take more infantry */
				if (cell->infantryNotFull())
					passable = true;
			}
		}
		else
		{
			ObjectClass *object = cell->getGroundObject();

			if ((object != NULL) && (object->getObjectID() == target.getObjectID())
				&& object->isAStructure()
				&& (object->getOwner()->getTeam() != owner->getTeam())
				&& object->isVisible(getOwner()->getTeam()))
			{
				passable = true;
			} else {
				passable = (!cell->hasANonInfantryGroundObject()
							&& (cell->infantryNotFull()
							&& (cell->getInfantryTeam() == getOwner()->getTeam())));
			}
		}
	}
	return passable;
}

void InfantryClass::checkPos()
{
	if(moving) {
		if(++walkFrame > 39) {
			walkFrame = 0;
		}
	}

	if(justStoppedMoving) {
		walkFrame = 0;

		if(currentGameMap->getCell(location)->isSpiceBloom()) {
			Coord realPos;
			realPos.x = location.x*BLOCKSIZE + BLOCKSIZE/2;
			realPos.y = location.y*BLOCKSIZE + BLOCKSIZE/2;
			currentGameMap->getCell(location)->damageCell(objectID, getOwner(), realPos, NONE, 500, BLOCKSIZE, false);
		}

        //check to see if close enough to blow up target
        if(target.getObjPointer() != NULL
            && target.getObjPointer()->isAStructure()
            && (getOwner()->getTeam() != target.getObjPointer()->getOwner()->getTeam()))
        {
            Coord	closestPoint;

            closestPoint = target.getObjPointer()->getClosestPoint(location);

            if(blockDistance(location, closestPoint) <= 0.5) {
                StructureClass* pCapturedStructure = target.getStructurePointer();
                if(pCapturedStructure->getHealthColor() == COLOR_RED) {
                    House* pOwner = pCapturedStructure->getOwner();
                    int targetID = pCapturedStructure->getItemID();
                    int posX = pCapturedStructure->getX();
                    int posY = pCapturedStructure->getY();
                    int origHouse = pCapturedStructure->getOriginalHouse();
                    int oldHealth = pCapturedStructure->getHealth();
                    bool isSelected = pCapturedStructure->isSelected();

                    double capturedSpice = 0.0;

                    UnitClass* pContainedUnit = NULL;

                    if(pCapturedStructure->getItemID() == Structure_Silo) {
                        capturedSpice = currentGame->objectData.data[Structure_Silo].capacity * (pOwner->getStoredCredits() / pOwner->getCapacity());
                    } else if(pCapturedStructure->getItemID() == Structure_Refinery) {
                        capturedSpice = currentGame->objectData.data[Structure_Silo].capacity * (pOwner->getStoredCredits() / pOwner->getCapacity());
                        RefineryClass* pRefinery = dynamic_cast<RefineryClass*>(pCapturedStructure);
                        if(pRefinery->isFree() == false) {
                            pContainedUnit = pRefinery->getHarvester();
                        }
                    } else if(pCapturedStructure->getItemID() == Structure_RepairYard) {
                        RepairYardClass* pRepairYard = dynamic_cast<RepairYardClass*>(pCapturedStructure);
                        if(pRepairYard->isFree() == false) {
                            pContainedUnit = pRepairYard->getRepairUnit();
                        }
                    }

                    Uint32 containedUnitID = NONE;
                    double containedUnitHealth = 0.0;
                    double containedHarvesterSpice = 0.0;
                    if(pContainedUnit != NULL) {
                        containedUnitID = pContainedUnit->getItemID();
                        containedUnitHealth = pContainedUnit->getHealth();
                        if(containedUnitID == Unit_Harvester) {
                            containedHarvesterSpice = dynamic_cast<HarvesterClass*>(pContainedUnit)->getAmountOfSpice();
                        }

                        // will be destroyed by the captured structure
                        pContainedUnit = NULL;
                    }

                    // remove all other infantry units capturing this building
                    Coord capturedStructureLocation = pCapturedStructure->getLocation();
                    for(int i = capturedStructureLocation.x; i < capturedStructureLocation.x + pCapturedStructure->getStructureSizeX(); i++) {
                        for(int j = capturedStructureLocation.y; j < capturedStructureLocation.y + pCapturedStructure->getStructureSizeY(); j++) {
                            std::list<Uint32> infantryList = currentGameMap->getCell(i,j)->getInfantryList();
                            std::list<Uint32>::iterator iter;
                            for(iter = infantryList.begin(); iter != infantryList.end(); ++iter) {
                                if(*iter != getObjectID()) {
                                    ObjectClass* pObject = currentGame->getObjectManager().getObject(*iter);
                                    if(pObject->getLocation() == Coord(i,j)) {
                                        pObject->destroy();
                                    }
                                }
                            }
                        }
                    }


                    // destroy captured structure ...
                    delete pCapturedStructure;

                    // ... and create a new one
                    StructureClass* pNewStructure = owner->placeStructure(NONE, targetID, posX, posY, true);

                    pNewStructure->setOriginalHouse(origHouse);
                    pNewStructure->setHealth(oldHealth);
                    if(isSelected == true) {
                        pNewStructure->setSelected(true);
                        currentGame->getSelectedList().insert(pNewStructure->getObjectID());
                        currentGame->getGameInterface().UpdateObjectInterface();
                    }

                    if(containedUnitID != NONE) {
                        UnitClass* pNewUnit = owner->createUnit(containedUnitID);

                        pNewUnit->setRespondable(false);
                        pNewUnit->setActive(false);
                        pNewUnit->setVisible(VIS_ALL, false);
                        pNewUnit->setHealth(containedUnitHealth);

                        if(pNewUnit->getItemID() == Unit_Harvester) {
                            dynamic_cast<HarvesterClass*>(pNewUnit)->setAmountOfSpice(containedHarvesterSpice);
                        }

                        if(pNewStructure->getItemID() == Structure_Refinery) {
                            RefineryClass* pRefinery = dynamic_cast<RefineryClass*>(pNewStructure);
                            pRefinery->book();
                            pRefinery->assignHarvester(dynamic_cast<HarvesterClass*>(pNewUnit));
                        } else if(pNewStructure->getItemID() == Structure_RepairYard) {
                            RepairYardClass* pRepairYard = dynamic_cast<RepairYardClass*>(pNewStructure);
                            pRepairYard->book();
                            pRepairYard->assignUnit(pNewUnit);
                        }
                    }

                    // steal credits
                    pOwner->takeCredits(capturedSpice);
                    owner->addCredits(capturedSpice, false);
                    owner->checkSelectionLists();

                } else {
                    pCapturedStructure->handleDamage(50, NONE, getOwner());
                }
                // destroy unit indirectly
                setTarget(NULL);
                setHealth(0.0);
                return;
            }
        } else if(target.getObjPointer() != NULL && target.getObjPointer()->isAStructure())	{
            Coord	closestPoint;
            closestPoint = target.getObjPointer()->getClosestPoint(location);

            if(blockDistance(location, closestPoint) <= 0.5) {
                // destroy unit indirectly
                setTarget(NULL);
                setHealth(0.0);
                return;
            }
        }
	}
}

void InfantryClass::destroy()
{
    if(currentGameMap->cellExists(location) && isVisible()) {
        TerrainClass* pTerrain = currentGameMap->getCell(location);

        if(pTerrain->hasANonInfantryGroundObject() == true) {
            if(pTerrain->getNonInfantryGroundObject()->isAUnit()) {
                // squashed
                pTerrain->assignDeadUnit( currentGame->RandomGen.randBool() ? DeadUnit_Infantry_Squashed1 : DeadUnit_Infantry_Squashed2,
                                            owner->getHouseID(),
                                            Coord((Sint32) realX, (Sint32) realY) );

                if(isVisible(getOwner()->getTeam())) {
                    soundPlayer->playSoundAt(Sound_Squashed,location);
                }
            } else {
                // this unit has captured a building
            }

        } else if(getItemID() != Unit_Saboteur) {
            // "normal" dead
            pTerrain->assignDeadUnit( DeadUnit_Infantry,
                                        owner->getHouseID(),
                                        Coord((Sint32) realX, (Sint32) realY));

            if(isVisible(getOwner()->getTeam()))
                soundPlayer->playSoundAt((Sound_enum) getRandomOf(5,Sound_Scream1,Sound_Scream2,Sound_Scream3,Sound_Scream4,Sound_Scream5),location);
        }
    }

	GroundUnit::destroy();
}

void InfantryClass::move()
{
	if(!moving && currentGame->RandomGen.rand(0,40) == 0) {
		currentGameMap->viewMap(owner->getTeam(), location, getViewRange());
	}

	if(moving) {
		Coord	wantedReal;

		realX += xSpeed;
		realY += ySpeed;

		wantedReal.x = nextSpot.x*BLOCKSIZE + BLOCKSIZE/2,
		wantedReal.y = nextSpot.y*BLOCKSIZE + BLOCKSIZE/2;

		switch (cellPosition) {

		case 1:
			wantedReal.x -= BLOCKSIZE/4;
			wantedReal.y -= BLOCKSIZE/4;
			break;

		case 2:
			wantedReal.x += BLOCKSIZE/4;
			wantedReal.y -= BLOCKSIZE/4;
			break;

		case 3:
			wantedReal.x -= BLOCKSIZE/4;
			wantedReal.y += BLOCKSIZE/4;
			break;

		case 4:
			wantedReal.x += BLOCKSIZE/4;
			wantedReal.y += BLOCKSIZE/4;
			break;

		default:
			break;
		}

        // check if vehicle is on the first half of the way
		if(location != nextSpot) {
		    // check if vehicle is half way out of old cell
            if ((abs(location.x*BLOCKSIZE - (int)realX + BLOCKSIZE/2) >= BLOCKSIZE/2)
                || (abs(location.y*BLOCKSIZE - (int)realY + BLOCKSIZE/2) >= BLOCKSIZE/2))
            {
                unassignFromMap(location);	//let something else go in
                oldLocation = location;
                location = nextSpot;
		    }
		}

		if((fabs((double)wantedReal.x - realX) <= 0.2) && (fabs((double)wantedReal.y - realY) <= 0.2)) {
			realX = wantedReal.x;
			realY = wantedReal.y;

            if(forced && (location == destination) && !target) {
				setForced(false);
			}

			moving = false;
			justStoppedMoving = true;

			currentGameMap->viewMap(owner->getTeam(), location, getViewRange());

		}
	} else {
		justStoppedMoving = false;
	}

	checkPos();
}


void InfantryClass::setLocation(int xPos, int yPos)
{
	if (currentGameMap->cellExists(xPos, yPos) || ((xPos == INVALID_POS) && (yPos == INVALID_POS)))
	{
		oldCellPosition = cellPosition = INVALID_POS;
		GroundUnit::setLocation(xPos, yPos);
		switch (cellPosition)
		{
		case 1:
			realX -= BLOCKSIZE/4;
			realY -= BLOCKSIZE/4;
			break;
		case 2:
			realX += BLOCKSIZE/4;
			realY -= BLOCKSIZE/4;
			break;
		case 3:
			realX -= BLOCKSIZE/4;
			realY += BLOCKSIZE/4;
			break;
		case 4:
			realX += BLOCKSIZE/4;
			realY += BLOCKSIZE/4;
			break;
		default:
			break;
		}
	}
}

void InfantryClass::setSpeeds()
{
	if (oldCellPosition == INVALID_POS)
		fprintf(stderr, "InfantryClass::setSpeeds(): Infantry cell position  == INVALID_POS.\n");
	else if (cellPosition == oldCellPosition)	//havent changed infantry position
		GroundUnit::setSpeeds();
	else
	{
		int sx = 0, sy = 0, dx = 0, dy = 0;
		switch (oldCellPosition)
		{
		case 0: sx = sy = 0; break;
		case 1: sx = -BLOCKSIZE/4; sy = -BLOCKSIZE/4; break;
		case 2: sx = BLOCKSIZE/4; sy = -BLOCKSIZE/4; break;
		case 3: sx = -BLOCKSIZE/4; sy = BLOCKSIZE/4; break;
		case 4: sx = BLOCKSIZE/4; sy = BLOCKSIZE/4; break;
		}

		switch (cellPosition)
		{
		case 0: dx = dy = 0; break;
		case 1: dx = -BLOCKSIZE/4; dy = -BLOCKSIZE/4; break;
		case 2: dx = BLOCKSIZE/4; dy = -BLOCKSIZE/4; break;
		case 3: dx = -BLOCKSIZE/4; dy = BLOCKSIZE/4; break;
		case 4: dx = BLOCKSIZE/4; dy = BLOCKSIZE/4; break;
		}

		switch (drawnAngle)
		{
		case (RIGHT): dx += BLOCKSIZE; break;
		case (RIGHTUP): dx += BLOCKSIZE; dy -= BLOCKSIZE; break;
		case (UP): dy -= BLOCKSIZE; break;
		case (LEFTUP): dx -= BLOCKSIZE; dy -= BLOCKSIZE; break;
		case (LEFT): dx -= BLOCKSIZE; break;
		case (LEFTDOWN): dx -= BLOCKSIZE; dy += BLOCKSIZE; break;
		case (DOWN): dy += BLOCKSIZE; break;
		case (RIGHTDOWN): dx += BLOCKSIZE; dy += BLOCKSIZE; break;
		}

		dx -= sx;
		dy -= sy;

		double scale = currentGame->objectData.data[itemID].maxspeed/sqrt((double)(dx*dx + dy*dy));
		xSpeed = dx*scale;
		ySpeed = dy*scale;
	}
}

void InfantryClass::squash()
{
	destroy();
	return;
}

void InfantryClass::playConfirmSound() {
	soundPlayer->playSound((Sound_enum) getRandomOf(2,MovingOut,InfantryOut));
}

void InfantryClass::playSelectSound() {
	soundPlayer->playSound(YesSir);
}
