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

#include <structures/RefineryClass.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <House.h>
#include <SoundPlayer.h>
#include <MapClass.h>

#include <units/UnitClass.h>
#include <units/HarvesterClass.h>
#include <units/Carryall.h>

#include <GUI/ObjectInterfaces/RefineryAndSiloInterface.h>

RefineryClass::RefineryClass(House* newOwner) : StructureClass(newOwner)
{
    RefineryClass::init();

    setHealth(getMaxHealth());

	extractingSpice = false;
    bookings = 0;

	firstRun = true;

    FirstAnimFrame = 2;
	LastAnimFrame = 3;
}

RefineryClass::RefineryClass(Stream& stream) : StructureClass(stream)
{
    RefineryClass::init();

	extractingSpice = stream.readBool();
	harvester.load(stream);
	bookings = stream.readUint32();

	if(extractingSpice) {
		FirstAnimFrame = 8;
		LastAnimFrame = 9;
		curAnimFrame = 8;
	} else if(bookings == 0) {
		stopAnimate();
	} else {
		startAnimate();
	}

	firstRun = false;
}
void RefineryClass::init()
{
    itemID = Structure_Refinery;
	owner->incrementStructures(itemID);

	structureSize.x = 3;
	structureSize.y = 2;

	GraphicID = ObjPic_Refinery;
	graphic = pGFXManager->getObjPic(GraphicID,getOwner()->getHouseID());
	imageW = graphic->w / 10;
	imageH = graphic->h;
}

RefineryClass::~RefineryClass()
{
    if (extractingSpice && harvester) {
		if(harvester.getUnitPointer() != NULL)
			harvester.getUnitPointer()->destroy();
		harvester.PointTo(NONE);
	}
}

void RefineryClass::save(Stream& stream) const
{
	StructureClass::save(stream);

    stream.writeBool(extractingSpice);
	harvester.save(stream);
	stream.writeUint32(bookings);
}

ObjectInterface* RefineryClass::GetInterfaceContainer() {
	if((pLocalHouse == owner) || (debug == true)) {
		return RefineryAndSiloInterface::Create(objectID);
	} else {
		return DefaultObjectInterface::Create(objectID);
	}
}

void RefineryClass::assignHarvester(HarvesterClass* newHarvester)
{
	extractingSpice = true;
	harvester.PointTo(newHarvester);
	drawnAngle = 1;
	FirstAnimFrame = 8;
	LastAnimFrame = 9;
	curAnimFrame = 8;
}



void RefineryClass::deployHarvester(Carryall* pCarryall)
{
	unBook();
	drawnAngle = 0;
	extractingSpice = false;

	if(firstRun) {
		if (getOwner() == pLocalHouse)
		{
			soundPlayer->playVoice(HarvesterDeployed,getOwner()->getHouseID());
		}
	}

	firstRun = false;

    HarvesterClass* pHarvester = (HarvesterClass*) harvester.getObjPointer();
	if(pCarryall != NULL) {
	    pCarryall->giveCargo(pHarvester);
	    pCarryall->setTarget(NULL);
	    pCarryall->setDestination(pHarvester->getGuardPoint());
	} else {
        Coord deployPos = currentGameMap->findDeploySpot(pHarvester, location, destination, structureSize);
        pHarvester->deploy(deployPos);
	}

	if(bookings == 0) {
        stopAnimate();
	} else {
        startAnimate();
	}
}

void RefineryClass::startAnimate()
{
	if(extractingSpice == false) {
		FirstAnimFrame = 2;
		LastAnimFrame = 7;
		curAnimFrame = 2;
	}
}

void RefineryClass::stopAnimate()
{
    FirstAnimFrame = 2;
	LastAnimFrame = 3;
	curAnimFrame = 2;
}

void RefineryClass::doSpecificStuff()
{
	if(extractingSpice) {
	    HarvesterClass* pHarvester = (HarvesterClass*) harvester.getObjPointer();

		if(pHarvester->getAmountOfSpice() > 0.0) {
			owner->addCredits(pHarvester->extractSpice(), true);
		} else if(pHarvester->isAwaitingPickup() == false) {
		    // find carryall
		    Carryall* pCarryall = NULL;
            if((pHarvester->getGuardPoint().x != INVALID_POS) && getOwner()->hasCarryalls())	{
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
                pHarvester->bookCarrier(pCarryall);
                pHarvester->setTarget(NULL);
                pHarvester->setDestination(pHarvester->getGuardPoint());
            } else {
                deployHarvester();
            }
		} else if(pHarvester->getCarrier() == NULL) {
		    deployHarvester();
		}
	}
}

void RefineryClass::destroy()
{
	StructureClass::destroy();
}
