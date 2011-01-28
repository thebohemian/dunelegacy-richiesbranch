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

#include <Game.h>

#include <globals.h>
#include <config.h>

#include <FileClasses/FileManager.h>
#include <FileClasses/GFXManager.h>
#include <FileClasses/FontManager.h>
#include <FileClasses/TextManager.h>
#include <FileClasses/music/MusicPlayer.h>
#include <SoundPlayer.h>
#include <misc/FileStream.h>
#include <misc/fnkdat.h>
#include <misc/draw_util.h>
#include <misc/string_util.h>
#include <misc/md5.h>

#include <RadarView.h>
#include <GUI/dune/InGameMenu.h>
#include <Menu/MentatHelp.h>
#include <Menu/BriefingMenu.h>
#include <Menu/MapChoice.h>

#include <House.h>
#include <MapClass.h>
#include <BulletClass.h>
#include <Explosion.h>
#include <GameInitSettings.h>
#include <ScreenBorder.h>
#include <sand.h>

#include <structures/StructureClass.h>
#include <units/UnitClass.h>
#include <structures/BuilderClass.h>
#include <structures/PalaceClass.h>
#include <units/HarvesterClass.h>
#include <units/InfantryClass.h>

#include <sstream>
#include <iomanip>
#include <SDL.h>


Game::Game() {
	whatNextParam = GAME_NOTHING;

	finished = false;

	gameType = GAMETYPE_CAMPAIGN;
	techLevel = 0;

    currentCursorMode = CursorMode_Normal;

	chatMode = false;
	scrollDownMode = false;
	scrollLeftMode = false;
	scrollRightMode = false;
	scrollUpMode = false;

	bShowFPS = false;
	bShowTime = false;

	bCheatsEnabled = false;

	radarMode = false;
	selectionMode = false;

	bQuitGame = false;

	bReplay = false;

	indicatorFrame = NONE;
	indicatorTime = 5;
	indicatorTimer = 0;

    pInterface = NULL;
	pInGameMenu = NULL;
	pInGameMentat = NULL;

	unitList.clear();   	//holds all the units
	structureList.clear();	//all the structures
	bulletList.clear();

	for(int i=0;i<MAX_PLAYERS;i++) {
		house[i] = NULL;
	}

	gamespeed = GAMESPEED_DEFAULT;

	SDL_Surface* pGamebarSurface = pGFXManager->getUIGraphic(UI_GameBar);
	gameBarPos.w = pGamebarSurface->w;
	gameBarPos.h = pGamebarSurface->h;
	gameBarPos.x = screen->w - gameBarPos.w;
	gameBarPos.y = 0;

	SDL_Surface* pTopBarSurface = pGFXManager->getUIGraphic(UI_TopBar);
	topBarPos.w = pTopBarSurface->w;
	topBarPos.h = pTopBarSurface->h;
	topBarPos.x = 0;
	topBarPos.y = 0;

    gameState = START;

	GameCycleCount = 0;
	SkipToGameCycle = 0;

	fps = 0;
	debug = false;

	powerIndicatorPos.x = 14;
	powerIndicatorPos.y = 146;
	spiceIndicatorPos.x = 20;
	spiceIndicatorPos.y = 146;
	powerIndicatorPos.w = spiceIndicatorPos.w = 4;
	powerIndicatorPos.h = spiceIndicatorPos.h = settings.Video.Height - 146 - 2;

	musicPlayer->changeMusic(MUSIC_PEACE);
	//////////////////////////////////////////////////////////////////////////
	SDL_Rect gameBoardRect = { 0, topBarPos.h, gameBarPos.x, screen->h - topBarPos.h };
	screenborder = new ScreenBorder(gameBoardRect);
	radarView = new RadarView(gameBarPos.x, gameBarPos.y);
}


/**
    The destructor frees up all the used memory.
*/
Game::~Game() {
    delete pInGameMenu;
    pInGameMenu = NULL;

    delete pInterface;
    pInterface = NULL;

    for(RobustList<StructureClass*>::const_iterator iter = structureList.begin(); iter != structureList.end(); ++iter) {
        delete *iter;
    }
    structureList.clear();

    for(RobustList<UnitClass*>::const_iterator iter = unitList.begin(); iter != unitList.end(); ++iter) {
        delete *iter;
    }
    unitList.clear();

	for(RobustList<BulletClass*>::const_iterator iter = bulletList.begin(); iter != bulletList.end(); ++iter) {
	    delete *iter;
	}
	bulletList.clear();

    for(RobustList<Explosion*>::const_iterator iter = explosionList.begin(); iter != explosionList.end(); ++iter) {
	    delete *iter;
	}
	explosionList.clear();

	for(int i=0;i<MAX_PLAYERS;i++) {
		delete house[i];
		house[i] = NULL;
	}

	delete currentGameMap;
	currentGameMap = NULL;
	delete screenborder;
	screenborder = NULL;
	delete radarView;
	radarView = NULL;
}


void Game::initGame(const GameInitSettings& newGameInitSettings) {
    gameInitSettings = newGameInitSettings;

    switch(gameInitSettings.getGameType()) {
        case GAMETYPE_LOAD_SAVEGAME: {
            if(loadSaveGame(gameInitSettings.getFilename()) == false) {
                throw std::runtime_error("Loading save game failed!");
            }
        } break;

        case GAMETYPE_CAMPAIGN:
        case GAMETYPE_SKIRMISH:
        case GAMETYPE_CUSTOM: {
            gameType = gameInitSettings.getGameType();
            RandomGen.setSeed(gameInitSettings.getRandomSeed());

            objectData.loadFromINIFile("ObjectData.ini");

            if(gameInitSettings.getMission() != 0) {
                techLevel = ((gameInitSettings.getMission() + 1)/3) + 1 ;
            }

            GameINILoader* pGameINILoader = new GameINILoader(this, gameInitSettings.getFilename());
            delete pGameINILoader;

            if(bReplay == false && gameInitSettings.getGameType() != GAMETYPE_CUSTOM) {
                /* do briefing */
                fprintf(stdout,"Briefing...");
                fflush(stdout);
                BriefingMenu* pBriefing = new BriefingMenu(gameInitSettings.getHouseID(), gameInitSettings.getMission(),BRIEFING);
                pBriefing->showMenu();
                delete pBriefing;

                fprintf(stdout,"\t\t\tfinished\n");
                fflush(stdout);
            }
        } break;

        default: {
        } break;
    }
}

void Game::initReplay(const std::string& filename) {
    bReplay = true;

	FileStream fs;

	if(fs.open(filename, "rb") == false) {
		perror("Game::loadSaveGame()");
		exit(EXIT_FAILURE);
	}

	// read GameInitInfo
	GameInitSettings loadedGameInitSettings(fs);

	// load all commands
	CmdManager.load(fs);

	initGame(loadedGameInitSettings);

	// fs is closed by its destructor
}



/**
	This method processes all objects in the current game. It should be executed exactly once per game tick.
*/
void Game::ProcessObjects()
{
	// Update the windtrap palette animation
	pGFXManager->DoWindTrapPalatteAnimation();

	// update all tiles
    for(int y = 0; y < currentGameMap->sizeY; y++) {
		for(int x = 0; x < currentGameMap->sizeX; x++) {
            currentGameMap->cell[x][y].update();
		}
	}


    for(RobustList<StructureClass*>::iterator iter = structureList.begin(); iter != structureList.end(); ++iter) {
        StructureClass* tempStructure = *iter;
        tempStructure->update();
    }

	if ((currentCursorMode == CursorMode_Placing) && selectedList.empty()) {
		currentCursorMode = CursorMode_Normal;
	}

	for(RobustList<UnitClass*>::iterator iter = unitList.begin(); iter != unitList.end(); ++iter) {
		UnitClass* tempUnit = *iter;
		tempUnit->update();
	}

    for(RobustList<BulletClass*>::iterator iter = bulletList.begin(); iter != bulletList.end(); ++iter) {
        (*iter)->update();
	}

    for(RobustList<Explosion*>::iterator iter = explosionList.begin(); iter != explosionList.end(); ++iter) {
        (*iter)->update();
	}
}

