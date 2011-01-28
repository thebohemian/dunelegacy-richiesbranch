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

#include <Menu/BriefingMenu.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <FileClasses/TextManager.h>
#include <FileClasses/music/MusicPlayer.h>

#include <stdlib.h>

BriefingMenu::BriefingMenu(int newHouse,int mission,int type) : MentatMenu(newHouse) {
	this->mission = mission;
    this->type = type;

	Animation* anim = NULL;

	SDL_Surface* surf;
	SDL_Surface* surfPressed;

	surf = pGFXManager->getUIGraphic(UI_MentatProcced);
	surfPressed = pGFXManager->getUIGraphic(UI_MentatProcced_Pressed);
	ProccedButton.SetSurfaces(surf,false,surfPressed,false);
	ProccedButton.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&BriefingMenu::OnProcced)));
	WindowWidget.AddWidget(&ProccedButton,Point(500,340),Point(surf->w,surf->h));

	int missionnumber;
	if(mission != 22) {
		missionnumber = ((mission+1)/3)+1;
	} else {
		missionnumber = 9;
	}

	switch(type) {
		case DEBRIEFING_WIN:
			anim = pGFXManager->getAnimation((rand() % 2 == 0) ? Anim_Win1 : Anim_Win2);
			setText(pTextManager->GetBriefingText(missionnumber,MISSION_WIN,house));
			break;
		case DEBRIEFING_LOST:
			anim = pGFXManager->getAnimation((rand() % 2 == 0) ? Anim_Lose1 : Anim_Lose2);
			setText(pTextManager->GetBriefingText(missionnumber,MISSION_LOSE,house));
			break;
		default:
		case BRIEFING:
		{
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
					anim = pGFXManager->getAnimation(Anim_HeavyFactory);
					break;
				case 7:
					anim = pGFXManager->getAnimation(Anim_IX);
					break;
				case 8:
					anim = pGFXManager->getAnimation(Anim_Palace);
					break;
				case 9:
					anim = pGFXManager->getAnimation(Anim_Sardaukar);
					break;
				default:
					anim = pGFXManager->getAnimation(Anim_ConstructionYard);
					break;
			}
			setText(pTextManager->GetBriefingText(missionnumber,MISSION_DESCRIPTION,house));
		} break;
	}
	Anim.SetAnimation(anim);
	WindowWidget.AddWidget(&Anim,Point(257,97),Point(anim->getFrame()->w,anim->getFrame()->h));
}

BriefingMenu::~BriefingMenu() {
}

int BriefingMenu::showMenu()
{
    switch(type) {
        case DEBRIEFING_WIN:
        {
            switch(house) {
                case HOUSE_ATREIDES:
                case HOUSE_FREMEN:
                    musicPlayer->changeMusic(MUSIC_WIN_A);
                    break;
                case HOUSE_ORDOS:
                case HOUSE_MERCENARY:
                    musicPlayer->changeMusic(MUSIC_WIN_O);
                    break;
                case HOUSE_HARKONNEN:
                case HOUSE_SARDAUKAR:
                    musicPlayer->changeMusic(MUSIC_WIN_H);
                    break;
            }
        } break;

        case DEBRIEFING_LOST:
        {
            musicPlayer->changeMusic(MUSIC_LOSE);
        } break;

        case BRIEFING:
        {
            switch(house) {
                case HOUSE_ATREIDES:
                case HOUSE_FREMEN:
                    musicPlayer->changeMusic(MUSIC_BRIEFING_A);
                    break;
                case HOUSE_ORDOS:
                case HOUSE_MERCENARY:
                    musicPlayer->changeMusic(MUSIC_BRIEFING_O);
                    break;
                case HOUSE_HARKONNEN:
                case HOUSE_SARDAUKAR:
                    musicPlayer->changeMusic(MUSIC_BRIEFING_H);
                    break;
            }
        } break;
    }

    return MentatMenu::showMenu();
}

void BriefingMenu::OnRepeat() {

}

void BriefingMenu::OnProcced() {
	quit(0);
}
