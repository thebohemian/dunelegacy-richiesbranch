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

#include <units/Carryall.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <House.h>
#include <MapClass.h>
#include <Game.h>

#include <structures/RepairYardClass.h>
#include <structures/RefineryClass.h>
#include <structures/ConstructionYardClass.h>
#include <units/HarvesterClass.h>

Carryall::Carryall(House* newOwner) : AirUnit(newOwner)
{
    Carryall::init();

    setHealth(getMaxHealth());

	booked = false;
    idle = true;
	firstRun = true;
    owned = true;

    aDropOfferer = false;
    droppedOffCargo = false;
    respondable = false;

    currentMaxSpeed = 0.5;

	curFlyPoint = 0;
	for(int i=0; i < 8; i++) {
		flyPoints[i].x = INVALID_POS;
		flyPoints[i].y = INVALID_POS;
	}
	constYardPoint.x = INVALID_POS;
	constYardPoint.y = INVALID_POS;
}

Carryall::Carryall(Stream& stream) : AirUnit(stream)
{
    Carryall::init();

	PickedUpUnitList = stream.readUint32List();
	if(!PickedUpUnitList.empty()) {
		drawnFrame = 1;
	}

    booked = stream.readBool();
    idle = stream.readBool();
    firstRun = stream.readBool();
    owned = stream.readBool();

	aDropOfferer = stream.readBool();
	droppedOffCargo = stream.readBool();

	currentMaxSpeed = stream.readDouble();

	curFlyPoint = stream.readUint8();
	for(int i=0; i < 8; i++) {
		flyPoints[i].x = stream.readSint32();
		flyPoints[i].y = stream.readSint32();
	}
	constYardPoint.x = stream.readSint32();
	constYardPoint.y = stream.readSint32();
}

void Carryall::init()
{
	itemID = Unit_Carryall;
	owner->incrementUnits(itemID);

	canAttackStuff = false;

	GraphicID = ObjPic_Carryall;
	graphic = pGFXManager->getObjPic(GraphicID,getOwner()->getHouseID());
	shadowGraphic = pGFXManager->getObjPic(ObjPic_CarryallShadow,getOwner()->getHouseID());

	imageW = graphic->w/NUM_ANGLES;
	imageH = graphic->h/2;
}

Carryall::~Carryall()
{
	;
}

void Carryall::save(Stream& stream) const
{
	AirUnit::save(stream);

	stream.writeUint32List(PickedUpUnitList);

    stream.writeBool(booked);
	stream.writeBool(idle);
	stream.writeBool(firstRun);
	stream.writeBool(owned);

	stream.writeBool(aDropOfferer);
	stream.writeBool(droppedOffCargo);

	stream.writeDouble(currentMaxSpeed);

	stream.writeUint8(curFlyPoint);
	for(int i=0; i < 8; i++) {
		stream.writeSint32(flyPoints[i].x);
		stream.writeSint32(flyPoints[i].y);
	}
	stream.writeSint32(constYardPoint.x);
	stream.writeSint32(constYardPoint.y);
}

bool Carryall::update() {

    if(AirUnit::update() == false) {
        return false;
    }

    double dist = distance_from(location.x*BLOCKSIZE + BLOCKSIZE/2, location.y*BLOCKSIZE + BLOCKSIZE/2,
                                destination.x*BLOCKSIZE + BLOCKSIZE/2, destination.y*BLOCKSIZE + BLOCKSIZE/2);

    if((target || hasCargo()) && dist < 64.0) {
        currentMaxSpeed = (((0.5 - currentGame->objectData.data[itemID].maxspeed)/64.0) * (64.0 - dist)) + currentGame->objectData.data[itemID].maxspeed;
        setSpeeds();
    } else {
        currentMaxSpeed = std::min(currentMaxSpeed + 0.05, currentGame->objectData.data[itemID].maxspeed);
        setSpeeds();
    }

	// check if this carryall has to be removed because it has just brought something
	// to the map (e.g. new harvester)
	if (active)	{
		if(aDropOfferer && droppedOffCargo
            && (    (location.x == 0) || (location.x == currentGameMap->sizeX-1)
                    || (location.y == 0) || (location.y == currentGameMap->sizeY-1))
            && !moving)	{

            setVisible(VIS_ALL, false);
            destroy();
            return false;
		}
	}
	return true;
}

double Carryall::getMaxSpeed() const {
    return currentMaxSpeed;
}

void Carryall::deploy(const Coord& newLocation) {
	AirUnit::deploy(newLocation);

	respondable = false;
}