/**
	This method draws a complete frame.
*/
void Game::drawScreen()
{
	SDL_Rect	source,	drawLocation;

	/* clear whole screen */
	SDL_FillRect(screen,NULL,0);

    Coord TopLeftTile = screenborder->getTopLeftTile();
    Coord BottomRightTile = screenborder->getBottomRightTile();

    // extend the view a little bit to avoid graphical glitches
    TopLeftTile.x = std::max(0, TopLeftTile.x - 1);
    TopLeftTile.y = std::max(0, TopLeftTile.y - 1);
    BottomRightTile.x = std::min(currentGameMap->sizeX-1, BottomRightTile.x + 1);
    BottomRightTile.y = std::min(currentGameMap->sizeY-1, BottomRightTile.y + 1);

    Coord currentTile;

    /* draw ground */
	for(currentTile.y = TopLeftTile.y; currentTile.y <= BottomRightTile.y; currentTile.y++) {
		for(currentTile.x = TopLeftTile.x; currentTile.x <= BottomRightTile.x; currentTile.x++) {

			if (currentGameMap->cellExists(currentTile))	{
				TerrainClass* cell = &currentGameMap->cell[currentTile.x][currentTile.y];

//				if(debug || cell->isExplored(pLocalHouse->getPlayerNumber())) {
					cell->blitGround( screenborder->world2screenX(currentTile.x*BLOCKSIZE),
                                      screenborder->world2screenY(currentTile.y*BLOCKSIZE));
//				}
			}
		}
	}

    /* draw structures */
	for(currentTile.y = TopLeftTile.y; currentTile.y <= BottomRightTile.y; currentTile.y++) {
		for(currentTile.x = TopLeftTile.x; currentTile.x <= BottomRightTile.x; currentTile.x++) {

			if (currentGameMap->cellExists(currentTile))	{
				TerrainClass* cell = &currentGameMap->cell[currentTile.x][currentTile.y];

//				if(debug || cell->isExplored(pLocalHouse->getPlayerNumber())) {
					cell->blitStructures( screenborder->world2screenX(currentTile.x*BLOCKSIZE),
                                          screenborder->world2screenY(currentTile.y*BLOCKSIZE));
//				}
			}
		}
	}

    /* draw underground units */
	for(currentTile.y = TopLeftTile.y; currentTile.y <= BottomRightTile.y; currentTile.y++) {
		for(currentTile.x = TopLeftTile.x; currentTile.x <= BottomRightTile.x; currentTile.x++) {

			if (currentGameMap->cellExists(currentTile))	{
				TerrainClass* cell = &currentGameMap->cell[currentTile.x][currentTile.y];

//				if(debug || cell->isExplored(pLocalHouse->getPlayerNumber())) {
					cell->blitUndergroundUnits( screenborder->world2screenX(currentTile.x*BLOCKSIZE),
                                                screenborder->world2screenY(currentTile.y*BLOCKSIZE));
//				}
			}
		}
	}

    /* draw dead objects */
	for(currentTile.y = TopLeftTile.y; currentTile.y <= BottomRightTile.y; currentTile.y++) {
		for(currentTile.x = TopLeftTile.x; currentTile.x <= BottomRightTile.x; currentTile.x++) {

			if (currentGameMap->cellExists(currentTile))	{
				TerrainClass* cell = &currentGameMap->cell[currentTile.x][currentTile.y];

//				if(debug || cell->isExplored(pLocalHouse->getPlayerNumber())) {
					cell->blitDeadUnits( screenborder->world2screenX(currentTile.x*BLOCKSIZE),
                                         screenborder->world2screenY(currentTile.y*BLOCKSIZE));
//				}
			}
		}
	}

    /* draw infantry */
	for(currentTile.y = TopLeftTile.y; currentTile.y <= BottomRightTile.y; currentTile.y++) {
		for(currentTile.x = TopLeftTile.x; currentTile.x <= BottomRightTile.x; currentTile.x++) {

			if (currentGameMap->cellExists(currentTile))	{
				TerrainClass* cell = &currentGameMap->cell[currentTile.x][currentTile.y];

//				if(debug || cell->isExplored(pLocalHouse->getPlayerNumber())) {
					cell->blitInfantry( screenborder->world2screenX(currentTile.x*BLOCKSIZE),
                                        screenborder->world2screenY(currentTile.y*BLOCKSIZE));
//				}
			}
		}
	}

    /* draw non-infantry ground units */
	for(currentTile.y = TopLeftTile.y; currentTile.y <= BottomRightTile.y; currentTile.y++) {
		for(currentTile.x = TopLeftTile.x; currentTile.x <= BottomRightTile.x; currentTile.x++) {

			if (currentGameMap->cellExists(currentTile))	{
				TerrainClass* cell = &currentGameMap->cell[currentTile.x][currentTile.y];

//				if(debug || cell->isExplored(pLocalHouse->getPlayerNumber())) {
					cell->blitNonInfantryGroundUnits( screenborder->world2screenX(currentTile.x*BLOCKSIZE),
                                                      screenborder->world2screenY(currentTile.y*BLOCKSIZE));
//				}
			}
		}
	}

	/* draw bullets */
    for(RobustList<BulletClass*>::const_iterator iter = bulletList.begin(); iter != bulletList.end(); ++iter) {
        BulletClass* pBullet = *iter;

		if(screenborder->isInsideScreen( Coord( (int) pBullet->getRealX(), (int) pBullet->getRealY()), pBullet->getImageSize()))
		{
			if(debug) {
				pBullet->blitToScreen();
			} else {
//				int xpos = (int) (*iter)->getRealX()/BLOCKSIZE;
//				int ypos = (int) (*iter)->getRealY()/BLOCKSIZE;

//				if( (currentGameMap->cellExists(xpos,ypos)) &&
//					(currentGameMap->cell[xpos][ypos].isExplored(pLocalHouse->getPlayerNumber())
//					|| !currentGameMap->cell[xpos][ypos].isFogged(pLocalHouse->getPlayerNumber()))) {
					pBullet->blitToScreen();
//                }
			}
		}
	}


	/* draw explosions */
	for(RobustList<Explosion*>::const_iterator iter = explosionList.begin(); iter != explosionList.end(); ++iter) {
        (*iter)->blitToScreen();
	}

    /* draw air units */
	for(currentTile.y = TopLeftTile.y; currentTile.y <= BottomRightTile.y; currentTile.y++) {
		for(currentTile.x = TopLeftTile.x; currentTile.x <= BottomRightTile.x; currentTile.x++) {

			if (currentGameMap->cellExists(currentTile))	{
				TerrainClass* cell = &currentGameMap->cell[currentTile.x][currentTile.y];

//				if(debug || cell->isExplored(pLocalHouse->getPlayerNumber())) {
					cell->blitAirUnits(   screenborder->world2screenX(currentTile.x*BLOCKSIZE),
                                          screenborder->world2screenY(currentTile.y*BLOCKSIZE));
//				}
			}
		}
	}

    /* draw selection rectangles */
	for(currentTile.y = TopLeftTile.y; currentTile.y <= BottomRightTile.y; currentTile.y++) {
		for(currentTile.x = TopLeftTile.x; currentTile.x <= BottomRightTile.x; currentTile.x++) {

			if (currentGameMap->cellExists(currentTile))	{
				TerrainClass* cell = &currentGameMap->cell[currentTile.x][currentTile.y];

				if(debug || cell->isExplored(pLocalHouse->getHouseID())) {
					cell->blitSelectionRects(   screenborder->world2screenX(currentTile.x*BLOCKSIZE),
                                                screenborder->world2screenY(currentTile.y*BLOCKSIZE));
				}
			}
		}
	}


//////////////////////////////draw unexplored/shade

	if(debug == false) {
		source.y = 0;
		source.w = source.h = drawLocation.w = drawLocation.h = BLOCKSIZE;
		for(int x = screenborder->getTopLeftTile().x - 1; x <= screenborder->getBottomRightTile().x + 1; x++) {
			for (int y = screenborder->getTopLeftTile().y - 1; y <= screenborder->getBottomRightTile().y + 1; y++) {

				if((x >= 0) && (x < currentGameMap->sizeX) && (y >= 0) && (y < currentGameMap->sizeY)) {
					TerrainClass* cell = &currentGameMap->cell[x][y];

					if(cell->isExplored(pLocalHouse->getHouseID())) {
					    int hideTile = cell->getHideTile(pLocalHouse->getHouseID());

						if(hideTile != 0) {
						    SDL_Surface* hiddenSurf = pGFXManager->getObjPic(ObjPic_Terrain_Hidden);

							source.x = hideTile*BLOCKSIZE;
							drawLocation.x = screenborder->world2screenX(x*BLOCKSIZE);
							drawLocation.y = screenborder->world2screenY(y*BLOCKSIZE);
							SDL_BlitSurface(hiddenSurf, &source, screen, &drawLocation);
						}

						if(gameInitSettings.hasFogOfWar() == true) {
                            int fogTile = cell->getFogTile(pLocalHouse->getHouseID());

                            if(cell->isFogged(pLocalHouse->getHouseID()) == true) {
                                fogTile = Terrain_HiddenFull;
                            }

                            if(fogTile != 0) {
                                source.x = fogTile*BLOCKSIZE;
                                drawLocation.x = screenborder->world2screenX(x*BLOCKSIZE);
                                drawLocation.y = screenborder->world2screenY(y*BLOCKSIZE);

                                SDL_Rect mini = {0, 0, 1, 1};
                                SDL_Rect drawLoc = {drawLocation.x, drawLocation.y, 0, 0};

                                SDL_Surface* hiddenSurf = pGFXManager->getObjPic(ObjPic_Terrain_Hidden);
                                SDL_Surface* fogSurf = pGFXManager->getTransparent40Surface();

                                SDL_LockSurface(hiddenSurf);
                                for(int i=0;i<BLOCKSIZE; i++) {
                                    for(int j=0;j<BLOCKSIZE; j++) {
                                        if(getpixel(hiddenSurf,source.x+i,source.y+j) == 12) {
                                            drawLoc.x = drawLocation.x + i;
                                            drawLoc.y = drawLocation.y + j;
                                            SDL_BlitSurface(fogSurf,&mini,screen,&drawLoc);
                                        }
                                    }
                                }
                                SDL_UnlockSurface(hiddenSurf);


                            }
						}
					} else {
					    if(!debug) {
					        SDL_Surface* hiddenSurf = pGFXManager->getObjPic(ObjPic_Terrain_Hidden);
                            source.x = BLOCKSIZE*15;
                            drawLocation.x = screenborder->world2screenX(x*BLOCKSIZE);
                            drawLocation.y = screenborder->world2screenY(y*BLOCKSIZE);
                            SDL_BlitSurface(hiddenSurf, &source, screen, &drawLocation);
					    }
					}
				} else {
                    // we are outside the map => draw complete hidden
                    SDL_Surface* hiddenSurf = pGFXManager->getObjPic(ObjPic_Terrain_Hidden);
                    source.x = BLOCKSIZE*15;
                    drawLocation.x = screenborder->world2screenX(x*BLOCKSIZE);
                    drawLocation.y = screenborder->world2screenY(y*BLOCKSIZE);
                    SDL_BlitSurface(hiddenSurf, &source, screen, &drawLocation);
				}
			}
		}
	}

/////////////draw placement position

	int mouse_x, mouse_y;

	SDL_GetMouseState(&mouse_x, &mouse_y);

	if(currentCursorMode == CursorMode_Placing) {
		//if user has selected to place a structure

		if(screenborder->isScreenCoordInsideMap(mouse_x, mouse_y)) {
		    //if mouse is not over game bar

			int	xPos = screenborder->screen2MapX(mouse_x);
			int yPos = screenborder->screen2MapY(mouse_y);

			bool withinRange = false;

			BuilderClass* builder = NULL;
			if(selectedList.size() == 1) {
			    builder = dynamic_cast<BuilderClass*>(objectManager.getObject(*selectedList.begin()));

                int placeItem = builder->GetCurrentProducedItem();
                Coord structuresize = getStructureSize(placeItem);

                SDL_Surface	*image,
                            *validPlace = pGFXManager->getUIGraphic(UI_ValidPlace),
                            *invalidPlace = pGFXManager->getUIGraphic(UI_InvalidPlace);

                for (int i = xPos; i < (xPos + structuresize.x); i++) {
                    for (int j = yPos; j < (yPos + structuresize.y); j++) {
                        if (currentGameMap->isWithinBuildRange(i, j, builder->getOwner())) {
                            withinRange = true;			//find out if the structure is close enough to other buildings
                        }
                    }
                }

                for (int i = xPos; i < (xPos + structuresize.x); i++) {
                    for (int j = yPos; j < (yPos + structuresize.y); j++) {
                        if(!withinRange || !currentGameMap->cellExists(i,j) || !currentGameMap->getCell(i,j)->isRock()
                            || currentGameMap->getCell(i,j)->isMountain() || currentGameMap->getCell(i,j)->hasAGroundObject()
                            || (((placeItem == Structure_Slab1) || (placeItem == Structure_Slab4)) && currentGameMap->getCell(i,j)->isConcrete())) {
                            image = invalidPlace;
                        } else {
                            image = validPlace;
                        }

                        drawLocation.x = screenborder->world2screenX(i*BLOCKSIZE);
                        drawLocation.y = screenborder->world2screenY(j*BLOCKSIZE);

                        SDL_BlitSurface(image, NULL, screen, &drawLocation);
                    }
                }
            }
		}
	}

///////////draw game selection rectangle
	if (selectionMode) {

		int finalMouseX = mouse_x;
		if(finalMouseX >= gameBarPos.x) {
		    //this keeps the box on the map, and not over game bar
			finalMouseX = gameBarPos.x-1;
		}

        // draw the mouse selection rectangle
		drawrect( screen,
                  screenborder->world2screenX(selectionRect.x),
                  screenborder->world2screenY(selectionRect.y),
                  finalMouseX,
                  mouse_y,
                  COLOR_WHITE);
	}



///////////draw action indicator

	if ((indicatorFrame != NONE) && (screenborder->isInsideScreen(indicatorPosition, Coord(BLOCKSIZE,BLOCKSIZE)) == true)) {
		source.w = drawLocation.w = pGFXManager->getUIGraphic(UI_Indicator)->w/3;
		source.h = drawLocation.h = pGFXManager->getUIGraphic(UI_Indicator)->h;
		source.x = indicatorFrame*source.w;
		source.y = 0;
		drawLocation.x = screenborder->world2screenX(indicatorPosition.x) - source.w/2;
		drawLocation.y = screenborder->world2screenY(indicatorPosition.y) - source.h/2;
		SDL_BlitSurface(pGFXManager->getUIGraphic(UI_Indicator), &source, screen, &drawLocation);
	}


///////////draw game bar
	pInterface->Draw(screen, Point(0,0));
	pInterface->DrawOverlay(screen, Point(0,0));

	SDL_Surface* surface;

/////////draw radar
    radarView->draw();

	// draw chat message currently typed
	if(chatMode) {
        surface = pFontManager->createSurfaceWithText("Chat: " + typingChatMessage + (((SDL_GetTicks() / 150) % 2 == 0) ? "_" : ""), COLOR_WHITE, FONT_STD12);
        drawLocation.x = 20;
        drawLocation.y = screen->h - 40;
        SDL_BlitSurface(surface, NULL, screen, &drawLocation);
        SDL_FreeSurface(surface);
	}

	if(bShowFPS) {
		char	temp[50];
		snprintf(temp,50,"fps: %.2f ",fps);

		SDL_Surface* fpsSurface = pFontManager->createSurfaceWithText(temp, COLOR_WHITE, FONT_STD12);

		drawLocation.x = screen->w - fpsSurface->w;
		drawLocation.y = 0;
		SDL_BlitSurface(fpsSurface, NULL, screen, &drawLocation);
		SDL_FreeSurface(fpsSurface);
	}

	if(bShowTime) {
		char	temp[50];
		int     seconds = GetGameTime() / 1000;
		snprintf(temp,50," %.2d:%.2d:%.2d", seconds / 3600, (seconds % 3600)/60, (seconds % 60) );

		SDL_Surface* timeSurface = pFontManager->createSurfaceWithText(temp, COLOR_WHITE, FONT_STD12);

		drawLocation.x = 0;
		drawLocation.y = screen->h - timeSurface->h;
		SDL_BlitSurface(timeSurface, NULL, screen, &drawLocation);
		SDL_FreeSurface(timeSurface);
	}

	//show ingame menu
	if (bPause || finished) {
		std::string message;

		if(finished) {
			if(won) {
				message = pTextManager->getLocalized("You Have Completed Your Mission.");
			} else {
				message = pTextManager->getLocalized("You Have Failed Your Mission.");
			}
		} else {
			message = pTextManager->getLocalized("Paused");
		}

		surface = pFontManager->createSurfaceWithText(message.c_str(), COLOR_WHITE, FONT_STD24);

		drawLocation.x = screen->w/2 - surface->w/2;
		drawLocation.y = screen->h/2 - surface->h/2;
		drawLocation.w = surface->w;
		drawLocation.h = surface->h;

		SDL_BlitSurface(surface, NULL, screen, &drawLocation);
		SDL_FreeSurface(surface);
	}

	if(pInGameMenu != NULL) {
		pInGameMenu->Draw(screen);
	} else if(pInGameMentat != NULL) {
		pInGameMentat->draw();
	}

	drawCursor();
}

