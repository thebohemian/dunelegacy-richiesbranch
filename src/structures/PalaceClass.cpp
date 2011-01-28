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

#include <structures/PalaceClass.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <FileClasses/TextManager.h>
#include <House.h>
#include <Game.h>
#include <MapClass.h>
#include <BulletClass.h>
#include <SoundPlayer.h>

#include <units/InfantryClass.h>
#include <units/TrooperClass.h>
#include <units/Saboteur.h>

#include <GUI/ObjectInterfaces/PalaceInterface.h>

#define PALACE_DEATHHAND_WEAPONDAMAGE       100

PalaceClass::PalaceClass(House* newOwner) : StructureClass(newOwner)
{
    PalaceClass::init();

    setHealth(getMaxHealth());
	specialTimer = getMaxSpecialTimer();
}

PalaceClass::PalaceClass(Stream& stream) : StructureClass(stream)
{
    PalaceClass::init();

    specialTimer = stream.readSint32();
}
void PalaceClass::init()
{
    itemID = Structure_Palace;
	owner->incrementStructures(itemID);

	structureSize.x = 3;
	structureSize.y = 3;

	GraphicID = ObjPic_Palace;
	graphic = pGFXManager->getObjPic(GraphicID,getOwner()->getHouseID());
	imageW = graphic->w / 4;
	imageH = graphic->h;
	FirstAnimFrame = 2;
	LastAnimFrame = 3;
}

PalaceClass::~PalaceClass()
{
}

void PalaceClass::save(Stream& stream) const
{
	StructureClass::save(stream);
	stream.writeSint32(specialTimer);
}

ObjectInterface* PalaceClass::GetInterfaceContainer() {
	if((pLocalHouse == owner) || (debug == true)) {
		return PalaceInterface::Create(objectID);
	} else {
		return DefaultObjectInterface::Create(objectID);
	}
}

void PalaceClass::HandleSpecialClick() {
    currentGame->GetCommandManager().addCommand(Command(CMD_PALACE_SPECIALWEAPON,objectID));
}

void PalaceClass::HandleDeathhandClick(int xPos, int yPos) {
    if (currentGameMap->cellExists(xPos, yPos)) {
        currentGame->GetCommandManager().addCommand(Command(CMD_PALACE_DEATHHAND,objectID, (Uint32) xPos, (Uint32) yPos));
    }
}

void PalaceClass::DoSpecialWeapon()
{
	if(!specialReady()) {
		return;
	}

    if((origHouse == HOUSE_HARKONNEN) || (origHouse == HOUSE_SARDAUKAR)) {
        // wrong house
        return;
    }

	switch (origHouse) {
		case HOUSE_ATREIDES:
		case HOUSE_FREMEN: {
			if(callFremen()) {
                specialTimer = getMaxSpecialTimer();
			}
        } break;

		case HOUSE_ORDOS:
		case HOUSE_MERCENARY: {
            if(spawnSaboteur()) {
                specialTimer = getMaxSpecialTimer();
			}
        } break;

        case HOUSE_HARKONNEN:
        case HOUSE_SARDAUKAR:
        default: {
            throw std::runtime_error("PalaceClass::DoSpecialWeapon(): Invalid house");
        } break;
	}
}

void PalaceClass::DoLaunchDeathhand(int xpos, int ypos)
{
    if(!specialReady()) {
		return;
	}

    if((origHouse != HOUSE_HARKONNEN) && (origHouse != HOUSE_SARDAUKAR)) {
        // wrong house
        return;
    }

    //int deathOffX = currentGame->RandomGen.rand(-100, 100);
    //int deathOffY = currentGame->RandomGen.rand(-100, 100);
    int deathOffX = currentGame->RandomGen.rand(-150, 150);
    int deathOffY = currentGame->RandomGen.rand(-150, 150);

    Coord centerPoint = getCenterPoint();
    Coord dest( xpos * BLOCKSIZE + BLOCKSIZE/2 + deathOffX,
                ypos * BLOCKSIZE + BLOCKSIZE/2 + deathOffY);

    bulletList.push_back(new BulletClass(objectID, &centerPoint, &dest, Bullet_LargeRocket, PALACE_DEATHHAND_WEAPONDAMAGE, false));
    soundPlayer->playSoundAt(Sound_Rocket, getLocation());

    if(getOwner() != pLocalHouse) {
        currentGame->AddToNewsTicker(pTextManager->getDuneText(DuneText_MissileApproaching));
        soundPlayer->playVoice(MissileApproaching, pLocalHouse->getHouseID());
    }

    specialTimer = getMaxSpecialTimer();
}

