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

#include <Menu/CustomGamePlayers.h>

#include <FileClasses/GFXManager.h>
#include <FileClasses/TextManager.h>
#include <FileClasses/INIFile.h>

#include <GUI/Spacer.h>
#include <GUI/GUIStyle.h>
#include <GUI/MessageBox.h>

#include <misc/fnkdat.h>
#include <misc/FileSystem.h>
#include <misc/draw_util.h>
#include <misc/string_util.h>

#include <GameINILoader.h>

#include <sand.h>
#include <globals.h>


#define PLAYER_OPEN         0
#define PLAYER_CLOSED       1
#define PLAYER_HUMAN        2
#define PLAYER_AI_EASY      -1
#define PLAYER_AI_MEDIUM    -2
#define PLAYER_AI_DIFFICULT -3


CustomGamePlayers::CustomGamePlayers(const GameInitSettings& newGameInitSettings, bool multiplePlayersPerHouse)
 : MenuClass(), gameInitSettings(newGameInitSettings), bMultiplePlayersPerHouse(multiplePlayersPerHouse) {

	// set up window
	SDL_Surface *surf;
	surf = pGFXManager->getUIGraphic(UI_MenuBackground);

	SetBackground(surf,false);
	Resize(surf->w,surf->h);

	SetWindowWidget(&WindowWidget);

	WindowWidget.AddWidget(&MainVBox, Point(24,23),	Point(screen->w - 48, screen->h - 46));

    CaptionLabel.SetText(extractMapname(gameInitSettings.getFilename()));
    CaptionLabel.SetAlignment(Alignment_HCenter);
    MainVBox.AddWidget(&CaptionLabel, 24);
    MainVBox.AddWidget(VSpacer::Create(24));


    MainVBox.AddWidget(&MainHBox, 1.0);

    MainHBox.AddWidget(Spacer::Create(), 0.5);
    MainHBox.AddWidget(&LeftVBox, 0.0);

    MainHBox.AddWidget(Spacer::Create(), 0.5);

    MainHBox.AddWidget(HSpacer::Create(8));

    MainHBox.AddWidget(&RightVBox, 180);
    Minimap.SetSurface( GUIStyle::GetInstance().CreateButtonSurface(130,130,pTextManager->getLocalized("Choose map"), true, false), true);
    RightVBox.AddWidget(&Minimap);

    setMapInfo(gameInitSettings.getFilename());

    RightVBox.AddWidget(VSpacer::Create(10));
    RightVBox.AddWidget(&MapPropertiesHBox, 0.01);
    MapPropertiesHBox.AddWidget(&MapPropertyNamesVBox, 75);
    MapPropertiesHBox.AddWidget(&MapPropertyValuesVBox, 105);
    MapPropertyNamesVBox.AddWidget(Label::Create(pTextManager->getLocalized("Size") + ":"));
    MapPropertyValuesVBox.AddWidget(&MapProperty_Size);
    MapPropertyNamesVBox.AddWidget(Label::Create(pTextManager->getLocalized("Players") + ":"));
    MapPropertyValuesVBox.AddWidget(&MapProperty_Players);
    MapPropertyNamesVBox.AddWidget(Label::Create(pTextManager->getLocalized("Author") + ":"));
    MapPropertyValuesVBox.AddWidget(&MapProperty_Authors);
    MapPropertyNamesVBox.AddWidget(Label::Create(pTextManager->getLocalized("License") + ":"));
    MapPropertyValuesVBox.AddWidget(&MapProperty_License);
    RightVBox.AddWidget(Spacer::Create());

    MainVBox.AddWidget(VSpacer::Create(20));
	MainVBox.AddWidget(&ButtonHBox, 24);

    ButtonHBox.AddWidget(HSpacer::Create(70));
    Button_Back.SetText(pTextManager->getLocalized("Back"));
	Button_Back.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&CustomGamePlayers::OnCancel)));
	ButtonHBox.AddWidget(&Button_Back, 0.2);

	ButtonHBox.AddWidget(Spacer::Create(), 0.6);

    Button_Next.SetText(pTextManager->getLocalized("Next"));
	Button_Next.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&CustomGamePlayers::OnNext)));
	ButtonHBox.AddWidget(&Button_Next, 0.2);
	ButtonHBox.AddWidget(HSpacer::Create(90));

    std::list<HOUSETYPE>  tmpBoundHousesOnMap = boundHousesOnMap;

    LeftVBox.AddWidget(VSpacer::Create(8));

    for(int i=0;i<NUM_HOUSES;i++) {
        HouseInfo& curHouseInfo = houseInfo[i];

        // set up header row with Label "House", DropDown for house selection and DropDown for team selection
        curHouseInfo.HouseLabel.SetText(pTextManager->getLocalized("House"));
        curHouseInfo.HouseHBox.AddWidget(&curHouseInfo.HouseLabel, 60);

        if(tmpBoundHousesOnMap.empty() == false) {
            HOUSETYPE housetype = tmpBoundHousesOnMap.front();
            tmpBoundHousesOnMap.pop_front();

            addToHouseDropDown(curHouseInfo.HouseDropDown, housetype, true);
        } else {
            addToHouseDropDown(curHouseInfo.HouseDropDown, HOUSE_INVALID, true);
        }

        curHouseInfo.HouseDropDown.SetOnSelectionChangeMethod(MethodPointer(this,(WidgetCallbackWithIntParameter) (&CustomGamePlayers::OnChangeHousesDropDownBoxes), i));
        curHouseInfo.HouseHBox.AddWidget(&curHouseInfo.HouseDropDown, 100);

        for(int team = 0 ; team<numHouses ; team++) {
            curHouseInfo.TeamDropDown.AddEntry(pTextManager->getLocalized("Team") + " " + stringify(team+1), team+1);
        }
        curHouseInfo.TeamDropDown.SetSelectedItem(i);
        curHouseInfo.HouseHBox.AddWidget(HSpacer::Create(10));
        curHouseInfo.HouseHBox.AddWidget(&curHouseInfo.TeamDropDown, 80);

        curHouseInfo.HouseInfoVBox.AddWidget(&curHouseInfo.HouseHBox);

        curHouseInfo.PlayerHBox.AddWidget(HSpacer::Create(25));

        // add 1. player
        curHouseInfo.Player1Label.SetText(pTextManager->getLocalized("Player") + (bMultiplePlayersPerHouse ? " 1" : ""));
        curHouseInfo.Player1Label.SetTextFont(FONT_STD10);
        curHouseInfo.PlayerHBox.AddWidget(&curHouseInfo.Player1Label, 70);

        if(i==0) {
            curHouseInfo.Player1DropDown.AddEntry(settings.General.PlayerName, PLAYER_HUMAN);
            curHouseInfo.Player1DropDown.SetSelectedItem(0);
        } else {
            curHouseInfo.Player1DropDown.AddEntry(pTextManager->getLocalized("open"), PLAYER_OPEN);
            curHouseInfo.Player1DropDown.SetSelectedItem(0);
            curHouseInfo.Player1DropDown.AddEntry(pTextManager->getLocalized("closed"), PLAYER_CLOSED);
            curHouseInfo.Player1DropDown.AddEntry("AI (easy)", PLAYER_AI_EASY);
            curHouseInfo.Player1DropDown.AddEntry("AI (medium)", PLAYER_AI_MEDIUM);
            curHouseInfo.Player1DropDown.AddEntry("AI (difficult)", PLAYER_AI_DIFFICULT);
        }
        curHouseInfo.Player1DropDown.SetOnSelectionChangeMethod(MethodPointer(this,(WidgetCallbackWithIntParameter) (&CustomGamePlayers::OnChangePlayerDropDownBoxes), 2*i));
        curHouseInfo.Player1DropDown.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithIntParameter) (&CustomGamePlayers::OnClickPlayerDropDownBox), 2*i));
        curHouseInfo.PlayerHBox.AddWidget(&curHouseInfo.Player1DropDown, 100);

        curHouseInfo.PlayerHBox.AddWidget(HSpacer::Create(10));

        // add 2. player
        curHouseInfo.Player2Label.SetText(pTextManager->getLocalized("Player") + " 2");
        curHouseInfo.Player2Label.SetTextFont(FONT_STD10);
        curHouseInfo.PlayerHBox.AddWidget(&curHouseInfo.Player2Label, 70);
        curHouseInfo.Player2DropDown.AddEntry(pTextManager->getLocalized("open"), PLAYER_OPEN);
        curHouseInfo.Player2DropDown.SetSelectedItem(0);
        curHouseInfo.Player2DropDown.AddEntry(pTextManager->getLocalized("closed"), PLAYER_CLOSED);
        curHouseInfo.Player2DropDown.AddEntry("AI (easy)", PLAYER_AI_EASY);
        curHouseInfo.Player2DropDown.AddEntry("AI (medium)", PLAYER_AI_MEDIUM);
        curHouseInfo.Player2DropDown.AddEntry("AI (difficult)", PLAYER_AI_DIFFICULT);
        curHouseInfo.Player2DropDown.SetOnSelectionChangeMethod(MethodPointer(this,(WidgetCallbackWithIntParameter) (&CustomGamePlayers::OnChangePlayerDropDownBoxes), 2*i + 1));
        curHouseInfo.Player2DropDown.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithIntParameter) (&CustomGamePlayers::OnClickPlayerDropDownBox), 2*i + 1));
        curHouseInfo.PlayerHBox.AddWidget(&curHouseInfo.Player2DropDown, 100);

        curHouseInfo.HouseInfoVBox.AddWidget(&curHouseInfo.PlayerHBox);

        LeftVBox.AddWidget(&curHouseInfo.HouseInfoVBox, 0.01);

        LeftVBox.AddWidget(VSpacer::Create(8));
        LeftVBox.AddWidget(Spacer::Create(), 0.07);

        if(i >= numHouses) {
            curHouseInfo.HouseInfoVBox.SetEnabled(false);
            curHouseInfo.HouseInfoVBox.SetVisible(false);
        }
    }

    OnChangeHousesDropDownBoxes();

    checkPlayerBoxes();

	LeftVBox.AddWidget(Spacer::Create(), 0.58);
}

