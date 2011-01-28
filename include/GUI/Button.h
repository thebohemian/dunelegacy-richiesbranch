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

#ifndef BUTTON_H
#define BUTTON_H

#include "Widget.h"
#include "GUIStyle.h"
#include <SDL.h>

#include <string>

/// A abstract base class for all buttons
class Button : public Widget {
public:
	/// Default contructor
	Button();

	/// desctructor
	virtual ~Button();

	/**
		Returns that this button can be set active.
		\return	true(= activatable)
	*/
	virtual inline bool IsActivatable() const { return IsEnabled(); };

	/**
		Enable or disable this button. A disabled button is not responding
		to clicks and key strokes and might look different.
		\param	bEnabled	true = enable button, false = disable button
	*/
	virtual inline void SetEnabled(bool bEnabled) {
		Widget::SetEnabled(bEnabled);
		if(bEnabled == false) {
			bPressed = false;
		}
	};


	/**
		Sets a tooltip text. This text is shown when the mouse remains a short time over this button.
		\param	text	The text for this tooltip
	*/
	inline void SetTooltipText(std::string text) {
		TooltipText = text;

		if(TooltipSurface != NULL) {
			SDL_FreeSurface(TooltipSurface);
			TooltipSurface = NULL;
		}

		if(TooltipText != "") {
			TooltipSurface = GUIStyle::GetInstance().CreateToolTip(TooltipText);
		}
	}

	/**
		Returns the current tooltip text.
		\return	the current tooltip text
	*/
	inline std::string GetTooltipText() {
		return TooltipText;
	}

	/**
		Sets the method that should be called when this button is clicked on.
		\param	pMethod	A pointer to this method
	*/
	inline void SetOnClickMethod(MethodPointer pMethod) {
		pOnClick = pMethod;
	}

	/**
		Sets whether this button is a toggle button.
		\param bToggleButton	true = toggle button, false = normal button
	*/
	inline void SetToggleButton(bool bToggleButton) {
		this->bToggleButton = bToggleButton;
	}

	/**
		Returns whether this button is a toggle button
		\return true = toggle button, false = normal button
	*/
	inline bool IsToggleButton() {
		return bToggleButton;
	}

	/**
		This method sets the current toggle state. If this button is no
		toggle button this method has no effect.
		\param bToggleState	true = toggled, false = untoggled
	*/
	inline void SetToggleState(bool bToggleState) {
		if(IsToggleButton()) {
			this->bToggleState = bToggleState;
		}
	}

	/**
		This method returns whether this button is currently toggled or not.
		\return	true = toggled, false = untoggled
	*/
	inline bool GetToggleState() {
		return bToggleState;
	}

	/**
		Handles a mouse movement. This method is for example needed for the tooltip.
		\param	x               x-coordinate (relative to the left top corner of the widget)
		\param	y               y-coordinate (relative to the left top corner of the widget)
		\param  insideOverlay   true, if (x,y) is inside an overlay and this widget may be behind it, false otherwise
	*/
	virtual void HandleMouseMovement(Sint32 x, Sint32 y, bool insideOverlay);

	/**
		Handles a left mouse click.
		\param	x x-coordinate (relative to the left top corner of the widget)
		\param	y y-coordinate (relative to the left top corner of the widget)
		\param	pressed	true = mouse button pressed, false = mouse button released
		\return	true = click was processed by the widget, false = click was not processed by the widget
	*/
	virtual bool HandleMouseLeft(Sint32 x, Sint32 y, bool pressed);

	/**
		Handles a key stroke. This method is neccessary for controlling an application
		without a mouse.
		\param	key the key that was pressed or released.
		\return	true = key stroke was processed by the widget, false = key stroke was not processed by the widget
	*/
	virtual bool HandleKeyPress(SDL_KeyboardEvent& key);

	/**
		Draws this button to screen. This method is called before DrawOverlay().
		\param	screen	Surface to draw on
		\param	Position	Position to draw the button to
	*/
	virtual void Draw(SDL_Surface* screen, Point Position);

	/**
		This method draws the tooltip for this button. This method is called after Draw().
		\param	screen	Surface to draw on
		\param	Position	Position to draw the widget to
	*/
	virtual void DrawOverlay(SDL_Surface* screen, Point Position);

protected:
	/**
		This method is used for setting the different surfaces for this button.
		\param	pUnpressedSurface		This surface is normally shown
		\param	bFreeUnpressedSurface	Should pUnpressedSurface be freed if this button is destroyed?
		\param	pPressedSurface			This surface is shown when the button is pressed
		\param	bFreePressedSurface		Should pPressedSurface be freed if this button is destroyed?
		\param	pActiveSurface			This surface is shown when the button is activated by keyboard or by mouse hover
		\param	bFreeActiveSurface		Should pActiveSurface be freed if this button is destroyed?
	*/
	virtual void SetSurfaces(	SDL_Surface* pUnpressedSurface,bool bFreeUnpressedSurface,
								SDL_Surface* pPressedSurface,bool bFreePressedSurface,
								SDL_Surface* pActiveSurface = NULL,bool bFreeActiveSurface = false);

	SDL_Surface* pUnpressedSurface;		///< Surface that is normally shown
	SDL_Surface* pPressedSurface;		///< Surface that is shown when the button is pressed
	SDL_Surface* pActiveSurface;		///< Surface that is shown when the button is activated by keyboard or by mouse hover
	bool bFreeUnpressedSurface;			///< Should pUnpressedSurface be freed if this button is destroyed?
	bool bFreePressedSurface;			///< Should pPressedSurface be freed if this button is destroyed?
	bool bFreeActiveSurface;			///< Should pActiveSurface be freed if this button is destroyed?

	/**
		This method frees all surfaces that should be freed by this button
	*/
	void FreeSurfaces();

	std::string TooltipText;			///< the tooltip text
	SDL_Surface* TooltipSurface;		///< the tooltip surface
	Uint32 Tooltip_LastMouseMotion;		///< the last time the mouse was moved

	MethodPointer pOnClick;				///< method that is called when this button is clicked
	bool bPressed;						///< true = currently pressed, false = currently unpressed
	bool bHover;						///< true = currently mouse hover, false = currently no mouse hover
	bool bToggleButton;					///< true = toggle button, false = normal button
	bool bToggleState;					///< true = currently toggled, false = currently not toggled
};


#endif //BUTTON_H
