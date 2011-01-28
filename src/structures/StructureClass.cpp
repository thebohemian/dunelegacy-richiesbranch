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

#include <structures/StructureClass.h>

#include <globals.h>

#include <House.h>
#include <Game.h>
#include <MapClass.h>
#include <ScreenBorder.h>
#include <Explosion.h>
#include <SoundPlayer.h>

#include <misc/draw_util.h>

#include <units/UnitClass.h>

#include <GUI/ObjectInterfaces/DefaultStructureInterface.h>

StructureClass::StructureClass(House* newOwner) : ObjectClass(newOwner)
{
    StructureClass::init();

    repairing = false;
    origHouse = owner->getHouseID();

    fogged = false;

	degradeTimer = 0;
}

StructureClass::StructureClass(Stream& stream): ObjectClass(stream)
{
    StructureClass::init();

    repairing = stream.readBool();
    origHouse = stream.readUint32();

    fogged = stream.readBool();
    lastVisibleFrame = stream.readUint32();

    degradeTimer = stream.readSint32();

    size_t numSmoke = stream.readUint32();
    for(size_t i=0;i<numSmoke; i++) {
        smoke.push_back(StructureSmoke(stream));
    }
}

void StructureClass::init() {
	aStructure = true;

	structureSize.x = 0;
	structureSize.y = 0;

	JustPlacedTimer = 0;

	lastVisibleFrame = curAnimFrame = 2;
	animationCounter = 0;

	structureList.push_back(this);
}

StructureClass::~StructureClass() {
    currentGameMap->removeObjectFromMap(getObjectID());	//no map point will reference now
	currentGame->getObjectManager().RemoveObject(getObjectID());
	structureList.remove(this);
	owner->decrementStructures(itemID, location);

    removeFromSelectionLists();
}

void StructureClass::save(Stream& stream) const
{
	ObjectClass::save(stream);

    stream.writeBool(repairing);
	stream.writeUint32(origHouse);

	stream.writeBool(fogged);
	stream.writeUint32(lastVisibleFrame);

	stream.writeSint32(degradeTimer);

	stream.writeUint32(smoke.size());
	std::list<StructureSmoke>::const_iterator iter;
	for(iter = smoke.begin(); iter != smoke.end(); ++iter) {
        iter->save(stream);
	}
}

void StructureClass::assignToMap(const Coord& pos)
{
	Coord temp;
	for (int i = pos.x; i < pos.x + structureSize.x; i++) {
		for (int j = pos.y; j < pos.y + structureSize.y; j++) {
			if (currentGameMap->cellExists(i, j)) {
				currentGameMap->cell[i][j].assignNonInfantryGroundObject(getObjectID());
				if((itemID != Structure_Wall) && (itemID != Structure_ConstructionYard)
                     && !currentGameMap->cell[i][j].isConcrete() && currentGame->getGameInitSettings().isConcreteRequired()
                     && (currentGame->gameState != START)) {
                    setHealth(getHealth() - (0.5*(double)getMaxHealth()/((double)(structureSize.x*structureSize.y))));
				}
				currentGameMap->cell[i][j].setType(Terrain_Rock);
				currentGameMap->cell[i][j].setOwner(getOwner()->getHouseID());
				currentGameMap->viewMap(getOwner()->getTeam(), Coord(i,j), getViewRange());

				setVisible(VIS_ALL, true);
				setActive(true);
				setRespondable(true);
			}
		}
	}
}

