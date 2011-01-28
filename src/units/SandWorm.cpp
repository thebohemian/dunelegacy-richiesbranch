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

#include <units/SandWorm.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <House.h>
#include <Game.h>
#include <MapClass.h>
#include <ScreenBorder.h>
#include <SoundPlayer.h>

#include <misc/draw_util.h>

#include <units/InfantryClass.h>

#define MAX_SANDWORMSLEEPTIME 50000
#define MIN_SANDWORMSLEEPTIME 10000

Sandworm::Sandworm(House* newOwner) : GroundUnit(newOwner)
{
    Sandworm::init();

    setHealth(getMaxHealth());

    kills = 0;
	attackFrameTimer = 0;
	sleepTimer = 0;
	respondable = false;
}

Sandworm::Sandworm(Stream& stream) : GroundUnit(stream)
{
    Sandworm::init();

	kills = stream.readSint32();
	attackFrameTimer = stream.readSint32();
	sleepTimer = stream.readSint32();
}

void Sandworm::init()
{
    itemID = Unit_Sandworm;
    owner->incrementUnits(itemID);

	numWeapons = 0;
	weaponReloadTime = 200;

	GraphicID = ObjPic_Sandworm;
	graphic = pGFXManager->getObjPic(GraphicID,getOwner()->getHouseID());

	imageW = graphic->w;
	imageH = graphic->h/5;

	drawnFrame = INVALID;

	shimmerSurface = new SDL_Surface*[SANDWORM_SEGMENTS];
	for(int i = 0; i < SANDWORM_SEGMENTS; i++)
	{
		shimmerSurface[i] = copySurface(pGFXManager->getObjPic(ObjPic_SandwormShimmerMask,HOUSE_HARKONNEN));
		SDL_FillRect(shimmerSurface[i], NULL, COLOR_TRANSPARENT);
	}
}

Sandworm::~Sandworm()
{
	for(int i = 0; i < SANDWORM_SEGMENTS; i++)
		SDL_FreeSurface(shimmerSurface[i]);

	delete[] shimmerSurface;
}

void Sandworm::save(Stream& stream) const
{
	GroundUnit::save(stream);

	stream.writeSint32(kills);
	stream.writeSint32(attackFrameTimer);
	stream.writeSint32(sleepTimer);
}

void Sandworm::assignToMap(const Coord& pos)
{
	if(currentGameMap->cellExists(pos)) {
		currentGameMap->cell[pos.x][pos.y].assignUndergroundUnit(getObjectID());
		// do not unhide map cause this would give fremen players an advantage
		// currentGameMap->viewMap(owner->getTeam(), location, getViewRange());
	}
}

void Sandworm::attack()
{
	if(primaryWeaponTimer == 0) {
		if(target) {
			soundPlayer->playSoundAt(Sound_WormAttack, location);
			drawnFrame = 0;
			attackFrameTimer = 15;
			primaryWeaponTimer = weaponReloadTime;
		}
	}
}

void Sandworm::deploy(const Coord& newLocation) {
	UnitClass::deploy(newLocation);

	respondable = false;
}

