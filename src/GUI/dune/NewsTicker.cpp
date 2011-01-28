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

#include <GUI/dune/NewsTicker.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <FileClasses/FontManager.h>

#define MESSAGETIME 440
#define SLOWDOWN timer/55

NewsTicker::NewsTicker() : Widget() {
	EnableResizing(false,false);

 	timer = -MESSAGETIME/2;
	pBackground = pGFXManager->getUIGraphic(UI_MessageBox);

	Resize(pBackground->w,pBackground->h);
}

NewsTicker::~NewsTicker() {

}

void NewsTicker::AddMessage(const std::string& msg)
{
	bool found = false;

	/*if message is already there, do nothing*/
	std::queue<std::string> msgcpy(Messages);
	while(msgcpy.size())
	{
		if(msgcpy.front() == msg)
			found = true;
	    msgcpy.pop();
	};

	if(!found)
	Messages.push(msg);
}

void NewsTicker::AddUrgentMessage(const std::string& msg)
{
	while(Messages.size())
		Messages.pop();

	Messages.push(msg);
}

void NewsTicker::Draw(SDL_Surface* screen, Point Position) {
	if(IsVisible() == false) {
		return;
	}

	//draw background
	if(pBackground == NULL) {
			return;
	}

	SDL_Rect dest;
	dest.x = Position.x;
	dest.y = Position.y;
	dest.w = pBackground->w;
	dest.h = pBackground->h;

	SDL_BlitSurface(pBackground,NULL,screen,&dest);

	// draw message
	if(Messages.size()>0)
	{
		if(timer++ == (MESSAGETIME/3))
		{
			timer = -MESSAGETIME/2;
			//delete first message
			if(Messages.size())
				Messages.pop();

			/*if no more messages leave*/
			if(Messages.size() == 0) {
				timer = -MESSAGETIME/2;
				return;
			};
		};

		//draw text
		SDL_Rect textLocation;

		textLocation.x = Position.x + 10;
		textLocation.y = Position.y + 5;

		if (timer>0)
			textLocation.y -= SLOWDOWN;

		SDL_Surface *surface = pFontManager->createSurfaceWithText(Messages.front(), COLOR_BLACK, FONT_STD10);

		SDL_Rect cut;
		cut.x = 0;
		cut.y = 0;

		if (timer>0)
			cut.y = 3*SLOWDOWN;

		cut.h = surface->h - cut.y;
		cut.w = surface->w;
		SDL_BlitSurface(surface, &cut, screen, &textLocation);
		SDL_FreeSurface(surface);
	};
}