void StructureClass::blitToScreen()
{
    SDL_Rect dest, source;
    dest.x = getDrawnX();
    dest.y = getDrawnY();
    dest.w = source.w = imageW;
    dest.h = source.h = imageH;

    source.x = imageW * (fogged ? lastVisibleFrame : curAnimFrame);
    source.y = 0;

    SDL_BlitSurface(graphic, &source, screen, &dest);

    if(fogged) {
        SDL_Surface* fogSurf = pGFXManager->getTransparent40Surface();
        SDL_BlitSurface(fogSurf, &source, screen, &dest);
    } else {
        SDL_Surface* pSmokeSurface = pGFXManager->getObjPic(ObjPic_Smoke,getOwner()->getHouseID());
        SDL_Rect smokeSource = { 0, 0, pSmokeSurface->w/3, pSmokeSurface->h};
        std::list<StructureSmoke>::const_iterator iter;
        for(iter = smoke.begin(); iter != smoke.end(); ++iter) {
            SDL_Rect smokeDest = { screenborder->world2screenX(iter->realPos.x) - smokeSource.w/2, screenborder->world2screenY(iter->realPos.y) - smokeSource.h, pSmokeSurface->w/3, pSmokeSurface->h};
            Uint32 cycleDiff = currentGame->GetGameCycleCount() - iter->startGameCycle;

            Uint32 smokeFrame = (cycleDiff/25) % 4;
            if(smokeFrame == 3) {
                smokeFrame = 1;
            }

            smokeSource.x = smokeFrame * smokeSource.w;
            SDL_BlitSurface(pSmokeSurface, &smokeSource, screen, &smokeDest);
        }

        lastVisibleFrame = curAnimFrame;
    }
}

ObjectInterface* StructureClass::GetInterfaceContainer() {
	if((pLocalHouse == owner) || (debug == true)) {
		return DefaultStructureInterface::Create(objectID);
	} else {
		return DefaultObjectInterface::Create(objectID);
	}
}

void StructureClass::drawSelectionBox()
{
	SDL_Rect dest;
	dest.x = getDrawnX();
	dest.y = getDrawnY();
	dest.w = imageW;
	dest.h = imageH;

	//now draw the selection box thing, with parts at all corners of structure
	if(!SDL_MUSTLOCK(screen) || (SDL_LockSurface(screen) == 0)) {
        // top left bit
		putpixel(screen, dest.x, dest.y, COLOR_WHITE);
		putpixel(screen, dest.x+1, dest.y, COLOR_WHITE);
		putpixel(screen, dest.x, dest.y+1, COLOR_WHITE);

        // top right bit
		putpixel(screen, dest.x-1 + imageW, dest.y, COLOR_WHITE);
		putpixel(screen, dest.x-2 + imageW, dest.y, COLOR_WHITE);
		putpixel(screen, dest.x-1 + imageW, dest.y+1, COLOR_WHITE);

        // bottom left bit
		putpixel(screen, dest.x, dest.y-1 + imageH, COLOR_WHITE);
		putpixel(screen, dest.x+1, dest.y-1 + imageH, COLOR_WHITE);
		putpixel(screen, dest.x, dest.y-2 + imageH, COLOR_WHITE);

        // bottom right bit
		putpixel(screen, dest.x-1 + imageW, dest.y-1 + imageH, COLOR_WHITE);
		putpixel(screen, dest.x-2 + imageW, dest.y-1 + imageH, COLOR_WHITE);
		putpixel(screen, dest.x-1 + imageW, dest.y-2 + imageH, COLOR_WHITE);

		if(SDL_MUSTLOCK(screen)) {
			SDL_UnlockSurface(screen);
		}
	}

	// Draws two lines right below the upper part of the selection box.
	drawrect(screen, dest.x+1, dest.y+1, dest.x + ((int)((getHealth()/(double)getMaxHealth())*(BLOCKSIZE*structureSize.x - 2))), dest.y+2, getHealthColor());
}

/**
    Returns the center point of this structure
    \return the center point in world coordinates
*/
Coord StructureClass::getCenterPoint() const
{
    return Coord( lround(realX + structureSize.x*BLOCKSIZE/2),
                  lround(realY + structureSize.y*BLOCKSIZE/2));
}

Coord StructureClass::getClosestCenterPoint(const Coord& objectLocation) const
{
	return getClosestPoint(objectLocation) * BLOCKSIZE + Coord(BLOCKSIZE/2, BLOCKSIZE/2);
}

int StructureClass::getDrawnX() const
{
	return screenborder->world2screenX(realX);
}

int StructureClass::getDrawnY() const
{
	return screenborder->world2screenY(realY);
}

void StructureClass::HandleActionClick(int xPos, int yPos)
{
	if ((xPos < location.x) || (xPos >= (location.x + structureSize.x)) || (yPos < location.y) || (yPos >= (location.y + structureSize.y))) {
		currentGame->GetCommandManager().addCommand(Command(CMD_STRUCTURE_SETDEPLOYPOSITION,objectID, (Uint32) xPos, (Uint32) yPos));
	} else {
		currentGame->GetCommandManager().addCommand(Command(CMD_STRUCTURE_SETDEPLOYPOSITION,objectID, (Uint32) NONE, (Uint32) NONE));
	}
}

