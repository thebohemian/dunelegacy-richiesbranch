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

#ifndef WIDGET_H
#define WIDGET_H

#include "CallbackTarget.h"
#include "MethodPointer.h"

#include <SDL.h>

#include <iostream>

/// A point class for representing a point.
class Point {
public:
	/// Default constructor.
	Point() : x(0) , y(0) { ; };

	/** A constructor that initializes the point with x and y
		\param x the x coordinate
		\param y the y coordinate
	*/
	Point(Sint32 x, Sint32 y) : x(x) , y(y) { ; };

	/** operator for adding two points.
		\param p the point to add
		\return the sum of both points
	*/
	inline Point operator+(const Point& p) const {
		return Point(x+p.x,y+p.y);
	};

	/** operator for subtracting two points.
		\param p the point to add
		\return this point minus p
	*/
	inline Point operator-(const Point& p) const {
		return Point(x-p.x,y-p.y);
	};

	/** operator for multiplying every both coordinates with an integer value.
		\param c the scalar to multiply with
		\return this point times c
	*/
	inline Point operator*(Sint32 c) const {
		return Point(x*c,y*c);
	};

	/** operator for dividing every both coordinates by an integer value.
		\param c the scalar to devide by
		\return this point divided by c
	*/
	inline Point operator/(Sint32 c) const {
		return Point(x/c,y/c);
	};

	/** operator for comparing two Points.
		\param op the other operator
		\return	true if both coordinates are equal, false otherwise
	*/
	inline bool operator==(const Point& op) const {
			return (x == op.x) && (y == op.y);
	}

	/// The x coordinate
	Sint32 x;

	/// The y coordinate
	Sint32 y;
};

// forward declarations
class Window;
template<class WidgetData> class Container;

/// The abstract base class for all widgets
class Widget : public CallbackTarget
{
public:
	template<class WidgetData> friend class Container;

	friend class Window;

	/** The default constructor.
	*/
	Widget() {
		parent = NULL;
		enabled = true;
		visible = true;
		active = false;
		Size.x = Size.y = 0;
		resizeX = resizeY = false;
		pAllocated = false;
	}

	/** Destructor
		Removes this widget from its parent.
	*/
	virtual ~Widget() {
		if(parent != NULL) {
			parent->RemoveChildWidget(this);
		}
	}

	/**
		Sets the parent of this widget.
		\param pParent new parent (NULL is also possible)
	*/
	inline void SetParent(Widget* pParent) { parent = pParent; };

	/**
		Returns the parent of this widget
		\return the parent of this widget or NULL if it has no parent.
	*/
	inline Widget* GetParent() const { return parent; };

	/**
		Enable or disable this widget. A disabled widget is not responding
		to clicks and key strokes and might look different.
		\param	bEnabled	true = enable widget, false = disable widget
	*/
	virtual inline void SetEnabled(bool bEnabled) {
		if((bEnabled == false) && (IsActive() == true)) {
			SetInactive();
		}
		enabled = bEnabled;
	};

	/**
		Returns whether this widget is enabled or not.
		\return	true = widget is enabled, false = widget is disabled
	*/
	inline bool IsEnabled() const { return enabled; };

	/**
		Sets this widget visible or invisible. An invisible widget does not
		responding to clicks and key presses.
		\return	bVisible	true = visible, false = invisible
	*/
	virtual inline void SetVisible(bool bVisible) { visible = bVisible; };

	/**
		Returns whether this widget is visible or not.
		\return	true = visible, false = invisible
	*/
	inline bool IsVisible() const { return visible; };

	/**
		Sets this widget active. The parent widgets are also activated and the
		currently active widget is set to inactive.
	*/
	virtual void SetActive();

	/**
		Sets this widget inactive. The next activatable widget is activated.
	*/
	virtual void SetInactive();

	/**
		Returns whether this widget is currently active.
		\return	true = currently active, false = currently inactive
	*/
	inline bool IsActive() const { return active; };

	/**
		Returns whether this widget can be set active.
		\return	true = activatable, false = not activatable
	*/
	virtual inline bool IsActivatable() const { return false; };


	/**
		Returns whether this widget is an container.
		\return	true = container, false = any other widget
	*/
	virtual inline bool IsContainer() const { return false; };

