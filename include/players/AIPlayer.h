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

#ifndef AIPLAYER_H
#define AIPLAYER_H

#include <players/Player.h>

#include <DataTypes.h>

#include <misc/Stream.h>

class AIPlayer : public Player
{
public:
	AIPlayer(House* associatedHouse, DIFFICULTYTYPE difficulty);
	AIPlayer(Stream& stream, House* associatedHouse);
	void init();
	~AIPlayer();
	virtual void save(Stream& stream) const;

    virtual void update();

    virtual void onIncrementStructures(int itemID);
    virtual void onDecrementStructures(int itemID, const Coord& location);
    virtual void onDamage(ObjectClass* pObject, const Coord& location);

    void setDifficulty(DIFFICULTYTYPE newDifficulty);
	inline DIFFICULTYTYPE getDifficulty() const { return difficulty; }

	void setAttackTimer(int newAttackTimer);
	bool isAttacking() const { return (attackTimer < 0); }
	inline int getAttackTimer() const { return attackTimer; }

private:
	Coord findPlaceLocation(Uint32 itemID);

	DIFFICULTYTYPE	difficulty;     ///< difficulty level
	Sint32  attackTimer;    ///< When to attack?
    Sint32  buildTimer;     ///< When to build the next structure/unit

	std::list<Coord> placeLocations;    ///< Where to place structures
};

#endif //AIPLAYER_H
