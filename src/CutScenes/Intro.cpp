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

#include <CutScenes/Intro.h>
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
#include <FileClasses/Vocfile.h>
#include <FileClasses/IndexedTextFile.h>

#include <misc/sound_util.h>

#include <string>

const char* Intro::VoiceFileNames[Voice_NUM_ENTRIES] =   {  "BLDING.VOC",
                                                            "DYNASTY.VOC",
                                                            "PLANET.VOC",
                                                            "KNOWN.VOC",
                                                            "SANDLAND.VOC",
                                                            "HOME.VOC",
                                                            "SPICE.VOC",
                                                            "MELANGE.VOC",
                                                            "SPICE2.VOC",
                                                            "CONTROLS.VOC",
                                                            "EMPIRE.VOC",
                                                            "WHOEVER.VOC",
                                                            "CONTROL2.VOC",
                                                            "CONTROL3.VOC",
                                                            "KING.VOC",
                                                            "PROPOSED.VOC",
                                                            "EACHHOME.VOC",
                                                            "EHOUSE.VOC",
                                                            "EPRODUCE.VOC",
                                                            "EMOST.VOC",
                                                            "ECONTROL.VOC",
                                                            "ENOSET.VOC",
                                                            "ETERRIT.VOC",
                                                            "EANDNO.VOC",
                                                            "ERULES.VOC",
                                                            "VAST.VOC",
                                                            "ARRIVED.VOC",
                                                            "ANDNOW.VOC",
                                                            "3HOUSES.VOC",
                                                            "CONTROL4.VOC",
                                                            "OFDUNE.VOC",
                                                            "NOBLE.VOC",
                                                            "INSID.VOC",
                                                            "ORD.VOC",
                                                            "EVIL.VOC",
                                                            "HARK.VOC",
                                                            "HOUSE2.VOC",
                                                            "PREVAIL.VOC",
                                                            "YOUR.VOC",
                                                            "BATTLE.VOC",
                                                            "BEGINS.VOC",
                                                            "NOW.VOC"
                                                        };

