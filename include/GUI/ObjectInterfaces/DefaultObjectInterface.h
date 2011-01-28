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

#ifndef DEFAULTOBJECTINTERFACE_H
#define DEFAULTOBJECTINTERFACE_H

#include "ObjectInterface.h"

#include <GUI/PictureLabel.h>

#include <globals.h>

#include <Game.h>

#include <sand.h>

#include <ObjectClass.h>



class DefaultObjectInterface : public ObjectInterface {
public:
	static DefaultObjectInterface* Create(int ObjectID) {
		DefaultObjectInterface* tmp = new DefaultObjectInterface(ObjectID);
		tmp->pAllocated = true;
		return tmp;
	}

protected:
	DefaultObjectInterface(int ObjectID) : ObjectInterface() {
		ObjectClass* pObject = currentGame->getObjectManager().getObject(ObjectID);
		if(pObject == NULL) {
			fprintf(stderr,"DefaultObjectInterface::DefaultObjectInterface(): Cannot resolve ObjectID %d!\n",ObjectID);
			exit(EXIT_FAILURE);
		}

		this->ObjectID = ObjectID;
		ItemID = pObject->getItemID();

		AddWidget(&TopBox,Point(0,0),Point(GAMEBARWIDTH - 25,100));

		AddWidget(&Main_HBox,Point(0,80),Point(GAMEBARWIDTH - 25,screen->h - 80 - 148));

		TopBox.AddWidget(&TopBox_HBox,Point(0,22),Point(GAMEBARWIDTH - 25,100));

		TopBox_HBox.AddWidget(Spacer::Create());
		TopBox_HBox.AddWidget(&ObjPicture);
		ObjPicture.SetSurface(resolveItemPicture(ItemID),false);

		TopBox_HBox.AddWidget(Spacer::Create());
	};

	virtual ~DefaultObjectInterface() { ; };

	/**
		This method updates the object interface.
		If the object doesn't exists anymore then update returns false.
		\return true = everything ok, false = the object container should be removed
	*/
	virtual bool update() {
		ObjectClass* pObject = currentGame->getObjectManager().getObject(ObjectID);
		return (pObject != NULL);
	}

	int				ObjectID;
	int 			ItemID;

	StaticContainer	TopBox;
	HBox			TopBox_HBox;
	HBox			Main_HBox;
	PictureLabel	ObjPicture;
};

#endif // DEFAULTOBJECTINTERFACE_H
