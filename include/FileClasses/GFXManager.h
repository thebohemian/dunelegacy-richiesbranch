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

#ifndef GFXMANAGER_H
#define GFXMANAGER_H

#include <SDL.h>
#include "Animation.h"
#include "Shpfile.h"
#include "Wsafile.h"
#include <DataTypes.h>
#include "PictureFactory.h"

#include <string>
#include <misc/memory.h>

#define NUM_MAPCHOICEPIECES	28
#define NUM_MAPCHOICEARROWS	9

// ObjPics
typedef enum {
	ObjPic_Tank_Base,
	ObjPic_Tank_Gun,
	ObjPic_Siegetank_Base,
	ObjPic_Siegetank_Gun,
	ObjPic_Devastator_Base,
	ObjPic_Devastator_Gun,
	ObjPic_Sonictank_Gun,
	ObjPic_Launcher_Gun,
	ObjPic_Quad,
	ObjPic_Trike,
	ObjPic_Harvester,
	ObjPic_Harvester_Sand,
	ObjPic_MCV,
	ObjPic_Carryall,
	ObjPic_CarryallShadow,
	ObjPic_Frigate,
    ObjPic_FrigateShadow,
	ObjPic_Ornithopter,
    ObjPic_OrnithopterShadow,
	ObjPic_Trooper,
	ObjPic_Infantry,
	ObjPic_Saboteur,
	ObjPic_Sandworm,
	ObjPic_ConstructionYard,
	ObjPic_Windtrap,
	ObjPic_Refinery,
	ObjPic_Barracks,
	ObjPic_WOR,
	ObjPic_Radar,
	ObjPic_LightFactory,
	ObjPic_Silo,
	ObjPic_HeavyFactory,
	ObjPic_HighTechFactory,
	ObjPic_IX,
	ObjPic_Palace,
	ObjPic_RepairYard,
	ObjPic_Starport,
	ObjPic_GunTurret,
	ObjPic_RocketTurret,
	ObjPic_Wall,
	ObjPic_Bullet_SmallRocket,
	ObjPic_Bullet_MediumRocket,
	ObjPic_Bullet_LargeRocket,
	ObjPic_Bullet_Small,
	ObjPic_Bullet_Medium,
	ObjPic_Bullet_Sonic,
	ObjPic_Hit_Gas,
	ObjPic_Hit_Shell,
	ObjPic_ExplosionSmall,
	ObjPic_ExplosionMedium1,
	ObjPic_ExplosionMedium2,
	ObjPic_ExplosionLarge1,
	ObjPic_ExplosionLarge2,
	ObjPic_ExplosionSmallUnit,
	ObjPic_ExplosionFlames,
	ObjPic_DeadInfantry,
	ObjPic_DeadAirUnit,
	ObjPic_Smoke,
	ObjPic_SandwormShimmerMask,
	ObjPic_Terrain,
	ObjPic_DestroyedStructure,
	ObjPic_RockDamage,
	ObjPic_SandDamage,
	ObjPic_Terrain_Hidden,
	ObjPic_Terrain_Tracks,
	NUM_OBJPICS
} ObjPic_enum;

