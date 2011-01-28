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

#ifndef HOUSE_H
#define HOUSE_H

#include <math.h>

#include <misc/Stream.h>
#include <Definitions.h>
#include <DataTypes.h>
#include <data.h>
#include <Choam.h>

#include <players/Player.h>

#include <misc/memory.h>

// forward declarations
class UnitClass;
class StructureClass;
class ObjectClass;
class HumanPlayer;

class House
{
public:
	House(int newHouse, int newCredits, Uint8 team = 0, int quota = 0);
	House(Stream& stream);
	void init();
	virtual ~House();
	virtual void save(Stream& stream) const;

	void addPlayer(std::shared_ptr<Player> newPlayer);

	inline int getHouseID() const { return houseID; }
	inline int getTeam() const { return team; }

    inline bool isAI() const { return ai; }
	inline bool isAlive() const { return (team == 0) || !(((numStructures - numItem[Structure_Wall]) <= 0) && (((numUnits - numItem[Unit_Carryall] - numItem[Unit_Harvester] - numItem[Unit_Frigate] - numItem[Unit_Sandworm]) <= 0))); }

	inline bool hasCarryalls() const { return (numItem[Unit_Carryall] > 0); }
	inline bool hasBarracks() const { return (numItem[Structure_Barracks] > 0); }
	inline bool hasIX() const { return (numItem[Structure_IX] > 0); }
	inline bool hasLightFactory() const { return (numItem[Structure_LightFactory] > 0); }
	inline bool hasHeavyFactory() const { return (numItem[Structure_HeavyFactory] > 0); }
	inline bool hasRefinery() const { return (numItem[Structure_Refinery] > 0); }
	inline bool hasRepairYard() const { return (numItem[Structure_RepairYard] > 0); }
	inline bool hasStarPort() const { return (numItem[Structure_StarPort] > 0); }
	inline bool hasWindTrap() const { return (numItem[Structure_WindTrap] > 0); }
	inline bool hasSandworm() const { return (numItem[Unit_Sandworm] > 0); }
    inline bool hasRadar() const { return (numItem[Structure_Radar] > 0); }

    inline bool hasRadarOn() const { return (hasRadar() && hasPower()); }
	inline bool hasPower() const { return (producedPower >= powerRequirement); }

	inline int getNumStructures() const { return numStructures; };
	inline int getNumUnits() const { return numUnits; };
	inline int getNumItems(int itemID) const { return (isStructure(itemID) || isUnit(itemID)) ? numItem[itemID] : 0; };

	inline int getCapacity() const { return capacity; }

	inline int getProducedPower() const { return producedPower; }
    void setProducedPower(int newPower);
	inline int getPowerRequirement() const { return powerRequirement; }

	inline int getDestroyedValue() const { return destroyedValue; }
	inline int getNumDestroyedUnits() const { return numDestroyedUnits; }
	inline int getNumDestroyedStructures() const { return numDestroyedStructures; }
	inline double getHarvestedSpice() const { return harvestedSpice; }

	inline int getQuota() const { return quota; };

	inline Choam& getChoam() { return choam; };


    inline double getStartingCredits() const { return startingCredits; }
	inline double getStoredCredits() const { return storedCredits; }
    inline int getCredits() const { return lround(storedCredits+startingCredits); }
	void addCredits(double newCredits, bool wasRefined = false);
    void returnCredits(double newCredits);
	double takeCredits(double amount);

	void printStat() const;

	void checkSelectionLists();

    void update();

	void incrementUnits(int itemID);
	void decrementUnits(int itemID);
	void incrementStructures(int itemID);
	void decrementStructures(int itemID, const Coord& location);
    void noteDamageLocation(ObjectClass* pObject, const Coord& location);
	void informHasKilled(Uint32 itemID);

	void lose();
	void win();

	void freeHarvester(int xPos, int yPos);
	StructureClass* placeStructure(Uint32 builderID, int itemID, int xPos, int yPos, bool bForcePlacing = false);
	UnitClass* createUnit(int itemID);
	UnitClass* placeUnit(int itemID, int xPos, int yPos);

	void changeRadar(bool status);

	Coord getCenterOfMainBase() const;

protected:
	void decrementHarvesters();


	std::list<std::shared_ptr<Player> > players;        ///< List of associated players that controll this house

	bool    ai;             ///< Is this an ai player?

	Uint8   houseID;        ///< The house number
	Uint8   team;           ///< The team number

    int numStructures;          ///< How many structures does this player have?
    int numUnits;               ///< How many units does this player have?
    int numItem[ItemID_max];    ///< This array contains the number of structures/units of a certain type this player has

    int capacity;           ///< Total spice capacity
    int producedPower;      ///< Power prodoced by this player
    int powerRequirement;   ///< How much power does this player use?

	double storedCredits;   ///< current number of credits that are stored in refineries/silos
    double startingCredits; ///< number of starting credits this player still has
    int oldCredits;         ///< amount of credits in the last game cycle (used for playing the credits tick sound)

    int quota;              ///< number of credits to win

    Choam   choam;          ///< the things that are deliverable at the starport

    int powerUsageTimer;    ///< every N ticks you have to pay for your power usage

    // statistic
    int destroyedValue;
    int numDestroyedUnits;
    int numDestroyedStructures;
    double harvestedSpice;
};

#endif // HOUSE_H
