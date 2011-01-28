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

#ifndef MAPEDITOR_H
#define MAPEDITOR_H

#include <GUI/CallbackTarget.h>

#include <data.h>

#include <SDL.h>

struct TerrainTile {
	int		Type;
	int		Tile;
	int		Spice;
};

class MapEditor : public CallbackTarget {
public:
	MapEditor();
	~MapEditor();

	void RunEditor();

private:
	void DrawScreen();
	void ProcessInput();
	void DrawCursor();
	void InitMap(int x, int y);
	void DeInitMap();
	void DrawMap();
	void InitButtons();

	void TypeBtnCallback(void* i);

private:
	SDL_Surface*	pGameBarSurface;
	SDL_Rect		GameBarPos;
	SDL_Surface*	pTopBarSurface;
	SDL_Rect		TopBarPos;

	SDL_Surface*	TerrainSprite;

	bool			QuitEditor;

	bool			MoveDownMode, MoveLeftMode, MoveRightMode, MoveUpMode;
	bool			shift;

	SDL_Rect		ScreenWorldCoords;

	int				MapSizeX, MapSizeY;

	TerrainTile**	Tiles;

//	Window_old			EditorButtonInterface;
//	VBox_old			VBox_RightPanel;
//	VBox_old			VBox_Type;
//	Button_old			BtnTerrain;
//	Button_old			BtnStructures;
//	Button_old			BtnUnits;

};

#endif // MAPEDITOR_H
