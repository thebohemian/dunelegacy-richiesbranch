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

#ifndef CHECKBOX_H
#define CHECKBOX_H

#include "Button.h"
#include "GUIStyle.h"

#include <string>

/// A class for a checkbox implemented as a toggle button
class Checkbox : public Button {
public:
	/// Default constructor
	Checkbox() : Button() {
        textcolor = -1;
	    textshadowcolor = -1;

		EnableResizing(true,false);
		SetToggleButton(true);
        pCheckedActiveSurface = NULL;
        bFreeCheckedActiveSurface = false;
	}

	/// destructor
	virtual ~Checkbox() {
        if((bFreeCheckedActiveSurface == true) && (pCheckedActiveSurface != NULL)) {
            SDL_FreeSurface(pCheckedActiveSurface);
            pCheckedActiveSurface = NULL;
            bFreeCheckedActiveSurface = false;
        }
	}

	/**
		This method sets a new text for this checkbox and resizes it
		to fit this text.
		\param	Text The new text for this checkbox
	*/
	virtual inline void SetText(std::string Text) {
		this->Text = Text;
		ResizeAll();
	}

	/**
		Get the text of this checkbox.
		\return the text of this checkbox
	*/
	inline std::string GetText() { return Text; };

    /**
		Sets the text color for this checkbox.
		\param	textcolor	    the color of the text (-1 = default color)
        \param	textshadowcolor	the color of the shadow of the text (-1 = default color)
	*/
	virtual inline void SetTextColor(int textcolor, int textshadowcolor = -1) {
		this->textcolor = textcolor;
		this->textshadowcolor = textshadowcolor;
		Resize(GetSize().x, GetSize().y);
	}

	/**
		This method sets this checkbox to checked or unchecked. It does the same as SetToggleState().
		\param bChecked	true = checked, false = unchecked
	*/
	inline void SetChecked(bool bChecked) {
		SetToggleState(bChecked);
	}

	/**
		This method returns whether this checkbox is checked. It is the same as GetToggleState().
		\return	true = checked, false = unchecked
	*/
	inline bool IsChecked() {
		return GetToggleState();
	}

	/**
		Draws this button to screen. This method is called before DrawOverlay().
		\param	screen	Surface to draw on
		\param	Position	Position to draw the button to
	*/
	virtual void Draw(SDL_Surface* screen, Point Position) {
        if(IsVisible() == false) {
            return;
        }

        SDL_Surface* surf;
        if(IsChecked()) {
            if((IsActive() || bHover) && pCheckedActiveSurface != NULL) {
                surf = pCheckedActiveSurface;
            } else {
                surf = pPressedSurface;
            }
        } else {
            if((IsActive() || bHover) && pActiveSurface != NULL) {
                surf = pActiveSurface;
            } else {
                surf = pUnpressedSurface;
            }
        }

        if(surf == NULL) {
            return;
        }

        SDL_Rect dest;
        dest.x = Position.x;
        dest.y = Position.y;
        dest.w = surf->w;
        dest.h = surf->h;

        SDL_BlitSurface(surf,NULL,screen,&dest);
    }

	/**
		This method resized the checkbox to width and height. This method should only
		called if the new size is a valid size for this checkbox (See GetMinimumSize).
		\param	width	the new width of this checkbox
		\param	height	the new height of this checkbox
	*/
	virtual void Resize(Uint32 width, Uint32 height) {
		SetSurfaces(GUIStyle::GetInstance().CreateCheckboxSurface(width, height, Text, false, false, textcolor, textshadowcolor),true,
                    GUIStyle::GetInstance().CreateCheckboxSurface(width, height, Text, true, false, textcolor, textshadowcolor),true,
                    GUIStyle::GetInstance().CreateCheckboxSurface(width, height, Text, false, true, textcolor, textshadowcolor),true);

        if((bFreeCheckedActiveSurface == true) && (pCheckedActiveSurface != NULL)) {
            SDL_FreeSurface(pCheckedActiveSurface);
            pCheckedActiveSurface = NULL;
            bFreeCheckedActiveSurface = false;
        }

        pCheckedActiveSurface = GUIStyle::GetInstance().CreateCheckboxSurface(width, height, Text, true, true);
        bFreeCheckedActiveSurface = true;

		Widget::Resize(width,height);
	}

	/**
		Returns the minimum size of this button. The button should not
		resized to a size smaller than this.
		\return the minimum size of this button
	*/
	virtual Point GetMinimumSize() const {
		return GUIStyle::GetInstance().GetMinimumCheckboxSize(Text);
	}

private:
    int textcolor;                          ///< Text color
    int textshadowcolor;                    ///< Text shadow color
	std::string Text;		                ///< Text of this checkbox
	SDL_Surface* pCheckedActiveSurface;		///< Surface that is shown when the checkbox is activated by keyboard or by mouse hover
	bool bFreeCheckedActiveSurface;			///< Should pActiveSurface be freed if this button is destroyed?
};

#endif // CHECKBOX_H
