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

#include <Menu/SinglePlayerSkirmishMenu.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <FileClasses/TextManager.h>

#include <GameInitSettings.h>
#include <sand.h>

SinglePlayerSkirmishMenu::SinglePlayerSkirmishMenu() : MenuClass()
{
	HouseChoice = HOUSE_ATREIDES;
	Mission = 1;

	// set up window
	SDL_Surface *surf, *surfPressed;
	surf = pGFXManager->getUIGraphic(UI_MenuBackground);

	SetBackground(surf,false);
	Resize(surf->w,surf->h);

	SetWindowWidget(&WindowWidget);

	// set up pictures in the background
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

	Button_Start.SetText(pTextManager->getLocalized("Start"));
	Button_Start.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&SinglePlayerSkirmishMenu::OnStart)));
	MenuButtons.AddWidget(&Button_Start);
	Button_Start.SetActive();

	MenuButtons.AddWidget(VSpacer::Create(79));

	Button_Back.SetText(pTextManager->getLocalized("Back"));
	Button_Back.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&SinglePlayerSkirmishMenu::OnCancel)));
	MenuButtons.AddWidget(&Button_Back);

	// Difficulty Button
	surf = pGFXManager->getUIGraphic(UI_Difficulty);
	Picture_Difficulty.SetSurface(surf,false);
	WindowWidget.AddWidget(	&Picture_Difficulty,
							Point((screen->w-160)/4 - surf->w/2,screen->h/2 + 64),
							Point(surf->w,surf->h));

	surf = pGFXManager->getUIGraphic(UI_Dif_Medium);
	Button_Difficulty.SetSurfaces(surf,false);
	Button_Difficulty.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&SinglePlayerSkirmishMenu::OnDifficulty)));
	WindowWidget.AddWidget(	&Button_Difficulty,
							Point((screen->w-160)/4 - surf->w/2 - 1,screen->h/2 + 104),
							Point(surf->w,surf->h));

	UpdateDifficulty();


	// set up house choice

	surf = pGFXManager->getUIGraphic(UI_HouseSelect);
	WindowWidget.AddWidget(&HouseChoiceContainer,
							Point((screen->w - surf->w)/2,screen->h/2 - surf->h + 10),
							Point(surf->w,surf->h));

	HeraldPicture.SetSurface(surf,false);
	HouseChoiceContainer.AddWidget(&HeraldPicture, Point(0,0), Point(surf->w,surf->h));


	// Atreides button
	surf = pGFXManager->getUIGraphic(UI_HeraldAtre_Colored);
	Picture_AtreidesSelected.SetSurface(surf,false);
	HouseChoiceContainer.AddWidget(	&Picture_AtreidesSelected, Point(20,53), Point(surf->w,surf->h));

	Button_Atreides.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&SinglePlayerSkirmishMenu::OnSelectAtreides)));
	HouseChoiceContainer.AddWidget(	&Button_Atreides, Point(20,53), Point(surf->w,surf->h));

	// Ordos button
	surf = pGFXManager->getUIGraphic(UI_HeraldOrd_Colored);
	Picture_OrdosSelected.SetSurface(surf,false);
	HouseChoiceContainer.AddWidget(	&Picture_OrdosSelected, Point(116,53), Point(surf->w,surf->h));

	Button_Ordos.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&SinglePlayerSkirmishMenu::OnSelectOrdos)));
	HouseChoiceContainer.AddWidget(	&Button_Ordos, Point(116,53), Point(surf->w,surf->h));

	// Harkonnen button
	surf = pGFXManager->getUIGraphic(UI_HeraldHark_Colored);
	Picture_HarkonnenSelected.SetSurface(surf,false);
	HouseChoiceContainer.AddWidget(	&Picture_HarkonnenSelected, Point(214,53), Point(surf->w,surf->h));

	Button_Harkonnen.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&SinglePlayerSkirmishMenu::OnSelectHarkonnen)));
	HouseChoiceContainer.AddWidget(	&Button_Harkonnen, Point(214,53), Point(surf->w,surf->h));

	UpdateChoice();


	// setup +/- Buttons to select misson

	Counter_Mission.SetCount(Mission);
	WindowWidget.AddWidget(	&Counter_Mission,
							Point(((screen->w/4)*3 + 160/4) - surf->w/2,screen->h/2 + 89),
							Counter_Mission.GetMinimumSize());



	surf = pGFXManager->getUIGraphic(UI_Plus);
	surfPressed = pGFXManager->getUIGraphic(UI_Plus_Pressed);
	Button_MissionPlus.SetSurfaces(surf,false,surfPressed,false);
	Button_MissionPlus.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&SinglePlayerSkirmishMenu::OnMissionIncrement)));
	WindowWidget.AddWidget(	&Button_MissionPlus,
							Point(((screen->w/4)*3 + 160/4) - surf->w/2 + 72,screen->h/2 + 96),
							Point(surf->w,surf->h));


	surf = pGFXManager->getUIGraphic(UI_Minus);
	surfPressed = pGFXManager->getUIGraphic(UI_Minus_Pressed);
	Button_MissionMinus.SetSurfaces(surf,false,surfPressed,false);
	Button_MissionMinus.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&SinglePlayerSkirmishMenu::OnMissionDecrement)));
	WindowWidget.AddWidget(	&Button_MissionMinus,
							Point(((screen->w/4)*3 + 160/4) - surf->w/2 + 72,screen->h/2 + 109),
							Point(surf->w,surf->h));

}

