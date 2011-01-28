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

#ifndef BUILDERCLASS_H
#define BUILDERCLASS_H

#include <structures/StructureClass.h>

#include <data.h>

#include <list>
#include <string>

class BuildItem {
public:
	BuildItem() {
		ItemID = ItemID_Invalid;
		price = 0;
		num = 0;
	}

	BuildItem(int ItemID, int price) {
		this->ItemID = ItemID;
		this->price = price;
		num = 0;
	}

	void save(Stream& stream) const {
		stream.writeUint32(ItemID);
		stream.writeUint32(price);
		stream.writeUint32(num);
	}

	void load(Stream& stream) {
		ItemID = stream.readUint32();
		price = stream.readUint32();
		num = stream.readUint32();
	}

	Uint32 ItemID;
	Uint32 price;
	Uint32 num;
};

class ProductionQueueItem {
public:
    ProductionQueueItem()
     : ItemID(0), price(0) {

    }

    ProductionQueueItem(Uint32 _ItemID, Uint32 _price)
     : ItemID(_ItemID), price(_price) {

    }

	void save(Stream& stream) const {
		stream.writeUint32(ItemID);
		stream.writeUint32(price);
	}

	void load(Stream& stream) {
		ItemID = stream.readUint32();
		price = stream.readUint32();
	}

    Uint32 ItemID;
    Uint32 price;
};


class BuilderClass : public StructureClass
{
public:
	BuilderClass(House* newOwner);
    BuilderClass(Stream& stream);
	void init();
	virtual ~BuilderClass();

	virtual void save(Stream& stream) const;

	virtual ObjectInterface* GetInterfaceContainer();

	void setOwner(House *no);

	virtual void setOriginalHouse(int i) { StructureClass::setOriginalHouse(i); checkSelectionList(); }

	void checkMinMaxSelection();
	virtual void checkSelectionList() { ; };	//goes and sees what should be in its list

	void setWaitingToPlace();
	void unSetWaitingToPlace();

	int getNumSelections() { return BuildList.size();} ;

	int getNumItemsToBuild() { return CurrentProductionList.size(); };

	virtual int MaxUpgradeLevel() const { return 0; };

	/**
        Updates this builder.
        \return true if this object still exists, false if it was destroyed
	*/
	virtual bool update();

	void HandleUpgradeClick();
	void HandleProduceItemClick(Uint32 ItemID, bool multipleMode = false);
	void HandleCancelItemClick(Uint32 ItemID, bool multipleMode = false);
	void HandleSetOnHoldClick(bool OnHold);

	virtual void DoUpgrade();
	virtual void DoProduceItem(Uint32 ItemID, bool multipleMode = false);
	virtual void DoCancelItem(Uint32 ItemID, bool multipleMode = false);
	inline void DoSetOnHold(bool OnHold) { CurrentItemOnHold = OnHold; };


	inline bool IsUpgrading() const { return upgrading; };
	inline bool AllowedToUpgrade() const { return (MaxUpgradeLevel() > 0); };
	inline int CurrentUpgradeLevel() const { return curUpgradeLev; };
	int GetUpgradeCost() const;
	inline double GetUpgradeProgress() const { return upgradeProgress; };

	inline Uint32 GetCurrentProducedItem() const { return CurrentProducedItem; };
	inline bool IsOnHold() { return CurrentItemOnHold; };
	bool IsWaitingToPlace() const;
	inline double GetProductionProgress() const { return ProductionProgress; };
	inline const std::list<BuildItem>& GetBuildList() const { return BuildList; };
	virtual void InsertItem(std::list<BuildItem>& List, std::list<BuildItem>::iterator& iter, Uint32 itemID, int price=-1);
	void RemoveItem(std::list<BuildItem>& List, std::list<BuildItem>::iterator& iter, Uint32 itemID);

	virtual void buildRandom();

	virtual void buildUpdate();

	virtual inline bool isAvailableToBuild(Uint32 ItemID) const {
		return (GetBuildItem(ItemID) != NULL);
	}

protected:
	BuildItem* GetBuildItem(Uint32 ItemID) {
		std::list<BuildItem>::iterator iter;
		for(iter = BuildList.begin(); iter != BuildList.end(); ++iter) {
			if(iter->ItemID == ItemID) {
				return &(*iter);
			}
		}
		return NULL;
	}

    const BuildItem* GetBuildItem(Uint32 ItemID) const {
		std::list<BuildItem>::const_iterator iter;
		for(iter = BuildList.begin(); iter != BuildList.end(); ++iter) {
			if(iter->ItemID == ItemID) {
				return &(*iter);
			}
		}
		return NULL;
	}

	void ProduceNextAvailableItem();

    // structure state
    bool    upgrading;              ///< Currently upgrading?
	double  upgradeProgress;        ///< The current state of the upgrade progress (measured in money spent)
	Uint8   curUpgradeLev;          ///< Current upgrade level

	bool    CurrentItemOnHold;      ///< Is the currently produced item on hold?
	Uint32  CurrentProducedItem;    ///< The ItemID of the currently produced item
	double  ProductionProgress;     ///< The current state of the production progress (measured in money spent)
	Uint32  deployTimer;            ///< Timer for deploying a unit

	std::list<ProductionQueueItem>  CurrentProductionList;      ///< This list is the production queue (It contains the item IDs of the units/structures to produce)
	std::list<BuildItem>            BuildList;                  ///< This list contains all the things that can be produced by this builder
};

#endif //BUILDERCLASS_H
