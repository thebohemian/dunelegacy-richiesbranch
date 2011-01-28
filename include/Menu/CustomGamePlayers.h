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

#ifndef CUSTOMGAMEPLAYERS_H
#define CUSTOMGAMEPLAYERS_H

#include <GameInitSettings.h>

#include <GUI/StaticContainer.h>
#include <GUI/VBox.h>
#include <GUI/HBox.h>
#include <GUI/Label.h>
#include <GUI/DropDownBox.h>
#include <GUI/TextButton.h>
#include <GUI/ListBox.h>
#include <GUI/PictureLabel.h>
#include <GUI/Checkbox.h>

#include <DataTypes.h>

#include <string>
#include <list>

#include "MenuClass.h"

class CustomGamePlayers : public MenuClass
{
public:
	CustomGamePlayers(const GameInitSettings& newGameInitSettings, bool multiplePlayersPerHouse);
	virtual ~CustomGamePlayers();

private:
    void setMapInfo(std::string filename);

    std::string extractMapname(std::string filename);

    void OnChangeHousesDropDownBoxes(int houseInfoNum = -1);
    void OnChangePlayerDropDownBoxes(int boxnum);
    void OnClickPlayerDropDownBox(int boxnum);
    void addToHouseDropDown(DropDownBox& houseDropDownBox, int house, bool bSelect = false);
    void removeFromHouseDropDown(DropDownBox& houseDropDownBox, int house);

    bool isBoundedHouseOnMap(HOUSETYPE houseID);

    void checkPlayerBoxes();
    bool addPlayerToHouseInfo(GameInitSettings::HouseInfo& houseInfo, int player);

    void OnNext();
	void OnCancel();

    GameInitSettings    gameInitSettings;
    bool                bMultiplePlayersPerHouse;

    StaticContainer WindowWidget;
    VBox			MainVBox;

    Label           CaptionLabel;

    HBox            MainHBox;

    // left VBox with player list and map options
    VBox            LeftVBox;
    HBox            LeftVBoxTitle;

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
	TextButton		Button_Back;

	class HouseInfo {
    public:
        VBox        HouseInfoVBox;
        HBox        HouseHBox;
        Label       HouseLabel;
        DropDownBox HouseDropDown;
        DropDownBox TeamDropDown;
        HBox        PlayerHBox;
        Label       Player1Label;
        DropDownBox Player1DropDown;
        Label       Player2Label;
        DropDownBox Player2DropDown;
	};


    HouseInfo             houseInfo[NUM_HOUSES];
    int                   numHouses;
    std::list<HOUSETYPE>  boundHousesOnMap;
};

#endif //CUSTOMGAMEPLAYERS_H
