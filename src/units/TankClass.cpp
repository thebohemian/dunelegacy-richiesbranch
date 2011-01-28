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

#include <units/TankClass.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <House.h>
#include <Game.h>
#include <MapClass.h>
#include <Explosion.h>
#include <ScreenBorder.h>
#include <SoundPlayer.h>


TankClass::TankClass(House* newOwner) : TankBaseClass(newOwner)
{
    TankClass::init();

    setHealth(getMaxHealth());
}

TankClass::TankClass(Stream& stream) : TankBaseClass(stream)
{
    TankClass::init();
}

void TankClass::init()
{
	itemID = Unit_Tank;
	owner->incrementUnits(itemID);

	numWeapons = 1;
	bulletType = Bullet_Shell;
	weaponReloadTime = 150;

	GraphicID = ObjPic_Tank_Base;
	graphic = pGFXManager->getObjPic(GraphicID,getOwner()->getHouseID());
	GunGraphicID = ObjPic_Tank_Gun;
	turretGraphic = pGFXManager->getObjPic(GunGraphicID,getOwner()->getHouseID());

	imageW = graphic->w/NUM_ANGLES;
	imageH = graphic->h;
}

TankClass::~TankClass()
{
}


void TankClass::blitToScreen()
{
	SDL_Rect dest, source;
	dest.x = getDrawnX();
	dest.y = getDrawnY();
	dest.w = source.w = imageW;
	dest.h = source.h = imageH;
	source.y = 0;
    source.x = drawnAngle*imageW;

    SDL_BlitSurface(graphic, &source, screen, &dest);	//blit base

    if(drawnTurretAngle < 0 || drawnTurretAngle > 7) {
        fprintf(stderr,"Invalid drawnTurretAngle(%d)\n", drawnTurretAngle);
    }

    source.x = drawnTurretAngle*source.w;
    source.y = 0;
    dest.x = getDrawnX();
    dest.y = getDrawnY();

    SDL_BlitSurface(turretGraphic, &source, screen, &dest);	//blit turret

    if(isBadlyDamaged())
        drawSmoke(screenborder->world2screenX(realX), screenborder->world2screenY(realY));
}

void TankClass::destroy() {
    if(currentGameMap->cellExists(location) && isVisible()) {
        Coord realPos((short)realX, (short)realY);
        Uint32 explosionID = currentGame->RandomGen.getRandOf(3,Explosion_Medium1, Explosion_Medium2,Explosion_Flames);
        currentGame->getExplosionList().push_back(new Explosion(explosionID, realPos, owner->getHouseID()));

        if(isVisible(getOwner()->getTeam()))
            soundPlayer->playSoundAt(Sound_ExplosionMedium,location);
    }

    TankBaseClass::destroy();
}

void TankClass::PlayAttackSound() {
	soundPlayer->playSoundAt(Sound_ExplosionSmall,location);
}