void Sandworm::blitToScreen()
{
    SDL_Rect dest, source;
    dest.w = source.w = shimmerSurface[0]->w;
    dest.h = source.h = shimmerSurface[0]->h;
    source.x = 0;
    source.y = 0;

    if(moving) {
        //create worms shimmer
        if(shimmerSurface[0]->format->BitsPerPixel == 8) {
            /*TerrainClass	*cell;
            SDL_Surface		*cellSurface = NULL,
                            *lastCellSurface = NULL;*/
            SDL_Surface *mask = pGFXManager->getObjPic(ObjPic_SandwormShimmerMask,HOUSE_HARKONNEN);
            if ((!SDL_MUSTLOCK(screen) || (SDL_LockSurface(screen) >= 0))
                && (!SDL_MUSTLOCK(mask) || (SDL_LockSurface(mask) >= 0)))
            {
                unsigned char	*maskPixels = (unsigned char*)mask->pixels,
                                *screenPixels = (unsigned char*)screen->pixels,
                                *surfacePixels;
                int count, i,j, x,y, maxX = screenborder->getRight();

                for(count = 0; count < SANDWORM_SEGMENTS; count++) {
                    //for each segment of the worms length
                    if ((!SDL_MUSTLOCK(shimmerSurface[count]) || (SDL_LockSurface(shimmerSurface[count]) >= 0))) {
                        surfacePixels = (unsigned char*)shimmerSurface[count]->pixels;

                        dest.x = screenborder->world2screenX(lastLocs[count*(SANDWORM_LENGTH/SANDWORM_SEGMENTS)].x) - dest.w/2;
                        dest.y = screenborder->world2screenY(lastLocs[count*(SANDWORM_LENGTH/SANDWORM_SEGMENTS)].y) - dest.h/2;

                        for (i = 0; i < dest.w; i++) {
                            for (j = 0; j < dest.h; j++) {
                                //lastCellSurface = cellSurface;
                                //surfacePixels[i + j*dest.w] = getRandomInt(89, 93);
                                if (maskPixels[i + j*mask->pitch] == 0) {
                                    //direct copy
                                    x = i;
                                    y = j;
                                } else {
                                    x = i + getRandomInt(-3, 3);
                                    y = j + getRandomInt(-3, 3);
                                }

                                if (dest.x + x < 0)
                                    dest.x = x = 0;
                                else if (dest.x + x >= maxX)
                                    dest.x = maxX - 1, x = 0;

                                if (dest.y + y < 0)
                                    dest.y = y = 0;
                                else if (dest.y + y >= screen->h)
                                    dest.y = screen->h - 1, y = 0;


                                surfacePixels[i + j*shimmerSurface[count]->pitch] = screenPixels[dest.x + x + (dest.y + y)*screen->pitch];
                            }
                        }

                        if (SDL_MUSTLOCK(shimmerSurface[count]))
                            SDL_UnlockSurface(shimmerSurface[count]);
                    }
                }

                if (SDL_MUSTLOCK(mask)) {
                    SDL_UnlockSurface(mask);
                }

                if(SDL_MUSTLOCK(screen)) {
                    SDL_UnlockSurface(screen);
                }
            }
        }
    }

    /////draw wormy shimmer segments
    for(int count = 0; count < SANDWORM_SEGMENTS; count++) {
        //draw all the shimmering images
        dest.x = screenborder->world2screenX(lastLocs[count*(SANDWORM_LENGTH/SANDWORM_SEGMENTS)].x) - dest.w/2;
        dest.y = screenborder->world2screenY(lastLocs[count*(SANDWORM_LENGTH/SANDWORM_SEGMENTS)].y) - dest.h/2;

        SDL_BlitSurface(shimmerSurface[count], &source, screen, &dest);
    }

    if(drawnFrame != INVALID) {
        dest.x = getDrawnX();
        dest.y = getDrawnY();
        dest.w = source.w = imageW;
        dest.h = source.h = imageH;
        source.x = 0;
        source.y = drawnFrame*imageH;

        SDL_BlitSurface(graphic, &source, screen, &dest);
    }
}

void Sandworm::checkPos()
{
	if(moving) {
		if((abs((short)realX - lastLocs[0].x) >= 1) || (abs((short)realY - lastLocs[0].y) >= 1)) {
			for(int i = (SANDWORM_LENGTH-1); i > 0 ; i--) {
				lastLocs[i] = lastLocs[i-1];
			}

			lastLocs[0].x = (short)realX;
			lastLocs[0].y = (short)realY;
		}
	}

	if(justStoppedMoving) {
		realX = location.x*BLOCKSIZE + BLOCKSIZE/2;
		realY = location.y*BLOCKSIZE + BLOCKSIZE/2;

		if(currentGameMap->cellExists(location)) {
			TerrainClass* cell = currentGameMap->getCell(location);
			if(cell->hasInfantry() && (cell->getInfantry()->getOwner() == pLocalHouse)) {
				soundPlayer->playSound(SomethingUnderTheSand);
			}
		}
	}
}

void Sandworm::engageTarget()
{
	if(target && (target.getObjPointer() != NULL)) {
		if(!canAttack(target.getObjPointer()) || (!forced && !isInAttackModeRange(target.getObjPointer()))) {
			forced = false;
			setTarget(NULL);

			setGuardPoint(location);	//so will stop at current location, remove this line to go to
			setDestination(guardPoint);	//old way of destroyed units location

			pathList.clear();
		} else {
			Coord targetLocation = target.getObjPointer()->getClosestPoint(location);
			targetDistance = blockDistance(location, targetLocation);

			if((destination.x != targetLocation.x) && (destination.y != targetLocation.y)) {
                // targets location has moved, recalculate path
				pathList.clear();
            }

			if(forced || attackMode == HUNT) {
				setDestination(targetLocation);
			} else {
			    int maxDistance;
                switch(attackMode) {
                    case GUARD: {
                        maxDistance = getWeaponRange();
                    } break;

                    case AREAGUARD: {
                        maxDistance = getGuardRange();
                    } break;

                    case AMBUSH: {
                        maxDistance = getViewRange();
                    } break;

                    case STOP:
                    default: {
                        maxDistance = 0;
                    } break;
                }

                if((blockDistance(guardPoint, targetLocation) + targetDistance) > maxDistance) {
                    // give up
                    setDestination(guardPoint);
                    setTarget(NULL);
                } else {
                    setDestination(targetLocation);
                }
			}

			if(target && (target.getObjPointer() != NULL)) {
				targetAngle = lround(8.0/256.0*dest_angle(location, targetLocation));

				if(targetAngle == 8)
					targetAngle = 0;

				if(targetDistance <= currentGame->objectData.data[itemID].weaponrange) {
					setDestination(location);
					nextSpotFound = false;
					attack();
				}
			}
		}
	}
}