/**
	This method proccesses all the user input.
*/
void Game::doInput()
{
	ObjectClass* tempObject;
	SDL_Event event;
	while(SDL_PollEvent(&event)) {
	    // check for a key press

		// first of all update mouse
		if(event.type == SDL_MOUSEMOTION) {
			SDL_MouseMotionEvent* mouse = &event.motion;
			drawnMouseX = mouse->x;
			drawnMouseY = mouse->y;
		}

		if(pInGameMenu != NULL) {
			pInGameMenu->HandleInput(event);

			if(bPause == false) {
                delete pInGameMenu;
                pInGameMenu = NULL;
			}

		} else if(pInGameMentat != NULL) {
			pInGameMentat->doInput(event);

			if(bPause == false) {
                delete pInGameMentat;
                pInGameMentat = NULL;
			}

		} else {
			/* Look for a keypress */
			switch (event.type) {

                case (SDL_KEYDOWN): {
                    if(chatMode) {
                        if(event.key.keysym.sym == SDLK_ESCAPE) {
                            chatMode = false;
                        } else if(event.key.keysym.sym == SDLK_RETURN) {
                            if(typingChatMessage.length() > 0) {
                                unsigned char md5sum[16];

                                md5((const unsigned char*) typingChatMessage.c_str(), typingChatMessage.size(), md5sum);

                                std::stringstream md5stream;
                                md5stream << std::setfill('0') << std::hex << std::uppercase << "0x";
                                for(int i=0;i<16;i++) {
                                    md5stream << std::setw(2) << (int) md5sum[i];
                                }

                                std::string md5string = md5stream.str();

                                if((bCheatsEnabled == false) && (md5string == "0xB8766C8EC7A61036B69893FC17AAF21E")) {
                                    bCheatsEnabled = true;
                                    pInterface->getChatManager().addInfoMessage("Cheat mode enabled");
                                } else if((bCheatsEnabled == true) && (md5string == "0xB8766C8EC7A61036B69893FC17AAF21E")) {
                                    pInterface->getChatManager().addInfoMessage("Cheat mode already enabled");
                                } else if((bCheatsEnabled == true) && (md5string == "0x57583291CB37F8167EDB0611D8D19E58")) {
                                    if (gameType != GAMETYPE_CUSTOM_MULTIPLAYER) {
                                        pInterface->getChatManager().addInfoMessage("You win this game");
                                        setGameWon();
                                    }
                                } else if((bCheatsEnabled == true) && (md5string == "0x1A12BE3DBE54C5A504CAA6EE9782C1C8")) {
                                    if(debug == true) {
                                        pInterface->getChatManager().addInfoMessage("You are already in debug mode");
                                    } else if (gameType != GAMETYPE_CUSTOM_MULTIPLAYER) {
                                        pInterface->getChatManager().addInfoMessage("Debug mode enabled");
                                        debug = true;
                                    }
                                } else if((bCheatsEnabled == true) && (md5string == "0x54F68155FC64A5BC66DCD50C1E925C0B")) {
                                    if(debug == false) {
                                        pInterface->getChatManager().addInfoMessage("You are not in debug mode");
                                    } else if (gameType != GAMETYPE_CUSTOM_MULTIPLAYER) {
                                        pInterface->getChatManager().addInfoMessage("Debug mode disabled");
                                        debug = false;
                                    }
                                } else if((bCheatsEnabled == true) && (md5string == "0xCEF1D26CE4B145DE985503CA35232ED8")) {
                                    if (gameType != GAMETYPE_CUSTOM_MULTIPLAYER) {
                                        pInterface->getChatManager().addInfoMessage("You got some credits");
                                        pLocalHouse->returnCredits(10000.0);
                                    }
                                } else {
                                    pInterface->getChatManager().addChatMessage(typingChatMessage, settings.General.PlayerName);
                                }
                            }

                            chatMode = false;
                        } else if(event.key.keysym.sym == SDLK_BACKSPACE) {
                            if(typingChatMessage.length() > 0) {
                                typingChatMessage.resize(typingChatMessage.length() - 1);
                            }
                        } else if(typingChatMessage.length() < 60)	{
                            if((event.key.keysym.unicode <= 0xFF) && (event.key.keysym.unicode > 0)) {
                                typingChatMessage += event.key.keysym.unicode;
                            } else {
                                // TODO
                            }
                        }

                    } else {

                        switch( event.key.keysym.sym ) {
                            case SDLK_0: {
                                //if ctrl and 0 remove selected units from all groups
                                if(SDL_GetModState() & KMOD_CTRL) {
                                    std::set<Uint32>::iterator iter;
                                    for(iter = selectedList.begin(); iter != selectedList.end(); ++iter) {
                                        ObjectClass* object = objectManager.getObject(*iter);
                                        object->setSelected(false);
                                        object->removeFromSelectionLists();
                                    }
                                    selectedList.clear();
                                    currentCursorMode = CursorMode_Normal;
                                } else {
                                    std::set<Uint32>::iterator iter;
                                    for(iter = selectedList.begin(); iter != selectedList.end(); ++iter) {
                                        ObjectClass* object = objectManager.getObject(*iter);
                                        object->setSelected(false);
                                    }
                                    selectedList.clear();
                                    currentCursorMode = CursorMode_Normal;
                                }
                                pInterface->UpdateObjectInterface();
                            } break;

                            case SDLK_1:
                            case SDLK_2:
                            case SDLK_3:
                            case SDLK_4:
                            case SDLK_5:
                            case SDLK_6:
                            case SDLK_7:
                            case SDLK_8:
                            case SDLK_9: {
                                //for SDLK_1 to SDLK_9 select group with that number, if ctrl create group from selected obj
                                if(SDL_GetModState() & KMOD_CTRL) {
                                    selectedLists[event.key.keysym.sym - SDLK_1] = selectedList;

                                    pInterface->UpdateObjectInterface();
                                } else {
                                    if(!(SDL_GetModState() & KMOD_SHIFT)) {
                                        unselectAll(selectedList);
                                        selectedList.clear();
                                    }

                                    std::set<Uint32>::iterator iter;
                                    for(iter = selectedLists[event.key.keysym.sym - SDLK_1].begin(); iter != selectedLists[event.key.keysym.sym - SDLK_1].end(); ++iter) {
                                        ObjectClass* object = objectManager.getObject(*iter);
                                        object->setSelected(true);
                                        selectedList.insert(object->getObjectID());
                                    }

                                    pInterface->UpdateObjectInterface();
                                }
                                currentCursorMode = CursorMode_Normal;
                            } break;

                            case SDLK_MINUS: {
                                if (++gamespeed > GAMESPEED_MAX)
                                    gamespeed = GAMESPEED_MAX;
                                char temp[20];
                                sprintf(temp, (pTextManager->getLocalized("Game speed") + ":%d").c_str(), gamespeed);
                                currentGame->AddToNewsTicker(temp);
                            } break;

                            case SDLK_EQUALS: {
                                //PLUS
                                if (--gamespeed < GAMESPEED_MIN)
                                    gamespeed = GAMESPEED_MIN;
                                char temp[20];
                                sprintf(temp, (pTextManager->getLocalized("Game speed") + ":%d").c_str(), gamespeed);
                                currentGame->AddToNewsTicker(temp);
                            } break;



                            case SDLK_a: {
                                //set object to attack
                                if(currentCursorMode != CursorMode_Attack) {
                                    std::set<Uint32>::iterator iter;
                                    for(iter = selectedList.begin(); iter != selectedList.end(); ++iter) {

                                        tempObject = objectManager.getObject(*iter);
                                        if(tempObject->isAUnit() && (tempObject->getOwner() == pLocalHouse)
                                            && tempObject->isRespondable() && tempObject->canAttack()) {

                                            currentCursorMode = CursorMode_Attack;
                                            break;
                                        } else if((tempObject->getItemID() == Structure_Palace)
                                                    && ((tempObject->getOwner()->getHouseID() == HOUSE_HARKONNEN) || (tempObject->getOwner()->getHouseID() == HOUSE_SARDAUKAR))) {
                                            if(((PalaceClass*) tempObject)->specialReady()) {
                                                currentCursorMode = CursorMode_Attack;
                                                break;
                                            }
                                        }
                                    }
                                }
                            } break;

                            case SDLK_t: {
                                bShowTime = !bShowTime;
                            } break;

                            case SDLK_ESCAPE: {
                                OnOptions();
                            } break;

                            case SDLK_F5: {
                                // skip a 30 seconds
                                SkipToGameCycle = GameCycleCount + (30*1000)/GAMESPEED_DEFAULT;
                            } break;

                            case SDLK_F6: {
                                // skip 2 minutes
                                SkipToGameCycle = GameCycleCount + (120*1000)/GAMESPEED_DEFAULT;
                            } break;

                            case SDLK_F10: {
                                soundPlayer->toggleSound();
                            } break;

                            case SDLK_F11: {
                                musicPlayer->toggleSound();
                            } break;

                            case SDLK_F12: {
                                bShowFPS = !bShowFPS;
                            } break;

                            case SDLK_m: {
                                //set object to move
                                if(currentCursorMode != CursorMode_Move) {
                                    std::set<Uint32>::iterator iter;
                                    for(iter = selectedList.begin(); iter != selectedList.end(); ++iter) {

                                        tempObject = objectManager.getObject(*iter);
                                        if(tempObject->isAUnit() && (tempObject->getOwner() == pLocalHouse)
                                            && tempObject->isRespondable()) {

                                            currentCursorMode = CursorMode_Move;
                                            break;
                                        }
                                    }
                                }
                            } break;

                            case SDLK_p:
                            case SDLK_PRINT:
                            case SDLK_SYSREQ: {
                                std::string screenshotFilename = "screenshot.bmp";
                                SDL_SaveBMP(screen, screenshotFilename.c_str());
                                currentGame->AddToNewsTicker(pTextManager->getLocalized("Screenshot saved") + ": '" + screenshotFilename + "'");
                            } break;

                            case SDLK_r: {
                                std::set<Uint32>::iterator iter;
                                for(iter = selectedList.begin(); iter != selectedList.end(); ++iter) {
                                    ObjectClass *tempObject = objectManager.getObject(*iter);
                                    if(tempObject->isAStructure()) {
                                            ((StructureClass*)tempObject)->HandleRepairClick();
                                    }
                                }
                            } break;

                            case SDLK_RETURN: {
                                if(SDL_GetModState() & KMOD_ALT) {
                                    SDL_WM_ToggleFullScreen(screen);
                                } else {
                                    typingChatMessage = "";
                                    chatMode = true;
                                }
                            } break;

                            case SDLK_TAB: {
                                if(SDL_GetModState() & KMOD_ALT) {
                                    SDL_WM_IconifyWindow();
                                }
                            } break;

                            case SDLK_SPACE: {
                                bPause = !bPause;
                            } break;

                            default:
                                break;
                        }
                    }
                } break;

                case SDL_MOUSEBUTTONDOWN: {
                    SDL_MouseButtonEvent* mouse = &event.button;

                    if((pInGameMenu == NULL) && (pInGameMentat==NULL)) {
                        if(mouse->button == SDL_BUTTON_LEFT) {
                            pInterface->HandleMouseLeft(mouse->x, mouse->y,true);
                        } else if(mouse->button == SDL_BUTTON_RIGHT) {
                            pInterface->HandleMouseRight(mouse->x, mouse->y,true);
                        }
                    }

                    switch(mouse->button) {

                        case (SDL_BUTTON_LEFT): {
                            if (currentCursorMode == CursorMode_Placing) {
                                if(screenborder->isScreenCoordInsideMap(mouse->x, mouse->y) == true) {
                                    //if mouse is not over game bar

                                    int xPos = screenborder->screen2MapX(mouse->x);
                                    int	yPos = screenborder->screen2MapY(mouse->y);

                                    BuilderClass* builder = NULL;
                                    if(selectedList.size() == 1) {
                                        builder = dynamic_cast<BuilderClass*>(objectManager.getObject(*selectedList.begin()));
                                    }

                                    int placeItem = builder->GetCurrentProducedItem();
                                    Coord structuresize = getStructureSize(placeItem);

                                    if(placeItem == Structure_Slab1) {
                                        if((currentGameMap->isWithinBuildRange(xPos, yPos, builder->getOwner()))
                                            && (currentGameMap->okayToPlaceStructure(xPos, yPos, 1, 1, false, builder->getOwner()))
                                            && (currentGameMap->getCell(xPos, yPos)->isConcrete() == false)) {
                                            GetCommandManager().addCommand(Command(CMD_PLACE_STRUCTURE,builder->getObjectID(), placeItem, xPos, yPos));
                                            //the user has tried to place and has been successful
                                            soundPlayer->playSound(PlaceStructure);
                                            currentCursorMode = CursorMode_Normal;
                                        } else {
                                            //the user has tried to place but clicked on impossible point
                                            currentGame->AddToNewsTicker(pTextManager->getDuneText(DuneText_CannotPlaceFoundationHere));
                                            soundPlayer->playSound(InvalidAction);	//can't place noise
                                        }
                                    } else if(placeItem == Structure_Slab4) {
                                        if(	(currentGameMap->isWithinBuildRange(xPos, yPos, builder->getOwner()) || currentGameMap->isWithinBuildRange(xPos+1, yPos, builder->getOwner())
                                                || currentGameMap->isWithinBuildRange(xPos+1, yPos+1, builder->getOwner()) || currentGameMap->isWithinBuildRange(xPos, yPos+1, builder->getOwner()))
                                            && ((currentGameMap->okayToPlaceStructure(xPos, yPos, 1, 1, false, builder->getOwner())
                                                || currentGameMap->okayToPlaceStructure(xPos+1, yPos, 1, 1, false, builder->getOwner())
                                                || currentGameMap->okayToPlaceStructure(xPos+1, yPos+1, 1, 1, false, builder->getOwner())
                                                || currentGameMap->okayToPlaceStructure(xPos, yPos, 1, 1+1, false, builder->getOwner())))
                                            && ((currentGameMap->getCell(xPos, yPos)->isConcrete() == false) || (currentGameMap->getCell(xPos+1, yPos)->isConcrete() == false)
                                                || (currentGameMap->getCell(xPos, yPos+1)->isConcrete() == false) || (currentGameMap->getCell(xPos+1, yPos+1)->isConcrete() == false)) ) {

                                            GetCommandManager().addCommand(Command(CMD_PLACE_STRUCTURE,builder->getObjectID(), placeItem, xPos, yPos));
                                            //the user has tried to place and has been successful
                                            soundPlayer->playSound(PlaceStructure);
                                            currentCursorMode = CursorMode_Normal;
                                        } else {
                                            //the user has tried to place but clicked on impossible point
                                            currentGame->AddToNewsTicker(pTextManager->getDuneText(DuneText_CannotPlaceFoundationHere));
                                            soundPlayer->playSound(InvalidAction);	//can't place noise
                                        }
                                    } else {
                                        if(currentGameMap->okayToPlaceStructure(xPos, yPos, structuresize.x, structuresize.y, false, builder->getOwner())) {
                                            GetCommandManager().addCommand(Command(CMD_PLACE_STRUCTURE,builder->getObjectID(), placeItem, xPos, yPos));
                                            //the user has tried to place and has been successful
                                            soundPlayer->playSound(PlaceStructure);
                                            currentCursorMode = CursorMode_Normal;
                                        } else {
                                            //the user has tried to place but clicked on impossible point
                                            if(placeItem == Structure_Slab1) {
                                                currentGame->AddToNewsTicker(pTextManager->getDuneText(DuneText_CannotPlaceFoundationHere));
                                            } else {
                                                currentGame->AddToNewsTicker(strprintf(pTextManager->getDuneText(DuneText_CannotPlaceHere).c_str(),
                                                                                       resolveItemName(placeItem, true).c_str()));
                                            }
                                            soundPlayer->playSound(InvalidAction);	//can't place noise
                                        }
                                    }
                                }
                            } else if(currentCursorMode == CursorMode_Attack) {

                                int xPos = INVALID_POS;
                                int yPos = INVALID_POS;

                                if(screenborder->isScreenCoordInsideMap(mouse->x, mouse->y) == true) {
                                    xPos = screenborder->screen2MapX(mouse->x);
                                    yPos = screenborder->screen2MapY(mouse->y);
                                } else if(radarView->isOnRadar(mouse->x, mouse->y) == true) {
                                    Coord position = radarView->getWorldCoords(mouse->x, mouse->y);

                                    xPos = position.x / BLOCKSIZE;
                                    yPos = position.y / BLOCKSIZE;
                                }

                                if((xPos != INVALID_POS) && (yPos != INVALID_POS)) {

                                    UnitClass* responder = NULL;

                                    std::set<Uint32>::iterator iter;
                                    for(iter = selectedList.begin(); iter != selectedList.end(); ++iter) {
                                        ObjectClass *tempObject = objectManager.getObject(*iter);
                                        if(tempObject->isAUnit() && (tempObject->getOwner() == pLocalHouse) && tempObject->isRespondable()) {
                                            responder = (UnitClass*) tempObject;
                                            responder->HandleAttackClick(xPos,yPos);
                                        } else if((tempObject->getItemID() == Structure_Palace)
                                                    && ((tempObject->getOwner()->getHouseID() == HOUSE_HARKONNEN) || (tempObject->getOwner()->getHouseID() == HOUSE_SARDAUKAR))) {

                                            if(((PalaceClass*) tempObject)->specialReady()) {
                                                ((PalaceClass*) tempObject)->HandleDeathhandClick(xPos, yPos);
                                            }
                                        }
                                    }

                                    if(responder) {
                                        responder->playConfirmSound();
                                    }

                                    currentCursorMode = CursorMode_Normal;
                                }
                            } else if(currentCursorMode == CursorMode_Move) {

                                int xPos = INVALID_POS;
                                int yPos = INVALID_POS;

                                if(screenborder->isScreenCoordInsideMap(mouse->x, mouse->y) == true) {
                                    xPos = screenborder->screen2MapX(mouse->x);
                                    yPos = screenborder->screen2MapY(mouse->y);
                                } else if(radarView->isOnRadar(mouse->x, mouse->y) == true) {
                                    Coord position = radarView->getWorldCoords(mouse->x, mouse->y);

                                    xPos = position.x / BLOCKSIZE;
                                    yPos = position.y / BLOCKSIZE;
                                }

                                if((xPos != INVALID_POS) && (yPos != INVALID_POS)) {

                                    UnitClass* responder = NULL;

                                    std::set<Uint32>::iterator iter;
                                    for(iter = selectedList.begin(); iter != selectedList.end(); ++iter) {
                                        ObjectClass *tempObject = objectManager.getObject(*iter);
                                        if (tempObject->isAUnit() && (tempObject->getOwner() == pLocalHouse) && tempObject->isRespondable()) {
                                            responder = (UnitClass*) tempObject;
                                            responder->HandleMoveClick(xPos,yPos);
                                        }
                                    }

                                    if(responder) {
                                        responder->playConfirmSound();
                                    }

                                    currentCursorMode = CursorMode_Normal;
                                }
                            } else if(currentCursorMode == CursorMode_Capture) {

                                int xPos = INVALID_POS;
                                int yPos = INVALID_POS;

                                if(screenborder->isScreenCoordInsideMap(mouse->x, mouse->y) == true) {
                                    xPos = screenborder->screen2MapX(mouse->x);
                                    yPos = screenborder->screen2MapY(mouse->y);
                                } else if(radarView->isOnRadar(mouse->x, mouse->y) == true) {
                                    Coord position = radarView->getWorldCoords(mouse->x, mouse->y);

                                    xPos = position.x / BLOCKSIZE;
                                    yPos = position.y / BLOCKSIZE;
                                }

                                if((xPos != INVALID_POS) && (yPos != INVALID_POS)) {

                                    StructureClass* pStructure = dynamic_cast<StructureClass*>(currentGameMap->getCell(xPos, yPos)->getGroundObject());

                                    if((pStructure != NULL) && (pStructure->canBeCaptured()) && (pStructure->getOwner()->getTeam() != pLocalHouse->getTeam())) {
                                        InfantryClass* responder = NULL;

                                        std::set<Uint32>::iterator iter;
                                        for(iter = selectedList.begin(); iter != selectedList.end(); ++iter) {
                                            ObjectClass *tempObject = objectManager.getObject(*iter);
                                            if (tempObject->isInfantry() && (tempObject->getOwner() == pLocalHouse) && tempObject->isRespondable()) {
                                                responder = (InfantryClass*) tempObject;
                                                responder->HandleCaptureClick(xPos,yPos);
                                            }
                                        }

                                        if(responder) {
                                            responder->playConfirmSound();
                                        }

                                        currentCursorMode = CursorMode_Normal;
                                    }
                                }
                            } else {
                                //if not placingMode

                                if (mouse->x < gameBarPos.x && mouse->y >= topBarPos.h) {
                                    // it isn't on the gamebar

                                    if(!selectionMode) {
                                        // if we have started the selection rectangle
                                        // the starting point of the selection rectangele
                                        selectionRect.x = screenborder->screen2worldX(mouse->x);
                                        selectionRect.y = screenborder->screen2worldY(mouse->y);
                                    }
                                    selectionMode = true;

                                } else if (!selectionMode) {

                                    if(radarView->isOnRadar(mouse->x,mouse->y) == true) {
                                        //if on the radar
                                        screenborder->SetNewScreenCenter(radarView->getWorldCoords(mouse->x,mouse->y));

                                        //stay within the box
                                        radarMode = true;
                                    }
                                }
                            }
                        } break;	//end of SDL_BUTTON_LEFT

                        case(SDL_BUTTON_RIGHT): {
                            //if the right mouse button is pressed

                            if(currentCursorMode != CursorMode_Normal) {
                                //cancel special cursor mode
                                currentCursorMode = CursorMode_Normal;
                            } else if((!selectedList.empty()
                                            && (((objectManager.getObject(*selectedList.begin()))->getOwner() == pLocalHouse))
                                            && (((objectManager.getObject(*selectedList.begin()))->isRespondable())) )
                                        && ((mouse->x < gameBarPos.x) || (radarView->isOnRadar(mouse->x,mouse->y))))
                            {
                                //if user has a controlable unit selected

                                //coordinate on map
                                int	xPos = INVALID_POS;
                                int yPos = INVALID_POS;

                                if(screenborder->isScreenCoordInsideMap(mouse->x, mouse->y) == true) {
                                    //if mouse isn't on the gamebar
                                    xPos = screenborder->screen2MapX(mouse->x);
                                    yPos = screenborder->screen2MapY(mouse->y);

                                    indicatorPosition.x = screenborder->screen2worldX(mouse->x);
                                    indicatorPosition.y = screenborder->screen2worldY(mouse->y);
                                } else if(radarView->isOnRadar(mouse->x, mouse->y) == true) {
                                    //if on the radar
                                    indicatorPosition = radarView->getWorldCoords(mouse->x, mouse->y);

                                    // calculate map coordinates
                                    xPos = indicatorPosition.x / BLOCKSIZE;
                                    yPos = indicatorPosition.y / BLOCKSIZE;
                                }

                                if((xPos != INVALID_POS) && (yPos != INVALID_POS)) {

                                    indicatorFrame = 0;

                                    //let unit handle right click on map or target
                                    double speedCap = -1.0;
                                    ObjectClass	*responder = NULL;
                                    ObjectClass	*tempObject = NULL;

                                    std::set<Uint32>::iterator iter;
                                    for(iter = selectedList.begin(); iter != selectedList.end(); ++iter) {
                                        tempObject = objectManager.getObject(*iter);

                                        if(tempObject->getOwner() == pLocalHouse && tempObject->isRespondable()) {
                                            tempObject->HandleActionClick(xPos, yPos);

                                            //if this object obey the command
                                            if((responder == NULL) && tempObject->isRespondable())
                                                responder = tempObject;

                                            //find slowest speed of all units
                                            if( (SDL_GetModState() & KMOD_SHIFT) && tempObject->isAUnit()
                                                 && ((speedCap < 0.0) || (((UnitClass*)tempObject)->getMaxSpeed() < speedCap))) {
                                                speedCap = ((UnitClass*)tempObject)->getMaxSpeed();
                                            }
                                        }
                                    }

                                    if(responder) {
                                        responder->playConfirmSound();
                                    }

                                    for(iter = selectedList.begin(); iter != selectedList.end(); ++iter) {
                                        tempObject = objectManager.getObject(*iter);
                                        if((tempObject->isAUnit()) && (tempObject->getOwner() == pLocalHouse) && tempObject->isRespondable()) {
                                            ((UnitClass*)tempObject)->setSpeedCap(speedCap);
                                        }
                                    }
                                }

                            }
                        } break;	//end of SDL_BUTTON_RIGHT

                    }
                } break;

                case SDL_MOUSEMOTION: {
                    SDL_MouseMotionEvent* mouse = &event.motion;

                    if((pInGameMenu==NULL) && (pInGameMentat==NULL)) {
                        pInterface->HandleMouseMovement(mouse->x,mouse->y);
                    }

                    if((radarMode) && radarView->isOnRadar(mouse->x,mouse->y) == true) {
                        screenborder->SetNewScreenCenter(radarView->getWorldCoords(mouse->x,mouse->y));
                    }

                } break;

                case SDL_MOUSEBUTTONUP: {
                    SDL_MouseButtonEvent* mouse = &event.button;
                    if((mouse->button == SDL_BUTTON_LEFT) || (mouse->button == SDL_BUTTON_RIGHT)) {
                        if((pInGameMenu==NULL) && (pInGameMentat==NULL)) {
                            if(mouse->button == SDL_BUTTON_LEFT) {
                                pInterface->HandleMouseLeft(mouse->x, mouse->y,false);
                            } else if(mouse->button == SDL_BUTTON_RIGHT) {
                                pInterface->HandleMouseRight(mouse->x, mouse->y,false);
                            }
                        }

                        if(selectionMode && (mouse->button == SDL_BUTTON_LEFT)) {
                            //this keeps the box on the map, and not over game bar
                            int finalMouseX = mouse->x;
                            int finalMouseY = mouse->y;

                            if(finalMouseX >= gameBarPos.x) {
                                finalMouseX = gameBarPos.x-1;
                            }

                            int rectFinishX = screenborder->screen2MapX(finalMouseX);
                            if(rectFinishX > (currentGameMap->sizeX-1)) {
                                rectFinishX = currentGameMap->sizeX-1;
                            }

                            int rectFinishY = screenborder->screen2MapY(finalMouseY);

                            // convert start also to map coordinates
                            int rectStartX = selectionRect.x/BLOCKSIZE;
                            int	rectStartY = selectionRect.y/BLOCKSIZE;

                            currentGameMap->selectObjects(  pLocalHouse->getHouseID(),
                                                            rectStartX, rectStartY, rectFinishX, rectFinishY,
                                                            screenborder->screen2worldX(finalMouseX),
                                                            screenborder->screen2worldY(finalMouseY),
                                                            SDL_GetModState() & KMOD_SHIFT);

                            if(selectedList.size() == 1) {
                                ObjectClass* pObject = objectManager.getObject( *selectedList.begin());
                                if(pObject != NULL && pObject->getOwner() == pLocalHouse && pObject->getItemID() == Unit_Harvester) {
                                    HarvesterClass* pHarvester = (HarvesterClass*) pObject;

                                    std::string harvesterMessage = resolveItemName(Unit_Harvester, true);

                                    int duneTextIndex = DuneText_IsPercentFull;
                                    if(pHarvester->isAwaitingPickup()) {
                                        duneTextIndex = DuneText_IsPercentFullAndAwaitingPickup;
                                    } else if(pHarvester->isReturning()) {
                                        duneTextIndex = DuneText_IsPercentFullAndHeadingBack;
                                    } else if(pHarvester->isHarvesting()) {
                                        duneTextIndex = DuneText_IsPercentFullAndHarvesting;
                                    }

                                    int percent = lround(pHarvester->getAmountOfSpice() * 100.0 / (double) HARVESTERMAXSPICE);
                                    if(percent > 0) {
                                        harvesterMessage += strprintf(pTextManager->getDuneText(duneTextIndex).c_str(), percent);
                                    } else {
                                        harvesterMessage += pTextManager->getDuneText(duneTextIndex+4);
                                    }
                                    pInterface->AddToNewsTicker(harvesterMessage);

                                }
                            }

                            pInterface->UpdateObjectInterface();
                        }

                        radarMode = selectionMode = false;

                    }
                } break;

                case (SDL_QUIT): {
                    bQuitGame = true;
                } break;

                default:
                    break;
			}
		}
	}

	if((pInGameMenu == NULL) && (pInGameMentat == NULL)) {

	    Uint8 *keystate = SDL_GetKeyState(NULL);
		scrollDownMode =  (drawnMouseY >= screen->h-1) || keystate[SDLK_DOWN];
		scrollLeftMode = (drawnMouseX <= 0) || keystate[SDLK_LEFT];
		scrollRightMode = (drawnMouseX >= screen->w-1) || keystate[SDLK_RIGHT];
        scrollUpMode = (drawnMouseY <= 0) || keystate[SDLK_UP];

        if(scrollLeftMode && scrollRightMode) {
            // do nothing
        } else if(scrollLeftMode) {
            scrollLeftMode = screenborder->scrollLeft();
        } else if(scrollRightMode) {
            scrollRightMode = screenborder->scrollRight();
        }

        if(scrollDownMode && scrollUpMode) {
            // do nothing
        } else if(scrollDownMode) {
            scrollDownMode = screenborder->scrollDown();
        } else if(scrollUpMode) {
            scrollUpMode = screenborder->scrollUp();
        }
	} else {
	    scrollDownMode = false;
	    scrollLeftMode = false;
	    scrollRightMode = false;
	    scrollUpMode = false;
	}
}


