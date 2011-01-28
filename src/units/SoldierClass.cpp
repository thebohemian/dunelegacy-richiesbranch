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

#include <units/SoldierClass.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <House.h>
#include <SoundPlayer.h>

SoldierClass::SoldierClass(House* newOwner) : InfantryClass(newOwner)
{
    SoldierClass::init();

    setHealth(getMaxHealth());
}

SoldierClass::SoldierClass(Stream& stream) : InfantryClass(stream)
{
    SoldierClass::init();
}

void SoldierClass::init()
{
	itemID = Unit_Soldier;
	owner->incrementUnits(itemID);

	numWeapons = 1;
	bulletType = Bullet_Inf_Gun;
	weaponReloadTime = 55;

	GraphicID = ObjPic_Infantry;
	graphic = pGFXManager->getObjPic(GraphicID,getOwner()->getHouseID());

	imageW = graphic->w/4;
	imageH = graphic->h/3;
}

SoldierClass::~SoldierClass()
{
}

bool SoldierClass::canAttack(const ObjectClass* object) const
{
	if ((object != NULL)
		&& (object->isAStructure()
			|| !object->isAFlyingUnit())
		&& ((object->getOwner()->getTeam() != owner->getTeam())
			|| object->getItemID() == Unit_Sandworm)
		&& object->isVisible(getOwner()->getTeam()))
	{
		return true;
	}
	else
		return false;
}


void SoldierClass::PlayAttackSound() {
	soundPlayer->playSoundAt(Sound_Gun,location);
}
