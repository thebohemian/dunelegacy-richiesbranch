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

#ifndef SIEGETANKCLASS_H
#define SIEGETANKCLASS_H

#include <units/TankBaseClass.h>

class SiegeTankClass : public TankBaseClass
{
public:
	SiegeTankClass(House* newOwner);
	SiegeTankClass(Stream& stream);
	void init();
	virtual ~SiegeTankClass();

	void blitToScreen();

	virtual void destroy();

	void PlayAttackSound();

private:
	static const Coord TurretOffset[];
};

#endif // SIEGETANKCLASS_H