/**
	Draws the cursor.
*/
void Game::drawCursor()
{

	SDL_Surface* pCursor = NULL;
    SDL_Rect dest = { drawnMouseX, drawnMouseY, 0, 0};
	if(scrollLeftMode || scrollRightMode || scrollUpMode || scrollDownMode) {
        if(scrollLeftMode && !scrollRightMode) {
	        pCursor = pGFXManager->getUIGraphic(UI_CursorLeft);
	        dest.y -= 5;
	    } else if(scrollRightMode && !scrollLeftMode) {
            pCursor = pGFXManager->getUIGraphic(UI_CursorRight);
	        dest.x -= pCursor->w / 2;
	        dest.y -= 5;
	    }

        if(pCursor == NULL) {
            if(scrollUpMode && !scrollDownMode) {
                pCursor = pGFXManager->getUIGraphic(UI_CursorUp);
                dest.x -= 5;
            } else if(scrollDownMode && !scrollUpMode) {
                pCursor = pGFXManager->getUIGraphic(UI_CursorDown);
                dest.x -= 5;
                dest.y -= pCursor->h / 2;
            } else {
                pCursor = pGFXManager->getUIGraphic(UI_CursorNormal);
            }
        }
	} else {
	    if( (pInGameMenu != NULL) || (pInGameMentat != NULL) || (((drawnMouseX >= gameBarPos.x) || (drawnMouseY < topBarPos.h)) && (radarView->isOnRadar(drawnMouseX, drawnMouseY) == false))) {
            // Menu mode or Mentat Menu or outside of game screen but not inside minimap
            pCursor = pGFXManager->getUIGraphic(UI_CursorNormal);
	    } else {

            switch(currentCursorMode) {
                case CursorMode_Normal:
                case CursorMode_Placing: {
                    pCursor = pGFXManager->getUIGraphic(UI_CursorNormal);
                } break;

                case CursorMode_Move: {
                    pCursor = pGFXManager->getUIGraphic(UI_CursorMove);
                    dest.x -= pCursor->w / 2;
                    dest.y -= pCursor->h / 2;
                } break;

                case CursorMode_Attack: {
                    pCursor = pGFXManager->getUIGraphic(UI_CursorAttack);
                    dest.x -= pCursor->w / 2;
                    dest.y -= pCursor->h / 2;
                } break;

                case CursorMode_Capture: {
                    pCursor = pGFXManager->getUIGraphic(UI_CursorCapture);
                    dest.x -= pCursor->w / 2;
                    dest.y -= pCursor->h;

                    int xPos = INVALID_POS;
                    int yPos = INVALID_POS;

                    if(screenborder->isScreenCoordInsideMap(drawnMouseX, drawnMouseY) == true) {
                        xPos = screenborder->screen2MapX(drawnMouseX);
                        yPos = screenborder->screen2MapY(drawnMouseY);
                    } else if(radarView->isOnRadar(drawnMouseX, drawnMouseY) == true) {
                        Coord position = radarView->getWorldCoords(drawnMouseX, drawnMouseY);

                        xPos = position.x / BLOCKSIZE;
                        yPos = position.y / BLOCKSIZE;
                    }

                    if((xPos != INVALID_POS) && (yPos != INVALID_POS)) {

                        TerrainClass* pCell = currentGameMap->getCell(xPos, yPos);

                        if(pCell->isExplored(pLocalHouse->getHouseID())) {

                            StructureClass* pStructure = dynamic_cast<StructureClass*>(pCell->getGroundObject());

                            if((pStructure != NULL) && (pStructure->canBeCaptured()) && (pStructure->getOwner()->getTeam() != pLocalHouse->getTeam())) {
                                dest.y += ((GetGameCycleCount() / 10) % 5);
                            }
                        }
                    }

                } break;

                default: {
                    throw std::runtime_error("Game::drawCursor(): Unknown cursor mode");
                };
            }
	    }
	}

    dest.w = pCursor->w;
    dest.h = pCursor->h;

	if(SDL_BlitSurface(pCursor, NULL, screen, &dest) != 0) {
        fprintf(stderr,"Game::drawCursor(): %s\n", SDL_GetError());
	}
}

