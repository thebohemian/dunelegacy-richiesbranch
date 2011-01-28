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

#include <units/HarvesterClass.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <House.h>
#include <Game.h>
#include <MapClass.h>
#include <Explosion.h>
#include <SoundPlayer.h>
#include <ScreenBorder.h>

#include <structures/RefineryClass.h>

#include <misc/draw_util.h>

/* how often is the same sandframe redrawn */
#define HARVESTERDELAY 30

/* how often to change harvester position while harvesting */
#define RANDOMHARVESTMOVE 500

/* how fast is spice extracted */
#define HARVESTEREXTRACTSPEED 0.5

Coord	harvesterSandOffset[] =
{
	Coord(-10, 5),
	Coord(-3, 9),
	Coord(4, 10),
	Coord(11, 9),
	Coord(18, 5),
	Coord(14, -2),
	Coord(4, -5),
	Coord(-5, -2)
};

HarvesterClass::HarvesterClass(House* newOwner) : TrackedUnit(newOwner)
{
    HarvesterClass::init();

    setHealth(getMaxHealth());

    spice = 0;
    harvestingMode = false;
	returningToRefinery = false;
	spiceCheckCounter = 0;

    attackMode = GUARD;
}

HarvesterClass::HarvesterClass(Stream& stream) : TrackedUnit(stream)
{
    HarvesterClass::init();

	harvestingMode = stream.readBool();
	returningToRefinery = stream.readBool();
    spice = stream.readDouble();
    spiceCheckCounter = stream.readUint32();
}

void HarvesterClass::init()
{
    itemID = Unit_Harvester;
    owner->incrementUnits(itemID);

	canAttackStuff = false;

	GraphicID = ObjPic_Harvester;
	graphic = pGFXManager->getObjPic(GraphicID,getOwner()->getHouseID());

	imageW = graphic->w/NUM_ANGLES;
	imageH = graphic->h;
}

HarvesterClass::~HarvesterClass()
{
	;
}

void HarvesterClass::save(Stream& stream) const
{
	TrackedUnit::save(stream);
	stream.writeBool(harvestingMode);
	stream.writeBool(returningToRefinery);
    stream.writeDouble(spice);
    stream.writeUint32(spiceCheckCounter);
}

void HarvesterClass::blitToScreen()
{
	SDL_Rect dest, source;
	dest.x = getDrawnX();
	dest.y = getDrawnY();
	dest.w = source.w = imageW;
	dest.h = source.h = imageH;
	source.x = drawnAngle*imageW;
	source.y = 0;

    SDL_BlitSurface(graphic, &source, screen, &dest);

    if(isHarvesting() == true) {
        /*using counter to avoid changing the picture to often*/
        static int counter = 0;
        static int lastY;
        SDL_Surface* sand = pGFXManager->getObjPic(ObjPic_Harvester_Sand,getOwner()->getHouseID());
        source.x = drawnAngle*sand->w/8;
        if(counter++ >= HARVESTERDELAY) {
            counter = 0;
            source.y = sand->h/3*getRandomInt(0, LASTSANDFRAME);
            lastY = source.y;
        } else {
            source.y = lastY;
        };

        dest.w = source.w = sand->w/8;
        dest.h = source.h = sand->h/3;
        dest.x = getDrawnX() + harvesterSandOffset[drawnAngle].x;
        dest.y = getDrawnY() + harvesterSandOffset[drawnAngle].y;

        SDL_BlitSurface(sand, &source, screen, &dest);
    }

    if(isBadlyDamaged())
        drawSmoke(screenborder->world2screenX(realX), screenborder->world2screenY(realY));
}

