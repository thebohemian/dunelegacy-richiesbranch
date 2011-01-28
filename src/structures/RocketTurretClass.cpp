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

#include <structures/RocketTurretClass.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <FileClasses/SFXManager.h>
#include <House.h>
#include <Game.h>

RocketTurretClass::RocketTurretClass(House* newOwner) : TurretClass(newOwner)
{
    RocketTurretClass::init();

    setHealth(getMaxHealth());
}

RocketTurretClass::RocketTurretClass(Stream& stream) : TurretClass(stream)
{
    RocketTurretClass::init();
}

void RocketTurretClass::init() {
	itemID = Structure_RocketTurret;
	owner->incrementStructures(itemID);

	attackSound = Sound_Rocket;
	bulletType = Bullet_Rocket;

	weaponReloadTime = 150;

	GraphicID = ObjPic_RocketTurret;
	graphic = pGFXManager->getObjPic(GraphicID,getOwner()->getHouseID());
	imageW = graphic->w / 10;
	imageH = graphic->h;
	curAnimFrame = FirstAnimFrame = LastAnimFrame = ((10-drawnAngle) % 8) + 2;
}

RocketTurretClass::~RocketTurretClass()
{
}

void RocketTurretClass::doSpecificStuff()
{
	if(getOwner()->hasPower() || (((currentGame->gameType == GAMETYPE_CAMPAIGN) || (currentGame->gameType == GAMETYPE_SKIRMISH)) && getOwner()->isAI())) {
		TurretClass::doSpecificStuff();
	}
}

bool RocketTurretClass::canAttack(const ObjectClass* object) const
{
	if ((object != NULL)
		&& ((object->getOwner()->getTeam() != owner->getTeam()) || object->getItemID() == Unit_Sandworm)
		&& object->isVisible(getOwner()->getTeam()))
		return true;
	else
		return false;
}