/**
	This method sets up the view. The start position is the center point of all owned units/structures
*/
void Game::setupView()
{
	int i = 0;
	int j = 0;
	int count = 0;

	//setup start location/view
	i = j = count = 0;

    RobustList<UnitClass*>::const_iterator unitIterator;
	for(unitIterator = unitList.begin(); unitIterator != unitList.end(); ++unitIterator) {
		UnitClass* pUnit = *unitIterator;
		if(pUnit->getOwner() == pLocalHouse) {
			i += pUnit->getX();
			j += pUnit->getY();
			count++;
		}
	}

    RobustList<StructureClass*>::const_iterator structureIterator;
	for(structureIterator = structureList.begin(); structureIterator != structureList.end(); ++structureIterator) {
		StructureClass* pStructure = *structureIterator;
		if(pStructure->getOwner() == pLocalHouse) {
			i += pStructure->getX();
			j += pStructure->getY();
			count++;
		}
	}

	if(count == 0) {
		i = currentGameMap->sizeX*BLOCKSIZE/2-1;
		j = currentGameMap->sizeY*BLOCKSIZE/2-1;
	} else {
		i = i*BLOCKSIZE/count;
		j = j*BLOCKSIZE/count;
	}

	screenborder->SetNewScreenCenter(Coord(i,j));
}