CustomGamePlayers::~CustomGamePlayers()
{
}

void CustomGamePlayers::OnNext()
{
    // check if we have at least two houses on the map and if we have more than one team
    int numUsedHouses = 0;
    int numTeams = 0;
    for(int i=0;i<NUM_HOUSES;i++) {
        HouseInfo& curHouseInfo = houseInfo[i];

        int player1 = curHouseInfo.Player1DropDown.GetEntryData(curHouseInfo.Player1DropDown.GetSelectedIndex());
        int player2 = curHouseInfo.Player2DropDown.GetEntryData(curHouseInfo.Player2DropDown.GetSelectedIndex());

        if((player1 != PLAYER_OPEN && player1 != PLAYER_CLOSED) || (player2 != PLAYER_OPEN && player2 != PLAYER_CLOSED)) {
            numUsedHouses++;

            int currentTeam = curHouseInfo.TeamDropDown.GetEntryData(curHouseInfo.TeamDropDown.GetSelectedIndex());
            bool bTeamFound = false;
            for(int j=0;j<i;j++) {
                int team = houseInfo[j].TeamDropDown.GetEntryData(houseInfo[j].TeamDropDown.GetSelectedIndex());
                if(currentTeam == team) {
                    bTeamFound = true;
                }
            }

            if(bTeamFound == false) {
                numTeams++;
            }
        }
    }

    if(numUsedHouses < 2) {
        // No game possible with only 1 house
        OpenWindow(MessageBox::Create(pTextManager->getLocalized("At least 2 houses must be controlled\nby a human player or an AI player!")));
    } else if(numTeams < 2) {
        // No game possible with only 1 team
        OpenWindow(MessageBox::Create(pTextManager->getLocalized("There must be at least two different teams!")));
    } else {
        // start game

        for(int i=0;i<NUM_HOUSES;i++) {
            HouseInfo& curHouseInfo = houseInfo[i];

            int houseID = curHouseInfo.HouseDropDown.GetEntryData(curHouseInfo.HouseDropDown.GetSelectedIndex());
            int team = curHouseInfo.TeamDropDown.GetEntryData(curHouseInfo.TeamDropDown.GetSelectedIndex());
            int player1 = curHouseInfo.Player1DropDown.GetEntryData(curHouseInfo.Player1DropDown.GetSelectedIndex());
            int player2 = curHouseInfo.Player2DropDown.GetEntryData(curHouseInfo.Player2DropDown.GetSelectedIndex());

            GameInitSettings::HouseInfo houseInfo((HOUSETYPE) houseID, team);

            bool bAdded = false;
            bAdded |= addPlayerToHouseInfo(houseInfo, player1);
            bAdded |= addPlayerToHouseInfo(houseInfo, player2);

            if(bAdded == true) {
                gameInitSettings.addHouseInfo(houseInfo);
            }
        }

        startSinglePlayerGame(gameInitSettings);

        quit(1);
    }
}

