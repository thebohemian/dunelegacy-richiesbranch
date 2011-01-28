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

#ifndef PALACECLASS_H
#define PALACECLASS_H

#include <structures/StructureClass.h>

class PalaceClass : public StructureClass
{
public:
	PalaceClass(House* newOwner);
    PalaceClass(Stream& stream);
	void init();
	virtual ~PalaceClass();

	virtual void save(Stream& stream) const;

	virtual ObjectInterface* GetInterfaceContainer();

	void HandleSpecialClick();
	void HandleDeathhandClick(int xPos, int yPos);
	void DoSpecialWeapon();
    void DoLaunchDeathhand(int xpos, int ypos);
	static void netDoSpecial(void* voidPalace);
	void doSpecificStuff();

    /**
        Can this structure be captured by infantry units?
        \return true, if this structure can be captured, false otherwise
	*/
	virtual bool canBeCaptured() const { return false; };

	void setSpecialTimer(int time);
	bool callFremen();
	bool spawnSaboteur();
	double getPercentComplete();

	inline bool specialReady() { return (specialTimer == 0); }
	inline int getSpecialTimer() { return specialTimer; }
	inline int getMaxSpecialTimer() {
	    if(origHouse == HOUSE_HARKONNEN || origHouse == HOUSE_SARDAUKAR) {
	        // 10 min
            return MILLI2CYCLES(10*60*1000);
	    } else {
	        // 5 min
            return MILLI2CYCLES(5*60*1000);
	    }
	}

private:
	Sint32  specialTimer;       ///< When is the special weapon ready?
};

#endif // PALACECLASS_H