void Sandworm::setLocation(int xPos, int yPos)
{
	if(currentGameMap->cellExists(xPos, yPos) || ((xPos == INVALID_POS) && (yPos == INVALID_POS))) {
		UnitClass::setLocation(xPos, yPos);

		for(int i = 0; i < SANDWORM_LENGTH; i++) {
			lastLocs[i].x = (short)realX;
			lastLocs[i].y = (short)realY;
		}
	}
}

void Sandworm::setSleepTimer(int newSleepTime)
{
	if((newSleepTime >= MIN_SANDWORMSLEEPTIME) && (newSleepTime <= MAX_SANDWORMSLEEPTIME)) {
		sleepTimer = newSleepTime;
	}
}

void Sandworm::sleep()
{	//put sandworm to sleep for a while
	sleepTimer = currentGame->RandomGen.rand(MIN_SANDWORMSLEEPTIME, MAX_SANDWORMSLEEPTIME);
	setActive(false);
	setVisible(VIS_ALL, false);
	setForced(false);
	currentGameMap->removeObjectFromMap(getObjectID());	//no map point will reference now
	setLocation(NONE, NONE);
	setHealth(getMaxHealth());
	kills = 0;
	drawnFrame = INVALID;
	attackFrameTimer = 0;
}

bool Sandworm::update()
{
	if(getHealth() <= getMaxHealth()/2) {
		sleep();
	} else {
		if(GroundUnit::update() == false) {
		    return false;
		}

        if(attackFrameTimer > 0)	{
            //death frame has started
            if(attackFrameTimer == 1) {
                drawnFrame++;

                if(drawnFrame < 5) {
                    attackFrameTimer = 15;
                    if(drawnFrame == 1) {
                        //the close mouth bit of graphic
                        bool wasAlive = ( target && target.getObjPointer()->isVisible(getOwner()->getTeam()));	//see if was alive before attack
                        Coord realPos = Coord((short)realX, (short)realY);
                        currentGameMap->getCell(location)->damageCell(objectID, getOwner(), realPos, Bullet_Sandworm, 5000, NONE, false);

                        if(wasAlive && (target.getObjPointer()->isVisible(getOwner()->getTeam()) == false)) {
                            kills++;
                        }
                    }
                } else {
                    drawnFrame = INVALID;
                    if(kills >= 3) {
                        sleep();
                    }
                }
            }

            attackFrameTimer--;
        }

		if(sleepTimer > 0) {
			if(--sleepTimer <= 0) {
			    //awaken the worm!
				int i, j;

				do {
					i = currentGame->RandomGen.rand(0, currentGameMap->sizeX - 1);
					j = currentGame->RandomGen.rand(0, currentGameMap->sizeY - 1);
					sleepTimer++;
				} while (!canPass(i, j) && (sleepTimer < 1000));

				if(canPass(i, j)) {
					deploy(currentGameMap->getCell(i, j)->getLocation());
					sleepTimer = 0;
				} else {
				    //no room for sandworm on map
					sleep();	//try again later
				}
			}
		}
	}

	return true;
}

bool Sandworm::canAttack(const ObjectClass* object) const
{
	if((object != NULL)
		&& object->isAGroundUnit()
		&& (object->getItemID() != Unit_Sandworm)	//wont kill other sandworms
		&& object->isVisible(getOwner()->getTeam())
		//&& (object->getOwner()->getTeam() != owner->getTeam())
		&& currentGameMap->cellExists(object->getLocation())
		&& canPass(object->getLocation().x, object->getLocation().y)
		&& (currentGameMap->getCell(object->getLocation())->getSandRegion() == currentGameMap->getCell(location)->getSandRegion())) {
		return true;
	} else {
		return false;
	}
}

bool Sandworm::canPass(int xPos, int yPos) const
{
	return (currentGameMap->cellExists(xPos, yPos)
			&& !currentGameMap->getCell(xPos, yPos)->isRock()
			&& (!currentGameMap->getCell(xPos, yPos)->hasAnUndergroundUnit()
				|| (currentGameMap->getCell(xPos, yPos)->getUndergroundUnit() == this)));
}

ObjectClass* Sandworm::findTarget()
{
	ObjectClass	*closestTarget = NULL;

	if(attackMode == HUNT) {
	    double closestDistance = 1000000.0;

        RobustList<UnitClass*>::const_iterator iter;
	    for(iter = unitList.begin(); iter != unitList.end(); ++iter) {
			UnitClass* tempUnit = *iter;
            if (canAttack(tempUnit)
				&& (blockDistance(location, tempUnit->getLocation()) < closestDistance)) {
                closestTarget = tempUnit;
                closestDistance = blockDistance(location, tempUnit->getLocation());
            }
		}
	} else {
		closestTarget = ObjectClass::findTarget();
		if (closestTarget != NULL) {
		    if(closestTarget->getOwner() == pLocalHouse) {
                soundPlayer->playVoice(WarningWormSign, pLocalHouse->getHouseID());
		    }

			attackMode = HUNT;	//now have been awoken, go forth and kill
		}
	}

	return closestTarget;
}

void Sandworm::PlayAttackSound() {
	soundPlayer->playSoundAt(Sound_WormAttack,location);
}
