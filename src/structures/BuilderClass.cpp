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

#include <structures/BuilderClass.h>

#include <FileClasses/TextManager.h>

#include <globals.h>

#include <SoundPlayer.h>
#include <MapClass.h>
#include <House.h>
#include <units/UnitClass.h>

#include <GUI/ObjectInterfaces/BuilderInterface.h>

BuilderClass::BuilderClass(House* newOwner) : StructureClass(newOwner) {
    BuilderClass::init();

	curUpgradeLev = 0;
	upgradeProgress = 0;
	upgrading = false;

	CurrentProducedItem = ItemID_Invalid;
	CurrentItemOnHold = false;
	ProductionProgress = 0;
	deployTimer = 0;
}

BuilderClass::BuilderClass(Stream& stream) : StructureClass(stream) {
    BuilderClass::init();

	upgrading = stream.readBool();
	upgradeProgress = stream.readDouble();
    curUpgradeLev = stream.readUint8();

	CurrentItemOnHold = stream.readBool();
	CurrentProducedItem = stream.readUint32();
	ProductionProgress = stream.readDouble();
	deployTimer = stream.readUint32();

	int numProductionQueueItem = stream.readUint32();
	for(int i=0;i<numProductionQueueItem;i++) {
		ProductionQueueItem tmp;
		tmp.load(stream);
		CurrentProductionList.push_back(tmp);
	}

	int numBuildItem = stream.readUint32();
	for(int i=0;i<numBuildItem;i++) {
		BuildItem tmp;
		tmp.load(stream);
		BuildList.push_back(tmp);
	}
}

void BuilderClass::init() {
    aBuilder = true;
}

BuilderClass::~BuilderClass() {
}


void BuilderClass::save(Stream& stream) const {
	StructureClass::save(stream);

    stream.writeBool(upgrading);
    stream.writeDouble(upgradeProgress);
	stream.writeUint8(curUpgradeLev);

	stream.writeBool(CurrentItemOnHold);
	stream.writeUint32(CurrentProducedItem);
	stream.writeDouble(ProductionProgress);
	stream.writeUint32(deployTimer);

	stream.writeUint32(CurrentProductionList.size());
	std::list<ProductionQueueItem>::const_iterator iter;
	for(iter = CurrentProductionList.begin(); iter != CurrentProductionList.end(); ++iter) {
		iter->save(stream);
	}

	stream.writeUint32(BuildList.size());
	std::list<BuildItem>::const_iterator iter2;
	for(iter2 = BuildList.begin(); iter2 != BuildList.end(); ++iter2) {
		iter2->save(stream);
	}
}

ObjectInterface* BuilderClass::GetInterfaceContainer() {
	if((pLocalHouse == owner) || (debug == true)) {
		return BuilderInterface::Create(objectID);
	} else {
		return DefaultObjectInterface::Create(objectID);
	}
}