	/**
		Returns the current size of this widget.
		\return current size of this widget
	*/
	inline const Point& GetSize() const { return Size; };

	/**
		Returns the minimum size of this widget. The widget should not
		resized to a size smaller than this. If the widget is not resizeable
		in a direction this method returns the size in that direction.
		\return the minimum size of this widget
	*/
	virtual Point GetMinimumSize() const { return Point(0,0); };

	/**
		Returns whether this widget is allowed to be resized in X direction.
		You should not resize a widget, that is not resizeable.
		\return	true = resizeable in X direction, false = not resizeable in X direction
	*/
	inline bool ResizingXAllowed() const { return resizeX; };

	/**
		Returns whether this widget is allowed to be resized in Y direction.
		You should not resize a widget, that is not resizeable.
		\return	true = resizeable in Y direction, false = not resizeable in Y direction
	*/
	inline bool ResizingYAllowed() const { return resizeY; };

	/**
		This method resized the widget to width and height. This method should only be
		called if the new size is a valid size for this widget (See ResizingXAllowed,
		ResizingYAllowed, GetMinumumSize).
		\param	width	the new width of this widget
		\param	height	the new height of this widget
	*/
	virtual inline void Resize(Uint32 width, Uint32 height) {
		Size.x = width;
		Size.y = height;
	};

	/**
		This method resizes the widget and its parent (the surrounding container).
	*/
	virtual void ResizeAll() {
		if(parent != NULL) {
			parent->ResizeAll();
		} else {
			Resize(	std::max(GetMinimumSize().x,GetSize().x),
					std::max(GetMinimumSize().y,GetSize().y));
		}
	};

	/**
		Handles a mouse movement.
		\param	x               x-coordinate (relative to the left top corner of the widget)
		\param	y               y-coordinate (relative to the left top corner of the widget)
		\param  insideOverlay   true, if (x,y) is inside an overlay and this widget may be behind it, false otherwise
	*/
	virtual inline void HandleMouseMovement(Sint32 x, Sint32 y, bool insideOverlay) { ; };

	/**
		Handles a left mouse click.
		\param	x x-coordinate (relative to the left top corner of the widget)
		\param	y y-coordinate (relative to the left top corner of the widget)
		\param	pressed	true = mouse button pressed, false = mouse button released
		\return	true = click was processed by the widget, false = click was not processed by the widget
	*/
	virtual inline bool HandleMouseLeft(Sint32 x, Sint32 y, bool pressed) { return false; };

	/**
		Handles a right mouse click.
		\param	x x-coordinate (relative to the left top corner of the widget)
		\param	y y-coordinate (relative to the left top corner of the widget)
		\param	pressed	true = mouse button pressed, false = mouse button released
		\return	true = click was processed by the widget, false = click was not processed by the widget
	*/
	virtual inline bool HandleMouseRight(Sint32 x, Sint32 y, bool pressed) { return false; };

	/**
		Handles mouse wheel scrolling.
		\param	x x-coordinate (relative to the left top corner of the widget)
		\param	y y-coordinate (relative to the left top corner of the widget)
		\param	up	true = mouse wheel up, false = mouse wheel down
		\return	true = the mouse wheel scrolling was processed by the widget, false = mouse wheel scrolling was not processed by the widget
	*/
	virtual inline bool HandleMouseWheel(Sint32 x, Sint32 y, bool up) { return false; };

	/**
		Handles a key stroke.
		\param	key the key that was pressed or released.
		\return	true = key stroke was processed by the widget, false = key stroke was not processed by the widget
	*/
	virtual inline bool HandleKeyPress(SDL_KeyboardEvent& key) {
		if(IsActive() && (key.keysym.sym == SDLK_TAB)) {
			SetInactive();
		}
		return false;
	}

	/**
		Handles mouse movement in overlays.
		\param	x x-coordinate (relative to the left top corner of the widget)
		\param	y y-coordinate (relative to the left top corner of the widget)
		\return true if (x,y) is in overlay of this widget, false otherwise
	*/
	virtual inline bool HandleMouseMovementOverlay(Sint32 x, Sint32 y) { return false; };

