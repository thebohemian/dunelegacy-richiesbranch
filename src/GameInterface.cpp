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

#include <GameInterface.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <House.h>
#include <Game.h>

#include <ObjectClass.h>
#include <GUI/ObjectInterfaces/ObjectInterface.h>
#include <GUI/ObjectInterfaces/MultiUnitInterface.h>

#include <misc/draw_util.h>

#include <SDL.h>

GameInterface::GameInterface() : Window(0,0,0,0) {
	pObjectContainer = NULL;
	ObjectID = NONE;

	SetTransparentBackground(true);

	SetCurrentPosition(0,0,screen->w,screen->h);

	SetWindowWidget(&WindowWidget);

	SDL_Surface* surf, *surfPressed;

	// top bar
	surf = pGFXManager->getUIGraphic(UI_TopBar, pLocalHouse->getHouseID());
	TopBar.SetSurface(surf,false),
	WindowWidget.AddWidget(&TopBar,Point(0,0),Point(surf->w,surf->h));

	// game bar
	surf = pGFXManager->getUIGraphic(UI_GameBar, pLocalHouse->getHouseID());
	GameBar.SetSurface(surf,false),
	WindowWidget.AddWidget(&GameBar,Point(screen->w - surf->w,0),Point(surf->w,surf->h));

	// add buttons
	WindowWidget.AddWidget(&TopBar_HBox,Point(5,5),
							Point(screen->w - GameBar.GetSize().x, TopBar.GetSize().y - 5));

	TopBar_HBox.AddWidget(&newsticker);

	TopBar_HBox.AddWidget(Spacer::Create());

	surf = pGFXManager->getUIGraphic(UI_Options, pLocalHouse->getHouseID());
	surfPressed = pGFXManager->getUIGraphic(UI_Options_Pressed, pLocalHouse->getHouseID());

	Button_Options.SetSurfaces(surf, false, surfPressed, false);
	Button_Options.SetOnClickMethod(MethodPointer(currentGame, (WidgetCallbackWithNoParameter) (&Game::OnOptions)));
	TopBar_HBox.AddWidget(&Button_Options);

	TopBar_HBox.AddWidget(Spacer::Create());

	surf = pGFXManager->getUIGraphic(UI_Mentat, pLocalHouse->getHouseID());
	surfPressed = pGFXManager->getUIGraphic(UI_Mentat_Pressed, pLocalHouse->getHouseID());

	Button_Mentat.SetSurfaces(surf, false, surfPressed, false);
	Button_Mentat.SetOnClickMethod(MethodPointer(currentGame, (WidgetCallbackWithNoParameter) (&Game::OnMentat)));
	TopBar_HBox.AddWidget(&Button_Mentat);

	TopBar_HBox.AddWidget(Spacer::Create());

	// add chat manager
	WindowWidget.AddWidget(&chatManager, Point(20, 60), Point(screen->w - GameBar.GetSize().x, 360));
}

GameInterface::~GameInterface() {
	RemoveOldContainer();
}

