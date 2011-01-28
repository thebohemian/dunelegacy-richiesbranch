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

#include <RadarView.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>

#include <Game.h>
#include <MapClass.h>
#include <House.h>
#include <ScreenBorder.h>
#include <TerrainClass.h>

#include <misc/draw_util.h>


#define NUM_STATIC_FRAMES 21
#define NUM_STATIC_FRAME_TIME 5

#define RADARVIEW_BORDERTHICKNESS 2

SDL_Rect RadarPosition = { 14, RADARVIEW_BORDERTHICKNESS, 128, 128 };

RadarView::RadarView(int x, int y)
	: currentRadarMode(Mode_RadarOff), animFrame(NUM_STATIC_FRAMES - 1), animCounter(NUM_STATIC_FRAME_TIME)
{
    radarStaticAnimation = pGFXManager->getUIGraphic(UI_RadarAnimation);

    radarPosition.x = 14 + x;
    radarPosition.y = RADARVIEW_BORDERTHICKNESS + y;
    radarPosition.w = 128;
    radarPosition.h = 128;
}


RadarView::~RadarView()
{
}

void RadarView::switchRadarMode(bool bOn)
{
    if(bOn == true) {
        currentRadarMode = Mode_AnimationRadarOn;
    } else {
        currentRadarMode = Mode_AnimationRadarOff;
    }
}

void RadarView::setRadarMode(bool bStatus)
{
    if(bStatus == true) {
        currentRadarMode = Mode_RadarOn;
        animFrame = 0;
        animCounter = NUM_STATIC_FRAME_TIME;
    } else {
        currentRadarMode = Mode_RadarOff;
        animFrame = NUM_STATIC_FRAMES - 1;
        animCounter = NUM_STATIC_FRAME_TIME;
    }
}

void RadarView::draw() const
{
    switch(currentRadarMode) {
        case Mode_RadarOff:
        case Mode_RadarOn:
        {
            int MapSizeX = currentGameMap->sizeX;
            int MapSizeY = currentGameMap->sizeY;

            int scale = 1;
            int offsetX = 0;
            int offsetY = 0;

            calculateScaleAndOffsets(MapSizeX, MapSizeY, scale, offsetX, offsetY);

            // Lock the screen for direct access to the pixels
            if (!SDL_MUSTLOCK(screen) || (SDL_LockSurface(screen) == 0)) {
                for (int x = 0; x <  MapSizeX; x++) {
                    for (int y = 0; y <  MapSizeY; y++) {

                        TerrainClass* tempTerrain = &currentGameMap->cell[x][y];

                        /* Selecting the right color is handled in TerrainClass::getRadarColor() */
                        Uint32 color = tempTerrain->getRadarColor(pLocalHouse, pLocalHouse->hasRadarOn());

                        for(int i = 0; i < scale; i++) {
                            for(int j = 0; j < scale; j++) {
                                putpixel(   screen,
                                            radarPosition.x + offsetX + scale*x + i,
                                            radarPosition.y + offsetY + scale*y + j,
                                            color);
                            }
                        }
                    }
                }

				if (SDL_MUSTLOCK(screen))
					SDL_UnlockSurface(screen);
            }

            SDL_Rect RadarRect;
            RadarRect.x = (screenborder->getLeft() * MapSizeX*scale) / (MapSizeX*BLOCKSIZE) + offsetX;
            RadarRect.y = (screenborder->getTop() * MapSizeY*scale) / (MapSizeY*BLOCKSIZE) + offsetY;
            RadarRect.w = ((screenborder->getRight() - screenborder->getLeft()) * MapSizeX*scale) / (MapSizeX*BLOCKSIZE);
            RadarRect.h = ((screenborder->getBottom() - screenborder->getTop()) * MapSizeY*scale) / (MapSizeY*BLOCKSIZE);

            if(RadarRect.x < offsetX) {
                RadarRect.w -= RadarRect.x;
                RadarRect.x = offsetX;
            }

            if(RadarRect.y < offsetY) {
                RadarRect.h -= RadarRect.y;
                RadarRect.y = offsetY;
            }

            int offsetFromRightX = 128 - MapSizeX*scale - offsetX;
            if(RadarRect.x + RadarRect.w > radarPosition.w - offsetFromRightX) {
                RadarRect.w  = radarPosition.w - offsetFromRightX - RadarRect.x - 1;
            }

            int offsetFromBottomY = 128 - MapSizeY*scale - offsetY;
            if(RadarRect.y + RadarRect.h > radarPosition.h - offsetFromBottomY) {
                RadarRect.h = radarPosition.h - offsetFromBottomY - RadarRect.y - 1;
            }

            drawrect(   screen,
                        radarPosition.x + RadarRect.x,
                        radarPosition.y + RadarRect.y,
                        radarPosition.x + (RadarRect.x + RadarRect.w),
                        radarPosition.y + (RadarRect.y + RadarRect.h),
                        COLOR_WHITE);

        } break;

        case Mode_AnimationRadarOff:
        case Mode_AnimationRadarOn:
        {
            int imageW = radarStaticAnimation->w / NUM_STATIC_FRAMES;
            int imageH = radarStaticAnimation->h;
            SDL_Rect source = { animFrame*imageW, 0, imageW, imageH };
            SDL_Rect dest = { radarPosition.x, radarPosition.y, imageW, imageH };
            SDL_BlitSurface(radarStaticAnimation, &source, screen, &dest);
        } break;
    }
}

