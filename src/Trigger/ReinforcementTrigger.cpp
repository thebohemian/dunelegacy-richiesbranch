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

#include <Trigger/ReinforcementTrigger.h>

#include <globals.h>
#include <Game.h>
#include <MapClass.h>
#include <House.h>

#include <units/UnitClass.h>
#include <units/Carryall.h>

#include <misc/memory.h>
#include <stdio.h>

ReinforcementTrigger::ReinforcementTrigger(int houseID, Uint32 itemID, DropLocation location, bool bRepeat, Uint32 triggerCycleNumber) : Trigger(triggerCycleNumber)
{
    this->houseID = houseID;
    dropLocation = location;
    repeatCycle = (bRepeat == true) ? triggerCycleNumber : 0;
    droppedUnits.push_back(itemID);
}

ReinforcementTrigger::ReinforcementTrigger(Stream& stream) : Trigger(stream)
{
    droppedUnits = stream.readUint32Vector();
    dropLocation = (DropLocation) stream.readUint32();
    houseID = stream.readSint32();
    repeatCycle = stream.readUint32();
}

ReinforcementTrigger::~ReinforcementTrigger()
{
}

void ReinforcementTrigger::save(Stream& stream)
{
    Trigger::save(stream);

    stream.writeUint32Vector(droppedUnits);
    stream.writeUint32(dropLocation);
    stream.writeSint32(houseID);
    stream.writeUint32(repeatCycle);
}

