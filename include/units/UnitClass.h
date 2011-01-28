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

#ifndef UNITCLASS_H
#define UNITCLASS_H

#include <ObjectClass.h>

#include <list>

// forward declarations
class TerrainClass;

class UnitClass : public ObjectClass
{
public:
	UnitClass(House* newOwner);
	UnitClass(Stream& stream);
	void init();
	virtual ~UnitClass();

	virtual void save(Stream& stream) const;

	void blitToScreen();

	virtual ObjectInterface* GetInterfaceContainer();

	virtual void checkPos() = 0;
	virtual void deploy(const Coord& newLocation);

	virtual void destroy();
	void deviate(House* newOwner);

	virtual void drawSelectionBox();

	int getDrawnX() const;
	int getDrawnY() const;

	/**
		This method is called when an unit is ordered by a right click
		\param	xPos	the x position on the map
		\param	yPos	the y position on the map
	*/
	virtual void HandleActionClick(int xPos, int yPos);

	/**
		This method is called when an unit is ordered to attack
		\param	xPos	the x position on the map
		\param	yPos	the y position on the map
	*/
	virtual void HandleAttackClick(int xPos, int yPos);

    /**
		This method is called when an unit is ordered to move
		\param	xPos	the x position on the map
		\param	yPos	the y position on the map
	*/
	virtual void HandleMoveClick(int xPos, int yPos);

	/**
		This method is called when an unit is ordered to be in a new attack mode
		\param	newAttackMode   the new attack mode the unit is put in.
	*/
	virtual void HandleSetAttackModeClick(ATTACKMODE newAttackMode);

	/**
		This method is called when an unit should move to (xPos,yPos)
		\param	xPos	the x position on the map
		\param	yPos	the y position on the map
		\param	bForced	true, if the unit should ignore everything else
	*/
	virtual void DoMove2Pos(int xPos, int yPos, bool bForced);

	/**
		This method is called when an unit should move to coord
		\param	coord	the position on the map
		\param	bForced	true, if the unit should ignore everything else
	*/
	virtual void DoMove2Pos(const Coord& coord, bool bForced);

	/**
		This method is called when an unit should move to another unit/structure
		\param	TargetObjectID	the ID of the other unit/structure
	*/
	virtual void DoMove2Object(Uint32 TargetObjectID);

	/**
		This method is called when an unit should move to another unit/structure
		\param	pTargetObject	the other unit/structure
	*/
	virtual void DoMove2Object(ObjectClass* pTargetObject);

	/**
		This method is called when an unit should attack a position
		\param	xPos	the x position on the map
		\param	yPos	the y position on the map
		\param	bForced	true, if the unit should ignore everything else
	*/
	virtual void DoAttackPos(int xPos, int yPos, bool bForced);

	/**
		This method is called when an unit should attack to another unit/structure
		\param	pTargetObject	the target unit/structure
	*/
	virtual void DoAttackObject(ObjectClass* pTargetObject);

	/**
		This method is called when an unit should attack to another unit/structure
		\param	TargetObjectID	the ID of the other unit/structure
	*/
	virtual void DoAttackObject(Uint32 TargetObjectID);

	/**
		This method is called when an unit should change it's current attack mode
		\param	newAttackMode	the new attack mode
	*/
	void DoSetAttackMode(ATTACKMODE newAttackMode);

    virtual void handleDamage(int damage, Uint32 damagerID, House* damagerOwner);

	virtual void DoRepair() { };

	bool isInAttackModeRange(ObjectClass* object);

	void setAngle(int newAngle);

	virtual void setTarget(ObjectClass* newTarget);

	void setGettingRepaired();

	inline void setGuardPoint(const Coord& newGuardPoint) { setGuardPoint(newGuardPoint.x, newGuardPoint.y); }

	void setGuardPoint(int newX, int newY);

	void setLocation(int xPos, int yPos);

	inline void setLocation(const Coord& location) { setLocation(location.x, location.y); }