/**
	This method starts the game. Will return when the game is finished or aborted.
*/
void Game::runMainLoop()
{
	printf("Starting game...\n");
	fflush(stdout);

	// add interface
	if(pInterface == NULL) {
        pInterface = new GameInterface();
	}

	finished = false;
	bPause = false;
	won = false;
	gameState = BEGUN;

	//setup endlevel conditions
	finishedLevel = false;

	bShowTime = winFlags & WINLOSEFLAGS_TIMEOUT;

	// Check if a player has lost
	for(int j = 0; j < MAX_PLAYERS; j++) {
		if(house[j] != NULL) {
			if(!house[j]->isAlive()) {
				house[j]->lose();
			}
		}
	}

	if(bReplay) {
		CmdManager.setReadOnly(true);
	} else {
		char tmp[FILENAME_MAX];
		fnkdat("replay/auto.rpl", tmp, FILENAME_MAX, FNKDAT_USER | FNKDAT_CREAT);
		std::string replayname(tmp);

		FileStream* pStream = new FileStream();
		pStream->open(replayname.c_str(), "wb");
		gameInitSettings.save(*pStream);

        // when this game was loaded we have to save the old commands to the replay file first
		CmdManager.save(*pStream);

		// now all new commands might be added
		CmdManager.setStream(pStream);

		// flush stream
		pStream->flush();
	}

	int		frameStart = SDL_GetTicks();
	int     frameEnd = 0;
	int     frameTime = 0;
	int     numFrames = 0;

	//main game loop
//	try
//	{
		do {
			drawScreen();

			SDL_Flip(screen);
			frameEnd = SDL_GetTicks();

			if(frameEnd == frameStart) {
				SDL_Delay(1);
			}

			frameTime += frameEnd - frameStart; // find difference to get frametime
			frameStart = SDL_GetTicks();

			numFrames++;

			if (bShowFPS) {
				fps = ((double) 1000.0)/((double)frameTime);
				frameTimer = frameTime;
			}

			if(settings.Video.FrameLimit == true) {
				if(frameTime < 32) {
					SDL_Delay(32 - frameTime);
				}
			}

            if(finished) {
                // end timer for the ending message
                if(SDL_GetTicks() - finishedLevelTime > END_WAIT_TIME) {
                    finishedLevel = true;
                }
            }

			while( (frameTime > gamespeed) || (!finished && (GameCycleCount < SkipToGameCycle)) )	{

				doInput();
				pInterface->UpdateObjectInterface();

				if(!finished && !bPause)	{
				    radarView->update();
					CmdManager.executeCommands(GameCycleCount);

                    /*
					if((GameCycleCount % 10) == 0) {
                        char tmp[20];
                        sprintf(tmp, "cycle%d.dls", GameCycleCount);
                        saveGame(tmp);
					}//*/

                    if((GameCycleCount % 50) == 0) {
                        // add every 100 gamecycles one test sync command
                        if(bReplay == false) {
                            CmdManager.addCommand(Command(CMD_TEST_SYNC, RandomGen.getSeed()));
                        }
                    }

					for (int i = 0; i < MAX_PLAYERS; i++) {
						if (house[i] != NULL) {
							house[i]->update();
						}
					}

					triggerManager.trigger(GameCycleCount);

					ProcessObjects();

					if ((indicatorFrame != NONE) && (--indicatorTimer <= 0)) {
						indicatorTimer = indicatorTime;

						if (++indicatorFrame > 2) {
							indicatorFrame = NONE;
						}
					}

                    GameCycleCount++;
				}

                if(GameCycleCount <= SkipToGameCycle) {
                    frameTime = 0;
                } else {
                    frameTime -= gamespeed;
                }

			}
			musicPlayer->musicCheck();	//if song has finished, start playing next one
		} while (!bQuitGame && !finishedLevel);//not sure if we need this extra bool
//	}
//	catch (...)
//	{
//		fprintf(stderr, "Argghhh. Exception caught: main game loop\n");
//	}


	// Game is finished

	if(bReplay == false && currentGame->won == true) {
        // save replay
	    std::string mapname = gameInitSettings.getFilename();

		char tmp[FILENAME_MAX];
		std::string mapnameBase = mapname.substr(mapname.find_last_of("/") == std::string::npos ? 0 : mapname.find_last_of("/"), mapname.length() - 4);
		fnkdat(std::string("replay/" + mapnameBase + ".rpl").c_str(), tmp, FILENAME_MAX, FNKDAT_USER | FNKDAT_CREAT);
		std::string replayname(tmp);

		FileStream* pStream = new FileStream();
		pStream->open(replayname.c_str(), "wb");
		gameInitSettings.save(*pStream);
		CmdManager.save(*pStream);
        delete pStream;
	}

    gameState = DEINITIALIZE;
	printf("Game finished!\n");
	fflush(stdout);
}

