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

#include <globals.h>

#include <CutScenes/Finale.h>
#include <CutScenes/WSAVideoEvent.h>
#include <CutScenes/HoldPictureVideoEvent.h>
#include <CutScenes/FadeOutVideoEvent.h>
#include <CutScenes/FadeInVideoEvent.h>
#include <CutScenes/CrossBlendVideoEvent.h>
#include <CutScenes/TextEvent.h>
#include <CutScenes/CutSceneSoundTrigger.h>
#include <CutScenes/CutSceneMusicTrigger.h>

#include <FileClasses/FileManager.h>
#include <FileClasses/Pakfile.h>
#include <FileClasses/Wsafile.h>
#include <FileClasses/Cpsfile.h>
#include <FileClasses/Vocfile.h>
#include <FileClasses/IndexedTextFile.h>

#include <misc/draw_util.h>
#include <misc/sound_util.h>

#include <string>

Finale::Finale(int house) : CutScene() {

    switch(house) {
        case HOUSE_ATREIDES: {
            SDL_RWops* afinala_wsa = pFileManager->OpenFile("AFINALA.WSA");
            if((pPalace1 = new Wsafile(afinala_wsa)) == NULL) {
                fprintf(stderr,"Finale::Finale(): Cannot open AFINALA.WSA!\n");
                exit(EXIT_FAILURE);
            }
            SDL_RWclose(afinala_wsa);

            SDL_RWops* afinalb_wsa = pFileManager->OpenFile("AFINALB.WSA");
            if((pPalace2 = new Wsafile(afinalb_wsa)) == NULL) {
                fprintf(stderr,"Finale::Finale(): Cannot open AFINALB.WSA!\n");
                exit(EXIT_FAILURE);
            }
            SDL_RWclose(afinalb_wsa);
        } break;

        case HOUSE_ORDOS: {
            SDL_RWops* ofinala_wsa = pFileManager->OpenFile("OFINALA.WSA");
            SDL_RWops* ofinalb_wsa = pFileManager->OpenFile("OFINALB.WSA");
            SDL_RWops* ofinalc_wsa = pFileManager->OpenFile("OFINALC.WSA");
            if((pPalace1 = new Wsafile(ofinala_wsa, ofinalb_wsa, ofinalc_wsa)) == NULL) {
                fprintf(stderr,"Finale::Finale(): Cannot open OFINALA.WSA, OFINALB.WSA or OFINALC.WSA!\n");
                exit(EXIT_FAILURE);
            }
            SDL_RWclose(ofinala_wsa);
            SDL_RWclose(ofinalb_wsa);
            SDL_RWclose(ofinalc_wsa);

            SDL_RWops* ofinald_wsa = pFileManager->OpenFile("OFINALD.WSA");
            if((pPalace2 = new Wsafile(ofinald_wsa)) == NULL) {
                fprintf(stderr,"Finale::Finale(): Cannot open OFINALD.WSA!\n");
                exit(EXIT_FAILURE);
            }
            SDL_RWclose(ofinald_wsa);

        } break;

        case HOUSE_HARKONNEN: {
            SDL_RWops* hfinala_wsa = pFileManager->OpenFile("HFINALA.WSA");
            if((pPalace1 = new Wsafile(hfinala_wsa)) == NULL) {
                fprintf(stderr,"Finale::Finale(): Cannot open HFINALA.WSA!\n");
                exit(EXIT_FAILURE);
            }
            SDL_RWclose(hfinala_wsa);

            SDL_RWops* hfinalb_wsa = pFileManager->OpenFile("HFINALB.WSA");
            SDL_RWops* hfinalc_wsa = pFileManager->OpenFile("HFINALC.WSA");
            if((pPalace2 = new Wsafile(hfinalb_wsa, hfinalc_wsa)) == NULL) {
                fprintf(stderr,"Finale::Finale(): Cannot open HFINALB.WSA or HFINALD.WSA!\n");
                exit(EXIT_FAILURE);
            }
            SDL_RWclose(hfinalb_wsa);
            SDL_RWclose(hfinalc_wsa);
        } break;

        default: {
            fprintf(stderr,"Finale::Finale(): Invalid house number: %d!\n", house);
            exit(EXIT_FAILURE);
        } break;
    }

    SDL_RWops* efinala_wsa = pFileManager->OpenFile("EFINALA.WSA");
    if((pImperator = new Wsafile(efinala_wsa)) == NULL) {
		fprintf(stderr,"Finale::Finale(): Cannot open EFINALA.WSA!\n");
		exit(EXIT_FAILURE);
	}
	SDL_RWclose(efinala_wsa);

    SDL_RWops* efinalb_wsa = pFileManager->OpenFile("EFINALB.WSA");
    if((pImperatorShocked = new Wsafile(efinalb_wsa)) == NULL) {
		fprintf(stderr,"Finale::Finale(): Cannot open EFINALB.WSA!\n");
		exit(EXIT_FAILURE);
	}
	SDL_RWclose(efinalb_wsa);

    SDL_Surface* pPlanetDuneNormalSurface;
    if((pPlanetDuneNormalSurface = LoadCPS_RW(pFileManager->OpenFile("BIGPLAN.CPS"),true)) == NULL) {
		fprintf(stderr,"Finale::Finale(): Cannot open BIGPLAN.CPS!\n");
		exit(EXIT_FAILURE);
	}

	SDL_Surface* pTempSurface;
    if((pTempSurface = LoadCPS_RW(pFileManager->OpenFile("MAPPLAN.CPS"),true)) == NULL) {
		fprintf(stderr,"Finale::Finale(): Cannot open MAPPLAN.CPS!\n");
		exit(EXIT_FAILURE);
	}
	SDL_Surface* pPlanetDuneInHouseColorSurface = MapImageHouseColor(pTempSurface, house, houseColor[HOUSE_HARKONNEN]);
	SDL_FreeSurface(pTempSurface);

    lizard = GetChunkFromFile("LIZARD1.VOC");
    glass = GetChunkFromFile("GLASS6.VOC");
    click = GetChunkFromFile("CLICK.VOC");
    blaster = GetChunkFromFile("BLASTER.VOC");
    blowup = GetChunkFromFile("BLOWUP1.VOC");

	SDL_RWops* intro_lng = pFileManager->OpenFile("INTRO." + settings.General.LanguageExt);
	IndexedTextFile* pIntroText = new IndexedTextFile(intro_lng);
	SDL_RWclose(intro_lng);

	switch(house) {
        case HOUSE_ATREIDES: {
            startNewScene();

            addVideoEvent(new FadeInVideoEvent(pPalace1->getPicture(0), 20, true));
            addVideoEvent(new HoldPictureVideoEvent(pPalace1->getPicture(0), 50, true));
            addTextEvent(new TextEvent(pIntroText->getString(FinaleText_Greetings_Emperor),15,48,false,true,false,houseColor[house]+1));
            addTrigger(new CutSceneMusicTrigger(0,MUSIC_FINALE_A));

            startNewScene();

            addVideoEvent(new WSAVideoEvent(pImperator));
            addVideoEvent(new HoldPictureVideoEvent(pImperator->getPicture(pImperator->getNumFrames()-1), 3, true));
            addTextEvent(new TextEvent(pIntroText->getString(FinaleText_What_is_the_meaning),2,100,false,false,false,COLOR_SARDAUKAR+1));

            startNewScene();

            addVideoEvent(new WSAVideoEvent(pPalace1));
            addVideoEvent(new HoldPictureVideoEvent(pPalace1->getPicture(pPalace1->getNumFrames()-1), 25, true));
            addTextEvent(new TextEvent(pIntroText->getString(FinaleText_You_are_formally_charged),0,105,true,false,false,houseColor[house]+1));

            startNewScene();

            addVideoEvent(new HoldPictureVideoEvent(pImperatorShocked->getPicture(0), 34, true));
            addVideoEvent(new HoldPictureVideoEvent(pImperatorShocked->getPicture(1), 20, true));
            addTextEvent(new TextEvent(pIntroText->getString(FinaleText_The_House_shall_determine),2,40,false,false,false,houseColor[house]+1));

            startNewScene();

            addVideoEvent(new HoldPictureVideoEvent(pPalace2->getPicture(0), 15, true));
            addVideoEvent(new WSAVideoEvent(pPalace2));
            addVideoEvent(new HoldPictureVideoEvent(pPalace2->getPicture(pPalace2->getNumFrames()-1), 30, true));
            addVideoEvent(new FadeOutVideoEvent(pPalace2->getPicture(pPalace2->getNumFrames()-1), 20, true));
            addTextEvent(new TextEvent(pIntroText->getString(FinaleText_Until_then_you_shall_no),2,48,false,true,false,houseColor[house]+1));

        } break;

        case HOUSE_ORDOS: {
            startNewScene();

            addVideoEvent(new FadeInVideoEvent(pPalace1->getPicture(0), 20, true));
            addVideoEvent(new HoldPictureVideoEvent(pPalace1->getPicture(0), 50, true));
            addTextEvent(new TextEvent(pIntroText->getString(FinaleText_You_are_aware_Emperor),22,46,false,true,false,houseColor[house]+1));
            addTrigger(new CutSceneMusicTrigger(0,MUSIC_FINALE_O));

            startNewScene();

            addVideoEvent(new WSAVideoEvent(pImperator));
            addVideoEvent(new HoldPictureVideoEvent(pImperator->getPicture(pImperator->getNumFrames()-1), 3, true));
            addTextEvent(new TextEvent(pIntroText->getString(FinaleText_What_games_What_are_you),2,100,false,true,false,COLOR_SARDAUKAR+1));

            startNewScene();

            addVideoEvent(new HoldPictureVideoEvent(pPalace1->getPicture(0), 40, true));
            addVideoEvent(new WSAVideoEvent(pPalace1));
            addVideoEvent(new HoldPictureVideoEvent(pPalace1->getPicture(pPalace1->getNumFrames()-1), 65, true));
            addTextEvent(new TextEvent(pIntroText->getString(FinaleText_I_am_referring_to_your_game),2,35,false,true,false,houseColor[house]+1));
            addTextEvent(new TextEvent(pIntroText->getString(FinaleText_We_were_your_pawns_and_Dune),40,45,true,true,false,houseColor[house]+1));
            addTextEvent(new TextEvent(pIntroText->getString(FinaleText_We_have_decided_to_take),88,105,true,false,false,houseColor[house]+1));
            addTrigger(new CutSceneSoundTrigger(42,lizard));
            addTrigger(new CutSceneSoundTrigger(62,lizard));

            startNewScene();

            addVideoEvent(new HoldPictureVideoEvent(pImperatorShocked->getPicture(0), 29, true));
            addVideoEvent(new HoldPictureVideoEvent(pImperatorShocked->getPicture(1), 20, true));
            addTextEvent(new TextEvent(pIntroText->getString(FinaleText_You_are_to_be_our_pawn),2,47,false,true,false,houseColor[house]+1));

            startNewScene();

            addVideoEvent(new WSAVideoEvent(pPalace2));
            addVideoEvent(new HoldPictureVideoEvent(pPalace2->getPicture(pPalace2->getNumFrames()-1), 15, true));
            addVideoEvent(new FadeOutVideoEvent(pPalace2->getPicture(pPalace2->getNumFrames()-1), 20, true));

        } break;

        case HOUSE_HARKONNEN: {
            startNewScene();

            addVideoEvent(new FadeInVideoEvent(pPalace1->getPicture(0), 20, true));
            addVideoEvent(new HoldPictureVideoEvent(pPalace1->getPicture(0), 25, true));
            addVideoEvent(new WSAVideoEvent(pPalace1));
            addVideoEvent(new HoldPictureVideoEvent(pPalace1->getPicture(0), 52, true));
            addVideoEvent(new WSAVideoEvent(pPalace1));
            addVideoEvent(new HoldPictureVideoEvent(pPalace1->getPicture(0), 23, true));
            addTextEvent(new TextEvent(pIntroText->getString(FinaleText_You_are_indeed_not_entirely),22,47,false,true,false,houseColor[house]+1));
            addTextEvent(new TextEvent(pIntroText->getString(FinaleText_You_have_lied_to_us),70,60,true,true,false,houseColor[house]+1));
            addTrigger(new CutSceneMusicTrigger(0,MUSIC_FINALE_H));

            startNewScene();

            addVideoEvent(new WSAVideoEvent(pImperator));
            addVideoEvent(new HoldPictureVideoEvent(pImperator->getPicture(pImperator->getNumFrames()-1), 3, true));
            addTextEvent(new TextEvent(pIntroText->getString(FinaleText_What_lies_What_are),2,100,false,true,false,COLOR_SARDAUKAR+1));

            startNewScene();

            addVideoEvent(new HoldPictureVideoEvent(pPalace1->getPicture(0), 50, true));
            addTextEvent(new TextEvent(pIntroText->getString(FinaleText_Your_lies_of_loyalty),0,50,true,true,false,houseColor[house]+1));

            startNewScene();

            addVideoEvent(new HoldPictureVideoEvent(pImperatorShocked->getPicture(0), 45, true));
            addVideoEvent(new HoldPictureVideoEvent(pImperatorShocked->getPicture(1), 15, true));
            addTextEvent(new TextEvent(pIntroText->getString(FinaleText_A_crime_for_which_you),2,38,true,false,false,houseColor[house]+1));
            addTextEvent(new TextEvent(pIntroText->getString(FinaleText_with_your_life),42,100,false,false,false,houseColor[house]+1));

            startNewScene();

            addVideoEvent(new HoldPictureVideoEvent(pPalace2->getPicture(0), 5, true));
            addVideoEvent(new WSAVideoEvent(pPalace2));
            addVideoEvent(new HoldPictureVideoEvent(pPalace2->getPicture(pPalace2->getNumFrames()-1), 15, true));
            addVideoEvent(new FadeOutVideoEvent(pPalace2->getPicture(pPalace2->getNumFrames()-1), 20, true));
            addTextEvent(new TextEvent(pIntroText->getString(FinaleText_NO_NO_NOOO),10,30,false,true,false,COLOR_SARDAUKAR+1));
            addTrigger(new CutSceneSoundTrigger(10,click));
            addTrigger(new CutSceneSoundTrigger(15,blaster));
            addTrigger(new CutSceneSoundTrigger(17,blowup));
            addTrigger(new CutSceneSoundTrigger(38,click));
            addTrigger(new CutSceneSoundTrigger(43,blaster));
            addTrigger(new CutSceneSoundTrigger(45,blowup));
            addTrigger(new CutSceneSoundTrigger(67,glass));

        } break;

        default: {
            fprintf(stderr,"Finale::Finale(): Invalid house number: %d!\n", house);
            exit(EXIT_FAILURE);
        }
	}

    addVideoEvent(new FadeInVideoEvent(pPlanetDuneNormalSurface, 20, false));
    addVideoEvent(new HoldPictureVideoEvent(pPlanetDuneNormalSurface, 10, false));
    addVideoEvent(new CrossBlendVideoEvent(pPlanetDuneNormalSurface, pPlanetDuneInHouseColorSurface, false));
    addVideoEvent(new HoldPictureVideoEvent(pPlanetDuneInHouseColorSurface, 50, false));
    addVideoEvent(new FadeOutVideoEvent(pPlanetDuneInHouseColorSurface, 20, false));
    addVideoEvent(new HoldPictureVideoEvent(NULL, 10, false));

	delete pIntroText;

	SDL_FreeSurface(pPlanetDuneNormalSurface);
	SDL_FreeSurface(pPlanetDuneInHouseColorSurface);
}

Finale::~Finale() {
    delete pPalace1;
    delete pPalace2;
    delete pImperator;
    delete pImperatorShocked;

    Mix_FreeChunk(lizard);
    Mix_FreeChunk(glass);
    Mix_FreeChunk(click);
    Mix_FreeChunk(blaster);
    Mix_FreeChunk(blowup);
}