	virtual void setPickedUp(UnitClass* newCarrier);

	/**
        Updates this unit.
        \return true if this unit still exists, false if it was destroyed
	*/
	virtual bool update();

	virtual bool canPass(int xPos, int yPos) const;

	virtual int getCurrentAttackAngle();

    virtual double getMaxSpeed() const;


	inline void clearNextSpotFound() { nextSpotFound = false; }

	inline void clearPath() { pathList.clear(); }

	inline void setAttacking(bool status) { attacking = status; }

	inline void setSpeedCap(double newSpeedCap) { speedCap = newSpeedCap; }
	inline bool isAttacking() const { return attacking; }
	inline bool isTracked() const { return tracked; }

	inline bool isTurreted() const { return turreted; }

	inline bool isMoving() const { return moving; }

	inline bool wasDeviated() const { return (owner != realOwner); }

	inline int getAngle() const { return drawnAngle; }

	inline ATTACKMODE getAttackMode() const { return attackMode; }

	inline const Coord& getGuardPoint() const { return guardPoint; }
	inline House* getRealOwner() { return realOwner; }

	virtual void PlayAttackSound();

protected:

	virtual void attack();

	virtual void engageTarget();
	virtual void move();

	virtual void navigate();

	virtual void setSpeeds();

	virtual void targeting();

	virtual void turn();
	void turnLeft();
	void turnRight();

	void quitDeviation();

	bool SearchPathWithAStar();

    void drawSmoke(int x, int y);

	// constant for all units of the same type
    bool    tracked;                ///< Does this unit have tracks?
	bool    turreted;               ///< Does this unit have a turret?
    int     numWeapons;             ///< How many weapons do we have?
    int     bulletType;             ///< Type of bullet to shot with
    int     weaponReloadTime;       ///< The time between two shots

    // unit state/properties
    bool	goingToRepairYard;      ///< Are we currently going to a repair yard?
	bool    pickedUp;               ///< Were we picked up by a carryall?
    bool    bFollow;                ///< Do we currently follow some other unit (specified by target)?
    Coord   guardPoint;             ///< The guard point where to return to after the micro-AI hunted some nearby enemy unit

    bool    moving;                 ///< Are we currently moving?
    bool    turning;                ///< Are we currently turning?
    bool    justStoppedMoving;      ///< Do we have just stopped moving?
    double	speedCap;               ///< What is the current speed cap for this unit?
    double	xSpeed;                 ///< Speed in x direction
    double	ySpeed;                 ///< Speed in y direction

    double	targetDistance;         ///< Distance to the destination
    Sint8   targetAngle;            ///< Angle to the destination

    // path finding
    Uint8   noCloserPointCount;     ///< How often have we tried to dinf a path?
    bool    nextSpotFound;          ///< Is the next spot to move to already found?
    Sint8   nextSpotAngle;          ///< The angle to get to the next spot
	Sint32  recalculatePathTimer;   ///< This timer is for recalculating the best path after x ticks
    Coord	nextSpot;               ///< The next spot to move to
    std::list<Coord>    pathList;   ///< The path to the destination found so far

    Sint32  findTargetTimer;        ///< When to look for the next target?
    bool	attacking;              ///< Attack the next structure/unit?
	bool	bAttackPos;             ///< Attack a position? (if true, the position is saved in destination)
	Sint32  primaryWeaponTimer;     ///< When can the primary weapon shot again?
	Sint32  secondaryWeaponTimer;   ///< When can the secondary weapon shot again?

    // deviation
	Sint32          deviationTimer; ///< When to revert back to the real owner?
    House*          realOwner;      ///< The real owner of this unit

    // drawing information
	int drawnFrame;                 ///< Which row in the picture should be drawn

    int         smokeCounter;       ///< When to show the next smoke frame?
	int         smokeFrame;         ///< Current smoke frame
};

#endif //UNITCLASS_H
