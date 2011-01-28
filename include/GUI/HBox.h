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

#ifndef HBOX_H
#define HBOX_H

#include "Container.h"

class HBox_WidgetData {
public:
	HBox_WidgetData() : pWidget(NULL), fixedWidth(0), weight(0.0) { };
	HBox_WidgetData(Widget* _pWidget, Sint32 _fixedWidth) : pWidget(_pWidget), fixedWidth(_fixedWidth), weight(0.0) { };
	HBox_WidgetData(Widget* _pWidget, double _weight) : pWidget(_pWidget), fixedWidth(-1), weight(_weight) { };

	Widget* pWidget;
	Sint32 fixedWidth;
	double weight;
};

/// A container class for horizontal aligned widgets.
class HBox : public Container<HBox_WidgetData> {
public:
	/// default constructor
	HBox() : Container<HBox_WidgetData>() {
		;
	}

	/// destructor
	virtual ~HBox() {
		;
	}

	/**
		This method adds a new widget to this container.
		\param newWidget	Widget to add
		\param fixedWidth	a fixed width for this widget (must be greater than the minimum size)
	*/
	virtual void AddWidget(Widget* newWidget, Sint32 fixedWidth) {
		if(newWidget != NULL) {
			ContainedWidgets.push_back(HBox_WidgetData(newWidget, fixedWidth));
			newWidget->SetParent(this);
			Widget::ResizeAll();
		}
	}

	/**
		This method adds a new widget to this container.
		\param newWidget	Widget to add
		\param weight		The weight for this widget (default=1.0)
	*/
	virtual void AddWidget(Widget* newWidget, double weight = 1.0) {
		if(newWidget != NULL) {
			ContainedWidgets.push_back(HBox_WidgetData(newWidget, weight));
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
			Widget* curWidget = iter->pWidget;
			if(iter->fixedWidth > 0) {
                p.x += iter->fixedWidth;
			} else {
                p.x += curWidget->GetMinimumSize().x;
			}
			p.y = std::max(p.y,curWidget->GetMinimumSize().y);
		}
		return p;
	}

	/**
		This method resizes the container to width and height. This method should only be
		called if the new size is a valid size for this container (See ResizingXAllowed,
		ResizingYAllowed, GetMinumumSize). It also resizes all child widgets.
		\param	width	the new width of this container
		\param	height	the new height of this container
	*/
	virtual void Resize(Uint32 width, Uint32 height) {
		Sint32 AvailableWidth = width;

		int RemainingWidgets = ContainedWidgets.size();

		// Find objects that are not allowed to be resized or have a fixed width
		// also find the sum of all weights
		double weightSum = 0.0;
		WidgetList::const_iterator iter;
		for(iter = ContainedWidgets.begin(); iter != ContainedWidgets.end(); ++iter) {
			Widget* curWidget = iter->pWidget;
			if(curWidget->ResizingXAllowed() == false) {
				AvailableWidth = AvailableWidth - curWidget->GetSize().x;
				RemainingWidgets--;
			} else if(iter->fixedWidth > 0) {
				AvailableWidth = AvailableWidth - iter->fixedWidth;
				RemainingWidgets--;
			} else {
				weightSum += iter->weight;
			}
		}

		// Under the resizeable widgets find all objects that are oversized (minimum size > availableWidth*weight)
		// also calculate the weight sum of all the resizeable widgets that are not oversized
		Sint32 neededOversizeWidth = 0;
		double notOversizedWeightSum = 0.0;
		for(iter = ContainedWidgets.begin(); iter != ContainedWidgets.end(); ++iter) {
			Widget* curWidget = iter->pWidget;
			if(curWidget->ResizingXAllowed() == true && iter->fixedWidth <= 0) {
				if((double) curWidget->GetMinimumSize().x > AvailableWidth * (iter->weight/weightSum)) {
					neededOversizeWidth += curWidget->GetMinimumSize().x;
				} else {
					notOversizedWeightSum += iter->weight;
				}
			}
		}


		Sint32 TotalAvailableWidth = AvailableWidth;
		for(iter = ContainedWidgets.begin(); iter != ContainedWidgets.end(); ++iter) {
			Widget* curWidget = iter->pWidget;
			Sint32 WidgetHeight;
			if(curWidget->ResizingYAllowed() == true) {
				WidgetHeight = height;
			} else {
				WidgetHeight = curWidget->GetMinimumSize().y;
			}

			if(curWidget->ResizingXAllowed() == true) {
				Sint32 WidgetWidth = 0;

				if(iter->fixedWidth <= 0) {
					if(RemainingWidgets <= 1) {
						WidgetWidth = AvailableWidth;
					} else if((double) curWidget->GetMinimumSize().x > TotalAvailableWidth * (iter->weight/weightSum)) {
						WidgetWidth = curWidget->GetMinimumSize().x;
					} else {
						WidgetWidth = (Sint32) ((TotalAvailableWidth-neededOversizeWidth) * (iter->weight/notOversizedWeightSum));
					}
					AvailableWidth -= WidgetWidth;
					RemainingWidgets--;
				} else {
					WidgetWidth = iter->fixedWidth;
				}

				curWidget->Resize(WidgetWidth,WidgetHeight);
			} else {
				curWidget->Resize(curWidget->GetSize().x,WidgetHeight);
			}
		}

		Container<HBox_WidgetData>::Resize(width,height);
	}

    /**
		This static method creates a dynamic HBox object.
		The idea behind this method is to simply create a new HBox on the fly and
		add it to a container. If the container gets destroyed also this HBox will be freed.
		\return	The new created HBox (will be automatically destroyed when it's parent widget is destroyed)
	*/
	static HBox* Create() {
		HBox* hbox = new HBox();
		hbox->pAllocated = true;
		return hbox;
	}

protected:
	/**
		This method must be overwritten by all container classes. It should return
		the position of the specified widget.
		\param widgetData	the widget data to get the position from.
		\return The position of the left upper corner
	*/
	virtual Point GetPosition(const HBox_WidgetData& widgetData) const {
		Point p(0,0);
		WidgetList::const_iterator iter;
		for(iter = ContainedWidgets.begin(); iter != ContainedWidgets.end(); ++iter) {
			Widget* curWidget = iter->pWidget;
			if(widgetData.pWidget == curWidget) {
				return p;
			} else {
				p.x = p.x + curWidget->GetSize().x;
			}
		}

		//should not happen
		return Point(0,0);
	}
};

#endif // HBOX_H

