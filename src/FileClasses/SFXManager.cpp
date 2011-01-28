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

#include <FileClasses/SFXManager.h>

#include <globals.h>

#include <FileClasses/FileManager.h>
#include <FileClasses/Vocfile.h>

#include <misc/sound_util.h>

SFXManager::SFXManager() {
	// load voice and language specific sounds
	switch(settings.General.Language) {
		case LNG_ENG:
			LoadVoice_English();
			break;
		case LNG_FRE:
			LoadVoice_NonEnglish("F");
			break;
		case LNG_GER:
			LoadVoice_NonEnglish("G");
			break;
		default:
			fprintf(stderr,"SFXManager: Unknown language!\n");
			exit(EXIT_FAILURE);
	}

	for(int i = 0; i < NUM_SOUNDCHUNK; i++) {
		if(SoundChunk[i] == NULL) {
			fprintf(stderr,"SFXManager::SFXManager: Not all sounds could be loaded\n");
			exit(EXIT_FAILURE);
		}
	}
}

SFXManager::~SFXManager() {
	// unload voice
	for(int i = 0; i < Num_Lng_Voice; i++) {
		if(Lng_Voice[i] != NULL) {
			Mix_FreeChunk(Lng_Voice[i]);
			Lng_Voice[i] = NULL;
		}
	}

	free(Lng_Voice);

	// unload sound
	for(int i = 0; i < NUM_SOUNDCHUNK; i++) {
		if(SoundChunk[i] != NULL) {
			Mix_FreeChunk(SoundChunk[i]);
			SoundChunk[i] = NULL;
		}
	}
}

Mix_Chunk* SFXManager::GetVoice(Voice_enum id, int house) {
	switch(settings.General.Language) {
		case LNG_ENG:
			return GetVoice_English(id,house);
			break;
        case LNG_FRE:
		case LNG_GER:
			return GetVoice_NonEnglish(id,house);
			break;
		default:
			fprintf(stderr,"SFXManager: Unknown language!\n");
			exit(EXIT_FAILURE);
	}
}

Mix_Chunk* SFXManager::GetSound(Sound_enum id) {
	if(id >= NUM_SOUNDCHUNK)
		return NULL;

	return SoundChunk[id];
}

