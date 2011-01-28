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

#ifndef BUILDERINTERFACE_H
#define BUILDERINTERFACE_H

#include "DefaultStructureInterface.h"

#include <FileClasses/FontManager.h>
#include <FileClasses/TextManager.h>

#include <GUI/TextButton.h>
#include <GUI/ProgressBar.h>
#include <GUI/dune/BuilderList.h>

#include <misc/draw_util.h>
#include <misc/string_util.h>

#include <structures/BuilderClass.h>
#include <structures/StarPortClass.h>

class BuilderInterface : public DefaultStructureInterface {
public:
	static BuilderInterface* Create(int ObjectID) {
		BuilderInterface* tmp = new BuilderInterface(ObjectID);
		tmp->pAllocated = true;
		return tmp;
	}

protected:
	BuilderInterface(int ObjectID) : DefaultStructureInterface(ObjectID) {
		Button_Upgrade.SetText(pTextManager->getLocalized("Upgrade"));
		Button_Upgrade.SetTextColor(houseColor[pLocalHouse->getHouseID()]+3);
		Button_Upgrade.SetVisible(false);
		Button_Upgrade.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&BuilderInterface::OnUpgrade)));

		ProgressBar_Upgrade.SetText(pTextManager->getLocalized("Upgrade"));
		ProgressBar_Upgrade.SetTextColor(houseColor[pLocalHouse->getHouseID()]+3);
		ProgressBar_Upgrade.SetVisible(false);
		TopBox.AddWidget(&Button_Upgrade,Point(18,2),Point(83,18));
		TopBox.AddWidget(&ProgressBar_Upgrade,Point(18,2),Point(83,18));

		Main_HBox.AddWidget(Spacer::Create());

		ObjectClass* pObject = currentGame->getObjectManager().getObject(ObjectID);
		BuilderClass* pBuilder = dynamic_cast<BuilderClass*>(pObject);
		pBuilderList = BuilderList::Create(pBuilder->getObjectID());
		Main_HBox.AddWidget(pBuilderList);

		Main_HBox.AddWidget(Spacer::Create());
	}


	void OnUpgrade() {
		ObjectClass* pObject = currentGame->getObjectManager().getObject(ObjectID);
		BuilderClass* pBuilder = dynamic_cast<BuilderClass*>(pObject);
		if(pBuilder != NULL) {
			pBuilder->HandleUpgradeClick();
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

		BuilderClass* pBuilder = dynamic_cast<BuilderClass*>(pObject);
		if(pBuilder != NULL) {
		    StarPortClass* pStarport = dynamic_cast<StarPortClass*>(pBuilder);
		    if(pStarport != NULL) {
		        int arrivalTimer = pStarport->getArrivalTimer();
		        SDL_Surface* pSurface = copySurface(resolveItemPicture(pStarport->getItemID()));

                if(arrivalTimer > 0) {
                    int seconds = ((arrivalTimer*10)/(MILLI2CYCLES(30*1000))) + 1;
                    SDL_Surface* pText = pFontManager->createSurfaceWithText(stringify<int>(seconds), COLOR_WHITE, FONT_STD24);

                    SDL_Rect dest = { (pSurface->w - pText->w)/2,(pSurface->h - pText->h)/2 + 5, pText->w, pText->h };
                    SDL_BlitSurface(pText, NULL, pSurface, &dest);
                    SDL_FreeSurface(pText);
                }

                ObjPicture.SetSurface(pSurface, true);
		    }

			ProgressBar_Upgrade.SetVisible(pBuilder->IsUpgrading());
			Button_Upgrade.SetVisible(!pBuilder->IsUpgrading());

			if(pBuilder->AllowedToUpgrade() == true) {
				ProgressBar_Upgrade.SetProgress((pBuilder->GetUpgradeProgress() * 100)/pBuilder->GetUpgradeCost());
			}

			if(pBuilder->getHealth() >= pBuilder->getMaxHealth()) {
				Button_Repair.SetVisible(false);
				if((pBuilder->AllowedToUpgrade() == true)
					&& (pBuilder->CurrentUpgradeLevel() < pBuilder->MaxUpgradeLevel())) {
					Button_Upgrade.SetVisible(true);
				} else {
					Button_Upgrade.SetVisible(false);
				}
			} else {
				Button_Repair.SetVisible(true);
				Button_Upgrade.SetVisible(false);
				Button_Repair.SetToggleState(pBuilder->IsRepairing());
			}
		}

		return true;
	}



	TextButton		Button_Upgrade;
	TextProgressBar	ProgressBar_Upgrade;
	BuilderList*	pBuilderList;
};

#endif // BUILDERINTERFACE_H
