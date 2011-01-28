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

#ifndef TURRETCLASS_H
#define TURRETCLASS_H

#include <structures/StructureClass.h>

#include <FileClasses/SFXManager.h>

class TurretClass : public StructureClass
{
public:
	TurretClass(House* newOwner);
	TurretClass(Stream& stream);
	void init();
	virtual ~TurretClass();

	virtual void save(Stream& stream) const;

	void doSpecificStuff();

	virtual void HandleActionCommand(int xPos, int yPos);
	virtual void DoAttackObject(Uint32 TargetObjectID);

	void turnLeft();
	void turnRight();
	virtual void attack();

	inline int getTurretAngle() { return lround(angle); }

protected:
    // constant for all turrets of the same type
    int bulletType;             ///< The type of bullet used
    int weaponReloadTime;       ///< Time between to shots
	Sound_enum attackSound;     ///< The id of the sound to play when attack

	// turret state
	Sint32  findTargetTimer;    ///< Timer used for finding a new target
    Sint32  weaponTimer;        ///< Time until we can shot again
};

#endif //TURRETCLASS_H
