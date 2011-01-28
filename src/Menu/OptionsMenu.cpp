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

#include <Menu/OptionsMenu.h>

#include <globals.h>

#include <GUI/Label.h>
#include <GUI/Spacer.h>

#include <main.h>

#include <FileClasses/GFXManager.h>
#include <FileClasses/TextManager.h>
#include <FileClasses/INIFile.h>

#include <string.h>
#include <algorithm>

OptionsMenu::OptionsMenu() : MenuClass()
{
    determineAvailableScreenResolutions();


	// set up window
	SDL_Surface *surf;
	surf = pGFXManager->getUIGraphic(UI_MenuBackground);

	SetBackground(surf,false);
	Resize(surf->w,surf->h);

	SetWindowWidget(&WindowWidget);

	WindowWidget.AddWidget(&MainVBox, Point(70,70), Point(GetSize().x - 140,GetSize().y - 140));

    MainVBox.AddWidget(Spacer::Create(), 0.2);

	NameHBox.AddWidget(Spacer::Create(), 0.5);
	NameHBox.AddWidget(Label::Create(pTextManager->getLocalized("Player Name")), 190);
	TextBox_Name.SetOnTextChangeMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&OptionsMenu::OnChangeOption)));
	NameHBox.AddWidget(&TextBox_Name, 250);
	NameHBox.AddWidget(Spacer::Create(), 0.5);
	TextBox_Name.SetText(settings.General.PlayerName);

	MainVBox.AddWidget(&NameHBox, 0.01);

	MainVBox.AddWidget(VSpacer::Create(1));

    GameOptionsHBox.AddWidget(Spacer::Create(), 0.5);

    Checkbox_Concrete.SetText(pTextManager->getLocalized("Concrete required"));
    Checkbox_Concrete.SetChecked(settings.General.ConcreteRequired);
	Checkbox_Concrete.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&OptionsMenu::OnChangeOption)));
	GameOptionsHBox.AddWidget(&Checkbox_Concrete, 220);

    Checkbox_FogOfWar.SetText(pTextManager->getLocalized("Fog of War"));
    Checkbox_FogOfWar.SetChecked(settings.General.FogOfWar);
    Checkbox_FogOfWar.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&OptionsMenu::OnChangeOption)));
	GameOptionsHBox.AddWidget(&Checkbox_FogOfWar, 220);

	GameOptionsHBox.AddWidget(Spacer::Create(), 0.5);

	MainVBox.AddWidget(&GameOptionsHBox, 0.01);

	MainVBox.AddWidget(Spacer::Create(), 0.2);

	LanguageHBox.AddWidget(Spacer::Create(), 0.5);
	LanguageHBox.AddWidget(Label::Create(pTextManager->getLocalized("Language")), 190);
	DropDownBox_Language.AddEntry("English", LNG_ENG);
	DropDownBox_Language.AddEntry("French", LNG_FRE);
	DropDownBox_Language.AddEntry("German", LNG_GER);
	switch(settings.General.Language) {
        case LNG_ENG: DropDownBox_Language.SetSelectedItem(0); break;
        case LNG_FRE: DropDownBox_Language.SetSelectedItem(1); break;
        case LNG_GER: DropDownBox_Language.SetSelectedItem(2); break;
	}
	DropDownBox_Language.SetOnSelectionChangeMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&OptionsMenu::OnChangeOption)));
	LanguageHBox.AddWidget(&DropDownBox_Language, 100);
	LanguageHBox.AddWidget(Spacer::Create(), 150);
	LanguageHBox.AddWidget(Spacer::Create(), 0.5);

	MainVBox.AddWidget(&LanguageHBox, 0.01);

	MainVBox.AddWidget(VSpacer::Create(1));

	GeneralHBox.AddWidget(Spacer::Create(), 0.5);
	Checkbox_Intro.SetText(pTextManager->getLocalized("Play Intro"));
	Checkbox_Intro.SetChecked(settings.General.PlayIntro);
	Checkbox_Intro.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&OptionsMenu::OnChangeOption)));
    GeneralHBox.AddWidget(&Checkbox_Intro, 440);
    GeneralHBox.AddWidget(Spacer::Create(), 0.5);

    MainVBox.AddWidget(&GeneralHBox, 0.01);

	MainVBox.AddWidget(Spacer::Create(), 0.2);

	ResolutionHBox.AddWidget(Spacer::Create(), 0.5);
	ResolutionHBox.AddWidget(Label::Create(pTextManager->getLocalized("Video Resolution")), 190);

    std::vector<Coord>::const_iterator iter;
    int i = 0;
    for(iter = availScreenRes.begin(); iter != availScreenRes.end(); ++iter, ++i) {
        char temp[20];
        sprintf(temp,"%d x %d", iter->x, iter->y);
        DropDownBox_Resolution.AddEntry(temp, i);
        if(iter->x == settings.Video.Width && iter->y == settings.Video.Height) {
            DropDownBox_Resolution.SetSelectedItem(i);
        }
    }
    DropDownBox_Resolution.SetOnSelectionChangeMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&OptionsMenu::OnChangeOption)));
	ResolutionHBox.AddWidget(&DropDownBox_Resolution, 100);
	ResolutionHBox.AddWidget(Spacer::Create(), 150);
	ResolutionHBox.AddWidget(Spacer::Create(), 0.5);

	MainVBox.AddWidget(&ResolutionHBox, 0.01);

    MainVBox.AddWidget(VSpacer::Create(1));

    VideoHBox.AddWidget(Spacer::Create(), 0.5);
    Checkbox_FullScreen.SetText(pTextManager->getLocalized("Full Screen"));
    Checkbox_FullScreen.SetChecked(settings.Video.Fullscreen);
    Checkbox_FullScreen.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&OptionsMenu::OnChangeOption)));
    VideoHBox.AddWidget(&Checkbox_FullScreen, 220);
    Checkbox_DoubleBuffering.SetText(pTextManager->getLocalized("Double Buffering"));
    Checkbox_DoubleBuffering.SetChecked(settings.Video.DoubleBuffering);
    Checkbox_DoubleBuffering.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&OptionsMenu::OnChangeOption)));
    VideoHBox.AddWidget(&Checkbox_DoubleBuffering, 220);
    VideoHBox.AddWidget(Spacer::Create(), 0.5);

    MainVBox.AddWidget(&VideoHBox, 0.01);

	MainVBox.AddWidget(Spacer::Create(), 0.2);

	OkCancel_HBox.AddWidget(Spacer::Create());

	Button_Back.SetText(pTextManager->getLocalized("Back"));
	Button_Back.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&OptionsMenu::OnOptionsCancel)));
	OkCancel_HBox.AddWidget(&Button_Back);

	OkCancel_HBox.AddWidget(Spacer::Create());

	Button_Accept.SetText(pTextManager->getLocalized("Accept"));
	Button_Accept.SetVisible(false);
	Button_Accept.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&OptionsMenu::OnOptionsOK)));
	OkCancel_HBox.AddWidget(&Button_Accept);

	OkCancel_HBox.AddWidget(Spacer::Create());

	MainVBox.AddWidget(&OkCancel_HBox, 26);

    MainVBox.AddWidget(Spacer::Create(), 0.1);
}

