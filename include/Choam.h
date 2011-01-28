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

#ifndef CHOAM_H
#define CHOAM_H

#include <structures/BuilderClass.h>
#include <misc/Stream.h>

#include <vector>

class House;

class Choam {
public:
    Choam(House* pHouse);
    ~Choam();

    void save(Stream& stream) const;
    void load(Stream& stream);

    int getPrice(Uint32 ItemID) const;
    int getNumAvailable(Uint32 ItemID) const;

    bool setNumAvailable(Uint32 ItemID, int newValue);

    void addItem(Uint32 ItemID, int num);

    void update();

private:
    House*                  house;               ///< The house of this choam
	std::vector<BuildItem>  availableItems;      ///< This list contains all the things that can be bought from a Starport
};

#endif // CHOAM_H
