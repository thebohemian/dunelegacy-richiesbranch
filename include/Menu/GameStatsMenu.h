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

#ifndef GAMESTATSMENU_H
#define GAMESTATSMENU_H

#include "MenuClass.h"

#include <GUI/StaticContainer.h>
#include <GUI/Label.h>
#include <GUI/ProgressBar.h>

#include <string>

class GameStatsMenu : public MenuClass {
public:
    GameStatsMenu(int level);
    virtual ~GameStatsMenu();

	virtual int showMenu();

    virtual bool doInput(SDL_Event &event);

    virtual void DrawSpecificStuff();

private:
    void doState(int elapsedTime);

    void calculateScore(int level);

    typedef enum {
        State_HumanSpice,
        State_Between_HumanSpice_and_AISpice,
        State_AISpice,
        State_Between_AISpice_and_HumanUnits,
        State_HumanUnits,
        State_Between_HumanUnits_and_AIUnits,
        State_AIUnits,
        State_Between_AIUnits_and_HumanBuildings,
        State_HumanBuildings,
        State_Between_HumanBuildings_and_AIBuildings,
        State_AIBuildings,
        State_Finished
    } GameStatsState;


	StaticContainer	WindowWidget;
	Label TextScore;
	Label TextTime;

	Label TextYourRank;
	Label TextRank;

    // spice statistics
	Label       TextSpiceHarvestedBy;
	Label       TextYou1;
    ProgressBar ProgressSpiceYouShadow;
	ProgressBar ProgressSpiceYou;
	Label       TextSpiceYou;
	Label       TextEnemy1;
	ProgressBar ProgressSpiceEnemyShadow;
	ProgressBar ProgressSpiceEnemy;
	Label       TextSpiceEnemy;

    // units statistics
	Label       TextUnitsDestroyedBy;
	Label       TextYou2;
    ProgressBar ProgressUnitsYouShadow;
	ProgressBar ProgressUnitsYou;
	Label       TextUnitsYou;
	Label       TextEnemy2;
	ProgressBar ProgressUnitsEnemyShadow;
	ProgressBar ProgressUnitsEnemy;
	Label       TextUnitsEnemy;

    // buildings statistics
	Label       TextBuildingsDestroyedBy;
	Label       TextYou3;
    ProgressBar ProgressBuildingsYouShadow;
	ProgressBar ProgressBuildingsYou;
	Label       TextBuildingsYou;
	Label       TextEnemy3;
	ProgressBar ProgressBuildingsEnemyShadow;
	ProgressBar ProgressBuildingsEnemy;
	Label       TextBuildingsEnemy;

	int currentStateStartTime;
	GameStatsState currentState;

    int Human_UnitsDestroyed;
	int AI_UnitsDestroyed;

    int Human_StructuresDestroyed;
	int AI_StructuresDestroyed;

	double Human_SpiceHarvested;
	double AI_SpiceHarvested;

    int totalTime;
	int totalScore;

	std::string rank;
};

#endif //GAMESTATSMENU_H
