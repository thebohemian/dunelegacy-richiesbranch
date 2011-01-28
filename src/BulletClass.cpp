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

#include <BulletClass.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <SoundPlayer.h>
#include <ObjectClass.h>
#include <Game.h>
#include <MapClass.h>
#include <House.h>
#include <Explosion.h>
#include <misc/draw_util.h>


BulletClass::BulletClass(Uint32 shooterID, Coord* newRealLocation, Coord* newRealDestination, Uint32 bulletID, int damage, bool air)
{
    airAttack = air;

    this->shooterID = shooterID;

    ObjectClass* pShooter = currentGame->getObjectManager().getObject(shooterID);
    if(pShooter == NULL) {
        owner = NULL;
    } else {
        owner = pShooter->getOwner();
    }

	this->bulletID = bulletID;

    this->damage = damage;

    int inaccuracy = BulletClass::init();

    destination.x = newRealDestination->x + currentGame->RandomGen.rand(-inaccuracy, inaccuracy);
	destination.y = newRealDestination->y + currentGame->RandomGen.rand(-inaccuracy, inaccuracy);

	if(bulletID == Bullet_Sonic) {
		int diffX = destination.x - newRealLocation->x;
        int diffY = destination.y - newRealLocation->y;

		double ratio;
		double square_root;

		int weaponrange = currentGame->objectData.data[Unit_SonicTank].weaponrange;

		if((diffX == 0) && (diffY == 0)) {
			diffY = weaponrange*BLOCKSIZE;
		}

		square_root = sqrt((double)(diffX*diffX + diffY*diffY));
		ratio = (weaponrange*BLOCKSIZE)/square_root;
		destination.x = newRealLocation->x + (int)(((double)diffX)*ratio);
		destination.y = newRealLocation->y + (int)(((double)diffY)*ratio);
	}

	realX = (double)newRealLocation->x;
	realY = (double)newRealLocation->y;
	source.x = newRealLocation->x;
	source.y = newRealLocation->y;
	location.x = newRealLocation->x/BLOCKSIZE;
	location.y = newRealLocation->y/BLOCKSIZE;

	angle = dest_angle(*newRealLocation, destination);
	drawnAngle = (int)((double)numFrames*angle/256.0);

    xSpeed = speed * cos(angle * conv2char);
	ySpeed = speed * -sin(angle * conv2char);
}

BulletClass::BulletClass(Stream& stream)
{
	bulletID = stream.readUint32();

	airAttack = stream.readBool();
	damage = stream.readSint32();

	shooterID = stream.readUint32();
	Uint32 x = stream.readUint32();
	if(x < MAX_PLAYERS) {
		owner = currentGame->house[x];
	} else {
		owner = currentGame->house[0];
	}

    source.x = stream.readSint32();
	source.y = stream.readSint32();
	destination.x = stream.readSint32();
	destination.y = stream.readSint32();
    location.x = stream.readSint32();
	location.y = stream.readSint32();
	realX = stream.readDouble();
	realY = stream.readDouble();

    xSpeed = stream.readDouble();
	ySpeed = stream.readDouble();

	drawnAngle = stream.readSint8();
    angle = stream.readDouble();

	BulletClass::init();
}

