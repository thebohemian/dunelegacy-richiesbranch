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

#include <sand.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <FileClasses/TextManager.h>

#include <Menu/BriefingMenu.h>
#include <Menu/GameStatsMenu.h>

#include <CutScenes/Meanwhile.h>
#include <CutScenes/Finale.h>

#include <Game.h>
#include <GameInitSettings.h>
#include <data.h>

#include <misc/string_util.h>

#include <algorithm>

/**
    This function draws the cursor to the screen. The coordinate is read from
    the two global variables drawnMouseX and drawnMouseY.
*/
void drawCursor()
{
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
        fprintf(stderr,"drawCursor(): %s\n", SDL_GetError());
	}
}

/**
    This function resolves the picture corresponding to one item id.
    \param itemID   the id of the item to resolve (e.g. Unit_Quad)
    \return the surface corresponding. This surface should not be freed or modified. NULL on error.
*/
SDL_Surface* resolveItemPicture(int itemID)
{
	int newPicID;

	switch(itemID) {
		case Unit_Carryall:			        newPicID = Picture_Carryall;		    break;
		case Unit_Devastator:		       	newPicID = Picture_Devastator;		    break;
		case Unit_Deviator:			        newPicID = Picture_Deviator;	    	break;
		case Unit_Fremen:			        newPicID = Picture_Fremen;	            break;
		case Unit_Frigate:			        return NULL;            	            break;
		case Unit_Harvester:		    	newPicID = Picture_Harvester;		    break;
		case Unit_Launcher:			        newPicID = Picture_Launcher;	    	break;
		case Unit_MCV:			            newPicID = Picture_MCV;		            break;
		case Unit_Ornithopter:			    newPicID = Picture_Ornithopter;		    break;
		case Unit_Quad:		            	newPicID = Picture_Quad;		        break;
		case Unit_Raider:			        newPicID = Picture_Raider;		        break;
		case Unit_SiegeTank:		    	newPicID = Picture_SiegeTank;	    	break;
		case Unit_SonicTank:		    	newPicID = Picture_SonicTank;	    	break;
		case Unit_Tank:			            newPicID = Picture_Tank;		        break;
		case Unit_Trike:		        	newPicID = Picture_Trike;		        break;
		case Unit_Saboteur:			        newPicID = Picture_Saboteur;	    	break;
		case Unit_Sandworm:			        return NULL;    	                	break;
		case Unit_Sardaukar:                newPicID = Picture_Sardaukar;		    break;
		case Unit_Soldier:			        newPicID = Picture_Soldier;	        	break;
		case Unit_Trooper:			        newPicID = Picture_Trooper;		        break;

		case Structure_Barracks:            newPicID = Picture_Barracks;		    break;
		case Structure_ConstructionYard:    newPicID = Picture_ConstructionYard;	break;
		case Structure_GunTurret:			newPicID = Picture_GunTurret;		    break;
		case Structure_HeavyFactory:		newPicID = Picture_HeavyFactory;		break;
		case Structure_HighTechFactory:		newPicID = Picture_HighTechFactory;		break;
		case Structure_IX:			        newPicID = Picture_IX;		            break;
		case Structure_LightFactory:		newPicID = Picture_LightFactory;		break;
		case Structure_Palace:			    newPicID = Picture_Palace;		        break;
		case Structure_Radar:			    newPicID = Picture_Radar;		        break;
		case Structure_Refinery:			newPicID = Picture_Refinery;	       	break;
		case Structure_RepairYard:			newPicID = Picture_RepairYard;		    break;
		case Structure_RocketTurret:		newPicID = Picture_RocketTurret;		break;
		case Structure_Silo:			    newPicID = Picture_Silo;		        break;
		case Structure_Slab1:			    newPicID = Picture_Slab1;               break;
		case Structure_Slab4:			    newPicID = Picture_Slab4;		        break;
		case Structure_StarPort:			newPicID = Picture_StarPort;	    	break;
		case Structure_Wall:			    newPicID = Picture_Wall;	        	break;
		case Structure_WindTrap:		   	newPicID = Picture_WindTrap;	    	break;
		case Structure_WOR:			        newPicID = Picture_WOR;		            break;

		default:
            throw std::invalid_argument("resolveItemPicture(): Invalid item ID " + stringify(itemID) + "!");
        break;
    }

    return pGFXManager->getSmallDetailPic(newPicID);
}

