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

#include <structures/WindTrapClass.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <House.h>

#include <GUI/ObjectInterfaces/WindTrapInterface.h>

WindTrapClass::WindTrapClass(House* newOwner) : StructureClass(newOwner)
{
    WindTrapClass::init();

	setHealth(getMaxHealth());
}

WindTrapClass::WindTrapClass(Stream& stream) : StructureClass(stream) {
    WindTrapClass::init();
}

void WindTrapClass::init() {
	itemID = Structure_WindTrap;
	owner->incrementStructures(itemID);

	structureSize.x = 2;
	structureSize.y = 2;

	GraphicID = ObjPic_Windtrap;
	graphic = pGFXManager->getObjPic(GraphicID,getOwner()->getHouseID());
	imageW = graphic->w / 4;
	imageH = graphic->h;
	FirstAnimFrame = 2;
	LastAnimFrame = 3;
}

WindTrapClass::~WindTrapClass()
{
}

ObjectInterface* WindTrapClass::GetInterfaceContainer() {
	if((pLocalHouse == owner) || (debug == true)) {
		return WindTrapInterface::Create(objectID);
	} else {
		return DefaultObjectInterface::Create(objectID);
	}
}

void WindTrapClass::setHealth(double newHealth) {
    int producedPowerBefore = getProducedPower();
    StructureClass::setHealth(newHealth);
    int producedPowerAfterwards = getProducedPower();

    owner->setProducedPower(owner->getProducedPower() - producedPowerBefore + producedPowerAfterwards);
}

int WindTrapClass::getProducedPower()
{
    int windTrapProducedPower = abs(currentGame->objectData.data[Structure_WindTrap].power);

    double ratio = getHealth() / ((double) getMaxHealth());
    return lround(ratio * windTrapProducedPower);
}