void StructureClass::HandleRepairClick()
{
	currentGame->GetCommandManager().addCommand(Command(CMD_STRUCTURE_REPAIR,objectID));
}

void StructureClass::DoSetDeployPosition(int xPos, int yPos) {
	setTarget(NULL);
	setDestination(xPos,yPos);
	setForced(true);
}


void StructureClass::DoRepair()
{
	repairing = true;
}

void StructureClass::setDestination(int newX, int newY)
{
	if(currentGameMap->cellExists(newX, newY) || ((newX == INVALID_POS) && (newY == INVALID_POS))) {
		destination.x = newX;
		destination.y = newY;
	}
}

void StructureClass::setJustPlaced()
{
	JustPlacedTimer = 4;
	curAnimFrame = 0;
}

bool StructureClass::update()
{
    //update map
    if(currentGame->RandomGen.rand(0,40) == 0) {
        // PROBLEM: causes very low fps
        currentGameMap->viewMap(owner->getTeam(), location, getViewRange());
    }

    // degrade
    if(owner->getPowerRequirement() > owner->getProducedPower()) {
        degradeTimer--;
        if(degradeTimer <= 0) {
            degradeTimer = MILLI2CYCLES(15*1000);

            int damageMultiplyer = 1;
            if(owner->getHouseID() == HOUSE_HARKONNEN) {
                damageMultiplyer = 3;
            } else if(owner->getHouseID() == HOUSE_ORDOS) {
                damageMultiplyer = 2;
            }

            if(getHealth() > getMaxHealth() / 2) {
                setHealth( getHealth() - ((damageMultiplyer * (double)getMaxHealth())/100));
            }
        }
    }

    doSpecificStuff();

    if(getHealth() <= 0) {
        destroy();
        return false;
    }

    if(repairing) {
        if(owner->getCredits() >= 5) {
            // Original dune 2 is doing the repair calculation with fix-point math (multiply everything with 256).
            // It is calculating what fraction 2 hitpoints of the maximum health would be.
            int fraction = (2*256)/getMaxHealth();
            double repairprice = (fraction * currentGame->objectData.data[itemID].price) / 256.0;

            // Original dune is always repairing 5 hitpoints (for the costs of 2) but we are only repairing 1/30th of that
            owner->takeCredits(repairprice/30.0);
            double newHealth = getHealth();
            newHealth += 5.0/30.0;
            if(newHealth >= getMaxHealth()) {
                setHealth(getMaxHealth());
                repairing = false;
            } else {
                setHealth(newHealth);
            }
        } else {
            repairing = false;
        }
    } else if(owner->isAI() && ((getHealth()/(double)getMaxHealth()) < 0.75)) {
        DoRepair();
    }

    // check smoke
    std::list<StructureSmoke>::iterator iter = smoke.begin();
    while(iter != smoke.end()) {
        if(currentGame->GetGameCycleCount() - iter->startGameCycle >= MILLI2CYCLES(8*1000)) {
            smoke.erase(iter++);
        } else {
            ++iter;
        }
    }

    // update animations
    animationCounter++;
    if(animationCounter > ANIMATIONTIMER) {
        animationCounter = 0;
        curAnimFrame++;
        if((curAnimFrame < FirstAnimFrame) || (curAnimFrame > LastAnimFrame)) {
            curAnimFrame = FirstAnimFrame;
        }

        JustPlacedTimer--;
        if((JustPlacedTimer > 0) && (JustPlacedTimer % 2 == 0)) {
            curAnimFrame = 0;
        }
    }

    return true;
}