void Carryall::checkPos()
{
	AirUnit::checkPos();

	if (active)	{
		if (hasCargo() && (location == destination) && (distance_from(realX, realY, destination.x * BLOCKSIZE + (BLOCKSIZE/2), destination.y * BLOCKSIZE + (BLOCKSIZE/2)) < BLOCKSIZE/8) ) {
            deployUnit(*(PickedUpUnitList.begin()) );

            if(PickedUpUnitList.empty() == false) {
                // find next place to drop
                for(int i=8;i<18;i++) {
                    int r = currentGame->RandomGen.rand(3,i/2);
                    double angle = 2*M_PI*currentGame->RandomGen.randDouble();

                    Coord dropCoord = location + Coord( (int) (r*sin(angle)), (int) (-r*cos(angle)));
                    if(currentGameMap->cellExists(dropCoord) && currentGameMap->getCell(dropCoord)->hasAGroundObject() == false) {
                        setDestination(dropCoord);
                        break;
                    }
                }
            } else {
                setTarget(NULL);
                setDestination(guardPoint);

                idle = true;
            }
		} else if((isBooked() == false) && idle && !firstRun) {
			//fly around const yard
			Coord point = this->getClosestPoint(location);

			if(point == guardPoint) {
				//arrived at point, move to next
				curFlyPoint++;

				if(curFlyPoint >= 8) {
					curFlyPoint = 0;
				}

                int looped = 0;
				while(!(currentGameMap->cellExists(flyPoints[curFlyPoint].x, flyPoints[curFlyPoint].y)) && looped <= 2) {
					curFlyPoint++;

					if(curFlyPoint >= 8) {
						curFlyPoint = 0;
						looped++;
					}
				}

				setGuardPoint(flyPoints[curFlyPoint]);
				setDestination(guardPoint);
			}
		} else if(firstRun && owned) {
			findConstYard();
			setGuardPoint(constYardPoint);
			setDestination(guardPoint);
			firstRun = false;
		}
	}

}

void Carryall::deployUnit(Uint32 UnitID)
{
	bool found = false;

	std::list<Uint32>::iterator iter;
	for(iter = PickedUpUnitList.begin() ; iter != PickedUpUnitList.end(); ++iter) {
		if(*iter == UnitID) {
			found = true;
			break;
		}
	}

	if(found == false) {
        return;
	}

	PickedUpUnitList.remove(UnitID);

	UnitClass* unit = (UnitClass*) (currentGame->getObjectManager().getObject(UnitID));

	if(unit == NULL)
		return;

	if (found) {
	    currentMaxSpeed = 0.0;
	    setSpeeds();

	    if (currentGameMap->getCell(location)->hasANonInfantryGroundObject()) {
			ObjectClass* object = currentGameMap->getCell(location)->getNonInfantryGroundObject();
			if (object->getOwner() == getOwner()) {
				if (object->getItemID() == Structure_RepairYard) {
					if (((RepairYardClass*)object)->isFree()) {
						unit->setTarget(object);
						unit->setGettingRepaired();
						unit = NULL;
					} else {
					    // carryall has booked this repair yard but now will not go there => unbook
						((RepairYardClass*)object)->unBook();

						// unit is still going to repair yard but was unbooked from repair yard at pickup => book now
						((RepairYardClass*)object)->book();
					}
				} else if ((object->getItemID() == Structure_Refinery) && (unit->getItemID() == Unit_Harvester)) {
					if (((RefineryClass*)object)->isFree()) {
						((HarvesterClass*)unit)->setTarget(object);
						((HarvesterClass*)unit)->setReturned();
						unit = NULL;
						goingToRepairYard = false;
					}
				}
			}
		}

		if(unit != NULL) {
			unit->setAngle(drawnAngle);
			Coord deployPos = currentGameMap->findDeploySpot(unit, location);
			unit->deploy(deployPos);
		}

		if (PickedUpUnitList.empty())
		{
			if(!aDropOfferer) {
				booked = false;
                idle = true;
			}
			droppedOffCargo = true;
			drawnFrame = 0;
			recalculatePathTimer = 0;
		}
	}
}

void Carryall::destroy()
{
    // destroy cargo
	std::list<Uint32>::const_iterator iter;
	for(iter = PickedUpUnitList.begin() ; iter != PickedUpUnitList.end(); ++iter) {
		UnitClass* unit = (UnitClass*) (currentGame->getObjectManager().getObject(*iter));
		if(unit != NULL) {
			unit->destroy();
		}
	}
	PickedUpUnitList.clear();

	// place wreck
    if(isVisible() && currentGameMap->cellExists(location)) {
        TerrainClass* pTerrain = currentGameMap->getCell(location);
        pTerrain->assignDeadUnit(DeadUnit_Carrall, owner->getHouseID(), Coord((Sint32) realX, (Sint32) realY));
    }

	AirUnit::destroy();
}

