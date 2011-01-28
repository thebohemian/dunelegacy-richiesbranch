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

#include <units/MCVClass.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <FileClasses/TextManager.h>
#include <House.h>
#include <Explosion.h>
#include <Game.h>
#include <SoundPlayer.h>
#include <MapClass.h>

MCVClass::MCVClass(House* newOwner) : GroundUnit(newOwner)
{
    MCVClass::init();

    setHealth(getMaxHealth());
    attackMode = STOP;
}

MCVClass::MCVClass(Stream& stream) : GroundUnit(stream)
{
    MCVClass::init();
}

void MCVClass::init() {
    itemID = Unit_MCV;
    owner->incrementUnits(itemID);

	canAttackStuff = false;

	GraphicID = ObjPic_MCV;
	graphic = pGFXManager->getObjPic(GraphicID,getOwner()->getHouseID());

	imageW = graphic->w/NUM_ANGLES;
	imageH = graphic->h;
}

MCVClass::~MCVClass() {

}

void MCVClass::HandleDeployClick() {
	currentGame->GetCommandManager().addCommand(Command(CMD_MCV_DEPLOY,objectID));
}

void MCVClass::DoDeploy() {
    // check if there is enough room for construction yard
	if (currentGameMap->okayToBuildExclusive(getX(), getY(), 2, 2)) {

        // save needed values
		House* pOwner = getOwner();
		Coord newLocation = getLocation();

		// destroy MCV but with base class method since we want no explosion
		GroundUnit::destroy();

		// place construction yard
		pOwner->placeStructure(NONE, Structure_ConstructionYard, newLocation.x, newLocation.y);

		return;
	} else {
		currentGame->AddToNewsTicker(pTextManager->getLocalized("You cannot deploy here."));
	}
}

void MCVClass::destroy() {
    if(currentGameMap->cellExists(location) && isVisible()) {
        Coord realPos((short)realX, (short)realY);
        currentGame->getExplosionList().push_back(new Explosion(Explosion_SmallUnit, realPos, owner->getHouseID()));

        if(isVisible(getOwner()->getTeam()))
            soundPlayer->playSoundAt(Sound_ExplosionSmall,location);
    }

    GroundUnit::destroy();
}
