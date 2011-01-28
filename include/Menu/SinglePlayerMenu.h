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

#ifndef SINGLEPLAYERMENU_H
#define SINGLEPLAYERMENU_H

#include "MenuClass.h"
#include <GUI/StaticContainer.h>
#include <GUI/VBox.h>
#include <GUI/TextButton.h>
#include <GUI/Spacer.h>
#include <GUI/PictureButton.h>
#include <GUI/PictureLabel.h>

class SinglePlayerMenu : public MenuClass
{
public:
	SinglePlayerMenu();
	virtual ~SinglePlayerMenu();

	/**
		Closes the child window.
	*/
	virtual void CloseChildWindow();

private:

	void OnCampaign();
	void OnCustom();
	void OnSkirmish();
	void OnLoadSavegame();
	void OnLoadReplay();
	void OnCancel();

	void OnDifficulty();
	void UpdateDifficulty();

	StaticContainer	WindowWidget;
	VBox			MenuButtons;

	TextButton	Button_Campaign;
	TextButton	Button_Custom;
	TextButton	Button_Skirmish;
	TextButton	Button_LoadSavegame;
	TextButton	Button_LoadReplay;
	TextButton	Button_Cancel;

	PictureLabel	Picture_Difficulty;
	PictureButton	Button_Difficulty;

	PictureLabel	PlanetPicture;
	PictureLabel	DuneLegacy;
	PictureLabel	ButtonBorder;
};

#endif //SINGLEPLAYERMENU_H
