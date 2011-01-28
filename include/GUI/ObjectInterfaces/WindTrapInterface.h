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

#ifndef WINDTRAPINTERFACE_H
#define WINDTRAPINTERFACE_H

#include "DefaultStructureInterface.h"

#include <FileClasses/FontManager.h>
#include <FileClasses/TextManager.h>

#include <House.h>

#include <GUI/Label.h>
#include <GUI/VBox.h>

#include <misc/string_util.h>

class WindTrapInterface : public DefaultStructureInterface {
public:
	static WindTrapInterface* Create(int ObjectID) {
		WindTrapInterface* tmp = new WindTrapInterface(ObjectID);
		tmp->pAllocated = true;
		return tmp;
	}

protected:
	WindTrapInterface(int ObjectID) : DefaultStructureInterface(ObjectID) {
        int color = houseColor[pLocalHouse->getHouseID()];

		Main_HBox.AddWidget(&TextVBox);

        Label_RequiredEnergy.SetTextFont(FONT_STD10);
        Label_RequiredEnergy.SetTextColor(color+3);
		TextVBox.AddWidget(&Label_RequiredEnergy, 0.005);
		Label_ProducedEnergy.SetTextFont(FONT_STD10);
		Label_ProducedEnergy.SetTextColor(color+3);
		TextVBox.AddWidget(&Label_ProducedEnergy, 0.005);
		TextVBox.AddWidget(Spacer::Create(),0.99);
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

		House* pOwner = pObject->getOwner();

        Label_RequiredEnergy.SetText(" " + pTextManager->getLocalized("Required") + ": " + stringify(pOwner->getPowerRequirement()));
        Label_ProducedEnergy.SetText(" " + pTextManager->getLocalized("Produced") + ": " + stringify(pOwner->getProducedPower()));

		return DefaultStructureInterface::update();
	}

private:
    VBox    TextVBox;

	Label   Label_RequiredEnergy;
	Label   Label_ProducedEnergy;
};

#endif // WINDTRAPINTERFACE_H
