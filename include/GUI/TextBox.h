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

#ifndef TEXTBOX_H
#define TEXTBOX_H

#include "Widget.h"
#include <string>
#include <SDL.h>

#include <stdio.h>

/// A class for a text box
class TextBox : public Widget {
public:
	/// default constructor
	TextBox() : Widget() {
        textcolor = -1;
	    textshadowcolor = -1;
		pSurfaceWithoutCarret = NULL;
		pSurfaceWithCarret = NULL;
		LastCarretTime = SDL_GetTicks();
		EnableResizing(true,false);
		Resize(GetMinimumSize().x,GetMinimumSize().y);
	}

	/// destructor
	virtual ~TextBox() {
		if(pSurfaceWithoutCarret != NULL) {
			SDL_FreeSurface(pSurfaceWithoutCarret);
			pSurfaceWithoutCarret = NULL;
		}

		if(pSurfaceWithCarret != NULL) {
			SDL_FreeSurface(pSurfaceWithCarret);
			pSurfaceWithCarret = NULL;
		}
	}

	/**
		Returns true.
		\return	true = activatable, false = not activatable
	*/
	virtual inline bool IsActivatable() const { return IsEnabled(); };

	/**
		This method sets a new text for this text box.
		\param	Text The new text for this text box
	*/
	virtual inline void SetText(std::string Text) {
	    bool bChanged = (Text != this->Text);
		this->Text = Text;
		UpdateSurfaces();
		if(bChanged) {
            pOnTextChange();
		}
	}

	/**
		Get the text of this text box.
		\return the text of this text box
	*/
	inline std::string GetText() { return Text; };

    /**
		Sets the text color for this text box.
		\param	textcolor	    the color of the text (-1 = default color)
        \param	textshadowcolor	the color of the shadow of the text (-1 = default color)
	*/
	virtual inline void SetTextColor(int textcolor, int textshadowcolor = -1) {
		this->textcolor = textcolor;
		this->textshadowcolor = textshadowcolor;
		Resize(GetSize().x, GetSize().y);
	}


	/**
		Sets the method that should be called when the text of this text box changes.
		\param	pMethod	A pointer to this method
	*/
	inline void SetOnTextChangeMethod(MethodPointer pMethod) {
		pOnTextChange = pMethod;
	}

	/**
		Returns the minimum size of this text box. The text box should not
		resized to a size smaller than this. If the text box is not resizeable
		in a direction this method returns the size in that direction.
		\return the minimum size of this text box
	*/
	virtual Point GetMinimumSize() const {
		return GUIStyle::GetInstance().GetMinimumTextBoxSize();
	}

	/**
		This method resized the text box to width and height. This method should only be
		called if the new size is a valid size for this text box (See ResizingXAllowed,
		ResizingYAllowed, GetMinumumSize).
		\param	width	the new width of this text box
		\param	height	the new height of this text box
	*/
	virtual void Resize(Uint32 width, Uint32 height) {
		Widget::Resize(width,height);
		UpdateSurfaces();
	}

	/**
		This method updates all surfaces for this text box. This method will be called
		if this text box is resized or the text changes.
	*/
	virtual void UpdateSurfaces() {
		if(pSurfaceWithoutCarret != NULL) {
			SDL_FreeSurface(pSurfaceWithoutCarret);
			pSurfaceWithoutCarret = NULL;
		}

		if(pSurfaceWithCarret != NULL) {
			SDL_FreeSurface(pSurfaceWithCarret);
			pSurfaceWithCarret = NULL;
		}

		pSurfaceWithoutCarret = GUIStyle::GetInstance().CreateTextBoxSurface(GetSize().x, GetSize().y, Text, false, textcolor, textshadowcolor);
		pSurfaceWithCarret = GUIStyle::GetInstance().CreateTextBoxSurface(GetSize().x, GetSize().y, Text, true, textcolor, textshadowcolor);
	}

	/**
		Draws this text box to screen.
		\param	screen	Surface to draw on
		\param	Position	Position to draw the text box to
	*/
	virtual void Draw(SDL_Surface* screen, Point Position) {
		if((IsVisible() == false) || (pSurfaceWithoutCarret == NULL) || (pSurfaceWithCarret == NULL)) {
			return;
		}

		SDL_Rect dest;
		dest.x = Position.x;
		dest.y = Position.y;
		dest.w = pSurfaceWithoutCarret->w;
		dest.h = pSurfaceWithoutCarret->h;

		if(IsActive()) {
			if((SDL_GetTicks() - LastCarretTime) < 500) {
				SDL_BlitSurface(pSurfaceWithCarret,NULL,screen,&dest);
			} else {
				SDL_BlitSurface(pSurfaceWithoutCarret,NULL,screen,&dest);
			}

			if(SDL_GetTicks() - LastCarretTime >= 1000) {
				LastCarretTime = SDL_GetTicks();
			}
		} else {
			SDL_BlitSurface(pSurfaceWithoutCarret,NULL,screen,&dest);
		}
	}

	/**
		Handles a left mouse click.
		\param	x x-coordinate (relative to the left top corner of the text box)
		\param	y y-coordinate (relative to the left top corner of the text box)
		\param	pressed	true = mouse button pressed, false = mouse button released
		\return	true = click was processed by the widget, false = click was not processed by the text box
	*/
	virtual bool HandleMouseLeft(Sint32 x, Sint32 y, bool pressed) {
		if((x < 0) || (x >= GetSize().x) || (y < 0) || (y >= GetSize().y)) {
			return false;
		}

		if((IsEnabled() == false) || (IsVisible() == false)) {
			return true;
		}

		if(pressed == true) {
			SetActive();
			LastCarretTime = SDL_GetTicks();
		}
		return true;
	}

	/**
		Handles a key stroke.
		\param	key the key that was pressed or released.
		\return	true = key stroke was processed by the text box, false = key stroke was not processed by the text box
	*/
	virtual bool HandleKeyPress(SDL_KeyboardEvent& key) {
		if((IsVisible() == false) || (IsEnabled() == false) || (IsActive() == false)) {
			return true;
		}

		if(key.keysym.sym == SDLK_TAB) {
			SetInactive();
			return true;
		}

		if(key.keysym.sym == SDLK_BACKSPACE) {
			if(Text.size() != 0) {
				Text.erase(Text.size() - 1);
				pOnTextChange();
			}
		} else if((key.keysym.unicode <= 0xFF) && (key.keysym.unicode != 0)) {
			Text += (char) key.keysym.unicode;
			pOnTextChange();
		}

		UpdateSurfaces();

		return true;
	}

private:
    int textcolor;                          ///< Text color
    int textshadowcolor;                    ///< Text shadow color
	std::string Text;			            ///< text in this text box

	Uint32 LastCarretTime;		            ///< Last time the carret changes from off to on or vise versa

	MethodPointer pOnTextChange;	        ///< method that is called when the text of this text box changes

	SDL_Surface* pSurfaceWithoutCarret;		///< Surface with carret off
	SDL_Surface* pSurfaceWithCarret;		///< Surface with carret on
};

#endif // TEXTBOX_H