void HarvesterClass::checkPos()
{
	TrackedUnit::checkPos();

	if(attackMode == STOP) {
        harvestingMode = false;
	}

	if(active)	{
		if (returningToRefinery) {
			if (target && (target.getObjPointer() != NULL) && (target.getObjPointer()->getItemID() == Structure_Refinery)) {
				//find a refinery to return to
				Coord closestPoint = target.getObjPointer()->getClosestPoint(location);
				if(moving == false && blockDistance(location, closestPoint) <= 1.5)	{
					awaitingPickup = false;
					if (((RefineryClass*)target.getObjPointer())->isFree())
						setReturned();
				} else if (!awaitingPickup) {
					requestCarryall();
				}
			} else if (!structureList.empty()) {
				int	leastNumBookings = 1000000; //huge amount so refinery couldn't possibly compete with any refinery num bookings
				double	closestLeastBookedRefineryDistance = 1000000;
				RefineryClass	*bestRefinery = NULL;

                RobustList<StructureClass*>::const_iterator iter;
                for(iter = structureList.begin(); iter != structureList.end(); ++iter) {
					StructureClass* tempStructure = *iter;

					if((tempStructure->getItemID() == Structure_Refinery) && (tempStructure->getOwner() == owner)) {
						RefineryClass* tempRefinery = static_cast<RefineryClass*>(tempStructure);
						Coord closestPoint = tempRefinery->getClosestPoint(location);
						double tempDistance = distance_from(location, closestPoint);
						int tempNumBookings = tempRefinery->getNumBookings();

						if (tempNumBookings < leastNumBookings)	{
							leastNumBookings = tempNumBookings;
							closestLeastBookedRefineryDistance = tempDistance;
							bestRefinery = tempRefinery;
						} else if (tempNumBookings == leastNumBookings) {
							if (tempDistance < closestLeastBookedRefineryDistance) {
								closestLeastBookedRefineryDistance = tempDistance;
								bestRefinery = tempRefinery;
							}
						}
					}
				}

				if (bestRefinery) {
					DoMove2Object(bestRefinery);

					bestRefinery->startAnimate();
				}
			}
		} else if (harvestingMode && !hasBookedCarrier() && (blockDistance(location, destination) > 10.0)) {
			requestCarryall();
        } else if(respondable && !harvestingMode && attackMode != STOP) {
            if(spiceCheckCounter == 0) {
                if(currentGameMap->findSpice(&destination, &guardPoint)) {
                    harvestingMode = true;
                    guardPoint = destination;
                } else {
                    harvestingMode = false;
                }
                spiceCheckCounter = 100;
            } else {
                spiceCheckCounter--;
            }
		}
	}
}

void HarvesterClass::deploy(const Coord& newLocation)
{
	if(currentGameMap->cellExists(newLocation)) {
		UnitClass::deploy(newLocation);
		if(spice == 0) {
			if((attackMode != STOP) && currentGameMap->findSpice(&destination, &guardPoint)) {
				harvestingMode = true;
				guardPoint = destination;
			} else {
				harvestingMode = false;
			}
		}
	}
}

void HarvesterClass::destroy()
{
    if(currentGameMap->cellExists(location) && isVisible()) {
        int xpos = location.x;
        int ypos = location.y;

        if(currentGameMap->cellExists(xpos,ypos)) {
            double spiceSpreaded = 0.75 * spice;
            int availableSandPos = 0;

            /* how many regions have sand */
            for(int i = -1; i <= 1; i++) {
                for(int j = -1; j <= 1; j++) {
                    if (currentGameMap->cellExists(xpos + i, ypos + j)
                        && (distance_from(xpos, ypos, xpos + i, ypos + j) <= 6))
                    {
                        TerrainClass *cell = currentGameMap->getCell(xpos + i, ypos + j);
                        if ((cell != NULL) & ((cell->isSand()) || (cell->isSpice()) )) {
                            availableSandPos++;
                        }
                    }
                }
            }

            /* now we can spread spice */
            for(int i = -1; i <= 1; i++) {
                for(int j = -1; j <= 1; j++) {
                    if (currentGameMap->cellExists(xpos + i, ypos + j)
                        && (distance_from(xpos, ypos, xpos + i, ypos + j) <= 6))
                    {
                        TerrainClass *cell = currentGameMap->getCell(xpos + i, ypos + j);
                        if ((cell != NULL) & ((cell->isSand()) || (cell->isSpice()) )) {
                            cell->setSpice(cell->getSpice() + spiceSpreaded / availableSandPos);
                        }
                    }
                }
            }
        }

        setTarget(NULL);

        Coord realPos((short)realX, (short)realY);
        Uint32 explosionID = currentGame->RandomGen.getRandOf(2,Explosion_Medium1, Explosion_Medium2);
        currentGame->getExplosionList().push_back(new Explosion(explosionID, realPos, owner->getHouseID()));

        if(isVisible(getOwner()->getTeam()))
            soundPlayer->playSoundAt(Sound_ExplosionLarge,location);
    }

	TrackedUnit::destroy();
}

