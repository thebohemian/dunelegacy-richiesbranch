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

#ifndef OBJECTDATA_H
#define OBJECTDATA_H

#include <string>

#include <misc/Stream.h>
#include <data.h>

/// This class stores all the unit/structure data
class ObjectData {
public:
    /**
        Default constructur. Initializes everything with zero
    */
    ObjectData();

    /**
        Destructor.
    */
    virtual ~ObjectData();

    /**
        This method loads all data from an INI-File specified by filename. There should be two sections
        in the INI-File for default values: "default structure" and "default unit". The other sections are
        for exactly one unit.
        Example:<br>
            [structure name]<br>
            HitPoints = 500<br>
            Price = 300<br>
            Power = 10<br>
            ViewRange = 3<br>
            Capacity = 1000<br>
            BuildTime = 80<br>
            <br>
            [unit name]
            HitPoints = 100<br>
            Price = 100<br>
            ViewRange = 5<br>
            WeaponDamage = 10<br>
            WeaponRange = 4<br>
            Speed = 3.0<br>
            TurnSpeed = 0.25<br>
            Radius = 8<br>
            BuildTime = 56<br>
        \param filename the INI-File to load.
    */
    void loadFromINIFile(std::string filename);

    /**
        Saves all stored data out into a binary stream.
        \param stream   the stream to save to
        \see load
    */
    void save(Stream& stream) const;

    /**
        Loads all stored data from a stream.
        \param stream   the stream to load from
        \see save
    */
    void load(Stream& stream);

    struct ObjectDataStruct {
        Sint32 hitpoints;    ///< what is the maximum health of this unit/structure?
        Sint32 price;        ///< how much does this structure cost?
        Sint32 power;        ///< how much power do this structure require. Wind traps have negative values because they produce power?
        Sint32 viewrange;    ///< much terrain is revealed when this structure is placed or this unit moves?
        Sint32 capacity;     ///< how much spice can this structure contain?
        Sint32 weapondamage; ///< how much damage does the weapon of this unit/structure have?
        Sint32 weaponrange;  ///< how far can this unit/structure shoot?
        double maxspeed;     ///< how fast can this unit move?
        double turnspeed;    ///< how fast can this unit turn around?
        Sint32 radius;       ///< how big is this unit?
        Sint32 buildtime;    ///< how much time does the production of this structure/unit take?
    };

    ObjectDataStruct data[ItemID_max];      ///< here is all the data stored. It is public for easy and fast access. Use only read-only.
};

#endif // OBJECTDATA_H
