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

#ifndef STATICCONTAINER_H
#define STATICCONTAINER_H

#include "Container.h"

class StaticContainer_WidgetData {
public:
	StaticContainer_WidgetData() {
		pWidget = NULL;
		Position = Point(0,0);
		Size = Point(0,0);
	}

	StaticContainer_WidgetData(Widget* pWidget, Point Position, Point Size) {
		this->pWidget = pWidget;
		this->Position = Position;
		this->Size = Size;
	}

	Widget* pWidget;
	Point Position;
	Point Size;

	inline bool operator==(const StaticContainer_WidgetData& op) const {
		return (pWidget == op.pWidget) && (Position == op.Position) && (Size == op.Size);
	}
};

/// A container class of explicit placed widgets
class StaticContainer : public Container<StaticContainer_WidgetData> {
public:


	/// default constructor
	StaticContainer() : Container<StaticContainer_WidgetData>() {
		;
	}

	/// default destructor
	virtual ~StaticContainer() {
	}

	/**
		This method adds a new widget to this container.
		\param newWidget	Widget to add
		\param position		Position of the new Widget
		\param size			Size of the new widget
	*/
	virtual void AddWidget(Widget* newWidget, Point position, Point size) {
		if(newWidget != NULL) {
			ContainedWidgets.push_back(StaticContainer_WidgetData(newWidget,position,size));
			newWidget->Resize(size.x, size.y);
			newWidget->SetParent(this);
			Widget::ResizeAll();
		}
	}

	/**
		Returns the minimum size of this container. The container should not
		resized to a size smaller than this. If the container is not resizeable
		in a direction this method returns the size in that direction.
		\return the minimum size of this container
	*/
	virtual Point GetMinimumSize() const {
		Point p(0,0);
		WidgetList::const_iterator iter;
		for(iter = ContainedWidgets.begin(); iter != ContainedWidgets.end(); ++iter) {
			const StaticContainer_WidgetData curWidgetData = *iter;
			p.x = std::max(p.x , curWidgetData.Position.x + curWidgetData.Size.x);
			p.y = std::max(p.y , curWidgetData.Position.y + curWidgetData.Size.y);
		}
		return p;
	}

	/**
		This method resized the container to width and height. This method should only be
		called if the new size is a valid size for this container (See ResizingXAllowed,
		ResizingYAllowed, GetMinumumSize). It also resizes all child widgets.
		\param	width	the new width of this container
		\param	height	the new height of this container
	*/
	virtual void Resize(Uint32 width, Uint32 height) {
		WidgetList::const_iterator iter;
		for(iter = ContainedWidgets.begin(); iter != ContainedWidgets.end(); ++iter) {
			const StaticContainer_WidgetData curWidgetData = *iter;
			curWidgetData.pWidget->Resize(curWidgetData.Size.x,curWidgetData.Size.y);
		}
		Container<StaticContainer_WidgetData>::Resize(width,height);
	}

protected:
	/**
		This method must be overwritten by all container classes. It should return
		the position of the specified widget.
		\param widgetData	the widget data to get the position from.
		\return The position of the left upper corner
	*/
	virtual Point GetPosition(const StaticContainer_WidgetData& widgetData) const {
		return widgetData.Position;
	}

	void SetWidgetGeometry(Widget* pWidget, Point position, Point size) {
		StaticContainer_WidgetData* pWidgetData = GetWidgetDataFromWidget(pWidget);
		if(pWidgetData != 0) {
			pWidgetData->Position = position;
			pWidgetData->Size = size;
			pWidget->Resize(size.x, size.y);
		}
	}
};


#endif // STATICCONTAINER_H