bool CustomGamePlayers::addPlayerToHouseInfo(GameInitSettings::HouseInfo& houseInfo, int player)
{
    std::string PlayerName;
    std::string PlayerClass;
    std::string PlayerClassParameter;

    HOUSETYPE houseID = houseInfo.HouseID;

    switch(player) {
        case PLAYER_HUMAN: {
            PlayerName = settings.General.PlayerName;
            PlayerClass = "HumanPlayer";
            gameInitSettings.setHouseID(houseID);
        } break;

        case PLAYER_AI_EASY: {
            PlayerName = (houseID == HOUSE_INVALID) ? "AI (easy)" : getHouseNameByNumber(houseID);
            PlayerClass = "AIPlayer";
            PlayerClassParameter = "0";
        } break;

        case PLAYER_AI_MEDIUM: {
            PlayerName = (houseID == HOUSE_INVALID) ? "AI (medium)" : getHouseNameByNumber(houseID);
            PlayerClass = "AIPlayer";
            PlayerClassParameter = "1";
        } break;

        case PLAYER_AI_DIFFICULT: {
            PlayerName = (houseID == HOUSE_INVALID) ? "AI (difficult)" : getHouseNameByNumber(houseID);
            PlayerClass = "AIPlayer";
            PlayerClassParameter = "2";
        } break;

        case PLAYER_OPEN:
        case PLAYER_CLOSED:
        default: {
            return false;
        }
    }

    houseInfo.addPlayerInfo(GameInitSettings::PlayerInfo(PlayerName, PlayerClass, PlayerClassParameter));
    return true;
}

