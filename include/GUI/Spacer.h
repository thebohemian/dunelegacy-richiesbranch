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

#ifndef SPACER_H
#define SPACER_H

#include "Widget.h"

#include <iostream>

/// A variable spacer class. This spacer fills in between widgets
class Spacer : public Widget {
public:
	/// default constructor
	Spacer() : Widget() {
		EnableResizing(true,true);
	}

	/// destructor
	virtual ~Spacer() {
	}

	/**
		Returns the minimum size of this spacer. That is (0,0).
		\return the minimum size of this widget
	*/
	virtual Point GetMinimumSize() const { return Point(0,0); };

	/**
		This static method creates a dynamic spacer object.
		The idea behind this method is to simply create a new spacer on the fly and
		add it to a container. If the container gets destroyed also this spacer will be freed.
		\return	The new created spacer (will be automatically destroyed when it's parent widget is destroyed)
	*/
	static Spacer* Create() {
		Spacer* sp = new Spacer();
		sp->pAllocated = true;
		return sp;
	}
};

/// A horizontal fixed-size spacer class
class HSpacer : public Widget {
public:
	/// default constructor
	HSpacer() : Widget() {
		Resize(0,0);
		EnableResizing(false,false);
	}

	/**
		Constructor.
		\param width	Width of this spacer.
	*/
	HSpacer(Uint32 width) : Widget() {
		this->width = width;
		Resize(width,0);
		EnableResizing(false,false);
	}

	/// destructor
	virtual ~HSpacer() {
		;
	}

	/**
		Returns the minimum size of this spacer. The returned size is (width,0).
		\return the minimum size of this spacer
	*/
	virtual Point GetMinimumSize() const {
		return Point(width,0);
	}

	/**
		This static method creates a dynamic horizontal spacer object.
		The idea behind this method is to simply create a new spacer on the fly and
		add it to a container. If the container gets destroyed also this spacer will be freed.
		\param width width of this spacer
		\return	The new created spacer (will be automatically destroyed when it's parent widget is destroyed)
	*/
	static HSpacer* Create(Uint32 width) {
		HSpacer* sp = new HSpacer(width);
		sp->pAllocated = true;
		return sp;
	}

private:
	Uint32 width;		///< width of this spacer
};

/// A vertical fixed-size spacer class
class VSpacer : public Widget {
public:
	/// default constructor
	VSpacer() : Widget() {
		Resize(0,0);
		EnableResizing(false,false);
	}

	/**
		Constructor.
		\param height	Height of this spacer.
	*/
	VSpacer(Uint32 height) : Widget() {
		this->height = height;
		Resize(0,height);
		EnableResizing(false,false);
	}

	/// destructor
	virtual ~VSpacer() {
		;
	}

	/**
		Returns the minimum size of this spacer. The returned size is (0,height).
		\return the minimum size of this spacer
	*/
	virtual Point GetMinimumSize() const {
		return Point(0,height);
	}

	/**
		This static method creates a dynamic vertical spacer object.
		The idea behind this method is to simply create a new spacer on the fly and
		add it to a container. If the container gets destroyed also this spacer will be freed.
		\param height height of this spacer
		\return	The new created spacer (will be automatically destroyed when it's parent widget is destroyed)
	*/
	static VSpacer* Create(Uint32 height) {
		VSpacer* sp = new VSpacer(height);
		sp->pAllocated = true;
		return sp;
	}

private:
	Uint32 height;		///< height of this spacer
};

#endif //SPACER_H
