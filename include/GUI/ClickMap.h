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

#ifndef CLICKMAP_H
#define CLICKMAP_H

#include "Widget.h"

/// This widget reports the coordinate where the user clicked on
class ClickMap : public Widget {
public:
	/// default constructor
	ClickMap() : Widget() {
		EnableResizing(true,true);
	}

	/// destructor
	virtual ~ClickMap() {

	}

	/**
		Sets the method that should be called when this click map is clicked on.
		\param	pMethod	A pointer to this method
	*/
	inline void SetOnClickMethod(MethodPointer pMethod) {
		pOnClick = pMethod;
	}

	/**
		Handles a left mouse click.
		\param	x x-coordinate (relative to the left top corner of the widget)
		\param	y y-coordinate (relative to the left top corner of the widget)
		\param	pressed	true = mouse button pressed, false = mouse button released
		\return	true = click was processed by the widget, false = click was not processed by the widget
	*/
	virtual bool HandleMouseLeft(Sint32 x, Sint32 y, bool pressed) {
		if((x < 0) || (x >= GetSize().x) || (y < 0) || (y >= GetSize().y)) {
			return false;
		}

		if((IsEnabled() == false) || (IsVisible() == false)) {
			return true;
		}

		if(pressed == true) {
			LastClickPosition = Point(x,y);
			pOnClick();
		}

		return true;
	}

	/**
		Return the position of the last click. This method is normally called from the method
		that is specified by SetOnClickMethod.
		\return	The point of the last click (relative to the left top corner of the widget)
	*/
	Point GetClickPosition() {
		return LastClickPosition;
	}

private:
	MethodPointer pOnClick;				///< method that is called when this click map is clicked
	Point LastClickPosition;
};


#endif //CLICKMAP_H
