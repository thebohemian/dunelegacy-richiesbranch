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

#include <structures/BarracksClass.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <House.h>

BarracksClass::BarracksClass(House* newOwner) : BuilderClass(newOwner)
{
    BarracksClass::init();

    setHealth(getMaxHealth());
}

BarracksClass::BarracksClass(Stream& stream) : BuilderClass(stream)
{
    BarracksClass::init();
}

void BarracksClass::init()
{
    itemID = Structure_Barracks;
	owner->incrementStructures(itemID);

	structureSize.x = 2;
	structureSize.y = 2;

	GraphicID = ObjPic_Barracks,
	graphic = pGFXManager->getObjPic(GraphicID,getOwner()->getHouseID());
	imageW = graphic->w / 4;
	imageH = graphic->h;
	FirstAnimFrame = 2;
	LastAnimFrame = 3;
}

BarracksClass::~BarracksClass()
{
}

void BarracksClass::checkSelectionList() {
	BuilderClass::checkSelectionList();

	std::list<BuildItem>::iterator iter = BuildList.begin();

	InsertItem(BuildList, iter, Unit_Soldier);
}

