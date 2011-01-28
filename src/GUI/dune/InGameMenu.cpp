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

#include <GUI/dune/InGameMenu.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <FileClasses/TextManager.h>
#include <misc/fnkdat.h>
#include <Game.h>
#include <GameInitSettings.h>

#include <GUI/MessageBox.h>
#include <GUI/dune/InGameSettingsMenu.h>
#include <GUI/dune/LoadSaveWindow.h>


InGameMenu::InGameMenu(int color) : Window(0,0,0,0)
{
    this->color = color;

	// set up window
	SDL_Surface *surf;
	surf = pGFXManager->getUIGraphic(UI_GameMenu);

	SetBackground(surf,false);

	int xpos = std::max(0,(screen->w - surf->w)/2);
	int ypos = std::max(0,(screen->h - surf->h)/2);

	SetCurrentPosition(xpos,ypos,surf->w,surf->h);

	SetWindowWidget(&Main_HBox);

	Main_HBox.AddWidget(HSpacer::Create(22));
	Main_HBox.AddWidget(&Main_VBox);
	Main_HBox.AddWidget(HSpacer::Create(22));


	Main_VBox.AddWidget(VSpacer::Create(34));

	Button_Resume.SetText(pTextManager->getLocalized("Resume Game"));
	Button_Resume.SetTextColor(color);
	Button_Resume.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&InGameMenu::OnResume)));
	Main_VBox.AddWidget(&Button_Resume);

	Main_VBox.AddWidget(VSpacer::Create(3));

	Button_GameSettings.SetText(pTextManager->getLocalized("Game Settings"));
	Button_GameSettings.SetTextColor(color);
	Button_GameSettings.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&InGameMenu::OnSettings)));
	Main_VBox.AddWidget(&Button_GameSettings);

	Main_VBox.AddWidget(VSpacer::Create(3));

	Button_SaveGame.SetText(pTextManager->getLocalized("Save Game"));
	Button_SaveGame.SetTextColor(color);
	Button_SaveGame.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&InGameMenu::OnSave)));
	Main_VBox.AddWidget(&Button_SaveGame);

	Main_VBox.AddWidget(VSpacer::Create(3));

	Button_LoadGame.SetText(pTextManager->getLocalized("Load Game"));
	Button_LoadGame.SetTextColor(color);
	Button_LoadGame.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&InGameMenu::OnLoad)));
	Main_VBox.AddWidget(&Button_LoadGame);

	Main_VBox.AddWidget(VSpacer::Create(3));

	Button_RestartGame.SetText(pTextManager->getLocalized("Restart Game"));
	Button_RestartGame.SetTextColor(color);
	Button_RestartGame.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&InGameMenu::OnRestart)));
	Main_VBox.AddWidget(&Button_RestartGame);

	Main_VBox.AddWidget(VSpacer::Create(3));

	Button_Quit.SetText(pTextManager->getLocalized("Quit to Menu"));
	Button_Quit.SetTextColor(color);
	Button_Quit.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&InGameMenu::OnQuit)));
	Main_VBox.AddWidget(&Button_Quit);

	Main_VBox.AddWidget(VSpacer::Create(6));
}

InGameMenu::~InGameMenu()
{
	;
}

bool InGameMenu::HandleKeyPress(SDL_KeyboardEvent& key) {
	switch( key.keysym.sym ) {
		case SDLK_ESCAPE:
		{
			currentGame->ResumeGame();
		} break;

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

void InGameMenu::CloseChildWindow() {
	LoadSaveWindow* pLoadSaveWindow = dynamic_cast<LoadSaveWindow*>(pChildWindow);
	if(pLoadSaveWindow != NULL) {
        std::string FileName = pLoadSaveWindow->GetFilename();
		bool bSave = pLoadSaveWindow->SaveWindow();

		Window::CloseChildWindow();

        if(FileName != "") {
            if(bSave == false) {
                // load window
                try {
                    currentGame->setNextGameInitSettings(GameInitSettings(FileName));
                } catch (std::exception& e) {
                    // most probably the savegame file is not valid or from a different dune legacy version
                    OpenWindow(MessageBox::Create(e.what()));
                }

                currentGame->ResumeGame();
                currentGame->quit_Game();

            } else {
                // save window
                currentGame->saveGame(FileName);

                currentGame->ResumeGame();
            }
        }
	} else {
	    Window::CloseChildWindow();
	}
}

void InGameMenu::OnResume()
{
	currentGame->ResumeGame();
}

void InGameMenu::OnSettings()
{
	OpenWindow(InGameSettingsMenu::Create());
}

void InGameMenu::OnSave()
{
	char tmp[FILENAME_MAX];
	fnkdat("save/", tmp, FILENAME_MAX, FNKDAT_USER | FNKDAT_CREAT);
	std::string savepath(tmp);
	OpenWindow(LoadSaveWindow::Create(true, pTextManager->getLocalized("Save Game"), savepath, "dls", color));
}

void InGameMenu::OnLoad()
{
	char tmp[FILENAME_MAX];
	fnkdat("save/", tmp, FILENAME_MAX, FNKDAT_USER | FNKDAT_CREAT);
	std::string savepath(tmp);
	OpenWindow(LoadSaveWindow::Create(false, pTextManager->getLocalized("Load Game"), savepath, "dls", color));
}

void InGameMenu::OnRestart()
{
	// set new current init settings as init info for next game
	currentGame->setNextGameInitSettings(currentGame->getGameInitSettings());

	// quit current game
	currentGame->ResumeGame();
	currentGame->quit_Game();
}

void InGameMenu::OnQuit()
{
	currentGame->quit_Game();
}
