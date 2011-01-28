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

#ifndef ROCKETTURRETCLASS_H
#define ROCKETTURRETCLASS_H

#include <structures/TurretClass.h>

class RocketTurretClass : public TurretClass
{
public:
	RocketTurretClass(House* newOwner);
	RocketTurretClass(Stream& stream);
	void init();
	virtual ~RocketTurretClass();

	void doSpecificStuff();
	bool canAttack(const ObjectClass* object) const;


private:
};

#endif //ROCKETTURRETCLASS_H