void PalaceClass::doSpecificStuff()
{
	if(specialTimer > 0) {
	    --specialTimer;
		if(specialTimer <= 0) {
			specialTimer = 0;

			if(getOwner() == pLocalHouse) {
				currentGame->AddToNewsTicker(pTextManager->getLocalized("Palace is ready"));
			}
		}
	} else if(getOwner()->isAI()) {

	    if((origHouse == HOUSE_HARKONNEN) || (origHouse == HOUSE_SARDAUKAR)) {
	        // Harkonnen and Sardaukar
            StructureClass* closestStructure = findClosestTargetStructure(this);
            if(closestStructure) {
                Coord temp = closestStructure->getClosestPoint(getLocation());
                DoLaunchDeathhand(temp.x, temp.y);
            }
	    } else {
	        // other houses
            DoSpecialWeapon();
	    }
	}
}

void PalaceClass::setSpecialTimer(int time)
{
	if (time >= 0) {
		specialTimer = time;
	}
}

bool PalaceClass::callFremen()
{
	bool ok = true;

	int count, x, y;
	Coord coord;

	count = 0;
	do {
		x = currentGame->RandomGen.rand(1, currentGameMap->sizeX-2);
		y = currentGame->RandomGen.rand(1, currentGameMap->sizeY-2);
	} while((currentGameMap->getCell(x-1, y-1)->hasAGroundObject()
			|| currentGameMap->getCell(x, y-1)->hasAGroundObject()
			|| currentGameMap->getCell(x+1, y-1)->hasAGroundObject()
			|| currentGameMap->getCell(x-1, y)->hasAGroundObject()
			|| currentGameMap->getCell(x, y)->hasAGroundObject()
			|| currentGameMap->getCell(x+1, y)->hasAGroundObject()
			|| currentGameMap->getCell(x-1, y+1)->hasAGroundObject()
			|| currentGameMap->getCell(x, y+1)->hasAGroundObject()
			|| currentGameMap->getCell(x+1, y+1)->hasAGroundObject())
			&& (count++ <= 1000));

	if (count < 1000) {
		int				i, j;
		InfantryClass	*fremen;

		for (count = 0; count < 20; count++) {
			if (currentGame->RandomGen.rand(1, 10) <= 6) {
				//60% troopers
				fremen = (TrooperClass*)getOwner()->createUnit(Unit_Fremen);
			} else {
				fremen = (InfantryClass*)getOwner()->createUnit(Unit_Soldier);
			}

			do {
				i = currentGame->RandomGen.rand(-1, 1);
				j = currentGame->RandomGen.rand(-1, 1);
			} while (!currentGameMap->getCell(x + i, y + j)->infantryNotFull());

			coord.x = x + i;
			coord.y = y + j;
			fremen->deploy(coord);

			fremen->setAttacking(true);
			fremen->DoSetAttackMode(HUNT);
			fremen->setRespondable(false);

			StructureClass* closestStructure = fremen->findClosestTargetStructure(fremen);
			if(closestStructure) {
				Coord closestPoint = closestStructure->getClosestPoint(fremen->getLocation());
				fremen->setGuardPoint(closestPoint);
				fremen->setDestination(closestPoint);
			} else {
				UnitClass* closestUnit = fremen->findClosestTargetUnit(fremen);
				if(closestUnit) {
					fremen->setGuardPoint(closestUnit->getLocation());
					fremen->setDestination(closestUnit->getLocation());
				}
			}
		}
	} else {
		ok = false;
		if (getOwner() == pLocalHouse) {
			currentGame->AddToNewsTicker(pTextManager->getLocalized("Unable to spawn Fremen"));
		}
	}

	return ok;
}

bool PalaceClass::spawnSaboteur()
{
	Saboteur*		saboteur = (Saboteur*)getOwner()->createUnit(Unit_Saboteur);
	Coord		spot = currentGameMap->findDeploySpot(saboteur, getLocation(), getDestination(), getStructureSize());

	saboteur->deploy(spot);

	if (getOwner()->isAI()) {
		saboteur->setAttacking(true);
		saboteur->DoSetAttackMode(HUNT);
		currentGame->AddToNewsTicker(pTextManager->getDuneText(DuneText_SaboteurApproaching));
        soundPlayer->playVoice(SaboteurApproaching, pLocalHouse->getHouseID());
	}

	return true;
}

double PalaceClass::getPercentComplete()
{
	return specialTimer*100.0/getMaxSpecialTimer();
}
