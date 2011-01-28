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

#ifndef DEFAULTSTRUCTUREINTERFACE_H
#define DEFAULTSTRUCTUREINTERFACE_H

#include "DefaultObjectInterface.h"

#include <globals.h>

#include <FileClasses/GFXManager.h>

#include <structures/StructureClass.h>

class DefaultStructureInterface : public DefaultObjectInterface {
public:
	static DefaultStructureInterface* Create(int ObjectID) {
		DefaultStructureInterface* tmp = new DefaultStructureInterface(ObjectID);
		tmp->pAllocated = true;
		return tmp;
	}

protected:
	DefaultStructureInterface(int ObjectID) : DefaultObjectInterface(ObjectID) {
		SDL_Surface* surf, *surfPressed;

		surf = pGFXManager->getUIGraphic(UI_Repair);
		surfPressed = pGFXManager->getUIGraphic(UI_Repair_Pressed);

		Button_Repair.SetSurfaces(surf, false, surfPressed,false);
		Button_Repair.SetToggleButton(true);
		Button_Repair.SetVisible(false);
		Button_Repair.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&DefaultStructureInterface::OnRepair)));

		TopBox.AddWidget(&Button_Repair,Point(2,2),Point(surf->w,surf->h));
	}

	void OnRepair() {
		ObjectClass* pObject = currentGame->getObjectManager().getObject(ObjectID);
		StructureClass* pStructure = dynamic_cast<StructureClass*>(pObject);
		if(pStructure != NULL) {
			pStructure->HandleRepairClick();
		}
	}

	/**
		This method updates the object interface.
		If the object doesn't exists anymore then update returns false.
		\return true = everything ok, false = the object container should be removed
	*/
	virtual bool update() {
		ObjectClass* pObject = currentGame->getObjectManager().getObject(ObjectID);
		if(pObject == NULL) {
			return false;
		}

		StructureClass* pStructure = dynamic_cast<StructureClass*>(pObject);
		if(pStructure != NULL) {
			if(pStructure->getHealth() >= pStructure->getMaxHealth()) {
				Button_Repair.SetVisible(false);
			} else {
				Button_Repair.SetVisible(true);
				Button_Repair.SetToggleState(pStructure->IsRepairing());
			}
		}

		return true;
	}

	PictureButton	Button_Repair;
};

#endif // DEFAULTSTRUCTUREINTERFACE_H
