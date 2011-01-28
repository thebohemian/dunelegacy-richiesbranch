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

#ifndef MENTATMENU_H
#define MENTATMENU_H

#include "MenuClass.h"
#include <GUI/StaticContainer.h>
#include <GUI/Button.h>
#include <GUI/dune/AnimationLabel.h>
#include <GUI/Label.h>

#include <string>

class MentatMenu : public MenuClass {
public:
	MentatMenu(int newHouse);
	virtual ~MentatMenu();

	void DrawSpecificStuff();

	void setText(std::string text) {
		TextLabel.SetText(text.c_str());
		TextLabel.SetVisible(true);
		TextLabel.Resize(620,240);
	}

protected:
	int house;

	StaticContainer	WindowWidget;
	AnimationLabel	eyesAnim;
	AnimationLabel	mouthAnim;
	AnimationLabel	specialAnim;
	AnimationLabel	shoulderAnim;
	Label			TextLabel;

};

#endif // MENTATMENU_H