void SFXManager::LoadVoice_English() {
	Num_Lng_Voice = NUM_VOICE*3;

	if((Lng_Voice = (Mix_Chunk**) malloc(sizeof(Mix_Chunk*) * Num_Lng_Voice)) == NULL) {
		fprintf(stderr,"SFXManager::LoadVoice_English: Cannot allocate memory!\n");
		exit(EXIT_FAILURE);
	}

	for(int i = 0; i < Num_Lng_Voice; i++) {
		Lng_Voice[i] = NULL;
	}

	// now we can load
	for(int house = 0; house < 3; house++) {
		Mix_Chunk* HouseNameChunk;

		std::string HouseString;
		int VoiceNum;
		switch(house) {
			case HOUSE_ATREIDES:
			case HOUSE_FREMEN:
				VoiceNum = 0;
				HouseString = "A";
				HouseNameChunk = GetChunkFromFile(HouseString + "ATRE.VOC");
				break;
			case HOUSE_ORDOS:
			case HOUSE_MERCENARY:
				VoiceNum = 1;
				HouseString = "O";
				HouseNameChunk = GetChunkFromFile(HouseString + "ORDOS.VOC");
				break;
			case HOUSE_HARKONNEN:
			case HOUSE_SARDAUKAR:
			default:
				VoiceNum = 2;
				HouseString = "H";
				HouseNameChunk = GetChunkFromFile(HouseString + "HARK.VOC");
				break;
		}

		// "... Harvester deployed"
		Mix_Chunk* Harvester = GetChunkFromFile(HouseString + "HARVEST.VOC");
		Mix_Chunk* Deployed = GetChunkFromFile(HouseString + "DEPLOY.VOC");
		Lng_Voice[HarvesterDeployed*3+VoiceNum] = Concat3Chunks(HouseNameChunk, Harvester, Deployed);
		Mix_FreeChunk(Harvester);
		Mix_FreeChunk(Deployed);

		// "Contruction complete"
		Lng_Voice[ConstructionComplete*3+VoiceNum] = GetChunkFromFile(HouseString + "CONST.VOC");

		// "Vehicle repaired"
		Mix_Chunk* Vehicle = GetChunkFromFile(HouseString + "VEHICLE.VOC");
		Mix_Chunk* Repaired = GetChunkFromFile(HouseString + "REPAIR.VOC");
		Lng_Voice[VehicleRepaired*3+VoiceNum] = Concat2Chunks(Vehicle, Repaired);
		Mix_FreeChunk(Vehicle);
		Mix_FreeChunk(Repaired);

		// "Frigate has arrived"
		Mix_Chunk* FrigateChunk = GetChunkFromFile(HouseString + "FRIGATE.VOC");
		Mix_Chunk* HasArrivedChunk = GetChunkFromFile(HouseString + "ARRIVE.VOC");
		Lng_Voice[FrigateHasArrived*3+VoiceNum] = Concat2Chunks(FrigateChunk, HasArrivedChunk);
		Mix_FreeChunk(FrigateChunk);
		Mix_FreeChunk(HasArrivedChunk);

		// "Your mission is complete"
		Lng_Voice[YourMissionIsComplete*3+VoiceNum] = GetChunkFromFile(HouseString + "WIN.VOC");

		// "You have failed your mission"
		Lng_Voice[YouHaveFailedYourMission*3+VoiceNum] = GetChunkFromFile(HouseString + "LOSE.VOC");

		// "Radar activated"/"Radar deactivated"
		Mix_Chunk* RadarChunk = GetChunkFromFile(HouseString + "RADAR.VOC");
		Mix_Chunk* RadarActivatedChunk = GetChunkFromFile(HouseString + "ON.VOC");
		Mix_Chunk* RadarDeactivatedChunk = GetChunkFromFile(HouseString + "OFF.VOC");
		Lng_Voice[RadarActivated*3+VoiceNum] = Concat2Chunks(RadarChunk, RadarActivatedChunk);
		Lng_Voice[RadarDeactivated*3+VoiceNum] = Concat2Chunks(RadarChunk, RadarDeactivatedChunk);
		Mix_FreeChunk(RadarChunk);
		Mix_FreeChunk(RadarActivatedChunk);
		Mix_FreeChunk(RadarDeactivatedChunk);

		// "Bloom located"
		Mix_Chunk* Bloom = GetChunkFromFile(HouseString + "BLOOM.VOC");
		Mix_Chunk* Located = GetChunkFromFile(HouseString + "LOCATED.VOC");
		Lng_Voice[BloomLocated*3+VoiceNum] = Concat2Chunks(Bloom, Located);
		Mix_FreeChunk(Bloom);
		Mix_FreeChunk(Located);

        // "Warning Wormsign"
        Mix_Chunk* WarningChunk = GetChunkFromFile(HouseString + "WARNING.VOC");
        Mix_Chunk* WormSignChunk = GetChunkFromFile(HouseString + "WORMY.VOC");
        Lng_Voice[WarningWormSign*3+VoiceNum] = Concat2Chunks(WarningChunk, WormSignChunk);
        Mix_FreeChunk(WarningChunk);
        Mix_FreeChunk(WormSignChunk);

        // "Our base is under attack"
		Lng_Voice[BaseIsUnderAttack*3+VoiceNum] = GetChunkFromFile(HouseString + "ATTACK.VOC");

        // "Saboteur approaching" and "Missile approaching"
        Mix_Chunk* SabotChunk = GetChunkFromFile(HouseString + "SABOT.VOC");
        Mix_Chunk* MissileChunk = GetChunkFromFile(HouseString + "MISSILE.VOC");
        Mix_Chunk* ApproachingChunk = GetChunkFromFile(HouseString + "APPRCH.VOC");
        Lng_Voice[SaboteurApproaching*3+VoiceNum] = Concat2Chunks(SabotChunk, ApproachingChunk);
        Lng_Voice[MissileApproaching*3+VoiceNum] = Concat2Chunks(MissileChunk, ApproachingChunk);
        Mix_FreeChunk(SabotChunk);
        Mix_FreeChunk(MissileChunk);
        Mix_FreeChunk(ApproachingChunk);

		Mix_FreeChunk(HouseNameChunk);
	}

	for(int i = 0; i < Num_Lng_Voice; i++) {
		if(Lng_Voice[i] == NULL) {
			fprintf(stderr,"SFXManager::LoadVoice_English: Not all voice sounds could be loaded\n");
			exit(EXIT_FAILURE);
		}
	}

	// "Yes Sir"
    SoundChunk[YesSir] = GetChunkFromFile("ZREPORT1.VOC", "REPORT1.VOC");

	// "Reporting"
	SoundChunk[Reporting] = GetChunkFromFile("ZREPORT2.VOC", "REPORT2.VOC");

	// "Acknowledged"
	SoundChunk[Acknowledged] = GetChunkFromFile("ZREPORT3.VOC", "REPORT3.VOC");

	// "Affirmative"
	SoundChunk[Affirmative] = GetChunkFromFile("ZAFFIRM.VOC", "AFFIRM.VOC");

	// "Moving out"
	SoundChunk[MovingOut] = GetChunkFromFile("ZMOVEOUT.VOC", "MOVEOUT.VOC");

	// "Infantry out"
	SoundChunk[InfantryOut] = GetChunkFromFile("ZOVEROUT.VOC", "OVEROUT.VOC");

	// "Somthing's under the sand"
	SoundChunk[SomethingUnderTheSand] = GetChunkFromFile("SANDBUG.VOC");

	// "House Atreides"
	SoundChunk[HouseAtreides] = GetChunkFromFile("MATRE.VOC");

	// "House Ordos"
	SoundChunk[HouseOrdos] = GetChunkFromFile("MORDOS.VOC");

	// "House Harkonnen"
	SoundChunk[HouseHarkonnen] = GetChunkFromFile("MHARK.VOC");

	// Sfx
	SoundChunk[PlaceStructure] = GetChunkFromFile("EXDUD.VOC");
	SoundChunk[ButtonClick] = GetChunkFromFile("BUTTON.VOC");
	SoundChunk[InvalidAction] = Mix_LoadWAV_RW(pFileManager->OpenFile("CANNOT.WAV"),1);
	SoundChunk[CreditsTick] = Mix_LoadWAV_RW(pFileManager->OpenFile("CREDIT.WAV"),1);
	SoundChunk[Tick] = Mix_LoadWAV_RW(pFileManager->OpenFile("TICK.WAV"),1);
	SoundChunk[RadarNoise] = GetChunkFromFile("STATICP.VOC");
	SoundChunk[Sound_ExplosionGas] = GetChunkFromFile("EXGAS.VOC");
	SoundChunk[Sound_ExplosionTiny] = GetChunkFromFile("EXTINY.VOC");
	SoundChunk[Sound_ExplosionSmall] = GetChunkFromFile("EXSMALL.VOC");
	SoundChunk[Sound_ExplosionMedium] = GetChunkFromFile("EXMED.VOC");
	SoundChunk[Sound_ExplosionLarge] = GetChunkFromFile("EXLARGE.VOC");
	SoundChunk[Sound_ExplosionStructure] = GetChunkFromFile("CRUMBLE.VOC");
	SoundChunk[Sound_WormAttack] = GetChunkFromFile("WORMET3P.VOC");
	SoundChunk[Sound_Gun] = GetChunkFromFile("GUN.VOC");
	SoundChunk[Sound_Rocket] = GetChunkFromFile("ROCKET.VOC");
	SoundChunk[Sound_Bloom] = GetChunkFromFile("EXSAND.VOC");
	SoundChunk[Sound_Scream1] = GetChunkFromFile("VSCREAM1.VOC");
	SoundChunk[Sound_Scream2] = GetChunkFromFile("VSCREAM1.VOC");
	SoundChunk[Sound_Scream3] = GetChunkFromFile("VSCREAM1.VOC");
	SoundChunk[Sound_Scream4] = GetChunkFromFile("VSCREAM1.VOC");
	SoundChunk[Sound_Scream5] = GetChunkFromFile("VSCREAM1.VOC");
	SoundChunk[Sound_Squashed] = GetChunkFromFile("SQUISH2.VOC");
	SoundChunk[Sound_MachineGun] = GetChunkFromFile("GUNMULTI.VOC");
	SoundChunk[Sound_Sonic] = Mix_LoadWAV_RW(pFileManager->OpenFile("SONIC.WAV"),1);
	SoundChunk[Sound_RocketSmall] = GetChunkFromFile("MISLTINP.VOC");
}


