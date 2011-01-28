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

#include <Menu/HouseChoiceMenu.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <Menu/HouseChoiceInfoMenu.h>
#include <SoundPlayer.h>


HouseChoiceMenu::HouseChoiceMenu() : MenuClass()
{
	// set up window
	SDL_Surface *surf;
	surf = pGFXManager->getUIGraphic(UI_HouseChoiceBackground);

	SetBackground(surf,false);

	int xpos = std::max(0,(screen->w - surf->w)/2);
	int ypos = std::max(0,(screen->h - surf->h)/2);

	SetCurrentPosition(xpos,ypos,surf->w,surf->h);

	SetWindowWidget(&WindowWidget);

	// set up buttons

	Button_Atreides.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&HouseChoiceMenu::OnAtreides)));
	WindowWidget.AddWidget(&Button_Atreides, Point(40,108),	Point(168,182));

	Button_Ordos.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&HouseChoiceMenu::OnOrdos)));
	WindowWidget.AddWidget(&Button_Ordos, Point(235,108),	Point(168,182));

	Button_Harkonnen.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&HouseChoiceMenu::OnHarkonnen)));
	WindowWidget.AddWidget(&Button_Harkonnen, Point(430,108),	Point(168,182));

}

HouseChoiceMenu::~HouseChoiceMenu() {
}

void HouseChoiceMenu::OnAtreides() {
    soundPlayer->playSound(HouseAtreides);

	HouseChoiceInfoMenu* myHouseChoiceInfoMenu = new HouseChoiceInfoMenu(HOUSE_ATREIDES);
	if(myHouseChoiceInfoMenu == NULL) {
		perror("HouseChoiceMenu::OnAtreides()");
		exit(EXIT_FAILURE);
	}
	if(myHouseChoiceInfoMenu->showMenu() == -1) {
		quit(-1);
	} else {
		quit(HOUSE_ATREIDES);
	}
	delete myHouseChoiceInfoMenu;
}

void HouseChoiceMenu::OnOrdos() {
    soundPlayer->playSound(HouseOrdos);

	HouseChoiceInfoMenu* myHouseChoiceInfoMenu = new HouseChoiceInfoMenu(HOUSE_ORDOS);
	if(myHouseChoiceInfoMenu == NULL) {
		perror("HouseChoiceMenu::OnOrdos()");
		exit(EXIT_FAILURE);
	}
	if(myHouseChoiceInfoMenu->showMenu() == -1) {
		quit(-1);
	} else {
		quit(HOUSE_ORDOS);
	}
	delete myHouseChoiceInfoMenu;
}

void HouseChoiceMenu::OnHarkonnen() {
    soundPlayer->playSound(HouseHarkonnen);

	HouseChoiceInfoMenu* myHouseChoiceInfoMenu = new HouseChoiceInfoMenu(HOUSE_HARKONNEN);
	if(myHouseChoiceInfoMenu == NULL) {
		perror("HouseChoiceMenu::OnHarkonnen()");
		exit(EXIT_FAILURE);
	}
	if(myHouseChoiceInfoMenu->showMenu() == -1) {
		quit(-1);
	} else {
		quit(HOUSE_HARKONNEN);
	}
	delete myHouseChoiceInfoMenu;
}
