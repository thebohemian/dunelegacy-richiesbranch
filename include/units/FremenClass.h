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

#ifndef FREMENCLASS_H
#define FREMENCLASS_H

#include <units/InfantryClass.h>

class FremenClass : public InfantryClass
{
public:
	FremenClass(House* newOwner);
	FremenClass(Stream& stream);
	void init();
	virtual ~FremenClass();

	bool canAttack(const ObjectClass* object) const;
	void PlayAttackSound();
};

#endif // FREMENCLASS_H