void StructureClass::destroy()
{
    int*    pDestroyedStructureTiles = NULL;
    int     DestroyedStructureTilesSizeY = 0;
    static int DestroyedStructureTilesWall[] = { DestroyedStructure_Wall };
    static int DestroyedStructureTiles1x1[] = { Destroyed1x1Structure };
    static int DestroyedStructureTiles2x2[] = { Destroyed2x2Structure_TopLeft, Destroyed2x2Structure_TopRight,
                                                Destroyed2x2Structure_BottomLeft, Destroyed2x2Structure_BottomRight };
    static int DestroyedStructureTiles3x2[] = { Destroyed3x2Structure_TopLeft, Destroyed3x2Structure_TopCenter, Destroyed3x2Structure_TopRight,
                                                Destroyed3x2Structure_BottomLeft, Destroyed3x2Structure_BottomCenter, Destroyed3x2Structure_BottomRight};
    static int DestroyedStructureTiles3x3[] = { Destroyed3x3Structure_TopLeft, Destroyed3x3Structure_TopCenter, Destroyed3x3Structure_TopRight,
                                                Destroyed3x3Structure_CenterLeft, Destroyed3x3Structure_CenterCenter, Destroyed3x3Structure_CenterRight,
                                                Destroyed3x3Structure_BottomLeft, Destroyed3x3Structure_BottomCenter, Destroyed3x3Structure_BottomRight};


    if(itemID == Structure_Wall) {
        pDestroyedStructureTiles = DestroyedStructureTilesWall;
        DestroyedStructureTilesSizeY = 1;
    } else {
        switch(structureSize.y) {
            case 1: {
                pDestroyedStructureTiles = DestroyedStructureTiles1x1;
                DestroyedStructureTilesSizeY = 1;
            } break;

            case 2: {
                if(structureSize.x == 2) {
                    pDestroyedStructureTiles = DestroyedStructureTiles2x2;
                    DestroyedStructureTilesSizeY = 2;
                } else if(structureSize.x == 3) {
                    pDestroyedStructureTiles = DestroyedStructureTiles3x2;
                    DestroyedStructureTilesSizeY = 3;
                } else {
                    throw std::runtime_error("StructureClass::destroy(): Invalid structure size");
                }
            } break;

            case 3: {
                pDestroyedStructureTiles = DestroyedStructureTiles3x3;
                DestroyedStructureTilesSizeY = 3;
            } break;

            default: {
                throw std::runtime_error("StructureClass::destroy(): Invalid structure size");
            } break;
        }
    }

    if(itemID != Structure_Wall) {
        for(int j = 0; j < structureSize.y; j++) {
            for(int i = 0; i < structureSize.x; i++) {
                TerrainClass* pTile = currentGameMap->getCell(location.x + i, location.y + j);
                pTile->setDestroyedStructureTile(pDestroyedStructureTiles[DestroyedStructureTilesSizeY*j + i]);

                Coord position((location.x+i)*BLOCKSIZE + BLOCKSIZE/2, (location.y+j)*BLOCKSIZE + BLOCKSIZE/2);
                Uint32 explosionID = currentGame->RandomGen.getRandOf(2,Explosion_Large1,Explosion_Large2);
                currentGame->getExplosionList().push_back(new Explosion(explosionID, position, owner->getHouseID()) );

                if(currentGame->RandomGen.randBool() == true) {
                    UnitClass* pNewUnit = owner->createUnit(Unit_Soldier);
                    pNewUnit->setHealth(pNewUnit->getMaxHealth()/2);
                    pNewUnit->deploy(location + Coord(i,j));
                }
            }
        }
    }

	if(isVisible(pLocalHouse->getTeam()))
		soundPlayer->playSoundAt(Sound_ExplosionStructure, location);


    delete this;
}

Coord StructureClass::getClosestPoint(const Coord& objectLocation) const
{
	Coord closestPoint;

	// find the closest cell of a structure from a location
	if(objectLocation.x <= location.x) {
	    // if we are left of the structure
        // set destination, left most point
		closestPoint.x = location.x;
	} else if(objectLocation.x >= (location.x + structureSize.x-1)) {
	    //vica versa
		closestPoint.x = location.x + structureSize.x-1;
	} else {
        //we are above or below at least on cell of the structure, closest path is straight
		closestPoint.x = objectLocation.x;
	}

	//same deal but with y
	if(objectLocation.y <= location.y) {
		closestPoint.y = location.y;
	} else if(objectLocation.y >= (location.y + structureSize.y-1)) {
		closestPoint.y = location.y + structureSize.y-1;
	} else {
		closestPoint.y = objectLocation.y;
	}

	return closestPoint;
}
