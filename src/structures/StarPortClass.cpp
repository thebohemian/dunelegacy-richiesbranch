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

#include <structures/StarPortClass.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <FileClasses/TextManager.h>
#include <House.h>
#include <Game.h>
#include <Choam.h>
#include <MapClass.h>
#include <SoundPlayer.h>

#include <units/Frigate.h>

// Starport is counting in 30s from 10 to 0
#define STARPORT_ARRIVETIME			(MILLI2CYCLES(30*1000))

#define STARPORT_NO_ARRIVAL_AWAITED -1



StarPortClass::StarPortClass(House* newOwner) : BuilderClass(newOwner)
{
    StarPortClass::init();

    setHealth(getMaxHealth());

    arrivalTimer = STARPORT_NO_ARRIVAL_AWAITED;
    deploying = false;
}

StarPortClass::StarPortClass(Stream& stream) : BuilderClass(stream)
{
    StarPortClass::init();

    arrivalTimer = stream.readSint32();
    if(stream.readBool() == true) {
        startDeploying();
    } else {
        deploying = false;
    }
}
void StarPortClass::init()
{
	itemID = Structure_StarPort;
	owner->incrementStructures(itemID);

	structureSize.x = 3;
	structureSize.y = 3;

	GraphicID = ObjPic_Starport;
	graphic = pGFXManager->getObjPic(GraphicID,getOwner()->getHouseID());
	imageW = graphic->w / 10;
	imageH = graphic->h;
	FirstAnimFrame = 2;
	LastAnimFrame = 3;
}

StarPortClass::~StarPortClass()
{
	;
}

void StarPortClass::save(Stream& stream) const
{
	BuilderClass::save(stream);
	stream.writeSint32(arrivalTimer);
	stream.writeBool(deploying);
}

void StarPortClass::buildRandom()
{
	int Item2Produce = ItemID_Invalid;

	do {
		int randNum = currentGame->RandomGen.rand(0, getNumSelections()-1);
		int i = 0;
		std::list<BuildItem>::iterator iter;
		for(iter = BuildList.begin(); iter != BuildList.end(); ++iter,i++) {
			if(i == randNum) {
				Item2Produce = iter->ItemID;
				break;
			}
		}
	} while((Item2Produce == Unit_Harvester) || (Item2Produce == Unit_MCV) || (Item2Produce == Unit_Carryall));

	DoProduceItem(Item2Produce);
}

void StarPortClass::HandlePlaceOrderClick() {
	currentGame->GetCommandManager().addCommand(Command(CMD_STARPORT_PLACEORDER, objectID));
}

void StarPortClass::HandleCancelOrderClick() {
	currentGame->GetCommandManager().addCommand(Command(CMD_STARPORT_CANCELORDER, objectID));
}

void StarPortClass::DoProduceItem(Uint32 ItemID, bool multipleMode) {
    Choam& choam = owner->getChoam();

	std::list<BuildItem>::iterator iter;
	for(iter = BuildList.begin(); iter != BuildList.end(); ++iter) {
		if(iter->ItemID == ItemID) {
			for(int i = 0; i < (multipleMode ? 5 : 1); i++) {
			    int numAvailable = choam.getNumAvailable(ItemID);

			    if(numAvailable <= 0) {
                    if(getOwner() == pLocalHouse) {
                        soundPlayer->playSound(InvalidAction);
                        currentGame->AddToNewsTicker(pTextManager->getLocalized("This unit is sold out"));
                    }
                    break;
			    }

				if((owner->getCredits() >= (int) iter->price)) {
                    iter->num++;
					CurrentProductionList.push_back( ProductionQueueItem(ItemID,iter->price) );
					owner->takeCredits(iter->price);

				    if(choam.setNumAvailable(ItemID, numAvailable - 1) == false) {
                        // sold out
                        if(getOwner() == pLocalHouse) {
                            currentGame->AddToNewsTicker(pTextManager->getLocalized("This unit is sold out"));
                        }
                        break;
					}
				} else {
                    if(getOwner() == pLocalHouse) {
                        soundPlayer->playSound(InvalidAction);
                        currentGame->AddToNewsTicker(pTextManager->getLocalized("Not enough money"));
                    }
				}
			}
			break;
		}
	}
}

