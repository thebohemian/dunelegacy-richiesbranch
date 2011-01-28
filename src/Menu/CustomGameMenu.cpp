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

#include <Menu/CustomGameMenu.h>
#include <Menu/CustomGamePlayers.h>

#include <FileClasses/GFXManager.h>
#include <FileClasses/TextManager.h>
#include <FileClasses/INIFile.h>

#include <GUI/Spacer.h>
#include <GUI/GUIStyle.h>

#include <config.h>
#include <misc/fnkdat.h>
#include <misc/FileSystem.h>
#include <misc/draw_util.h>
#include <misc/string_util.h>

#include <GameINILoader.h>
#include <GameInitSettings.h>

#include <globals.h>


CustomGameMenu::CustomGameMenu() : MenuClass()
{
	// set up window
	SDL_Surface *surf;
	surf = pGFXManager->getUIGraphic(UI_MenuBackground);

	SetBackground(surf,false);
	Resize(surf->w,surf->h);

	SetWindowWidget(&WindowWidget);

	WindowWidget.AddWidget(&MainVBox, Point(24,23),	Point(screen->w - 48, screen->h - 46));

    CaptionLabel.SetText(pTextManager->getLocalized("Custom Game"));
    CaptionLabel.SetAlignment(Alignment_HCenter);
    MainVBox.AddWidget(&CaptionLabel, 24);
    MainVBox.AddWidget(VSpacer::Create(24));


    MainVBox.AddWidget(&MainHBox, 1.0);

    MainHBox.AddWidget(&LeftVBox, 1.0);

    LeftVBox.AddWidget(&MapTypeButtonsHBox, 24);


    SingleplayerMapsButton.SetText(pTextManager->getLocalized("SP Maps"));
    SingleplayerMapsButton.SetToggleButton(true);
    SingleplayerMapsButton.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithIntParameter) (&CustomGameMenu::OnMapTypeChange), 0));
    MapTypeButtonsHBox.AddWidget(&SingleplayerMapsButton);

    SingleplayerUserMapsButton.SetText(pTextManager->getLocalized("SP User Maps"));
    SingleplayerUserMapsButton.SetToggleButton(true);
    SingleplayerUserMapsButton.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithIntParameter) (&CustomGameMenu::OnMapTypeChange), 1));
    MapTypeButtonsHBox.AddWidget(&SingleplayerUserMapsButton);

    MultiplayerMapsButton.SetText(pTextManager->getLocalized("MP Maps"));
    MultiplayerMapsButton.SetToggleButton(true);
    MultiplayerMapsButton.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithIntParameter) (&CustomGameMenu::OnMapTypeChange), 2));
    MapTypeButtonsHBox.AddWidget(&MultiplayerMapsButton);

    MultiplayerUserMapsButton.SetText(pTextManager->getLocalized("MP User Maps"));
    MultiplayerUserMapsButton.SetToggleButton(true);
    MultiplayerUserMapsButton.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithIntParameter) (&CustomGameMenu::OnMapTypeChange), 3));
    MapTypeButtonsHBox.AddWidget(&MultiplayerUserMapsButton);

    DummyButton.SetEnabled(false);
    MapTypeButtonsHBox.AddWidget(&DummyButton, 17);
    MapList.setAutohideScrollbar(false);
    MapList.SetOnSelectionChangeMethod(MethodPointer(this, (WidgetCallbackWithNoParameter) (&CustomGameMenu::OnMapListSelectionChange)));
    MapList.SetOnDoubleClickMethod(MethodPointer(this, (WidgetCallbackWithNoParameter) (&CustomGameMenu::OnNext)));
    LeftVBox.AddWidget(&MapList, 0.95);

    LeftVBox.AddWidget(VSpacer::Create(10));

    MainHBox.AddWidget(HSpacer::Create(8));

    MainHBox.AddWidget(&RightVBox, 180);
    Minimap.SetSurface( GUIStyle::GetInstance().CreateButtonSurface(130,130,pTextManager->getLocalized("Choose map"), true, false), true);
    RightVBox.AddWidget(&Minimap);

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

    MultiplePlayersPerHouse.SetText(pTextManager->getLocalized("Multiple players per house"));
    OptionsHBox.AddWidget(&MultiplePlayersPerHouse);
    RequireConcrete.SetText(pTextManager->getLocalized("Concrete required"));
    RequireConcrete.SetChecked(true);
    OptionsHBox.AddWidget(&RequireConcrete);
    FogOfWar.SetText(pTextManager->getLocalized("Fog of War"));
    OptionsHBox.AddWidget(&FogOfWar);

    MainVBox.AddWidget(&OptionsHBox, 30);

    MainVBox.AddWidget(VSpacer::Create(20));
	MainVBox.AddWidget(&ButtonHBox, 24);

    ButtonHBox.AddWidget(HSpacer::Create(70));
    Button_Cancel.SetText(pTextManager->getLocalized("Back"));
	Button_Cancel.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&CustomGameMenu::OnCancel)));
	ButtonHBox.AddWidget(&Button_Cancel, 0.2);

	ButtonHBox.AddWidget(Spacer::Create(), 0.6);

    Button_Next.SetText(pTextManager->getLocalized("Next"));
	Button_Next.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&CustomGameMenu::OnNext)));
	ButtonHBox.AddWidget(&Button_Next, 0.2);
	ButtonHBox.AddWidget(HSpacer::Create(90));

	OnMapTypeChange(0);
}

