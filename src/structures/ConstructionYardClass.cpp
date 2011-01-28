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

#include <structures/ConstructionYardClass.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <House.h>
#include <Game.h>

ConstructionYardClass::ConstructionYardClass(House* newOwner) : BuilderClass(newOwner)
{
    ConstructionYardClass::init();

    setHealth(getMaxHealth());
}

ConstructionYardClass::ConstructionYardClass(Stream& stream) : BuilderClass(stream)
{
    ConstructionYardClass::init();
}

void ConstructionYardClass::init()
{
    itemID = Structure_ConstructionYard;
	owner->incrementStructures(itemID);

	structureSize.x = 2;
	structureSize.y = 2;

	GraphicID = ObjPic_ConstructionYard;
	graphic = pGFXManager->getObjPic(GraphicID,getOwner()->getHouseID());
	imageW = graphic->w / 4;
	imageH = graphic->h;

	FirstAnimFrame = 2;
	LastAnimFrame = 3;
}

ConstructionYardClass::~ConstructionYardClass()
{
}

int ConstructionYardClass::MaxUpgradeLevel() const
{
    if(currentGame->techLevel >= 6) {
        return 2;
	} else if(currentGame->techLevel >= 4) {
        return 1;
	} else {
        return 0;
	}
}

void ConstructionYardClass::checkSelectionList()
{
	//needed so selection list will have things in the right order when changed
	BuilderClass::checkSelectionList();

	std::list<BuildItem>::iterator iter = BuildList.begin();

	if((currentGame->techLevel >= 4) && (curUpgradeLev >= 1)) {
		InsertItem(BuildList, iter, Structure_Slab4);
	} else {
		RemoveItem(BuildList, iter, Structure_Slab4);
	}

	InsertItem(BuildList, iter, Structure_Slab1);

	if(owner->hasWindTrap() && owner->hasRefinery() && owner->hasStarPort() && (currentGame->techLevel >= 7)) {
		InsertItem(BuildList, iter, Structure_IX);
	} else {
		RemoveItem(BuildList, iter, Structure_IX);
	}

	if(owner->hasWindTrap() && owner->hasRefinery() && (currentGame->techLevel >= 6)) {
		InsertItem(BuildList, iter, Structure_StarPort);
	} else {
		RemoveItem(BuildList, iter, Structure_StarPort);
	}

    if(owner->hasWindTrap() && owner->hasRadar() && owner->hasLightFactory() && (currentGame->techLevel >= 5)) {
		InsertItem(BuildList, iter, Structure_HighTechFactory);
	} else {
		RemoveItem(BuildList, iter, Structure_HighTechFactory);
	}

	if(owner->hasWindTrap() && owner->hasRadar() && owner->hasLightFactory() && (currentGame->techLevel >= 4)) {
		InsertItem(BuildList, iter, Structure_HeavyFactory);
	} else {
		RemoveItem(BuildList, iter, Structure_HeavyFactory);
	}

	if(owner->hasWindTrap() && owner->hasRadar()  && (curUpgradeLev >= 2) && (currentGame->techLevel >= 6)) {
		InsertItem(BuildList, iter, Structure_RocketTurret);
	} else {
		RemoveItem(BuildList, iter, Structure_RocketTurret);
	}

	if(owner->hasWindTrap() && owner->hasRadar() && owner->hasLightFactory() && (currentGame->techLevel >= 5)) {
		InsertItem(BuildList, iter, Structure_RepairYard);
	} else {
		RemoveItem(BuildList, iter, Structure_RepairYard);
	}

	if(owner->hasWindTrap() && owner->hasRadar() && (currentGame->techLevel >= 5)) {
		InsertItem(BuildList, iter, Structure_GunTurret);
	} else {
		RemoveItem(BuildList, iter, Structure_GunTurret);
	}

	switch (getOwner()->getHouseID()) {
		case HOUSE_ATREIDES: {
			if(owner->hasWindTrap() && owner->hasRadar() && (currentGame->techLevel >= 2)) {
				InsertItem(BuildList, iter, Structure_Barracks);
			} else {
				RemoveItem(BuildList, iter, Structure_Barracks);
			}
		} break;

		case HOUSE_HARKONNEN: {
			if(owner->hasWindTrap() && owner->hasRadar() && (currentGame->techLevel >= 2)) {
				InsertItem(BuildList, iter, Structure_WOR);
			} else {
				RemoveItem(BuildList, iter, Structure_WOR);
			}
		} break;

        case HOUSE_ORDOS:
        case HOUSE_FREMEN:
		case HOUSE_SARDAUKAR:
		case HOUSE_MERCENARY:
		default: {
            if(owner->hasWindTrap() && owner->hasRadar() && owner->hasBarracks() && currentGame->techLevel >= 5) {
				InsertItem(BuildList, iter, Structure_WOR);
			} else {
				RemoveItem(BuildList, iter, Structure_WOR);
			}

			if(owner->hasWindTrap() && owner->hasRadar() && (currentGame->techLevel >= 2)) {
				InsertItem(BuildList, iter, Structure_Barracks);
			} else {
				RemoveItem(BuildList, iter, Structure_Barracks);
			}
		} break;
	}

    if((currentGame->techLevel >= 4) && owner->hasWindTrap() && owner->hasRadar()) {
	    InsertItem(BuildList, iter, Structure_Wall);
	} else {
	    RemoveItem(BuildList, iter, Structure_Wall);
	}

    int techLevelForLightFactory = ((getOwner()->getHouseID() == HOUSE_HARKONNEN) || (getOwner()->getHouseID() == HOUSE_SARDAUKAR)) ? 3 : 2;
    if(owner->hasWindTrap() && owner->hasRefinery() && (currentGame->techLevel >= techLevelForLightFactory)) {
        InsertItem(BuildList, iter, Structure_LightFactory);
    } else {
        RemoveItem(BuildList, iter, Structure_LightFactory);
    }

    if(owner->hasWindTrap() && owner->hasRefinery() && (currentGame->techLevel >= 2)) {
		InsertItem(BuildList, iter, Structure_Silo);
	} else {
		RemoveItem(BuildList, iter, Structure_Silo);
	}

    if(owner->hasWindTrap() && (currentGame->techLevel >= 2)) {
		InsertItem(BuildList, iter, Structure_Radar);
	} else {
		RemoveItem(BuildList, iter, Structure_Radar);
	}

	if(owner->hasWindTrap()) {
		InsertItem(BuildList, iter, Structure_Refinery);
	} else {
		RemoveItem(BuildList, iter, Structure_Refinery);
	}

	InsertItem(BuildList, iter, Structure_WindTrap);

	if(owner->hasStarPort() && (currentGame->techLevel >= 8)) {
		InsertItem(BuildList, iter, Structure_Palace);
	} else {
		RemoveItem(BuildList, iter, Structure_Palace);
	}
}
