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

#include <units/DeviatorClass.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <House.h>
#include <Game.h>
#include <MapClass.h>
#include <Explosion.h>
#include <ScreenBorder.h>
#include <SoundPlayer.h>

const Coord DeviatorClass::TurretOffset[] = {   Coord(3, 0),
                                                Coord(3, 1),
                                                Coord(3, 1),
                                                Coord(3, 1),
                                                Coord(3, 0),
                                                Coord(3, 1),
                                                Coord(3, 1),
                                                Coord(3, 1)
                                            };

DeviatorClass::DeviatorClass(House* newOwner) : TrackedUnit(newOwner)
{
    DeviatorClass::init();

    setHealth(getMaxHealth());
}

DeviatorClass::DeviatorClass(Stream& stream) : TrackedUnit(stream)
{
    DeviatorClass::init();
}

void DeviatorClass::init()
{
    itemID = Unit_Deviator;
    owner->incrementUnits(itemID);

	GraphicID = ObjPic_Tank_Base;
	GunGraphicID = ObjPic_Launcher_Gun;
	graphic = pGFXManager->getObjPic(GraphicID,getOwner()->getHouseID());
	turretGraphic = pGFXManager->getObjPic(GunGraphicID,getOwner()->getHouseID());

	imageW = graphic->w/NUM_ANGLES;
	imageH = graphic->h;

	numWeapons = 1;
	bulletType = Bullet_DRocket;
	weaponReloadTime = 285;
}

DeviatorClass::~DeviatorClass()
{
}

void DeviatorClass::blitToScreen()
{
	SDL_Rect dest, source;
	dest.x = getDrawnX();
	dest.y = getDrawnY();
	dest.w = source.w = imageW;
	dest.h = source.h = imageH;
	source.y = 0;
    source.x = drawnAngle*imageW;

    SDL_BlitSurface(graphic, &source, screen, &dest);	//blit base
    source.w = turretGraphic->w/NUM_ANGLES;
    source.h = turretGraphic->h;
    source.x = drawnAngle*source.w;
    source.y = 0;
    dest.x = getDrawnX() + TurretOffset[drawnAngle].x;
    dest.y = getDrawnY() + TurretOffset[drawnAngle].y;
    SDL_BlitSurface(turretGraphic, &source, screen, &dest);	//blit turret
    if(isBadlyDamaged())
        drawSmoke(screenborder->world2screenX(realX), screenborder->world2screenY(realY));
}

void DeviatorClass::destroy() {
    if(currentGameMap->cellExists(location) && isVisible()) {
        Coord realPos((short)realX, (short)realY);
        Uint32 explosionID = currentGame->RandomGen.getRandOf(3,Explosion_Medium1, Explosion_Medium2,Explosion_Flames);
        currentGame->getExplosionList().push_back(new Explosion(explosionID, realPos, owner->getHouseID()));

        if(isVisible(getOwner()->getTeam()))
            soundPlayer->playSoundAt(Sound_ExplosionMedium,location);
    }

    TrackedUnit::destroy();
}

bool DeviatorClass::canAttack(const ObjectClass* object) const
{
	if ((object != NULL) &&	!object->isAStructure()
		&& ((object->getOwner()->getTeam() != owner->getTeam()) || object->getItemID() == Unit_Sandworm)
		&& object->isVisible(getOwner()->getTeam()))
		return true;
	else
		return false;
}

void DeviatorClass::PlayAttackSound() {
	soundPlayer->playSoundAt(Sound_Rocket,location);
}