OptionsMenu::~OptionsMenu()
{
	;
}

void OptionsMenu::OnChangeOption() {
    bool bChanged = false;

    bChanged |= (settings.General.PlayerName != TextBox_Name.GetText());
    bChanged |= (settings.General.ConcreteRequired != Checkbox_Concrete.IsChecked());
    bChanged |= (settings.General.FogOfWar != Checkbox_FogOfWar.IsChecked());
    bChanged |= (settings.General.Language != DropDownBox_Language.GetEntryData(DropDownBox_Language.GetSelectedIndex()));
    bChanged |= (settings.General.PlayIntro != Checkbox_Intro.IsChecked());

    int selectedResolution = DropDownBox_Resolution.GetEntryData(DropDownBox_Resolution.GetSelectedIndex());
    bChanged |= (settings.Video.Width != availScreenRes[selectedResolution].x);
    bChanged |= (settings.Video.Height != availScreenRes[selectedResolution].y);
    bChanged |= (settings.Video.Fullscreen != Checkbox_FullScreen.IsChecked());
    bChanged |= (settings.Video.DoubleBuffering != Checkbox_DoubleBuffering.IsChecked());

    Button_Accept.SetVisible(bChanged);
}

void OptionsMenu::OnOptionsOK() {
	settings.General.PlayerName = TextBox_Name.GetText();

	settings.General.ConcreteRequired = Checkbox_Concrete.IsChecked();
	settings.General.FogOfWar = Checkbox_FogOfWar.IsChecked();

    settings.General.setLanguage(DropDownBox_Language.GetEntryData(DropDownBox_Language.GetSelectedIndex()));
	settings.General.PlayIntro = Checkbox_Intro.IsChecked();

    int selectedResolution = DropDownBox_Resolution.GetEntryData(DropDownBox_Resolution.GetSelectedIndex());
    settings.Video.Width = availScreenRes[selectedResolution].x;
	settings.Video.Height = availScreenRes[selectedResolution].y;
	settings.Video.Fullscreen = Checkbox_FullScreen.IsChecked();
	settings.Video.DoubleBuffering = Checkbox_DoubleBuffering.IsChecked();

	SaveConfiguration2File();

	quit(1);
}

