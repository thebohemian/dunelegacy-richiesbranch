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

#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#include <misc/Stream.h>

#include <map>
#include <SDL.h>

// forward declarations
class ObjectClass;

typedef std::map<Uint32,ObjectClass*> ObjectMap;

/// This class holds all objects (structures and units) in the game.
class ObjectManager{
public:
	/**
		Default constructor
	*/
    ObjectManager() : nextFreeObjectID(1)
    {
    }

    /**
		Default destructor
	*/
    ~ObjectManager() { ; }


    /**
		Saves all objects to a stream
		\param	stream	Stream to save to
	*/
	void save(Stream& stream) const;

	/**
		Loads all objects from a stream
		\param	stream	Stream to load from
	*/
	void load(Stream& stream);

    /**
		This method adds one object. The ObjectID is choosen automatically.
		\param	pObject	A pointer to the object.
		\return ObjectID of the added object.
	*/
    Uint32 AddObject(ObjectClass* pObject);

    /**
		This method searches for the object with ObjectID.
		\param	ObjectID		ID of the object to search for
		\return Pointer to this object (NULL if not found)
	*/
	inline ObjectClass* getObject(Uint32 ObjectID) const {
	    ObjectMap::const_iterator iter = objectMap.find(ObjectID);

	    if(iter == objectMap.end()) {
            return NULL;
	    } else {
            return iter->second;
	    }
	}

	/**
		This method removes one object.
		\param	ObjectID		ID of the object to remove
		\return false if there was no object with this ObjectID, true if it could be removed
	*/
	bool RemoveObject(Uint32 ObjectID) {
	    return (bool) objectMap.erase(ObjectID);
	}

private:
    Uint32 nextFreeObjectID;
    ObjectMap objectMap;
};

#endif //OBJECTMANAGER_H