Mix_Chunk* SFXManager::GetVoice_English(Voice_enum id, int house) {
	if((int) id >= Num_Lng_Voice)
		return NULL;

	int VoiceNum;
	switch(house) {
		case HOUSE_ATREIDES:
		case HOUSE_FREMEN:
			VoiceNum = 0;
			break;
		case HOUSE_ORDOS:
		case HOUSE_MERCENARY:
			VoiceNum = 1;
			break;
		case HOUSE_HARKONNEN:
		case HOUSE_SARDAUKAR:
		default:
			VoiceNum = 2;
			break;
	}

	return Lng_Voice[id*3 + VoiceNum];
}

void SFXManager::LoadVoice_NonEnglish(std::string languagePrefix) {
	Num_Lng_Voice = NUM_VOICE;

	if((Lng_Voice = (Mix_Chunk**) malloc(sizeof(Mix_Chunk*) * NUM_VOICE)) == NULL) {
		fprintf(stderr,"SFXManager::LoadVoice_NonEnglish: Cannot allocate memory!\n");
		exit(EXIT_FAILURE);
	}

	for(int i = 0; i < NUM_VOICE; i++) {
		Lng_Voice[i] = NULL;
	}


	Lng_Voice[HarvesterDeployed] = GetChunkFromFile(languagePrefix + "HARVEST.VOC");

	// "Contruction complete"
	Lng_Voice[ConstructionComplete] = GetChunkFromFile(languagePrefix + "CONST.VOC");

	// "Vehicle repaired"
	Lng_Voice[VehicleRepaired] = GetChunkFromFile(languagePrefix + "REPAIR.VOC");

	// "Frigate has arrived"
	Lng_Voice[FrigateHasArrived] = GetChunkFromFile(languagePrefix + "FRIGATE.VOC");

	// "Your mission is complete" (No non-english voc available)
	Lng_Voice[YourMissionIsComplete] = CreateEmptyChunk();

	// "You have failed your mission" (No non-english voc available)
	Lng_Voice[YouHaveFailedYourMission] = CreateEmptyChunk();

	// "Radar activated"/"Radar deactivated"
	Lng_Voice[RadarActivated] = GetChunkFromFile(languagePrefix + "ON.VOC");
	Lng_Voice[RadarDeactivated] = GetChunkFromFile(languagePrefix + "OFF.VOC");

	// "Bloom located"
	Lng_Voice[BloomLocated] = GetChunkFromFile(languagePrefix + "BLOOM.VOC");

	// "Warning Wormsign"
	if(pFileManager->exists(languagePrefix + "WORMY.VOC")) {
        Mix_Chunk* WarningChunk = GetChunkFromFile(languagePrefix + "WARNING.VOC");
        Mix_Chunk* WormSignChunk = GetChunkFromFile(languagePrefix + "WORMY.VOC");
        Lng_Voice[WarningWormSign] = Concat2Chunks(WarningChunk, WormSignChunk);
        Mix_FreeChunk(WarningChunk);
        Mix_FreeChunk(WormSignChunk);
	} else {
        Lng_Voice[WarningWormSign] = GetChunkFromFile(languagePrefix + "WARNING.VOC");
	}

    // "Our base is under attack"
	Lng_Voice[BaseIsUnderAttack] = GetChunkFromFile(languagePrefix + "ATTACK.VOC");

    // "Saboteur approaching"
	Lng_Voice[SaboteurApproaching] = GetChunkFromFile(languagePrefix + "SABOT.VOC");

    // "Missile approaching"
	Lng_Voice[MissileApproaching] = GetChunkFromFile(languagePrefix + "MISSILE.VOC");

	for(int i = 0; i < Num_Lng_Voice; i++) {
		if(Lng_Voice[i] == NULL) {
			fprintf(stderr,"SFXManager::LoadVoice_NonEnglish: Not all voice sounds could be loaded\n");
			exit(EXIT_FAILURE);
		}
	}

	// "Yes Sir"
	SoundChunk[YesSir] = GetChunkFromFile(languagePrefix + "REPORT1.VOC");

	// "Reporting"
	SoundChunk[Reporting] = GetChunkFromFile(languagePrefix + "REPORT2.VOC");

	// "Acknowledged"
	SoundChunk[Acknowledged] = GetChunkFromFile(languagePrefix + "REPORT3.VOC");

	// "Affirmative"
	SoundChunk[Affirmative] = GetChunkFromFile(languagePrefix + "AFFIRM.VOC");

	// "Moving out"
	SoundChunk[MovingOut] = GetChunkFromFile(languagePrefix + "MOVEOUT.VOC");

	// "Infantry out"
	SoundChunk[InfantryOut] = GetChunkFromFile(languagePrefix + "OVEROUT.VOC");

	// "Somthing's under the sand"
	SoundChunk[SomethingUnderTheSand] = GetChunkFromFile("SANDBUG.VOC");

	// "House Atreides"
	SoundChunk[HouseAtreides] = GetChunkFromFile(languagePrefix + "ATRE.VOC");

	// "House Ordos"
	SoundChunk[HouseOrdos] = GetChunkFromFile(languagePrefix + "ORDOS.VOC");

	// "House Harkonnen"
	SoundChunk[HouseHarkonnen] = GetChunkFromFile(languagePrefix + "HARK.VOC");

	// Sfx
	SoundChunk[PlaceStructure] = GetChunkFromFile("EXDUD.VOC");
	SoundChunk[ButtonClick] = GetChunkFromFile("BUTTON.VOC");
	SoundChunk[InvalidAction] = Mix_LoadWAV_RW(pFileManager->OpenFile("CANNOT.WAV"),1);
	SoundChunk[CreditsTick] = Mix_LoadWAV_RW(pFileManager->OpenFile("CREDIT.WAV"),1);
	SoundChunk[Tick] = Mix_LoadWAV_RW(pFileManager->OpenFile("TICK.WAV"),1);
	SoundChunk[RadarNoise] = GetChunkFromFile("STATICP.VOC");
	SoundChunk[Sound_ExplosionGas] = GetChunkFromFile("EXGAS.VOC");
	SoundChunk[Sound_ExplosionTiny] = GetChunkFromFile("EXTINY.VOC");
	SoundChunk[Sound_ExplosionSmall] = GetChunkFromFile("EXSMALL.VOC");
	SoundChunk[Sound_ExplosionMedium] = GetChunkFromFile("EXMED.VOC");
	SoundChunk[Sound_ExplosionLarge] = GetChunkFromFile("EXLARGE.VOC");
	SoundChunk[Sound_ExplosionStructure] = GetChunkFromFile("CRUMBLE.VOC");
	SoundChunk[Sound_WormAttack] = GetChunkFromFile("WORMET3P.VOC");
	SoundChunk[Sound_Gun] = GetChunkFromFile("GUN.VOC");
	SoundChunk[Sound_Rocket] = GetChunkFromFile("ROCKET.VOC");
	SoundChunk[Sound_Bloom] = GetChunkFromFile("EXSAND.VOC");
	SoundChunk[Sound_Scream1] = GetChunkFromFile("VSCREAM1.VOC");
	SoundChunk[Sound_Scream2] = GetChunkFromFile("VSCREAM1.VOC");
	SoundChunk[Sound_Scream3] = GetChunkFromFile("VSCREAM1.VOC");
	SoundChunk[Sound_Scream4] = GetChunkFromFile("VSCREAM1.VOC");
	SoundChunk[Sound_Scream5] = GetChunkFromFile("VSCREAM1.VOC");
	SoundChunk[Sound_Squashed] = GetChunkFromFile("SQUISH2.VOC");
	SoundChunk[Sound_MachineGun] = GetChunkFromFile("GUNMULTI.VOC");
	SoundChunk[Sound_Sonic] = Mix_LoadWAV_RW(pFileManager->OpenFile("SONIC.WAV"),1);
	SoundChunk[Sound_RocketSmall] = GetChunkFromFile("MISLTINP.VOC");
}

Mix_Chunk* SFXManager::GetVoice_NonEnglish(Voice_enum id, int house) {
	if((int)id >= Num_Lng_Voice)
		return NULL;

	return Lng_Voice[id];
}
