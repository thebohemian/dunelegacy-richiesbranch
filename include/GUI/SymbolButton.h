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

#ifndef SYMBOLBUTTON_H
#define SYMBOLBUTTON_H

#include "Button.h"

#include <SDL.h>

/// A class for a symbol button
class SymbolButton : public Button {
public:
	/// Default contructor
	SymbolButton() : Button() {
		EnableResizing(true,true);
		pSymbolSurface = NULL;
		bFreeSymbolSurface = false;
	}

	/// destructor
	virtual ~SymbolButton() {
		if(bFreeSymbolSurface) {
			SDL_FreeSurface(pSymbolSurface);
			pSymbolSurface = NULL;
			bFreeSymbolSurface = false;
		}
	};

	/**
		This method is used for setting the symbol for this button.
		\param	pSymbolSurface		This is the symbol to show
		\param	bFreeSymbolSurface	Should pSymbolSurface be freed if this button is destroyed?
	*/
	virtual void SetSymbol(SDL_Surface* pSymbolSurface,bool bFreeSymbolSurface) {
		if(pSymbolSurface == NULL) {
			return;
		}

		if(this->bFreeSymbolSurface) {
			SDL_FreeSurface(this->pSymbolSurface);
			this->pSymbolSurface = NULL;
			this->bFreeSymbolSurface = false;
		}

		this->pSymbolSurface = pSymbolSurface;
		this->bFreeSymbolSurface = bFreeSymbolSurface;

		ResizeAll();
	}

	/**
		This method resized the button to width and height. This method should only
		called if the new size is a valid size for this button (See GetMinumumSize).
		\param	width	the new width of this button
		\param	height	the new height of this button
	*/
	virtual void Resize(Uint32 width, Uint32 height) {
		SDL_Surface *Unpressed;
		SDL_Surface *Pressed;

		Unpressed = GUIStyle::GetInstance().CreateButtonSurface(width, height, "", false, false);
		Pressed = GUIStyle::GetInstance().CreateButtonSurface(width, height, "", true, false);

		if(pSymbolSurface != NULL) {
			SDL_Rect dest;
			dest.x = (Unpressed->w / 2) - (pSymbolSurface->w / 2);
			dest.y = (Unpressed->h / 2) - (pSymbolSurface->h / 2);
			dest.w = pSymbolSurface->w;
			dest.h = pSymbolSurface->h;

			SDL_BlitSurface(pSymbolSurface, NULL, Unpressed, &dest);
			dest.x++;
			dest.y++;
			SDL_BlitSurface(pSymbolSurface, NULL, Pressed, &dest);
		}

		Button::SetSurfaces(Unpressed,true,Pressed,true);

		Widget::Resize(width,height);
	}

	/**
		Returns the minimum size of this button. The button should not
		resized to a size smaller than this.
		\return the minimum size of this button
	*/
	virtual Point GetMinimumSize() const {
		if(pSymbolSurface != NULL) {
			return Point((Sint32) pSymbolSurface->w + 10, (Sint32) pSymbolSurface->h + 10);
		} else {
			return Point(0,0);
		}
	}

private:
	SDL_Surface* pSymbolSurface;
	bool bFreeSymbolSurface;

};

#endif //SYMBOLBUTTON_H