void BuilderClass::InsertItem(std::list<BuildItem>& List, std::list<BuildItem>::iterator& iter, Uint32 itemID, int price) {
	if(iter != List.end()) {
		if(iter->ItemID == itemID) {
			if(price != -1) {
				iter->price = price;
			}
			++iter;
			return;
		}
	}

	if(price == -1) {
        price = currentGame->objectData.data[itemID].price;
	}

	BuildItem newItem;

	switch(itemID) {
		case (Structure_Barracks):			newItem = BuildItem(itemID,price); break;
		case (Structure_ConstructionYard):	newItem = BuildItem(itemID,price); break;
		case (Structure_GunTurret):			newItem = BuildItem(itemID,price); break;
		case (Structure_HeavyFactory):		newItem = BuildItem(itemID,price); break;
		case (Structure_HighTechFactory):	newItem = BuildItem(itemID,price); break;
		case (Structure_IX):				newItem = BuildItem(itemID,price); break;
		case (Structure_LightFactory):		newItem = BuildItem(itemID,price); break;
		case (Structure_Palace):			newItem = BuildItem(itemID,price); break;
		case (Structure_Radar):				newItem = BuildItem(itemID,price); break;
		case (Structure_Refinery):			newItem = BuildItem(itemID,price); break;
		case (Structure_RepairYard):		newItem = BuildItem(itemID,price); break;
		case (Structure_RocketTurret):		newItem = BuildItem(itemID,price); break;
		case (Structure_Silo):				newItem = BuildItem(itemID,price); break;
		case (Structure_StarPort):			newItem = BuildItem(itemID,price); break;
		case (Structure_Slab1):				newItem = BuildItem(itemID,price); break;
		case (Structure_Slab4):				newItem = BuildItem(itemID,price); break;
		case (Structure_Wall):				newItem = BuildItem(itemID,price); break;
		case (Structure_WindTrap):			newItem = BuildItem(itemID,price); break;
		case (Structure_WOR):				newItem = BuildItem(itemID,price); break;

		case (Unit_Carryall):				newItem = BuildItem(itemID,price); break;
		case (Unit_Devastator):				newItem = BuildItem(itemID,price); break;
		case (Unit_Deviator):				newItem = BuildItem(itemID,price); break;
		case (Unit_Harvester):				newItem = BuildItem(itemID,price); break;
		case (Unit_Soldier):				newItem = BuildItem(itemID,price); break;
		case (Unit_Launcher):				newItem = BuildItem(itemID,price); break;
		case (Unit_MCV):					newItem = BuildItem(itemID,price); break;
		case (Unit_Ornithopter):			newItem = BuildItem(itemID,price); break;
		case (Unit_Quad):					newItem = BuildItem(itemID,price); break;
		case (Unit_SiegeTank):				newItem = BuildItem(itemID,price); break;
		case (Unit_SonicTank):				newItem = BuildItem(itemID,price); break;
		case (Unit_Tank):					newItem = BuildItem(itemID,price); break;
		case (Unit_Trike):					newItem = BuildItem(itemID,price); break;
		case (Unit_Raider):					newItem = BuildItem(itemID,price); break;
		case (Unit_Trooper):				newItem = BuildItem(itemID,price); break;
		case (Unit_Fremen):					newItem = BuildItem(itemID,price); break;
		case (Unit_Sardaukar):				newItem = BuildItem(itemID,price); break;

		default: {
		    throw std::invalid_argument("BuilderClass::InsertItem(): Cannot insert item with ItemID + " + stringify(itemID) + "!");
        } break;
	}

	List.insert(iter,newItem);
}

void BuilderClass::RemoveItem(std::list<BuildItem>& List, std::list<BuildItem>::iterator& iter, Uint32 itemID) {
	if(iter != List.end()) {
		if(iter->ItemID == itemID) {
			std::list<BuildItem>::iterator iter2 = iter;
			++iter;
			List.erase(iter2);

            // is this item currently produced?
            if(CurrentProducedItem == itemID) {
                owner->returnCredits(ProductionProgress);
                ProductionProgress = 0.0;
                CurrentProducedItem = ItemID_Invalid;
            }

            // remove from production list
            std::list<ProductionQueueItem>::iterator iter3 = CurrentProductionList.begin();
            while(iter3 != CurrentProductionList.end()) {
                if(iter3->ItemID == itemID) {
                    std::list<ProductionQueueItem>::iterator iter4 = iter3;
                    ++iter3;
                    CurrentProductionList.erase(iter4);
                } else {
                    ++iter3;
                }
            }

            ProduceNextAvailableItem();
		}
	}
}


void BuilderClass::setOwner(House *no) {
	this->owner = no;
}

bool BuilderClass::IsWaitingToPlace() const {
	if((CurrentProducedItem == ItemID_Invalid) || isUnit(CurrentProducedItem)) {
		return false;
	}

	const BuildItem* tmp = GetBuildItem(CurrentProducedItem);
	if(tmp == NULL) {
		return false;
	} else {
		return (ProductionProgress >= tmp->price);
	}
}


