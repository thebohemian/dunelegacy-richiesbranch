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

#include <structures/LightFactoryClass.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <House.h>
#include <Game.h>

LightFactoryClass::LightFactoryClass(House* newOwner) : BuilderClass(newOwner)
{
    LightFactoryClass::init();

    setHealth(getMaxHealth());
}

LightFactoryClass::LightFactoryClass(Stream& stream) : BuilderClass(stream)
{
    LightFactoryClass::init();
}

void LightFactoryClass::init()
{
    itemID = Structure_LightFactory;
	owner->incrementStructures(itemID);

	structureSize.x = 2;
	structureSize.y = 2;

	GraphicID = ObjPic_LightFactory;
	graphic = pGFXManager->getObjPic(GraphicID,getOwner()->getHouseID());
	imageW = graphic->w / 6;
	imageH = graphic->h;
	FirstAnimFrame = 2;
	LastAnimFrame = 3;
}

LightFactoryClass::~LightFactoryClass()
{
}

int LightFactoryClass::MaxUpgradeLevel() const
{
    return (origHouse != HOUSE_HARKONNEN && currentGame->techLevel >= 3) ? 1 : 0;
}

void LightFactoryClass::checkSelectionList()
{
	BuilderClass::checkSelectionList();

	std::list<BuildItem>::iterator iter = BuildList.begin();

	switch(origHouse) {
		case HOUSE_ORDOS: {
            if(curUpgradeLev >= 1) {
				InsertItem(BuildList, iter, Unit_Quad);
			} else {
				RemoveItem(BuildList, iter, Unit_Quad);
			}

			InsertItem(BuildList, iter, Unit_Raider);
			RemoveItem(BuildList, iter, Unit_Trike);
		} break;

		case HOUSE_HARKONNEN: {
			InsertItem(BuildList, iter, Unit_Quad);
			RemoveItem(BuildList, iter, Unit_Trike);
			RemoveItem(BuildList, iter, Unit_Raider);
		} break;

		case HOUSE_ATREIDES:
        default: {
			if(curUpgradeLev >= 1) {
				InsertItem(BuildList, iter, Unit_Quad);
			} else {
				RemoveItem(BuildList, iter, Unit_Quad);
			}
			InsertItem(BuildList, iter, Unit_Trike);
			RemoveItem(BuildList, iter, Unit_Raider);
		} break;
	};
}

void LightFactoryClass::doSpecificStuff()
{
    if(deployTimer > 0) {
	    FirstAnimFrame = 4;
        LastAnimFrame = 5;
	} else {
        FirstAnimFrame = 2;
        LastAnimFrame = 3;
	}
}
