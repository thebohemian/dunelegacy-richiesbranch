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

#ifndef DIGITSCOUNTER_H
#define DIGITSCOUNTER_H

#include <GUI/Widget.h>

#include <FileClasses/GFXManager.h>

extern GFXManager* pGFXManager;


/// A widget for showing digits (like the credits in dune are shown)
class DigitsCounter : public Widget
{
public:

	/// default constructor
	DigitsCounter() {
		EnableResizing(false,false);
		count = 0;
	}

	/// destructor
	virtual ~DigitsCounter() { ; };

	/**
		Get the current count of this digits counter
		\return	the number that this digits counter currently shows
	*/
	inline unsigned int GetCount() { return count; }

	/**
		Set the count of this digits counter
		\param	newCount	the new number to show
	*/
	inline void SetCount(unsigned int newCount) { count = newCount; }

	/**
		Draws this widget to screen. This method is called before DrawOverlay().
		\param	screen	Surface to draw on
		\param	Position	Position to draw the widget to
	*/
	virtual inline void Draw(SDL_Surface* screen, Point Position) {
		SDL_Rect	dest;
		SDL_Rect	source;
		SDL_Surface* surface;

		surface = pGFXManager->getUIGraphic(UI_MissionSelect);
		dest.x = Position.x;
		dest.y = Position.y;
		dest.w = surface->w;
		dest.h = surface->h;

		SDL_BlitSurface(surface, NULL, screen, &dest);


		char creditsBuffer[3];
		sprintf(creditsBuffer, "%d", count);
		int digits = strlen(creditsBuffer);
		surface = pGFXManager->getUIGraphic(UI_CreditsDigits);
		source.w = dest.w = surface->w/10;
		source.h = dest.h = surface->h;
		source.y = 0;
		dest.y = Position.y + 16;
		for (int i=digits-1; i>=0; i--)
		{
			source.x = atoi(&creditsBuffer[i])*source.w;
			creditsBuffer[i] = '\0';	//so next round atoi only does one character
			dest.x = Position.x + 40 + (6 - digits + i)*10;
			SDL_BlitSurface(surface, &source, screen, &dest);
		}

	};

	/**
		Returns the minimum size of this digits counter. The widget should not
		be resized to a size smaller than this.
		\return the minimum size of this digits counter
	*/
	virtual Point GetMinimumSize() const {
		SDL_Surface* surface = pGFXManager->getUIGraphic(UI_MissionSelect);
		if(surface != NULL) {
			return Point((Sint32) surface->w, (Sint32) surface->h);
		} else {
			return Point(0,0);
		}
	}

private:

	unsigned int count;
};

#endif // DIGITSCOUNTER_H
