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

#ifndef REPAIRYARDCLASS_H
#define REPAIRYARDCLASS_H

#include <structures/StructureClass.h>
#include <ObjectPointer.h>

class Carryall;

class RepairYardClass : public StructureClass
{
public:
	RepairYardClass(House* newOwner);
    RepairYardClass(Stream& stream);
	void init();
	virtual ~RepairYardClass();

	virtual void save(Stream& stream) const;

	virtual ObjectInterface* GetInterfaceContainer();

	void deployRepairUnit(Carryall* pCarryall = NULL);
	void doSpecificStuff();

	inline void book() { bookings++; }
	inline void unBook() { bookings--; }
	inline void assignUnit(ObjectPointer newUnit) { repairUnit = newUnit; repairing = true; }
	inline bool isFree() { return !repairing; }
	inline int getNumBookings() { return bookings; }	//number of harvesters goings there
	inline UnitClass* getRepairUnit() { return repairUnit.getUnitPointer(); }

	void destroy();

private:
	bool            repairing;      ///< Currently repairing?
	ObjectPointer   repairUnit;     ///< The unit to repair
	Uint32          bookings;       ///< Number of bookings for this repair yard
};

#endif // REPAIRYARDCLASS_H
