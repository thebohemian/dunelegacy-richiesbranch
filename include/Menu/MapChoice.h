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

#ifndef MAPCHOICE_H
#define MAPCHOICE_H

#include "MenuClass.h"
#include <DataTypes.h>
#include <GUI/dune/MessageTicker.h>
#include <misc/draw_util.h>
#include <misc/BlendBlitter.h>
#include <mmath.h>
#include <vector>
#include <SDL.h>

#define MAPCHOICESTATE_BLENDING		0
#define MAPCHOICESTATE_FASTBLENDING	1
#define MAPCHOICESTATE_ARROWS		2
#define MAPCHOICESTATE_BLINKING		3

class MapChoice : public MenuClass
{
public:
	MapChoice(int newHouse, unsigned int LastMission);
	virtual ~MapChoice();

	virtual int showMenu();

	void DrawSpecificStuff();
	bool doInput(SDL_Event &event);

private:
	void PrepareMapSurface();
	void LoadINI();

private:
	struct TGroup {
		std::vector<int> newRegion[NUM_HOUSES];

		struct TAttackRegion {
			int RegionNum;
			int ArrowNum;
			Coord ArrowPosition;
		} AttackRegion[4];

		struct TText {
			std::string Message;
			int Region;		//< when this region is changed, this message will appear.
		};

		std::vector<TText> Text;
	} Group[9];

	int house;
	unsigned int LastScenario;
	SDL_Surface* MapSurface;
	Coord PiecePosition[28];
	BlendBlitter* curBlendBlitter;
	unsigned int curHouse2Blit;
	unsigned int curRegion2Blit;
	int MapChoiceState;
	int selectedRegion;
	Uint32	selectionTime;
	MessageTicker  msgticker;

	SDL_Rect CenterArea;
};

#endif //MAPCHOICE_H