void CustomGamePlayers::OnCancel()
{
    quit();
}

void CustomGamePlayers::setMapInfo(std::string filename)
{
    INIFile map(filename);

    int sizeX = 0;
    int sizeY = 0;

    if(map.hasKey("MAP","Seed")) {
        // old map format with seed value
        int mapscale = map.getIntValue("BASIC", "MapScale", -1);

        switch(mapscale) {
            case 0: {
                sizeX = 62;
                sizeY = 62;
            } break;

            case 1: {
                sizeX = 32;
                sizeY = 32;
            } break;

            case 2: {
                sizeX = 21;
                sizeY = 21;
            } break;

            default: {
                sizeX = 64;
                sizeY = 64;
            }
        }
    } else {
        // new map format with saved map
        sizeX = map.getIntValue("MAP","SizeX", 0);
        sizeY = map.getIntValue("MAP","SizeY", 0);
    }

    MapProperty_Size.SetText(stringify(sizeX) + " x " + stringify(sizeY));

    SDL_Surface* pMapSurface = GUIStyle::GetInstance().CreateButtonSurface(130,130,"", true, false);

    SDL_Surface* pMinimap = GameINILoader::createMinimapImageOfMap(map);
    SDL_Rect dest = { 1, 1, pMinimap->w, pMinimap->h};
    SDL_BlitSurface(pMinimap, NULL, pMapSurface, &dest);
    SDL_FreeSurface(pMinimap);

    Minimap.SetSurface(pMapSurface, true);


    boundHousesOnMap.clear();
    if(map.hasSection("Atreides"))  boundHousesOnMap.push_back(HOUSE_ATREIDES);
    if(map.hasSection("Ordos"))     boundHousesOnMap.push_back(HOUSE_ORDOS);
    if(map.hasSection("Harkonnen")) boundHousesOnMap.push_back(HOUSE_HARKONNEN);
    if(map.hasSection("Fremen"))    boundHousesOnMap.push_back(HOUSE_FREMEN);
    if(map.hasSection("Mercenary")) boundHousesOnMap.push_back(HOUSE_MERCENARY);
    if(map.hasSection("Sardaukar")) boundHousesOnMap.push_back(HOUSE_SARDAUKAR);

    numHouses = boundHousesOnMap.size();
    if(map.hasSection("Player1"))   numHouses++;
    if(map.hasSection("Player2"))   numHouses++;
    if(map.hasSection("Player3"))   numHouses++;
    if(map.hasSection("Player4"))   numHouses++;
    if(map.hasSection("Player5"))   numHouses++;
    if(map.hasSection("Player6"))   numHouses++;

    MapProperty_Players.SetText(stringify(numHouses));

    std::string authors = map.getStringValue("BASIC","Author", "-");
    if(authors.size() > 11) {
        authors = authors.substr(0,9) + "...";
    }
    MapProperty_Authors.SetText(authors);


    MapProperty_License.SetText(map.getStringValue("BASIC","License", "-"));

}

