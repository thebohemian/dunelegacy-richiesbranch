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

#include <CutScenes/CrossBlendVideoEvent.h>
#include <misc/draw_util.h>

CrossBlendVideoEvent::CrossBlendVideoEvent(SDL_Surface* pSourceSurface, SDL_Surface* pDestSurface, bool bFreeSurfaces, bool bCenterVertical) : VideoEvent()
{
    this->pSourceSurface = DoublePicture(pSourceSurface, bFreeSurfaces);
    this->pDestSurface = DoublePicture(pDestSurface, bFreeSurfaces);
    this->bFreeSurfaces = bFreeSurfaces;
    this->bCenterVertical = bCenterVertical;
    currentFrame = 0;

    SDL_Rect dest = {	0,0, this->pSourceSurface->w, this->pSourceSurface->h};
    pBlendBlitter = new BlendBlitter(this->pDestSurface, this->pSourceSurface, dest, 30);
}

CrossBlendVideoEvent::~CrossBlendVideoEvent()
{
    SDL_FreeSurface(pSourceSurface);
    SDL_FreeSurface(pDestSurface);

    delete pBlendBlitter;
    pBlendBlitter = NULL;
}

int CrossBlendVideoEvent::draw(SDL_Surface* pScreen)
{
	if(pBlendBlitter->nextStep() == 0) {
		delete pBlendBlitter;
		pBlendBlitter = NULL;
    }

    SDL_Rect dest;
    dest.x = (pScreen->w - pSourceSurface->w) / 2;
    dest.y = bCenterVertical ? (pScreen->h - pSourceSurface->h) / 2 : 0;
    dest.w = pSourceSurface->w;
    dest.h = pSourceSurface->h;

    SDL_BlitSurface(pSourceSurface,NULL,pScreen,&dest);

    currentFrame++;

    return 100;
}

bool CrossBlendVideoEvent::isFinished()
{
    return (pBlendBlitter == NULL);
}