#define GROUNDUNIT_ROW(i) (i+2)|TILE_NORMAL,(i+1)|TILE_NORMAL,i|TILE_NORMAL,(i+1)|TILE_FLIPV,(i+2)|TILE_FLIPV,(i+3)|TILE_FLIPV, (i+4)|TILE_NORMAL,(i+3)|TILE_NORMAL
#define AIRUNIT_ROW(i) (i+2)|TILE_NORMAL,(i+1)|TILE_NORMAL,i|TILE_NORMAL,(i+1)|TILE_FLIPV,(i+2)|TILE_FLIPV,(i+1)|TILE_ROTATE, i|TILE_FLIPH,(i+1)|TILE_FLIPH
#define ORNITHOPTER_ROW(i) (i+6)|TILE_NORMAL,(i+3)|TILE_NORMAL,i|TILE_NORMAL,(i+3)|TILE_FLIPV,(i+6)|TILE_FLIPV,(i+3)|TILE_ROTATE, i|TILE_FLIPH,(i+3)|TILE_FLIPH
#define INFANTRY_ROW(i) (i+3)|TILE_NORMAL,i|TILE_NORMAL,(i+3)|TILE_FLIPV,(i+6)|TILE_NORMAL
#define HARVESTERSAND_ROW(i) (i+6)|TILE_NORMAL,(i+3)|TILE_NORMAL,i|TILE_NORMAL,(i+3)|TILE_FLIPV,(i+6)|TILE_FLIPV,(i+9)|TILE_FLIPV,(i+12)|TILE_NORMAL,(i+9)|TILE_NORMAL
#define ROCKET_ROW(i)	(i+4)|TILE_NORMAL,(i+3)|TILE_NORMAL,(i+2)|TILE_NORMAL,(i+1)|TILE_NORMAL,i|TILE_NORMAL,(i+1)|TILE_FLIPV,(i+2)|TILE_FLIPV,(i+3)|TILE_FLIPV, \
						(i+4)|TILE_FLIPV,(i+3)|TILE_ROTATE,(i+2)|TILE_ROTATE, (i+1)|TILE_ROTATE,i|TILE_FLIPH,(i+1)|TILE_FLIPH,(i+2)|TILE_FLIPH,(i+3)|TILE_FLIPH


// SmallDetailPics
typedef enum {
	Picture_Barracks,
	Picture_ConstructionYard,
	Picture_Carryall,
	Picture_Devastator,
	Picture_Deviator,
	Picture_DeathHand,
	Picture_Fremen,
	Picture_GunTurret,
	Picture_Harvester,
	Picture_HeavyFactory,
	Picture_HighTechFactory,
	Picture_Soldier,
	Picture_IX,
	Picture_Launcher,
	Picture_LightFactory,
	Picture_MCV,
	Picture_Ornithopter,
	Picture_Palace,
	Picture_Quad,
	Picture_Radar,
	Picture_Raider,
	Picture_Refinery,
	Picture_RepairYard,
	Picture_RocketTurret,
	Picture_Saboteur,
	Picture_Sardaukar,
	Picture_SiegeTank,
	Picture_Silo,
	Picture_Slab1,
	Picture_Slab4,
	Picture_SonicTank,
	Picture_StarPort,
	Picture_Tank,
	Picture_Trike,
	Picture_Trooper,
	Picture_Wall,
	Picture_WindTrap,
	Picture_WOR,
	NUM_SMALLDETAILPICS
} SmallDetailPics_Enum;

// UI Graphics
typedef enum {
	UI_RadarAnimation,
	UI_CursorNormal,
	UI_CursorUp,
	UI_CursorRight,
	UI_CursorDown,
	UI_CursorLeft,
	UI_CursorMove,
	UI_CursorAttack,
    UI_CursorCapture,
	UI_CreditsDigits,
	UI_GameBar,
	UI_Indicator,
	UI_InvalidPlace,
	UI_ValidPlace,
	UI_MenuBackground,
	UI_Background,
	UI_GameStatsBackground,
	UI_SelectionBox,
	UI_TopBar,
	UI_ButtonUp,
	UI_ButtonUp_Pressed,
	UI_ButtonDown,
	UI_ButtonDown_Pressed,
	UI_MessageBox,
	UI_Mentat,
	UI_Mentat_Pressed,
	UI_Options,
	UI_Options_Pressed,
	UI_Upgrade,
	UI_Upgrade_Pressed,
	UI_Repair,
	UI_Repair_Pressed,
	UI_Difficulty,
	UI_HeraldAtre_Colored,
	UI_HeraldHark_Colored,
	UI_HeraldOrd_Colored,
	UI_Dif_Easy,
	UI_Dif_Hard,
	UI_Dif_Medium,
	UI_Minus,
	UI_Minus_Pressed,
	UI_Plus,
	UI_Plus_Pressed,
	UI_HouseSelect,
	UI_MissionSelect,
	UI_OptionsMenu,
	UI_LoadSaveWindow,
	UI_GameMenu,
	UI_HouseChoiceBackground,
	UI_MentatBackground,
	UI_MentatYes,
	UI_MentatYes_Pressed,
	UI_MentatNo,
	UI_MentatNo_Pressed,
	UI_MentatExit,
	UI_MentatExit_Pressed,
	UI_MentatProcced,
	UI_MentatProcced_Pressed,
	UI_MentatRepeat,
	UI_MentatRepeat_Pressed,
	UI_PlanetBackground,
	UI_MenuButtonBorder,
	UI_DuneLegacy,
	UI_MapChoiceScreen,
	UI_MapChoiceMapOnly,
	UI_MapChoiceMap,
	UI_MapChoiceClickMap,
	UI_StructureSizeLattice,
	UI_StructureSizeConcrete,
	NUM_UIGRAPHICS
} UIGraphics_Enum;

