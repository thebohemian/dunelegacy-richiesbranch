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

#include <Menu/MentatHelp.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <FileClasses/TextManager.h>

#include <Game.h>

MentatHelp::MentatHelp(int newHouse,int mission) : MentatMenu(newHouse) {
	SDL_Surface* surf = pGFXManager->getUIGraphic(UI_MentatExit);
	SDL_Surface* surfPressed = pGFXManager->getUIGraphic(UI_MentatExit_Pressed);
	ExitButton.SetSurfaces(surf,false,surfPressed,false);

	ExitButton.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&MentatHelp::OnExit)));
	WindowWidget.AddWidget(&ExitButton,Point(370,340),Point(surf->w,surf->h));

	SetClearScreen(false);

	int missionnumber = ((mission+1)/3)+1;
	setText(pTextManager->GetBriefingText(missionnumber,MISSION_ADVICE,house));

	Animation* anim = NULL;
    switch(missionnumber) {
        case 1:
            anim = pGFXManager->getAnimation(Anim_Harvester);
            break;
        case 2:
            anim = pGFXManager->getAnimation(Anim_Radar);
            break;
        case 3:
            anim = pGFXManager->getAnimation(Anim_Quad);
            break;
        case 4:
            anim = pGFXManager->getAnimation(Anim_Tank);
            break;
        case 5:
            anim = pGFXManager->getAnimation(Anim_RepairYard);
            break;
        case 6:
            anim = pGFXManager->getAnimation(Anim_StarPort);
            break;
        case 7:
            anim = pGFXManager->getAnimation(Anim_IX);
            break;
        case 8:
            anim = pGFXManager->getAnimation(Anim_Palace);
            break;
        default:
            anim = pGFXManager->getAnimation(Anim_ConstructionYard);
            break;
    }

    Anim.SetAnimation(anim);
    WindowWidget.AddWidget(&Anim,Point(257,97),Point(anim->getFrame()->w,anim->getFrame()->h));
}

MentatHelp::~MentatHelp() {
}

void MentatHelp::OnExit() {
	currentGame->ResumeGame();
	quit(1);
}
