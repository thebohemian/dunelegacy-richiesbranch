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

#include <Menu/SinglePlayerMenu.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <FileClasses/TextManager.h>
#include <misc/fnkdat.h>
#include <misc/string_util.h>

#include <Menu/CustomGameMenu.h>
#include <Menu/SinglePlayerSkirmishMenu.h>
#include <Menu/HouseChoiceMenu.h>

#include <GUI/dune/LoadSaveWindow.h>
#include <GUI/MessageBox.h>

#include <Game.h>
#include <GameInitSettings.h>
#include <sand.h>

#include <stdexcept>

SinglePlayerMenu::SinglePlayerMenu() : MenuClass()
{
	// set up window
	SDL_Surface *surf;
	surf = pGFXManager->getUIGraphic(UI_MenuBackground);

	SetBackground(surf,false);
	Resize(surf->w,surf->h);

	SetWindowWidget(&WindowWidget);

	// set up pictures in the background
	surf = pGFXManager->getUIGraphic(UI_PlanetBackground);
	PlanetPicture.SetSurface(surf,false);
	WindowWidget.AddWidget(&PlanetPicture,
							Point((screen->w - surf->w)/2,screen->h/2 - surf->h + 10),
							Point(surf->w,surf->h));

	surf = pGFXManager->getUIGraphic(UI_DuneLegacy);
	DuneLegacy.SetSurface(surf,false);
	WindowWidget.AddWidget(&DuneLegacy,
							Point((screen->w - surf->w)/2, screen->h/2 + 28),
							Point(surf->w,surf->h));

	surf = pGFXManager->getUIGraphic(UI_MenuButtonBorder);
	ButtonBorder.SetSurface(surf,false);
	WindowWidget.AddWidget(&ButtonBorder,
							Point((screen->w - surf->w)/2, screen->h/2 + 59),
							Point(surf->w,surf->h));

	// set up menu buttons
	WindowWidget.AddWidget(&MenuButtons,Point((screen->w - 160)/2,screen->h/2 + 64),
										Point(160,111));

	Button_Campaign.SetText(pTextManager->getLocalized("CAMPAIGN"));
	Button_Campaign.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&SinglePlayerMenu::OnCampaign)));
	MenuButtons.AddWidget(&Button_Campaign);
	Button_Campaign.SetActive();

	MenuButtons.AddWidget(VSpacer::Create(3));

	Button_Custom.SetText(pTextManager->getLocalized("CUSTOM GAME"));
	Button_Custom.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&SinglePlayerMenu::OnCustom)));
	MenuButtons.AddWidget(&Button_Custom);

	MenuButtons.AddWidget(VSpacer::Create(3));

	Button_Skirmish.SetText(pTextManager->getLocalized("SKIRMISH"));
	Button_Skirmish.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&SinglePlayerMenu::OnSkirmish)));
	MenuButtons.AddWidget(&Button_Skirmish);

	MenuButtons.AddWidget(VSpacer::Create(3));

	Button_LoadSavegame.SetText(pTextManager->getLocalized("LOAD GAME"));
	Button_LoadSavegame.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&SinglePlayerMenu::OnLoadSavegame)));
	MenuButtons.AddWidget(&Button_LoadSavegame);

	MenuButtons.AddWidget(VSpacer::Create(3));

	Button_LoadReplay.SetText(pTextManager->getLocalized("LOAD REPLAY"));
	Button_LoadReplay.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&SinglePlayerMenu::OnLoadReplay)));
	MenuButtons.AddWidget(&Button_LoadReplay);

	MenuButtons.AddWidget(VSpacer::Create(3));

	Button_Cancel.SetText(pTextManager->getLocalized("BACK"));
	Button_Cancel.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&SinglePlayerMenu::OnCancel)));
	MenuButtons.AddWidget(&Button_Cancel);


	// Difficulty Button
	surf = pGFXManager->getUIGraphic(UI_Difficulty);
	Picture_Difficulty.SetSurface(surf,false);
	WindowWidget.AddWidget(	&Picture_Difficulty,
							Point((screen->w-160)/4 - surf->w/2,screen->h/2 + 64),
							Point(surf->w,surf->h));

	surf = pGFXManager->getUIGraphic(UI_Dif_Medium);
	Button_Difficulty.SetSurfaces(surf,false);
	Button_Difficulty.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&SinglePlayerMenu::OnDifficulty)));
	WindowWidget.AddWidget(	&Button_Difficulty,
							Point((screen->w-160)/4 - surf->w/2 - 1,screen->h/2 + 104),
							Point(surf->w,surf->h));

	UpdateDifficulty();
}

SinglePlayerMenu::~SinglePlayerMenu()
{
}

