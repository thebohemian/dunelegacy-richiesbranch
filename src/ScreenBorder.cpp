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

#include <ScreenBorder.h>

#include <globals.h>

#include <MapClass.h>
#include <algorithm>

void ScreenBorder::SetNewScreenCenter(const Coord& newPosition) {
    Coord currentBorderSize = BottomRightCorner - TopLeftCorner;

    TopLeftCorner = newPosition - currentBorderSize/2;
    BottomRightCorner = newPosition + currentBorderSize/2;

    int worldSizeX = currentGameMap->sizeX * BLOCKSIZE;
    int worldSizeY = currentGameMap->sizeY * BLOCKSIZE;

    if((TopLeftCorner.x < 0) && (BottomRightCorner.x >= worldSizeX)) {
        // screen is wider than map
        TopLeftCorner.x = 0;
        BottomRightCorner.x = worldSizeX;
    } else if(TopLeftCorner.x < 0) {
        // leaving the map at the left border
        BottomRightCorner.x -= TopLeftCorner.x;
        TopLeftCorner.x = 0;
    } else if(BottomRightCorner.x >= worldSizeX) {
        // leaving the map at the right border
        TopLeftCorner.x -= (BottomRightCorner.x - worldSizeX);
        BottomRightCorner.x = worldSizeX;
    }

    if((TopLeftCorner.y < 0) && (BottomRightCorner.y >= worldSizeY)) {
        // screen is higher than map
        TopLeftCorner.y = 0;
        BottomRightCorner.y = worldSizeY;
    } else if(TopLeftCorner.y < 0) {
        // leaving the map at the top border
        BottomRightCorner.y -= TopLeftCorner.y;
        TopLeftCorner.y = 0;
    } else if(BottomRightCorner.y >= worldSizeY) {
        // leaving the map at the bottom border
        TopLeftCorner.y -= (BottomRightCorner.y - worldSizeY);
        BottomRightCorner.y = worldSizeY;
    }
}

bool ScreenBorder::scrollLeft() {
    if(TopLeftCorner.x > 0) {
        int scrollAmount = std::min(SCROLLSPEED, TopLeftCorner.x);
        TopLeftCorner.x -= scrollAmount;
        BottomRightCorner.x -= scrollAmount;
        return true;
    } else {
        return false;
    }
}

bool ScreenBorder::scrollRight() {
    if(BottomRightCorner.x < currentGameMap->sizeX*BLOCKSIZE-1) {
        int scrollAmount = std::min(SCROLLSPEED,currentGameMap->sizeX*BLOCKSIZE-1-BottomRightCorner.x);
        TopLeftCorner.x += scrollAmount;
        BottomRightCorner.x += scrollAmount;
        return true;
    } else {
        return false;
    }
}

bool ScreenBorder::scrollUp() {
    if(TopLeftCorner.y > 0) {
        int scrollAmount = std::min(SCROLLSPEED, TopLeftCorner.y);
        TopLeftCorner.y -= scrollAmount;
        BottomRightCorner.y -= scrollAmount;
        return true;
    } else {
        return false;
    }
}

bool ScreenBorder::scrollDown() {
    if(BottomRightCorner.y < currentGameMap->sizeY*BLOCKSIZE-1) {
        int scrollAmount = std::min(SCROLLSPEED,currentGameMap->sizeY*BLOCKSIZE-1-BottomRightCorner.y);
        TopLeftCorner.y += scrollAmount;
        BottomRightCorner.y += scrollAmount;
        return true;
    } else {
        return false;
    }
}

void ScreenBorder::adjustScreenBorderToMapsize() {
    int worldSizeX = currentGameMap->sizeX * BLOCKSIZE;
    int worldSizeY = currentGameMap->sizeY * BLOCKSIZE;

    if(worldSizeX >= gameBoardRect.w) {
        TopLeftCorner.x = 0;
        BottomRightCorner.x = gameBoardRect.w;
        TopLeftCornerOnScreen.x = gameBoardRect.x;
        BottomRightCornerOnScreen.x = gameBoardRect.x + gameBoardRect.w;
    } else {
        TopLeftCorner.x = 0;
        BottomRightCorner.x = worldSizeX;
        TopLeftCornerOnScreen.x = gameBoardRect.x + (gameBoardRect.w - worldSizeX) / 2;
        BottomRightCornerOnScreen.x = gameBoardRect.x + (gameBoardRect.w - worldSizeX) / 2 + worldSizeX;
    }

    if(worldSizeY >= gameBoardRect.h) {
        TopLeftCorner.y = 0;
        BottomRightCorner.y = gameBoardRect.h;
        TopLeftCornerOnScreen.y = gameBoardRect.y;
        BottomRightCornerOnScreen.y = gameBoardRect.y + gameBoardRect.h;
    } else {
        TopLeftCorner.y = 0;
        BottomRightCorner.y = worldSizeY;
        TopLeftCornerOnScreen.y = gameBoardRect.y + (gameBoardRect.h - worldSizeY) / 2;
        BottomRightCornerOnScreen.y = gameBoardRect.y + (gameBoardRect.h - worldSizeY) / 2 + worldSizeY;
    }
}
