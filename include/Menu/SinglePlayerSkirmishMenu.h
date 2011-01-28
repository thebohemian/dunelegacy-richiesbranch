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

#ifndef SINGLEPLAYERSKIRMISHMENU_H
#define SINGLEPLAYERSKIRMISHMENU_H

#include "MenuClass.h"

#include <GUI/StaticContainer.h>
#include <GUI/VBox.h>
#include <GUI/TextButton.h>
#include <GUI/PictureButton.h>
#include <GUI/InvisibleButton.h>
#include <GUI/Spacer.h>
#include <GUI/PictureLabel.h>
#include <GUI/dune/DigitsCounter.h>

#include <misc/string_util.h>

class SinglePlayerSkirmishMenu : public MenuClass
{
public:
	SinglePlayerSkirmishMenu();
	virtual ~SinglePlayerSkirmishMenu();

private:

	void OnDifficulty();
	void OnStart();
	void OnCancel();

	void OnSelectAtreides();
	void OnSelectOrdos();
	void OnSelectHarkonnen();
	void OnMissionIncrement();
	void OnMissionDecrement();

	void UpdateDifficulty();
	void UpdateChoice();

	PictureLabel	Picture_Difficulty;
	PictureButton	Button_Difficulty;

	InvisibleButton	Button_Atreides;
	PictureLabel	Picture_AtreidesSelected;
	InvisibleButton	Button_Harkonnen;
	PictureLabel	Picture_HarkonnenSelected;
	InvisibleButton	Button_Ordos;
	PictureLabel	Picture_OrdosSelected;

	PictureButton	Button_MissionPlus;
	PictureButton	Button_MissionMinus;
	DigitsCounter	Counter_Mission;

	StaticContainer	WindowWidget;
	StaticContainer	HouseChoiceContainer;
	VBox			MenuButtons;

	TextButton		Button_Start;
	TextButton		Button_Back;

	PictureLabel	HeraldPicture;
	PictureLabel	DuneLegacy;
	PictureLabel	ButtonBorder;

	int HouseChoice;
	int Mission;
};

#endif //SINGLEPLAYERSKIRMISHMENU_H