void Carryall::engageTarget()
{
    if ((target.getObjPointer() == NULL)
		|| (!target.getObjPointer()->isVisible(getOwner()->getTeam()))
		|| (target.getObjPointer()->isAGroundUnit()
			&& !((GroundUnit*)target.getObjPointer())->isAwaitingPickup())
		|| (target.getObjPointer()->getOwner()->getTeam() != owner->getTeam())) {

		setTarget(NULL);

		if (!hasCargo()) {
			booked = false;
			idle = true;
			setDestination(guardPoint);
			nextSpotFound = false;
		}

	} else {
		Coord targetLocation;
		if(target.getObjPointer()->getItemID() == Structure_Refinery) {
		    targetLocation = target.getObjPointer()->getLocation() + Coord(2,0);
		} else {
            targetLocation = target.getObjPointer()->getClosestPoint(location);
		}
		Coord realLocation, realDestination;
		realLocation.x = lround(realX);
		realLocation.y = lround(realY);
		realDestination.x = targetLocation.x * BLOCKSIZE + BLOCKSIZE/2;
		realDestination.y = targetLocation.y * BLOCKSIZE + BLOCKSIZE/2;

		targetAngle = lround((double)NUM_ANGLES*dest_angle(location, destination)/256.0);
		if (targetAngle == 8) {
			targetAngle = 0;
		}

		targetDistance = distance_from(realLocation, realDestination);

		if (targetDistance <= BLOCKSIZE/8) {
			if (target.getObjPointer()->isAUnit()) {
				targetAngle = ((GroundUnit*)target.getObjPointer())->getAngle();
			}

			//if (drawnAngle == targetAngle) {
			    if(hasCargo()) {
			        if(target.getObjPointer()->isAStructure()) {
                        while(PickedUpUnitList.begin() != PickedUpUnitList.end()) {
                            deployUnit(*(PickedUpUnitList.begin()) );
                        }

                        setTarget(NULL);
                        setDestination(guardPoint);
                    }
			    } else {
                    pickupTarget();
			    }
			//}
		} else {
			setDestination(targetLocation);
		}
	}
}

void Carryall::giveCargo(UnitClass* newUnit)
{
	if(newUnit == NULL)
		return;

	booked = true;
	PickedUpUnitList.push_back(newUnit->getObjectID());

	newUnit->setPickedUp(this);

	if (getItemID() != Unit_Frigate)
		drawnFrame = 1;

	droppedOffCargo = false;
}

void Carryall::DoSetDeployStructure(Uint32 TargetObjectID) {
	if (target.getObjPointer() != NULL && target.getObjPointer()->isAStructure()) {
		if (goingToRepairYard) {
			((RepairYardClass*)target.getObjPointer())->unBook();
			goingToRepairYard = false;
			target.PointTo(NONE);
		} else if (target.getObjPointer()->getItemID() == Structure_Refinery) {
			((RefineryClass*)target.getObjPointer())->unBook();
			target.PointTo(NONE);
		}
	}

	if(TargetObjectID == NONE) {
		std::list<Uint32>::iterator iter;
		for(iter = PickedUpUnitList.begin() ; iter != PickedUpUnitList.end(); ++iter) {
			deployUnit(*iter);
		}
		PickedUpUnitList.clear();
	} else {
		ObjectClass* pTarget = currentGame->getObjectManager().getObject(TargetObjectID);
		if (pTarget->isAStructure())	{
			int targetID = pTarget->getItemID();
			if (targetID == Structure_RepairYard) {
				((RepairYardClass*) pTarget)->book();
				goingToRepairYard = true;
			} else if (targetID == Structure_Refinery) {
				((RefineryClass*) pTarget)->book();
			}
		}
	}

	pathList.clear();
}

void Carryall::pickupTarget()
{
    currentMaxSpeed = 0.0;
    setSpeeds();

	if(target.getObjPointer()->isAGroundUnit()) {

		if (target.getObjPointer()->hasATarget()
			|| ( ((GroundUnit*)target.getObjPointer())->getGuardPoint() != target.getObjPointer()->getLocation())
			|| ((GroundUnit*)target.getObjPointer())->isBadlyDamaged())	{

			if(((GroundUnit*)target.getObjPointer())->isBadlyDamaged() || (target.getObjPointer()->hasATarget() == false && target.getObjPointer()->getItemID() != Unit_Harvester))	{
				((GroundUnit*)target.getObjPointer())->DoRepair();
			}

			ObjectClass* newTarget = target.getObjPointer()->hasATarget() ? target.getObjPointer()->getTarget() : NULL;

			PickedUpUnitList.push_back(target.getObjectID());
			((GroundUnit*)target.getObjPointer())->setPickedUp(this);

			drawnFrame = 1;
			booked = true;

            if(newTarget && ((newTarget->getItemID() == Structure_Refinery)
                              || (newTarget->getItemID() == Structure_RepairYard)))
            {
                setTarget(newTarget);
                if(newTarget->getItemID() == Structure_Refinery) {
                    setDestination(target.getObjPointer()->getLocation() + Coord(2,0));
                } else {
                    setDestination(target.getObjPointer()->getClosestPoint(location));
                }
            } else if (((GroundUnit*)target.getObjPointer())->getDestination().x != INVALID_POS) {
                setDestination(((GroundUnit*)target.getObjPointer())->getDestination());
            }
            clearPath();
            recalculatePathTimer = 0;
		} else {
			((GroundUnit*)target.getObjPointer())->setAwaitingPickup(false);
			setTarget(NULL);
		}
	} else {
        // get unit from structure
        ObjectClass* pObject = target.getObjPointer();
        if(pObject->getItemID() == Structure_Refinery) {
            // get harvester
            ((RefineryClass*) pObject)->deployHarvester(this);
        } else if(pObject->getItemID() == Structure_RepairYard) {
            // get repaired unit
            ((RepairYardClass*) pObject)->deployRepairUnit(this);
        }
	}
}

