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

#include <structures/HeavyFactoryClass.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <House.h>
#include <Game.h>

HeavyFactoryClass::HeavyFactoryClass(House* newOwner) : BuilderClass(newOwner)
{
    HeavyFactoryClass::init();

    setHealth(getMaxHealth());
}

HeavyFactoryClass::HeavyFactoryClass(Stream& stream) : BuilderClass(stream)
{
    HeavyFactoryClass::init();
}

void HeavyFactoryClass::init()
{
   	itemID = Structure_HeavyFactory;
	owner->incrementStructures(itemID);

	structureSize.x = 3;
	structureSize.y = 2;

	GraphicID = ObjPic_HeavyFactory;
	graphic = pGFXManager->getObjPic(GraphicID,getOwner()->getHouseID());
	imageW = graphic->w / 8;
	imageH = graphic->h;
	FirstAnimFrame = 2;
	LastAnimFrame = 3;
}

HeavyFactoryClass::~HeavyFactoryClass()
{
}

void HeavyFactoryClass::buildRandom() {
	if((AllowedToUpgrade() == true) && (CurrentUpgradeLevel() < MaxUpgradeLevel())
		&& (GetUpgradeCost() <= owner->getCredits())) {
			DoUpgrade();
			return;
	}

	int Item2Produce = ItemID_Invalid;

	do {
		int randNum = currentGame->RandomGen.rand(0, getNumSelections()-1);
		int i = 0;
		std::list<BuildItem>::iterator iter;
		for(iter = BuildList.begin(); iter != BuildList.end(); ++iter) {
			if(i == randNum) {
				Item2Produce = iter->ItemID;
				break;
			}
		}
	} while((Item2Produce == Unit_Harvester) || (Item2Produce == Unit_MCV));

	DoProduceItem(Item2Produce);
}

int HeavyFactoryClass::MaxUpgradeLevel() const
{
    if(origHouse == HOUSE_ORDOS) {
	    // Ordos and Mercenary can not produce launchers => only 1 upgrade
        if(currentGame->techLevel >= 6) {
            return 2;
        } else {
            return 1;
        }
	} else {
        // All other houses have 2 upgrades
        if(currentGame->techLevel >= 6) {
            return 3;
        } else if (currentGame->techLevel >= 5) {
            return 2;
        } else {
            return 1;
        }
	}
}

void HeavyFactoryClass::checkSelectionList()
{
	BuilderClass::checkSelectionList();

	std::list<BuildItem>::iterator iter = BuildList.begin();

	switch(origHouse) {
		case HOUSE_ATREIDES: {
			if(getOwner()->hasIX()) {
				InsertItem(BuildList, iter, Unit_SonicTank);
			} else {
				RemoveItem(BuildList, iter, Unit_SonicTank);
			}

            RemoveItem(BuildList, iter, Unit_Deviator);
            RemoveItem(BuildList, iter, Unit_Devastator);

		} break;

		case HOUSE_ORDOS: {
			if(getOwner()->hasIX()) {
				InsertItem(BuildList, iter, Unit_Deviator);
            } else {
				RemoveItem(BuildList, iter, Unit_Deviator);
			}

            RemoveItem(BuildList, iter, Unit_SonicTank);
			RemoveItem(BuildList, iter, Unit_Devastator);

		} break;

		case HOUSE_HARKONNEN: {
			if(getOwner()->hasIX()) {
				InsertItem(BuildList, iter, Unit_Devastator);
			} else {
				RemoveItem(BuildList, iter, Unit_Devastator);
			}

            RemoveItem(BuildList, iter, Unit_SonicTank);
			RemoveItem(BuildList, iter, Unit_Deviator);

		} break;

        case HOUSE_FREMEN:
        case HOUSE_SARDAUKAR:
        case HOUSE_MERCENARY:
		default: {
			if(getOwner()->hasIX()) {
			    InsertItem(BuildList, iter, Unit_SonicTank);
				InsertItem(BuildList, iter, Unit_Devastator);
			} else {
			    RemoveItem(BuildList, iter, Unit_SonicTank);
				RemoveItem(BuildList, iter, Unit_Devastator);
			}
		} break;
	}

	if(origHouse == HOUSE_ORDOS) {
	    RemoveItem(BuildList, iter, Unit_Launcher);

	    // Ordos and Mercenary can not produce launchers => only 1 upgrade
        if(currentGame->techLevel >= 7 && curUpgradeLev >= 2) {
            InsertItem(BuildList, iter, Unit_SiegeTank);
        } else {
            RemoveItem(BuildList, iter, Unit_SiegeTank);
        }
	} else {
        if(currentGame->techLevel >= 5 && curUpgradeLev >= 2) {
            InsertItem(BuildList, iter, Unit_Launcher);
        } else {
            RemoveItem(BuildList, iter, Unit_Launcher);
        }

        // All other houses have 2 upgrades
        if(currentGame->techLevel >= 6 && curUpgradeLev >= 3) {
            InsertItem(BuildList, iter, Unit_SiegeTank);
        } else {
            RemoveItem(BuildList, iter, Unit_SiegeTank);
        }
	}

	InsertItem(BuildList, iter, Unit_Tank);

	if(curUpgradeLev >= 1) {
        InsertItem(BuildList, iter, Unit_MCV);
	} else {
	    RemoveItem(BuildList, iter, Unit_MCV);
	}

	InsertItem(BuildList, iter, Unit_Harvester);
}

void HeavyFactoryClass::doSpecificStuff()
{
	if(deployTimer > 0) {
	    FirstAnimFrame = 4;
        LastAnimFrame = 5;
	} else {
        FirstAnimFrame = 2;
        LastAnimFrame = 3;
	}
}