/**
    This method resumes the current paused game.
*/
void Game::ResumeGame()
{
	bPause = false;
}

/**
    This method is the callback method for the OPTIONS button at the top of the screen.
    It pauses the game and loads the in game menu.
*/
void Game::OnOptions()
{
    if(bReplay == true) {
        // don't show menu
        quit_Game();
    } else {
        pInGameMenu = new InGameMenu(houseColor[pLocalHouse->getHouseID()] + 3);
        bPause = true;
    }
}

/**
    This method is the callback method for the MENTAT button at the top of the screen.
    It pauses the game and loads the mentat help screen.
*/
void Game::OnMentat()
{
	if((pInGameMentat = new MentatHelp(gameInitSettings.getHouseID(), gameInitSettings.getMission())) == NULL) {
		fprintf(stderr,"Game::OnMentat(): Cannot create a MentatHelp object\n");
		exit(EXIT_FAILURE);
	}

	bPause = true;
}


/**
    This method should be called if whatNext() returns GAME_NEXTMISSION or GAME_LOAD. You should
    destroy this Game and create a new one. Call Game::initGame() with the GameInitClass
    that was returned previously by getNextGameInitSettings().
    \return a GameInitSettings-Object that describes the next game.
*/
GameInitSettings Game::getNextGameInitSettings()
{
	if(nextGameInitSettings.getGameType() != GAMETYPE_INVALID) {
		// return the prepared game init settings (load game or restart mission)
		return nextGameInitSettings;
	}

	switch(gameInitSettings.getGameType()) {
		case GAMETYPE_CAMPAIGN: {
			/* do map choice */
			fprintf(stdout,"Map Choice...");
			fflush(stdout);
			MapChoice* pMapChoice = new MapChoice(gameInitSettings.getHouseID(), gameInitSettings.getMission());
			int nextMission = pMapChoice->showMenu();
			delete pMapChoice;

			fprintf(stdout,"\t\t\tfinished\n");
			fflush(stdout);

            return GameInitSettings(gameInitSettings, nextMission);
		} break;

		default: {
			fprintf(stderr,"Game::getNextGameInitClass(): Wrong gameType for next Game.\n");
			fflush(stderr);
			return GameInitSettings();
		} break;
	}

	return GameInitSettings();
}