void GameInterface::Draw(SDL_Surface* screen, Point Position) {
	Window::Draw(screen,Position);

	// draw Power Indicator and Spice indicator

	SDL_Rect PowerIndicatorPos = {	screen->w - GameBar.GetSize().x + 14, 146,
									4, screen->h - 146 - 2 };
	SDL_FillRect(screen,&PowerIndicatorPos,0);

	SDL_Rect SpiceIndicatorPos = {	screen->w - GameBar.GetSize().x + 20, 146,
									4, screen->h - 146 - 2 };
	SDL_FillRect(screen,&SpiceIndicatorPos,0);

	int xCount = 0, yCount = 0;
	int	yCount2 = 0;

	if (!SDL_MUSTLOCK(screen) || (SDL_LockSurface(screen) == 0)) {
		//draw power level indicator
		if (pLocalHouse->getPowerRequirement() == 0)	{
			if (pLocalHouse->getProducedPower() > 0) {
				yCount2 = PowerIndicatorPos.h + 1;
			} else {
				yCount2 = PowerIndicatorPos.h/2;
			}
		} else {
			yCount2 = lround((double)pLocalHouse->getProducedPower()/(double)pLocalHouse->getPowerRequirement()*(double)(PowerIndicatorPos.h/2));
		}

		if (yCount2 > PowerIndicatorPos.h + 1) {
			yCount2 = PowerIndicatorPos.h + 1;
		}

		for (yCount = 0; yCount < yCount2; yCount++) {
			for (xCount = 1; xCount < PowerIndicatorPos.w - 1; xCount++) {
				if(((yCount/2) % 3) != 0) {
					putpixel(screen, xCount + PowerIndicatorPos.x, PowerIndicatorPos.y + PowerIndicatorPos.h - yCount, COLOR_GREEN);
				}
			}
		}

		//draw spice level indicator
		if (pLocalHouse->getCapacity() == 0) {
			yCount2 = 0;
		} else {
			yCount2 = lround((double)pLocalHouse->getStoredCredits()/(double)pLocalHouse->getCapacity()*(double)SpiceIndicatorPos.h);
		}

		if (yCount2 > SpiceIndicatorPos.h + 1) {
			yCount2 = SpiceIndicatorPos.h + 1;
		}

		for (yCount = 0; yCount < yCount2; yCount++) {
			for (xCount = 1; xCount < SpiceIndicatorPos.w - 1; xCount++) {
				if(((yCount/2) % 3) != 0) {
					putpixel(screen, xCount + SpiceIndicatorPos.x, SpiceIndicatorPos.y + SpiceIndicatorPos.h - yCount, COLOR_ORANGE);
				}
			}
		}

		if (SDL_MUSTLOCK(screen)) {
				SDL_UnlockSurface(screen);
		}
	}

	//draw credits
	char CreditsBuffer[10];
	int credits = pLocalHouse->getCredits();
	sprintf(CreditsBuffer, "%d", (credits < 0) ? 0 : credits);
	int NumDigits = strlen(CreditsBuffer);
	SDL_Surface* surface = pGFXManager->getUIGraphic(UI_CreditsDigits);
	SDL_Rect source = {0, 0, surface->w/10, surface->h};
	SDL_Rect dest = {0, 135, surface->w/10, surface->h};

	for(int i=NumDigits-1; i>=0; i--) {
		source.x = atoi(&CreditsBuffer[i])*source.w;
		CreditsBuffer[i] = '\0';	//so next round atoi only does one character
		dest.x = (screen->w - GameBar.GetSize().x + 49) + (6 - NumDigits + i)*10;
		SDL_BlitSurface(surface, &source, screen, &dest);
	}
}

void GameInterface::UpdateObjectInterface() {
	if(currentGame->getSelectedList().size() == 1) {
		ObjectClass* pObject = currentGame->getObjectManager().getObject( *(currentGame->getSelectedList().begin()));
		Uint32 newObjectID = pObject->getObjectID();

		if(newObjectID != ObjectID) {
			RemoveOldContainer();

			pObjectContainer = pObject->GetInterfaceContainer();

			if(pObjectContainer != NULL) {
				ObjectID = newObjectID;

				WindowWidget.AddWidget(pObjectContainer,
										Point(screen->w - GameBar.GetSize().x + 24, 146),
										Point(GameBar.GetSize().x - 25,screen->h - 148));

			}

		} else {
			if(pObjectContainer->update() == false) {
				RemoveOldContainer();
			}
		}
	} else if(currentGame->getSelectedList().size() > 1) {

	    if((pObjectContainer == NULL) || (ObjectID != NONE)) {
	        // either there was nothing selected before or exactly one unit

            if(pObjectContainer != NULL) {
                RemoveOldContainer();
            }

            pObjectContainer = MultiUnitInterface::Create();

            WindowWidget.AddWidget(pObjectContainer,
                                    Point(screen->w - GameBar.GetSize().x + 24, 146),
                                    Point(GameBar.GetSize().x - 25,screen->h - 148));
	    } else {
	        if(pObjectContainer->update() == false) {
				RemoveOldContainer();
			}
	    }
	} else {
		RemoveOldContainer();
	}
}

void GameInterface::RemoveOldContainer() {
	if(pObjectContainer != NULL) {
		delete pObjectContainer;
		pObjectContainer = NULL;
		ObjectID = NONE;
	}
}
