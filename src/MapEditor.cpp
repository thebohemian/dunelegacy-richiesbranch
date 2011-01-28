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

#include <MapEditor.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>


MapEditor::MapEditor() {
	QuitEditor = false;
	MoveDownMode = MoveLeftMode = MoveRightMode = MoveUpMode = false;
	shift = false;

	// Load GameBar
	pGameBarSurface = pGFXManager->getUIGraphic(UI_GameBar);
	GameBarPos.w = pGameBarSurface->w;
	GameBarPos.h = pGameBarSurface->h;
	GameBarPos.x = screen->w - GameBarPos.w;
	GameBarPos.y = 0;

	// Load TopBar
	pTopBarSurface =  pGFXManager->getUIGraphic(UI_TopBar);
	TopBarPos.w = pTopBarSurface->w;
	TopBarPos.h = pTopBarSurface->h;
	TopBarPos.x = TopBarPos.y = 0;

	// Load Terrain Surface
	TerrainSprite = pGFXManager->getObjPic(ObjPic_Terrain);

	ScreenWorldCoords.x = 0;
	ScreenWorldCoords.y = 0;
	ScreenWorldCoords.w = screen->w - GameBarPos.w;
	ScreenWorldCoords.h = screen->h - TopBarPos.h;

	Tiles = NULL;
	MapSizeX = MapSizeY = 0;

	InitMap(64,64);

	InitButtons();
}

MapEditor::~MapEditor() {
}

void MapEditor::RunEditor() {
	while(!QuitEditor) {
		ProcessInput();
		DrawScreen();
	}
}

void MapEditor::DrawScreen() {
	// clear whole screen
	SDL_FillRect(screen,NULL,0);

	//the actuall map
	DrawMap();

	// Draw Gamebar
	SDL_BlitSurface(pGameBarSurface, NULL, screen, &GameBarPos);

	// Draw Topbar
	SDL_BlitSurface(pTopBarSurface, NULL, screen, &TopBarPos);

	// Draw Buttons
//	EditorButtonInterface.draw();

	// Cursor
	DrawCursor();

	SDL_Flip(screen);
}

