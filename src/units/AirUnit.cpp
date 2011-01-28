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

#include <units/AirUnit.h>

#include <globals.h>

#include <House.h>
#include <Game.h>
#include <Explosion.h>
#include <SoundPlayer.h>
#include <MapClass.h>
#include <ScreenBorder.h>

#include <FileClasses/GFXManager.h>

#include <misc/draw_util.h>

#include <structures/RepairYardClass.h>

AirUnit::AirUnit(House* newOwner) : UnitClass(newOwner)
{
    AirUnit::init();
}

AirUnit::AirUnit(Stream& stream) : UnitClass(stream)
{
    AirUnit::init();
}

void AirUnit::init()
{
    shadowGraphic = NULL;
    aFlyingUnit = true;
}

AirUnit::~AirUnit()
{
}

void AirUnit::save(Stream& stream) const
{
	UnitClass::save(stream);
}

void AirUnit::destroy()
{
    if(isVisible()) {
        Coord position((short) realX, (short) realY);
        currentGame->getExplosionList().push_back(new Explosion(Explosion_Medium2, position, owner->getHouseID()));

        if(isVisible(getOwner()->getTeam()))
            soundPlayer->playSoundAt(Sound_ExplosionMedium,location);
    }

    UnitClass::destroy();
}

void AirUnit::assignToMap(const Coord& pos)
{
	if(currentGameMap->cellExists(pos)) {
		currentGameMap->cell[pos.x][pos.y].assignAirUnit(getObjectID());
//		currentGameMap->viewMap(owner->getTeam(), location, getViewRange());
	}
}

void AirUnit::checkPos()
{
}

void AirUnit::blitToScreen()
{
	SDL_Rect dest, source;
	dest.x = getDrawnX() + 1;
	dest.y = getDrawnY() + 3;
	dest.w = source.w = imageW;
	dest.h = source.h = imageH;

    if( screenborder->isInsideScreen(Coord((int) getRealX() + 1, (int) getRealY() + 3),Coord(getImageW(), getImageH())) == true) {
		// Not out of screen
        if(drawnAngle > 7) {
            fprintf(stderr,"Invalid drawnAngle(%d)\n", drawnAngle);
        }

        source.x = drawnAngle*imageW;
        source.y = drawnFrame*imageH;

        if(shadowGraphic != NULL) {
            SDL_Rect mini = {0, 0, 1, 1};
            SDL_Rect drawLoc = {dest.x, dest.y, 1, 1};
            SDL_Surface* transparentSurf = pGFXManager->getTransparent150Surface();

            SDL_LockSurface(shadowGraphic);
            for(int i=0;i<imageW; i++) {
                for(int j=0;j<imageH; j++) {
                    if(getpixel(shadowGraphic,source.x+i,source.y+j) == 12) {
                        drawLoc.x = dest.x + i;
                        drawLoc.y = dest.y + j;
                        SDL_BlitSurface(transparentSurf,&mini,screen,&drawLoc);
                    };
                }
            }
            SDL_UnlockSurface(shadowGraphic);
        }
	}

	UnitClass::blitToScreen();
}

bool AirUnit::canPass(int xPos, int yPos) const
{
	return currentGameMap->cellExists(xPos, yPos);
}