void ReinforcementTrigger::trigger()
{
    House* dropHouse = currentGame->house[houseID];

    if(dropHouse == NULL) {
        return;
    }

    switch(dropLocation) {
        case Drop_North:
        case Drop_East:
        case Drop_South:
        case Drop_West: {

            Coord placeCoord(INVALID_POS, INVALID_POS);

            switch(dropLocation) {

                case Drop_North: {
                    placeCoord = Coord(currentGame->RandomGen.rand(0,currentGameMap->sizeX-1), 0);
                } break;

                case Drop_East: {
                    placeCoord = Coord(currentGameMap->sizeX-1, currentGame->RandomGen.rand(0,currentGameMap->sizeY-1));
                } break;

                case Drop_South: {
                    placeCoord = Coord(currentGame->RandomGen.rand(0,currentGameMap->sizeX-1), currentGameMap->sizeY-1);
                } break;

                case Drop_West: {
                    placeCoord = Coord(0, currentGame->RandomGen.rand(0,currentGameMap->sizeY-1));
                } break;

                default: {
                } break;
            }


            if(placeCoord == Coord(INVALID_POS, INVALID_POS)) {
                break;
            }

            std::vector<Uint32> units2Drop = droppedUnits;

            // try 30 times
            int r = 1;
            while(units2Drop.empty() == false && ++r < 32) {

                Coord newCoord = placeCoord;
                if(dropLocation == Drop_North || dropLocation == Drop_South) {
                    newCoord += Coord(currentGame->RandomGen.rand(-r,r), 0);
                } else {
                    newCoord += Coord(0, currentGame->RandomGen.rand(-r,r));
                }

                if(currentGameMap->cellExists(newCoord) && currentGameMap->getCell(newCoord)->hasAGroundObject() == false) {
                    UnitClass* pUnit2Drop = dropHouse->createUnit(units2Drop.front());
                    units2Drop.erase(units2Drop.begin());

                    pUnit2Drop->deploy(newCoord);

                    if (newCoord.x == 0) {
                        pUnit2Drop->setAngle(RIGHT);
                        pUnit2Drop->setDestination(newCoord + Coord(1,0));
                    } else if (newCoord.x == currentGameMap->sizeX-1) {
                        pUnit2Drop->setAngle(LEFT);
                        pUnit2Drop->setDestination(newCoord + Coord(-1,0));
                    } else if (newCoord.y == 0) {
                        pUnit2Drop->setAngle(DOWN);
                        pUnit2Drop->setDestination(newCoord + Coord(0,1));
                    } else if (newCoord.y == currentGameMap->sizeY-1) {
                        pUnit2Drop->setAngle(UP);
                        pUnit2Drop->setDestination(newCoord + Coord(0,-1));
                    }
                }
            }

        } break;

        case Drop_Air:
        case Drop_Visible:
        case Drop_Enemybase:
        case Drop_Homebase: {
            Coord dropCoord(INVALID_POS, INVALID_POS);

            switch(dropLocation) {
                case Drop_Air: {
                    dropCoord = Coord(currentGame->RandomGen.rand(0,currentGameMap->sizeX-1), currentGame->RandomGen.rand(0,currentGameMap->sizeY-1));
                } break;

                case Drop_Visible: {
                    dropCoord = Coord(currentGameMap->sizeX / 2, currentGameMap->sizeY / 2);
                } break;

                case Drop_Enemybase: {
                    for(int i=0;i<NUM_HOUSES;i++) {
                        if((currentGame->house[i] != NULL)
                             && (currentGame->house[i]->getNumStructures() != 0)
                             && currentGame->house[i]->getTeam() != 0
                             && currentGame->house[i]->getTeam() != dropHouse->getTeam()) {
                            dropCoord = currentGame->house[i]->getCenterOfMainBase();
                            break;
                        }
                    }
                } break;

                case Drop_Homebase: {
                    if(dropHouse->getNumStructures() == 0) {
                        // house has only units
                        break;
                    }
                    dropCoord = dropHouse->getCenterOfMainBase();
                } break;

                default: {
                } break;
            }

            if(dropCoord == Coord(INVALID_POS, INVALID_POS)) {
                break;
            }

            // try 32 times
            for(int i=0;i<32;i++) {
                int r = currentGame->RandomGen.rand(0,7);
                double angle = 2*M_PI*currentGame->RandomGen.randDouble();

                dropCoord += Coord( (int) (r*sin(angle)), (int) (-r*cos(angle)));

                if(currentGameMap->cellExists(dropCoord) && currentGameMap->getCell(dropCoord)->hasAGroundObject() == false) {
                    // found the an empty drop location => drop here

                    Carryall* carryall = (Carryall*) dropHouse->createUnit(Unit_Carryall);
                    carryall->setOwned(false);

                    std::vector<Uint32>::const_iterator iter;
                    for(iter = droppedUnits.begin(); iter != droppedUnits.end(); ++iter) {
                        UnitClass* pUnit2Drop = dropHouse->createUnit(*iter);
                        pUnit2Drop->setActive(false);
                        carryall->giveCargo(pUnit2Drop);
                    }

                    Coord closestPos = currentGameMap->findClosestEdgePoint(dropCoord, Coord(1,1));
                    carryall->deploy(closestPos);
                    carryall->setDropOfferer(true);

                    if (closestPos.x == 0)
                        carryall->setAngle(RIGHT);
                    else if (closestPos.x == currentGameMap->sizeX-1)
                        carryall->setAngle(LEFT);
                    else if (closestPos.y == 0)
                        carryall->setAngle(DOWN);
                    else if (closestPos.y == currentGameMap->sizeY-1)
                        carryall->setAngle(UP);

                    carryall->setDestination(dropCoord);

                    break;
                }
            }

        } break;


        default: {
            fprintf(stderr,"ReinforcementTrigger::trigger(): Invalid drop location!\n");
        } break;
    }

    if(isRepeat()) {
        ReinforcementTrigger* pReinforcementTrigger = new ReinforcementTrigger(*this);
        pReinforcementTrigger->cycleNumber += repeatCycle;
        std::shared_ptr<Trigger> newTrigger = std::shared_ptr<Trigger>(pReinforcementTrigger);
        currentGame->GetTriggerManager().addTrigger(newTrigger);
    }
}
