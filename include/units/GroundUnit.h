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

#ifndef GROUNDUNIT_H
#define GROUNDUNIT_H

#include <units/UnitClass.h>


class GroundUnit : public UnitClass
{

public:

	GroundUnit(House* newOwner);
	GroundUnit(Stream& stream);
	void init();
	virtual ~GroundUnit();

	virtual void save(Stream& stream) const;

	virtual void assignToMap(const Coord& pos);

	void playConfirmSound();
	void playSelectSound();

	void bookCarrier(UnitClass* newCarrier);
	virtual void checkPos();
	bool requestCarryall();
	void setPickedUp(UnitClass* newCarrier);

	virtual void DoRepair();

	inline void setAwaitingPickup(bool status) { awaitingPickup = status; }
	inline bool isAwaitingPickup() { return awaitingPickup; }
	bool hasBookedCarrier() const;
	UnitClass* getCarrier() const;

protected:

	void	navigate();

	bool	awaitingPickup;     ///< Is this unit waiting for pickup?
	Uint32  bookedCarrier;      ///< What is the carrier if waiting for pickup?
};

#endif // GROUNDUNIT_H