void BuilderClass::buildUpdate() {
    if(CurrentProducedItem != ItemID_Invalid) {
        BuildItem* tmp = GetBuildItem(CurrentProducedItem);

        if((ProductionProgress < tmp->price) && (IsOnHold() == false) && (owner->getCredits() > 0)) {

            double buildSpeed = getHealth() / (double) getMaxHealth();
            double totalBuildCosts = currentGame->objectData.data[CurrentProducedItem].price;
            double totalBuildGameTicks = currentGame->objectData.data[CurrentProducedItem].buildtime*15.0;
            double buildCosts = totalBuildCosts / totalBuildGameTicks;

            double oldProgress = ProductionProgress;

            ProductionProgress += owner->takeCredits(buildCosts*buildSpeed);

            /* That was wrong. Build speed does not depend on power production
            if (getOwner()->hasPower() || (((currentGame->gameType == GAMETYPE_CAMPAIGN) || (currentGame->gameType == GAMETYPE_SKIRMISH)) && getOwner()->isAI())) {
                //if not enough power, production is halved
                ProductionProgress += owner->takeCredits(0.25);
            } else {
                ProductionProgress += owner->takeCredits(0.125);
            }*/

            if ((oldProgress == ProductionProgress) && (owner == pLocalHouse)) {
                currentGame->AddToNewsTicker(pTextManager->getLocalized("Not enough money"));
            }

            if(ProductionProgress >= tmp->price) {
                setWaitingToPlace();
            }
        }
	}
}

void BuilderClass::buildRandom() {
	int randNum = currentGame->RandomGen.rand(0, getNumSelections()-1);
	int i = 0;
	std::list<BuildItem>::iterator iter;
	for(iter = BuildList.begin(); iter != BuildList.end(); ++iter) {
		if(i == randNum) {
			DoProduceItem(iter->ItemID);
			break;
		}
	}
}

void BuilderClass::ProduceNextAvailableItem() {
	if(CurrentProductionList.empty() == true) {
		CurrentProducedItem = ItemID_Invalid;
	} else {
		CurrentProducedItem = CurrentProductionList.front().ItemID;
	}

	ProductionProgress = 0.0;
	CurrentItemOnHold = false;
}


void BuilderClass::setWaitingToPlace() {
	if (CurrentProducedItem != ItemID_Invalid)	{
		if (owner == pLocalHouse) {
			soundPlayer->playVoice(ConstructionComplete,getOwner()->getHouseID());
		}

		if (isUnit(CurrentProducedItem)) {
			//if its a unit
            deployTimer = MILLI2CYCLES(750);
		} else {
			//its a structure
			if (owner == pLocalHouse) {
				currentGame->AddToNewsTicker(pTextManager->getDuneText(DuneText_ConstructionComplete));
			}
		}
	}
}

void BuilderClass::unSetWaitingToPlace() {
	ProductionProgress = 0.0;
	DoCancelItem(CurrentProducedItem);
}

int BuilderClass::GetUpgradeCost() const {
    return currentGame->objectData.data[itemID].price / 2.0;
}

