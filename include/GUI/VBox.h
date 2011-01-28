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

#ifndef VBOX_H
#define VBOX_H

#include "Container.h"

class VBox_WidgetData {
public:
	VBox_WidgetData() : pWidget(NULL), fixedHeight(0), weight(0.0) { };
	VBox_WidgetData(Widget* _pWidget, Sint32 _fixedHeight) : pWidget(_pWidget), fixedHeight(_fixedHeight), weight(0.0) { };
	VBox_WidgetData(Widget* _pWidget, double _weight) : pWidget(_pWidget), fixedHeight(-1), weight(_weight) { };

	Widget* pWidget;
	Sint32 fixedHeight;
	double weight;
};

/// A container class for vertical aligned widgets.
class VBox : public Container<VBox_WidgetData> {
public:
	/// default constructor
	VBox() : Container<VBox_WidgetData>() {
		;
	}

	/// destructor
	virtual ~VBox() {
		;
	}

	/**
		This method adds a new widget to this container.
		\param newWidget	Widget to add
		\param fixedHeight	a fixed height for this widget (must be greater than the minimum size)
	*/
	virtual void AddWidget(Widget* newWidget, Sint32 fixedHeight) {
		if(newWidget != NULL) {
			ContainedWidgets.push_back(VBox_WidgetData(newWidget, fixedHeight));
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
			ContainedWidgets.push_back(VBox_WidgetData(newWidget, weight));
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
			p.x = std::max(p.x,curWidget->GetMinimumSize().x);
			if(iter->fixedHeight > 0) {
                p.y += iter->fixedHeight;
			} else {
                p.y += curWidget->GetMinimumSize().y;
			}
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
		Sint32 AvailableHeight = height;

		int RemainingWidgets = ContainedWidgets.size();

		// Find objects that are not allowed to be resized or have a fixed width
		// also find the sum of all weights
		double weightSum = 0.0;
		WidgetList::const_iterator iter;
		for(iter = ContainedWidgets.begin(); iter != ContainedWidgets.end(); ++iter) {
			Widget* curWidget = iter->pWidget;
			if(curWidget->ResizingYAllowed() == false) {
				AvailableHeight = AvailableHeight - curWidget->GetSize().y;
				RemainingWidgets--;
			} else if(iter->fixedHeight > 0) {
				AvailableHeight = AvailableHeight - iter->fixedHeight;
				RemainingWidgets--;
			} else {
				weightSum += iter->weight;
			}
		}

		// Under the resizeable widgets find all objects that are oversized (minimum size > AvailableHeight*weight)
		// also calculate the weight sum of all the resizeable widgets that are not oversized
		Sint32 neededOversizeHeight = 0;
		double notOversizedWeightSum = 0.0;
		for(iter = ContainedWidgets.begin(); iter != ContainedWidgets.end(); ++iter) {
			Widget* curWidget = iter->pWidget;
			if(curWidget->ResizingYAllowed() == true && iter->fixedHeight <= 0) {
				if((double) curWidget->GetMinimumSize().y > AvailableHeight * (iter->weight/weightSum)) {
					neededOversizeHeight += curWidget->GetMinimumSize().y;
				} else {
					notOversizedWeightSum += iter->weight;
				}
			}
		}


		Sint32 TotalAvailableHeight = AvailableHeight;
		for(iter = ContainedWidgets.begin(); iter != ContainedWidgets.end(); ++iter) {
			Widget* curWidget = iter->pWidget;
			Sint32 WidgetWidth;
			if(curWidget->ResizingXAllowed() == true) {
				WidgetWidth = width;
			} else {
				WidgetWidth = curWidget->GetMinimumSize().x;
			}

			if(curWidget->ResizingYAllowed() == true) {
				Sint32 WidgetHeight = 0;

				if(iter->fixedHeight <= 0) {
					if(RemainingWidgets <= 1) {
						WidgetHeight = AvailableHeight;
					} else if((double) curWidget->GetMinimumSize().y > TotalAvailableHeight * (iter->weight/weightSum)) {
						WidgetHeight = curWidget->GetMinimumSize().y;
					} else {
						WidgetHeight = (Sint32) ((TotalAvailableHeight-neededOversizeHeight) * (iter->weight/notOversizedWeightSum));
					}
					AvailableHeight -= WidgetHeight;
					RemainingWidgets--;
				} else {
					WidgetHeight = iter->fixedHeight;
				}

				curWidget->Resize(WidgetWidth,WidgetHeight);
			} else {
				curWidget->Resize(WidgetWidth, curWidget->GetSize().y);
			}
		}

		Container<VBox_WidgetData>::Resize(width,height);
	}

    /**
		This static method creates a dynamic VBox object.
		The idea behind this method is to simply create a new VBox on the fly and
		add it to a container. If the container gets destroyed also this VBox will be freed.
		\return	The new created VBox (will be automatically destroyed when it's parent widget is destroyed)
	*/
	static VBox* Create() {
		VBox* vbox = new VBox();
		vbox->pAllocated = true;
		return vbox;
	}

protected:
	/**
		This method must be overwritten by all container classes. It should return
		the position of the specified widget.
		\param widgetData	the widget data to get the position from.
		\return The position of the left upper corner
	*/
	virtual Point GetPosition(const VBox_WidgetData& widgetData) const {
		Point p(0,0);
		WidgetList::const_iterator iter;
		for(iter = ContainedWidgets.begin(); iter != ContainedWidgets.end(); ++iter) {
			Widget* curWidget = iter->pWidget;
			if(widgetData.pWidget == curWidget) {
				return p;
			} else {
				p.y = p.y + curWidget->GetSize().y;
			}
		}

		//should not happen
		return Point(0,0);
	}
};

#endif // VBOX_H

