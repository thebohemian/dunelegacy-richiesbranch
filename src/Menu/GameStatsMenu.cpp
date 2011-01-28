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

#include <Menu/GameStatsMenu.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <FileClasses/TextManager.h>
#include <FileClasses/music/MusicPlayer.h>
#include <misc/string_util.h>
#include <House.h>
#include <SoundPlayer.h>
#include <Game.h>
#include <structures/StructureClass.h>
#include <units/UnitClass.h>
#include <units/HarvesterClass.h>

#include <climits>
#include <algorithm>

#define max3(a,b,c) (std::max((a),std::max((b),(c))))

#define PROGRESSBARTIME 4000.0
#define WAITTIME 1000

GameStatsMenu::GameStatsMenu(int level) : MenuClass()
{
    calculateScore(level);

	// set up window
	SDL_Surface *surf;
	surf = pGFXManager->getUIGraphic(UI_GameStatsBackground);

	SetBackground(surf,false);
	Resize(surf->w,surf->h);

	SetWindowWidget(&WindowWidget);

    TextScore.SetTextColor(COLOR_WHITE, COLOR_BLACK);
    TextScore.SetText(strprintf(pTextManager->getDuneText(DuneText_Score).c_str(), totalScore));
	WindowWidget.AddWidget(&TextScore, (GetSize()/2) + Point(-175, -173), TextScore.GetSize());

    TextTime.SetTextColor(COLOR_WHITE, COLOR_BLACK);
    TextTime.SetText(strprintf(pTextManager->getDuneText(DuneText_Time).c_str(), totalTime/3600, (totalTime%3600)/60));
	WindowWidget.AddWidget(&TextTime, (GetSize()/2) + Point(+180 - TextTime.GetSize().x, -173), TextTime.GetSize());

    TextYourRank.SetAlignment(Alignment_HCenter);
    TextYourRank.SetTextColor(COLOR_WHITE, COLOR_BLACK);
    TextYourRank.SetText(pTextManager->getDuneText(DuneText_You_have_attained_the_rank));
	WindowWidget.AddWidget(&TextYourRank, (GetSize()/2) + Point(-TextYourRank.GetSize().x/2, -127), TextYourRank.GetSize());

    TextRank.SetAlignment(Alignment_HCenter);
    TextRank.SetText(rank);
	WindowWidget.AddWidget(&TextRank, (GetSize()/2) + Point(-TextRank.GetSize().x/2, -105), TextRank.GetSize());

    TextSpiceHarvestedBy.SetTextColor(COLOR_WHITE, COLOR_BLACK, COLOR_THICKSPICE);
    TextSpiceHarvestedBy.SetAlignment(Alignment_HCenter);
    TextSpiceHarvestedBy.SetText(pTextManager->getDuneText(DuneText_SpiceHarvestedBy));
	WindowWidget.AddWidget(&TextSpiceHarvestedBy, (GetSize()/2) + Point(-TextSpiceHarvestedBy.GetSize().x/2, -40), TextSpiceHarvestedBy.GetSize());

    TextUnitsDestroyedBy.SetTextColor(COLOR_WHITE, COLOR_BLACK, COLOR_THICKSPICE);
    TextUnitsDestroyedBy.SetAlignment(Alignment_HCenter);
    TextUnitsDestroyedBy.SetText(pTextManager->getDuneText(DuneText_UnitsDestroyedBy));
	WindowWidget.AddWidget(&TextUnitsDestroyedBy, (GetSize()/2) + Point(-TextUnitsDestroyedBy.GetSize().x/2, 34), TextUnitsDestroyedBy.GetSize());

    TextBuildingsDestroyedBy.SetTextColor(COLOR_WHITE, COLOR_BLACK, COLOR_THICKSPICE);
    TextBuildingsDestroyedBy.SetAlignment(Alignment_HCenter);
    TextBuildingsDestroyedBy.SetText(pTextManager->getDuneText(DuneText_BuildingsDestroyedBy));
	WindowWidget.AddWidget(&TextBuildingsDestroyedBy, (GetSize()/2) + Point(-TextBuildingsDestroyedBy.GetSize().x/2, 108), TextBuildingsDestroyedBy.GetSize());

    // spice statistics

    TextYou1.SetTextColor(COLOR_WHITE, COLOR_BLACK);
    TextYou1.SetAlignment(Alignment_Right);
    TextYou1.SetText(pTextManager->getDuneText(DuneText_You));
	WindowWidget.AddWidget(&TextYou1, (GetSize()/2) + Point(-229 - TextYou1.GetSize().x, -23), TextYou1.GetSize());

	ProgressSpiceYouShadow.SetColor(COLOR_BLACK);
	ProgressSpiceYouShadow.SetProgress(0.0);
	WindowWidget.AddWidget(&ProgressSpiceYouShadow, (GetSize()/2) + Point(-228 + 2, -15 + 2), Point(440,12));

	ProgressSpiceYou.SetColor(houseColor[pLocalHouse->getHouseID()] + 1);
	ProgressSpiceYou.SetProgress(0.0);
	WindowWidget.AddWidget(&ProgressSpiceYou, (GetSize()/2) + Point(-228, -15), Point(440,12));

	TextSpiceYou.SetTextColor(COLOR_WHITE, COLOR_BLACK);
	TextSpiceYou.SetAlignment(Alignment_HCenter);
	TextSpiceYou.SetVisible(false);
	WindowWidget.AddWidget(&TextSpiceYou, (GetSize()/2) + Point(222, -21), Point(66,21));

    TextEnemy1.SetTextColor(COLOR_WHITE, COLOR_BLACK);
    TextEnemy1.SetAlignment(Alignment_Right);
    TextEnemy1.SetText(pTextManager->getDuneText(DuneText_Enemy));
	WindowWidget.AddWidget(&TextEnemy1, (GetSize()/2) + Point(-229 - TextEnemy1.GetSize().x, -5), TextEnemy1.GetSize());

	ProgressSpiceEnemyShadow.SetColor(COLOR_BLACK);
	ProgressSpiceEnemyShadow.SetProgress(0.0);
	WindowWidget.AddWidget(&ProgressSpiceEnemyShadow, (GetSize()/2) + Point(-228 + 2, 3 + 2), Point(440,12));

	ProgressSpiceEnemy.SetColor(COLOR_SARDAUKAR + 1);
	ProgressSpiceEnemy.SetProgress(0.0);
	WindowWidget.AddWidget(&ProgressSpiceEnemy, (GetSize()/2) + Point(-228, 3), Point(440,12));

	TextSpiceEnemy.SetTextColor(COLOR_WHITE, COLOR_BLACK);
	TextSpiceEnemy.SetAlignment(Alignment_HCenter);
	TextSpiceEnemy.SetVisible(false);
	WindowWidget.AddWidget(&TextSpiceEnemy, (GetSize()/2) + Point(222, -3), Point(66,21));

    // unit kill statistics

    TextYou2.SetTextColor(COLOR_WHITE, COLOR_BLACK);
    TextYou2.SetAlignment(Alignment_Right);
    TextYou2.SetText(pTextManager->getDuneText(DuneText_You));
	WindowWidget.AddWidget(&TextYou2, (GetSize()/2) + Point(-229 - TextYou2.GetSize().x, 51), TextYou2.GetSize());

	ProgressUnitsYouShadow.SetColor(COLOR_BLACK);
	ProgressUnitsYouShadow.SetProgress(0.0);
	WindowWidget.AddWidget(&ProgressUnitsYouShadow, (GetSize()/2) + Point(-228 + 2, 59 + 2), Point(440,12));

	ProgressUnitsYou.SetColor(houseColor[pLocalHouse->getHouseID()] + 1);
	ProgressUnitsYou.SetProgress(0.0);
	WindowWidget.AddWidget(&ProgressUnitsYou, (GetSize()/2) + Point(-228, 59), Point(440,12));

	TextUnitsYou.SetTextColor(COLOR_WHITE, COLOR_BLACK);
	TextUnitsYou.SetAlignment(Alignment_HCenter);
	TextUnitsYou.SetVisible(false);
	WindowWidget.AddWidget(&TextUnitsYou, (GetSize()/2) + Point(222, 53), Point(66,21));

    TextEnemy2.SetTextColor(COLOR_WHITE, COLOR_BLACK);
    TextEnemy2.SetAlignment(Alignment_Right);
    TextEnemy2.SetText(pTextManager->getDuneText(DuneText_Enemy));
	WindowWidget.AddWidget(&TextEnemy2, (GetSize()/2) + Point(-229 - TextEnemy2.GetSize().x, 69), TextEnemy2.GetSize());

	ProgressUnitsEnemyShadow.SetColor(COLOR_BLACK);
	ProgressUnitsEnemyShadow.SetProgress(0.0);
	WindowWidget.AddWidget(&ProgressUnitsEnemyShadow, (GetSize()/2) + Point(-228 + 2, 77 + 2), Point(440,12));

	ProgressUnitsEnemy.SetColor(COLOR_SARDAUKAR + 1);
	ProgressUnitsEnemy.SetProgress(0.0);
	WindowWidget.AddWidget(&ProgressUnitsEnemy, (GetSize()/2) + Point(-228, 77), Point(440,12));

	TextUnitsEnemy.SetTextColor(COLOR_WHITE, COLOR_BLACK);
	TextUnitsEnemy.SetAlignment(Alignment_HCenter);
	TextUnitsEnemy.SetVisible(false);
	WindowWidget.AddWidget(&TextUnitsEnemy, (GetSize()/2) + Point(222, 71), Point(66,21));

    // buildings kill statistics

    TextYou3.SetTextColor(COLOR_WHITE, COLOR_BLACK);
    TextYou3.SetAlignment(Alignment_Right);
    TextYou3.SetText(pTextManager->getDuneText(DuneText_You));
	WindowWidget.AddWidget(&TextYou3, (GetSize()/2) + Point(-229 - TextYou3.GetSize().x, 125), TextYou3.GetSize());

	ProgressBuildingsYouShadow.SetColor(COLOR_BLACK);
	ProgressBuildingsYouShadow.SetProgress(0.0);
	WindowWidget.AddWidget(&ProgressBuildingsYouShadow, (GetSize()/2) + Point(-228 + 2, 133 + 2), Point(440,12));

	ProgressBuildingsYou.SetColor(houseColor[pLocalHouse->getHouseID()] + 1);
	ProgressBuildingsYou.SetProgress(0.0);
	WindowWidget.AddWidget(&ProgressBuildingsYou, (GetSize()/2) + Point(-228, 133), Point(440,12));

	TextBuildingsYou.SetTextColor(COLOR_WHITE, COLOR_BLACK);
	TextBuildingsYou.SetAlignment(Alignment_HCenter);
	TextBuildingsYou.SetVisible(false);
	WindowWidget.AddWidget(&TextBuildingsYou, (GetSize()/2) + Point(222, 127), Point(66,21));

    TextEnemy3.SetTextColor(COLOR_WHITE, COLOR_BLACK);
    TextEnemy3.SetAlignment(Alignment_Right);
    TextEnemy3.SetText(pTextManager->getDuneText(DuneText_Enemy));
	WindowWidget.AddWidget(&TextEnemy3, (GetSize()/2) + Point(-229 - TextEnemy2.GetSize().x, 143), TextEnemy3.GetSize());

	ProgressBuildingsEnemyShadow.SetColor(COLOR_BLACK);
	ProgressBuildingsEnemyShadow.SetProgress(0.0);
	WindowWidget.AddWidget(&ProgressBuildingsEnemyShadow, (GetSize()/2) + Point(-228 + 2, 151 + 2), Point(440,12));

	ProgressBuildingsEnemy.SetColor(COLOR_SARDAUKAR + 1);
	ProgressBuildingsEnemy.SetProgress(0.0);
	WindowWidget.AddWidget(&ProgressBuildingsEnemy, (GetSize()/2) + Point(-228, 151), Point(440,12));

	TextBuildingsEnemy.SetTextColor(COLOR_WHITE, COLOR_BLACK);
	TextBuildingsEnemy.SetAlignment(Alignment_HCenter);
	TextBuildingsEnemy.SetVisible(false);
	WindowWidget.AddWidget(&TextBuildingsEnemy, (GetSize()/2) + Point(222, 145), Point(66,21));

}