int BulletClass::init()
{
    groundBlocked = false;
	int inaccuracy;

	int houseID = (owner == NULL) ? HOUSE_HARKONNEN : owner->getHouseID();

	switch(bulletID) {
        case(Bullet_Inf_Gun): {
            damageRadius = 2;
            groundBlocked = true;
            inaccuracy = 2;
            speed = 9.0;
            numFrames = 1;
            graphic = pGFXManager->getObjPic(ObjPic_Bullet_Small, houseID);
        } break;

        case(Bullet_Gun): {
            damageRadius = 2;
            groundBlocked = true;
            inaccuracy = 2;
            speed = 9.0;
            numFrames = 1;
            graphic = pGFXManager->getObjPic(ObjPic_Bullet_Small, houseID);
        } break;

        case(Bullet_DRocket): {
            damageRadius = 16;
            inaccuracy = 16;
            speed = 4.0;
            numFrames = 16;
            graphic = pGFXManager->getObjPic(ObjPic_Bullet_MediumRocket, houseID);
        } break;

        case(Bullet_LargeRocket): {
            damageRadius = 16;
            inaccuracy = 0;
            speed = 4.0;
            numFrames = 16;
            graphic = pGFXManager->getObjPic(ObjPic_Bullet_LargeRocket, houseID);
        } break;

        case(Bullet_Rocket): {
            damageRadius = 4;
            ObjectClass* pShooter = currentGame->getObjectManager().getObject(shooterID);
            if((pShooter != NULL) && (pShooter->getItemID() == Unit_Launcher)) {
                inaccuracy = 16;
            } else {
                inaccuracy = 8;
            }
            speed = 5.0;
            numFrames = 16;
            graphic = pGFXManager->getObjPic(ObjPic_Bullet_MediumRocket, houseID);
        } break;

        case(Bullet_Shell): {
            damageRadius = 3;
            groundBlocked = true;
            inaccuracy = 4;
            speed = 9.0;
            numFrames = 1;
            graphic = pGFXManager->getObjPic(ObjPic_Bullet_Medium, houseID);
        } break;

        case(Bullet_SmallRocket): {
            damageRadius = 5;
            inaccuracy = 6;
            speed = 4.0;
            numFrames = 16;
            graphic = pGFXManager->getObjPic(ObjPic_Bullet_SmallRocket, houseID);
        } break;

        case(Bullet_SmallRocketHeavy): {
            damageRadius = 5;
            inaccuracy = 6;
            speed = 4.0;
            numFrames = 16;
            graphic = pGFXManager->getObjPic(ObjPic_Bullet_SmallRocket, houseID);
        } break;

        case(Bullet_Sonic): {
            damageRadius = 7;
            inaccuracy = 2;
            speed = 2.0;
            numFrames = 1;
            SDL_Surface* tmpSurface = pGFXManager->getObjPic(ObjPic_Bullet_Sonic, houseID);
            graphic =  copySurface(tmpSurface);	//make a copy of the image
        } break;

        case(Bullet_Sandworm): {
            fprintf(stderr,"BulletClass: Unknown Bullet_Sandworm not allowed.\n");
            inaccuracy = 0;
            graphic = NULL;
        } break;

        default: {
            fprintf(stderr,"BulletClass: Unknown Bullet type %d.\n",bulletID);
            inaccuracy = 0;
            graphic = NULL;
        } break;
	}

	imageW = graphic->w/numFrames;
	imageH = graphic->h;

	return inaccuracy;
}


BulletClass::~BulletClass()
{
    if((bulletID == Bullet_Sonic) && (graphic != NULL)) {
        SDL_FreeSurface(graphic);
        graphic = NULL;
    }
}

void BulletClass::save(Stream& stream) const
{
	stream.writeUint32(bulletID);

	stream.writeBool(airAttack);
    stream.writeSint32(damage);

    stream.writeUint32(shooterID);
    stream.writeUint32(owner->getHouseID());

	stream.writeSint32(source.x);
	stream.writeSint32(source.y);
	stream.writeSint32(destination.x);
	stream.writeSint32(destination.y);
    stream.writeSint32(location.x);
	stream.writeSint32(location.y);
	stream.writeDouble(realX);
	stream.writeDouble(realY);

    stream.writeDouble(xSpeed);
	stream.writeDouble(ySpeed);

	stream.writeSint8(drawnAngle);
    stream.writeDouble(angle);
}


void BulletClass::blitToScreen()
{
    SDL_Rect source = { (numFrames > 1) ? drawnAngle * imageW : 0, 0, imageW, imageH };
	SDL_Rect dest = { getDrawnX(), getDrawnY(), imageW, imageH };

    if(bulletID == Bullet_Sonic) {
        SDL_Surface *mask = pGFXManager->getObjPic(ObjPic_Bullet_Sonic,(owner == NULL) ? HOUSE_HARKONNEN : owner->getHouseID());
        if(mask->format->BitsPerPixel == 8) {
            if ((!SDL_MUSTLOCK(screen) || (SDL_LockSurface(screen) == 0))
                && (!SDL_MUSTLOCK(mask) || (SDL_LockSurface(mask) == 0))
                && (!SDL_MUSTLOCK(graphic) || (SDL_LockSurface(graphic) == 0)))
            {
                unsigned char	*maskPixels = (unsigned char*)mask->pixels,
                                *screenPixels = (unsigned char*)screen->pixels,
                                *surfacePixels = (unsigned char*)graphic->pixels;
                int	i,j, x,y, maxX = screenborder->getRight();
                for(i = 0; i < dest.w; i++) {
                    for (j = 0; j < dest.h; j++) {
                        if(maskPixels[i + j*mask->pitch] == 0) {
                            //not masked, direct copy
                            x = i;
                            y = j;
                        } else {
                            x = i + getRandomInt(-3, 3);
                            y = j + getRandomInt(-3, 3);
                        }

                        if(dest.x < 0) {
                            dest.x = 0;
                        } else if (dest.x >= maxX) {
                            dest.x = maxX - 1;
                        }

                        if(x < 0) {
                            x = 0;
                        } else if (dest.x + x >= maxX) {
                            x = maxX - dest.x - 1;
                        }

                        if(dest.y < 0) {
                            dest.y = 0;
                        } else if (dest.y >= screen->h) {
                            dest.y = screen->h - 1;
                        }

                        if(y < 0) {
                            y = 0;
                        } else if (dest.y + y >= screen->h) {
                            x = screen->h - dest.y - 1;
                        }

                        if((dest.x + x >= 0) && (dest.x + x < screen->w) && (dest.y + y >= 0) && (dest.y + y < screen->h)) {
                            surfacePixels[i + j*graphic->pitch] = screenPixels[dest.x + x + (dest.y + y)*screen->pitch];
                        } else {
                            surfacePixels[i + j*graphic->pitch] = 0;
                        }
                    }
                }

                if (SDL_MUSTLOCK(graphic))
                    SDL_UnlockSurface(graphic);

                if (SDL_MUSTLOCK(mask))
                    SDL_UnlockSurface(mask);

                if (SDL_MUSTLOCK(screen))
                    SDL_UnlockSurface(screen);
            }
        }
    }//end of if sonic

    SDL_BlitSurface(graphic, &source, screen, &dest);
}


