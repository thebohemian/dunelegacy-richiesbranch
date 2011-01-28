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

#ifndef TEXTBUTTON_H
#define TEXTBUTTON_H

#include "Button.h"
#include "GUIStyle.h"

#include <string>

#include <iostream>

/// A class for a text button
class TextButton : public Button {
public:
	/// Default contructor
	TextButton() : Button() {
	    textcolor = -1;
	    textshadowcolor = -1;

		EnableResizing(true,true);
	}

	/// destructor
	virtual ~TextButton() {
	}

	/**
		This method sets a new text for this button and resizes this button
		to fit this text.
		\param	Text The new text for this button
	*/
	virtual inline void SetText(std::string Text) {
		this->Text = Text;
		ResizeAll();
	}

	/**
		Get the text of this button.
		\return the text of this button
	*/
	inline std::string GetText() { return Text; };

    /**
		Sets the text color for this button.
		\param	textcolor	    the color of the text (-1 = default color)
        \param	textshadowcolor	the color of the shadow of the text (-1 = default color)
	*/
	virtual inline void SetTextColor(int textcolor, int textshadowcolor = -1) {
		this->textcolor = textcolor;
		this->textshadowcolor = textshadowcolor;
		Resize(GetSize().x, GetSize().y);
	}

	/**
		This method resized the button to width and height. This method should only
		called if the new size is a valid size for this button (See GetMinumumSize).
		\param	width	the new width of this button
		\param	height	the new height of this button
	*/
	virtual void Resize(Uint32 width, Uint32 height) {
		SetSurfaces(GUIStyle::GetInstance().CreateButtonSurface(width, height, Text, false, false, textcolor, textshadowcolor),true,
					GUIStyle::GetInstance().CreateButtonSurface(width, height, Text, true, true, textcolor, textshadowcolor),true,
					GUIStyle::GetInstance().CreateButtonSurface(width, height, Text, false, true, textcolor, textshadowcolor),true);
		Widget::Resize(width,height);
	}

	/**
		Returns the minimum size of this button. The button should not
		resized to a size smaller than this.
		\return the minimum size of this button
	*/
	virtual Point GetMinimumSize() const {
		return GUIStyle::GetInstance().GetMinimumButtonSize(Text);
	}

private:
    int textcolor;
    int textshadowcolor;

	std::string Text;		///< Text of this button
};

#endif // TEXTBUTTON_H