void StarPortClass::DoCancelItem(Uint32 ItemID, bool multipleMode) {
    Choam& choam = owner->getChoam();

	std::list<BuildItem>::iterator iter;
	for(iter = BuildList.begin(); iter != BuildList.end(); ++iter) {
		if(iter->ItemID == ItemID) {
			for(int i = 0; i < (multipleMode ? 5 : 1); i++) {
				if(iter->num > 0) {
					iter->num--;
					choam.setNumAvailable(iter->ItemID, choam.getNumAvailable(iter->ItemID) + 1);

                    // find the most expensive item to cancel
                    std::list<ProductionQueueItem>::iterator iterMostExpensiveItem = CurrentProductionList.end();
					std::list<ProductionQueueItem>::iterator iter2;
					for(iter2 = CurrentProductionList.begin(); iter2 != CurrentProductionList.end(); ++iter2) {
						if(iter2->ItemID == ItemID) {

						    // have we found a better item to cancel?
						    if(iterMostExpensiveItem == CurrentProductionList.end() || iter2->price > iterMostExpensiveItem->price) {
                                iterMostExpensiveItem = iter2;
						    }
						}
					}

					// Cancel the best found item if any was found
					if(iterMostExpensiveItem != CurrentProductionList.end()) {
                        owner->returnCredits(iterMostExpensiveItem->price);
                        CurrentProductionList.erase(iterMostExpensiveItem);
					}
				}
			}
			break;
		}
	}
}

void StarPortClass::DoPlaceOrder() {

	if (CurrentProductionList.size() > 0) {

		arrivalTimer = STARPORT_ARRIVETIME;

		FirstAnimFrame = 2;
		LastAnimFrame = 7;
	}
}

void StarPortClass::DoCancelOrder() {
    if (arrivalTimer == STARPORT_NO_ARRIVAL_AWAITED) {
        while(CurrentProductionList.empty() == false) {
            DoCancelItem(CurrentProductionList.back().ItemID, false);
        }

		CurrentProducedItem = ItemID_Invalid;
	}
}


void StarPortClass::checkSelectionList()
{
	BuilderClass::checkSelectionList();

	std::list<BuildItem>::iterator iter = BuildList.begin();

    Choam& choam = owner->getChoam();

    // Launcher
    if(choam.getNumAvailable(Unit_Launcher) != INVALID) {
        InsertItem(BuildList, iter, Unit_Launcher, choam.getPrice(Unit_Launcher));
    } else {
        RemoveItem(BuildList, iter, Unit_Launcher);
    }

    // Siege Tank
    if(choam.getNumAvailable(Unit_SiegeTank) != INVALID) {
        InsertItem(BuildList, iter, Unit_SiegeTank, choam.getPrice(Unit_SiegeTank));
    } else {
        RemoveItem(BuildList, iter, Unit_SiegeTank);
    }

    // Tank
    if(choam.getNumAvailable(Unit_Tank) != INVALID) {
        InsertItem(BuildList, iter, Unit_Tank, choam.getPrice(Unit_Tank));
    } else {
        RemoveItem(BuildList, iter, Unit_Tank);
    }

    // MCV
    if(choam.getNumAvailable(Unit_MCV) != INVALID) {
        InsertItem(BuildList, iter, Unit_MCV, choam.getPrice(Unit_MCV));
    } else {
        RemoveItem(BuildList, iter, Unit_MCV);
    }

    // Harvester
    if(choam.getNumAvailable(Unit_Harvester) != INVALID) {
        InsertItem(BuildList, iter, Unit_Harvester, choam.getPrice(Unit_Harvester));
    } else {
        RemoveItem(BuildList, iter, Unit_Harvester);
    }

    // Ornithopter
    if(choam.getNumAvailable(Unit_Ornithopter) != INVALID) {
        InsertItem(BuildList, iter, Unit_Ornithopter, choam.getPrice(Unit_Ornithopter));
    } else {
        RemoveItem(BuildList, iter, Unit_Ornithopter);
    }

    // Carryall
    if(choam.getNumAvailable(Unit_Carryall) != INVALID) {
        InsertItem(BuildList, iter, Unit_Carryall, choam.getPrice(Unit_Carryall));
    } else {
        RemoveItem(BuildList, iter, Unit_Carryall);
    }

    // Quad
    if(choam.getNumAvailable(Unit_Quad) != INVALID) {
        InsertItem(BuildList, iter, Unit_Quad, choam.getPrice(Unit_Quad));
    } else {
        RemoveItem(BuildList, iter, Unit_Quad);
    }

    // Trike
    if(choam.getNumAvailable(Unit_Trike) != INVALID) {
        InsertItem(BuildList, iter, Unit_Trike, choam.getPrice(Unit_Trike));
    } else {
        RemoveItem(BuildList, iter, Unit_Trike);
    }
}

