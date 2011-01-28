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

#ifndef CUSTOMGAMEMENU_H
#define CUSTOMGAMEMENU_H

#include <GUI/StaticContainer.h>
#include <GUI/VBox.h>
#include <GUI/HBox.h>
#include <GUI/Label.h>
#include <GUI/TextButton.h>
#include <GUI/ListBox.h>
#include <GUI/PictureLabel.h>
#include <GUI/Checkbox.h>

#include <string>

#include "MenuClass.h"

class CustomGameMenu : public MenuClass
{
public:
	CustomGameMenu();
	virtual ~CustomGameMenu();

private:
    void OnNext();
	void OnCancel();
	void OnMapTypeChange(int buttonID);
	void OnMapListSelectionChange();


	std::string currentMapDirectory;

    StaticContainer WindowWidget;
    VBox			MainVBox;

    Label           CaptionLabel;

    HBox            MainHBox;

    // left VBox with map list and map options
    VBox            LeftVBox;
    HBox            MapTypeButtonsHBox;
    TextButton      SingleplayerMapsButton;
    TextButton      SingleplayerUserMapsButton;
    TextButton      MultiplayerMapsButton;
    TextButton      MultiplayerUserMapsButton;
    TextButton      DummyButton;
    ListBox         MapList;
    HBox            OptionsHBox;
    Checkbox        RequireConcrete;
    Checkbox        FogOfWar;
    Checkbox        MultiplePlayersPerHouse;

    // right VBox with mini map
    VBox            RightVBox;
    PictureLabel    Minimap;
    HBox            MapPropertiesHBox;
    VBox            MapPropertyNamesVBox;
    VBox            MapPropertyValuesVBox;
    Label           MapProperty_Size;
    Label           MapProperty_Players;
    Label           MapProperty_Authors;
    Label           MapProperty_License;

    // bottom row of buttons
    HBox            ButtonHBox;
    TextButton		Button_Next;
	TextButton		Button_Cancel;

};

#endif //CUSTOMGAMEMENU_H
