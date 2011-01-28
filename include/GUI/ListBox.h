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

#ifndef LISTBOX_H
#define LISTBOX_H

#include "Widget.h"
#include "ScrollBar.h"

#include <SDL.h>
#include <vector>
#include <string>

/// A class for a list box widget
class ListBox : public Widget {
public:
	/// default constructor
	ListBox();

	/// destructor
	virtual ~ListBox();

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
		Handles mouse wheel scrolling.
		\param	x x-coordinate (relative to the left top corner of the widget)
		\param	y y-coordinate (relative to the left top corner of the widget)
		\param	up	true = mouse wheel up, false = mouse wheel down
		\return	true = the mouse wheel scrolling was processed by the widget, false = mouse wheel scrolling was not processed by the widget
	*/
	virtual bool HandleMouseWheel(Sint32 x, Sint32 y, bool up);

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
		Point tmp = scrollbar.GetMinimumSize();
		tmp.x += 30;
		return tmp;
	}

	/**
		Sets this widget active. The parent widgets are also activated and the
		currently active widget is set to inactive.
	*/
	virtual void SetActive();

	/**
		Returns whether this widget can be set active.
		\return	true = activatable, false = not activatable
	*/
	virtual inline bool IsActivatable() const { return IsEnabled(); };

	/**
		Adds a new entry to this list box
		\param	text	the text to be added to the list
		\param	data	an integer value that is assigned to this entry (see GetEntryData)
	*/
	void AddEntry(std::string text, int data = 0) {
		Elements.push_back(ListEntry(text,data));
		UpdateList();
	}

    /**
		Insert a new entry to this list box at the specified index
		\param  index   the index this entry should be inserted before.
		\param	text	the text to be added to the list
		\param	data	an integer value that is assigned to this entry (see GetEntryData)
	*/
	void InsertEntry(int index, std::string text, int data = 0) {
	    if(index <= SelectedElement) SelectedElement++;

		Elements.insert(Elements.begin() + index, ListEntry(text,data));
		UpdateList();
	}

	/**
		Returns the number of entries in this list box
		\return	number of entries
	*/
	int GetNumEntries() {
		return Elements.size();
	}

	/**
		Returns the text of the entry specified by index.
		\param	index   the zero-based index of the entry
		\return	the text of the entry
	*/
	std::string GetEntry(unsigned int index) {
		if(index < Elements.size()) {
			return Elements.at(index).text;
		} else {
			return "";
		}
	}

	/**
		Returns the data assigned to the entry specified by index.
		\param	index   the zero-based index of the entry
		\return	the data of the entry
	*/
	int GetEntryData(unsigned int index) {
		if(index < Elements.size()) {
			return Elements.at(index).data;
		} else {
			return 0;
		}
	}

	/**
		Returns the zero-based index of the current selected entry.
		\return	the index of the selected element (-1 if none is selected)
	*/
	int GetSelectedIndex() {
		return SelectedElement;
	}

	/**
		Sets the selected item and scrolls the scrollbar to that position.
		\param index	the new index (-1 == select nothing)
	*/
	void SetSelectedItem(int index);

	/**
		Removes the entry which is specified by index
		\param	index	the zero-based index of the element to remove
	*/
	void RemoveEntry(int index) {
		std::vector<ListEntry>::iterator iter = Elements.begin() + index;
		Elements.erase(iter);
		if(index == SelectedElement) {
			SelectedElement = -1;
		} else if(index < SelectedElement) {
            SelectedElement--;
		}

	    if(index < FirstVisibleElement) FirstVisibleElement--;

		UpdateList();
	}

	/**
	        Deletes all entries in the list.
	*/
	void ClearAllEntries() {
	    Elements.clear();
	    SelectedElement = -1;
	    UpdateList();
	}

	/**
		Sets the method that should be called when the selection in this list box changes.
		\param	pMethod	A pointer to this method
	*/
	inline void SetOnSelectionChangeMethod(MethodPointer pMethod) {
		pOnSelectionChange = pMethod;
	}

	/**
		Sets the method that should be called when a list entry is double clicked.
		\param	pMethod	A pointer to this method
	*/
	inline void SetOnDoubleClickMethod(MethodPointer pMethod) {
		pOnDoubleClick = pMethod;
	}

    /**
		Sets the color for this list box.
		\param	color   the color (-1 = default color)
	*/
	virtual inline void SetColor(int color) {
		this->color = color;
		UpdateList();
		scrollbar.SetColor(color);
	}

    /**
		Is the scrollbar always shown or is it hidden if not needed
		\return	true if scrollbar is hidden if not needed
	*/
	bool getAutohideScrollbar() const { return bAutohideScrollbar; }

    /**
		Set if the scrollbar shall be hidden if not needed
		\param  bAutohideScrollbar	true = hide scrollbar, false = show always
	*/
    void setAutohideScrollbar(bool bAutohideScrollbar) {
        this->bAutohideScrollbar = bAutohideScrollbar;
    }

private:
	void UpdateList();

	void OnScrollbarChange() {
		FirstVisibleElement = scrollbar.GetCurrentValue();
		UpdateList();
	}

	class ListEntry {
	public:
		ListEntry(std::string text, int data) {
			this->text = text;
			this->data = data;
		}

		std::string text;
		int data;
	};

	std::vector<ListEntry> Elements;
	SDL_Surface* pBackground;
	SDL_Surface* pForeground;
	ScrollBar scrollbar;

	MethodPointer pOnSelectionChange;			///< this method is called when the selection changes
	MethodPointer pOnDoubleClick;				///< this method is called when a list entry is double clicked

    int color;				                    ///< the color
    bool bAutohideScrollbar;                    ///< hide the scrollbar if not needed (default = true)
	int FirstVisibleElement;                    ///< the index of the first shown element in the list
	int SelectedElement;                        ///< the selected element
	Uint32 LastClickTime;                       ///< the time an element was clicked on the last time (needed for double clicking)
};

#endif //LISTBOX_H