void StarPortClass::doSpecificStuff()
{
    checkSelectionList();

	if (arrivalTimer > 0) {
		if (--arrivalTimer == 0) {

			Frigate*		frigate;
			Coord		pos;

			//make a frigate with all the cargo
			frigate = (Frigate*)owner->createUnit(Unit_Frigate);
			pos = currentGameMap->findClosestEdgePoint(getLocation() + Coord(1,1), Coord(1,1));
			frigate->deploy(pos);
			frigate->setTarget(this);
			Coord closestPoint = getClosestPoint(frigate->getLocation());
			frigate->setDestination(closestPoint);

			if (pos.x == 0)
				frigate->setAngle(RIGHT);
			else if (pos.x == currentGameMap->sizeX-1)
				frigate->setAngle(LEFT);
			else if (pos.y == 0)
				frigate->setAngle(DOWN);
			else if (pos.y == currentGameMap->sizeY-1)
				frigate->setAngle(UP);

            deployTimer = MILLI2CYCLES(750);

			CurrentProducedItem = ItemID_Invalid;

			if(getOwner() == pLocalHouse) {
				soundPlayer->playVoice(FrigateHasArrived,getOwner()->getHouseID());
				currentGame->AddToNewsTicker(pTextManager->getDuneText(DuneText_FrigateArrived));
			}

		}
	} else if(deploying == true) {
        deployTimer--;
        if(deployTimer == 0) {

            if(CurrentProductionList.empty() == false) {
                Uint32 newUnitItemID = CurrentProductionList.front().ItemID;
                UnitClass* newUnit = getOwner()->createUnit(newUnitItemID);
                if (newUnit != NULL) {
                    Coord spot = newUnit->isAFlyingUnit() ? location + Coord(1,1) : currentGameMap->findDeploySpot(newUnit, location, destination, structureSize);
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

                std::list<BuildItem>::iterator iter2;
                for(iter2 = BuildList.begin(); iter2 != BuildList.end(); ++iter2) {
                    if(iter2->ItemID == newUnitItemID) {
                        iter2->num--;
                        break;
                    }
                }

                CurrentProductionList.pop_front();

                if(CurrentProductionList.empty() == true) {
                    arrivalTimer = STARPORT_NO_ARRIVAL_AWAITED;
                    deploying = false;
                    // Remove box from starport
                    FirstAnimFrame = 2;
                    LastAnimFrame = 3;
                } else {
                    deployTimer = MILLI2CYCLES(750);
                }
            }
        }
	}



}

void StarPortClass::setArrivalTimer(int newArrivalTimer) {
	if (newArrivalTimer >= 0) {
		arrivalTimer = newArrivalTimer;
	}
}

void StarPortClass::informFrigateDestroyed() {
    CurrentProductionList.clear();
    arrivalTimer = STARPORT_NO_ARRIVAL_AWAITED;
    deployTimer = 0;
    deploying = false;
    // stop blinking
    FirstAnimFrame = 2;
    LastAnimFrame = 3;
}
