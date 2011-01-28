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

#include <FileClasses/GFXManager.h>

#include <globals.h>

#include <FileClasses/FileManager.h>
#include <FileClasses/Shpfile.h>
#include <FileClasses/Cpsfile.h>
#include <FileClasses/Icnfile.h>
#include <FileClasses/Wsafile.h>
#include <FileClasses/Palfile.h>

#include <misc/draw_util.h>

#include <stdexcept>

using std::shared_ptr;

GFXManager::GFXManager() {
	WindTrapColorTimer = -139;

	// init whole ObjPic array
	for(int i = 0; i < NUM_OBJPICS; i++) {
		for(int j = 0; j < (int) NUM_HOUSES; j++) {
			ObjPic[i][j] = NULL;
		}
	}

	// init whole SmallDetailPics array
	for(int i = 0; i < NUM_SMALLDETAILPICS; i++) {
		SmallDetailPic[i] = NULL;
	}

	// init whole UIGraphic array
	for(int i = 0; i < NUM_UIGRAPHICS; i++) {
		for(int j = 0; j < (int) NUM_HOUSES; j++) {
			UIGraphic[i][j] = NULL;
		}
	}

	// init whole MapChoicePieces array
	for(int i = 0; i < NUM_MAPCHOICEPIECES; i++) {
		for(int j = 0; j < (int) NUM_HOUSES; j++) {
			MapChoicePieces[i][j] = NULL;
		}
	}

	// init whole MapChoiceArrows array
	for(int i = 0; i < NUM_MAPCHOICEARROWS; i++) {
		MapChoiceArrows[i] = NULL;
	}

	// init whole Anim array
	for(int i = 0; i < NUM_ANIMATION; i++) {
		Anim[i] = NULL;
	}

	// open all shp files
	shared_ptr<Shpfile> units = LoadShpfile("UNITS.SHP");
	shared_ptr<Shpfile> units1 = LoadShpfile("UNITS1.SHP");
	shared_ptr<Shpfile> units2 = LoadShpfile("UNITS2.SHP");
	shared_ptr<Shpfile> mouse = LoadShpfile("MOUSE.SHP");
	shared_ptr<Shpfile> shapes = LoadShpfile("SHAPES.SHP");
	shared_ptr<Shpfile> menshpa = LoadShpfile("MENSHPA.SHP");
	shared_ptr<Shpfile> menshph = LoadShpfile("MENSHPH.SHP");
	shared_ptr<Shpfile> menshpo = LoadShpfile("MENSHPO.SHP");
	shared_ptr<Shpfile> menshpm = LoadShpfile("MENSHPM.SHP");

	shared_ptr<Shpfile> choam;
    if(pFileManager->exists("CHOAM." + settings.General.LanguageExt)) {
        choam = LoadShpfile("CHOAM." + settings.General.LanguageExt);
    } else if(pFileManager->exists("CHOAMSHP.SHP")) {
        choam = LoadShpfile("CHOAMSHP.SHP");
    } else {
        throw std::runtime_error("GFXManager::GFXManager(): Cannot open CHOAMSHP.SHP or CHOAM."+settings.General.LanguageExt+"!");
    }

	shared_ptr<Shpfile> bttn;
    if(pFileManager->exists("BTTN." + settings.General.LanguageExt)) {
        bttn = LoadShpfile("BTTN." + settings.General.LanguageExt);
    } else {
        // The US-Version has the buttons in SHAPES.SHP
        // => bttn == NULL
    }

	shared_ptr<Shpfile> mentat;
    if(pFileManager->exists("MENTAT." + settings.General.LanguageExt)) {
        mentat = LoadShpfile("MENTAT." + settings.General.LanguageExt);
    } else {
        mentat = LoadShpfile("MENTAT.SHP");
    }

	shared_ptr<Shpfile> pieces = LoadShpfile("PIECES.SHP");
	shared_ptr<Shpfile> arrows = LoadShpfile("ARROWS.SHP");

    // Load icon file
    shared_ptr<Icnfile> icon = shared_ptr<Icnfile>(new Icnfile(pFileManager->OpenFile("ICON.ICN"),pFileManager->OpenFile("ICON.MAP"), true));

    // Load radar static
    shared_ptr<Wsafile> radar = LoadWsafile("STATIC.WSA");

	// open bene palette
	Palette benePalette = LoadPalette_RW(pFileManager->OpenFile("BENE.PAL"), true);

	//create PictureFactory
	shared_ptr<PictureFactory> PicFactory = shared_ptr<PictureFactory>(new PictureFactory());

	// load Object pics
	ObjPic[ObjPic_Tank_Base][HOUSE_HARKONNEN] = units2->getPictureArray(8,1,GROUNDUNIT_ROW(0));
	ObjPic[ObjPic_Tank_Gun][HOUSE_HARKONNEN] = units2->getPictureArray(8,1,GROUNDUNIT_ROW(5));
	ObjPic[ObjPic_Siegetank_Base][HOUSE_HARKONNEN] = units2->getPictureArray(8,1,GROUNDUNIT_ROW(10));
	ObjPic[ObjPic_Siegetank_Gun][HOUSE_HARKONNEN] = units2->getPictureArray(8,1,GROUNDUNIT_ROW(15));
	ObjPic[ObjPic_Devastator_Base][HOUSE_HARKONNEN] = units2->getPictureArray(8,1,GROUNDUNIT_ROW(20));
	ObjPic[ObjPic_Devastator_Gun][HOUSE_HARKONNEN] = units2->getPictureArray(8,1,GROUNDUNIT_ROW(25));
	ObjPic[ObjPic_Sonictank_Gun][HOUSE_HARKONNEN] = units2->getPictureArray(8,1,GROUNDUNIT_ROW(30));
	ObjPic[ObjPic_Launcher_Gun][HOUSE_HARKONNEN] = units2->getPictureArray(8,1,GROUNDUNIT_ROW(35));
	ObjPic[ObjPic_Quad][HOUSE_HARKONNEN] = units->getPictureArray(8,1,GROUNDUNIT_ROW(0));
	ObjPic[ObjPic_Trike][HOUSE_HARKONNEN] = units->getPictureArray(8,1,GROUNDUNIT_ROW(5));
	ObjPic[ObjPic_Harvester][HOUSE_HARKONNEN] = units->getPictureArray(8,1,GROUNDUNIT_ROW(10));
	ObjPic[ObjPic_Harvester_Sand][HOUSE_HARKONNEN] = units1->getPictureArray(8,3,HARVESTERSAND_ROW(72),HARVESTERSAND_ROW(73),HARVESTERSAND_ROW(74));
	ObjPic[ObjPic_MCV][HOUSE_HARKONNEN] = units->getPictureArray(8,1,GROUNDUNIT_ROW(15));
	ObjPic[ObjPic_Carryall][HOUSE_HARKONNEN] = units->getPictureArray(8,2,AIRUNIT_ROW(45),AIRUNIT_ROW(48));
	ObjPic[ObjPic_CarryallShadow][HOUSE_HARKONNEN] = CreateShadowSurface(ObjPic[ObjPic_Carryall][HOUSE_HARKONNEN]);
	ObjPic[ObjPic_Frigate][HOUSE_HARKONNEN] = units->getPictureArray(8,1,AIRUNIT_ROW(60));
	ObjPic[ObjPic_FrigateShadow][HOUSE_HARKONNEN] = CreateShadowSurface(ObjPic[ObjPic_Frigate][HOUSE_HARKONNEN]);
	ObjPic[ObjPic_Ornithopter][HOUSE_HARKONNEN] = units->getPictureArray(8,3,ORNITHOPTER_ROW(51),ORNITHOPTER_ROW(52),ORNITHOPTER_ROW(53));
	ObjPic[ObjPic_OrnithopterShadow][HOUSE_HARKONNEN] = CreateShadowSurface(ObjPic[ObjPic_Ornithopter][HOUSE_HARKONNEN]);
	ObjPic[ObjPic_Trooper][HOUSE_HARKONNEN] = units->getPictureArray(4,3,INFANTRY_ROW(82),INFANTRY_ROW(83),INFANTRY_ROW(84));
	ObjPic[ObjPic_Infantry][HOUSE_HARKONNEN] = units->getPictureArray(4,3,INFANTRY_ROW(73),INFANTRY_ROW(74),INFANTRY_ROW(75));
	ObjPic[ObjPic_Saboteur][HOUSE_HARKONNEN] = units->getPictureArray(4,3,INFANTRY_ROW(63),INFANTRY_ROW(64),INFANTRY_ROW(65));
	ObjPic[ObjPic_Sandworm][HOUSE_HARKONNEN] = units1->getPictureArray(1,5,67|TILE_NORMAL,68|TILE_NORMAL,69|TILE_NORMAL,70|TILE_NORMAL,71|TILE_NORMAL);
	ObjPic[ObjPic_ConstructionYard][HOUSE_HARKONNEN] = icon->getPictureArray(17);
	ObjPic[ObjPic_Windtrap][HOUSE_HARKONNEN] = icon->getPictureArray(19);
	ObjPic[ObjPic_Refinery][HOUSE_HARKONNEN] = icon->getPictureArray(21);
	ObjPic[ObjPic_Barracks][HOUSE_HARKONNEN] = icon->getPictureArray(18);
	ObjPic[ObjPic_WOR][HOUSE_HARKONNEN] = icon->getPictureArray(16);
	ObjPic[ObjPic_Radar][HOUSE_HARKONNEN] = icon->getPictureArray(26);
	ObjPic[ObjPic_LightFactory][HOUSE_HARKONNEN] = icon->getPictureArray(12);
	ObjPic[ObjPic_Silo][HOUSE_HARKONNEN] = icon->getPictureArray(25);
	ObjPic[ObjPic_HeavyFactory][HOUSE_HARKONNEN] = icon->getPictureArray(13);
	ObjPic[ObjPic_HighTechFactory][HOUSE_HARKONNEN] = icon->getPictureArray(14);
	ObjPic[ObjPic_IX][HOUSE_HARKONNEN] = icon->getPictureArray(15);
	ObjPic[ObjPic_Palace][HOUSE_HARKONNEN] = icon->getPictureArray(11);
	ObjPic[ObjPic_RepairYard][HOUSE_HARKONNEN] = icon->getPictureArray(22);
	ObjPic[ObjPic_Starport][HOUSE_HARKONNEN] = icon->getPictureArray(20);
	ObjPic[ObjPic_GunTurret][HOUSE_HARKONNEN] = icon->getPictureArray(23);
	ObjPic[ObjPic_RocketTurret][HOUSE_HARKONNEN] = icon->getPictureArray(24);
	ObjPic[ObjPic_Wall][HOUSE_HARKONNEN] = icon->getPictureArray(6,1,1,75);
	ObjPic[ObjPic_Bullet_SmallRocket][HOUSE_HARKONNEN] = units->getPictureArray(16,1,ROCKET_ROW(35));
	ObjPic[ObjPic_Bullet_MediumRocket][HOUSE_HARKONNEN] = units->getPictureArray(16,1,ROCKET_ROW(20));
	ObjPic[ObjPic_Bullet_LargeRocket][HOUSE_HARKONNEN] = units->getPictureArray(16,1,ROCKET_ROW(40));
	ObjPic[ObjPic_Bullet_Small][HOUSE_HARKONNEN] = units1->getPicture(23);
	ObjPic[ObjPic_Bullet_Medium][HOUSE_HARKONNEN] = units1->getPicture(24);
	ObjPic[ObjPic_Bullet_Sonic][HOUSE_HARKONNEN] = units1->getPicture(10);
	ObjPic[ObjPic_Hit_Gas][HOUSE_ORDOS] = units1->getPictureArray(5,1,57|TILE_NORMAL,58|TILE_NORMAL,59|TILE_NORMAL,60|TILE_NORMAL,61|TILE_NORMAL);
	ObjPic[ObjPic_Hit_Gas][HOUSE_HARKONNEN] = MapImageHouseColor(ObjPic[ObjPic_Hit_Gas][HOUSE_ORDOS],HOUSE_HARKONNEN, COLOR_ORDOS);
	ObjPic[ObjPic_Hit_Shell][HOUSE_HARKONNEN] = units1->getPictureArray(3,1,2|TILE_NORMAL,3|TILE_NORMAL,4|TILE_NORMAL);
	ObjPic[ObjPic_ExplosionSmall][HOUSE_HARKONNEN] = units1->getPictureArray(5,1,32|TILE_NORMAL,33|TILE_NORMAL,34|TILE_NORMAL,35|TILE_NORMAL,36|TILE_NORMAL);
	ObjPic[ObjPic_ExplosionMedium1][HOUSE_HARKONNEN] = units1->getPictureArray(5,1,47|TILE_NORMAL,48|TILE_NORMAL,49|TILE_NORMAL,50|TILE_NORMAL,51|TILE_NORMAL);
	ObjPic[ObjPic_ExplosionMedium2][HOUSE_HARKONNEN] = units1->getPictureArray(5,1,52|TILE_NORMAL,53|TILE_NORMAL,54|TILE_NORMAL,55|TILE_NORMAL,56|TILE_NORMAL);
	ObjPic[ObjPic_ExplosionLarge1][HOUSE_HARKONNEN] = units1->getPictureArray(5,1,37|TILE_NORMAL,38|TILE_NORMAL,39|TILE_NORMAL,40|TILE_NORMAL,41|TILE_NORMAL);
	ObjPic[ObjPic_ExplosionLarge2][HOUSE_HARKONNEN] = units1->getPictureArray(5,1,42|TILE_NORMAL,43|TILE_NORMAL,44|TILE_NORMAL,45|TILE_NORMAL,46|TILE_NORMAL);
	ObjPic[ObjPic_ExplosionSmallUnit][HOUSE_HARKONNEN] = units1->getPictureArray(2,1,0|TILE_NORMAL,1|TILE_NORMAL);
	ObjPic[ObjPic_ExplosionFlames][HOUSE_HARKONNEN] = units1->getPictureArray(21,1,	11|TILE_NORMAL,12|TILE_NORMAL,13|TILE_NORMAL,17|TILE_NORMAL,18|TILE_NORMAL,19|TILE_NORMAL,17|TILE_NORMAL,
																					18|TILE_NORMAL,19|TILE_NORMAL,17|TILE_NORMAL,18|TILE_NORMAL,19|TILE_NORMAL,17|TILE_NORMAL,18|TILE_NORMAL,
																					19|TILE_NORMAL,17|TILE_NORMAL,18|TILE_NORMAL,19|TILE_NORMAL,20|TILE_NORMAL,21|TILE_NORMAL,22|TILE_NORMAL);
	ObjPic[ObjPic_DeadInfantry][HOUSE_HARKONNEN] = icon->getPictureArray(4,1,1,6);
	ObjPic[ObjPic_DeadAirUnit][HOUSE_HARKONNEN] = icon->getPictureArray(3,1,1,6);
	ObjPic[ObjPic_Smoke][HOUSE_HARKONNEN] = units1->getPictureArray(3,1,29|TILE_NORMAL,30|TILE_NORMAL,31|TILE_NORMAL);
	ObjPic[ObjPic_SandwormShimmerMask][HOUSE_HARKONNEN] = units1->getPicture(10);
	ObjPic[ObjPic_Terrain][HOUSE_HARKONNEN] = icon->getPictureRow(124,209);
	ObjPic[ObjPic_DestroyedStructure][HOUSE_HARKONNEN] = icon->getPictureRow2(14, 33, 125, 213, 214, 215, 223, 224, 225, 232, 233, 234, 240, 246, 247);
	ObjPic[ObjPic_RockDamage][HOUSE_HARKONNEN] = icon->getPictureRow(1,6);
	ObjPic[ObjPic_SandDamage][HOUSE_HARKONNEN] = units1->getPictureArray(3,1,5|TILE_NORMAL,6|TILE_NORMAL,7|TILE_NORMAL);
	ObjPic[ObjPic_Terrain_Hidden][HOUSE_HARKONNEN] = icon->getPictureRow(108,123);
	ObjPic[ObjPic_Terrain_Tracks][HOUSE_HARKONNEN] = icon->getPictureRow(25,32);

	// load small detail pics
	SmallDetailPic[Picture_Barracks] = ExtractSmallDetailPic("BARRAC.WSA");
	SmallDetailPic[Picture_ConstructionYard] = ExtractSmallDetailPic("CONSTRUC.WSA");
	SmallDetailPic[Picture_Carryall] = ExtractSmallDetailPic("CARRYALL.WSA");
	SmallDetailPic[Picture_Devastator] = ExtractSmallDetailPic("HARKTANK.WSA");
	SmallDetailPic[Picture_Deviator] = ExtractSmallDetailPic("ORDRTANK.WSA");
	SmallDetailPic[Picture_DeathHand] = ExtractSmallDetailPic("GOLD-BB.WSA");
	SmallDetailPic[Picture_Fremen] = ExtractSmallDetailPic("FREMEN.WSA");
	SmallDetailPic[Picture_GunTurret] = ExtractSmallDetailPic("TURRET.WSA");
	SmallDetailPic[Picture_Harvester] = ExtractSmallDetailPic("HARVEST.WSA");
	SmallDetailPic[Picture_HeavyFactory] = ExtractSmallDetailPic("HVYFTRY.WSA");
	SmallDetailPic[Picture_HighTechFactory] = ExtractSmallDetailPic("HITCFTRY.WSA");
	SmallDetailPic[Picture_Soldier] = ExtractSmallDetailPic("INFANTRY.WSA");
	SmallDetailPic[Picture_IX] = ExtractSmallDetailPic("IX.WSA");
	SmallDetailPic[Picture_Launcher] = ExtractSmallDetailPic("RTANK.WSA");
	SmallDetailPic[Picture_LightFactory] = ExtractSmallDetailPic("LITEFTRY.WSA");
	SmallDetailPic[Picture_MCV] = ExtractSmallDetailPic("MCV.WSA");
	SmallDetailPic[Picture_Ornithopter] = ExtractSmallDetailPic("ORNI.WSA");
	SmallDetailPic[Picture_Palace] = ExtractSmallDetailPic("PALACE.WSA");
	SmallDetailPic[Picture_Quad] = ExtractSmallDetailPic("QUAD.WSA");
	SmallDetailPic[Picture_Radar] = ExtractSmallDetailPic("HEADQRTS.WSA");
	SmallDetailPic[Picture_Raider] = ExtractSmallDetailPic("OTRIKE.WSA");
	SmallDetailPic[Picture_Refinery] = ExtractSmallDetailPic("REFINERY.WSA");
	SmallDetailPic[Picture_RepairYard] = ExtractSmallDetailPic("REPAIR.WSA");
	SmallDetailPic[Picture_RocketTurret] = ExtractSmallDetailPic("RTURRET.WSA");
	SmallDetailPic[Picture_Saboteur] = ExtractSmallDetailPic("SABOTURE.WSA");
	SmallDetailPic[Picture_Sardaukar] = ExtractSmallDetailPic("SARDUKAR.WSA");
	SmallDetailPic[Picture_SiegeTank] = ExtractSmallDetailPic("HTANK.WSA");
	SmallDetailPic[Picture_Silo] = ExtractSmallDetailPic("STORAGE.WSA");
	SmallDetailPic[Picture_Slab1] = ExtractSmallDetailPic("SLAB.WSA");
	SmallDetailPic[Picture_Slab4] = ExtractSmallDetailPic("4SLAB.WSA");
	SmallDetailPic[Picture_SonicTank] = ExtractSmallDetailPic("STANK.WSA");
	SmallDetailPic[Picture_StarPort] = ExtractSmallDetailPic("STARPORT.WSA");
	SmallDetailPic[Picture_Tank] = ExtractSmallDetailPic("LTANK.WSA");
	SmallDetailPic[Picture_Trike] = ExtractSmallDetailPic("TRIKE.WSA");
	SmallDetailPic[Picture_Trooper] = ExtractSmallDetailPic("HYINFY.WSA");
	SmallDetailPic[Picture_Wall] = ExtractSmallDetailPic("WALL.WSA");
	SmallDetailPic[Picture_WindTrap] = ExtractSmallDetailPic("WINDTRAP.WSA");
	SmallDetailPic[Picture_WOR] = ExtractSmallDetailPic("WOR.WSA");
	// unused: FRIGATE.WSA, WORM.WSA, FARTR.WSA, FHARK.WSA, FORDOS.WSA

	// load animations
	Anim[Anim_AtreidesEyes] = menshpa->getAnimation(0,4,true,true);
	Anim[Anim_AtreidesEyes]->setFrameRate(0.5);
	Anim[Anim_AtreidesMouth] = menshpa->getAnimation(5,9,true,true);
	Anim[Anim_AtreidesMouth]->setFrameRate(5.0);
	Anim[Anim_AtreidesShoulder] = menshpa->getAnimation(10,10,true,true);
	Anim[Anim_AtreidesShoulder]->setFrameRate(1.0);
	Anim[Anim_AtreidesBook] = menshpa->getAnimation(11,12,true,true);
	Anim[Anim_AtreidesBook]->setFrameRate(0.1);
	Anim[Anim_HarkonnenEyes] = menshph->getAnimation(0,4,true,true);
	Anim[Anim_HarkonnenEyes]->setFrameRate(0.3);
	Anim[Anim_HarkonnenMouth] = menshph->getAnimation(5,9,true,true);
	Anim[Anim_HarkonnenMouth]->setFrameRate(5.0);
	Anim[Anim_HarkonnenShoulder] = menshph->getAnimation(10,10,true,true);
	Anim[Anim_HarkonnenShoulder]->setFrameRate(1.0);
	Anim[Anim_OrdosEyes] = menshpo->getAnimation(0,4,true,true);
	Anim[Anim_OrdosEyes]->setFrameRate(0.5);
	Anim[Anim_OrdosMouth] = menshpo->getAnimation(5,9,true,true);
	Anim[Anim_OrdosMouth]->setFrameRate(5.0);
	Anim[Anim_OrdosShoulder] = menshpo->getAnimation(10,10,true,true);
	Anim[Anim_OrdosShoulder]->setFrameRate(1.0);
	Anim[Anim_OrdosRing] = menshpo->getAnimation(11,14,true,true);
	Anim[Anim_OrdosRing]->setFrameRate(6.0);
	Anim[Anim_MercenaryEyes] = menshpm->getAnimation(0,4,true,true);
	if(Anim[Anim_MercenaryEyes] != NULL) {
	    Anim[Anim_MercenaryEyes]->setPalette(benePalette);
	}
	Anim[Anim_MercenaryEyes]->setFrameRate(0.5);
	Anim[Anim_MercenaryMouth] = menshpm->getAnimation(5,9,true,true);
	if(Anim[Anim_MercenaryMouth] != NULL) {
	    Anim[Anim_MercenaryMouth]->setPalette(benePalette);
	}
	Anim[Anim_MercenaryMouth]->setFrameRate(5.0);
	Anim[Anim_AtreidesPlanet] = LoadAnimationFromWsa("FARTR.WSA");
	Anim[Anim_AtreidesPlanet]->setFrameRate(12);
	Anim[Anim_HarkonnenPlanet] = LoadAnimationFromWsa("FHARK.WSA");
	Anim[Anim_HarkonnenPlanet]->setFrameRate(12);
	Anim[Anim_OrdosPlanet] = LoadAnimationFromWsa("FORDOS.WSA");
	Anim[Anim_OrdosPlanet]->setFrameRate(12);
	Anim[Anim_Win1] = LoadAnimationFromWsa("WIN1.WSA");
	Anim[Anim_Win2] = LoadAnimationFromWsa("WIN2.WSA");
	Anim[Anim_Lose1] = LoadAnimationFromWsa("LOSTBILD.WSA");
	Anim[Anim_Lose2] = LoadAnimationFromWsa("LOSTVEHC.WSA");
	Anim[Anim_Barracks] = LoadAnimationFromWsa("BARRAC.WSA");
	Anim[Anim_Carryall] = LoadAnimationFromWsa("CARRYALL.WSA");
	Anim[Anim_ConstructionYard] = LoadAnimationFromWsa("CONSTRUC.WSA");
	Anim[Anim_Fremen] = LoadAnimationFromWsa("FREMEN.WSA");
	Anim[Anim_DeathHand] = LoadAnimationFromWsa("GOLD-BB.WSA");
	Anim[Anim_Devastator] = LoadAnimationFromWsa("HARKTANK.WSA");
	Anim[Anim_Harvester] = LoadAnimationFromWsa("HARVEST.WSA");
	Anim[Anim_Radar] = LoadAnimationFromWsa("HEADQRTS.WSA");
	Anim[Anim_HighTechFactory] = LoadAnimationFromWsa("HITCFTRY.WSA");
	Anim[Anim_SiegeTank] = LoadAnimationFromWsa("HTANK.WSA");
	Anim[Anim_HeavyFactory] = LoadAnimationFromWsa("HVYFTRY.WSA");
	Anim[Anim_Trooper] = LoadAnimationFromWsa("HYINFY.WSA");
	Anim[Anim_Infantry] = LoadAnimationFromWsa("INFANTRY.WSA");
	Anim[Anim_IX] = LoadAnimationFromWsa("IX.WSA");
	Anim[Anim_LightFactory] = LoadAnimationFromWsa("LITEFTRY.WSA");
	Anim[Anim_Tank] = LoadAnimationFromWsa("LTANK.WSA");
	Anim[Anim_MCV] = LoadAnimationFromWsa("MCV.WSA");
	Anim[Anim_Deviator] = LoadAnimationFromWsa("ORDRTANK.WSA");
	Anim[Anim_Ornithopter] = LoadAnimationFromWsa("ORNI.WSA");
	Anim[Anim_Raider] = LoadAnimationFromWsa("OTRIKE.WSA");
	Anim[Anim_Palace] = LoadAnimationFromWsa("PALACE.WSA");
	Anim[Anim_Quad] = LoadAnimationFromWsa("QUAD.WSA");
	Anim[Anim_Refinery] = LoadAnimationFromWsa("REFINERY.WSA");
	Anim[Anim_RepairYard] = LoadAnimationFromWsa("REPAIR.WSA");
	Anim[Anim_Launcher] = LoadAnimationFromWsa("RTANK.WSA");
	Anim[Anim_RocketTurret] = LoadAnimationFromWsa("RTURRET.WSA");
	Anim[Anim_Saboteur] = LoadAnimationFromWsa("SABOTURE.WSA");
	Anim[Anim_Slab1] = LoadAnimationFromWsa("SLAB.WSA");
	Anim[Anim_SonicTank] = LoadAnimationFromWsa("STANK.WSA");
	Anim[Anim_StarPort] = LoadAnimationFromWsa("STARPORT.WSA");
	Anim[Anim_Silo] = LoadAnimationFromWsa("STORAGE.WSA");
	Anim[Anim_Trike] = LoadAnimationFromWsa("TRIKE.WSA");
	Anim[Anim_GunTurret] = LoadAnimationFromWsa("TURRET.WSA");
	Anim[Anim_Wall] = LoadAnimationFromWsa("WALL.WSA");
	Anim[Anim_WindTrap] = LoadAnimationFromWsa("WINDTRAP.WSA");
	Anim[Anim_WOR] = LoadAnimationFromWsa("WOR.WSA");
	Anim[Anim_Sandworm] = LoadAnimationFromWsa("WORM.WSA");
	Anim[Anim_Sardaukar] = LoadAnimationFromWsa("SARDUKAR.WSA");
	if(pFileManager->exists("FRIGATE.WSA")) {
        Anim[Anim_Frigate] = LoadAnimationFromWsa("FRIGATE.WSA");
	} else {
	    // There are some MENTAT.PAK files without FRIGATE.WSA.
	    // We replace it with the starport
        Anim[Anim_Frigate] = LoadAnimationFromWsa("STARPORT.WSA");
	}
	Anim[Anim_Slab4] = LoadAnimationFromWsa("4SLAB.WSA");

	for(int i = Anim_Barracks; i <= Anim_Slab4; i++) {
		Anim[i]->setFrameRate(15.0);
	}

	// load UI graphics
	UIGraphic[UI_RadarAnimation][HOUSE_HARKONNEN] = DoublePicture(radar->getAnimationAsPictureRow());

	UIGraphic[UI_CursorNormal][HOUSE_HARKONNEN] = mouse->getPicture(0);
	SDL_SetColorKey(UIGraphic[UI_CursorNormal][HOUSE_HARKONNEN], SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
    UIGraphic[UI_CursorUp][HOUSE_HARKONNEN] = mouse->getPicture(1);
	SDL_SetColorKey(UIGraphic[UI_CursorUp][HOUSE_HARKONNEN], SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
    UIGraphic[UI_CursorRight][HOUSE_HARKONNEN] = mouse->getPicture(2);
	SDL_SetColorKey(UIGraphic[UI_CursorRight][HOUSE_HARKONNEN], SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
    UIGraphic[UI_CursorDown][HOUSE_HARKONNEN] = mouse->getPicture(3);
	SDL_SetColorKey(UIGraphic[UI_CursorDown][HOUSE_HARKONNEN], SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
    UIGraphic[UI_CursorLeft][HOUSE_HARKONNEN] = mouse->getPicture(4);
	SDL_SetColorKey(UIGraphic[UI_CursorLeft][HOUSE_HARKONNEN], SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
    UIGraphic[UI_CursorMove][HOUSE_HARKONNEN] = mouse->getPicture(5);
	SDL_SetColorKey(UIGraphic[UI_CursorMove][HOUSE_HARKONNEN], SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
	UIGraphic[UI_CursorAttack][HOUSE_HARKONNEN] = MapImageHouseColor(UIGraphic[UI_CursorMove][HOUSE_HARKONNEN], HOUSE_HARKONNEN, 232);
	SDL_SetColorKey(UIGraphic[UI_CursorAttack][HOUSE_HARKONNEN], SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
	UIGraphic[UI_CursorCapture][HOUSE_HARKONNEN] = SDL_LoadBMP_RW(pFileManager->OpenFile("Capture.bmp"),true);
	SDL_SetColorKey(UIGraphic[UI_CursorCapture][HOUSE_HARKONNEN], SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);

	UIGraphic[UI_CreditsDigits][HOUSE_HARKONNEN] = shapes->getPictureArray(10,1,2|TILE_NORMAL,3|TILE_NORMAL,4|TILE_NORMAL,5|TILE_NORMAL,6|TILE_NORMAL,
																				7|TILE_NORMAL,8|TILE_NORMAL,9|TILE_NORMAL,10|TILE_NORMAL,11|TILE_NORMAL);
	UIGraphic[UI_GameBar][HOUSE_HARKONNEN] = PicFactory->createGameBar();
	UIGraphic[UI_Indicator][HOUSE_HARKONNEN] = units1->getPictureArray(3,1,8|TILE_NORMAL,9|TILE_NORMAL,10|TILE_NORMAL);
	SDL_SetColorKey(UIGraphic[UI_Indicator][HOUSE_HARKONNEN], SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
	UIGraphic[UI_InvalidPlace][HOUSE_HARKONNEN] = PicFactory->createInvalidPlace();
	UIGraphic[UI_ValidPlace][HOUSE_HARKONNEN] = PicFactory->createValidPlace();
	UIGraphic[UI_MenuBackground][HOUSE_HARKONNEN] = PicFactory->createMainBackground();
	UIGraphic[UI_Background][HOUSE_HARKONNEN] = PicFactory->createBackground();
	UIGraphic[UI_GameStatsBackground][HOUSE_HARKONNEN] = PicFactory->createGameStatsBackground(HOUSE_HARKONNEN);
    UIGraphic[UI_GameStatsBackground][HOUSE_ATREIDES] = PicFactory->createGameStatsBackground(HOUSE_ATREIDES);
    UIGraphic[UI_GameStatsBackground][HOUSE_ORDOS] = PicFactory->createGameStatsBackground(HOUSE_ORDOS);
	UIGraphic[UI_SelectionBox][HOUSE_HARKONNEN] = SDL_LoadBMP_RW(pFileManager->OpenFile("UI_SelectionBox.bmp"),true);
	SDL_SetColorKey(UIGraphic[UI_SelectionBox][HOUSE_HARKONNEN], SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
	UIGraphic[UI_TopBar][HOUSE_HARKONNEN] = PicFactory->createTopBar();
	UIGraphic[UI_ButtonUp][HOUSE_HARKONNEN] = choam->getPicture(0);
	UIGraphic[UI_ButtonUp_Pressed][HOUSE_HARKONNEN] = choam->getPicture(1);
	UIGraphic[UI_ButtonDown][HOUSE_HARKONNEN] = choam->getPicture(2);
	UIGraphic[UI_ButtonDown_Pressed][HOUSE_HARKONNEN] = choam->getPicture(3);
	UIGraphic[UI_MessageBox][HOUSE_HARKONNEN] = PicFactory->createMessageBoxBorder();

	if(bttn.get() != NULL) {
        UIGraphic[UI_Mentat][HOUSE_HARKONNEN] = bttn->getPicture(0);
        UIGraphic[UI_Mentat_Pressed][HOUSE_HARKONNEN] = bttn->getPicture(1);
        UIGraphic[UI_Options][HOUSE_HARKONNEN] = bttn->getPicture(2);
        UIGraphic[UI_Options_Pressed][HOUSE_HARKONNEN] = bttn->getPicture(3);
	} else {
        UIGraphic[UI_Mentat][HOUSE_HARKONNEN] = shapes->getPicture(94);
        UIGraphic[UI_Mentat_Pressed][HOUSE_HARKONNEN] = shapes->getPicture(95);
        UIGraphic[UI_Options][HOUSE_HARKONNEN] = shapes->getPicture(96);
        UIGraphic[UI_Options_Pressed][HOUSE_HARKONNEN] = shapes->getPicture(97);
	}

	UIGraphic[UI_Upgrade][HOUSE_HARKONNEN] = choam->getPicture(4);
	SDL_SetColorKey(UIGraphic[UI_Upgrade][HOUSE_HARKONNEN], SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
	UIGraphic[UI_Upgrade_Pressed][HOUSE_HARKONNEN] = choam->getPicture(5);
	SDL_SetColorKey(UIGraphic[UI_Upgrade_Pressed][HOUSE_HARKONNEN], SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
	UIGraphic[UI_Repair][HOUSE_HARKONNEN] = SDL_LoadBMP_RW(pFileManager->OpenFile("Button_Repair.bmp"),true);
	UIGraphic[UI_Repair_Pressed][HOUSE_HARKONNEN] = SDL_LoadBMP_RW(pFileManager->OpenFile("Button_RepairPushed.bmp"),true);
	UIGraphic[UI_Difficulty][HOUSE_HARKONNEN] = SDL_LoadBMP_RW(pFileManager->OpenFile("Menu_Difficulty.bmp"),true);
	SDL_Rect dest1 = { 0,0,UIGraphic[UI_Difficulty][HOUSE_HARKONNEN]->w,30};
	PicFactory->drawFrame(UIGraphic[UI_Difficulty][HOUSE_HARKONNEN],PictureFactory::DecorationFrame1,&dest1);
	SDL_SetColorKey(UIGraphic[UI_Difficulty][HOUSE_HARKONNEN], SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
	UIGraphic[UI_Dif_Easy][HOUSE_HARKONNEN] = SDL_LoadBMP_RW(pFileManager->OpenFile("Difficulty_Easy.bmp"),true);
	UIGraphic[UI_Dif_Hard][HOUSE_HARKONNEN] = SDL_LoadBMP_RW(pFileManager->OpenFile("Difficulty_Hard.bmp"),true);
	UIGraphic[UI_Dif_Medium][HOUSE_HARKONNEN] = SDL_LoadBMP_RW(pFileManager->OpenFile("Difficulty_Medium.bmp"),true);
	UIGraphic[UI_Minus][HOUSE_HARKONNEN] = SDL_LoadBMP_RW(pFileManager->OpenFile("Button_Minus.bmp"),true);
	UIGraphic[UI_Minus_Pressed][HOUSE_HARKONNEN] = SDL_LoadBMP_RW(pFileManager->OpenFile("Button_MinusPushed.bmp"),true);
	UIGraphic[UI_Plus][HOUSE_HARKONNEN] = SDL_LoadBMP_RW(pFileManager->OpenFile("Button_Plus.bmp"),true);
	UIGraphic[UI_Plus_Pressed][HOUSE_HARKONNEN] = SDL_LoadBMP_RW(pFileManager->OpenFile("Button_PlusPushed.bmp"),true);
	UIGraphic[UI_MissionSelect][HOUSE_HARKONNEN] = SDL_LoadBMP_RW(pFileManager->OpenFile("Menu_MissionSelect.bmp"),true);
	PicFactory->drawFrame(UIGraphic[UI_MissionSelect][HOUSE_HARKONNEN],PictureFactory::SimpleFrame,NULL);
	SDL_SetColorKey(UIGraphic[UI_MissionSelect][HOUSE_HARKONNEN], SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
	UIGraphic[UI_OptionsMenu][HOUSE_HARKONNEN] = PicFactory->createOptionsMenu();
	SDL_Surface* tmp;
	if((tmp = SDL_CreateRGBSurface(SDL_HWSURFACE,192,27,8,0,0,0,0)) == NULL) {
		fprintf(stderr,"GFXManager::GFXManager(): Cannot create surface!\n");
		exit(EXIT_FAILURE);
	}
	palette.applyToSurface(tmp);
	SDL_FillRect(tmp,NULL,133);
	UIGraphic[UI_LoadSaveWindow][HOUSE_HARKONNEN] = PicFactory->createMenu(tmp,208);
	SDL_FreeSurface(tmp);
	UIGraphic[UI_DuneLegacy][HOUSE_HARKONNEN] = SDL_LoadBMP_RW(pFileManager->OpenFile("DuneLegacy.bmp"),true);
	UIGraphic[UI_GameMenu][HOUSE_HARKONNEN] = PicFactory->createMenu(UIGraphic[UI_DuneLegacy][HOUSE_HARKONNEN],158);
	PicFactory->drawFrame(UIGraphic[UI_DuneLegacy][HOUSE_HARKONNEN],PictureFactory::SimpleFrame);

	UIGraphic[UI_PlanetBackground][HOUSE_HARKONNEN] = LoadCPS_RW(pFileManager->OpenFile("BIGPLAN.CPS"),true);
	PicFactory->drawFrame(UIGraphic[UI_PlanetBackground][HOUSE_HARKONNEN],PictureFactory::SimpleFrame);
	UIGraphic[UI_MenuButtonBorder][HOUSE_HARKONNEN] = PicFactory->createFrame(PictureFactory::DecorationFrame1,190,123,false);

	PicFactory->drawFrame(UIGraphic[UI_DuneLegacy][HOUSE_HARKONNEN],PictureFactory::SimpleFrame);

	UIGraphic[UI_MentatBackground][HOUSE_ATREIDES] = DoublePicture(LoadCPS_RW(pFileManager->OpenFile("MENTATA.CPS"),true));
	UIGraphic[UI_MentatBackground][HOUSE_FREMEN] = DoublePicture(LoadCPS_RW(pFileManager->OpenFile("MENTATA.CPS"),true));
	UIGraphic[UI_MentatBackground][HOUSE_ORDOS] = DoublePicture(LoadCPS_RW(pFileManager->OpenFile("MENTATO.CPS"),true));
	UIGraphic[UI_MentatBackground][HOUSE_MERCENARY] = DoublePicture(LoadCPS_RW(pFileManager->OpenFile("MENTATM.CPS"),true));
	if(UIGraphic[UI_MentatBackground][HOUSE_MERCENARY] != NULL) {
	    benePalette.applyToSurface(UIGraphic[UI_MentatBackground][HOUSE_MERCENARY]);
	}
	UIGraphic[UI_MentatBackground][HOUSE_HARKONNEN] = DoublePicture(LoadCPS_RW(pFileManager->OpenFile("MENTATH.CPS"),true));
	UIGraphic[UI_MentatBackground][HOUSE_SARDAUKAR] = DoublePicture(LoadCPS_RW(pFileManager->OpenFile("MENTATH.CPS"),true));
	UIGraphic[UI_MentatYes][HOUSE_HARKONNEN] = DoublePicture(mentat->getPicture(0));
	UIGraphic[UI_MentatYes_Pressed][HOUSE_HARKONNEN] = DoublePicture(mentat->getPicture(1));
	UIGraphic[UI_MentatNo][HOUSE_HARKONNEN] = DoublePicture(mentat->getPicture(2));
	UIGraphic[UI_MentatNo_Pressed][HOUSE_HARKONNEN] = DoublePicture(mentat->getPicture(3));
	UIGraphic[UI_MentatExit][HOUSE_HARKONNEN] = DoublePicture(mentat->getPicture(4));
	UIGraphic[UI_MentatExit_Pressed][HOUSE_HARKONNEN] = DoublePicture(mentat->getPicture(5));
	UIGraphic[UI_MentatProcced][HOUSE_HARKONNEN] = DoublePicture(mentat->getPicture(6));
	UIGraphic[UI_MentatProcced_Pressed][HOUSE_HARKONNEN] = DoublePicture(mentat->getPicture(7));
	UIGraphic[UI_MentatRepeat][HOUSE_HARKONNEN] = DoublePicture(mentat->getPicture(8));
	UIGraphic[UI_MentatRepeat_Pressed][HOUSE_HARKONNEN] = DoublePicture(mentat->getPicture(9));

    if(pFileManager->exists("HERALD." + settings.General.LanguageExt)) {
        UIGraphic[UI_HouseChoiceBackground][HOUSE_HARKONNEN] = LoadCPS_RW(pFileManager->OpenFile("HERALD." + settings.General.LanguageExt),true);
    } else {
        UIGraphic[UI_HouseChoiceBackground][HOUSE_HARKONNEN] = LoadCPS_RW(pFileManager->OpenFile("HERALD.CPS"),true);
    }

	UIGraphic[UI_HouseSelect][HOUSE_HARKONNEN] = PicFactory->createHouseSelect(UIGraphic[UI_HouseChoiceBackground][HOUSE_HARKONNEN]);
	UIGraphic[UI_HeraldAtre_Colored][HOUSE_HARKONNEN] = GetSubPicture(UIGraphic[UI_HouseChoiceBackground][HOUSE_HARKONNEN],20,54,83,91);
	UIGraphic[UI_HeraldOrd_Colored][HOUSE_HARKONNEN] = GetSubPicture(UIGraphic[UI_HouseChoiceBackground][HOUSE_HARKONNEN],117,54,83,91);
	UIGraphic[UI_HeraldHark_Colored][HOUSE_HARKONNEN] = GetSubPicture(UIGraphic[UI_HouseChoiceBackground][HOUSE_HARKONNEN],215,54,82,91);
	UIGraphic[UI_HouseChoiceBackground][HOUSE_HARKONNEN] = DoublePicture(UIGraphic[UI_HouseChoiceBackground][HOUSE_HARKONNEN]);

	UIGraphic[UI_MapChoiceScreen][HOUSE_ATREIDES] = PicFactory->createMapChoiceScreen(HOUSE_ATREIDES);
	UIGraphic[UI_MapChoiceScreen][HOUSE_ORDOS] = PicFactory->createMapChoiceScreen(HOUSE_ORDOS);
	UIGraphic[UI_MapChoiceScreen][HOUSE_HARKONNEN] = PicFactory->createMapChoiceScreen(HOUSE_HARKONNEN);
	UIGraphic[UI_MapChoiceMapOnly][HOUSE_HARKONNEN] = DoublePicture(LoadCPS_RW(pFileManager->OpenFile("DUNEMAP.CPS"),true));
	SDL_SetColorKey(UIGraphic[UI_MapChoiceMapOnly][HOUSE_HARKONNEN], SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
	UIGraphic[UI_MapChoiceMap][HOUSE_HARKONNEN] = DoublePicture(LoadCPS_RW(pFileManager->OpenFile("DUNERGN.CPS"),true));
	SDL_SetColorKey(UIGraphic[UI_MapChoiceMap][HOUSE_HARKONNEN], SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
	UIGraphic[UI_MapChoiceClickMap][HOUSE_HARKONNEN] = DoublePicture(LoadCPS_RW(pFileManager->OpenFile("RGNCLK.CPS"),true));

	UIGraphic[UI_StructureSizeLattice][HOUSE_HARKONNEN] = SDL_LoadBMP_RW(pFileManager->OpenFile("StructureSizeLattice.bmp"),true);
	SDL_SetColorKey(UIGraphic[UI_StructureSizeLattice][HOUSE_HARKONNEN], SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
	UIGraphic[UI_StructureSizeConcrete][HOUSE_HARKONNEN] = SDL_LoadBMP_RW(pFileManager->OpenFile("StructureSizeConcrete.bmp"),true);
	SDL_SetColorKey(UIGraphic[UI_StructureSizeConcrete][HOUSE_HARKONNEN], SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);

	// load map choice pieces
	for(int i = 0; i < NUM_MAPCHOICEPIECES; i++) {
		MapChoicePieces[i][HOUSE_HARKONNEN] = DoublePicture(pieces->getPicture(i));
		SDL_SetColorKey(MapChoicePieces[i][HOUSE_HARKONNEN], SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
	}

	// load map choice arrows
	for(int i = 0; i < NUM_MAPCHOICEARROWS; i++) {
		MapChoiceArrows[i] = DoublePicture(arrows->getPicture(i));
		SDL_SetColorKey(MapChoiceArrows[i], SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
	}

	// Create alpha blending surfaces (128x128 pixel)
	Transparent40Surface = SDL_CreateRGBSurface(SDL_HWSURFACE,128,128,32,0,0,0,0);
    SDL_SetAlpha(Transparent40Surface, SDL_SRCALPHA, 40);

	Transparent150Surface = SDL_CreateRGBSurface(SDL_HWSURFACE,128,128,32,0,0,0,0);
    SDL_SetAlpha(Transparent150Surface, SDL_SRCALPHA, 150);

	for(int i = 0; i < NUM_OBJPICS; i++) {
		for(int j = 0; j < (int) NUM_HOUSES; j++) {
			if(ObjPic[i][j] != NULL) {
				SDL_SetColorKey(ObjPic[i][j], SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
				SDL_Surface* tmp;
				tmp = ObjPic[i][j];
				if((ObjPic[i][j] = SDL_DisplayFormat(tmp)) == NULL) {
					fprintf(stderr,"GFXManager: SDL_DisplayFormat() failed!\n");
					exit(EXIT_FAILURE);
				}
				SDL_FreeSurface(tmp);
			}
		}
	}

	for(int i = 0; i < NUM_SMALLDETAILPICS; i++) {
		if(SmallDetailPic[i] != NULL) {
			SDL_Surface* tmp;
			tmp = SmallDetailPic[i];
			if((SmallDetailPic[i] = SDL_DisplayFormat(tmp)) == NULL) {
				fprintf(stderr,"GFXManager: SDL_DisplayFormat() failed!\n");
				exit(EXIT_FAILURE);
			}
			SDL_FreeSurface(tmp);
		}
	}


	for(int i = 0; i < NUM_UIGRAPHICS; i++) {
		for(int j = 0; j < (int) NUM_HOUSES; j++) {
			if(UIGraphic[i][j] != NULL) {
				SDL_Surface* tmp;
				tmp = UIGraphic[i][j];
				if((UIGraphic[i][j] = SDL_DisplayFormat(tmp)) == NULL) {
					fprintf(stderr,"GFXManager: SDL_DisplayFormat() failed!\n");
					exit(EXIT_FAILURE);
				}
				SDL_FreeSurface(tmp);
			}
		}
	}

	for(int i = 0; i < NUM_ANIMATION; i++) {
		if(Anim[i] == NULL) {
			fprintf(stderr,"GFXManager::GFXManager: Not all animations could be loaded\n");
			exit(EXIT_FAILURE);
		}
	}
}

GFXManager::~GFXManager() {
	for(int i = 0; i < NUM_OBJPICS; i++) {
		for(int j = 0; j < (int) NUM_HOUSES; j++) {
			if(ObjPic[i][j] != NULL) {
				SDL_FreeSurface(ObjPic[i][j]);
				ObjPic[i][j] = NULL;
			}
		}
	}

	for(int i = 0; i < NUM_SMALLDETAILPICS; i++) {
		if(SmallDetailPic[i] != NULL) {
				SDL_FreeSurface(SmallDetailPic[i]);
				SmallDetailPic[i] = NULL;
		}
	}

	for(int i = 0; i < NUM_UIGRAPHICS; i++) {
		for(int j = 0; j < (int) NUM_HOUSES; j++) {
			if(UIGraphic[i][j] != NULL) {
					SDL_FreeSurface(UIGraphic[i][j]);
					UIGraphic[i][j] = NULL;
			}
		}
	}

	for(int i = 0; i < NUM_MAPCHOICEPIECES; i++) {
		for(int j = 0; j < (int) NUM_HOUSES; j++) {
			if(MapChoicePieces[i][j] != NULL) {
					SDL_FreeSurface(MapChoicePieces[i][j]);
					MapChoicePieces[i][j] = NULL;
			}
		}
	}

	for(int i = 0; i < NUM_MAPCHOICEARROWS; i++) {
		if(MapChoiceArrows[i] != NULL) {
				SDL_FreeSurface(MapChoiceArrows[i]);
				MapChoiceArrows[i] = NULL;
		}
	}

	for(int i = 0; i < NUM_ANIMATION; i++) {
		if(Anim[i] != NULL) {
				delete Anim[i];
				Anim[i] = NULL;
		}
	}

	SDL_FreeSurface(Transparent40Surface);
	SDL_FreeSurface(Transparent150Surface);
}

SDL_Surface* GFXManager::getObjPic(unsigned int id, int house) {
	if(id >= NUM_OBJPICS) {
		fprintf(stderr,"GFXManager::GetUnitPic(): Unit Picture with id %d is not available!\n",id);
		exit(EXIT_FAILURE);
	}

	if(ObjPic[id][house] == NULL) {
		// remap to this color
		if(ObjPic[id][HOUSE_HARKONNEN] == NULL) {
			fprintf(stderr,"GFXManager::GetUnitPic(): Unit Picture with id %d is not loaded!\n",id);
			exit(EXIT_FAILURE);
		}

		ObjPic[id][house] = MapImageHouseColor(ObjPic[id][HOUSE_HARKONNEN],house, COLOR_HARKONNEN);
	}

	return ObjPic[id][house];
}


SDL_Surface* GFXManager::getSmallDetailPic(unsigned int id) {
	if(id >= NUM_SMALLDETAILPICS) {
		return NULL;
	}
	return SmallDetailPic[id];
}


SDL_Surface* GFXManager::getUIGraphic(unsigned int id, int house) {
	if(id >= NUM_UIGRAPHICS) {
		fprintf(stderr,"GFXManager::getUIGraphic(): UI Graphic with id %d is not available!\n",id);
		exit(EXIT_FAILURE);
	}

	if(UIGraphic[id][house] == NULL) {
		// remap to this color
		if(UIGraphic[id][HOUSE_HARKONNEN] == NULL) {
			fprintf(stderr,"GFXManager::getUIGraphic(): UI Graphic with id %d is not loaded!\n",id);
			exit(EXIT_FAILURE);
		}

		UIGraphic[id][house] = MapImageHouseColor(UIGraphic[id][HOUSE_HARKONNEN],house, COLOR_HARKONNEN);
	}

	return UIGraphic[id][house];
}

SDL_Surface* GFXManager::getMapChoicePiece(unsigned int num, int house) {
	if(num >= NUM_MAPCHOICEPIECES) {
		fprintf(stderr,"GFXManager::getMapChoicePiece(): Map Piece with number %d is not available!\n",num);
		exit(EXIT_FAILURE);
	}

	if(MapChoicePieces[num][house] == NULL) {
		// remap to this color
		if(MapChoicePieces[num][HOUSE_HARKONNEN] == NULL) {
			fprintf(stderr,"GFXManager::getMapChoicePiece(): Map Piece with number %d is not loaded!\n",num);
			exit(EXIT_FAILURE);
		}

		MapChoicePieces[num][house] = MapImageHouseColor(MapChoicePieces[num][HOUSE_HARKONNEN],house, COLOR_HARKONNEN);
	}

	return MapChoicePieces[num][house];
}

SDL_Surface* GFXManager::getMapChoiceArrow(unsigned int num) {
	if(num >= NUM_MAPCHOICEARROWS) {
		fprintf(stderr,"GFXManager::getMapChoiceArrow(): Arrow number %d is not available!\n",num);
		exit(EXIT_FAILURE);
	}

	return MapChoiceArrows[num];
}

Animation* GFXManager::getAnimation(unsigned int id) {
	if(id >= NUM_ANIMATION) {
		fprintf(stderr,"GFXManager::getAnimation(): Animation with id %d is not available!\n",id);
		exit(EXIT_FAILURE);
	}

	return Anim[id];
}

void GFXManager::DoWindTrapPalatteAnimation() {

	int oldWindTrapColorTimer = WindTrapColorTimer;
	if (++WindTrapColorTimer >= 140)
		WindTrapColorTimer = -139;

	if (oldWindTrapColorTimer/20 != WindTrapColorTimer/20) {
		for(int h = 0; h < NUM_HOUSES; h++) {
			SDL_Surface* graphic = ObjPic[ObjPic_Windtrap][h];

			if(graphic == NULL)
				continue;

			if (settings.Video.DoubleBuffering && (graphic->format->BitsPerPixel == 8)) {
				if (!SDL_MUSTLOCK(graphic) || (SDL_LockSurface(graphic) == 0)) {
					Uint8	*pixel;
					int		i, j;

					for (i = 0; i < graphic->w; i++) {
						for (j = 0; j < graphic->h; j++) {
							pixel = &((Uint8*)graphic->pixels)[j * graphic->pitch + i];
								if (*pixel == 223)
									*pixel = 128 + abs(WindTrapColorTimer/20);
						}
					}
					SDL_UnlockSurface(graphic);
				}
			} else {
				SDL_SetColors(graphic, &graphic->format->palette->colors[128 + abs(WindTrapColorTimer/20)], 223, 1);
			}
		}
	}
}

shared_ptr<Shpfile> GFXManager::LoadShpfile(std::string filename) {
    try {
        return shared_ptr<Shpfile>(new Shpfile(pFileManager->OpenFile(filename), true));
    } catch (std::exception &e) {
        throw std::runtime_error("Error in file \"" + filename + "\":" + e.what());
    }
}

shared_ptr<Wsafile> GFXManager::LoadWsafile(std::string filename) {
    SDL_RWops* file_wsa = NULL;
    std::shared_ptr<Wsafile> wsafile;
    try {
        file_wsa = pFileManager->OpenFile(filename);
        wsafile = std::shared_ptr<Wsafile>(new Wsafile(file_wsa));
        SDL_RWclose(file_wsa);
        return wsafile;
	} catch (std::exception &e) {
	    if(file_wsa != NULL) {
            SDL_RWclose(file_wsa);
	    }
	    throw std::runtime_error(std::string("Error in file \"" + filename + "\":") + e.what());
	}
}

SDL_Surface* GFXManager::ExtractSmallDetailPic(std::string filename) {
	Wsafile* myWsafile;
	SDL_RWops* myFile;

	if((myFile = pFileManager->OpenFile(filename.c_str())) == NULL) {
		fprintf(stderr,"GFXManager::ExtractSmallDetailPic: Cannot open %s!\n",filename.c_str());
		exit(EXIT_FAILURE);
	}

	if((myWsafile = new Wsafile(myFile)) == NULL) {
		fprintf(stderr,"GFXManager::ExtractSmallDetailPic: Cannot open %s!\n",filename.c_str());
		exit(EXIT_FAILURE);
	}

	SDL_Surface* tmp;

	if((tmp = myWsafile->getPicture(0)) == NULL) {
		fprintf(stderr,"GFXManager::ExtractSmallDetailPic: Cannot decode first frame in file %s!\n",filename.c_str());
		exit(EXIT_FAILURE);
	}

	if((tmp->w != 184) || (tmp->h != 112)) {
		fprintf(stderr,"GFXManager::ExtractSmallDetailPic: Picture %s is not 184x112!\n",filename.c_str());
		exit(EXIT_FAILURE);
	}

	SDL_Surface* returnPic;

	// create new picture surface
	if((returnPic = SDL_CreateRGBSurface(SDL_HWSURFACE,91,55,8,0,0,0,0))== NULL) {
		fprintf(stderr,"GFXManager::ExtractSmallDetailPic: Cannot create new Picture for %s!\n",filename.c_str());
		exit(EXIT_FAILURE);
	}

	palette.applyToSurface(returnPic);
	SDL_LockSurface(returnPic);
	SDL_LockSurface(tmp);

	//Now we can copy pixel by pixel
	for(int y = 0; y < 55;y++) {
		for(int x = 0; x < 91; x++) {
			*( ((char*) (returnPic->pixels)) + y*returnPic->pitch + x)
				= *( ((char*) (tmp->pixels)) + ((y*2)+1)*tmp->pitch + (x*2)+1);
		}
	}

	SDL_UnlockSurface(tmp);
	SDL_UnlockSurface(returnPic);

	SDL_FreeSurface(tmp);
	delete myWsafile;
	SDL_RWclose(myFile);

	return returnPic;
}

Animation* GFXManager::LoadAnimationFromWsa(std::string filename) {
	SDL_RWops* file;
	if((file = pFileManager->OpenFile(filename)) == NULL) {
		fprintf(stderr,"GFXManager::LoadAnimationFromWsa(): Cannot open %s!\n",filename.c_str());
		exit(EXIT_FAILURE);
	}

	Wsafile* wsafile;
	if((wsafile = new Wsafile(file)) == NULL) {
		fprintf(stderr,"GFXManager::LoadAnimationFromWsa(): Cannot open %s!\n",filename.c_str());
		exit(EXIT_FAILURE);
	}

	Animation* ret = wsafile->getAnimation(0,wsafile->getNumFrames() - 1,true,false);
	delete wsafile;
	SDL_RWclose(file);
	return ret;
}
