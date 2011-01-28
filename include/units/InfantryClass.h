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

#ifndef INFANTRYCLASS_H
#define INFANTRYCLASS_H

#include <units/GroundUnit.h>

class InfantryClass : public GroundUnit
{

public:
	InfantryClass(House* newOwner);
	InfantryClass(Stream& stream);
	void init();
	virtual ~InfantryClass();

	void save(Stream& stream) const;

    /**
		This method is called when an unit is ordered to capture
		\param	xPos	the x position on the map
		\param	yPos	the y position on the map
	*/
	virtual void HandleCaptureClick(int xPos, int yPos);

    /**
		This method is called when an unit should capture a structure
		\param	TargetStructureID	the ID of the structure to capture
	*/
	virtual void DoCaptureObject(Uint32 TargetStructureID);

	void assignToMap(const Coord& pos);
	void blitToScreen();
	virtual void checkPos();
	void destroy();
	void move();

    inline void setLocation(const Coord& location) { setLocation(location.x, location.y); }
	void setLocation(int xPos, int yPos);

	void squash();

	void playConfirmSound();
	void playSelectSound();

	bool canPass(int xPos, int yPos) const;

	inline int getCellPosition() { return cellPosition; }

protected:
	void setSpeeds();

    // infantry state
	Sint8   cellPosition;       ///< The position in the current cell (0 to 4)
    Sint8   oldCellPosition;    ///< The previous cell position (0 to 4)

    // drawing information
	int     walkFrame;          ///< What frame to draw
};

#endif // INFANTRYCLASS_H