Intro::Intro() : CutScene() {

    SDL_RWops* intro1_wsa = pFileManager->OpenFile("INTRO1.WSA");
    if((pDuneText = new Wsafile(intro1_wsa)) == NULL) {
		fprintf(stderr,"Intro::Intro(): Cannot open INTRO1.WSA!\n");
		exit(EXIT_FAILURE);
	}
	SDL_RWclose(intro1_wsa);

    SDL_RWops* intro2_wsa = pFileManager->OpenFile("INTRO2.WSA");
    if((pPlanet = new Wsafile(intro2_wsa)) == NULL) {
		fprintf(stderr,"Intro::Intro(): Cannot open INTRO2.WSA!\n");
		exit(EXIT_FAILURE);
	}
	SDL_RWclose(intro2_wsa);

    SDL_RWops* intro3_wsa = pFileManager->OpenFile("INTRO3.WSA");
    if((pSandstorm = new Wsafile(intro3_wsa)) == NULL) {
		fprintf(stderr,"Intro::Intro(): Cannot open INTRO3.WSA!\n");
		exit(EXIT_FAILURE);
	}
	SDL_RWclose(intro3_wsa);

    SDL_RWops* intro9_wsa = pFileManager->OpenFile("INTRO9.WSA");
    if((pHarvesters = new Wsafile(intro9_wsa)) == NULL) {
		fprintf(stderr,"Intro::Intro(): Cannot open INTRO9.WSA!\n");
		exit(EXIT_FAILURE);
	}
	SDL_RWclose(intro9_wsa);

    SDL_RWops* intro10_wsa = pFileManager->OpenFile("INTRO10.WSA");
    if((pPalace = new Wsafile(intro10_wsa)) == NULL) {
		fprintf(stderr,"Intro::Intro(): Cannot open INTRO10.WSA!\n");
		exit(EXIT_FAILURE);
	}
	SDL_RWclose(intro10_wsa);

    SDL_RWops* intro11_wsa = pFileManager->OpenFile("INTRO11.WSA");
    if((pImperator = new Wsafile(intro11_wsa)) == NULL) {
		fprintf(stderr,"Intro::Intro(): Cannot open INTRO11.WSA!\n");
		exit(EXIT_FAILURE);
	}
	SDL_RWclose(intro11_wsa);

    SDL_RWops* intro4_wsa = pFileManager->OpenFile("INTRO4.WSA");
    if((pStarport = new Wsafile(intro4_wsa)) == NULL) {
		fprintf(stderr,"Intro::Intro(): Cannot open INTRO4.WSA!\n");
		exit(EXIT_FAILURE);
	}
	SDL_RWclose(intro4_wsa);

    SDL_RWops* intro6_wsa = pFileManager->OpenFile("INTRO6.WSA");
    if((pAtreides = new Wsafile(intro6_wsa)) == NULL) {
		fprintf(stderr,"Intro::Intro(): Cannot open INTRO6.WSA!\n");
		exit(EXIT_FAILURE);
	}
	SDL_RWclose(intro6_wsa);

    SDL_RWops* intro7a_wsa = pFileManager->OpenFile("INTRO7A.WSA");
    SDL_RWops* intro7b_wsa = pFileManager->OpenFile("INTRO7B.WSA");
    if((pOrdos = new Wsafile(intro7a_wsa, intro7b_wsa)) == NULL) {
		fprintf(stderr,"Intro::Intro(): Cannot open INTRO7A.WSA or INTRO7B.WSA!\n");
		exit(EXIT_FAILURE);
	}
	SDL_RWclose(intro7a_wsa);
    SDL_RWclose(intro7b_wsa);

    SDL_RWops* intro8a_wsa = pFileManager->OpenFile("INTRO8A.WSA");
    SDL_RWops* intro8b_wsa = pFileManager->OpenFile("INTRO8B.WSA");
    SDL_RWops* intro8c_wsa = pFileManager->OpenFile("INTRO8C.WSA");
    if((pHarkonnen = new Wsafile(intro8a_wsa, intro8b_wsa, intro8c_wsa)) == NULL) {
		fprintf(stderr,"Intro::Intro(): Cannot open INTRO8A.WSA, INTRO8B.WSA or INTRO8C.WSA!\n");
		exit(EXIT_FAILURE);
	}
	SDL_RWclose(intro8a_wsa);
    SDL_RWclose(intro8b_wsa);
    SDL_RWclose(intro8c_wsa);

    SDL_RWops* intro5_wsa = pFileManager->OpenFile("INTRO5.WSA");
    if((pDestroyedTank = new Wsafile(intro5_wsa)) == NULL) {
		fprintf(stderr,"Intro::Intro(): Cannot open INTRO5.WSA!\n");
		exit(EXIT_FAILURE);
	}
	SDL_RWclose(intro5_wsa);

	SDL_RWops* intro_lng = pFileManager->OpenFile("INTRO." + settings.General.LanguageExt);
	IndexedTextFile* pIntroText = new IndexedTextFile(intro_lng);
	SDL_RWclose(intro_lng);


    wind = GetChunkFromFile("WIND2BP.VOC");
    carryallLanding = GetChunkFromFile("CLANK.VOC");
    harvester = GetChunkFromFile("BRAKES2P.VOC");

    Mix_Chunk* singleGunshot = GetChunkFromFile("GUNSHOT.VOC");
    Mix_Chunk* fourGunshots = Concat4Chunks(singleGunshot, singleGunshot, singleGunshot, singleGunshot);
    Mix_Chunk* thirteenGunshots = Concat4Chunks(fourGunshots, fourGunshots, fourGunshots, singleGunshot);
    gunshot = Concat3Chunks(thirteenGunshots, singleGunshot, singleGunshot);
    Mix_FreeChunk(singleGunshot);
    Mix_FreeChunk(fourGunshots);
    Mix_FreeChunk(thirteenGunshots);

    glass = GetChunkFromFile("GLASS.VOC");
    missle = GetChunkFromFile("MISSLE8.VOC");
    blaster = GetChunkFromFile("BLASTER.VOC");
    blowup1 = GetChunkFromFile("BLOWUP1.VOC");
    blowup2 = GetChunkFromFile("BLOWUP2.VOC");


    bool bEnableVoice = (settings.General.Language == LNG_ENG);
    if(bEnableVoice == true) {
        // Load english voice
        for(int i=0;i<Voice_NUM_ENTRIES;i++) {
            Voice[i] = GetChunkFromFile(VoiceFileNames[i]);
        }
    } else {
        for(int i=0;i<Voice_NUM_ENTRIES;i++) {
            Voice[i] = NULL;
        }
    }


	startNewScene();

    addVideoEvent(new WSAVideoEvent(pDuneText, false));
    addVideoEvent(new HoldPictureVideoEvent(pDuneText->getPicture(pDuneText->getNumFrames()-1), 30, true, false));
    addVideoEvent(new FadeOutVideoEvent(pDuneText->getPicture(pDuneText->getNumFrames()-1), 20, true, false));
    addTextEvent(new TextEvent(pIntroText->getString(IntroText_The_Battle_for_Arrakis),48,40,true,true,true));
    addTextEvent(new TextEvent("The remake is called Dune Legacy!",48,40,true));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(52,Voice[Voice_The_building]));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(64,Voice[Voice_of_a_Dynasty]));

    startNewScene();

    addVideoEvent(new FadeInVideoEvent(pPlanet->getPicture(0), 20, true));
    addVideoEvent(new WSAVideoEvent(pPlanet));
    addVideoEvent(new HoldPictureVideoEvent(pPlanet->getPicture(pPlanet->getNumFrames()-1), 20, true));
    addVideoEvent(new FadeOutVideoEvent(pPlanet->getPicture(pPlanet->getNumFrames()-1), 20, true));
    addTextEvent(new TextEvent(pIntroText->getString(IntroText_The_planet_Arrakis),20,60,true,true,false));
    addTrigger(new CutSceneMusicTrigger(25,MUSIC_INTRO));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(25,Voice[Voice_The_Planet_Arrakis]));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(38,Voice[Voice_Known_As_Dune]));

    startNewScene();

    addVideoEvent(new FadeInVideoEvent(pSandstorm->getPicture(0), 20, true));
    addVideoEvent(new WSAVideoEvent(pSandstorm));
    addVideoEvent(new HoldPictureVideoEvent(pSandstorm->getPicture(pSandstorm->getNumFrames()-1), 50, true));
    addTextEvent(new TextEvent(pIntroText->getString(IntroText_Land_of_sand),20,40,true,true));
    addTextEvent(new TextEvent(pIntroText->getString(IntroText_Home_of_the_Spice_Melange),61,45,true,true,false));
    addTrigger(new CutSceneSoundTrigger(25,wind));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(15,Voice[Voice_Land_of_sand]));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(70,Voice[Voice_Home]));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(78,Voice[Voice_of_the_spice]));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(92,Voice[Voice_Melange]));

    startNewScene();

    addVideoEvent(new CrossBlendVideoEvent(pSandstorm->getPicture(pSandstorm->getNumFrames()-1), pHarvesters->getPicture(0), true));
    addVideoEvent(new WSAVideoEvent(pHarvesters));
    addVideoEvent(new HoldPictureVideoEvent(pHarvesters->getPicture(pHarvesters->getNumFrames()-1), 22, true));
    addVideoEvent(new FadeOutVideoEvent(pHarvesters->getPicture(pHarvesters->getNumFrames()-1), 20, true));
    addTextEvent(new TextEvent(pIntroText->getString(IntroText_Spice_controls_the_Empire),25,40,true,true,false));
    addTextEvent(new TextEvent(pIntroText->getString(IntroText_Whoever_controls_Dune),66,55,true,true,false));
    addTrigger(new CutSceneSoundTrigger(45,carryallLanding));
    addTrigger(new CutSceneSoundTrigger(79,harvester));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(40,Voice[Voice_The_spice]));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(47,Voice[Voice_controls]));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(53,Voice[Voice_the_Empire]));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(90,Voice[Voice_Whoever]));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(96,Voice[Voice_controls_dune]));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(108,Voice[Voice_controls_the_spice]));

    startNewScene();

    addVideoEvent(new FadeInVideoEvent(pPalace->getPicture(0), 20, true));
    addVideoEvent(new HoldPictureVideoEvent(pPalace->getPicture(0), 50, true));
    addTextEvent(new TextEvent(pIntroText->getString(IntroText_The_Emperor_has_proposed),20,48,true,true,false));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(22,Voice[Voice_The_Emperor]));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(31,Voice[Voice_has_proposed]));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(49,Voice[Voice_to_each_of_the_houses]));

    startNewScene();

    addVideoEvent(new WSAVideoEvent(pImperator));
    addVideoEvent(new HoldPictureVideoEvent(pImperator->getPicture(pImperator->getNumFrames()-1), 30, true));
    addVideoEvent(new WSAVideoEvent(pImperator));
    addVideoEvent(new HoldPictureVideoEvent(pImperator->getPicture(pImperator->getNumFrames()-1), 15, true));
    addVideoEvent(new FadeOutVideoEvent(pImperator->getPicture(pImperator->getNumFrames()-1), 20, true));
    addTextEvent(new TextEvent(pIntroText->getString(IntroText_The_House_that_produces),0,52,true,true,false,COLOR_SARDAUKAR+1));
    addTextEvent(new TextEvent(pIntroText->getString(IntroText_There_are_no_set_territories),68,30,true,true,false,COLOR_SARDAUKAR+1));
    addTextEvent(new TextEvent(pIntroText->getString(IntroText_And_no_rules_of_engagement),99,30,true,true,false,COLOR_SARDAUKAR+1));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(8,Voice[Voice_The_House]));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(14,Voice[Voice_that_produces]));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(20,Voice[Voice_the_most_spice]));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(30,Voice[Voice_will_control_dune]));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(81,Voice[Voice_There_are_no_set]));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(89,Voice[Voice_territories]));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(98,Voice[Voice_and_no]));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(102,Voice[Voice_rules_of_engagment]));

    startNewScene();

    addVideoEvent(new FadeInVideoEvent(pStarport->getPicture(0), 20, true));
    addVideoEvent(new WSAVideoEvent(pStarport));
    addVideoEvent(new HoldPictureVideoEvent(pStarport->getPicture(pStarport->getNumFrames()-1), 20, true));
    addVideoEvent(new FadeOutVideoEvent(pStarport->getPicture(pStarport->getNumFrames()-1), 20, true));
    addTextEvent(new TextEvent(pIntroText->getString(IntroText_Vast_armies_have_arrived),25,60,true,true,false));
    addTrigger(new CutSceneSoundTrigger(57,carryallLanding));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(30,Voice[Voice_Vast_armies]));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(37,Voice[Voice_have_arrived]));

    startNewScene();

    addVideoEvent(new HoldPictureVideoEvent(NULL, 80, true));
    addTextEvent(new TextEvent(pIntroText->getString(IntroText_Now_three_houses_fight),0,80,true,false,true));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(10,Voice[Voice_Now]));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(17,Voice[Voice_three_Houses_fight]));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(34,Voice[Voice_for_control]));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(46,Voice[Voice_of_Dune]));

    startNewScene();

    addVideoEvent(new WSAVideoEvent(pAtreides));
    addVideoEvent(new HoldPictureVideoEvent(pAtreides->getPicture(pAtreides->getNumFrames()-1), 8, true));
    addTextEvent(new TextEvent(pIntroText->getString(IntroText_The_noble_Atreides),25,58,true,true,false));
    addTrigger(new CutSceneSoundTrigger(21,gunshot));
    addTrigger(new CutSceneSoundTrigger(31,glass));
    addTrigger(new CutSceneSoundTrigger(32,glass));
    addTrigger(new CutSceneSoundTrigger(33,glass));
    addTrigger(new CutSceneSoundTrigger(51,gunshot));
    addTrigger(new CutSceneSoundTrigger(61,glass));
    addTrigger(new CutSceneSoundTrigger(62,glass));
    addTrigger(new CutSceneSoundTrigger(63,glass));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(36,Voice[Voice_The_noble_Atreides]));


    startNewScene();

    addVideoEvent(new WSAVideoEvent(pOrdos));
    addTextEvent(new TextEvent(pIntroText->getString(IntroText_The_insidious_Ordos),-2,47,true,true,false));
    addTrigger(new CutSceneSoundTrigger(3,missle));
    addTrigger(new CutSceneSoundTrigger(8,missle));
    addTrigger(new CutSceneSoundTrigger(28,missle));
    addTrigger(new CutSceneSoundTrigger(38,missle));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(12,Voice[Voice_The_insidious]));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(20,Voice[Voice_Ordos]));

    startNewScene();

    addVideoEvent(new WSAVideoEvent(pHarkonnen));
    addVideoEvent(new FadeOutVideoEvent(pHarkonnen->getPicture(pHarkonnen->getNumFrames()-1), 15, true, true ,true));
    addTextEvent(new TextEvent(pIntroText->getString(IntroText_And_the_evil_Harkonnen),-2,45,true,true,false));
    addTrigger(new CutSceneSoundTrigger(0,gunshot));
    addTrigger(new CutSceneSoundTrigger(5,blaster));
    addTrigger(new CutSceneSoundTrigger(7,blaster));
    addTrigger(new CutSceneSoundTrigger(17,gunshot));
    addTrigger(new CutSceneSoundTrigger(21,blaster));
    addTrigger(new CutSceneSoundTrigger(28,gunshot));
    addTrigger(new CutSceneSoundTrigger(37,gunshot));
    addTrigger(new CutSceneSoundTrigger(50,blowup1));
    addTrigger(new CutSceneSoundTrigger(60,blowup2));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(4,Voice[Voice_And_the]));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(10,Voice[Voice_evil_Harkonnen]));

    startNewScene();

    addVideoEvent(new FadeInVideoEvent(pDestroyedTank->getPicture(0), 15, true, true ,true));
    addVideoEvent(new WSAVideoEvent(pDestroyedTank));
    addVideoEvent(new WSAVideoEvent(pDestroyedTank));
    addVideoEvent(new WSAVideoEvent(pDestroyedTank));
    addVideoEvent(new WSAVideoEvent(pDestroyedTank));
    addVideoEvent(new WSAVideoEvent(pDestroyedTank));
    addVideoEvent(new WSAVideoEvent(pDestroyedTank));
    addVideoEvent(new WSAVideoEvent(pDestroyedTank));
    addVideoEvent(new WSAVideoEvent(pDestroyedTank));
    addVideoEvent(new FadeOutVideoEvent(pDestroyedTank->getPicture(pDestroyedTank->getNumFrames()-1), 15, true));
    addTextEvent(new TextEvent(pIntroText->getString(IntroText_Only_one_House_will_prevail),15,35,true,true,false));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(18,Voice[Voice_Only_one_house]));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(27,Voice[Voice_will_prevail]));

    startNewScene();

    addVideoEvent(new HoldPictureVideoEvent(NULL, 150, true));
    addTextEvent(new TextEvent(pIntroText->getString(IntroText_Your_battle_for_Dune_begins),0,45,true,false,true));
    addTextEvent(new TextEvent(pIntroText->getString(IntroText_NOW),47,45,false,true,true));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(10,Voice[Voice_Your]));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(16,Voice[Voice_battle_for_Dune]));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(28,Voice[Voice_begins]));
    if(bEnableVoice) addTrigger(new CutSceneSoundTrigger(47,Voice[Voice_Now_Now]));

    delete pIntroText;
}

Intro::~Intro() {
    delete pDuneText;
    delete pPlanet;
    delete pSandstorm;
    delete pHarvesters;
    delete pPalace;
    delete pImperator;
    delete pStarport;
    delete pOrdos;
    delete pAtreides;
    delete pHarkonnen;
    delete pDestroyedTank;

    Mix_FreeChunk(wind);
    Mix_FreeChunk(carryallLanding);
    Mix_FreeChunk(harvester);
    Mix_FreeChunk(gunshot);
    Mix_FreeChunk(glass);
    Mix_FreeChunk(missle);
    Mix_FreeChunk(blaster);
    Mix_FreeChunk(blowup1);
    Mix_FreeChunk(blowup2);

    // Free english voice
    for(int i=0;i<Voice_NUM_ENTRIES;i++) {
        Mix_FreeChunk(Voice[i]);
    }
}
