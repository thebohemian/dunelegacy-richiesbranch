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

#ifndef BULLETCLASS_H
#define BULLETCLASS_H

#include <globals.h>

#include <DataTypes.h>
#include <ScreenBorder.h>
#include <misc/Stream.h>

// forward declarations
class House;


class BulletClass
{
public:
	BulletClass(Uint32 shooterID, Coord* newLocation, Coord* newDestination, Uint32 bulletID, int damage, bool air);
	BulletClass(Stream& stream);
	int init();
	~BulletClass();

	void save(Stream& stream) const;

	void blitToScreen();

	void update();
	void destroy();

	inline bool isDirectShot() const { return ((source.x == destination.x) && (source.y == destination.y)); }
	inline int getDrawnX() const { return screenborder->world2screenX(realX - imageW/2); }
	inline int getDrawnY() const { return screenborder->world2screenY(realY - imageH/2); }
	inline int getBulletID() const { return bulletID; }
	inline int getImageW() const { return imageW; }
	inline int getImageH() const { return imageH; }
	inline Coord getImageSize() const { return Coord(imageW, imageH); }
	inline double getRealX() const { return realX; }
	inline double getRealY() const { return realY; }

private:
    // constants for each bullet type
    int     damageRadius;       ///< The radius of the bullet
    bool    groundBlocked;      ///< false = bullet goes through objects, true = bullet explodes at ground objects
    double  speed;              ///< The speed of this bullet

    // bullet state
    Uint32  bulletID;           ///< The ID of the bullet

	bool	airAttack;          ///< Is this an air attack?
    Sint32  damage;             ///< the damage this bullet causes

	Uint32  shooterID;          ///< the ItemId of the shooter
	House*	owner;              ///< the owner of this bullet

	Coord   source;             ///< the source location (in world coordinates) of this bullet
	Coord	destination;        ///< the destination (in world coordinates) of this bullet
	Coord   location;           ///< the current location of this bullet (in map coordinates)
	double  realX;              ///< the x-coordinate of the current position (in world coordinates)
	double  realY;              ///< the y-coordinate of the current position (in world coordinates)

	double  xSpeed;             ///< Speed in x direction
	double	ySpeed;             ///< Speed in x direction

	Sint8   drawnAngle;         ///< the drawn angle of the bullet
	double	angle;              ///< the angle of the bullet

	// drawing information
	SDL_Surface*    graphic;    ///< The graphic of the bullet
	int             imageW;     ///< Width of the image
	int             imageH;     ///< Height of the image
	int             numFrames;  ///< Number of frames of the bullet
};

#endif // BULLETCLASS_H
