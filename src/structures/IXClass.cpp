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

#include <structures/IXClass.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <House.h>

IXClass::IXClass(House* newOwner) : StructureClass(newOwner)
{
    IXClass::init();

    setHealth(getMaxHealth());
}

IXClass::IXClass(Stream& stream) : StructureClass(stream)
{
    IXClass::init();
}

void IXClass::init()
{
	itemID = Structure_IX;
	owner->incrementStructures(itemID);

	structureSize.x = 2;
	structureSize.y = 2;

	GraphicID = ObjPic_IX;
	graphic = pGFXManager->getObjPic(GraphicID,getOwner()->getHouseID());
	imageW = graphic->w / 4;
	imageH = graphic->h;
	FirstAnimFrame = 2;
	LastAnimFrame = 3;
}

IXClass::~IXClass()
{
}

