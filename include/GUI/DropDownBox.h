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

#ifndef DROPDOWNBOX_H
#define DROPDOWNBOX_H

#include "Widget.h"
#include "PictureButton.h"
#include "ListBox.h"

#include <SDL.h>
#include <vector>
#include <string>

/// A class for a dropdown box widget
class DropDownBox : public Widget {
public:
	/// default constructor
	DropDownBox();

	/// destructor
	virtual ~DropDownBox();

	/**
		Handles a mouse movement. This method is for example needed for the tooltip.
		\param	x               x-coordinate (relative to the left top corner of the widget)
		\param	y               y-coordinate (relative to the left top corner of the widget)
		\param  insideOverlay   true, if (x,y) is inside an overlay and this widget may be behind it, false otherwise
	*/
	virtual void HandleMouseMovement(Sint32 x, Sint32 y, bool insideOverlay);

	/**
		Handles mouse movement in overlays.
		\param	x x-coordinate (relative to the left top corner of the widget)
		\param	y y-coordinate (relative to the left top corner of the widget)
		\return true if (x,y) is in overlay of this widget, false otherwise
	*/
	virtual bool HandleMouseMovementOverlay(Sint32 x, Sint32 y);

	/**
		Handles a left mouse click.
		\param	x x-coordinate (relative to the left top corner of the widget)
		\param	y y-coordinate (relative to the left top corner of the widget)
		\param	pressed	true = mouse button pressed, false = mouse button released
		\return	true = click was processed by the widget, false = click was not processed by the widget
	*/
	virtual bool HandleMouseLeft(Sint32 x, Sint32 y, bool pressed);

	/**
		Handles a left mouse click in overlays.
		\param	x x-coordinate (relative to the left top corner of the widget)
		\param	y y-coordinate (relative to the left top corner of the widget)
		\param	pressed	true = mouse button pressed, false = mouse button released
		\return	true = click was processed by the widget, false = click was not processed by the widget
	*/
	virtual bool HandleMouseLeftOverlay(Sint32 x, Sint32 y, bool pressed);

	/**
		Handles mouse wheel scrolling.
		\param	x x-coordinate (relative to the left top corner of the widget)
		\param	y y-coordinate (relative to the left top corner of the widget)
		\param	up	true = mouse wheel up, false = mouse wheel down
		\return	true = the mouse wheel scrolling was processed by the widget, false = mouse wheel scrolling was not processed by the widget
	*/
	virtual bool HandleMouseWheel(Sint32 x, Sint32 y, bool up);

	/**
		Handles mouse wheel scrolling in overlays.
		\param	x x-coordinate (relative to the left top corner of the widget)
		\param	y y-coordinate (relative to the left top corner of the widget)
		\param	up	true = mouse wheel up, false = mouse wheel down
		\return	true = the mouse wheel scrolling was processed by the widget, false = mouse wheel scrolling was not processed by the widget
	*/
	virtual bool HandleMouseWheelOverlay(Sint32 x, Sint32 y, bool up);

	/**
		Handles a key stroke. This method is neccessary for controlling an application
		without a mouse.
		\param	key the key that was pressed or released.
		\return	true = key stroke was processed by the widget, false = key stroke was not processed by the widget
	*/
	virtual bool HandleKeyPress(SDL_KeyboardEvent& key);

	/**
		Draws this scroll bar to screen. This method is called before DrawOverlay().
		\param	screen	Surface to draw on
		\param	Position	Position to draw the scroll bar to
	*/
	virtual void Draw(SDL_Surface* screen, Point Position);

	/**
		This method draws the parts of this widget that must be drawn after all the other
		widgets are drawn (e.g. tooltips). This method is called after Draw().
		\param	screen	Surface to draw on
		\param	Position	Position to draw the widget to
	*/
	virtual void DrawOverlay(SDL_Surface* screen, Point Position);

	/**
		This method resized the scroll bar to width and height. This method should only
		called if the new size is a valid size for this scroll bar (See GetMinumumSize).
		\param	width	the new width of this scroll bar
		\param	height	the new height of this scroll bar
	*/
	virtual void Resize(Uint32 width, Uint32 height);

	/**
		Returns the minimum size of this scroll bar. The scroll bar should not
		resized to a size smaller than this.
		\return the minimum size of this scroll bar
	*/
	virtual Point GetMinimumSize() const {
		return Point(listBox.GetMinimumSize().x,openListBoxButton.GetSize().y+2);
	}

	/**
		Returns whether this widget can be set active.
		\return	true = activatable, false = not activatable
	*/
	virtual inline bool IsActivatable() const { return IsEnabled(); };

	/**
		Adds a new entry to this dropdown box
		\param	text	the text to be added to the list
		\param	data	an integer value that is assigned to this entry (see GetEntryData)
	*/
	void AddEntry(std::string text, int data = 0) {
		listBox.AddEntry(text, data);
		ResizeListBox();
	}

