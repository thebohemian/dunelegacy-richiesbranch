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

#ifndef ANIMATIONLABEL_H
#define ANIMATIONLABEL_H

#include <GUI/Widget.h>
#include <FileClasses/Animation.h>

/// A widget for showning an animation
class AnimationLabel : public Widget
{
public:

	/// default constructor
	AnimationLabel() {
		EnableResizing(false,false);
		pAnim = NULL;
	}

	/// destructor
	virtual ~AnimationLabel() { ; };

	/**
		Set the current animation that should be shown in this widget.
		\param newAnimation	the new animation to show
	*/
	void SetAnimation(Animation *newAnimation) {
		pAnim = newAnimation;
	};

	/**
		Draws this widget to screen. This method is called before DrawOverlay().
		\param	screen	Surface to draw on
		\param	Position	Position to draw the widget to
	*/
	virtual inline void Draw(SDL_Surface* screen, Point Position) {
		if(pAnim == NULL) {
			return;
		}

		SDL_Surface* surface = pAnim->getFrame();

		SDL_Rect dest;
		dest.x = Position.x;
		dest.y = Position.y;
		dest.w = surface->w;
		dest.h = surface->h;

		if(IsVisible()) {
			SDL_BlitSurface(surface, NULL, screen, &dest);
		}
	};

	/**
		Returns the minimum size of this animation label. The widget should not
		be resized to a size smaller than this.
		\return the minimum size of this animation label
	*/
	virtual Point GetMinimumSize() const {
		SDL_Surface* surface = pAnim->getFrame();;
		if(surface != NULL) {
			return Point((Sint32) surface->w, (Sint32) surface->h);
		} else {
			return Point(0,0);
		}
	}

private:
	Animation* pAnim;
};

#endif //ANIMATIONLABEL_H