void OptionsMenu::OnOptionsCancel() {
	quit();
}

void OptionsMenu::SaveConfiguration2File() {
	INIFile myINIFile(GetConfigFilepath());

	myINIFile.setBoolValue("General","Concrete Required",settings.General.ConcreteRequired);
    myINIFile.setBoolValue("General","Fog of War",settings.General.FogOfWar);
	myINIFile.setBoolValue("General","Play Intro",settings.General.PlayIntro);
	myINIFile.setIntValue("Video","Width",settings.Video.Width);
	myINIFile.setIntValue("Video","Height",settings.Video.Height);
	myINIFile.setBoolValue("Video","Fullscreen",settings.Video.Fullscreen);
	myINIFile.setBoolValue("Video","Double Buffering",settings.Video.DoubleBuffering);
	myINIFile.setStringValue("General","Player Name",settings.General.PlayerName);
	myINIFile.setStringValue("General","Language",settings.General.getLanguageString());

	myINIFile.SaveChangesTo(GetConfigFilepath());
}

void OptionsMenu::determineAvailableScreenResolutions() {
    availScreenRes.clear();

	SDL_Rect** pResolutions = SDL_ListModes(NULL , SDL_HWSURFACE | SDL_FULLSCREEN);

	if(pResolutions == NULL || pResolutions == (SDL_Rect**) -1) {
		// Not possible or not available
		// try some standard resolutions

		availScreenRes.push_back( Coord(640, 480) );    // VGA (4:3)
		availScreenRes.push_back( Coord(800, 480) );    // WVGA (5:3)
		availScreenRes.push_back( Coord(800, 600) );    // SVGA (4:3)
		availScreenRes.push_back( Coord(960, 540) );    // Quarter HD (16:9)
		availScreenRes.push_back( Coord(960, 720) );    // ? (4:3)
		availScreenRes.push_back( Coord(1024, 576) );   // WSVGA (16:9)
		availScreenRes.push_back( Coord(1024, 640) );   // ? (16:10)
		availScreenRes.push_back( Coord(1024, 768) );   // XGA (4:3)
		availScreenRes.push_back( Coord(1152, 864) );   // XGA+ (4:3)
		availScreenRes.push_back( Coord(1280, 720 ) );  // WXGA (16:9)
		availScreenRes.push_back( Coord(1280, 768) );   // WXGA (5:3)
		availScreenRes.push_back( Coord(1280, 800) );   // WXGA (16:10)
		availScreenRes.push_back( Coord(1280, 960) );   // SXGA- (4:3)
		availScreenRes.push_back( Coord(1280, 1024) );  // SXGA (5:4)
		availScreenRes.push_back( Coord(1366, 768) );   // HDTV 720p (~16:9)
		availScreenRes.push_back( Coord(1400, 1050) );  // SXGA+ (4:3)
		availScreenRes.push_back( Coord(1440, 900) );   // WXGA+ (16:10)
		availScreenRes.push_back( Coord(1440, 1080) );  // ? (4:3)
		availScreenRes.push_back( Coord(1600, 1200) );  // UXGA (4:3)
		availScreenRes.push_back( Coord(1680, 1050) );  // WSXGA+ (16:10)
		availScreenRes.push_back( Coord(1920, 1080) );  // 1080p (16:9)
		availScreenRes.push_back( Coord(1920, 1200) );  // WUXGA (16:10)
	} else {
	    // step backward through resolutions
		for(int i=0; pResolutions[i] != NULL;  i++) {
		    if(pResolutions[i]->w >= SCREEN_MIN_WIDTH && pResolutions[i]->h >= SCREEN_MIN_HEIGHT) {
		        Coord newRes = Coord(pResolutions[i]->w,pResolutions[i]->h);
		        if(std::find(availScreenRes.begin(), availScreenRes.end(), newRes) == availScreenRes.end()) {
		            // not yet in the list
		            availScreenRes.insert(availScreenRes.begin(), newRes);
		        }
		    }
		}
	}

	Coord currentRes(settings.Video.Width, settings.Video.Height);

    if(std::find(availScreenRes.begin(), availScreenRes.end(), currentRes) == availScreenRes.end()) {
        // not yet in the list
        availScreenRes.insert(availScreenRes.begin(), currentRes);
    }
}
