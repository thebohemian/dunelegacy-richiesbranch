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

#ifndef PLAYER_H
#define PLAYER_H

#include <DataTypes.h>
#include <misc/Stream.h>

class House;
class ObjectClass;

class Player {
public:
    Player(House* associatedHouse);
    virtual ~Player();
    virtual void save(Stream& stream) const = 0;

    virtual void update() = 0;

    virtual void onIncrementStructures(int itemID) = 0;
    virtual void onDecrementStructures(int itemID, const Coord& location) = 0;
    virtual void onDamage(ObjectClass* pObject, const Coord& location) = 0;

    House* getHouse() const { return pHouse; };

private:
    House* pHouse;
};

#endif // PLAYER_H
