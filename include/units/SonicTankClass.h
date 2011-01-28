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

#ifndef SONICTANKCLASS_H
#define SONICTANKCLASS_H

#include <units/TrackedUnit.h>

class SonicTankClass : public TrackedUnit
{
public:
	SonicTankClass(House* newOwner);
	SonicTankClass(Stream& stream);
	void init();
	virtual ~SonicTankClass();

	void blitToScreen();

	virtual void destroy();

	void handleDamage(int damage, Uint32 damagerID, House* damagerOwner);

	bool canAttack(const ObjectClass* object) const;

	void PlayAttackSound();

private:
    // drawing information
	SDL_Surface     *turretGraphic;     ///< The turret graphic
	int             GunGraphicID;       ///< The id of the turret graphic (needed if we want to reload the graphic)
};

#endif //SONICTANKCLASS_H
