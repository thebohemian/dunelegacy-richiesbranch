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

#include <structures/WORClass.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <House.h>

WORClass::WORClass(House* newOwner) : BuilderClass(newOwner)
{
    WORClass::init();

    setHealth(getMaxHealth());
}

WORClass::WORClass(Stream& stream) : BuilderClass(stream)
{
    WORClass::init();
}

void WORClass::init()
{
    itemID = Structure_WOR;
	owner->incrementStructures(itemID);

	structureSize.x = 2;
	structureSize.y = 2;

	GraphicID = ObjPic_WOR;
	graphic = pGFXManager->getObjPic(GraphicID,getOwner()->getHouseID());
	imageW = graphic->w / 4;
	imageH = graphic->h;
	FirstAnimFrame = 2;
	LastAnimFrame = 3;
}

WORClass::~WORClass()
{
}

void WORClass::checkSelectionList()
{
	BuilderClass::checkSelectionList();

	std::list<BuildItem>::iterator iter = BuildList.begin();

	switch(owner->getHouseID()) {
		case(HOUSE_ATREIDES):
		case(HOUSE_ORDOS):
		case(HOUSE_HARKONNEN):
		case(HOUSE_MERCENARY): {
			InsertItem(BuildList, iter, Unit_Trooper);
		} break;

		case(HOUSE_FREMEN): {
			InsertItem(BuildList, iter, Unit_Fremen);
		} break;

		case(HOUSE_SARDAUKAR): {
			InsertItem(BuildList, iter, Unit_Sardaukar);
		} break;

		default: {
		} break;
	}
}