void BulletClass::update()
{
	Coord oldLocation = location;

	realX += xSpeed;  //keep the bullet moving by its current speeds
	realY += ySpeed;
	location.x = (short)(realX/BLOCKSIZE);
	location.y = (short)(realY/BLOCKSIZE);

	if((location.x < -5) || (location.x >= currentGameMap->sizeX + 5) || (location.y < -5) || (location.y >= currentGameMap->sizeY + 5)) {
        // it's off the screen => delete it
        bulletList.remove(this);
        delete this;
	} else {
		if(distance_from(source.x, source.y, realX, realY) >= distance_from(source, destination))	{
			realX = destination.x;
			realY = destination.y;
			destroy();
		} else if(bulletID == Bullet_Sonic) {
			//if((location.x != oldLocation.x) || (location.y != oldLocation.y)) {
				Coord realPos = Coord((short)realX, (short)realY);
				currentGameMap->damage(shooterID, owner, realPos, bulletID,
                                        (int) (((double)damage/2.0)/((double)currentGame->objectData.data[Unit_SonicTank].weaponrange)),
                                        damageRadius, false);

                realX += xSpeed;  //keep the bullet moving by its current speeds
                realY += ySpeed;

				realPos = Coord((short)realX, (short)realY);
				currentGameMap->damage(shooterID, owner, realPos, bulletID,
                                        (int) (((double)damage/2.0)/((double)currentGame->objectData.data[Unit_SonicTank].weaponrange)),
                                        damageRadius, false);
			//}
		} else if( currentGameMap->cellExists(location)
                    && currentGameMap->getCell(location)->hasAGroundObject()
                    && currentGameMap->getCell(location)->getGroundObject()->isAStructure()) {
			if(groundBlocked) {
				destroy();
			}
		}
	}
}


void BulletClass::destroy()
{
    Coord position = Coord((short)realX, (short)realY);

    int houseID = (owner == NULL) ? HOUSE_HARKONNEN : owner->getHouseID();

    switch(bulletID) {
        case Bullet_Inf_Gun:
        case Bullet_Gun:
        {
            currentGameMap->damage(shooterID, owner, position, bulletID, damage, damageRadius, airAttack);
        } break;

        case Bullet_DRocket: {
            currentGameMap->damage(shooterID, owner, position, bulletID, damage, damageRadius, airAttack);
            soundPlayer->playSound(Sound_ExplosionGas);
            currentGame->getExplosionList().push_back(new Explosion(Explosion_Gas,position,houseID));
        } break;

        case Bullet_LargeRocket: {
            soundPlayer->playSound(Sound_ExplosionLarge);

            for(int i = 0; i < 5; i++) {
                for(int j = 0; j < 5; j++) {
                    if (((i != 0) && (i != 4)) || ((j != 0) && (j != 4))) {
                        position.x = (short)realX + (i - 2)*BLOCKSIZE;
                        position.y = (short)realY + (j - 2)*BLOCKSIZE;

                        currentGameMap->damage(shooterID, owner, position, bulletID, damage, damageRadius, airAttack);

                        Uint32 explosionID = currentGame->RandomGen.getRandOf(2,Explosion_Large1,Explosion_Large2);
                        currentGame->getExplosionList().push_back(new Explosion(explosionID,position,houseID));
                    }
                }
            }
        } break;

        case Bullet_Rocket:
        case Bullet_SmallRocket:
        case Bullet_SmallRocketHeavy:
        {
            currentGameMap->damage(shooterID, owner, position, bulletID, damage, damageRadius, airAttack);
            currentGame->getExplosionList().push_back(new Explosion(Explosion_Small,position,houseID));
        } break;

        case Bullet_Shell: {
            currentGameMap->damage(shooterID, owner, position, bulletID, damage, damageRadius, airAttack);
            currentGame->getExplosionList().push_back(new Explosion(Explosion_Shell,position,houseID));
        } break;

        case Bullet_Sonic:
        case Bullet_Sandworm:
        default: {
            // do nothing
        } break;
    }

    if((bulletID == Bullet_Sonic) && (graphic != NULL)) {
        SDL_FreeSurface(graphic);
        graphic = NULL;
    }

    bulletList.remove(this);
    delete this;
}

