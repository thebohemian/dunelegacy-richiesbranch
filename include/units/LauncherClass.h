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

#ifndef LAUNCHERCLASS_H
#define LAUNCHERCLASS_H

#include <units/TrackedUnit.h>

class LauncherClass : public TrackedUnit
{

public:
	LauncherClass(House* newOwner);
	LauncherClass(Stream& stream);
	void init();
	virtual ~LauncherClass();

	void blitToScreen();
	virtual void destroy();
	virtual bool canAttack(const ObjectClass* object) const;

	void PlayAttackSound();

private:
    // drawing information
	SDL_Surface     *turretGraphic;     ///< The turret graphic
	int             GunGraphicID;       ///< The id of the turret graphic (needed if we want to reload the graphic)

	static const Coord TurretOffset[];
};

#endif //LAUNCHERCLASS_H
