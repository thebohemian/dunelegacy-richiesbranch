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

#include <units/SiegeTankClass.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <House.h>
#include <Game.h>
#include <MapClass.h>
#include <Explosion.h>
#include <ScreenBorder.h>
#include <SoundPlayer.h>

const Coord	SiegeTankClass::TurretOffset[] = {  Coord(2, -4),
                                                Coord(1, -5),
                                                Coord(0, -5),
                                                Coord(-1, -5),
                                                Coord(-2, -4),
                                                Coord(-1, -2),
                                                Coord(-1, -4),
                                                Coord(1, -2)
};

SiegeTankClass::SiegeTankClass(House* newOwner) : TankBaseClass(newOwner)
{
    SiegeTankClass::init();

    setHealth(getMaxHealth());
}

SiegeTankClass::SiegeTankClass(Stream& stream) : TankBaseClass(stream)
{
    SiegeTankClass::init();
}
void SiegeTankClass::init()
{
    itemID = Unit_SiegeTank;
    owner->incrementUnits(itemID);

	numWeapons = 2;
	bulletType = Bullet_Shell;
	weaponReloadTime = 150;

	GraphicID = ObjPic_Siegetank_Base;
	graphic = pGFXManager->getObjPic(GraphicID,getOwner()->getHouseID());
	GunGraphicID = ObjPic_Siegetank_Gun;
	turretGraphic = pGFXManager->getObjPic(GunGraphicID,getOwner()->getHouseID());

	imageW = graphic->w/NUM_ANGLES;
	imageH = graphic->h;
}

SiegeTankClass::~SiegeTankClass()
{
}

void SiegeTankClass::blitToScreen()
{
	SDL_Rect dest, source;
	dest.x = getDrawnX();
	dest.y = getDrawnY();
	dest.w = source.w = imageW;
	dest.h = source.h = imageH;
	source.y = 0;

    source.x = drawnAngle*imageW;

    SDL_BlitSurface(graphic, &source, screen, &dest);	//blit base
    source.x = drawnTurretAngle*source.w;
    source.y = 0;
    dest.x = getDrawnX() + TurretOffset[drawnTurretAngle].x;
    dest.y = getDrawnY() + TurretOffset[drawnTurretAngle].y;
    SDL_BlitSurface(turretGraphic, &source, screen, &dest);	//blit turret
    if(isBadlyDamaged())
        drawSmoke(screenborder->world2screenX(realX), screenborder->world2screenY(realY));
}

void SiegeTankClass::destroy() {
    if(currentGameMap->cellExists(location) && isVisible()) {
        Coord realPos((short)realX, (short)realY);
        Uint32 explosionID = currentGame->RandomGen.getRandOf(2,Explosion_Medium1, Explosion_Medium2);
        currentGame->getExplosionList().push_back(new Explosion(explosionID, realPos, owner->getHouseID()));

        if(isVisible(getOwner()->getTeam()))
            soundPlayer->playSoundAt(Sound_ExplosionLarge,location);
    }

    TankBaseClass::destroy();
}

void SiegeTankClass::PlayAttackSound() {
	soundPlayer->playSoundAt(Sound_ExplosionSmall,location);
}

