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

#include <structures/RepairYardClass.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <House.h>
#include <MapClass.h>
#include <SoundPlayer.h>

#include <units/Carryall.h>
#include <units/GroundUnit.h>

#include <GUI/ObjectInterfaces/RepairYardInterface.h>

RepairYardClass::RepairYardClass(House* newOwner) : StructureClass(newOwner)
{
    RepairYardClass::init();

    setHealth(getMaxHealth());
    bookings = 0;
	repairing = false;
}

RepairYardClass::RepairYardClass(Stream& stream) : StructureClass(stream)
{
    RepairYardClass::init();

	repairing = stream.readBool();
	repairUnit.load(stream);
	bookings = stream.readUint32();
}

void RepairYardClass::init()
{
    itemID = Structure_RepairYard;
	owner->incrementStructures(itemID);

	structureSize.x = 3;
	structureSize.y = 2;

	GraphicID = ObjPic_RepairYard;
	graphic = pGFXManager->getObjPic(GraphicID,getOwner()->getHouseID());
	imageW = graphic->w / 10;
	imageH = graphic->h;
	FirstAnimFrame = 2;
	LastAnimFrame = 5;
}

RepairYardClass::~RepairYardClass()
{
    if(repairing) {
		unBook();
		repairUnit.getUnitPointer()->destroy();
	}
}

void RepairYardClass::save(Stream& stream) const
{
	StructureClass::save(stream);

	stream.writeBool(repairing);
	repairUnit.save(stream);
	stream.writeUint32(bookings);
}


ObjectInterface* RepairYardClass::GetInterfaceContainer() {
	if((pLocalHouse == owner) || (debug == true)) {
		return RepairYardInterface::Create(objectID);
	} else {
		return DefaultObjectInterface::Create(objectID);
	}
}

void RepairYardClass::deployRepairUnit(Carryall* pCarryall)
{
	unBook();
	repairing = false;
	FirstAnimFrame = 2;
	LastAnimFrame = 5;

    UnitClass* pRepairUnit = repairUnit.getUnitPointer();
	if(pCarryall != NULL) {
	    pCarryall->giveCargo(pRepairUnit);
	    pCarryall->setTarget(NULL);
	    pCarryall->setDestination(pRepairUnit->getGuardPoint());
	} else {
        Coord deployPos = currentGameMap->findDeploySpot(pRepairUnit, location, destination, structureSize);
        pRepairUnit->deploy(deployPos);
        pRepairUnit->setTarget(NULL);
        pRepairUnit->setDestination(pRepairUnit->getLocation());
	}

	repairUnit.PointTo(NONE);

	if(getOwner() == pLocalHouse) {
		soundPlayer->playVoice(VehicleRepaired,getOwner()->getHouseID());
	}
}

void RepairYardClass::doSpecificStuff()
{
	if(repairing) {
		if(curAnimFrame < 6) {
			FirstAnimFrame = 6;
			LastAnimFrame = 9;
			curAnimFrame = 6;
		}
	} else {
		if(curAnimFrame > 5) {
			FirstAnimFrame = 2;
			LastAnimFrame = 5;
			curAnimFrame = 2;
		}
	}

	if(repairing == true) {
	    UnitClass* pRepairUnit = repairUnit.getUnitPointer();

		if (pRepairUnit->getHealth()*100/pRepairUnit->getMaxHealth() < 100) {
			if (owner->takeCredits(UNIT_REPAIRCOST) > 0) {
				pRepairUnit->addHealth();
			}
		} else if(((GroundUnit*)pRepairUnit)->isAwaitingPickup() == false) {
		    // find carryall
		    Carryall* pCarryall = NULL;
            if((pRepairUnit->getGuardPoint().x != INVALID_POS) && getOwner()->hasCarryalls())	{
                RobustList<UnitClass*>::const_iterator iter;
                for(iter = unitList.begin(); iter != unitList.end(); ++iter) {
                    UnitClass* unit = *iter;
                    if ((unit->getOwner() == owner) && (unit->getItemID() == Unit_Carryall)) {
                        if (((Carryall*)unit)->isRespondable() && !((Carryall*)unit)->isBooked()) {
                            pCarryall = (Carryall*)unit;
                        }
                    }
                }
            }

            if(pCarryall != NULL) {
                pCarryall->setTarget(this);
                pCarryall->clearPath();
                ((GroundUnit*)pRepairUnit)->bookCarrier(pCarryall);
                pRepairUnit->setTarget(NULL);
                pRepairUnit->setDestination(pRepairUnit->getGuardPoint());
            } else {
                deployRepairUnit();
            }
		} else if(((GroundUnit*)pRepairUnit)->getCarrier() == NULL) {
            deployRepairUnit();
		}
	}
}

void RepairYardClass::destroy() {
	StructureClass::destroy();
}
