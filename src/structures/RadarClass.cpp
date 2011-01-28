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

#include <structures/RadarClass.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <House.h>

RadarClass::RadarClass(House* newOwner) : StructureClass(newOwner)
{
    RadarClass::init();

    setHealth(getMaxHealth());
}

RadarClass::RadarClass(Stream& stream) : StructureClass(stream)
{
    RadarClass::init();
}

void RadarClass::init()
{
    itemID = Structure_Radar;
	owner->incrementStructures(itemID);

	structureSize.x = 2;
	structureSize.y = 2;

	GraphicID = ObjPic_Radar;
	graphic = pGFXManager->getObjPic(GraphicID,getOwner()->getHouseID());
	imageW = graphic->w / 6;
	imageH = graphic->h;
	FirstAnimFrame = 2;
	LastAnimFrame = 5;
}

RadarClass::~RadarClass()
{
}


void RadarClass::destroy()
{
    House* owner = getOwner();

	StructureClass::destroy();

	if(!owner->hasRadar()) {
		owner->changeRadar(false);
	}
}