	/**
		Handles a left mouse click in overlays.
		\param	x x-coordinate (relative to the left top corner of the widget)
		\param	y y-coordinate (relative to the left top corner of the widget)
		\param	pressed	true = mouse button pressed, false = mouse button released
		\return	true = click was processed by the widget, false = click was not processed by the widget
	*/
	virtual inline bool HandleMouseLeftOverlay(Sint32 x, Sint32 y, bool pressed) { return false; };

	/**
		Handles a right mouse click in overlays.
		\param	x x-coordinate (relative to the left top corner of the widget)
		\param	y y-coordinate (relative to the left top corner of the widget)
		\param	pressed	true = mouse button pressed, false = mouse button released
		\return	true = click was processed by the widget, false = click was not processed by the widget
	*/
	virtual inline bool HandleMouseRightOverlay(Sint32 x, Sint32 y, bool pressed) { return false; };

	/**
		Handles mouse wheel scrolling in overlays.
		\param	x x-coordinate (relative to the left top corner of the widget)
		\param	y y-coordinate (relative to the left top corner of the widget)
		\param	up	true = mouse wheel up, false = mouse wheel down
		\return	true = the mouse wheel scrolling was processed by the widget, false = mouse wheel scrolling was not processed by the widget
	*/
	virtual inline bool HandleMouseWheelOverlay(Sint32 x, Sint32 y, bool up) { return false; };

	/**
		Handles a key stroke in overlays.
		\param	key the key that was pressed or released.
		\return	true = key stroke was processed by the widget, false = key stroke was not processed by the widget
	*/
	virtual inline bool HandleKeyPressOverlay(SDL_KeyboardEvent& key) {	return false; }


	/**
		Draws this widget to screen. This method is called before DrawOverlay().
		\param	screen	Surface to draw on
		\param	Position	Position to draw the widget to
	*/
	virtual inline void Draw(SDL_Surface* screen, Point Position) { ; };

	/**
		This method draws the parts of this widget that must be drawn after all the other
		widgets are drawn (e.g. tooltips). This method is called after Draw().
		\param	screen	Surface to draw on
		\param	Position	Position to draw the widget to
	*/
	virtual inline void DrawOverlay(SDL_Surface* screen, Point Position) { ; };


	/**
		This method is called if a child widget is destroyed (see Widget::~Widget).
		\param	pChildWidget	widget to remove
	*/
	virtual void RemoveChildWidget(Widget* pChildWidget) {
		;
	}

	/**
		This method is called if the widget is removed from a container. If
		the widget was created via a named constructor (static create method) then this
		method automatically frees the memory of this object.
	*/
	virtual void Destroy() {
		if(pAllocated == true) {
			delete this;
		} else {
			if(parent != NULL) {
				parent->RemoveChildWidget(this);
			}
		}
	};

protected:
	/**
		This method is called by containers to enable a widget or disable a widget explicitly.
		It is the responsibility of the container to take care that there is only one active
		widget.
		\param	bActive	true = activate this widget, false = deactiviate this widget
	*/
	virtual void inline SetActive(bool bActive) { active = bActive; };

	/**
		This method enables or disables resizing of a widget. By default resizing in both directions is not allowed
		and this method should be called by all constructors of a concrete widget that is resizible.
		\param	bResizeX	true = resizing in x direction allowed, false = resizing in x direction not allowed
		\param	bResizeY	true = resizing in y direction allowed, false = resizing in y direction not allowed
	*/
	virtual inline void EnableResizing(bool bResizeX, bool bResizeY) {
		resizeX = bResizeX;
		resizeY = bResizeY;
	};

	/**
		This method activates or deactivates one specific widget in this widget/container. It is mainly used
		by widgets that are activated/deactivated and have to inform their parent container.
		\param	active	true = activate, false = deactivate
		\param	childWidget	the widget to activate/deactivate
	*/
	virtual void SetActiveChildWidget(bool active, Widget* childWidget) {
	}

	/// If this widget is created via a named constructor (static create method) then bAllocated is true
	bool pAllocated;

private:
	bool visible;	///< Is this widget visible?
	bool enabled;	///< Is this widget enabled?
	bool active;	///< Is this widget active?
	bool resizeX;	///< Is this widget resizable in X direction?
	bool resizeY;	///< Is this widget resizable in Y direction?
	Point Size;		///< The size of this widget
	Widget* parent;	///< The parent widget
};


#endif //WIDGET_H
