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

#include <units/TrikeClass.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <House.h>
#include <Game.h>
#include <MapClass.h>
#include <Explosion.h>
#include <SoundPlayer.h>

TrikeClass::TrikeClass(House* newOwner) : GroundUnit(newOwner)
{
    TrikeClass::init();

    setHealth(getMaxHealth());
}

TrikeClass::TrikeClass(Stream& stream) : GroundUnit(stream)
{
    TrikeClass::init();
}

void TrikeClass::init()
{
    itemID = Unit_Trike;
    owner->incrementUnits(itemID);

	numWeapons = 2;
	bulletType = Bullet_Gun;
	weaponReloadTime = 160;

	GraphicID = ObjPic_Trike;
	graphic = pGFXManager->getObjPic(GraphicID,getOwner()->getHouseID());

	imageW = graphic->w/NUM_ANGLES;
	imageH = graphic->h;
}

TrikeClass::~TrikeClass()
{
}

void TrikeClass::destroy() {
    if(currentGameMap->cellExists(location) && isVisible()) {
        Coord realPos((short)realX, (short)realY);
        currentGame->getExplosionList().push_back(new Explosion(Explosion_SmallUnit, realPos, owner->getHouseID()));

        if(isVisible(getOwner()->getTeam()))
            soundPlayer->playSoundAt(Sound_ExplosionSmall,location);
    }

    GroundUnit::destroy();
}

void TrikeClass::PlayAttackSound() {
	soundPlayer->playSoundAt(Sound_MachineGun,location);
}
