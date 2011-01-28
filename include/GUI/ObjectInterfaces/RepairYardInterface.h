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

#ifndef REPAIRYARDINTERFACE_H
#define REPAIRYARDINTERFACE_H

#include "DefaultStructureInterface.h"

#include <GUI/ProgressBar.h>

#include <units/UnitClass.h>

#include <structures/RepairYardClass.h>

class RepairYardInterface : public DefaultStructureInterface {
public:
	static RepairYardInterface* Create(int ObjectID) {
		RepairYardInterface* tmp = new RepairYardInterface(ObjectID);
		tmp->pAllocated = true;
		return tmp;
	}

protected:
	RepairYardInterface(int ObjectID) : DefaultStructureInterface(ObjectID) {
		Main_HBox.AddWidget(Spacer::Create());
		Main_HBox.AddWidget(&RepairUnit);
		Main_HBox.AddWidget(Spacer::Create());
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

		RepairYardClass* pRepairYard = dynamic_cast<RepairYardClass*>(pObject);

		if(pRepairYard != NULL) {
			UnitClass* pUnit = pRepairYard->getRepairUnit();

			if(pUnit != NULL) {
				RepairUnit.SetVisible(true);
				RepairUnit.SetSurface(resolveItemPicture(pUnit->getItemID()),false);
				RepairUnit.SetProgress((pUnit->getHealth()*100)/pUnit->getMaxHealth());
			} else {
				RepairUnit.SetVisible(false);
			}
		}

		return DefaultStructureInterface::update();
	}

private:
	PictureProgressBar	RepairUnit;
};

#endif // REPAIRYARDINTERFACE_H
