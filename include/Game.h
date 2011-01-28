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

#ifndef GAMECLASS_H
#define GAMECLASS_H

#include <GUI/CallbackTarget.h>

#include <misc/Random.h>
#include <misc/RobustList.h>
#include <ObjectData.h>
#include <ObjectManager.h>
#include <CommandManager.h>
#include <GameInterface.h>
#include <GameINILoader.h>
#include <GameInitSettings.h>
#include <Trigger/TriggerManager.h>

#include <DataTypes.h>

#include <SDL.h>
#include <stdarg.h>
#include <string>
#include <map>

// forward declarations
class ObjectClass;
class InGameMenu;
class MentatHelp;
class RadarView;
class ObjectManager;
class House;
class Explosion;


#define END_WAIT_TIME				(6*1000)

#define GAME_NOTHING			-1
#define	GAME_RETURN_TO_MENU		0
#define GAME_NEXTMISSION		1
#define	GAME_LOAD				2
#define GAME_DEBRIEFING_WIN		3
#define	GAME_DEBRIEFING_LOST	4


class Game : public CallbackTarget
{
public:

    /**
        Default constructor. Call initGame() or initReplay() afterwards.
    */
	Game();

    /**
        Destructor
    */
	~Game();

    /**
        Initializes a game with the specified settings
        \param  newGameInitSettings the game init settings to initialize the game
    */
	void initGame(const GameInitSettings& newGameInitSettings);

	/**
        Initializes a replay from the specified filename
        \param  filename    the file containing the replay
	*/
	void initReplay(const std::string& filename);



	friend class GameINILoader; // loading INI Maps is done with a GameINILoader helper object



	void ProcessObjects();
	void drawScreen();
	void doInput();

    /**
        Returns the current game cycle number.
        \return the current game cycle
    */
	Uint32 GetGameCycleCount() { return GameCycleCount; };

	/**
        Return the game time in milliseconds.
        \return the current game time in milliseconds
	*/
	Uint32 GetGameTime() { return GameCycleCount * GAMESPEED_DEFAULT; };

    /**
        Get the command manager of this game
        \return the command manager
    */
	CommandManager& GetCommandManager() { return CmdManager; };

    /**
        Get the trigger manager of this game
        \return the trigger manager
    */
	TriggerManager& GetTriggerManager() { return triggerManager; };

	/**
        Get the explosion list.
        \return the explosion list
	*/
	RobustList<Explosion*>& getExplosionList() { return explosionList; };

	/**
        The current game is finished and the local house has won
	*/
	void setGameWon() {
        won = true;
        finished = true;
        finishedLevelTime = SDL_GetTicks();
	}

	/**
        The current game is finished and the local house has lost
	*/
	void setGameLost() {
        won = false;
        finished = true;
        finishedLevelTime = SDL_GetTicks();
	}

	void drawCursor();
	void setupView();

	bool loadSaveGame(std::string filename);
	bool saveGame(std::string filename);
	void runMainLoop();
	inline void quit_Game() { bQuitGame = true;};
	void ResumeGame();

	void SaveObject(Stream& stream, ObjectClass* obj);
	ObjectClass* LoadObject(Stream& stream, Uint32 ObjectID);
	inline ObjectManager& getObjectManager() { return objectManager; };
	inline GameInterface& getGameInterface() { return *pInterface; };

	const GameInitSettings& getGameInitSettings() const { return gameInitSettings; };
    void setNextGameInitSettings(const GameInitSettings& nextGameInitSettings) { this->nextGameInitSettings = nextGameInitSettings; };
	GameInitSettings getNextGameInitSettings();
	int whatNext();

	void OnOptions();
	void OnMentat();

	void selectAll(std::set<Uint32>& aList);
	void unselectAll(std::set<Uint32>& aList);

    /**
        Returns a list of all currently selected objects.
        \return list of currently selected units/structures
    */
	std::set<Uint32>& getSelectedList() { return selectedList; };

	/**
        Returns one of the 10 saved units lists
        \param  i   which list should be returned
        \return the i-th list.
	*/
	std::set<Uint32>& getGroupList(int i) { return selectedLists[i]; };

    /**
        Adds a new message to the news ticker.
        \param  text    the text to add
    */
	void AddToNewsTicker(const std::string& text) {
		if(pInterface != NULL) {
			pInterface->AddToNewsTicker(text);
		}
	}

