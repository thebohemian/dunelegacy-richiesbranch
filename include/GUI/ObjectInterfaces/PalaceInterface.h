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

#ifndef PALACEINTERFACE_H
#define PALACEINTERFACE_H

#include "DefaultStructureInterface.h"

#include <FileClasses/FontManager.h>
#include <FileClasses/TextManager.h>

#include <GUI/ProgressBar.h>

#include <structures/PalaceClass.h>

class PalaceInterface : public DefaultStructureInterface {
public:
	static PalaceInterface* Create(int ObjectID) {
		PalaceInterface* tmp = new PalaceInterface(ObjectID);
		tmp->pAllocated = true;
		return tmp;
	}

protected:
	PalaceInterface(int ObjectID) : DefaultStructureInterface(ObjectID) {
		Main_HBox.AddWidget(&WeaponBox);

		SDL_Surface* pSurface = pGFXManager->getSmallDetailPic(Picture_DeathHand);
		WeaponBox.AddWidget(&Weapon,Point((GAMEBARWIDTH - 25 - pSurface->w)/2,0),
									Point(pSurface->w, pSurface->h));

		WeaponBox.AddWidget(&WeaponSelect,Point((GAMEBARWIDTH - 25 - pSurface->w)/2,0),
									Point(pSurface->w, pSurface->h));

		SDL_Surface* pText = pFontManager->createSurfaceWithText(pTextManager->getLocalized("READY"), COLOR_WHITE, FONT_STD10);


		SDL_Surface* pReady = SDL_CreateRGBSurface(SDL_HWSURFACE,pSurface->w,pSurface->h,8,0,0,0,0);
		palette.applyToSurface(pReady);
		SDL_SetColorKey(pReady, SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);

		SDL_Rect dest = { (pReady->w - pText->w)/2,(pReady->h - pText->h)/2, pText->w, pText->h };
		SDL_BlitSurface(pText, NULL, pReady, &dest);

		SDL_FreeSurface(pText);
		WeaponSelect.SetSurfaces(pReady,true);
		WeaponSelect.SetVisible(false);

		WeaponSelect.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&PalaceInterface::OnSpecial)));
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

		PalaceClass* pPalace = dynamic_cast<PalaceClass*>(pObject);

		if(pPalace != NULL) {
			int picID;

			switch (pPalace->getOwner()->getHouseID())
			{
				case(HOUSE_ATREIDES): case(HOUSE_FREMEN):
					picID = Picture_Fremen;
					break;
				case(HOUSE_ORDOS): case(HOUSE_MERCENARY):
					picID = Picture_Saboteur;
					break;
				case(HOUSE_HARKONNEN): case(HOUSE_SARDAUKAR):
					picID = Picture_DeathHand;
					break;
				default:
					picID = Picture_Fremen;
					break;
			}

			Weapon.SetSurface(pGFXManager->getSmallDetailPic(picID),false);
			Weapon.SetProgress(pPalace->getPercentComplete());

			WeaponSelect.SetVisible(pPalace->specialReady());
		}

		return DefaultStructureInterface::update();
	}

private:
	void OnSpecial() {
		ObjectClass* pObject = currentGame->getObjectManager().getObject(ObjectID);
		if(pObject == NULL) {
			return;
		}

		PalaceClass* pPalace = dynamic_cast<PalaceClass*>(pObject);

		if(pPalace != NULL) {
		    if((pPalace->getOriginalHouse() == HOUSE_HARKONNEN) || (pPalace->getOriginalHouse() == HOUSE_SARDAUKAR)) {
                currentGame->currentCursorMode = Game::CursorMode_Attack;
		    } else {
                pPalace->HandleSpecialClick();
		    }
		}
	};

	StaticContainer		WeaponBox;
	PictureProgressBar	Weapon;
	PictureButton		WeaponSelect;
};

#endif // PALACEINTERFACE_H