CustomGameMenu::~CustomGameMenu()
{
	;
}

void CustomGameMenu::OnNext()
{
    if(MapList.GetSelectedIndex() < 0) {
        return;
    }

    std::string mapFilename = currentMapDirectory + MapList.GetEntry(MapList.GetSelectedIndex()) + ".ini";
    GetCaseInsensitiveFilename(mapFilename);

    CustomGamePlayers* pCustomGamePlayers = new CustomGamePlayers(GameInitSettings(mapFilename, RequireConcrete.IsChecked(), FogOfWar.IsChecked()), MultiplePlayersPerHouse.IsChecked());
    if(pCustomGamePlayers == NULL) {
		perror("CustomGameMenu::OnNext()");
		exit(EXIT_FAILURE);
	}
    int ret = pCustomGamePlayers->showMenu();
    delete pCustomGamePlayers;
    if(ret != -1) {
        quit(ret);
    }
}

void CustomGameMenu::OnCancel()
{
    quit();
}

void CustomGameMenu::OnMapTypeChange(int buttonID)
{
    SingleplayerMapsButton.SetToggleState(buttonID == 0);
    SingleplayerUserMapsButton.SetToggleState(buttonID == 1);
    MultiplayerMapsButton.SetToggleState(buttonID == 2);
    MultiplayerUserMapsButton.SetToggleState(buttonID == 3);

    switch(buttonID) {
        case 0: {
            currentMapDirectory = std::string(DUNELEGACY_DATADIR) + std::string("/maps/singleplayer/");
        } break;
        case 1: {
            char tmp[FILENAME_MAX];
            fnkdat("maps/singleplayer/", tmp, FILENAME_MAX, FNKDAT_USER | FNKDAT_CREAT);
            currentMapDirectory = tmp;
        } break;
        case 2: {
            currentMapDirectory = std::string(DUNELEGACY_DATADIR) + std::string("/maps/multiplayer/");
        } break;
        case 3: {
            char tmp[FILENAME_MAX];
            fnkdat("maps/multiplayer/", tmp, FILENAME_MAX, FNKDAT_USER | FNKDAT_CREAT);
            currentMapDirectory = tmp;
        } break;
    }

    MapList.ClearAllEntries();

    std::list<std::string> filesList = GetFileNamesList(currentMapDirectory, "ini", true, FileListOrder_Name_CaseInsensitive_Asc);

    std::list<std::string>::iterator iter;
    for(iter = filesList.begin(); iter != filesList.end(); ++iter) {
        MapList.AddEntry(iter->substr(0, iter->length() - 4));
    }

    if(filesList.empty() == false) {
        MapList.SetSelectedItem(0);
    } else {
        Minimap.SetSurface( GUIStyle::GetInstance().CreateButtonSurface(130,130,pTextManager->getLocalized("No map available"), true, false), true);
        MapProperty_Size.SetText("");
        MapProperty_Players.SetText("");
        MapProperty_Authors.SetText("");
        MapProperty_License.SetText("");
    }
}

void CustomGameMenu::OnMapListSelectionChange()
{
    if(MapList.GetSelectedIndex() < 0) {
        return;
    }

    std::string mapFilename = currentMapDirectory + MapList.GetEntry(MapList.GetSelectedIndex()) + ".ini";
    GetCaseInsensitiveFilename(mapFilename);
    INIFile map(mapFilename);


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

    int numPlayers = 0;
    if(map.hasSection("Atreides")) numPlayers++;
    if(map.hasSection("Ordos")) numPlayers++;
    if(map.hasSection("Harkonnen")) numPlayers++;
    if(map.hasSection("Fremen")) numPlayers++;
    if(map.hasSection("Mercenary")) numPlayers++;
    if(map.hasSection("Sardaukar")) numPlayers++;
    if(map.hasSection("Player1")) numPlayers++;
    if(map.hasSection("Player2")) numPlayers++;
    if(map.hasSection("Player3")) numPlayers++;
    if(map.hasSection("Player4")) numPlayers++;
    if(map.hasSection("Player5")) numPlayers++;
    if(map.hasSection("Player6")) numPlayers++;

    MapProperty_Players.SetText(stringify(numPlayers));



    std::string authors = map.getStringValue("BASIC","Author", "-");
    if(authors.size() > 11) {
        authors = authors.substr(0,9) + "...";
    }
    MapProperty_Authors.SetText(authors);


    MapProperty_License.SetText(map.getStringValue("BASIC","License", "-"));

}