/**
    This method should be called after startGame() has returned. whatNext() will tell the caller
    what should be done after the current game has finished.<br>
    Possible return values are:<br>
    GAME_RETURN_TO_MENU  - the game is finished and you should return to the main menu<br>
    GAME_NEXTMISSION     - the game is finished and you should load the next mission<br>
    GAME_LOAD			 - from inside the game the user requests to load a savegame and you should do this now<br>
    GAME_DEBRIEFING_WIN  - show debriefing (player has won) and call whatNext() again afterwards<br>
    GAME_DEBRIEFING_LOST - show debriefing (player has lost) and call whatNext() again afterwards<br>
    <br>
    \return one of GAME_RETURN_TO_MENU, GAME_NEXTMISSION, GAME_LOAD, GAME_DEBRIEFING_WIN, GAME_DEBRIEFING_LOST
*/
int Game::whatNext()
{
	if(whatNextParam != GAME_NOTHING) {
		int tmp = whatNextParam;
		whatNextParam = GAME_NOTHING;
		return tmp;
	}

	if(nextGameInitSettings.getGameType() != GAMETYPE_INVALID) {
		return GAME_LOAD;
	}

	switch(gameType) {
		case GAMETYPE_CAMPAIGN: {
			if(bQuitGame == true) {
				return GAME_RETURN_TO_MENU;
			} else if(won == true) {
				if(gameInitSettings.getMission() == 22) {
					// there is no mission after this mission
					whatNextParam = GAME_RETURN_TO_MENU;
				} else {
					// there is a mission after this mission
					whatNextParam = GAME_NEXTMISSION;
				}
				return GAME_DEBRIEFING_WIN;
			} else {
                // copy old init class to init class for next game
                nextGameInitSettings = gameInitSettings;

				return GAME_DEBRIEFING_LOST;
			}
		} break;

		case GAMETYPE_SKIRMISH: {
			if(bQuitGame == true) {
				return GAME_RETURN_TO_MENU;
			} else if(won == true) {
				whatNextParam = GAME_RETURN_TO_MENU;
				return GAME_DEBRIEFING_WIN;
			} else {
				whatNextParam = GAME_RETURN_TO_MENU;
				return GAME_DEBRIEFING_LOST;
			}
		} break;

		default: {
			return GAME_RETURN_TO_MENU;
		} break;
	}
}


/**
    This method loads a previously saved game.
    \param filename the name of the file to load from
    \return true on success, false on failure
*/
bool Game::loadSaveGame(std::string filename)
{
	short	mapSizeX, mapSizeY;
	int		i, x;
	Coord		source, destination;

	FileStream fs;

	if(fs.open(filename.c_str(), "rb") == false) {
		perror("Game::loadSaveGame()");
		exit(EXIT_FAILURE);
	}

	gameState = LOADING;

	Uint32 magicNum = fs.readUint32();
	if (magicNum != SAVEMAGIC) {
		fprintf(stderr,"Game::loadSaveGame(): No valid savegame: %s\n",filename.c_str());
		exit(EXIT_FAILURE);
	}

	Uint32 savegameVersion = fs.readUint32();
	if (savegameVersion != SAVEGAMEVERSION) {
		fprintf(stderr,"Game::loadSaveGame(): No valid savegame: %s\n",filename.c_str());
		exit(EXIT_FAILURE);
	}

	std::string duneVersion = fs.readString();

	//read map size
	mapSizeX = fs.readUint32();
	mapSizeY = fs.readUint32();

	//create the new map
	currentGameMap = new MapClass(mapSizeX, mapSizeY);

	//read GameCycleCount
	GameCycleCount = fs.readUint32();

	// read gameInitSettings
	gameInitSettings = GameInitSettings(fs);

	// read some settings
	gameType = (GAMETYPE) fs.readSint8();
	techLevel = fs.readUint8();
	RandomGen.setSeed(fs.readUint32());

    // read in the unit/structure data
    objectData.load(fs);

	//load the house(s) info
	for (i=0; i<MAX_PLAYERS; i++) {
		if (fs.readBool() == true) {
		    //house in game
	        house[i] = new House(fs);
		}
	}

	debug = fs.readBool();
    bCheatsEnabled = fs.readBool();

	winFlags = fs.readUint32();
	loseFlags = fs.readUint32();

	currentGameMap->load(fs);

	//load the structures and units
	objectManager.load(fs);

	x = fs.readUint32();
	for(i = 0; i < x; i++) {
		bulletList.push_back(new BulletClass(fs));
	}

    x = fs.readUint32();
	for(i = 0; i < x; i++) {
		explosionList.push_back(new Explosion(fs));
	}

	//load selection lists
	selectedList = fs.readUint32Set();

    for(int i=0;i< NUMSELECTEDLISTS; i++) {
        selectedLists[i] = fs.readUint32Set();
    }

	//load the screenborder info
    screenborder->adjustScreenBorderToMapsize();
    screenborder->load(fs);

    // load triggers
    triggerManager.load(fs);

    // CommandManager is at the very end of the file. DO NOT CHANGE THIS!
	CmdManager.load(fs);

	fs.close();
	finished = false;

	return true;
}

/**
    This method saves the current running game.
    \param filename the name of the file to save to
    \return true on success, false on failure
*/
bool Game::saveGame(std::string filename)
{
	char	temp[256];
	int	i;

	FileStream fs;

	if( fs.open(filename.c_str(), "wb") == false) {
		perror("Game::saveGame()");
		sprintf(temp, "Game NOT saved: Cannot open \"%s\".", filename.c_str());
		currentGame->AddToNewsTicker(temp);
		return false;
	}

	fs.writeUint32(SAVEMAGIC);

	fs.writeUint32(SAVEGAMEVERSION);

	fs.writeString("Dune Legacy " VERSION);

	//write the map size
	fs.writeUint32(currentGameMap->sizeX);
	fs.writeUint32(currentGameMap->sizeY);

	// write GameCycleCount
	fs.writeUint32(GameCycleCount);

	// write gameInitSettings
	gameInitSettings.save(fs);

	// write some settings
	fs.writeSint8(gameType);
	fs.writeUint8(techLevel);
	fs.writeUint32(RandomGen.getSeed());

    // write out the unit/structure data
    objectData.save(fs);

	//write the house(s) info
	for (i=0; i<MAX_PLAYERS; i++) {
		fs.writeBool(house[i] != NULL);

		if(house[i] != NULL) {
			house[i]->save(fs);
		}
	}

	fs.writeBool(debug);
	fs.writeBool(bCheatsEnabled);

	fs.writeUint32(winFlags);
    fs.writeUint32(loseFlags);

	currentGameMap->save(fs);

	// save the structures and units
	objectManager.save(fs);

	fs.writeUint32(bulletList.size());
	for(RobustList<BulletClass*>::const_iterator iter = bulletList.begin(); iter != bulletList.end(); ++iter) {
		(*iter)->save(fs);
	}

	fs.writeUint32(explosionList.size());
	for(RobustList<Explosion*>::const_iterator iter = explosionList.begin(); iter != explosionList.end(); ++iter) {
		(*iter)->save(fs);
	}

	// save selection lists

	// write out selected units list
	fs.writeUint32Set(selectedList);

	// write out selection groups (Key 1 to 9)
	for(int i=0; i < NUMSELECTEDLISTS; i++) {
	    fs.writeUint32Set(selectedLists[i]);
	}

	// write the screenborder info
	screenborder->save(fs);

    // save triggers
	triggerManager.save(fs);

    // CommandManager is at the very end of the file. DO NOT CHANGE THIS!
	CmdManager.save(fs);

	fs.close();

	return true;
}

/**
    This method writes out an object to a stream.
    \param stream   the stream to write to
    \param obj      the object to be saved
*/
void Game::SaveObject(Stream& stream, ObjectClass* obj) {
	if(obj == NULL)
		return;

	stream.writeUint32(obj->getItemID());
	obj->save(stream);
}

/**
    This method loads an object from the stream.
    \param stream   the stream to read from
    \param ObjectID the object id that this unit/structure should get
    \return the read unit/structure
*/
ObjectClass* Game::LoadObject(Stream& stream, Uint32 ObjectID)
{
	Uint32 itemID;

	itemID = stream.readUint32();

	ObjectClass* newObject = ObjectClass::loadObject(stream, itemID, ObjectID);
	if(newObject == NULL) {
		fprintf(stderr,"Game::LoadObject(): ObjectClass::loadObject() returned NULL!\n");
		exit(EXIT_FAILURE);
	}

	return newObject;
}

/**
    This method selects all units/structures in the list aList.
    \param aList the list containing all the units/structures to be selected
*/
void Game::selectAll(std::set<Uint32>& aList)
{
    std::set<Uint32>::iterator iter;
    for(iter = aList.begin(); iter != aList.end(); ++iter) {
        ObjectClass *tempObject = objectManager.getObject(*iter);
        tempObject->setSelected(true);
    }
}

/**
    This method unselects all units/structures in the list aList.
    \param aList the list containing all the units/structures to be unselected
*/
void Game::unselectAll(std::set<Uint32>& aList)
{
    std::set<Uint32>::iterator iter;
    for(iter = aList.begin(); iter != aList.end(); ++iter) {
        ObjectClass *tempObject = objectManager.getObject(*iter);
        tempObject->setSelected(false);
    }
}