bool BuilderClass::update() {
	if(StructureClass::update() == false) {
        return false;
	}

	if(isUnit(CurrentProducedItem) && (ProductionProgress >= GetBuildItem(CurrentProducedItem)->price)) {
        deployTimer--;
        if(deployTimer == 0) {
            int FinishedItemID = CurrentProducedItem;
			ProductionProgress = 0.0;
			DoCancelItem(CurrentProducedItem);

			UnitClass* newUnit = getOwner()->createUnit(FinishedItemID);

			if (newUnit != NULL) {
				Coord spot = currentGameMap->findDeploySpot(newUnit, location, destination, structureSize);
				newUnit->deploy(spot);

				if (getOwner()->isAI()
					&& (newUnit->getItemID() != Unit_Carryall)
					&& (newUnit->getItemID() != Unit_Harvester)
					&& (newUnit->getItemID() != Unit_MCV)) {
					newUnit->DoSetAttackMode(AREAGUARD);
				}

				if (destination.x != INVALID_POS) {
					newUnit->setGuardPoint(destination);
					newUnit->setDestination(destination);
					newUnit->setAngle(lround(8.0/256.0*dest_angle(newUnit->getLocation(), newUnit->getDestination())));
				}
			}
        }
	}

	if(upgrading == true) {
	    double totalUpgradePrice = GetUpgradeCost();
	    double totalUpgradeGameTicks = 30.0 * 100.0 / 5.0;
		upgradeProgress += owner->takeCredits(totalUpgradePrice / totalUpgradeGameTicks);

		if(upgradeProgress >= totalUpgradePrice) {
			upgrading = false;
			curUpgradeLev++;
			this->checkSelectionList();

			upgradeProgress = 0;
		}
	} else {
		buildUpdate();
	}

	return true;
}

void BuilderClass::HandleUpgradeClick() {
	currentGame->GetCommandManager().addCommand(Command(CMD_BUILDER_UPGRADE, objectID));
}

void BuilderClass::HandleProduceItemClick(Uint32 ItemID, bool multipleMode) {
	currentGame->GetCommandManager().addCommand(Command(CMD_BUILDER_PRODUCEITEM, objectID, ItemID, (Uint32) multipleMode));
}

void BuilderClass::HandleCancelItemClick(Uint32 ItemID, bool multipleMode) {
	currentGame->GetCommandManager().addCommand(Command(CMD_BUILDER_CANCELITEM, objectID, ItemID, (Uint32) multipleMode));
}

void BuilderClass::HandleSetOnHoldClick(bool OnHold) {
	currentGame->GetCommandManager().addCommand(Command(CMD_BUILDER_SETONHOLD, objectID, (Uint32) OnHold));
}


void BuilderClass::DoUpgrade() {
	if(owner->getCredits() >= GetUpgradeCost()) {
		upgrading = true;
		upgradeProgress = 0.0;
	}
}

void BuilderClass::DoProduceItem(Uint32 ItemID, bool multipleMode) {
	std::list<BuildItem>::iterator iter;
	for(iter = BuildList.begin(); iter != BuildList.end(); ++iter) {
		if(iter->ItemID == ItemID) {
			for(int i = 0; i < (multipleMode ? 5 : 1); i++) {
			    if(ItemID == Structure_Palace && (iter->num > 0 || owner->getNumItems(Structure_Palace) > 0)) {
			        // only one palace allowed
                    return;
			    }

				iter->num++;
				CurrentProductionList.push_back( ProductionQueueItem(ItemID, iter->price) );
				if(CurrentProducedItem == ItemID_Invalid) {
					ProductionProgress = 0;
					CurrentProducedItem = ItemID;
				}
			}
			break;
		}
	}
}

void BuilderClass::DoCancelItem(Uint32 ItemID, bool multipleMode) {
	std::list<BuildItem>::iterator iter;
	for(iter = BuildList.begin(); iter != BuildList.end(); ++iter) {
		if(iter->ItemID == ItemID) {
			for(int i = 0; i < (multipleMode ? 5 : 1); i++) {
				if(iter->num > 0) {
					iter->num--;

                    bool cancelCurrentItem = (ItemID == CurrentProducedItem);
					std::list<ProductionQueueItem>::reverse_iterator iter2;
					for(iter2 = CurrentProductionList.rbegin(); iter2 != CurrentProductionList.rend(); ++iter2) {
						if(iter2->ItemID == ItemID) {
						    if(iter->num == 0 && cancelCurrentItem == true) {
                                owner->returnCredits(ProductionProgress);
						    } else {
                                cancelCurrentItem = false;
						    }
                            CurrentProductionList.erase((++iter2).base());

							break;
						}
					}

					if(cancelCurrentItem == true) {
					    deployTimer = 0;
						ProduceNextAvailableItem();
					}
				}
			}
			break;
		}
	}
}