GameStatsMenu::~GameStatsMenu() {
}

int GameStatsMenu::showMenu()
{
    musicPlayer->changeMusic(MUSIC_GAMESTAT);

    currentStateStartTime = SDL_GetTicks();
    currentState = State_HumanSpice;

    return MenuClass::showMenu();
}

bool GameStatsMenu::doInput(SDL_Event &event)
{
    if(event.type == SDL_MOUSEBUTTONUP) {
        if(currentState == State_Finished) {
            quit(0);
        } else {
            while(currentState != State_Finished) {
                doState(INT_MAX);
            }
        }
    }

    return MenuClass::doInput(event);
}

void GameStatsMenu::DrawSpecificStuff()
{
    doState(SDL_GetTicks() - currentStateStartTime);
}
void GameStatsMenu::doState(int elapsedTime)
{
    switch(currentState) {
        case State_HumanSpice:
        {
            double MaxSpiceHarvested = max3(Human_SpiceHarvested, AI_SpiceHarvested, 3000.0);
            double SpiceComplete = std::min(elapsedTime / PROGRESSBARTIME, 1.0);

            double Human_PercentSpiceComplete;
            if(SpiceComplete < Human_SpiceHarvested / MaxSpiceHarvested) {
                Human_PercentSpiceComplete = SpiceComplete * 100.0;
                TextSpiceYou.SetText( stringify( (int) (SpiceComplete*MaxSpiceHarvested)));
                soundPlayer->playSound(CreditsTick);
            } else {
                Human_PercentSpiceComplete = Human_SpiceHarvested * 100.0 / MaxSpiceHarvested;
                TextSpiceYou.SetText(stringify( (int) Human_SpiceHarvested));
                soundPlayer->playSound(Tick);
                currentState = State_Between_HumanSpice_and_AISpice;
                currentStateStartTime = SDL_GetTicks();
            }

            TextSpiceYou.SetVisible(true);
            ProgressSpiceYou.SetProgress( Human_PercentSpiceComplete );
            ProgressSpiceYouShadow.SetProgress( Human_PercentSpiceComplete );
        } break;

        case State_Between_HumanSpice_and_AISpice:
        {
            if(elapsedTime > WAITTIME) {
                currentState = State_AISpice;
                currentStateStartTime = SDL_GetTicks();
            }
        }
        break;

        case State_AISpice:
        {
            double MaxSpiceHarvested = max3(Human_SpiceHarvested, AI_SpiceHarvested, 3000.0);
            double SpiceComplete = std::min(elapsedTime / PROGRESSBARTIME, 1.0);

            double AI_PercentSpiceComplete;
            if(SpiceComplete < AI_SpiceHarvested / MaxSpiceHarvested) {
                AI_PercentSpiceComplete = SpiceComplete * 100.0;
                TextSpiceEnemy.SetText( stringify( (int) (SpiceComplete*MaxSpiceHarvested)));
                soundPlayer->playSound(CreditsTick);
            } else {
                AI_PercentSpiceComplete = AI_SpiceHarvested * 100.0 / MaxSpiceHarvested;
                TextSpiceEnemy.SetText(stringify( (int) AI_SpiceHarvested));
                soundPlayer->playSound(Tick);
                currentState = State_Between_AISpice_and_HumanUnits;
                currentStateStartTime = SDL_GetTicks();
            }

            TextSpiceEnemy.SetVisible(true);
            ProgressSpiceEnemy.SetProgress( AI_PercentSpiceComplete );
            ProgressSpiceEnemyShadow.SetProgress( AI_PercentSpiceComplete );
        } break;

        case State_Between_AISpice_and_HumanUnits:
            if(elapsedTime > WAITTIME) {
                currentState = State_HumanUnits;
                currentStateStartTime = SDL_GetTicks();
            }
        break;

        case State_HumanUnits:
        {
            double MaxUnitsDestroyed = max3(Human_UnitsDestroyed, AI_UnitsDestroyed, 200);
            double UnitsComplete = std::min(elapsedTime / PROGRESSBARTIME, 1.0);

            double Human_PercentUnitsComplete;
            if(UnitsComplete < Human_UnitsDestroyed / MaxUnitsDestroyed) {
                Human_PercentUnitsComplete = UnitsComplete * 100.0;
                TextUnitsYou.SetText( stringify( (int) (UnitsComplete*MaxUnitsDestroyed)));
                soundPlayer->playSound(CreditsTick);
            } else {
                Human_PercentUnitsComplete = Human_UnitsDestroyed * 100.0 / MaxUnitsDestroyed;
                TextUnitsYou.SetText( stringify(Human_UnitsDestroyed));
                soundPlayer->playSound(Tick);
                currentState = State_Between_HumanUnits_and_AIUnits;
                currentStateStartTime = SDL_GetTicks();
            }

            TextUnitsYou.SetVisible(true);
            ProgressUnitsYou.SetProgress( Human_PercentUnitsComplete );
            ProgressUnitsYouShadow.SetProgress( Human_PercentUnitsComplete );
        } break;

        case State_Between_HumanUnits_and_AIUnits:
            if(elapsedTime > WAITTIME) {
                currentState = State_AIUnits;
                currentStateStartTime = SDL_GetTicks();
            }
        break;

        case State_AIUnits:
        {
            double MaxUnitsDestroyed = max3(Human_UnitsDestroyed, AI_UnitsDestroyed, 200);
            double UnitsComplete = std::min(elapsedTime / PROGRESSBARTIME, 1.0);

            double AI_PercentUnitsComplete;
            if(UnitsComplete < AI_UnitsDestroyed / MaxUnitsDestroyed) {
                AI_PercentUnitsComplete = UnitsComplete * 100.0;
                TextUnitsEnemy.SetText( stringify( (int) (UnitsComplete*MaxUnitsDestroyed)));
                soundPlayer->playSound(CreditsTick);
            } else {
                AI_PercentUnitsComplete = AI_UnitsDestroyed * 100.0 / MaxUnitsDestroyed;
                TextUnitsEnemy.SetText( stringify(AI_UnitsDestroyed));
                soundPlayer->playSound(Tick);
                currentState = State_Between_AIUnits_and_HumanBuildings;
                currentStateStartTime = SDL_GetTicks();
            }

            TextUnitsEnemy.SetVisible(true);
            ProgressUnitsEnemy.SetProgress( AI_PercentUnitsComplete );
            ProgressUnitsEnemyShadow.SetProgress( AI_PercentUnitsComplete );
        } break;

        case State_Between_AIUnits_and_HumanBuildings:
            if(elapsedTime > WAITTIME) {
                currentState = State_HumanBuildings;
                currentStateStartTime = SDL_GetTicks();
            }
        break;

        case State_HumanBuildings:
        {
            double MaxBuildingsDestroyed = max3(Human_StructuresDestroyed, AI_StructuresDestroyed, 200);
            double BuildingsComplete = std::min(elapsedTime / PROGRESSBARTIME, 1.0);

            double Human_PercentBuildingsComplete;
            if(BuildingsComplete < Human_StructuresDestroyed / MaxBuildingsDestroyed) {
                Human_PercentBuildingsComplete = BuildingsComplete * 100.0;
                TextBuildingsYou.SetText( stringify( (int) (BuildingsComplete*MaxBuildingsDestroyed)));
                soundPlayer->playSound(CreditsTick);
            } else {
                Human_PercentBuildingsComplete = Human_StructuresDestroyed * 100.0 / MaxBuildingsDestroyed;
                TextBuildingsYou.SetText( stringify(Human_StructuresDestroyed));
                soundPlayer->playSound(Tick);
                currentState = State_Between_HumanBuildings_and_AIBuildings;
                currentStateStartTime = SDL_GetTicks();
            }

            TextBuildingsYou.SetVisible(true);
            ProgressBuildingsYou.SetProgress( Human_PercentBuildingsComplete );
            ProgressBuildingsYouShadow.SetProgress( Human_PercentBuildingsComplete );
        } break;

        case State_Between_HumanBuildings_and_AIBuildings:
            if(elapsedTime > WAITTIME) {
                currentState = State_AIBuildings;
                currentStateStartTime = SDL_GetTicks();
            }
        break;

        case State_AIBuildings:
        {
            double MaxBuildingsDestroyed = max3(Human_StructuresDestroyed, AI_StructuresDestroyed, 200);
            double BuildingsComplete = std::min(elapsedTime / PROGRESSBARTIME, 1.0);

            double AI_PercentBuildingsComplete;
            if(BuildingsComplete < AI_StructuresDestroyed / MaxBuildingsDestroyed) {
                AI_PercentBuildingsComplete = BuildingsComplete * 100.0;
                TextBuildingsEnemy.SetText( stringify( (int) (BuildingsComplete*MaxBuildingsDestroyed)));
                soundPlayer->playSound(CreditsTick);
            } else {
                AI_PercentBuildingsComplete = AI_StructuresDestroyed * 100.0 / MaxBuildingsDestroyed;
                TextBuildingsEnemy.SetText( stringify(AI_StructuresDestroyed));
                soundPlayer->playSound(Tick);
                currentState = State_Finished;
                currentStateStartTime = SDL_GetTicks();
            }

            TextBuildingsEnemy.SetVisible(true);
            ProgressBuildingsEnemy.SetProgress( AI_PercentBuildingsComplete );
            ProgressBuildingsEnemyShadow.SetProgress( AI_PercentBuildingsComplete );
        } break;

        case State_Finished:
        default:
        {
            // nothing
        } break;
    }
}

