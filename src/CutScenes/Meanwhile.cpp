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

#include <CutScenes/Meanwhile.h>
#include <CutScenes/WSAVideoEvent.h>
#include <CutScenes/HoldPictureVideoEvent.h>
#include <CutScenes/FadeOutVideoEvent.h>
#include <CutScenes/FadeInVideoEvent.h>
#include <CutScenes/TextEvent.h>
#include <CutScenes/CutSceneMusicTrigger.h>

#include <FileClasses/FileManager.h>
#include <FileClasses/TextManager.h>
#include <FileClasses/Pakfile.h>
#include <FileClasses/Wsafile.h>

#include <string>

Meanwhile::Meanwhile(int house, bool firstMeanwhile) : CutScene() {

    if(house != HOUSE_ATREIDES && house != HOUSE_ORDOS && house != HOUSE_HARKONNEN) {
		fprintf(stderr,"Meanwhile::Meanwhile(): Invalid house number: %d!\n", house);
		exit(EXIT_FAILURE);
    }

    int textBaseIndex = MeanwhileText_Base + house * MeanwhileText_NumTextsPerHouse;

    SDL_RWops* meanwhil_wsa = pFileManager->OpenFile("MEANWHIL.WSA");
    if((pMeanwhile = new Wsafile(meanwhil_wsa)) == NULL) {
		fprintf(stderr,"Meanwhile::Meanwhile(): Cannot open MEANWHIL.WSA!\n");
		exit(EXIT_FAILURE);
	}
	SDL_RWclose(meanwhil_wsa);

    SDL_RWops* efinala_wsa = pFileManager->OpenFile("EFINALA.WSA");
    if((pImperator = new Wsafile(efinala_wsa)) == NULL) {
		fprintf(stderr,"Intro::Intro(): Cannot open EFINALA.WSA!\n");
		exit(EXIT_FAILURE);
	}
	SDL_RWclose(efinala_wsa);

	if(firstMeanwhile == true) {
        // Meanwhile after level 4
        int houseOfVisitor = (house+2)%3;

        startNewScene();

        addVideoEvent(new HoldPictureVideoEvent(NULL, 45, true));
        addTextEvent(new TextEvent(pTextManager->getDuneText(textBaseIndex+MeanwhileText_At_the_Emperor_s_Palace),0,42,true,true,true,houseColor[house]+1));
        addTrigger(new CutSceneMusicTrigger(0,MUSIC_MEANWHILE));

        startNewScene();

        addVideoEvent(new HoldPictureVideoEvent(pMeanwhile->getPicture(houseOfVisitor), 75, true));
        addTextEvent(new TextEvent(pTextManager->getDuneText(textBaseIndex+MeanwhileText_You_of_all_people),0,45,true,true,false,COLOR_SARDAUKAR+1));
        addTextEvent(new TextEvent(pTextManager->getDuneText(textBaseIndex+MeanwhileText_Yes_your_excellency_I),45,30,true,false,false,houseColor[houseOfVisitor]+1));

        startNewScene();

        addVideoEvent(new WSAVideoEvent(pImperator));
        addVideoEvent(new HoldPictureVideoEvent(pImperator->getPicture(pImperator->getNumFrames()-1), 3, true));
        addTextEvent(new TextEvent(pTextManager->getDuneText(textBaseIndex+MeanwhileText_You_let_the),3,100,false,false,false,COLOR_SARDAUKAR+1));

        startNewScene();
        addVideoEvent(new HoldPictureVideoEvent(pMeanwhile->getPicture(houseOfVisitor), 75, true));
        addVideoEvent(new FadeOutVideoEvent(pMeanwhile->getPicture(houseOfVisitor), 20, true));
        addTextEvent(new TextEvent(pTextManager->getDuneText(textBaseIndex+MeanwhileText_I_did_not_let),0,35,true,false,false,houseColor[houseOfVisitor]+1));
        addTextEvent(new TextEvent(pTextManager->getDuneText(textBaseIndex+MeanwhileText_I_will_not_allow),37,38,false,true,false,COLOR_SARDAUKAR+1));

	} else {
        // Meanwhile after level 8
        int houseOfVisitor = (house+2)%3;

        startNewScene();

        addVideoEvent(new HoldPictureVideoEvent(NULL, 45, true));
        addTextEvent(new TextEvent(pTextManager->getDuneText(textBaseIndex+MeanwhileText_At_the_Emperor_s_Palace_on_Dune),0,42,true,true,true,houseColor[house]+1));
        addTrigger(new CutSceneMusicTrigger(0,MUSIC_MEANWHILE));

        if(house == HOUSE_ATREIDES) {
            startNewScene();

            addVideoEvent(new HoldPictureVideoEvent(pMeanwhile->getPicture(5-house), 130, true));
            addTextEvent(new TextEvent(pTextManager->getDuneText(textBaseIndex+MeanwhileText_Fools),0,45,true,false,false,COLOR_SARDAUKAR));
            addTextEvent(new TextEvent(pTextManager->getDuneText(textBaseIndex+MeanwhileText_And_still_you_fail),50,45,false,false,false,COLOR_SARDAUKAR+1));
            addTextEvent(new TextEvent(pTextManager->getDuneText(textBaseIndex+MeanwhileText_But_excell),100,30,true,false,false,houseColor[houseOfVisitor]+1));

            startNewScene();

            addVideoEvent(new WSAVideoEvent(pImperator));
            addVideoEvent(new HoldPictureVideoEvent(pImperator->getPicture(pImperator->getNumFrames()-1), 3, true));
            addVideoEvent(new FadeOutVideoEvent(pImperator->getPicture(pImperator->getNumFrames()-1), 20, true));
            addTextEvent(new TextEvent(pTextManager->getDuneText(textBaseIndex+MeanwhileText_Enough_Together_we_must),3,42,false,true,false,COLOR_SARDAUKAR+1));
        } else {
            startNewScene();

            addVideoEvent(new HoldPictureVideoEvent(pMeanwhile->getPicture(5-house), 80, true));
            addTextEvent(new TextEvent(pTextManager->getDuneText(textBaseIndex+MeanwhileText_The_Ordos_were_not_supposed),0,45,true,true,false,COLOR_SARDAUKAR+1));
            addTextEvent(new TextEvent(pTextManager->getDuneText(textBaseIndex+MeanwhileText_Your_highness),46,35,true,false,false,houseColor[houseOfVisitor]+1));

            startNewScene();

            addVideoEvent(new WSAVideoEvent(pImperator));
            addVideoEvent(new WSAVideoEvent(pImperator));
            addVideoEvent(new FadeOutVideoEvent(pImperator->getPicture(pImperator->getNumFrames()-1), 20, true));

            addTextEvent(new TextEvent(pTextManager->getDuneText(textBaseIndex+MeanwhileText_No_more_explanations),3, (house == HOUSE_ORDOS) ? 21 : 11,false,false,false,COLOR_SARDAUKAR+1));
            addTextEvent(new TextEvent(pTextManager->getDuneText(textBaseIndex+MeanwhileText_Only_together_will_we),(house == HOUSE_ORDOS) ? 28 : 18,(house == HOUSE_ORDOS) ? 39 : 49,false,true,false,COLOR_SARDAUKAR+1));
        }

	}
}

Meanwhile::~Meanwhile() {
    delete pMeanwhile;
    delete pImperator;
}
