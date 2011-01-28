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

#include <FileClasses/Animation.h>

#include <misc/draw_util.h>

#include <stdio.h>
#include <stdlib.h>

Animation::Animation() {
	CurFrameStartTime = SDL_GetTicks();
	FrameDurationTime = 1;
	curFrame = 0;
}

Animation::~Animation() {
    while(Frames.empty() == false) {
        SDL_FreeSurface(Frames.back());
        Frames.pop_back();
    }
}

SDL_Surface* Animation::getFrame() {
    if(Frames.empty()) {
		return NULL;
	}

	if((SDL_GetTicks() - CurFrameStartTime) > FrameDurationTime) {
		CurFrameStartTime = SDL_GetTicks();
		curFrame++;
		if(curFrame >= Frames.size()) {
			curFrame = 0;
		}
	}
	return Frames[curFrame];
}

void Animation::addFrame(SDL_Surface* newFrame, bool DoublePic,bool SetColorKey) {
	if(DoublePic == true) {
	    newFrame = DoublePicture(newFrame);
	}

	if(SetColorKey == true) {
		SDL_SetColorKey(newFrame, SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
	}

	Frames.push_back(newFrame);
}

void Animation::setPalette(const Palette& newPalette) {
    std::vector<SDL_Surface*>::const_iterator iter;
    for(iter = Frames.begin(); iter != Frames.end(); ++iter) {
        newPalette.applyToSurface(*iter);
    }
}