void RadarView::update()
{
    switch(currentRadarMode) {
        case Mode_RadarOff: {

        } break;

        case Mode_RadarOn: {

        } break;

        case Mode_AnimationRadarOff: {
            if(animFrame >= NUM_STATIC_FRAMES-1) {
                currentRadarMode = Mode_RadarOff;
            } else {
                animCounter--;
                if(animCounter <= 0) {
                    animFrame++;
                    animCounter = NUM_STATIC_FRAME_TIME;
                }
            }
        } break;

        case Mode_AnimationRadarOn: {
            if(animFrame <= 0) {
                currentRadarMode = Mode_RadarOn;
            } else {
                animCounter--;
                if(animCounter <= 0) {
                    animFrame--;
                    animCounter = NUM_STATIC_FRAME_TIME;
                }
            }
        } break;
    }
}

Coord RadarView::getWorldCoords(int mouseX, int mouseY) const
{
    Coord positionOnRadar(mouseX - radarPosition.x, mouseY - radarPosition.y);

    int scale = 1;
    int offsetX = 0;
    int offsetY = 0;

    calculateScaleAndOffsets(currentGameMap->sizeX, currentGameMap->sizeY, scale, offsetX, offsetY);

    return Coord( ((positionOnRadar.x - offsetX) * currentGameMap->sizeX * BLOCKSIZE) / (currentGameMap->sizeX * scale),
                  ((positionOnRadar.y - offsetY) * currentGameMap->sizeY * BLOCKSIZE) / (currentGameMap->sizeY * scale));
}

bool RadarView::isOnRadar(int mouseX, int mouseY) const
{
    int scale = 1;
    int offsetX = 0;
    int offsetY = 0;

    calculateScaleAndOffsets(currentGameMap->sizeX, currentGameMap->sizeY, scale, offsetX, offsetY);

    int offsetFromRightX = 128 - currentGameMap->sizeX*scale - offsetX;
    int offsetFromBottomY = 128 - currentGameMap->sizeY*scale - offsetY;

    return ((mouseX >= radarPosition.x + offsetX - RADARVIEW_BORDERTHICKNESS)
            && (mouseX < radarPosition.x + RadarPosition.w - offsetFromRightX + 2*RADARVIEW_BORDERTHICKNESS)
            && (mouseY >= radarPosition.y + offsetY - RADARVIEW_BORDERTHICKNESS)
            && (mouseY < radarPosition.y + RadarPosition.h - offsetFromBottomY + 2*RADARVIEW_BORDERTHICKNESS) );
}
