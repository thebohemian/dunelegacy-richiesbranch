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

#ifndef TANKBASECLASS_H
#define TANKBASECLASS_H

#include <units/TrackedUnit.h>

class TankBaseClass : public TrackedUnit
{
public:
	TankBaseClass(House* newOwner);
	TankBaseClass(Stream& stream);
	void init();
	virtual ~TankBaseClass();

	virtual void save(Stream& stream) const;

	void navigate();

	inline int getTurretAngle() { return lround(turretAngle); }

	virtual int getCurrentAttackAngle();

protected:
	void engageTarget();
	void targeting();
	void turn();
	void turnTurretLeft();
	void turnTurretRight();

    // constant for all tanks of the same type
	double      turretTurnSpeed;    ///< How fast can we turn the turret

    // tank state
    double  turretAngle;            ///< The angle of the turret
	Sint8   drawnTurretAngle;       ///< The drawn angle of the turret

	ObjectPointer	closeTarget;	///< a enemy target that can be shot at while moving

    // drawing information
	SDL_Surface     *turretGraphic;     ///< The turret graphic
	int             GunGraphicID;       ///< The id of the turret graphic (needed if we want to reload the graphic)
};

#endif // TANKBASECLASS_H