std::string CustomGamePlayers::extractMapname(std::string filename) {
    size_t slashpos = filename.find_last_of("/\\");
    if(slashpos == std::string::npos) {
        slashpos = 0;
    }

    return filename.substr(slashpos + 1, filename.size() - slashpos - 1 - 4);
}

void CustomGamePlayers::OnChangeHousesDropDownBoxes(int houseInfoNum) {

    int numBoundHouses = boundHousesOnMap.size();
    int numUsedBoundHouses = 0;
    int numUsedRandomHouses = 0;
    for(int i=0;i<numHouses;i++) {
        int selectedHouseID = houseInfo[i].HouseDropDown.GetEntryData(houseInfo[i].HouseDropDown.GetSelectedIndex());

        if(selectedHouseID == HOUSE_INVALID) {
            numUsedRandomHouses++;
        } else {
            if(isBoundedHouseOnMap((HOUSETYPE) selectedHouseID)) {
                numUsedBoundHouses++;
            }
        }
    }


    for(int i=0;i<numHouses;i++) {
        HouseInfo& curHouseInfo = houseInfo[i];

        int house = curHouseInfo.HouseDropDown.GetEntryData(curHouseInfo.HouseDropDown.GetSelectedIndex());

        if(houseInfoNum == -1 || houseInfoNum == i) {

            int color = ((house == HOUSE_INVALID) ? 234 : houseColor[house]) + 3;
            curHouseInfo.HouseLabel.SetTextColor(color);
            curHouseInfo.HouseDropDown.SetColor(color);
            curHouseInfo.TeamDropDown.SetColor(color);
            curHouseInfo.Player1Label.SetTextColor(color);
            curHouseInfo.Player1DropDown.SetColor(color);
            curHouseInfo.Player2Label.SetTextColor(color);
            curHouseInfo.Player2DropDown.SetColor(color);
        }

        addToHouseDropDown(curHouseInfo.HouseDropDown, HOUSE_INVALID);

        for(int h=0;h<NUM_HOUSES;h++) {
            bool bAddHouse;

            bool bCheck;

            if((house == HOUSE_INVALID) || (isBoundedHouseOnMap((HOUSETYPE) house))) {
                if(numUsedBoundHouses + numUsedRandomHouses - 1 >= numBoundHouses) {
                    bCheck = true;
                } else {
                    bCheck = false;
                }
            } else {
                if(numUsedBoundHouses + numUsedRandomHouses >= numBoundHouses) {
                    bCheck = true;
                } else {
                    bCheck = false;
                }
            }

            if(bCheck == true) {
                bAddHouse = true;
                for(int j = 0; j < numHouses; j++) {
                    if(i != j) {
                        DropDownBox& tmpDropDown = houseInfo[j].HouseDropDown;
                        if(tmpDropDown.GetEntryData(tmpDropDown.GetSelectedIndex()) == h) {
                            bAddHouse = false;
                            break;
                        }
                    }
                }
            } else {
                bAddHouse = (h == house);

                if(((house == HOUSE_INVALID) || (isBoundedHouseOnMap((HOUSETYPE) house))) && isBoundedHouseOnMap((HOUSETYPE) h)) {
                    // check if this entry is random or a bounded house but is needed for a bounded house
                    bAddHouse = true;
                    for(int j = 0; j < numHouses; j++) {
                        if(i != j) {
                            DropDownBox& tmpDropDown = houseInfo[j].HouseDropDown;
                            if(tmpDropDown.GetEntryData(tmpDropDown.GetSelectedIndex()) == h) {
                                bAddHouse = false;
                                break;
                            }
                        }
                    }
                }
            }

            if(bAddHouse == true) {
                addToHouseDropDown(curHouseInfo.HouseDropDown, h);
            } else {
                removeFromHouseDropDown(curHouseInfo.HouseDropDown, h);
            }
        }
    }
}

void CustomGamePlayers::OnChangePlayerDropDownBoxes(int boxnum) {
    checkPlayerBoxes();
}

