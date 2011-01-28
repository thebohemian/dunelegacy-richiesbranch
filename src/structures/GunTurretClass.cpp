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

#include <structures/GunTurretClass.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <FileClasses/SFXManager.h>
#include <House.h>

GunTurretClass::GunTurretClass(House* newOwner) : TurretClass(newOwner)
{
    GunTurretClass::init();

    setHealth(getMaxHealth());
}

GunTurretClass::GunTurretClass(Stream& stream) : TurretClass(stream)
{
    GunTurretClass::init();
}

void GunTurretClass::init() {
    itemID = Structure_GunTurret;
	owner->incrementStructures(itemID);

	attackSound = Sound_Gun;
	bulletType = Bullet_Shell;

    weaponReloadTime = 150;

	GraphicID = ObjPic_GunTurret;
	graphic = pGFXManager->getObjPic(ObjPic_GunTurret,getOwner()->getHouseID());
	imageW = graphic->w / 10;
	imageH = graphic->h;
	curAnimFrame = FirstAnimFrame = LastAnimFrame = ((10-drawnAngle) % 8) + 2;
}

GunTurretClass::~GunTurretClass()
{
	;
}