SinglePlayerSkirmishMenu::~SinglePlayerSkirmishMenu()
{
	;
}

void SinglePlayerSkirmishMenu::OnDifficulty()
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


void SinglePlayerSkirmishMenu::OnStart()
{
	GameInitSettings init((HOUSETYPE) HouseChoice, Mission, settings.General.ConcreteRequired, settings.General.FogOfWar);

    for(int houseID = 0; houseID < NUM_HOUSES; houseID++) {
	    if(houseID == HouseChoice) {
	        GameInitSettings::HouseInfo humanHouseInfo((HOUSETYPE) HouseChoice, 1);
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

void SinglePlayerSkirmishMenu::OnCancel()
{
	quit();
}

void SinglePlayerSkirmishMenu::OnSelectAtreides()
{
	HouseChoice = HOUSE_ATREIDES;
	UpdateChoice();
}

void SinglePlayerSkirmishMenu::OnSelectOrdos()
{
	HouseChoice = HOUSE_ORDOS;
	UpdateChoice();
}

void SinglePlayerSkirmishMenu::OnSelectHarkonnen()
{
	HouseChoice = HOUSE_HARKONNEN;
	UpdateChoice();
}

void SinglePlayerSkirmishMenu::OnMissionIncrement()
{
	Mission++;
	if(Mission > 22) {
		Mission = 1;
	}
	Counter_Mission.SetCount(Mission);
}

void SinglePlayerSkirmishMenu::OnMissionDecrement()
{
	Mission--;
	if(Mission < 1) {
		Mission = 22;
	}
	Counter_Mission.SetCount(Mission);
}

void SinglePlayerSkirmishMenu::UpdateDifficulty()
{
	if (settings.playerDifficulty == EASY) {
		Button_Difficulty.SetSurfaces(pGFXManager->getUIGraphic(UI_Dif_Easy), false);
	} else if (settings.playerDifficulty == MEDIUM)	{
		Button_Difficulty.SetSurfaces(pGFXManager->getUIGraphic(UI_Dif_Medium), false);
	} else {
		Button_Difficulty.SetSurfaces(pGFXManager->getUIGraphic(UI_Dif_Hard), false);
	}
}

void SinglePlayerSkirmishMenu::UpdateChoice()
{
	switch(HouseChoice) {
		case HOUSE_ATREIDES:
			Button_Ordos.SetEnabled(true);
			Button_Harkonnen.SetEnabled(true);
			Button_Atreides.SetEnabled(false);

			Picture_AtreidesSelected.SetVisible(true);
			Picture_OrdosSelected.SetVisible(false);
			Picture_HarkonnenSelected.SetVisible(false);
			break;

		case HOUSE_ORDOS:
			Button_Atreides.SetEnabled(true);
			Button_Harkonnen.SetEnabled(true);
			Button_Ordos.SetEnabled(false);

			Picture_AtreidesSelected.SetVisible(false);
			Picture_OrdosSelected.SetVisible(true);
			Picture_HarkonnenSelected.SetVisible(false);
			break;

		case HOUSE_HARKONNEN:
		default:
			Button_Atreides.SetEnabled(true);
			Button_Ordos.SetEnabled(true);
			Button_Harkonnen.SetEnabled(false);

			Picture_AtreidesSelected.SetVisible(false);
			Picture_OrdosSelected.SetVisible(false);
			Picture_HarkonnenSelected.SetVisible(true);
			break;
	}
}
