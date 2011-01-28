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

#include <units/Frigate.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <House.h>
#include <MapClass.h>

#include <structures/StarPortClass.h>

Frigate::Frigate(House* newOwner) : AirUnit(newOwner)
{
    Frigate::init();

    setHealth(getMaxHealth());

    attackMode = STOP;

    respondable = false;
	droppedOffCargo = false;
}

Frigate::Frigate(Stream& stream) : AirUnit(stream)
{
    Frigate::init();

    droppedOffCargo = stream.readBool();
}

void Frigate::init()
{
	itemID = Unit_Frigate;
	owner->incrementUnits(itemID);

	canAttackStuff = false;

	GraphicID = ObjPic_Frigate;
	graphic = pGFXManager->getObjPic(GraphicID,getOwner()->getHouseID());
    shadowGraphic = pGFXManager->getObjPic(ObjPic_FrigateShadow,getOwner()->getHouseID());

	imageW = graphic->w/NUM_ANGLES;
	imageH = graphic->h;
}

Frigate::~Frigate()
{
    if(target.getObjPointer() != NULL && target.getObjPointer()->getItemID() == Structure_StarPort) {
        dynamic_cast<StarPortClass*>(target.getStructurePointer())->informFrigateDestroyed();
    }
}

void Frigate::save(Stream& stream) const
{
	AirUnit::save(stream);

	stream.writeBool(droppedOffCargo);
}

bool Frigate::canPass(int xPos, int yPos) const
{
	// frigate can always pass other air units
	return currentGameMap->cellExists(xPos, yPos);
}

void Frigate::checkPos()
{
	AirUnit::checkPos();

    if ((location == destination) && (distance_from(realX, realY, destination.x * BLOCKSIZE + (BLOCKSIZE/2), destination.y * BLOCKSIZE + (BLOCKSIZE/2)) < BLOCKSIZE/8) ) {
        StarPortClass* pStarport = dynamic_cast<StarPortClass*>(target.getStructurePointer());

        if(pStarport != NULL) {
            pStarport->startDeploying();
            setTarget(NULL);
            setDestination(guardPoint);
            droppedOffCargo = true;
        }
	}
}

bool Frigate::update() {

    if(AirUnit::update() == false) {
        return false;
    }

    // check if target is destroyed
    if((droppedOffCargo == false) && target.getStructurePointer() == NULL) {
        setDestination(guardPoint);
        droppedOffCargo = true;
    }

	// check if this frigate has to be removed because it has just brought all units to the Starport
	if (active)	{
		if(droppedOffCargo
            && (    (location.x == 0) || (location.x == currentGameMap->sizeX-1)
                    || (location.y == 0) || (location.y == currentGameMap->sizeY-1))
            && !moving)	{

            setVisible(VIS_ALL, false);
            destroy();
            return false;
		}
	}
	return true;
}

void Frigate::deploy(const Coord& newLocation) {
	AirUnit::deploy(newLocation);

	respondable = false;
}
