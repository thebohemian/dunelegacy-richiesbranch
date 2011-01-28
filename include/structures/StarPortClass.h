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

#ifndef STARPORTCLASS_H
#define STARPORTCLASS_H

#include <structures/BuilderClass.h>

class StarPortClass : public BuilderClass
{
public:
	StarPortClass(House* newOwner);
    StarPortClass(Stream& stream);
	void init();
	virtual ~StarPortClass();

	virtual void save(Stream& stream) const;

	virtual void HandlePlaceOrderClick();
	virtual void HandleCancelOrderClick();
	virtual void DoProduceItem(Uint32 ItemID, bool multipleMode = false);
	virtual void DoCancelItem(Uint32 ItemID, bool multipleMode = false);
	void DoPlaceOrder();
	void DoCancelOrder();

	virtual void buildRandom();

	void checkSelectionList();
	void doSpecificStuff();
	void setArrivalTimer(int newArrivalTimer);
	void startDeploying() {
	    deploying = true;
        FirstAnimFrame = 8;
		LastAnimFrame = 9;
    };

    void informFrigateDestroyed();

	inline bool okToOrder() const { return (arrivalTimer < 0); }
	inline int getArrivalTimer() const { return arrivalTimer; }

private:
	Sint32  arrivalTimer;       ///< When will the frigate arrive?
	bool    deploying;          ///< Currently deploying units
};

#endif // STARPORTCLASS_H
