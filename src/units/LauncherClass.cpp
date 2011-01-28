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

#include <units/LauncherClass.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <House.h>
#include <Game.h>
#include <MapClass.h>
#include <Explosion.h>
#include <ScreenBorder.h>
#include <SoundPlayer.h>

const Coord LauncherClass::TurretOffset[] = {   Coord(3, 0),
                                                Coord(3, 1),
                                                Coord(3, 1),
                                                Coord(3, 1),
                                                Coord(3, 0),
                                                Coord(3, 1),
                                                Coord(3, 1),
                                                Coord(3, 1)
                                            };

LauncherClass::LauncherClass(House* newOwner) : TrackedUnit(newOwner)
{
    LauncherClass::init();

    setHealth(getMaxHealth());
}

LauncherClass::LauncherClass(Stream& stream) : TrackedUnit(stream)
{
    LauncherClass::init();
}
void LauncherClass::init()
{
    itemID = Unit_Launcher;
    owner->incrementUnits(itemID);

	GraphicID = ObjPic_Tank_Base;
	GunGraphicID = ObjPic_Launcher_Gun;
	graphic = pGFXManager->getObjPic(GraphicID,getOwner()->getHouseID());
	turretGraphic = pGFXManager->getObjPic(GunGraphicID,getOwner()->getHouseID());

	imageW = graphic->w/NUM_ANGLES;
	imageH = graphic->h;

	numWeapons = 2;
	bulletType = Bullet_Rocket;
	weaponReloadTime = 285;
}

LauncherClass::~LauncherClass()
{
}

void LauncherClass::blitToScreen()
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

void LauncherClass::destroy() {
    if(currentGameMap->cellExists(location) && isVisible()) {
        Coord realPos((short)realX, (short)realY);
        Uint32 explosionID = currentGame->RandomGen.getRandOf(3,Explosion_Medium1, Explosion_Medium2,Explosion_Flames);
        currentGame->getExplosionList().push_back(new Explosion(explosionID, realPos, owner->getHouseID()));

        if(isVisible(getOwner()->getTeam()))
            soundPlayer->playSoundAt(Sound_ExplosionMedium,location);
    }

    TrackedUnit::destroy();
}

bool LauncherClass::canAttack(const ObjectClass* object) const
{
	return ((object != NULL)
			&& ((object->getOwner()->getTeam() != owner->getTeam()) || object->getItemID() == Unit_Sandworm)
			&& object->isVisible(getOwner()->getTeam()));
}

void LauncherClass::PlayAttackSound() {
	soundPlayer->playSoundAt(Sound_Rocket,location);
}