void MapEditor::ProcessInput() {
	SDL_Event event;

	while(SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_KEYDOWN:
			{
				switch(event.key.keysym.sym) {
					case SDLK_DOWN:
						MoveDownMode = true;
						break;
					case SDLK_LEFT:
						MoveLeftMode = true;
						break;
					case SDLK_LSHIFT:
						shift = true;
						break;
					case SDLK_RIGHT:
						MoveRightMode = true;
						break;
					case SDLK_UP:
						MoveUpMode = true;
						break;
					default:
						break;
				}

			} break;

			case SDL_KEYUP:
			{
				switch(event.key.keysym.sym) {
					case SDLK_DOWN:
						MoveDownMode = false;
						break;
					case SDLK_LEFT:
						MoveLeftMode = false;
						break;
					case SDLK_LSHIFT:
						shift = false;
						break;
					case SDLK_RIGHT:
						MoveRightMode = false;
						break;
					case SDLK_UP:
						MoveUpMode = false;
						break;
					case SDLK_ESCAPE:
						// quiting
						QuitEditor = true;
						break;
					default:
						break;
				}
			} break;

			case SDL_MOUSEMOTION:
			{
				MoveDownMode =  (event.motion.y >= screen->h-1);	//if user is trying to move view down
				MoveLeftMode = (event.motion.x <= 0);				//if user is trying to move view left
				MoveRightMode = (event.motion.x >= screen->w-1);	//if user is trying to move view right
				MoveUpMode = (event.motion.y <= 0);					//if user is trying to move view up

				drawnMouseX = event.motion.x;
				drawnMouseY = event.motion.y;

//				EditorButtonInterface.handleMotion(drawnMouseX,drawnMouseY);
			} break;

			case SDL_MOUSEBUTTONDOWN:
			{
				switch (event.button.button) {
					case SDL_BUTTON_LEFT:
					{
//						EditorButtonInterface.handlePress(drawnMouseX,drawnMouseY);
					} break;
					case SDL_BUTTON_RIGHT:
					{
//						EditorButtonInterface.handleCancelPress(drawnMouseX,drawnMouseY);
					} break;
				}
			} break;

			case SDL_MOUSEBUTTONUP:
			{
				switch (event.button.button) {
					case SDL_BUTTON_LEFT:
					{
//						EditorButtonInterface.handleRelease(drawnMouseX,drawnMouseY);
					} break;
					case SDL_BUTTON_RIGHT:
					{
//						EditorButtonInterface.handleCancelRelease(drawnMouseX,drawnMouseY);
					} break;
				}
			} break;
		}
	}

	// Update Screen Coords
	if(MoveDownMode) {
		ScreenWorldCoords.y++;
		cursorFrame = UI_CursorDown;
	}
	if(MoveLeftMode) {
		ScreenWorldCoords.x--;
		cursorFrame = UI_CursorLeft;
	}
	if(MoveRightMode) {
		ScreenWorldCoords.x++;
		cursorFrame = UI_CursorRight;
	}
	if(MoveUpMode) {
		ScreenWorldCoords.y--;
		cursorFrame = UI_CursorUp;
	}

	if(!MoveDownMode && !MoveLeftMode && !MoveRightMode && !MoveUpMode) {
		cursorFrame = UI_CursorNormal;
	}

	// Check Screen Coords for valid values
	if(ScreenWorldCoords.x < 0)
		ScreenWorldCoords.x = 0;
	if(ScreenWorldCoords.y < 0)
		ScreenWorldCoords.y = 0;
	if(ScreenWorldCoords.x + ScreenWorldCoords.w >= MapSizeX*BLOCKSIZE)
		ScreenWorldCoords.x = MapSizeX*BLOCKSIZE - ScreenWorldCoords.w;
	if(ScreenWorldCoords.y + ScreenWorldCoords.h >= MapSizeY*BLOCKSIZE)
		ScreenWorldCoords.y = MapSizeY*BLOCKSIZE - ScreenWorldCoords.h;
}

void MapEditor::DrawCursor() {
	SDL_Rect dest;

	SDL_Surface* surface = pGFXManager->getUIGraphic(cursorFrame);

	dest.x = drawnMouseX;
	dest.y = drawnMouseY;
	dest.w = surface->w;
	dest.h = surface->h;

	//reposition image so pointing on right spot

	if (cursorFrame == UI_CursorRight) {
		dest.x -= dest.w/2;
	} else if (cursorFrame == UI_CursorDown) {
		dest.y -= dest.h/2;
	}

	if ((cursorFrame == UI_CursorAttack) || (cursorFrame == UI_CursorMove)) {
		dest.x -= dest.w/2;
		dest.y -= dest.h/2;
	}

	if(SDL_BlitSurface(surface, NULL, screen, &dest) != 0) {
        fprintf(stderr,"MapEditor::drawCursor(): %s\n", SDL_GetError());
	}
}

void MapEditor::InitMap(int x, int y) {
	DeInitMap();

	MapSizeX = x;
	MapSizeY = y;

	Tiles = new TerrainTile*[x];
	if(Tiles == NULL) {
		fprintf(stderr,"MapEditor::InitMap(): Cannot allocate memory!\n");
		exit(EXIT_FAILURE);
	}

	for(int i = 0; i < MapSizeX; i++) {
		Tiles[i] = new TerrainTile[y];
		if(Tiles[i] == NULL) {
			fprintf(stderr,"MapEditor::InitMap(): Cannot allocate memory!\n");
			exit(EXIT_FAILURE);
		}
	}

	for(int x = 0; x < MapSizeX; x++) {
		for(int y = 0; y < MapSizeY ; y++) {
			Tiles[x][y].Type = Terrain_Sand;
			Tiles[x][y].Tile = 4;
			Tiles[x][y].Spice = 0;
		}
	}

	//Test
	Tiles[0][0].Tile = 3;
	Tiles[MapSizeX-1][0].Tile = 3;
	Tiles[0][MapSizeY-1].Tile = 3;
	Tiles[MapSizeX-1][MapSizeY-1].Tile = 3;
}

