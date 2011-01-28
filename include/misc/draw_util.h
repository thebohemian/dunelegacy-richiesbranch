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

#ifndef DRAW_UTIL_H
#define DRAW_UTIL_H

#include <SDL.h>

void mapImageHouseColor(SDL_Surface* graphic, int house);
void mapImageHouseColorBase(SDL_Surface* graphic, int house, int baseCol);
SDL_Surface*	MapImageHouseColor(SDL_Surface* source, int house, int baseCol);
SDL_Surface*    CreateShadowSurface(SDL_Surface* source);

// Image manipulation functions
SDL_Surface*	DoublePicture(SDL_Surface* inputPic, bool bFreeInputPic = true);
SDL_Surface*	RotatePictureLeft(SDL_Surface* inputPic, bool bFreeInputPic = true);
SDL_Surface*	RotatePictureRight(SDL_Surface* inputPic, bool bFreeInputPic = true);
SDL_Surface*	GetSubPicture(SDL_Surface* Pic, unsigned int left, unsigned int top,
										unsigned int width, unsigned int height);
SDL_Surface*	FlipHPicture(SDL_Surface* inputPic, bool bFreeInputPic = true);
SDL_Surface*	FlipVPicture(SDL_Surface* inputPic, bool bFreeInputPic = true);

void putpixel(SDL_Surface *surface, int x, int y, Uint32 color);
Uint32 getpixel(SDL_Surface *surface, int x, int y);
void hlineNoLock(SDL_Surface *surface, int x1, int y, int x2, Uint32 color);
void vlineNoLock(SDL_Surface *surface, int x, int y1, int y2, Uint32 color);
void drawhline(SDL_Surface *surface, int x1, int y, int x2, Uint32 color);
void drawvline(SDL_Surface *surface, int x, int y1, int y2, Uint32 color);
void drawrect(SDL_Surface *surface, int x1, int y1, int x2, int y2, Uint32 color);

SDL_Surface* copySurface(SDL_Surface* inSurface);
SDL_Surface* scaleSurface(SDL_Surface *surf, double ratio);

#endif // DRAW_UTIL_H
