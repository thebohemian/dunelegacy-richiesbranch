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

#include <MapGenerator.h>

#include <globals.h>

#include <MapClass.h>
#include <House.h>
#include <Game.h>

#include <ObjectClass.h>
#include <structures/WallClass.h>
#include <structures/TurretClass.h>
#include <units/UnitClass.h>
#include <units/TankClass.h>


#define ROCKFILLER 1		//how many times random generator will try to remove sand "holes" for rock from the map
#define SPICEFILLER 1		//for spice
#define ROCKFIELDS 10		//how many fields it will randomly generate
#define SPICEFIELDS 10


bool fix_cell(int& cellX, int& cellY) {
	bool error = false;

	if (cellX < 0) {
		cellX = 0;
		error = true;
	} else if (cellX >= currentGameMap->sizeX) {
		cellX = currentGameMap->sizeX - 1;
		error = true;
	}

	if(cellY < 0) {
		cellY = 0;
		error = true;
	} else if (cellY >= currentGameMap->sizeY) {
		cellY = currentGameMap->sizeY - 1;
		error = true;
	}

	return error;
}

/**
    Checks if the tile to the left of (cellX,cellY) is of type tile
    \param cellX x-coordinate in tile coordinates
    \param cellY y-coordinate in tile coordinates
    \param type  the tile type to check
    \return 1 if of tile type, 0 if not
*/
int on_left(int cellX, int cellY, int type) {
	cellX--;
	fix_cell(cellX, cellY);

	if (currentGameMap->cell[cellX][cellY].getType() == type)
		return 1;

	return 0;
}

/**
    Checks if the tile to the right of (cellX,cellY) is of type tile
    \param cellX x-coordinate in tile coordinates
    \param cellY y-coordinate in tile coordinates
    \param type  the tile type to check
    \return 1 if of type tile, 0 if not
*/
int on_right(int cellX, int cellY, int tile) {
	cellX++;
	fix_cell(cellX, cellY);

	if (currentGameMap->cell[cellX][cellY].getType() == tile)
		return 1;

	return 0;
}

/**
    Checks if the tile above (cellX,cellY) is of type tile
    \param cellX x-coordinate in tile coordinates
    \param cellY y-coordinate in tile coordinates
    \param tile  the tile type to check
    \return 1 if of type tile, 0 if not
*/
int on_up(int cellX, int cellY, int tile) {
	cellY--;
	fix_cell(cellX, cellY);

	if (currentGameMap->cell[cellX][cellY].getType() == tile)
		return 1;

	return 0;
}

/**
    Checks if the tile below (cellX,cellY) is of type tile
    \param cellX x-coordinate in tile coordinates
    \param cellY y-coordinate in tile coordinates
    \param tile  the tile type to check
    \return 1 if of type tile, 0 if not
*/
int on_down(int cellX, int cellY, int tile) {
	cellY++;
	fix_cell(cellX, cellY);

	if (currentGameMap->cell[cellX][cellY].getType() == tile)
		return 1;

	return 0;
}

/**
    Count how many tiles around (cellX,cellY) are of type tile
    \param cellX x-coordinate in tile coordinates
    \param cellY y-coordinate in tile coordinates
    \param tile  the tile type to check
    \return number of surounding tiles of type tile (0 to 4)
*/
int side4(int cellX, int cellY, int tile) {
	// Check at 4 sides for 'tile'
	int flag = 0;

	if (on_left(cellX, cellY, tile))
		flag++;

	if (on_right(cellX, cellY, tile))
		flag++;

	if (on_up(cellX, cellY, tile))
		flag++;

	if (on_down(cellX, cellY, tile))
		flag++;

	return flag;
}