void MapEditor::DeInitMap() {
	if(Tiles != NULL) {
		for(int i = 0; i < MapSizeX; i++) {
			if(Tiles[i] != NULL)
				delete [] Tiles[i];
		}
		delete Tiles;
	}
	Tiles = NULL;
	MapSizeX = MapSizeY = 0;
}

void MapEditor::DrawMap() {
	SDL_Rect	source;
	SDL_Rect	drawLocation;

	source.y = 0;
	source.w = source.h = BLOCKSIZE;
	drawLocation.w = drawLocation.h = BLOCKSIZE;


	SDL_Rect	TilesWindow;
	TilesWindow.x = ScreenWorldCoords.x / BLOCKSIZE;
	TilesWindow.y = ScreenWorldCoords.y / BLOCKSIZE;
	TilesWindow.w = (ScreenWorldCoords.w / BLOCKSIZE)+2;
	TilesWindow.h = (ScreenWorldCoords.h / BLOCKSIZE)+2;

	if(TilesWindow.x + TilesWindow.w >= MapSizeX)
		TilesWindow.w = MapSizeX-TilesWindow.x;
	if(TilesWindow.y + TilesWindow.h >= MapSizeY)
		TilesWindow.h = MapSizeY-TilesWindow.y;

	//draw terrain
	drawLocation.x = (TilesWindow.x * BLOCKSIZE) - ScreenWorldCoords.x;
	for(int x = TilesWindow.x ; x < TilesWindow.x+TilesWindow.w ; x++) {
		for(int y = TilesWindow.y ; y < TilesWindow.y+TilesWindow.h ; y++) {
			//draw Tiles[x][y]
			source.x = Tiles[x][y].Tile*BLOCKSIZE;
			drawLocation.x = (x * BLOCKSIZE) - ScreenWorldCoords.x;
			drawLocation.y = (y * BLOCKSIZE) - ScreenWorldCoords.y + TopBarPos.h;
			SDL_BlitSurface(TerrainSprite, &source, screen, &drawLocation);
		}
	}
}

void MapEditor::InitButtons() {
/*	EditorButtonInterface.setPos(0,0);
	EditorButtonInterface.setSize(screen->w,screen->h);

	VBox_RightPanel.setPos(GameBarPos.x+15,GameBarPos.y+180);
	VBox_RightPanel.setSize(GameBarPos.w-20,GameBarPos.h-190);
	EditorButtonInterface.addChild(&VBox_RightPanel);
	VBox_RightPanel.addChild(&VBox_Type);

	BtnTerrain.setText("Terrain");
	BtnTerrain.setToggler(true);
	BtnTerrain.setCallBack(this,(WidgetCallbackWithParameter) (&MapEditor::TypeBtnCallback),(void*) 0);
	VBox_Type.addChild(&BtnTerrain);

	BtnUnits.setText("Units");
	BtnUnits.setToggler(true);
	BtnUnits.setCallBack(this,(WidgetCallbackWithParameter) (&MapEditor::TypeBtnCallback),(void*)1);
	VBox_Type.addChild(&BtnUnits);

	BtnStructures.setText("Structures");
	BtnStructures.setToggler(true);
	BtnStructures.setCallBack(this,(WidgetCallbackWithParameter) (&MapEditor::TypeBtnCallback),(void*)2);
	VBox_Type.addChild(&BtnStructures);
*/
}

void MapEditor::TypeBtnCallback(void* i) {
	int j = (long) i;
	switch(j) {
		// Terrain Button
		case 0:
		{
//			BtnUnits.setToggled(false);
//			BtnStructures.setToggled(false);
		} break;

		// Unit Button
		case 1:
		{
//			BtnTerrain.setToggled(false);
//			BtnStructures.setToggled(false);
		} break;

		// Structure Button
		case 2:
		{
//			BtnUnits.setToggled(false);
//			BtnTerrain.setToggled(false);
		} break;
	}
}
