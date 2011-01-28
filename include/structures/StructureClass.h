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

#ifndef STRUCTURECLASS_H
#define STRUCTURECLASS_H

#define ANIMATIONTIMER 25
#define ANIMMOVETIMER 50

#include <ObjectClass.h>

#include <list>

struct StructureSmoke {
    StructureSmoke(const Coord& pos, Uint32 gameCycle)
     : realPos(pos), startGameCycle(gameCycle) {
    };

    StructureSmoke(Stream& stream) {
        realPos.x = stream.readSint32();
        realPos.y = stream.readSint32();
        startGameCycle = stream.readUint32();
    };

    void save(Stream& stream) const {
        stream.writeSint32(realPos.x);
        stream.writeSint32(realPos.y);
        stream.writeUint32(startGameCycle);
    };

    Coord   realPos;
    Uint32  startGameCycle;
};

class StructureClass : public ObjectClass
{
public:
	StructureClass(House* newOwner);
	StructureClass(Stream& stream);
	void init();
	virtual ~StructureClass();

	virtual void save(Stream& stream) const;

	void assignToMap(const Coord& pos);
	void blitToScreen();

	virtual ObjectInterface* GetInterfaceContainer();

	void destroy();
	virtual void drawSelectionBox();

	virtual Coord getCenterPoint() const;
	virtual Coord getClosestCenterPoint(const Coord& objectLocation) const;
	void setDestination(int newX, int newY);
	void setJustPlaced();
	void setFogged(bool bFogged){fogged = bFogged;};

	int getDrawnX() const;
	int getDrawnY() const;

	void playConfirmSound() { ; };
	void playSelectSound() { ; };

	/**
		This method is called when a structure is ordered by a right click
		\param	xPos	the x position on the map
		\param	yPos	the y position on the map
	*/
	virtual void HandleActionClick(int xPos, int yPos);

	/**
		This method is called when the user clicks on the repair button for this building
	*/
	virtual void HandleRepairClick();


	virtual void DoSetDeployPosition(int xPos, int yPos);

	void DoRepair();

	/**
        Updates this object.
        \return true if this object still exists, false if it was destroyed
	*/
	virtual bool update();

	virtual void doSpecificStuff() { };

	/**
        Can this structure be captured by infantry units?
        \return true, if this structure can be captured, false otherwise
	*/
	virtual bool canBeCaptured() const { return true; };

	bool IsRepairing() { return repairing; }

	virtual Coord getClosestPoint(const Coord& objectLocation) const;

	inline short getStructureSizeX() const { return structureSize.x; }
	inline short getStructureSizeY() const { return structureSize.y; }
	inline const Coord& getStructureSize() const { return structureSize; }
	inline int getOriginalHouse() const { return origHouse; }
	virtual void setOriginalHouse(int i) { origHouse = i; }

	inline void addSmoke(const Coord& pos, Uint32 gameCycle) {
	    std::list<StructureSmoke>::iterator iter;
	    for(iter = smoke.begin(); iter != smoke.end(); ++iter) {
            if(iter->realPos == pos) {
                iter->startGameCycle = gameCycle;
                return;
            } else if(iter->realPos.y > pos.y) {
                smoke.insert(iter, StructureSmoke(pos, gameCycle));
            }
        }

        smoke.push_back(StructureSmoke(pos, gameCycle));
    };
	inline size_t getNumSmoke() const { return smoke.size(); };

protected:
	// constant for all structures of the same type
    Coord	structureSize;      ///< The size of this structure in tile coordinates (e.g. (3,2) for a refinery)

    // structure state
	bool    repairing;          ///< currently repairing?
    int		origHouse;          ///< for takeover, we still want to keep track of what the original owner was
    int     degradeTimer;       ///< after which time of insufficient power should we degrade this building again

    // TODO: fogging is currently broken
	bool        fogged;             ///< Currently fogged?
	int         lastVisibleFrame;   ///< store picture drawn before fogged

    // drawing information
    int		JustPlacedTimer;          ///< When the structure is justed placed, we draw some special graphic
    std::list<StructureSmoke> smoke;  ///< A vector containing all the smoke for this structure

	int		FirstAnimFrame;     ///< First frame of the current animation
	int		LastAnimFrame;      ///< Last frame of the current animation
	int		curAnimFrame;       ///< The current frame of the current animation
	int     animationCounter;   ///< When to show the next animation frame?
};

#endif //STRUCTURECLASS_H
