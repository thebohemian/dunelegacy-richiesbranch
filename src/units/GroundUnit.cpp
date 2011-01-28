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

#include <units/GroundUnit.h>

#include <globals.h>

#include <Game.h>
#include <House.h>
#include <MapClass.h>
#include <SoundPlayer.h>

#include <structures/RepairYardClass.h>
#include <units/Carryall.h>

GroundUnit::GroundUnit(House* newOwner) : UnitClass(newOwner)
{
    GroundUnit::init();

    awaitingPickup = false;
	bookedCarrier = NONE;
}

GroundUnit::GroundUnit(Stream& stream) : UnitClass(stream)
{
    GroundUnit::init();

    awaitingPickup = stream.readBool();
    bookedCarrier = stream.readUint32();
}

void GroundUnit::init()
{
    aGroundUnit = true;
}

GroundUnit::~GroundUnit()
{
}

void GroundUnit::save(Stream& stream) const
{
	UnitClass::save(stream);

	stream.writeBool(awaitingPickup);
	stream.writeUint32(bookedCarrier);
}

void GroundUnit::assignToMap(const Coord& pos)
{
	if (currentGameMap->cellExists(pos)) {
		currentGameMap->cell[pos.x][pos.y].assignNonInfantryGroundObject(getObjectID());
		currentGameMap->viewMap(owner->getTeam(), location, getViewRange());
	}
}

void GroundUnit::checkPos()
{
    if(!moving && !isInfantry()) {
        currentGameMap->getCell(location.x,location.y)->setTrack(drawnAngle);
    }

	if(justStoppedMoving)
	{
		realX = location.x*BLOCKSIZE + BLOCKSIZE/2;
		realY = location.y*BLOCKSIZE + BLOCKSIZE/2;
		//findTargetTimer = 0;	//allow a scan for new targets now

		if(currentGameMap->getCell(location)->isSpiceBloom()) {
		    setHealth(0.0);
			currentGameMap->getCell(location)->triggerSpiceBloom(getOwner());
		} else if(currentGameMap->getCell(location)->isSpecialBloom()){
            currentGameMap->getCell(location)->triggerSpecialBloom(getOwner());
		}
	}

	if(goingToRepairYard) {
	    if(target.getObjPointer() == NULL) {
            goingToRepairYard = false;
            awaitingPickup = false;
            bookedCarrier = NONE;
            pathList.clear();
	    } else {
            Coord closestPoint = target.getObjPointer()->getClosestPoint(location);
            if ((moving == false) && (blockDistance(location, closestPoint) <= 1.5)
                && ((RepairYardClass*)target.getObjPointer())->isFree())
            {
                if (getHealth()*100.0/getMaxHealth() < 100.0) {
                    setGettingRepaired();
                } else {
                    setTarget(NULL);
                    setDestination(guardPoint);
                }
            }
	    }
	}
}

void GroundUnit::playConfirmSound() {
	soundPlayer->playSound((Sound_enum) getRandomOf(2,Acknowledged,Affirmative));
}

void GroundUnit::playSelectSound() {
	soundPlayer->playSound(Reporting);
}

bool GroundUnit::requestCarryall()
{
	if (getOwner()->hasCarryalls())	{
		Carryall* carryall = NULL;

        RobustList<UnitClass*>::const_iterator iter;
	    for(iter = unitList.begin(); iter != unitList.end(); ++iter) {
			UnitClass* unit = *iter;
			if ((unit->getOwner() == owner) && (unit->getItemID() == Unit_Carryall)) {
				if(!((Carryall*)unit)->isBooked()) {
					carryall = (Carryall*)unit;
					carryall->setTarget(this);
					carryall->clearPath();
					bookCarrier(carryall);

					//setDestination(&location);	//stop moving, and wait for carryall to arrive

					return true;
				}
			}
		}
	}
	return false;
}

void GroundUnit::setPickedUp(UnitClass* newCarrier)
{
	UnitClass::setPickedUp(newCarrier);
	awaitingPickup = false;
	bookedCarrier = NONE;
	pathList.clear();
}

void GroundUnit::bookCarrier(UnitClass* newCarrier)
{
    if(newCarrier == NULL) {
        bookedCarrier = NONE;
        awaitingPickup = false;
    } else {
        bookedCarrier = newCarrier->getObjectID();
        awaitingPickup = true;
    }
}

bool GroundUnit::hasBookedCarrier() const
{
    if(bookedCarrier == NONE) {
        return false;
    } else {
        return (currentGame->getObjectManager().getObject(bookedCarrier) != NULL);
    }
}

UnitClass* GroundUnit::getCarrier() const
{
    return (UnitClass*) currentGame->getObjectManager().getObject(bookedCarrier);
}

void GroundUnit::navigate()
{
	if (!awaitingPickup)
		UnitClass::navigate();
}

void GroundUnit::DoRepair()
{
	if(getHealth() < getMaxHealth()) {
		//find a repair yard to return to

		double	closestLeastBookedRepairYardDistance = 1000000;
        RepairYardClass* bestRepairYard = NULL;

        RobustList<StructureClass*>::const_iterator iter;
        for(iter = structureList.begin(); iter != structureList.end(); ++iter) {
            StructureClass* tempStructure = *iter;

			if ((tempStructure->getItemID() == Structure_RepairYard) && (tempStructure->getOwner() == owner)) {
                RepairYardClass* tempRepairYard = ((RepairYardClass*)tempStructure);

                if(tempRepairYard->getNumBookings() == 0) {
                    double tempDistance = distance_from(location, tempRepairYard->getClosestPoint(location));
					if(tempDistance < closestLeastBookedRepairYardDistance) {
                        closestLeastBookedRepairYardDistance = tempDistance;
                        bestRepairYard = tempRepairYard;
                    }
                }
            }
        }

        if(bestRepairYard) {
            if(requestCarryall()) {
                DoMove2Object(bestRepairYard);
            } else if(owner->isAI()) {
                DoMove2Object(bestRepairYard);
            }
        }
	}
}
