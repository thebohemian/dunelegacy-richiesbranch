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

#include <Explosion.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <Game.h>
#include <ScreenBorder.h>

#define CYCLES_PER_FRAME    5

Explosion::Explosion()
 : explosionID(NONE), house(HOUSE_HARKONNEN)
{
    graphic = NULL;
    numFrames = 0;
    FrameTimer = CYCLES_PER_FRAME;
    currentFrame = 0;
}

Explosion::Explosion(Uint32 explosionID, const Coord& position, int house)
 : explosionID(explosionID), position(position) , house(house)
{
    init();

    FrameTimer = CYCLES_PER_FRAME;
    currentFrame = 0;
}

Explosion::Explosion(Stream& stream)
{
    explosionID = stream.readUint32();
    position.x = stream.readSint16();
	position.y = stream.readSint16();
	house = stream.readUint32();
    FrameTimer = stream.readSint32();
    currentFrame = stream.readSint32();

    init();
}

Explosion::~Explosion()
{

}

void Explosion::init()
{
    switch(explosionID) {
        case Explosion_Small: {
            graphic = pGFXManager->getObjPic(ObjPic_ExplosionSmall);
            numFrames = 5;
        } break;

        case Explosion_Medium1: {
            graphic = pGFXManager->getObjPic(ObjPic_ExplosionMedium1);
            numFrames = 5;
        } break;

        case Explosion_Medium2: {
            graphic = pGFXManager->getObjPic(ObjPic_ExplosionMedium2);
            numFrames = 5;
        } break;

        case Explosion_Large1: {
            graphic = pGFXManager->getObjPic(ObjPic_ExplosionLarge1);
            numFrames = 5;
        } break;

        case Explosion_Large2: {
            graphic = pGFXManager->getObjPic(ObjPic_ExplosionLarge2);
            numFrames = 5;
        } break;

        case Explosion_Gas: {
            graphic = pGFXManager->getObjPic(ObjPic_Hit_Gas, house);
            numFrames = 5;
        } break;

        case Explosion_Shell: {
            graphic = pGFXManager->getObjPic(ObjPic_Hit_Shell);
            numFrames = 3;
        } break;

        case Explosion_SmallUnit: {
            graphic = pGFXManager->getObjPic(ObjPic_ExplosionSmallUnit);
            numFrames = 2;
        } break;

        case Explosion_Flames: {
            graphic = pGFXManager->getObjPic(ObjPic_ExplosionFlames);
            numFrames = 21;
        } break;

        default: {
            fprintf(stderr,"Explosion: Unknown explosion type %d.\n",explosionID);
            exit(EXIT_FAILURE);
        } break;
    }
}

void Explosion::save(Stream& stream) const
{
    stream.writeUint32(explosionID);
    stream.writeSint16(position.x);
	stream.writeSint16(position.y);
	stream.writeUint32(house);
    stream.writeSint32(FrameTimer);
    stream.writeSint32(currentFrame);
}

void Explosion::blitToScreen() const
{
    SDL_Rect dest;
    SDL_Rect source;

    source.w = dest.w = graphic->w/numFrames;
    source.h = dest.h = graphic->h;
    source.x = source.w * currentFrame;
    source.y = 0;
    dest.x = screenborder->world2screenX(position.x - source.w/2);
    dest.y = screenborder->world2screenY(position.y - source.h/2);

    if(screenborder->isInsideScreen(position, Coord(source.w, source.h))) {
        SDL_BlitSurface(graphic, &source, screen, &dest);
    }
}

void Explosion::update()
{
    FrameTimer--;

    if(FrameTimer < 0) {
        FrameTimer = CYCLES_PER_FRAME;
        currentFrame++;

        if(currentFrame >= numFrames) {
            //this explosion is finished
            currentGame->getExplosionList().remove(this);
            delete this;
        }
    }
}
