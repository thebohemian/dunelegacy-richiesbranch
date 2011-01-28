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

class ObjectPointer;

#ifndef OBJECTPOINTER_H
#define OBJECTPOINTER_H

#include <Definitions.h>
#include <misc/Stream.h>

class ObjectClass;
class UnitClass;
class StructureClass;

class ObjectPointer
{
public:
	ObjectPointer() { ObjectID = NONE; };
	ObjectPointer(Uint32 newItemID) { PointTo(newItemID); };
	ObjectPointer(ObjectClass* newObject) { PointTo(newObject); };
	~ObjectPointer() { ; };

	inline void PointTo(Uint32 newItemID) { ObjectID = newItemID; };
	void PointTo(ObjectClass* newObject);

	inline Uint32 getObjectID() const { return ObjectID; };
	ObjectClass* getObjPointer() const;
	inline UnitClass* getUnitPointer() const { return (UnitClass*) getObjPointer(); };
	inline StructureClass* getStructurePointer() const { return (StructureClass*) getObjPointer(); };

	void save(Stream& stream) const;
	void load(Stream& stream);

	operator bool() const
	{
		return (ObjectID != NONE);
	};

private:
	mutable Uint32 ObjectID;
};


#endif // OBJECTPOINTER_H