//Animation
typedef enum {
	Anim_AtreidesEyes,
	Anim_AtreidesMouth,
	Anim_AtreidesShoulder,
	Anim_AtreidesBook,
	Anim_HarkonnenEyes,
	Anim_HarkonnenMouth,
	Anim_HarkonnenShoulder,
	Anim_OrdosEyes,
	Anim_OrdosMouth,
	Anim_OrdosShoulder,
	Anim_OrdosRing,
	Anim_MercenaryEyes,
	Anim_MercenaryMouth,
	Anim_AtreidesPlanet,
	Anim_HarkonnenPlanet,
	Anim_OrdosPlanet,
	Anim_Win1,
	Anim_Win2,
	Anim_Lose1,
	Anim_Lose2,
	Anim_Barracks,
	Anim_Carryall,
	Anim_ConstructionYard,
	Anim_Fremen,
	Anim_DeathHand,
	Anim_Devastator,
	Anim_Harvester,
	Anim_Radar,
	Anim_HighTechFactory,
	Anim_SiegeTank,
	Anim_HeavyFactory,
	Anim_Trooper,
	Anim_Infantry,
	Anim_IX,
	Anim_LightFactory,
	Anim_Tank,
	Anim_MCV,
	Anim_Deviator,
	Anim_Ornithopter,
	Anim_Raider,
	Anim_Palace,
	Anim_Quad,
	Anim_Refinery,
	Anim_RepairYard,
	Anim_Launcher,
	Anim_RocketTurret,
	Anim_Saboteur,
	Anim_Slab1,
	Anim_SonicTank,
	Anim_StarPort,
	Anim_Silo,
	Anim_Trike,
	Anim_GunTurret,
	Anim_Wall,
	Anim_WindTrap,
	Anim_WOR,
	Anim_Sandworm,
	Anim_Sardaukar,
	Anim_Frigate,
	Anim_Slab4,
	NUM_ANIMATION
} Animation_enum;


class GFXManager {
public:
	GFXManager();
	~GFXManager();

	SDL_Surface*	getObjPic(unsigned int id, int house=HOUSE_HARKONNEN);
	SDL_Surface*	getSmallDetailPic(unsigned int id);
	SDL_Surface*	getUIGraphic(unsigned int id, int house=HOUSE_HARKONNEN);
	SDL_Surface*	getMapChoicePiece(unsigned int num, int house);
	SDL_Surface*	getMapChoiceArrow(unsigned int num);

	SDL_Surface*    getTransparent40Surface() { return Transparent40Surface; };
	SDL_Surface*    getTransparent150Surface() { return Transparent150Surface; };

	Animation*		getAnimation(unsigned int id);
	void			DoWindTrapPalatteAnimation();

private:
	Animation* LoadAnimationFromWsa(std::string filename);

    std::shared_ptr<Shpfile>  LoadShpfile(std::string filename);
    std::shared_ptr<Wsafile>  LoadWsafile(std::string filename);

	SDL_Surface*	ExtractSmallDetailPic(std::string filename);

	SDL_Surface*	ObjPic[NUM_OBJPICS][(int) NUM_HOUSES];
	SDL_Surface*	SmallDetailPic[NUM_SMALLDETAILPICS];
	SDL_Surface*	UIGraphic[NUM_UIGRAPHICS][(int) NUM_HOUSES];
	SDL_Surface*	MapChoicePieces[NUM_MAPCHOICEPIECES][(int) NUM_HOUSES];
	SDL_Surface*	MapChoiceArrows[NUM_MAPCHOICEARROWS];
	Animation*		Anim[NUM_ANIMATION];

	SDL_Surface*    Transparent40Surface;
	SDL_Surface*    Transparent150Surface;

	int WindTrapColorTimer;
};

#endif // GFXMANAGER_H