/**
    This function resolves the name corresponding to one item id.
    \param itemID   the id of the item to resolve (e.g. Unit_Quad)
    \param bShortname   return the short name of the unit/structure
    \return the string corresponding.
*/
std::string resolveItemName(int itemID, bool bShortname)
{
	int newDuneTextID;

	switch(itemID) {
		case Unit_Carryall:			        newDuneTextID = DuneText_Carryall;		    break;
		case Unit_Devastator:		       	newDuneTextID = DuneText_Devastator;		break;
		case Unit_Deviator:			        newDuneTextID = DuneText_Deviator;	    	break;
		case Unit_Fremen:			        return "Fremen";               	            break;
		case Unit_Frigate:			        return "Frigate";               	        break;
		case Unit_Harvester:		    	newDuneTextID = DuneText_Harvester;		    break;
		case Unit_Launcher:			        newDuneTextID = DuneText_Launcher;	    	break;
		case Unit_MCV:			            newDuneTextID = DuneText_MCV;		        break;
		case Unit_Ornithopter:			    newDuneTextID = DuneText_Ornithopter;		break;
		case Unit_Quad:		            	newDuneTextID = DuneText_Quad;		        break;
		case Unit_Raider:			        newDuneTextID = DuneText_Raider;		    break;
		case Unit_SiegeTank:		    	newDuneTextID = DuneText_SiegeTank;	    	break;
		case Unit_SonicTank:		    	newDuneTextID = DuneText_SonicTank;	    	break;
		case Unit_Tank:			            newDuneTextID = DuneText_Tank;		        break;
		case Unit_Trike:		        	newDuneTextID = DuneText_Trike;		        break;
		case Unit_Saboteur:			        newDuneTextID = DuneText_Saboteur;	    	break;
		case Unit_Sandworm:                 newDuneTextID = DuneText_Sandworm;		    break;
		case Unit_Sardaukar:                return "Sardaukar";               	        break;
		case Unit_Soldier:			        newDuneTextID = DuneText_Soldier;	        break;
		case Unit_Trooper:			        newDuneTextID = DuneText_Trooper;		    break;

		case Structure_Barracks:            newDuneTextID = DuneText_Barracks;		    break;
		case Structure_ConstructionYard:    newDuneTextID = DuneText_ConstructionYard;	break;
		case Structure_GunTurret:			newDuneTextID = DuneText_GunTurret;		    break;
		case Structure_HeavyFactory:		newDuneTextID = DuneText_HeavyFactory;		break;
		case Structure_HighTechFactory:		newDuneTextID = DuneText_HighTechFactory;	break;
		case Structure_IX:			        newDuneTextID = DuneText_IX;		        break;
		case Structure_LightFactory:		newDuneTextID = DuneText_LightFactory;		break;
		case Structure_Palace:			    newDuneTextID = DuneText_Palace;		    break;
		case Structure_Radar:			    newDuneTextID = DuneText_Radar;		        break;
		case Structure_Refinery:			newDuneTextID = DuneText_Refinery;	       	break;
		case Structure_RepairYard:			newDuneTextID = DuneText_RepairYard;		break;
		case Structure_RocketTurret:		newDuneTextID = DuneText_RocketTurret;		break;
		case Structure_Silo:			    newDuneTextID = DuneText_Silo;		        break;
		case Structure_Slab1:			    newDuneTextID = DuneText_Slab1;             break;
		case Structure_Slab4:			    newDuneTextID = DuneText_Slab4;		        break;
		case Structure_StarPort:			newDuneTextID = DuneText_StarPort;	    	break;
		case Structure_Wall:			    newDuneTextID = DuneText_Wall;	        	break;
		case Structure_WindTrap:		   	newDuneTextID = DuneText_WindTrap;	    	break;
		case Structure_WOR:			        newDuneTextID = DuneText_WOR;		        break;

		default:
            throw std::invalid_argument("resolveItemName(): Invalid item ID!");
        break;
    }

    return pTextManager->getDuneText( (bShortname == true) ? newDuneTextID-1 : newDuneTextID );
}

/**
    This function returns the size of the specified item.
    \param ItemID   the id of the item (e.g. Structure_HeavyFactory)
    \return a Coord containg the size (e.g. (3,2) ). Returns (0,0) on error.
*/
Coord getStructureSize(int ItemID) {

	switch(ItemID) {
		case Structure_Barracks:			return Coord(2,2); break;
		case Structure_ConstructionYard:	return Coord(2,2); break;
		case Structure_GunTurret: 			return Coord(1,1); break;
		case Structure_HeavyFactory: 		return Coord(3,2); break;
		case Structure_HighTechFactory:		return Coord(3,2); break;
		case Structure_IX:					return Coord(2,2); break;
		case Structure_LightFactory:		return Coord(2,2); break;
		case Structure_Palace:				return Coord(3,3); break;
		case Structure_Radar:				return Coord(2,2); break;
		case Structure_Refinery:			return Coord(3,2); break;
		case Structure_RepairYard:			return Coord(3,2); break;
		case Structure_RocketTurret:		return Coord(1,1); break;
		case Structure_Silo:				return Coord(2,2); break;
		case Structure_StarPort:			return Coord(3,3); break;
		case Structure_Slab1:				return Coord(1,1); break;
		case Structure_Slab4:				return Coord(2,2); break;
		case Structure_Wall:				return Coord(1,1); break;
		case Structure_WindTrap:			return Coord(2,2); break;
		case Structure_WOR:					return Coord(2,2); break;
		default:							return Coord(0,0); break;
	}

	return Coord(0,0);
}

