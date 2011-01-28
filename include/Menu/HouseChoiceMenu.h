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

#ifndef HOUSECHOICEMENU_H
#define HOUSECHOICEMENU_H

#include "MenuClass.h"
#include <GUI/StaticContainer.h>
#include <GUI/InvisibleButton.h>

class HouseChoiceMenu : public MenuClass {
public:
	HouseChoiceMenu();
	virtual ~HouseChoiceMenu();

private:
	void OnAtreides();
	void OnOrdos();
	void OnHarkonnen();

	StaticContainer	WindowWidget;

	InvisibleButton Button_Atreides;
	InvisibleButton Button_Ordos;
	InvisibleButton Button_Harkonnen;
};

#endif // HOUSECHOICEMENU_H
