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

#include <Menu/HouseChoiceInfoMenu.h>

#include <globals.h>

#include <FileClasses/FileManager.h>
#include <FileClasses/GFXManager.h>
#include <FileClasses/TextManager.h>
#include <FileClasses/Palfile.h>

#include <string>

HouseChoiceInfoMenu::HouseChoiceInfoMenu(int newHouse) : MentatMenu(HOUSE_MERCENARY)
{
	DisableQuiting(true);

	house = newHouse;

	Animation* anim = NULL;

	switch(house) {
		case HOUSE_ATREIDES:
		case HOUSE_FREMEN:
			anim = pGFXManager->getAnimation(Anim_AtreidesPlanet);
			break;
		case HOUSE_ORDOS:
		case HOUSE_MERCENARY:
			anim = pGFXManager->getAnimation(Anim_OrdosPlanet);
			break;
		case HOUSE_HARKONNEN:
		case HOUSE_SARDAUKAR:
		default:
			anim = pGFXManager->getAnimation(Anim_HarkonnenPlanet);
			break;
	}

	PlanetAnimation.SetAnimation(anim);
	WindowWidget.AddWidget(&PlanetAnimation, Point(257,97),
							Point(anim->getFrame()->w,anim->getFrame()->h));

	// init textbox but skip first line (this line contains "House ???")
	std::string desc = pTextManager->GetBriefingText(0,MISSION_DESCRIPTION,house);
	int linebreak = desc.find("\n",0) + 1;
	setText(desc.substr(linebreak,desc.length()-linebreak));

	SDL_Surface* surf;
	SDL_Surface* surfPressed;

	surf = pGFXManager->getUIGraphic(UI_MentatYes);
	surfPressed = pGFXManager->getUIGraphic(UI_MentatYes_Pressed);

	YesButton.SetSurfaces(surf,false,surfPressed,false);
	YesButton.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&HouseChoiceInfoMenu::OnYes)));
	WindowWidget.AddWidget(&YesButton,Point(370,340),Point(surf->w,surf->h));

	surf = pGFXManager->getUIGraphic(UI_MentatNo);
	surfPressed = pGFXManager->getUIGraphic(UI_MentatNo_Pressed);

	NoButton.SetSurfaces(surf,false,surfPressed,false);
	NoButton.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&HouseChoiceInfoMenu::OnNo)));
	WindowWidget.AddWidget(&NoButton,Point(450,340),Point(surf->w,surf->h));

	// change palette
	Palette benePalette = LoadPalette_RW(pFileManager->OpenFile("BENE.PAL"), true);
    benePalette.applyToSurface(screen);
}

HouseChoiceInfoMenu::~HouseChoiceInfoMenu() {
    SDL_FillRect(screen, NULL, 0);

    // Change palette back to normal
    palette.applyToSurface(screen);
}

void HouseChoiceInfoMenu::DrawSpecificStuff() {
	MentatMenu::DrawSpecificStuff();
}

void HouseChoiceInfoMenu::OnYes() {
	quit(1);
}

void HouseChoiceInfoMenu::OnNo() {
	quit(-1);
}
