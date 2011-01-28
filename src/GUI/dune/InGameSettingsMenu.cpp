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

#include <GUI/dune/InGameSettingsMenu.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <FileClasses/TextManager.h>
#include <Game.h>
#include <House.h>
#include <SoundPlayer.h>

#include <GUI/Spacer.h>


InGameSettingsMenu::InGameSettingsMenu() : Window(0,0,0,0)
{
    int houseID = pLocalHouse->getHouseID();
	int color = houseColor[houseID];

	// set up window
	SDL_Surface *surf,*surfPressed;
	surf = pGFXManager->getUIGraphic(UI_OptionsMenu, houseID);

	SetBackground(surf,false);

	int xpos = std::max(0,(screen->w - surf->w)/2);
	int ypos = std::max(0,(screen->h - surf->h)/2);

	SetCurrentPosition(xpos,ypos,surf->w,surf->h);

	SetWindowWidget(&WindowWidget);

	// Game speed
    surf = pGFXManager->getUIGraphic(UI_Minus, houseID);
	surfPressed = pGFXManager->getUIGraphic(UI_Minus_Pressed, houseID);
    GameSpeedMinus.SetSurfaces(surf,false,surfPressed,false);
	GameSpeedMinus.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&InGameSettingsMenu::OnGameSpeedMinus)));
	WindowWidget.AddWidget(&GameSpeedMinus, Point(5,52), GameSpeedMinus.GetSize());

    GameSpeedBar.SetColor(color + 2);
	WindowWidget.AddWidget(&GameSpeedBar, Point(23,56), Point(146,6));

	surf = pGFXManager->getUIGraphic(UI_Plus, houseID);
	surfPressed = pGFXManager->getUIGraphic(UI_Plus_Pressed, houseID);
	GameSpeedPlus.SetSurfaces(surf,false,surfPressed,false);
	GameSpeedPlus.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&InGameSettingsMenu::OnGameSpeedPlus)));
	WindowWidget.AddWidget(&GameSpeedPlus, Point(172,52), GameSpeedPlus.GetSize());

	// Volume
	surf = pGFXManager->getUIGraphic(UI_Minus, houseID);
	surfPressed = pGFXManager->getUIGraphic(UI_Minus_Pressed, houseID);
	VolumeMinus.SetSurfaces(surf,false,surfPressed,false);
	VolumeMinus.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&InGameSettingsMenu::OnVolumeMinus)));
	WindowWidget.AddWidget(&VolumeMinus, Point(5,83), VolumeMinus.GetSize());

	VolumeBar.SetColor(color + 2);
	WindowWidget.AddWidget(&VolumeBar, Point(23,87), Point(146,6));

	surf = pGFXManager->getUIGraphic(UI_Plus, houseID);
	surfPressed = pGFXManager->getUIGraphic(UI_Plus_Pressed, houseID);
	VolumePlus.SetSurfaces(surf,false,surfPressed,false);
	VolumePlus.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&InGameSettingsMenu::OnVolumePlus)));
	WindowWidget.AddWidget(&VolumePlus, Point(172,83), VolumePlus.GetSize());


	// Scroll speed
	surf = pGFXManager->getUIGraphic(UI_Minus, houseID);
	surfPressed = pGFXManager->getUIGraphic(UI_Minus_Pressed, houseID);
	ScrollSpeedMinus.SetSurfaces(surf,false,surfPressed,false);
	ScrollSpeedMinus.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&InGameSettingsMenu::OnScrollSpeedMinus)));
	ScrollSpeedMinus.SetEnabled(false);
	WindowWidget.AddWidget(&ScrollSpeedMinus, Point(5,114), ScrollSpeedMinus.GetSize());

	ScrollSpeedBar.SetColor(color + 2);
	WindowWidget.AddWidget(&ScrollSpeedBar, Point(23,118), Point(146,6));

	surf = pGFXManager->getUIGraphic(UI_Plus, houseID);
	surfPressed = pGFXManager->getUIGraphic(UI_Plus_Pressed, houseID);
	ScrollSpeedPlus.SetSurfaces(surf,false,surfPressed,false);
	ScrollSpeedPlus.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&InGameSettingsMenu::OnScrollSpeedPlus)));
	ScrollSpeedPlus.SetEnabled(false);
	WindowWidget.AddWidget(&ScrollSpeedPlus, Point(172,114), ScrollSpeedPlus.GetSize());


    // buttons
	Button_OK.SetText(pTextManager->getLocalized("OK"));
	Button_OK.SetTextColor(color+3);
	Button_OK.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&InGameSettingsMenu::OnOK)));
	WindowWidget.AddWidget(&Button_OK, Point(12,134), Point(79,15));

	Button_Cancel.SetText(pTextManager->getLocalized("Cancel"));
	Button_Cancel.SetTextColor(color+3);
	Button_Cancel.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&InGameSettingsMenu::OnCancel)));
	WindowWidget.AddWidget(&Button_Cancel, Point(101,134), Point(79,15));

	Update();
}