/**
    Removes holes in rock and spice
    \param tpe  the type to remove holes from
*/
void thick_spots(int tpe) {
    for (int i = 0; i < currentGameMap->sizeX; i++) {
        for (int j = 0; j < currentGameMap->sizeY; j++) {
            if (currentGameMap->cell[i][j].getType() != tpe) {
                // Found something else than what thickining
				if (side4(i, j, tpe) >= 3)
					currentGameMap->cell[i][j].setType(tpe);                // Seems enough of the type around it so make this rock

				if (side4(i, j, tpe) == 2) {
                    // Gamble, fifty fifty... rock or not?
					if (getRandomInt(0,1) == 1)
						currentGameMap->cell[i][j].setType(tpe);
				}
			}
		}
    }
}

/**
    This function creates a spot of type tpy.
    \param cellX    the x coordinate in tile coordinates to start making the spot
    \param cellY    the y coordinate in tile coordinates to start making the spot
    \param tpe      type of the spot
*/
void make_spot(int cellX, int cellY, int tpe) {
	int dir;		// Direction
	int j;			// Loop control

	for(j = 0; j < 1000; j++) {
		dir = getRandomInt(0,3);	// Random Dir

		switch(dir) {
			case 0 : cellX--; break;
			case 1 : cellX++; break;
			case 2 : cellY--; break;
			case 3 : cellY++; break;
		}

		fix_cell(cellX, cellY);

		if(tpe == Terrain_Spice) {
			if (currentGameMap->cell[cellX][cellY].getType() == Terrain_Rock)
                continue;		// Do not place the spice spot, priority is ROCK!
		}

		currentGameMap->cell[cellX][cellY].setType(tpe);
	}
}


/**
    Adds amount number of rock tiles to the map
    \param amount the number of rock tiles to add
*/
void add_rock_bits(int amount) {
	int spotX, spotY;
	int done = 0;
	while (done < amount) {
		spotX = getRandomInt(0, currentGameMap->sizeX-1);
		spotY = getRandomInt(0, currentGameMap->sizeY-1);

		if(currentGameMap->cell[spotX][spotY].getType() == Terrain_Sand) {
			currentGameMap->cell[spotX][spotY].setType(Terrain_Rock);
            done++;
		}
	}
}

/**
    Adds amount number of spice blooms to the map
    \param amount the number of spice blooms to add
*/
void add_blooms(int amount) {
	int spotX, spotY;
	int done = 0;
	while(done < amount) {
		spotX = getRandomInt(0, currentGameMap->sizeX-1);
		spotY = getRandomInt(0, currentGameMap->sizeY-1);
		if(currentGameMap->cell[spotX][spotY].getType() == Terrain_Sand) {
			currentGameMap->cell[spotX][spotY].setType(Terrain_SpiceBloom);      // Spice bloom
            done++;
        }
	}
}

/**
    Creates a random map
    \param sizeX width of the new map (in tiles)
    \param sizeY height of the new map (in tiles)
*/
void make_random_map(int sizeX, int sizeY) {
    int i, count;
    int spots = ROCKFIELDS;
    int fields = SPICEFIELDS;
    int spotX, spotY;

	currentGameMap = new MapClass(sizeX, sizeY);

    // set whole map to type Terrain_Sand
	for (int i = 0; i < currentGameMap->sizeX; i++) {
		for (int j = 0; j < currentGameMap->sizeY; j++) {
			currentGameMap->cell[i][j].setType(Terrain_Sand);
		}
	}

    for (i = 0; i < spots; i++) {
        spotX = getRandomInt(0, currentGameMap->sizeX-1);
        spotY = getRandomInt(0, currentGameMap->sizeY-1);

        make_spot(spotX, spotY, Terrain_Rock);
    }

    // Spice fields
    for (i = 0; i < fields; i++) {
        spotX = getRandomInt(0, currentGameMap->sizeX-1);
        spotY = getRandomInt(0, currentGameMap->sizeY-1);

        make_spot(spotX, spotY, Terrain_Spice);
    }

    for(count = 0; count < ROCKFILLER; count++) {
        thick_spots(Terrain_Rock); //SPOT ROCK
    }

    for(count = 0; count < SPICEFILLER; count++) {
        thick_spots(Terrain_Spice);
    }

 	add_rock_bits(getRandomInt(0,9));
	add_blooms(getRandomInt(0,9));
}
