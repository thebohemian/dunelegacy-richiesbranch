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

#ifndef CONTAINER_H
#define CONTAINER_H

#include "Widget.h"
#include <misc/RobustList.h>

/// The abstract base class for container widgets
/**
	WidgetData is used for storing information for every widget added to the container. It must contain a pointer pWidget which stores the contained widget and can contain any
	additional data needed by the derived container class for managing the widget size and position (like weight factors, margins, etc)
*/
template<class WidgetData> class Container : public Widget {
protected:
	typedef RobustList<WidgetData> WidgetList;

public:

	/// default constructor
	Container() : Widget() {
		pActiveChildWidget = NULL;
		EnableResizing(true,true);
	}

	/// destructor
	virtual ~Container() {
		while(ContainedWidgets.begin() != ContainedWidgets.end()) {
			Widget* curWidget = ContainedWidgets.front().pWidget;
			curWidget->Destroy();
		}
	}

	/**
		This method removes a widget from this container. Everything
		will be resized afterwards.
		\param pChildWidget	Widget to remove
	*/
	virtual void RemoveChildWidget(Widget* pChildWidget) {
		for(typename WidgetList::iterator iter = ContainedWidgets.begin(); iter != ContainedWidgets.end(); ++iter) {
			if(iter->pWidget == pChildWidget) {
				ContainedWidgets.erase(iter);
				break;
			}
		}
		pChildWidget->SetParent(NULL);
		ResizeAll();
	}



	/**
		Handles a mouse movement.
		\param	x               x-coordinate (relative to the left top corner of the container)
		\param	y               y-coordinate (relative to the left top corner of the container)
		\param  insideOverlay   true, if (x,y) is inside an overlay and this container may be behind it, false otherwise
	*/
	virtual void HandleMouseMovement(Sint32 x, Sint32 y, bool insideOverlay) {

		typename WidgetList::const_iterator iter;
		for(iter = ContainedWidgets.begin(); iter != ContainedWidgets.end(); ++iter) {
			Widget* curWidget = iter->pWidget;
			Point pos = GetPosition(*iter);
			curWidget->HandleMouseMovement(x - pos.x, y - pos.y, insideOverlay);
		}
	}

	/**
		Handles a left mouse click.
		\param	x x-coordinate (relative to the left top corner of the container)
		\param	y y-coordinate (relative to the left top corner of the container)
		\param	pressed	true = mouse button pressed, false = mouse button released
		\return	true = click was processed by the container, false = click was not processed by the container
	*/
	virtual bool HandleMouseLeft(Sint32 x, Sint32 y, bool pressed) {
		if((IsEnabled() == false) || (IsVisible() == false)) {
			return false;
		}

		bool WidgetFound = false;
		typename WidgetList::const_iterator iter;
		for(iter = ContainedWidgets.begin(); iter != ContainedWidgets.end(); ++iter) {
			Widget* curWidget = iter->pWidget;
			Point pos = GetPosition(*iter);
			WidgetFound |= curWidget->HandleMouseLeft(x - pos.x, y - pos.y, pressed);
		}
		return WidgetFound;
	}

	/**
		Handles a right mouse click.
		\param	x x-coordinate (relative to the left top corner of the container)
		\param	y y-coordinate (relative to the left top corner of the container)
		\param	pressed	true = mouse button pressed, false = mouse button released
		\return	true = click was processed by the container, false = click was not processed by the container
	*/
	virtual bool HandleMouseRight(Sint32 x, Sint32 y, bool pressed) {
		if((IsEnabled() == false) || (IsVisible() == false)) {
			return false;
		}

		bool WidgetFound = false;
		typename WidgetList::const_iterator iter;
		for(iter = ContainedWidgets.begin(); iter != ContainedWidgets.end(); ++iter) {
			Widget* curWidget = iter->pWidget;
			Point pos = GetPosition(*iter);
			WidgetFound |= curWidget->HandleMouseRight(x - pos.x, y - pos.y, pressed);
		}
		return WidgetFound;
	}

	/**
		Handles mouse wheel scrolling.
		\param	x x-coordinate (relative to the left top corner of the widget)
		\param	y y-coordinate (relative to the left top corner of the widget)
		\param	up	true = mouse wheel up, false = mouse wheel down
		\return	true = the mouse wheel scrolling was processed by the widget, false = mouse wheel scrolling was not processed by the widget
	*/
	virtual inline bool HandleMouseWheel(Sint32 x, Sint32 y, bool up)  {
		if((IsEnabled() == false) || (IsVisible() == false)) {
			return false;
		}

		bool bProcessed = false;
		typename WidgetList::const_iterator iter;
		for(iter = ContainedWidgets.begin(); iter != ContainedWidgets.end(); ++iter) {
			Widget* curWidget = iter->pWidget;
			Point pos = GetPosition(*iter);

			int childX = x - pos.x;
			int childY = y - pos.y;

			if((childX >= 0) && (childX < curWidget->GetSize().x)
				&& (childY >= 0) && (childY < curWidget->GetSize().y)) {
				bProcessed = curWidget->HandleMouseWheel(childX, childY, up);
				break;
			}
		}

		if((bProcessed == false) && (pActiveChildWidget != NULL)) {
			Point pos = GetPosition(*GetWidgetDataFromWidget(pActiveChildWidget));
			return pActiveChildWidget->HandleMouseWheel(x - pos.x, y - pos.y, up);
		}

		return bProcessed;
	}

	/**
		Handles a key stroke.
		\param	key the key that was pressed or released.
		\return	true = key stroke was processed by the container, false = key stroke was not processed by the container
	*/
	virtual bool HandleKeyPress(SDL_KeyboardEvent& key) {
		if((IsEnabled() == false) || (IsVisible() == false) || (IsActive() == false)) {
			return false;
		}

		if(pActiveChildWidget != NULL) {
			return pActiveChildWidget->HandleKeyPress(key);
		} else {
			if(key.keysym.sym == SDLK_TAB) {
				ActivateFirstActivatableWidget();
				return true;
			} else {
                return false;
			}
		}
	}

	/**
		Handles mouse movement in overlays.
		\param	x x-coordinate (relative to the left top corner of the container)
		\param	y y-coordinate (relative to the left top corner of the container)
		\return true if (x,y) is in overlay of this container, false otherwise
	*/
	virtual inline bool HandleMouseMovementOverlay(Sint32 x, Sint32 y) {
	    bool insideOverlay = false;

		typename WidgetList::const_iterator iter;
		for(iter = ContainedWidgets.begin(); iter != ContainedWidgets.end(); ++iter) {
			Widget* curWidget = iter->pWidget;
			Point pos = GetPosition(*iter);
			insideOverlay |= curWidget->HandleMouseMovementOverlay(x - pos.x, y - pos.y);
		}

		return insideOverlay;
    };

	/**
		Handles a left mouse click in overlays.
		\param	x x-coordinate (relative to the left top corner of the container)
		\param	y y-coordinate (relative to the left top corner of the container)
		\param	pressed	true = mouse button pressed, false = mouse button released
		\return	true = click was processed by the container, false = click was not processed by the container
	*/
	virtual bool HandleMouseLeftOverlay(Sint32 x, Sint32 y, bool pressed) {
		if((IsEnabled() == false) || (IsVisible() == false)) {
			return false;
		}

		bool WidgetFound = false;
		typename WidgetList::const_iterator iter;
		for(iter = ContainedWidgets.begin(); iter != ContainedWidgets.end(); ++iter) {
			Widget* curWidget = iter->pWidget;
			Point pos = GetPosition(*iter);
			WidgetFound |= curWidget->HandleMouseLeftOverlay(x - pos.x, y - pos.y, pressed);
		}
		return WidgetFound;
	}

	/**
		Handles a right mouse click in overlays.
		\param	x x-coordinate (relative to the left top corner of the container)
		\param	y y-coordinate (relative to the left top corner of the container)
		\param	pressed	true = mouse button pressed, false = mouse button released
		\return	true = click was processed by the container, false = click was not processed by the container
	*/
	virtual bool HandleMouseRightOverlay(Sint32 x, Sint32 y, bool pressed) {
		if((IsEnabled() == false) || (IsVisible() == false)) {
			return false;
		}

		bool WidgetFound = false;
		typename WidgetList::const_iterator iter;
		for(iter = ContainedWidgets.begin(); iter != ContainedWidgets.end(); ++iter) {
			Widget* curWidget = iter->pWidget;
			Point pos = GetPosition(*iter);
			WidgetFound |= curWidget->HandleMouseRightOverlay(x - pos.x, y - pos.y, pressed);
		}
		return WidgetFound;
	}

	/**
		Handles mouse wheel scrolling in overlays.
		\param	x x-coordinate (relative to the left top corner of the widget)
		\param	y y-coordinate (relative to the left top corner of the widget)
		\param	up	true = mouse wheel up, false = mouse wheel down
		\return	true = the mouse wheel scrolling was processed by the widget, false = mouse wheel scrolling was not processed by the widget
	*/
	virtual inline bool HandleMouseWheelOverlay(Sint32 x, Sint32 y, bool up)  {
		if((IsEnabled() == false) || (IsVisible() == false)) {
			return false;
		}

		bool bProcessed = false;
		typename WidgetList::const_iterator iter;
		for(iter = ContainedWidgets.begin(); iter != ContainedWidgets.end(); ++iter) {
			Widget* curWidget = iter->pWidget;
			Point pos = GetPosition(*iter);

			int childX = x - pos.x;
			int childY = y - pos.y;

            bProcessed = curWidget->HandleMouseWheelOverlay(childX, childY, up);
            if(bProcessed) {
                break;
            }
		}

		if((bProcessed == false) && (pActiveChildWidget != NULL)) {
			Point pos = GetPosition(*GetWidgetDataFromWidget(pActiveChildWidget));
			return pActiveChildWidget->HandleMouseWheelOverlay(x - pos.x, y - pos.y, up);
		}

		return bProcessed;
	}

	/**
		Handles a key stroke in overlays.
		\param	key the key that was pressed or released.
		\return	true = key stroke was processed by the container, false = key stroke was not processed by the container
	*/
	virtual bool HandleKeyPressOverlay(SDL_KeyboardEvent& key) {
		if((IsEnabled() == false) || (IsVisible() == false) || (IsActive() == false)) {
			return false;
		}

		if(pActiveChildWidget != NULL) {
			return pActiveChildWidget->HandleKeyPressOverlay(key);
		}
		return false;
	}

	/**
		Draws this container and it's children to screen. This method is called before DrawOverlay().
		\param	screen	Surface to draw on
		\param	Position	Position to draw the container to
	*/
	virtual void Draw(SDL_Surface* screen, Point Position) {
		if(IsVisible() == false) {
			return;
		}

		typename WidgetList::const_iterator iter;
		for(iter = ContainedWidgets.begin(); iter != ContainedWidgets.end(); ++iter) {
			Widget* curWidget = iter->pWidget;
			Point pos = GetPosition(*iter);
			curWidget->Draw(screen,Position+pos);
		}
	}

	/**
		This method draws the parts of this container that must be drawn after all the other
		widgets are drawn (e.g. tooltips). This method is called after Draw().
		\param	screen	Surface to draw on
		\param	Position	Position to draw the container to
	*/
	virtual void DrawOverlay(SDL_Surface* screen, Point Position) {
		if(IsVisible() == false) {
			return;
		}

		typename WidgetList::const_iterator iter;
		for(iter = ContainedWidgets.begin(); iter != ContainedWidgets.end(); ++iter) {
			Widget* curWidget = iter->pWidget;
			Point pos = GetPosition(*iter);
			curWidget->DrawOverlay(screen,Position+pos);
		}
	}

	/**
		This method resized the container to width and height. This method should only be
		called if the new size is a valid size for this container (See GetMinumumSize).
		\param	width	the new width of this container
		\param	height	the new height of this container
	*/
	virtual void Resize(Uint32 width, Uint32 height) {
		Widget::Resize(width,height);
	}

	/**
		Sets this container and it's children active. The parent widgets are also activated and the
		currently widget is set to inactive.
	*/
	virtual void SetActive() {
		if(pActiveChildWidget == NULL) {
            ActivateFirstActivatableWidget();
		}

		Widget::SetActive();
	}

	/**
		Sets this container and it's children inactive. The next activatable widget is activated.
	*/
	virtual void SetInactive() {
		if(pActiveChildWidget != NULL) {
			pActiveChildWidget->SetActive(false);
			pActiveChildWidget = NULL;
		}

		Widget::SetInactive();
	}

	/**
		Returns whether one of this container's children can be set active.
		\return	true = activatable, false = not activatable
	*/
	virtual inline bool IsActivatable() const {
	    if(IsEnabled() == false) {
            return false;
	    }

		typename WidgetList::const_iterator iter = ContainedWidgets.begin();
		while(iter != ContainedWidgets.end()) {
			if(iter->pWidget->IsActivatable() == true) {
				return true;
			}
			++iter;
		}
		return false;
	}

    /**
		Returns whether this widget is an container.
		\return	true = container, false = any other widget
	*/
	virtual inline bool IsContainer() const { return true; }

protected:
	/**
		This method is called by other containers to enable this container or disable this container explicitly.
		It is the responsibility of the calling container to take care that there is only one active
		widget.
		\param	bActive	true = activate this widget, false = deactiviate this widget
	*/
	virtual void SetActive(bool bActive) {
		if(pActiveChildWidget != NULL) {
			pActiveChildWidget->SetActive(bActive);
			if(bActive == false) {
                pActiveChildWidget = NULL;
			}
		}
		Widget::SetActive(bActive);
	}

	/**
		This method activates or deactivates one specific widget in this container. It is mainly used
		by widgets that are activated/deactivated and have to inform their parent container.
		\param	active	true = activate, false = deactivate
		\param	childWidget	the widget to activate/deactivate
	*/
	virtual void SetActiveChildWidget(bool active, Widget* childWidget) {
		if(childWidget == NULL) {
			return;
		}

		if(active == true) {
			// deactivate current active widget
			if((pActiveChildWidget != NULL) && (pActiveChildWidget != childWidget)) {
				pActiveChildWidget->SetActive(false);
				pActiveChildWidget = childWidget;
			} else {
				pActiveChildWidget = childWidget;

				// activate this container and upper containers
				Widget::SetActive();
			}
		} else {
			if(childWidget != pActiveChildWidget) {
				return;
			}

			// deactivate current active widget
			if(pActiveChildWidget != NULL) {
				pActiveChildWidget->SetActive(false);
			}

			// find childWidget in the widget list
			typename WidgetList::const_iterator iter = ContainedWidgets.begin();
			while((iter != ContainedWidgets.end()) && (iter->pWidget != childWidget)) {
				++iter;
			}

			++iter;

			while(iter != ContainedWidgets.end()) {
				if(iter->pWidget->IsActivatable() == true) {
					// activate next widget
					pActiveChildWidget = iter->pWidget;
					pActiveChildWidget->SetActive();
					return;
				}
				++iter;
			}

			// we are at the end of the list
			if((GetParent() != NULL) && GetParent()->IsContainer()) {
				pActiveChildWidget = NULL;
				SetInactive();
			} else {
				ActivateFirstActivatableWidget();
				SetActive();
			}
		}
	}

	/**
		This method activates the first activatable widget in this container.
	*/
	void ActivateFirstActivatableWidget() {
		typename WidgetList::const_iterator iter = ContainedWidgets.begin();
		while(iter != ContainedWidgets.end()) {
			if(iter->pWidget->IsActivatable() == true) {
				// activate next widget
				pActiveChildWidget = iter->pWidget;
				pActiveChildWidget->SetActive();
				return;
			}
			++iter;
		}
		pActiveChildWidget = NULL;
	}

	/**
		This method must be overwritten by all container classes. It should return
		the position of the specified widget.
		\param widgetData	the widget data to get the position from.
		\return The position of the left upper corner
	*/
	virtual Point GetPosition(const WidgetData& widgetData) const = 0;

	/**
		This method returns from a Widget* the corresponding WidgetData.
		\param	pWidget	the widget to look for
		\return	a pointer to the WidgetData, NULL if not found
	*/
	WidgetData* GetWidgetDataFromWidget(Widget* pWidget) {
		typename WidgetList::iterator iter = ContainedWidgets.begin();
		while(iter != ContainedWidgets.end()) {
			if(iter->pWidget == pWidget) {
				return &(*iter);
			}
			++iter;
		}

		return NULL;
	}


	WidgetList ContainedWidgets;				///< List of widgets
	Widget* pActiveChildWidget;				///< currently active widget
};

#endif // CONTAINER_H
