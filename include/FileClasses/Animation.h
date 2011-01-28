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

#ifndef ANIMATION_H
#define ANIMATION_H

#include <FileClasses/Palette.h>

#include <SDL.h>
#include <vector>

class Animation
{
public:
	Animation();
	~Animation();

	SDL_Surface* getFrame();

	void setFrameRate(double FrameRate) {
		if(FrameRate == 0.0) {
			FrameDurationTime = 1;
		} else {
			FrameDurationTime = (int) (1000.0/FrameRate);
		}
	}

	void addFrame(SDL_Surface* newFrame, bool DoublePic = false,bool SetColorKey = false);

	void setPalette(const Palette& newPalette);

private:
	Uint32 CurFrameStartTime;
	Uint32 FrameDurationTime;
	unsigned int curFrame;
	std::vector<SDL_Surface*> Frames;
};

#endif // ANIMATION_H
