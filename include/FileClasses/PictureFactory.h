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

#ifndef PICTUREFACTORY_H
#define PICTUREFACTORY_H

#include <SDL.h>

#include <misc/memory.h>

class PictureFactory {
public:
	PictureFactory();
	~PictureFactory();

	SDL_Surface* createTopBar();
	SDL_Surface* createGameBar();
	SDL_Surface* createValidPlace();
	SDL_Surface* createInvalidPlace();
	void drawFrame(SDL_Surface* Pic, unsigned int DecorationType, SDL_Rect* dest=NULL);
	SDL_Surface* createBackground();
	SDL_Surface* createMainBackground();
	SDL_Surface* createGameStatsBackground(int House);
	SDL_Surface* createFrame(unsigned int DecorationType,int width, int height,bool UseBackground);
	SDL_Surface* createMenu(SDL_Surface* CaptionPic,int y);
	SDL_Surface* createOptionsMenu();
	SDL_Surface* createMessageBoxBorder();
	SDL_Surface* createHouseSelect(SDL_Surface* HouseChoice);
	SDL_Surface* createMapChoiceScreen(int House);

	typedef enum {
		SimpleFrame,
		DecorationFrame1,
		DecorationFrame2,
		NUM_DECORATIONFRAMES
	} DecorationFrame;

private:
	struct DecorationBorderType {
		std::shared_ptr<SDL_Surface> ball;
		std::shared_ptr<SDL_Surface> hspacer;
		std::shared_ptr<SDL_Surface> vspacer;
		std::shared_ptr<SDL_Surface> hborder;
		std::shared_ptr<SDL_Surface> vborder;
	} DecorationBorder;

	struct BorderStyle {
		std::shared_ptr<SDL_Surface> LeftUpperCorner;
		std::shared_ptr<SDL_Surface> RightUpperCorner;
		std::shared_ptr<SDL_Surface> LeftLowerCorner;
		std::shared_ptr<SDL_Surface> RightLowerCorner;
		std::shared_ptr<SDL_Surface> hborder;
		std::shared_ptr<SDL_Surface> vborder;
	} Frame[NUM_DECORATIONFRAMES];

	std::shared_ptr<SDL_Surface> Background;
	std::shared_ptr<SDL_Surface> GameStatsBackground;
	std::shared_ptr<SDL_Surface> CreditsBorder;

	std::shared_ptr<SDL_Surface> HarkonnenLogo;
	std::shared_ptr<SDL_Surface> AtreidesLogo;
	std::shared_ptr<SDL_Surface> OrdosLogo;

	std::shared_ptr<SDL_Surface> MessageBoxBorder;
};

#endif // PICTUREFACTORY_H
