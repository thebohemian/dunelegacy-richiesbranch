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

#ifndef REFINERYCLASS_H
#define REFINERYCLASS_H

#include <structures/StructureClass.h>
#include <ObjectPointer.h>

// forward declarations
class HarvesterClass;
class Carryall;

class RefineryClass : public StructureClass
{
public:
	RefineryClass(House* newOwner);
    RefineryClass(Stream& stream);
	void init();
	virtual ~RefineryClass();

	virtual void save(Stream& stream) const;

    virtual ObjectInterface* GetInterfaceContainer();

	void assignHarvester(HarvesterClass* newHarvester);
	void deployHarvester(Carryall* pCarryall = NULL);
	void doSpecificStuff();
	void startAnimate();
	void stopAnimate();

	inline void book() {
	    bookings++;
	    startAnimate();
    }
	inline void unBook() {
        bookings--;
        if(bookings == 0) {
            stopAnimate();
        }
    }
	inline bool isFree() { return !extractingSpice; }
	inline int getNumBookings() { return bookings; }	//number of units goings there
	inline HarvesterClass* getHarvester() { return (HarvesterClass*)harvester.getObjPointer(); }

	virtual void destroy();

private:

	bool            extractingSpice;    ///< Currently extracting spice?
	ObjectPointer   harvester;          ///< The harverster currently in the refinery
    Uint32          bookings;           ///< How many bookings?

    bool    firstRun;       ///< On first deploy of a harvester we tell it to the user
};

#endif // REFINERYCLASS_H
