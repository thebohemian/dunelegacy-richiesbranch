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

#include <Menu/MainMenu.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <FileClasses/TextManager.h>
#include <FileClasses/music/MusicPlayer.h>

#include <MapEditor.h>

#include <Menu/SinglePlayerMenu.h>
#include <Menu/OptionsMenu.h>
#include <Menu/AboutMenu.h>

MainMenu::MainMenu() : MenuClass()
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

	Button_SinglePlayer.SetText(pTextManager->getLocalized("SINGLE PLAYER"));
	Button_SinglePlayer.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&MainMenu::OnSinglePlayer)));
	MenuButtons.AddWidget(&Button_SinglePlayer);
	Button_SinglePlayer.SetActive();

	MenuButtons.AddWidget(VSpacer::Create(3));

    MenuButtons.AddWidget(VSpacer::Create(18));
/*	Button_MultiPlayer.SetText("MULTIPLAYER");
	Button_MultiPlayer.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&MainMenu::OnMultiPlayer)));
	Button_MultiPlayer.SetEnabled(false);
	MenuButtons.AddWidget(&Button_MultiPlayer);
*/
	MenuButtons.AddWidget(VSpacer::Create(3));

    MenuButtons.AddWidget(VSpacer::Create(12));
/*	Button_MapEditor.SetText("MAP EDITOR");
	Button_MapEditor.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&MainMenu::OnMapEditor)));
	MenuButtons.AddWidget(&Button_MapEditor);
*/
	MenuButtons.AddWidget(VSpacer::Create(3));

	Button_Options.SetText(pTextManager->getLocalized("OPTIONS"));
	Button_Options.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&MainMenu::OnOptions)));
	MenuButtons.AddWidget(&Button_Options);

	MenuButtons.AddWidget(VSpacer::Create(3));

	Button_About.SetText(pTextManager->getLocalized("ABOUT"));
	Button_About.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&MainMenu::OnAbout)));
	MenuButtons.AddWidget(&Button_About);

	MenuButtons.AddWidget(VSpacer::Create(3));

	Button_Quit.SetText(pTextManager->getLocalized("QUIT"));
	Button_Quit.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&MainMenu::OnQuit)));
	MenuButtons.AddWidget(&Button_Quit);
}

MainMenu::~MainMenu() {
}

int MainMenu::showMenu()
{
    musicPlayer->changeMusic(MUSIC_MENU);

    return MenuClass::showMenu();
}

void MainMenu::OnSinglePlayer() {
	SinglePlayerMenu* mySinglePlayer = new SinglePlayerMenu();
	if(mySinglePlayer == NULL) {
		perror("MainMenu::OnSinglePlayer()");
		exit(EXIT_FAILURE);
	}
	mySinglePlayer->showMenu();
	delete mySinglePlayer;
}

void MainMenu::OnMultiPlayer() {
	; //You should first enable this Button :)
}

void MainMenu::OnMapEditor() {
	MapEditor *pMapEditor;
	if((pMapEditor = new MapEditor()) == NULL) {
		perror("MainMenu::OnMapEditor()");
		exit(EXIT_FAILURE);
	}

	pMapEditor->RunEditor();
	delete pMapEditor;
}


void MainMenu::OnOptions() {
	OptionsMenu* myOptions = new OptionsMenu();
	if(myOptions == NULL) {
		perror("MainMenu::OnOptions()");
		exit(EXIT_FAILURE);
	}
	int ret = myOptions->showMenu();
	delete myOptions;

	if(ret == 1) {
		quit(1);
	}
}

void MainMenu::OnAbout() {
	AboutMenu* myAbout = new AboutMenu();
	if(myAbout == NULL) {
		perror("MainMenu::OnAbout()");
		exit(EXIT_FAILURE);
	}
	myAbout->showMenu();
	delete myAbout;
}

void MainMenu::OnQuit() {
	quit();
}