/**
    This function return the item id of an item specified by name. There may be multiple names for
    one item. The case of the name is ignored.
    \param name the name of the item (e.g. "rocket-turret" or "r-turret".
    \return the id of the item (e.g. Structure_RocketTurret)
*/
Uint32  getItemIDByName(std::string name) {
    convertToLower(name);

    if(name == "barracks")                                              return Structure_Barracks;
    else if((name == "const yard") || (name == "construction yard"))    return Structure_ConstructionYard;
    else if((name == "r-turret") || (name == "rocket-turret"))          return Structure_RocketTurret;
    else if((name == "turret") || (name == "gun-turret"))               return Structure_GunTurret;
    else if((name == "heavy fctry") || (name == "heavy factory"))       return Structure_HeavyFactory;
	else if((name == "hi-tech") || (name == "hightech factory"))        return Structure_HighTechFactory;
	else if((name == "ix") || (name == "house ix"))                     return Structure_IX;
    else if((name == "light fctry") || (name == "light factory"))       return Structure_LightFactory;
	else if(name == "palace")                                           return Structure_Palace;
    else if((name == "outpost") || (name == "radar"))                   return Structure_Radar;
	else if(name == "refinery")                                         return Structure_Refinery;
    else if((name == "repair") || (name == "repair yard"))              return Structure_RepairYard;
	else if((name == "spice silo") || (name == "silo"))                 return Structure_Silo;
	else if((name == "concrete") || (name == "slab1"))                  return Structure_Slab1;
	else if(name == "slab4")                                            return Structure_Slab4;
    else if((name == "star port") || (name == "starport"))              return Structure_StarPort;
    else if(name == "wall")                                             return Structure_Wall;
    else if(name == "windtrap")                                         return Structure_WindTrap;
    else if(name == "wor")                                              return Structure_WOR;
    else if((name == "carryall") || (name == "carry-all"))              return Unit_Carryall;
	else if((name == "devastator") || (name == "devistator"))           return Unit_Devastator;
    else if(name == "deviator")                                         return Unit_Deviator;
    else if(name == "fremen")                                           return Unit_Fremen;
    else if(name == "frigate")                                          return Unit_Frigate;
	else if(name == "harvester")                                        return Unit_Harvester;
	else if(name == "soldier")                                          return Unit_Soldier;
	else if(name == "launcher")                                         return Unit_Launcher;
	else if(name == "mcv")                                              return Unit_MCV;
	else if((name == "thopters") || (name == "'thopters")
            || (name == "thopter") || (name == "'thopter")
            || (name == "ornithopter"))                                 return Unit_Ornithopter;
	else if(name == "quad")                                             return Unit_Quad;
	else if(name == "saboteur")                                         return Unit_Saboteur;
    else if(name == "sandworm")                                         return Unit_Sandworm;
    else if(name == "sardaukar")                                        return Unit_Sardaukar;
    else if(name == "siege tank")                                       return Unit_SiegeTank;
	else if((name == "sonic tank") || (name == "sonictank"))            return Unit_SonicTank;
	else if(name == "tank")                                             return Unit_Tank;
    else if(name == "trike")                                            return Unit_Trike;
    else if((name == "raider trike") || (name == "raider"))             return Unit_Raider;
	else if(name == "trooper")                                          return Unit_Trooper;
	else                                                                return ItemID_Invalid;
}

/**
    This function returns the number of each house providing the house name as a string. The comparison is
    done case-insensitive.
    \param name the name of the house (e.g."Atreides")
    \return the number of the house (e.g. HOUSE_ATREIDES). HOUSE_INVALID is returned on error.
*/
HOUSETYPE getHouseByName(std::string name)
{
    convertToLower(name);

    if(name == "atreides")          return HOUSE_ATREIDES;
    else if(name == "ordos")        return HOUSE_ORDOS;
    else if(name == "harkonnen")    return HOUSE_HARKONNEN;
    else if(name == "fremen")       return HOUSE_FREMEN;
	else if(name == "sardaukar")    return HOUSE_SARDAUKAR;
	else if(name == "mercenary")    return HOUSE_MERCENARY;
    else                            return HOUSE_INVALID;
}

