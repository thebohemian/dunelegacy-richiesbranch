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

#ifndef MENUCLASS_H
#define MENUCLASS_H

#include <GUI/Window.h>
#include <SDL.h>


class MenuClass: public Window
{
public:

	MenuClass();
	virtual ~MenuClass();

	virtual int showMenu();
	virtual void quit(int returnVal = -1);
	virtual bool IsQuit() { return quiting; };
	void DisableQuiting(bool disable) { bAllowQuiting = !disable; };

	virtual void DrawSpecificStuff();


	virtual void draw();
	virtual bool doInput(SDL_Event &event);

	void SetClearScreen(bool bClearScreen) {
		this->bClearScreen = bClearScreen;
	};

private:
	bool bClearScreen;
	bool bAllowQuiting;
	bool quiting;
	int  retVal;
};

#endif // MENUCLASS_H
