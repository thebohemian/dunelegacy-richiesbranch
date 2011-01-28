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

#include <units/DevastatorClass.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <House.h>
#include <Game.h>
#include <ScreenBorder.h>
#include <Explosion.h>
#include <MapClass.h>
#include <SoundPlayer.h>


Coord	devastatorTurretOffset[] =
{
	Coord(2, -4),
	Coord(-1, -3),
	Coord(0, -4),
	Coord(1, -3),
	Coord(-2, -4),
	Coord(0, -3),
	Coord(-1, -3),
	Coord(0, -3)
};


DevastatorClass::DevastatorClass(House* newOwner) : TrackedUnit(newOwner)
{
    DevastatorClass::init();

    setHealth(getMaxHealth());

    devastateTimer = 0;
}

DevastatorClass::DevastatorClass(Stream& stream) : TrackedUnit(stream)
{
    DevastatorClass::init();

	devastateTimer = stream.readSint32();
}

void DevastatorClass::init()
{
    itemID = Unit_Devastator;
    owner->incrementUnits(itemID);

	numWeapons = 2;
	bulletType = Bullet_Shell;
	weaponReloadTime = 150;

	GraphicID = ObjPic_Devastator_Base;
	graphic = pGFXManager->getObjPic(GraphicID,getOwner()->getHouseID());
	GunGraphicID = ObjPic_Devastator_Gun;
	turretGraphic = pGFXManager->getObjPic(GunGraphicID,getOwner()->getHouseID());

	imageW = graphic->w/NUM_ANGLES;
	imageH = graphic->h;
}


DevastatorClass::~DevastatorClass()
{
}

void DevastatorClass::save(Stream& stream) const
{
	TrackedUnit::save(stream);
	stream.writeSint32(devastateTimer);
}

void DevastatorClass::blitToScreen()
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
    dest.x = getDrawnX() + devastatorTurretOffset[drawnAngle].x;
    dest.y = getDrawnY() + devastatorTurretOffset[drawnAngle].y;
    SDL_BlitSurface(turretGraphic, &source, screen, &dest);	//blit turret
    if (isBadlyDamaged())
        drawSmoke(screenborder->world2screenX(realX), screenborder->world2screenY(realY));
}

void DevastatorClass::HandleStartDevastateClick() {
	currentGame->GetCommandManager().addCommand(Command(CMD_DEVASTATOR_STARTDEVASTATE,objectID));
}

void DevastatorClass::DoStartDevastate()
{
	if (devastateTimer <= 0)
		devastateTimer = 200;
}

void DevastatorClass::destroy()
{
    if(currentGameMap->cellExists(location) && isVisible()) {
        for(int i = 0; i < 3; i++) {
            for(int j = 0; j < 3; j++) {
                Coord realPos((short)realX + (i - 1)*BLOCKSIZE, (short)realY + (j - 1)*BLOCKSIZE);

                currentGameMap->damage(objectID, owner, realPos, itemID, 150, 16, false);

                Uint32 explosionID = currentGame->RandomGen.getRandOf(2,Explosion_Large1, Explosion_Large2);
                currentGame->getExplosionList().push_back(new Explosion(explosionID, realPos, owner->getHouseID()));
            }
        }

        if(isVisible(getOwner()->getTeam()))
            soundPlayer->playSoundAt(Sound_ExplosionLarge,location);
    }

    TrackedUnit::destroy();
}

bool DevastatorClass::update()
{
	if (active) {
		if ((devastateTimer > 0) && (--devastateTimer == 0)) {
			destroy();
			return false;
		}
	}

	return UnitClass::update();
}

void DevastatorClass::PlayAttackSound() {
	soundPlayer->playSoundAt(Sound_ExplosionSmall,location);
}