    /**
        Adds an urgent message to the news ticker.
        \param  text    the text to add
    */
	void AddUrgentMessageToNewsTicker(const std::string& text) {
		if(pInterface != NULL) {
			pInterface->AddUrgentMessageToNewsTicker(text);
		}
	}

	/**
        Are cheats enabled?
        \return true = cheats enabled, false = cheats disabled
	*/
	bool CheatsEnabled() const { return bCheatsEnabled; };

public:
    enum {
        CursorMode_Normal,
        CursorMode_Attack,
        CursorMode_Move,
        CursorMode_Capture,
        CursorMode_Placing
    };

    int         currentCursorMode;

	GAMETYPE	gameType;
	int			techLevel;
	int			winFlags;
	int         loseFlags;

	House*	    house[MAX_PLAYERS];

	int		    gamespeed;

	Random      RandomGen;          ///< This is the random number generator for this game
	ObjectData  objectData;         ///< This contains all the unit/structure data
	RadarView*  radarView;          ///< This is the minimap/radar in the game bar

	GAMESTATETYPE gameState;

private:
	bool	    chatMode;           ///< chat mode on?
    std::string typingChatMessage;  ///< currently typed chat message

	bool	    scrollDownMode;     ///< currently scrolling the map down?
	bool        scrollLeftMode;     ///< currently scrolling the map left?
	bool        scrollRightMode;    ///< currently scrolling the map right?
	bool        scrollUpMode;       ///< currently scrolling the map up?

	bool	    radarMode;          ///< currently moving the view rectangle on the radar?
	bool	    selectionMode;      ///< currently selection multiple units with a selection rectangle?
	SDL_Rect    selectionRect;      ///< the drawn rectangle while selection multiple units

	int		    whatNextParam;

	Uint32      indicatorFrame;
	int         indicatorTime;
	int         indicatorTimer;
	Coord       indicatorPosition;

	double      fps;
	int         frameTimer;

	Uint32      GameCycleCount;

	Uint32      SkipToGameCycle;    ///< skip to this game cycle

	SDL_Rect	powerIndicatorPos;  ///< position of the power indicator in the right game bar
	SDL_Rect	spiceIndicatorPos;  ///< position of the spice indicator in the right game bar
	SDL_Rect	topBarPos;          ///< position of the top game bar
    SDL_Rect    gameBarPos;         ///< position of the right game bar

	////////////////////

	GameInitSettings	gameInitSettings;       ///< the init settings this game was started with
	GameInitSettings	nextGameInitSettings;   ///< the init settings the next game shall be started with (restarting the mission, loading a savegame

	ObjectManager       objectManager;          ///< This manages all the object and maps object ids to the actual objects

	CommandManager      CmdManager;			    ///< This is the manager for all the game commands (e.g. moving a unit)

	TriggerManager      triggerManager;         ///< This is the manager for all the triggers the scenario has (e.g. reinforcements)

	bool	bQuitGame;					///< Should the game be quited after this game tick
	bool	bPause;						///< Is the game currently halted
	bool	bReplay;					///< Is this game actually a replay

	bool	bShowFPS;					///< Show the FPS

	bool    bShowTime;                  ///< Show how long this game is running

	bool    bCheatsEnabled;             ///< Cheat codes are enabled?

    bool	finished;                   ///< Is the game finished (won or lost) and we are just waiting for the end message to be shown
	bool	won;                        ///< If the game is finished, is it won or lost
	Uint32  finishedLevelTime;          ///< The time in milliseconds when the level was finished (won or lost)
	bool    finishedLevel;              ///< Set, when the game is really finished and the end message was shown

	GameInterface*	pInterface;			///< This is the whole interface (top bar and side bar)
	InGameMenu*		pInGameMenu;		///< This is the menu that is opened by the option button
	MentatHelp*		pInGameMentat;		///< This is the mentat dialog opened by the mentat button

	std::set<Uint32> selectedList;                      ///< A set of all selected units/structures
    std::set<Uint32> selectedLists[NUMSELECTEDLISTS];   ///< Sets of all the different groups on key 1 to 9
    RobustList<Explosion*> explosionList;               ///< A list containing all the explosions that must be drawn

	///////////////////////

//	MapClass		*map;
};

#endif // GAMECLASS_H