void SinglePlayerMenu::OnCampaign()
{
	int player;

	HouseChoiceMenu* pHouseChoiceMenu = new HouseChoiceMenu();
	if(pHouseChoiceMenu == NULL) {
		perror("SinglePlayerMenu::OnCampaign()");
		exit(EXIT_FAILURE);
	}
	player = pHouseChoiceMenu->showMenu();
	delete pHouseChoiceMenu;

	if(player < 0) {
		return;
	}

	GameInitSettings init((HOUSETYPE) player, settings.General.ConcreteRequired, settings.General.FogOfWar);

    for(int houseID = 0; houseID < NUM_HOUSES; houseID++) {
	    if(houseID == player) {
	        GameInitSettings::HouseInfo humanHouseInfo((HOUSETYPE) player, 1);
	        humanHouseInfo.addPlayerInfo( GameInitSettings::PlayerInfo(settings.General.PlayerName, "HumanPlayer") );
            init.addHouseInfo(humanHouseInfo);
	    } else {
            GameInitSettings::HouseInfo aiHouseInfo((HOUSETYPE) houseID, 2);
	        aiHouseInfo.addPlayerInfo( GameInitSettings::PlayerInfo(getHouseNameByNumber( (HOUSETYPE) houseID), "AIPlayer", stringify(settings.playerDifficulty)) );
            init.addHouseInfo(aiHouseInfo);
	    }
	}

	startSinglePlayerGame(init);

	quit();
}

void SinglePlayerMenu::OnCustom()
{
    CustomGameMenu* pCustomGameMenu = new CustomGameMenu();
    if(pCustomGameMenu == NULL) {
		perror("SinglePlayerMenu::OnCustom()");
		exit(EXIT_FAILURE);
	}
    pCustomGameMenu->showMenu();
    delete pCustomGameMenu;
}

void SinglePlayerMenu::OnSkirmish()
{

	SinglePlayerSkirmishMenu* pSinglePlayerSkirmishMenu = new SinglePlayerSkirmishMenu();
	if(pSinglePlayerSkirmishMenu == NULL) {
		perror("SinglePlayerMenu::OnSkirmish()");
		exit(EXIT_FAILURE);
	}
	pSinglePlayerSkirmishMenu->showMenu();
	delete pSinglePlayerSkirmishMenu;

	// Submenu might have changed difficulty
	UpdateDifficulty();
}

void SinglePlayerMenu::OnLoadSavegame()
{
	char tmp[FILENAME_MAX];
	fnkdat("save/", tmp, FILENAME_MAX, FNKDAT_USER | FNKDAT_CREAT);
	std::string savepath(tmp);
	OpenWindow(LoadSaveWindow::Create(false, pTextManager->getLocalized("Load Game"), savepath, "dls"));
}

void SinglePlayerMenu::OnLoadReplay()
{
	char tmp[FILENAME_MAX];
	fnkdat("replay/", tmp, FILENAME_MAX, FNKDAT_USER | FNKDAT_CREAT);
	std::string replaypath(tmp);
	OpenWindow(LoadSaveWindow::Create(false, pTextManager->getLocalized("Load Replay"), replaypath, "rpl"));
}

void SinglePlayerMenu::OnCancel()
{
	quit();
}

void SinglePlayerMenu::OnDifficulty()
{
	switch(settings.playerDifficulty) {
		case EASY:
			settings.playerDifficulty = MEDIUM;
			break;
		case MEDIUM:
			settings.playerDifficulty = HARD;
			break;
		case HARD:
		default:
			settings.playerDifficulty = EASY;
			break;
	}

	UpdateDifficulty();
}


void SinglePlayerMenu::UpdateDifficulty()
{
	if (settings.playerDifficulty == EASY) {
		Button_Difficulty.SetSurfaces(pGFXManager->getUIGraphic(UI_Dif_Easy), false);
	} else if (settings.playerDifficulty == MEDIUM)	{
		Button_Difficulty.SetSurfaces(pGFXManager->getUIGraphic(UI_Dif_Medium), false);
	} else {
		Button_Difficulty.SetSurfaces(pGFXManager->getUIGraphic(UI_Dif_Hard), false);
	}
}


void SinglePlayerMenu::CloseChildWindow() {
	std::string filename = "";
	std::string extension = "";
	LoadSaveWindow* pLoadSaveWindow = dynamic_cast<LoadSaveWindow*>(pChildWindow);
	if(pLoadSaveWindow != NULL) {
		filename = pLoadSaveWindow->GetFilename();
		extension = pLoadSaveWindow->GetExtension();
	}

	MenuClass::CloseChildWindow();

	if(filename != "") {
		if(extension == "dls") {

		    try {
                startSinglePlayerGame( GameInitSettings(filename) );
		    } catch (std::exception& e) {
		        // most probably the savegame file is not valid or from a different dune legacy version
                OpenWindow(MessageBox::Create(e.what()));
            }
		} else if(extension == "rpl") {
			startReplay(filename);
		}
	}
}