void GameStatsMenu::calculateScore(int level)
{
    Human_UnitsDestroyed = 0;
	AI_UnitsDestroyed = 0;

    Human_StructuresDestroyed = 0;
	AI_StructuresDestroyed = 0;

	Human_SpiceHarvested = 0.0;
	AI_SpiceHarvested = 0.0;

	totalTime = currentGame->GetGameTime()/1000;

    totalScore = level*45;

    double totalHumanCredits = 0.0;
	for(int i=0; i < MAX_PLAYERS; i++) {
        House* pHouse = currentGame->house[i];
        if(pHouse != NULL) {
            if(pHouse->isAI() == true) {
                AI_UnitsDestroyed += pHouse->getNumDestroyedUnits();
                AI_StructuresDestroyed += pHouse->getNumDestroyedStructures();
                AI_SpiceHarvested += pHouse->getHarvestedSpice();

                totalScore -= pHouse->getDestroyedValue();
            } else {
                Human_UnitsDestroyed += pHouse->getNumDestroyedUnits();
                Human_StructuresDestroyed += pHouse->getNumDestroyedStructures();
                Human_SpiceHarvested += pHouse->getHarvestedSpice();

                totalHumanCredits += pHouse->getCredits();

                totalScore += pHouse->getDestroyedValue();
            }
        }
	}

	totalScore += ((int) totalHumanCredits) / 100;

    for(RobustList<StructureClass*>::const_iterator iter = structureList.begin(); iter != structureList.end(); ++iter) {
        StructureClass* pStructure = *iter;
        if(pStructure->getOwner()->isAI() == false) {
            totalScore += currentGame->objectData.data[pStructure->getItemID()].price / 100;
        }
    }

    totalScore -= ((totalTime/60) + 1);

    for(RobustList<UnitClass*>::const_iterator iter = unitList.begin(); iter != unitList.end(); ++iter) {
        UnitClass* pUnit = *iter;
        if(pUnit->getItemID() == Unit_Harvester) {
            HarvesterClass* pHarvester = (HarvesterClass*) pUnit;
            if(pHarvester->getOwner()->isAI() == true) {
                AI_SpiceHarvested += pHarvester->getAmountOfSpice();
            } else {
                Human_SpiceHarvested += pHarvester->getAmountOfSpice();
            }
        }
    }

    if(currentGame->CheatsEnabled() == true) {
        rank = "Cheater";
    } else {
        int rankID = DuneText_SandFlea;

        if(totalScore >= 25)   rankID++;
        if(totalScore >= 50)   rankID++;
        if(totalScore >= 100)  rankID++;
        if(totalScore >= 150)  rankID++;
        if(totalScore >= 200)  rankID++;
        if(totalScore >= 300)  rankID++;
        if(totalScore >= 400)  rankID++;
        if(totalScore >= 500)  rankID++;
        if(totalScore >= 700)  rankID++;
        if(totalScore >= 1000) rankID++;
        if(totalScore >= 1400) rankID++;
    //    if(totalScore >= 1800) rankID++;

        rank = pTextManager->getDuneText(rankID);
    }
}
