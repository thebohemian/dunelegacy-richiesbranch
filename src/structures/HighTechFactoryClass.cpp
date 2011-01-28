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

#include <structures/HighTechFactoryClass.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <House.h>
#include <Game.h>

HighTechFactoryClass::HighTechFactoryClass(House* newOwner) : BuilderClass(newOwner)
{
    HighTechFactoryClass::init();

    setHealth(getMaxHealth());
}

HighTechFactoryClass::HighTechFactoryClass(Stream& stream) : BuilderClass(stream)
{
    HighTechFactoryClass::init();
}

void HighTechFactoryClass::init()
{
    itemID = Structure_HighTechFactory;
	owner->incrementStructures(itemID);

	structureSize.x = 3;
	structureSize.y = 2;

	GraphicID = ObjPic_HighTechFactory;
	graphic = pGFXManager->getObjPic(GraphicID,getOwner()->getHouseID());
	imageW = graphic->w / 8;
	imageH = graphic->h;
	FirstAnimFrame = 2;
	LastAnimFrame = 3;
}

HighTechFactoryClass::~HighTechFactoryClass()
{
}

int HighTechFactoryClass::MaxUpgradeLevel() const {
    return (((origHouse != HOUSE_HARKONNEN) && (currentGame->techLevel >= 7)) ? 1 : 0);
}

void HighTechFactoryClass::checkSelectionList()
{
	BuilderClass::checkSelectionList();

	std::list<BuildItem>::iterator iter = BuildList.begin();

	if(currentGame->techLevel >= 7 && curUpgradeLev >= 1 && owner->hasIX() && origHouse != HOUSE_HARKONNEN) {
		InsertItem(BuildList, iter, Unit_Ornithopter);
	} else {
		RemoveItem(BuildList, iter, Unit_Ornithopter);
	}

	InsertItem(BuildList, iter, Unit_Carryall);
}