void HarvesterClass::drawSelectionBox()
{
	SDL_Rect	dest;
	SDL_Surface* selectionBox = pGFXManager->getUIGraphic(UI_SelectionBox);

	dest.x = getDrawnX() + imageW/2 - selectionBox->w/2;
	dest.y = getDrawnY() + imageH/2 - selectionBox->h/2;
	dest.w = selectionBox->w;
	dest.h = selectionBox->h;

	SDL_BlitSurface(selectionBox, NULL, screen, &dest);

	dest.x = getDrawnX() + imageW/2 - selectionBox->w/2;
	dest.y = getDrawnY() + imageH/2 - selectionBox->h/2;

	drawhline(screen, dest.x+1, dest.y-1, dest.x+1 + ((int)((getHealth()/(double)getMaxHealth())*(selectionBox->w-3))), getHealthColor());

	if ((getOwner() == pLocalHouse) && (spice > 0.0))
	{
		dest.x = getDrawnX() + imageW/2 - selectionBox->w/2;
		dest.y = getDrawnY() + imageH/2 - selectionBox->h/2;
		drawhline(screen, dest.x+1, dest.y-2, dest.x+1 + ((int)((((double)spice)/HARVESTERMAXSPICE)*(selectionBox->w-3))), COLOR_ORANGE);

		//want it to start in one from edges		finish one from right edge
	}
}

void HarvesterClass::handleDamage(int damage, Uint32 damagerID, House* damagerOwner)
{
    TrackedUnit::handleDamage(damage, damagerID, damagerOwner);

    ObjectClass* damager = currentGame->getObjectManager().getObject(damagerID);

    if(!target && !forced && damager && canAttack(damager) && (attackMode != STOP)) {
        setTarget(damager);
    }
}

void HarvesterClass::harvesterReturn()
{
	returningToRefinery = true;
	harvestingMode = false;
}

void HarvesterClass::setAmountOfSpice(double newSpice)
{
	if ((newSpice >=0) && (newSpice <= HARVESTERMAXSPICE))
		spice = newSpice;
}

void HarvesterClass::setDestination(int newX, int newY)
{
	ObjectClass::setDestination(newX, newY);

	harvestingMode =  (attackMode != STOP) && (currentGameMap->cellExists(newX, newY) && currentGameMap->cell[newX][newY].hasSpice());
}

void HarvesterClass::setTarget(ObjectClass* newTarget)
{
	if (returningToRefinery && target && (target.getObjPointer()!= NULL)
		&& (target.getObjPointer()->getItemID() == Structure_Refinery))
	{
		((RefineryClass*)target.getObjPointer())->unBook();
		returningToRefinery = false;
	}

	TrackedUnit::setTarget(newTarget);

	if (target && (target.getObjPointer()!= NULL)
		&& (target.getObjPointer()->getOwner() == getOwner())
		&& (target.getObjPointer()->getItemID() == Structure_Refinery))
	{
		((RefineryClass*)target.getObjPointer())->book();
		returningToRefinery = true;
	}

}

void HarvesterClass::setReturned()
{
	if (selected)
		removeFromSelectionLists();

	currentGameMap->removeObjectFromMap(getObjectID());

	((RefineryClass*)target.getObjPointer())->assignHarvester(this);

	returningToRefinery = false;
	moving = false;
	respondable = false;
	setActive(false);

	setLocation(INVALID_POS, INVALID_POS);
	setVisible(VIS_ALL, false);
}

void HarvesterClass::move()
{
	UnitClass::move();

	if(active && !moving) {
		if(harvestingMode) {
			//every now and then move around whilst harvesting
			if(!forced && currentGameMap->cell[location.x][location.y].hasSpice() && (currentGame->RandomGen.rand(0, RANDOMHARVESTMOVE) == 0)) {
				currentGameMap->findSpice(&destination, &location);
				DoMove2Pos(destination, false);
			}

			if(location == destination) {
				if(spice < HARVESTERMAXSPICE) {
					if(currentGameMap->cell[location.x][location.y].hasSpice()) {
						spice += currentGameMap->cell[location.x][location.y].harvestSpice();

						if(!currentGameMap->cell[location.x][location.y].hasSpice()) {
							currentGameMap->removeSpice(location);
						}
					} else if (!currentGameMap->findSpice(&destination, &location)) {
						harvesterReturn();
					} else {
					    DoMove2Pos(destination, false);
					}
				} else {
					harvesterReturn();
				}
			}
		}
	}
}

bool HarvesterClass::isHarvesting() const {
    return harvestingMode && (blockDistance(location, destination) <= DIAGONALCOST) && currentGameMap->cellExists(location) && currentGameMap->getCell(location)->hasSpice();
}

bool HarvesterClass::canAttack(const ObjectClass* object) const
{
	return((object != NULL)
			&& object->isInfantry()
			&& (object->getOwner()->getTeam() != owner->getTeam())
			&& object->isVisible(getOwner()->getTeam()));
}

double HarvesterClass::extractSpice()
{
	double oldSpice = spice;

	if((spice - HARVESTEREXTRACTSPEED) >= 0) {
		spice -= HARVESTEREXTRACTSPEED;
	} else {
		spice = 0;
	}

	return (oldSpice - spice);
}
