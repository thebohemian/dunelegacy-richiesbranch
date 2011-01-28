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

#include <units/FremenClass.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <House.h>
#include <SoundPlayer.h>
FremenClass::FremenClass(House* newOwner) : InfantryClass(newOwner)
{
    FremenClass::init();

    setHealth(getMaxHealth());
}

FremenClass::FremenClass(Stream& stream) : InfantryClass(stream)
{
    FremenClass::init();
}

void FremenClass::init()
{
    itemID = Unit_Fremen;
    owner->incrementUnits(itemID);

	numWeapons = 1;
	bulletType = Bullet_SmallRocket;
	weaponReloadTime = 300;

	GraphicID = ObjPic_Trooper;
	graphic = pGFXManager->getObjPic(GraphicID,getOwner()->getHouseID());

	imageW = graphic->w/4;
	imageH = graphic->h/3;
}

FremenClass::~FremenClass()
{
}

bool FremenClass::canAttack(const ObjectClass* object) const
{
	if ((object != NULL)
		&& ((object->getOwner()->getTeam() != owner->getTeam()) || object->getItemID() == Unit_Sandworm)
		&& object->isVisible(getOwner()->getTeam()))
	{
		return true;
	}
	else
		return false;
}


void FremenClass::PlayAttackSound() {
	soundPlayer->playSoundAt(Sound_RocketSmall,location);
}

