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

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define SCREEN_BPP			8
#define SCREEN_MIN_WIDTH	640
#define SCREEN_MIN_HEIGHT	480

#define LNG_UNKNOWN 0
#define	LNG_ENG     1
#define LNG_GER     2
#define LNG_FRE     3


#define SAVEMAGIC           8675309
#define SAVEGAMEVERSION     9600

#ifndef M_PI
    #define M_PI 3.1415926535897932384626433832795
#endif

#define DIAGONALSPEEDCONST 0.70710678118654752440084436210485
#define DIAGONALCOST 1.4142135623730950488016887242097


#define GAMESPEED_MAX 32
#define GAMESPEED_MIN 8
#define GAMESPEED_DEFAULT 16
#define MILLI2CYCLES(MILLISECONDS) ((MILLISECONDS)/GAMESPEED_DEFAULT)   // this is calculated in game milliseconds (dune 2 has about the same in game speed "fastest")
#define VOLUME_MAX 100
#define VOLUME_MIN 0

#define WINLOSEFLAGS_AI_NO_BUILDINGS        0x01
#define WINLOSEFLAGS_HUMAN_HAS_BUILDINGS    0x02
#define WINLOSEFLAGS_QUOTA                  0x04
#define WINLOSEFLAGS_TIMEOUT                0x08

#define MAX_PLAYERS 6

#define MAX_XSIZE 256
#define MAX_YSIZE 256

#define BUILDRANGE 2
#define RANDOMSPICEMIN 75	//how much spice on each spice tile
#define RANDOMSPICEMAX 150
#define RANDOMTHICKSPICEMIN 150
#define RANDOMTHICKSPICEMAX 300
#define conv2char 2.0 * M_PI / 256
#define BLOCKSIZE 16		//size of tile pieces 16x16
#define GAMEBARWIDTH 144
#define NONE ((Uint32) -1)			// unsigned -1
#define INVALID_POS	(-1)
#define INVALID (-1)

#define DEVIATIONTIME MILLI2CYCLES(120*1000)
#define HARVESTERMAXSPICE 700
#define HARVESTSPEED 0.1
#define HEAVILYDAMAGEDRATIO 0.3	//if health/getMaxHealth() < this, when damaged will become heavily damage- smoke and shit
#define HEAVILYDAMAGEDSPEEDMULTIPLIER 0.75	// 0.666666666666
#define RANDOMTURNTIMER 2000	//less of this makes units randomly turn more
#define NUMSELECTEDLISTS 9
#define NUM_INFANTRY_PER_CELL 5		//how many infantry can fit in a cell
#define LASTSANDFRAME 2	//is number spice output frames - 1

#define UNIT_REPAIRCOST 0.1
#define DEFAULT_GUARDRANGE 10			//0 - 10, how far unit will search for enemy when guarding
#define DEFAULT_STARTINGCREDITS 3000

#define COLOR_TRANSPARENT 0
#define COLOR_BLACK 12
#define COLOR_DARKGREY 13
#define COLOR_BLUE 11
#define COLOR_LIGHTBLUE 9
#define COLOR_LIGHTGREY 14
#define COLOR_BROWN 95
#define COLOR_YELLOW 123
#define COLOR_GREEN 3
#define COLOR_LIGHTGREEN 4
#define COLOR_RED 231
#define COLOR_WHITE 15
#define COLOR_ORANGE 83
#define COLOR_COLORCYCLE 255

#define COLOR_DESERTSAND 105
#define COLOR_SPICE 111
#define COLOR_THICKSPICE 116
#define COLOR_MOUNTAIN 47

#define COLOR_ATREIDES 160
#define COLOR_ORDOS 176
#define COLOR_HARKONNEN 144
#define COLOR_SARDAUKAR 208
#define COLOR_FREMEN 192
#define COLOR_MERCENARY 224

#endif //DEFINITIONS_H
