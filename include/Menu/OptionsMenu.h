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

#ifndef OPTIONSMENU_H
#define OPTIONSMENU_H

#include "MenuClass.h"
#include <GUI/StaticContainer.h>
#include <GUI/HBox.h>
#include <GUI/VBox.h>
#include <GUI/TextButton.h>
#include <GUI/TextBox.h>
#include <GUI/Checkbox.h>
#include <GUI/DropDownBox.h>
#include <DataTypes.h>

#include <SDL.h>
#include <vector>


class OptionsMenu : public MenuClass
{
public:
	OptionsMenu();
	virtual ~OptionsMenu();

private:
	void	OnChangeOption();
	void	OnOptionsOK();
	void	OnOptionsCancel();

	void	SaveConfiguration2File();

	/**
        This method determines all available suitable screen resolutions.
        If the system returns that any resolution is possible a reasonable
        set of resolutions is provided.
    */
    void determineAvailableScreenResolutions();

	std::vector<Coord> availScreenRes;

	StaticContainer	WindowWidget;

	VBox		MainVBox;

	HBox        NameHBox;
	TextBox		TextBox_Name;
	HBox        GameOptionsHBox;
	Checkbox	Checkbox_Concrete;
	Checkbox	Checkbox_FogOfWar;

	HBox        LanguageHBox;
	DropDownBox DropDownBox_Language;
	HBox        GeneralHBox;
	Checkbox	Checkbox_Intro;

	HBox        ResolutionHBox;
	DropDownBox DropDownBox_Resolution;
	HBox        VideoHBox;
	Checkbox	Checkbox_FullScreen;
	Checkbox	Checkbox_DoubleBuffering;

	HBox		OkCancel_HBox;
	TextButton	Button_Accept;
	TextButton	Button_Back;
};


#endif // OPTIONSMENU_H