/**
    This function returns the name of house the house number.
    \param house the number of the house (e.g. HOUSE_ATREIDES)
    \return the name of the house (e.g. "Atreides").
*/
std::string getHouseNameByNumber(HOUSETYPE house)
{
    static const char* houseName[NUM_HOUSES] = {    "Atreides",
                                                    "Ordos",
                                                    "Harkonnen",
                                                    "Sardaukar",
                                                    "Fremen",
                                                    "Mercenary"
                                                };

    if(house >= 0 && house < NUM_HOUSES) {
        return houseName[house];
    } else {
        fprintf(stderr,"getHouseNameByNumber(): Invalid house number %d!\n", house);
        exit(EXIT_FAILURE);
    }
}

ATTACKMODE getAttackModeByName(std::string name)
{
    convertToLower(name);

    if(name == "guard")                             return GUARD;
    else if(name == "area guard")                   return AREAGUARD;
    else if(name == "stop")                         return STOP;
    else if(name == "ambush")                       return AMBUSH;
	else if((name == "hunt") || (name == "attack")) return HUNT;
    else                                            return ATTACKMODE_INVALID;
}

/**
	Starts a game replay
	\param	filename	the filename of the replay file
*/
void startReplay(std::string filename) {
    printf("Initing Replay:\n");
    currentGame = new Game();
    currentGame->initReplay(filename);

    printf("Initialization finished!\n");
    fflush(stdout);

    currentGame->runMainLoop();

    delete currentGame;
}


/**
	Starts a new game. If this game is quit it might start another game. This other game is also started from
	this function. This is done until there is no more game to be started.
	\param init	contains all the information to start the game
*/
void startSinglePlayerGame(const GameInitSettings& init)
{
    GameInitSettings currentGameInitInfo = init;

	while(1) {

        printf("Initing Game:\n");
		currentGame = new Game();
		currentGame->initGame(currentGameInitInfo);

		printf("Initialization finished!\n");
		fflush(stdout);

		// get init settings from game as it might have changed (through loading the game)
		currentGameInitInfo = currentGame->getGameInitSettings();

		currentGame->runMainLoop();

		bool bGetNext = true;
		while(bGetNext) {
			switch(currentGame->whatNext()) {
				case GAME_DEBRIEFING_WIN: {
					fprintf(stdout,"Debriefing...");
					fflush(stdout);
					BriefingMenu* pBriefing = new BriefingMenu(currentGameInitInfo.getHouseID(), currentGameInitInfo.getMission(), DEBRIEFING_WIN);
					pBriefing->showMenu();
					delete pBriefing;
					fprintf(stdout,"\t\t\tfinished\n");
					fflush(stdout);

					fprintf(stdout,"Game statistics...");
					fflush(stdout);
                    GameStatsMenu* pGameStats = new GameStatsMenu(missionNumberToLevelNumber(currentGameInitInfo.getMission()));
					pGameStats->showMenu();
					delete pGameStats;
					fprintf(stdout,"\t\tfinished\n");
					fflush(stdout);

					if(currentGameInitInfo.getGameType() == GAMETYPE_CAMPAIGN) {
                        int level = missionNumberToLevelNumber(currentGameInitInfo.getMission());

                        if(level == 4) {
                            fprintf(stdout, "playing meanwhile.....");fflush(stdout);
                            Meanwhile* pMeanwhile = new Meanwhile(currentGameInitInfo.getHouseID(),true);
                            pMeanwhile->run();
                            delete pMeanwhile;
                            fprintf(stdout, "\t\tfinished\n"); fflush(stdout);
                        } else if(level == 8) {
                            fprintf(stdout, "playing meanwhile.....");fflush(stdout);
                            Meanwhile* pMeanwhile = new Meanwhile(currentGameInitInfo.getHouseID(),false);
                            pMeanwhile->run();
                            delete pMeanwhile;
                            fprintf(stdout, "\t\tfinished\n"); fflush(stdout);
                        } else if(level == 9) {
                            fprintf(stdout, "playing finale.....");fflush(stdout);
                            Finale* pFinale = new Finale(currentGameInitInfo.getHouseID());
                            pFinale->run();
                            delete pFinale;
                            fprintf(stdout, "\t\tfinished\n"); fflush(stdout);
                        }
					}
				} break;

				case GAME_DEBRIEFING_LOST: {
					fprintf(stdout,"Debriefing...");
					fflush(stdout);
					BriefingMenu* pBriefing = new BriefingMenu(currentGameInitInfo.getHouseID(), currentGameInitInfo.getMission(), DEBRIEFING_LOST);
					pBriefing->showMenu();
					delete pBriefing;
					fprintf(stdout,"\t\t\tfinished\n");
					fflush(stdout);
				} break;

				case GAME_LOAD:
				case GAME_NEXTMISSION: {
					currentGameInitInfo = currentGame->getNextGameInitSettings();
					delete currentGame;
					bGetNext = false;
				} break;

				case GAME_RETURN_TO_MENU:
				default: {
					delete currentGame;
					return;
				} break;
			}
		}


	}
}
