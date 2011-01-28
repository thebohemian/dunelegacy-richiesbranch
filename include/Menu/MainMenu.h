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

#ifndef MAINMENU_H
#define MAINMENU_H

#include "MenuClass.h"
#include <GUI/StaticContainer.h>
#include <GUI/VBox.h>
#include <GUI/TextButton.h>
#include <GUI/Spacer.h>
#include <GUI/PictureLabel.h>

class MainMenu : public MenuClass {
public:
	MainMenu();
	virtual ~MainMenu();

    virtual int showMenu();

private:
	void OnSinglePlayer();
	void OnMultiPlayer();
	void OnMapEditor();
	void OnOptions();
	void OnAbout();
	void OnQuit();

	StaticContainer	WindowWidget;
	VBox			MenuButtons;

	TextButton		Button_SinglePlayer;
	TextButton		Button_MultiPlayer;
	TextButton		Button_MapEditor;
	TextButton		Button_Options;
	TextButton		Button_About;
	TextButton		Button_Quit;

	PictureLabel	PlanetPicture;
	PictureLabel	DuneLegacy;
	PictureLabel	ButtonBorder;
};

#endif // MAINMENU_H
