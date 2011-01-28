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

#include <Menu/AboutMenu.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>

AboutMenu::AboutMenu() : MenuClass()
{
	// set up window
	SDL_Surface *surf;
	surf = pGFXManager->getUIGraphic(UI_MenuBackground);

	SetBackground(surf,false);
	Resize(surf->w,surf->h);

	SetWindowWidget(&WindowWidget);

	// set up pictures in the background
	surf = pGFXManager->getUIGraphic(UI_PlanetBackground);
	PlanetPicture.SetSurface(surf,false);
	WindowWidget.AddWidget(&PlanetPicture,
							Point((screen->w - surf->w)/2,screen->h/2 - surf->h + 10),
							Point(surf->w,surf->h));

	surf = pGFXManager->getUIGraphic(UI_DuneLegacy);
	DuneLegacy.SetSurface(surf,false);
	WindowWidget.AddWidget(&DuneLegacy,
							Point((screen->w - surf->w)/2, screen->h/2 + 28),
							Point(surf->w,surf->h));

	surf = pGFXManager->getUIGraphic(UI_MenuButtonBorder);
	ButtonBorder.SetSurface(surf,false);
	WindowWidget.AddWidget(&ButtonBorder,
							Point((screen->w - surf->w)/2, screen->h/2 + 59),
							Point(surf->w,surf->h));


	Text.SetText("Written by\n  Anthony Cole,\n    Richard Schaller\n      and many others\n");
	Text.SetAlignment(Alignment_Left);
	WindowWidget.AddWidget(&Text,
							Point((screen->w - 160)/2,screen->h/2 + 74),
							Point(170,110));
}

AboutMenu::~AboutMenu()
{
	;
}

bool AboutMenu::doInput(SDL_Event &event)
{
    if(event.type == SDL_MOUSEBUTTONUP) {
        quit(0);
    }

    return MenuClass::doInput(event);
}