void Carryall::setTarget(ObjectClass* newTarget)
{
	if (target.getObjPointer() != NULL
		&& targetFriendly
		&& target.getObjPointer()->isAGroundUnit()
		&& (((GroundUnit*)target.getObjPointer())->getCarrier() == this))
	{
		((GroundUnit*)target.getObjPointer())->bookCarrier(NULL);
	}

	UnitClass::setTarget(newTarget);

	if (target && targetFriendly && target.getObjPointer()->isAGroundUnit())
		((GroundUnit*)target.getObjPointer())->setAwaitingPickup(true);

	booked = target;
}

void Carryall::targeting()
{
	if (target)
		engageTarget();
}

void Carryall::turn()
{
	if(!moving) {
		int wantedAngle;

		if(target && (targetDistance <= currentGame->objectData.data[itemID].weaponrange)) {
			wantedAngle = targetAngle;
		} else {
			wantedAngle = nextSpotAngle;
		}

		if(wantedAngle != -1) {
			if(justStoppedMoving) {
				angle = wantedAngle;
				drawnAngle = lround(angle);
			} else {
				double	angleLeft = 0,
						angleRight = 0;

				if(angle > wantedAngle)	{
					angleRight = angle - wantedAngle;
					angleLeft = fabs(8-angle)+wantedAngle;
				} else if (angle < wantedAngle)	{
					angleRight = abs(8-wantedAngle) + angle;
					angleLeft = wantedAngle - angle;
				}

				if (angleLeft <= angleRight) {
					turnLeft();
				} else {
					turnRight();
				}
			}
		}
	}
}
bool Carryall::hasCargo()
{
	return (PickedUpUnitList.size() > 0);
}

void Carryall::findConstYard()
{
    double	closestYardDistance = 1000000;
    ConstructionYardClass* bestYard = NULL;

    RobustList<StructureClass*>::const_iterator iter;
    for(iter = structureList.begin(); iter != structureList.end(); ++iter) {
        StructureClass* tempStructure = *iter;

        if((tempStructure->getItemID() == Structure_ConstructionYard) && (tempStructure->getOwner() == owner)) {
            ConstructionYardClass* tempYard = ((ConstructionYardClass*) tempStructure);
            Coord closestPoint = tempYard->getClosestPoint(location);
            double tempDistance = distance_from(location, closestPoint);

            if(tempDistance < closestYardDistance) {
                closestYardDistance = tempDistance;
                bestYard = tempYard;
            }
        }
    }

    if(bestYard) {
        constYardPoint = bestYard->getClosestPoint(location);
    } else {
        constYardPoint = guardPoint;
    }

    static const Coord circles[][8] = {
                                        { Coord(-2,-6), Coord(3,-6), Coord(7,-2), Coord(7,3), Coord(3,7), Coord(-2,7), Coord(-6,3), Coord(-6,-2) },
                                        { Coord(-2,-6), Coord(-6,-2), Coord(-6,3), Coord(-2,7), Coord(3,7), Coord(7,3), Coord(7,-2), Coord(3,-6) },
                                        { Coord(-3,-8), Coord(4,-8), Coord(9,-3), Coord(9,4), Coord(4,9), Coord(-3,9), Coord(-8,4), Coord(-8,-3) },
                                        { Coord(-3,-8), Coord(-8,-3), Coord(-8,4), Coord(-3,9), Coord(4,9), Coord(9,4), Coord(9,-3), Coord(4,-8) }
                                      };

    const Coord* pUsedCircle = circles[currentGame->RandomGen.rand(0,3)];


    for(int i=0;i<8; i++) {
        flyPoints[i] = constYardPoint + pUsedCircle[i];
    }
}