InGameSettingsMenu::~InGameSettingsMenu()
{
	;
}

void InGameSettingsMenu::Update()
{
	newGamespeed = currentGame->gamespeed;
	GameSpeedBar.SetProgress(100 - ((newGamespeed-GAMESPEED_MIN)*100)/(GAMESPEED_MAX - GAMESPEED_MIN));

	Volume = soundPlayer->GetSfxVolume();
	VolumeBar.SetProgress((100*Volume)/MIX_MAX_VOLUME);

	ScrollSpeedBar.SetProgress(50.0);
}

bool InGameSettingsMenu::HandleKeyPress(SDL_KeyboardEvent& key) {
	switch( key.keysym.sym ) {
		case SDLK_RETURN:
            if(SDL_GetModState() & KMOD_ALT) {
                SDL_WM_ToggleFullScreen(screen);
            }
            break;

        case SDLK_TAB:
            if(SDL_GetModState() & KMOD_ALT) {
                SDL_WM_IconifyWindow();
            }
            break;

		default:
			break;
	}

	return Window::HandleKeyPress(key);
}

void InGameSettingsMenu::OnCancel()
{
	Window* pParentWindow = dynamic_cast<Window*>(GetParent());
	if(pParentWindow != NULL) {
		pParentWindow->CloseChildWindow();
	}
}

void InGameSettingsMenu::OnOK()
{
	currentGame->gamespeed = newGamespeed;
	soundPlayer->SetSfxVolume(Volume);

	Window* pParentWindow = dynamic_cast<Window*>(GetParent());
	if(pParentWindow != NULL) {
		pParentWindow->CloseChildWindow();
	}
}

void InGameSettingsMenu::OnGameSpeedPlus()
{
	if(newGamespeed > GAMESPEED_MIN)
		newGamespeed -= 1;

	GameSpeedBar.SetProgress(100 - ((newGamespeed-GAMESPEED_MIN)*100)/(GAMESPEED_MAX - GAMESPEED_MIN));
}

void InGameSettingsMenu::OnGameSpeedMinus()
{
	if(newGamespeed < GAMESPEED_MAX)
		newGamespeed += 1;

	GameSpeedBar.SetProgress(100 - ((newGamespeed-GAMESPEED_MIN)*100)/(GAMESPEED_MAX - GAMESPEED_MIN));
}

void InGameSettingsMenu::OnVolumePlus()
{
	if(Volume <= MIX_MAX_VOLUME - 4) {
		Volume += 4;
		VolumeBar.SetProgress((100*Volume)/MIX_MAX_VOLUME);
	}
}

void InGameSettingsMenu::OnVolumeMinus()
{
	if(Volume >= 4) {
		Volume -= 4;
		VolumeBar.SetProgress((100*Volume)/MIX_MAX_VOLUME);
	}
}

void InGameSettingsMenu::OnScrollSpeedPlus()
{

}

void InGameSettingsMenu::OnScrollSpeedMinus()
{

}