void CustomGamePlayers::OnClickPlayerDropDownBox(int boxnum) {
    DropDownBox& dropDownBox = (boxnum % 2 == 0) ? houseInfo[boxnum / 2].Player1DropDown : houseInfo[boxnum / 2].Player2DropDown;

    if(dropDownBox.GetEntryData(dropDownBox.GetSelectedIndex()) != PLAYER_OPEN) {
        return;
    }


    for(int i=0;i<numHouses*2;i++) {
        DropDownBox& curDropDownBox = (i % 2 == 0) ? houseInfo[i / 2].Player1DropDown : houseInfo[i / 2].Player2DropDown;

        if(curDropDownBox.GetEntryData(curDropDownBox.GetSelectedIndex()) == PLAYER_HUMAN) {
            curDropDownBox.ClearAllEntries();
            curDropDownBox.AddEntry(pTextManager->getLocalized("open"), PLAYER_OPEN);
            curDropDownBox.SetSelectedItem(0);
            curDropDownBox.AddEntry(pTextManager->getLocalized("closed"), PLAYER_CLOSED);
            curDropDownBox.AddEntry("AI (easy)", PLAYER_AI_EASY);
            curDropDownBox.AddEntry("AI (medium)", PLAYER_AI_MEDIUM);
            curDropDownBox.AddEntry("AI (difficult)", PLAYER_AI_DIFFICULT);
            break;
        }
    }

    dropDownBox.ClearAllEntries();
    dropDownBox.AddEntry(settings.General.PlayerName, PLAYER_HUMAN);
    dropDownBox.SetSelectedItem(0);

    checkPlayerBoxes();
}

void CustomGamePlayers::checkPlayerBoxes() {
    for(int i=0;i<numHouses;i++) {
        HouseInfo& curHouseInfo = houseInfo[i];

        int player1 = curHouseInfo.Player1DropDown.GetEntryData(curHouseInfo.Player1DropDown.GetSelectedIndex());
        int player2 = curHouseInfo.Player2DropDown.GetEntryData(curHouseInfo.Player2DropDown.GetSelectedIndex());

        if((bMultiplePlayersPerHouse == false) || (player1 == PLAYER_OPEN && player2 == PLAYER_OPEN)) {
            curHouseInfo.Player2DropDown.SetVisible(false);
            curHouseInfo.Player2Label.SetVisible(false);
        } else {
            curHouseInfo.Player2DropDown.SetVisible(true);
            curHouseInfo.Player2Label.SetVisible(true);
        }
    }
}


void CustomGamePlayers::addToHouseDropDown(DropDownBox& houseDropDownBox, int house, bool bSelect) {

    if(houseDropDownBox.GetNumEntries() == 0) {
        if(house == HOUSE_INVALID) {
            houseDropDownBox.AddEntry(pTextManager->getLocalized("Random"), HOUSE_INVALID);
        } else {
            houseDropDownBox.AddEntry(getHouseNameByNumber((HOUSETYPE) house), house);
        }

        if(bSelect) {
            houseDropDownBox.SetSelectedItem(0);
        }
    } else {

        if(house == HOUSE_INVALID) {
            if(houseDropDownBox.GetEntryData(0) != HOUSE_INVALID) {
                houseDropDownBox.InsertEntry(0, pTextManager->getLocalized("Random"), HOUSE_INVALID);
            }

            if(bSelect) {
                houseDropDownBox.SetSelectedItem(houseDropDownBox.GetNumEntries()-1);
            }
        } else {

            int currentItemIndex = (houseDropDownBox.GetEntryData(0) == HOUSE_INVALID) ? 1 : 0;

            for(int h = 0; h < NUM_HOUSES; h++) {
                if(currentItemIndex < houseDropDownBox.GetNumEntries() && houseDropDownBox.GetEntryData(currentItemIndex) == h) {
                    if(h == house) {
                        if(bSelect) {
                            houseDropDownBox.SetSelectedItem(currentItemIndex);
                        }
                        break;
                    }

                    currentItemIndex++;
                } else {
                    if(h == house) {
                        houseDropDownBox.InsertEntry(currentItemIndex, getHouseNameByNumber((HOUSETYPE) h), h);

                        if(bSelect) {
                            houseDropDownBox.SetSelectedItem(currentItemIndex);
                        }
                        break;
                    }
                }


            }
        }
    }
}

void CustomGamePlayers::removeFromHouseDropDown(DropDownBox& houseDropDownBox, int house) {

    for(int i=0;i<houseDropDownBox.GetNumEntries(); i++) {
        if(houseDropDownBox.GetEntryData(i) == house) {
            houseDropDownBox.RemoveEntry(i);
            break;
        }
    }
}

bool CustomGamePlayers::isBoundedHouseOnMap(HOUSETYPE houseID) {
    std::list<HOUSETYPE>::const_iterator iter;
    for(iter = boundHousesOnMap.begin(); iter != boundHousesOnMap.end(); ++iter) {
        if(*iter == houseID) {
            return true;
        }
    }

    return false;
}
