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

#include <units/Saboteur.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <House.h>
#include <MapClass.h>


Saboteur::Saboteur(House* newOwner) : InfantryClass(newOwner)
{
    Saboteur::init();

    setHealth(getMaxHealth());

	setVisible(VIS_ALL, false);
	setVisible(getOwner()->getTeam(), true);
    attackMode = STOP;
}

Saboteur::Saboteur(Stream& stream) : InfantryClass(stream)
{
    Saboteur::init();
}

void Saboteur::init()
{
	itemID = Unit_Saboteur;
	owner->incrementUnits(itemID);

	GraphicID = ObjPic_Saboteur;
	graphic = pGFXManager->getObjPic(GraphicID,getOwner()->getHouseID());

	imageW = graphic->w/4;
	imageH = graphic->h/3;

	numWeapons = 0;
}

Saboteur::~Saboteur()
{
}


void Saboteur::checkPos()
{
	bool	canBeSeen[MAX_PLAYERS+1];

	InfantryClass::checkPos();
	if(active) {
		for(int i = 1; i <= MAX_PLAYERS; i++) {
			canBeSeen[i] = false;
		}

		for(int x = location.x - 2; (x <= location.x + 2); x++) {
			for(int y = location.y - 2; (y <= location.y + 2); y++) {
				if(currentGameMap->cellExists(x, y) && currentGameMap->getCell(x, y)->hasAnObject()) {
					canBeSeen[currentGameMap->getCell(x, y)->getObject()->getOwner()->getTeam()] = true;
				}
			}
		}

		for(int i = 1; i <= MAX_PLAYERS; i++) {
			setVisible(i, canBeSeen[i]);
		}

		setVisible(getOwner()->getTeam(), true);	//owner team can always see it
		//setVisible(pLocalHouse->getTeam(), true);
	}
}

bool Saboteur::update() {
	if(active) {
		if(moving == false) {
			//check to osee if close enough to blow up target
			if(target.getObjPointer() != NULL && target.getObjPointer()->isAStructure()
				&& (getOwner()->getTeam() != target.getObjPointer()->getOwner()->getTeam()))
			{
				Coord	closestPoint;
				closestPoint = target.getObjPointer()->getClosestPoint(location);


				if(blockDistance(location, closestPoint) <= 1.5)	{
				    ObjectClass* pObject = target.getObjPointer();
				    destroy();
					pObject->destroy();
					return false;
				}
			}
		}
	}

	return UnitClass::update();
}

void Saboteur::deploy(const Coord& newLocation) {
	UnitClass::deploy(newLocation);

	setVisible(VIS_ALL, false);
	setVisible(getOwner()->getTeam(), true);
}


bool Saboteur::canAttack(const ObjectClass* object) const {
	if ((object != NULL) && object->isAStructure()
		&& (object->getOwner()->getTeam() != owner->getTeam())
		&& object->isVisible(getOwner()->getTeam()))
		return true;
	else
		return false;
}
