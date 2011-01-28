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

#include <structures/SiloClass.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <House.h>

#include <GUI/ObjectInterfaces/RefineryAndSiloInterface.h>

SiloClass::SiloClass(House* newOwner) : StructureClass(newOwner)
{
    SiloClass::init();

    setHealth(getMaxHealth());
}

SiloClass::SiloClass(Stream& stream) : StructureClass(stream)
{
    SiloClass::init();
}

void SiloClass::init()
{
	itemID = Structure_Silo;
	owner->incrementStructures(itemID);

	structureSize.x = 2;
	structureSize.y = 2;

	GraphicID = ObjPic_Silo;
	graphic = pGFXManager->getObjPic(GraphicID,getOwner()->getHouseID());
	imageW = graphic->w / 4;
	imageH = graphic->h;
	FirstAnimFrame = 2;
	LastAnimFrame = 3;
}

SiloClass::~SiloClass()
{
}

ObjectInterface* SiloClass::GetInterfaceContainer() {
	if((pLocalHouse == owner) || (debug == true)) {
		return RefineryAndSiloInterface::Create(objectID);
	} else {
		return DefaultObjectInterface::Create(objectID);
	}
}