    /**
		Insert a new entry to this list box at the specified index
		\param  index   the index this entry should be inserted before.
		\param	text	the text to be added to the list
		\param	data	an integer value that is assigned to this entry (see GetEntryData)
	*/
	void InsertEntry(int index, std::string text, int data = 0) {
        listBox.InsertEntry(index, text, data);
        ResizeListBox();
	}

	/**
		Returns the number of entries in this dropdown box
		\return	number of entries
	*/
	int GetNumEntries() {
		return listBox.GetNumEntries();
	}

	/**
		Returns the text of the entry specified by index.
		\param	index   the zero-based index of the entry
		\return	the text of the entry
	*/
	std::string GetEntry(unsigned int index) {
		return listBox.GetEntry(index);
	}

	/**
		Returns the data assigned to the entry specified by index.
		\param	index   the zero-based index of the entry
		\return	the data of the entry
	*/
	int GetEntryData(unsigned int index) {
		return listBox.GetEntryData(index);
	}

	/**
		Returns the zero-based index of the current selected entry.
		\return	the index of the selected element (-1 if none is selected)
	*/
	int GetSelectedIndex() {
		return listBox.GetSelectedIndex();
	}

	/**
		Sets the selected item and scrolls the scrollbar to that position.
		\param index	the new index (-1 == select nothing)
	*/
	void SetSelectedItem(int index) {
		listBox.SetSelectedItem(index);
	}

	/**
		Removes the entry which is specified by index
		\param	index	the zero-based index of the element to remove
	*/
	void RemoveEntry(int index) {
		listBox.RemoveEntry(index);
		if((pForeground != NULL) && (listBox.GetSelectedIndex() < 0)) {
			SDL_FreeSurface(pForeground);
            pForeground = NULL;
		}
		ResizeListBox();
	}

	/**
        	Deletes all entries in the list.
	*/
	void ClearAllEntries() {
		listBox.ClearAllEntries();
		if(pForeground != NULL) {
			SDL_FreeSurface(pForeground);
            pForeground = NULL;
		}
		ResizeListBox();
	}

	/**
		Sets the method that should be called when the selection in this dropdown box changes.
		\param	pMethod	A pointer to this method
	*/
	inline void SetOnSelectionChangeMethod(MethodPointer pMethod) {
		pOnSelectionChange = pMethod;
	}

	/**
		Sets the method that should be called when the drop down box is clicked. If an empty
		Method pointer is set, the default behaviour of opening the drop down list is activated.
		Otherwise the drop down list is not opened and the specified method is called. Furthermore
		a on mouse hover effect is only active if a OnClick-Method is set.
		\param	pMethod	A pointer to this method
	*/
	inline void SetOnClickMethod(MethodPointer pMethod) {
		pOnClick = pMethod;
	}

	/**
		Sets this widget active. The parent widgets are also activated and the
		currently active widget is set to inactive.
	*/
	virtual void SetActive() {
	    openListBoxButton.SetActive();
		Widget::SetActive();
	}

    /**
		Sets the color for this drop down box.
		\param	color   the color (-1 = default color)
	*/
	virtual inline void SetColor(int color) {
		this->color = color;
		UpdateButtonSurface();
		UpdateForeground();
		listBox.SetColor(color);
	}

    /**
		This static method creates a dynamic dropdown box object.
		The idea behind this method is to simply create a new dropdown box on the fly and
		add it to a container. If the container gets destroyed also this dropdown box will be freed.
		\return	The new created dropdown box (will be automatically destroyed when it's parent widget is destroyed)
	*/
	static DropDownBox* Create() {
		DropDownBox* dropDownBox = new DropDownBox();
		dropDownBox->pAllocated = true;
		return dropDownBox;
	}

protected:
	/**
		This method is called by containers to enable a widget or disable a widget explicitly.
		It is the responsibility of the container to take care that there is only one active
		widget.
		\param	bActive	true = activate this widget, false = deactiviate this widget
	*/
	virtual void SetActive(bool bActive);


private:

	void OnSelectionChange();

	void ResizeListBox();

	void UpdateButtonSurface();

	void UpdateForeground();

	void OnOpenListBoxButton() {
		bShowListBox = !bShowListBox;
	}

	bool bShowListBox;
	bool bAutocloseListBoxOnSelectionChange;	///< This is a small hack to allow the list box to be open while selection with up/down keys

	SDL_Surface* pBackground;
	SDL_Surface* pForeground;
	SDL_Surface* pActiveForeground;             ///< Ís shown while the mouse cursor is over this drop down box

	PictureButton openListBoxButton;
	ListBox listBox;

	MethodPointer pOnSelectionChange;			///< this method is called when the selection changes
	MethodPointer pOnClick;				        ///< method that is called when this drop down box is clicked

    int color;				                    ///< the color
    bool bHover;						        ///< true = currenlty mouse hover, false = currently no mouse hover
};

#endif //DROPDOWNBOX_H
