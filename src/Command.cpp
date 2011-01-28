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

#include <Command.h>

#include <globals.h>

#include <Game.h>
#include <House.h>

#include <units/UnitClass.h>
#include <units/Carryall.h>
#include <units/DevastatorClass.h>
#include <units/MCVClass.h>
#include <units/InfantryClass.h>
#include <structures/BuilderClass.h>
#include <structures/TurretClass.h>
#include <structures/PalaceClass.h>
#include <structures/StarPortClass.h>

#include <stdexcept>

Command::Command(CMDTYPE id) {
	CommandID = id;
}

Command::Command(CMDTYPE id, Uint32 parameter1) {
	CommandID = id;
	Parameter.push_back(parameter1);
}

Command::Command(CMDTYPE id, Uint32 parameter1, Uint32 parameter2) {
	CommandID = id;
	Parameter.push_back(parameter1);
	Parameter.push_back(parameter2);
}

Command::Command(CMDTYPE id, Uint32 parameter1, Uint32 parameter2, Uint32 parameter3) {
	CommandID = id;
	Parameter.push_back(parameter1);
	Parameter.push_back(parameter2);
	Parameter.push_back(parameter3);
}

Command::Command(CMDTYPE id, Uint32 parameter1, Uint32 parameter2, Uint32 parameter3, Uint32 parameter4) {
	CommandID = id;
	Parameter.push_back(parameter1);
	Parameter.push_back(parameter2);
	Parameter.push_back(parameter3);
	Parameter.push_back(parameter4);
}

Command::Command(Uint8* data, Uint32 length) {
	if(length % 4 != 0) {
		throw std::invalid_argument("Command::Command(): Length must be multiple of 4!");
	}

	if(length < 4) {
		throw std::invalid_argument("Command::Command(): Command must be at least 4 bytes long!");
	}

	CommandID = (CMDTYPE) *((Uint32*) data);

	if(CommandID >= CMD_MAX) {
		throw std::invalid_argument("Command::Command(): CommandID unknown!");
	}

	Uint32* pData = (Uint32*) (data+4);
	for(Uint32 i=0;i<(length-4)/4;i++) {
		Parameter.push_back(*pData);
		*pData++;
	}
}

Command::Command(Stream& stream) {
	CommandID = (CMDTYPE) stream.readUint32();
	Parameter = stream.readUint32Vector();
}

Command::~Command() {
}

void Command::save(Stream& stream) const {
	stream.writeUint32((Uint32) CommandID);
	stream.writeUint32Vector(Parameter);
	stream.flush();
}

void Command::executeCommand() const {
	switch(CommandID) {
        case CMD_TEST_SYNC: {
			if(Parameter.size() != 1) {
				throw std::invalid_argument("Command::executeCommand(): CMD_TEST_SYNC needs 1 Parameters!");
			}

			if(currentGame->RandomGen.getSeed() != Parameter[0]) {
			    static bool bOutOfSyncPrinted = false;

			    if(bOutOfSyncPrinted == false) {
                    fprintf(stderr,"Game-cycle %d: The game is out of sync\n", currentGame->GetGameCycleCount());

                    for(int i=0;i<1000000;i++) {
                        currentGame->RandomGen.rand();
                        if(currentGame->RandomGen.getSeed() == Parameter[0]) {
                            fprintf(stderr,"%d is %d random numbers away\n",Parameter[0], i);
                            break;
                        }
                    }
                    bOutOfSyncPrinted = true;
			    }

                // exit(EXIT_FAILURE);
			}

		} break;

		case CMD_PLACE_STRUCTURE: {
			if(Parameter.size() != 4) {
				throw std::invalid_argument("Command::executeCommand(): CMD_PLACE_STRUCTURE needs 4 Parameters!");
			}
			BuilderClass* pBuilder = dynamic_cast<BuilderClass*>(currentGame->getObjectManager().getObject(Parameter[0]));
			if(pBuilder == NULL) {
				return;
			}
			pBuilder->getOwner()->placeStructure(Parameter[0], (int) Parameter[1], (int) Parameter[2], (int) Parameter[3]);
		} break;


		case CMD_UNIT_MOVE2POS: {
			if(Parameter.size() != 4) {
				throw std::invalid_argument("Command::executeCommand(): CMD_UNIT_MOVE2POS needs 4 Parameters!");
			}
			UnitClass* unit = dynamic_cast<UnitClass*>(currentGame->getObjectManager().getObject(Parameter[0]));
			unit->DoMove2Pos((int) Parameter[1], (int) Parameter[2], (bool) Parameter[3]);
		} break;

		case CMD_UNIT_MOVE2OBJECT: {
			if(Parameter.size() != 2) {
				throw std::invalid_argument("Command::executeCommand(): CMD_UNIT_MOVE2OBJECT needs 2 Parameters!");
			}
			UnitClass* unit = dynamic_cast<UnitClass*>(currentGame->getObjectManager().getObject(Parameter[0]));
			unit->DoMove2Object((int) Parameter[1]);
		} break;

		case CMD_UNIT_ATTACKPOS: {
			if(Parameter.size() != 4) {
				throw std::invalid_argument("Command::executeCommand(): CMD_UNIT_ATTACKPOS needs 4 Parameters!");
			}
			UnitClass* unit = dynamic_cast<UnitClass*>(currentGame->getObjectManager().getObject(Parameter[0]));
			unit->DoAttackPos((int) Parameter[1], (int) Parameter[2], (bool) Parameter[3]);
		} break;

		case CMD_UNIT_ATTACKOBJECT: {
			if(Parameter.size() != 2) {
				throw std::invalid_argument("Command::executeCommand(): CMD_UNIT_ATTACKOBJECT needs 2 Parameters!");
			}
			UnitClass* pUnit = dynamic_cast<UnitClass*>(currentGame->getObjectManager().getObject(Parameter[0]));
			pUnit->DoAttackObject((int) Parameter[1]);
		} break;

        case CMD_INFANTRY_CAPTURE: {
			if(Parameter.size() != 2) {
				throw std::invalid_argument("Command::executeCommand(): CMD_INFANTRY_CAPTURE needs 2 Parameters!");
			}
			InfantryClass* pInfantry = dynamic_cast<InfantryClass*>(currentGame->getObjectManager().getObject(Parameter[0]));
			pInfantry->DoCaptureObject((int) Parameter[1]);
		} break;

		case CMD_UNIT_SETMODE: {
			if(Parameter.size() != 2) {
				throw std::invalid_argument("Command::executeCommand(): CMD_UNIT_SETMODE needs 2 Parameter!");
			}
			UnitClass* pUnit = dynamic_cast<UnitClass*>(currentGame->getObjectManager().getObject(Parameter[0]));
			pUnit->DoSetAttackMode((ATTACKMODE) Parameter[1]);
		} break;

		case CMD_CARRYALL_SETDEPLOYSTRUCTURE: {
			if(Parameter.size() != 2) {
				throw std::invalid_argument("Command::executeCommand(): CMD_CARRYALL_SETDEPLOYSTRUCTURE needs 2 Parameters!");
			}
			Carryall* pCarryall = dynamic_cast<Carryall*>(currentGame->getObjectManager().getObject(Parameter[0]));
			pCarryall->DoSetDeployStructure((int) Parameter[1]);
		} break;

		case CMD_DEVASTATOR_STARTDEVASTATE: {
			if(Parameter.size() != 1) {
				throw std::invalid_argument("Command::executeCommand(): CMD_DEVASTATOR_STARTDEVASTATE needs 1 Parameter!");
			}
			DevastatorClass* pDevastator = dynamic_cast<DevastatorClass*>(currentGame->getObjectManager().getObject(Parameter[0]));
			pDevastator->DoStartDevastate();
		} break;

		case CMD_MCV_DEPLOY: {
			if(Parameter.size() != 1) {
				throw std::invalid_argument("Command::executeCommand(): CMD_MCV_DEPLOY needs 1 Parameter!");
			}
			MCVClass* pMCV = dynamic_cast<MCVClass*>(currentGame->getObjectManager().getObject(Parameter[0]));
			pMCV->DoDeploy();
		} break;

		case CMD_STRUCTURE_SETDEPLOYPOSITION: {
			if(Parameter.size() != 3) {
				throw std::invalid_argument("Command::executeCommand(): CMD_STRUCTURE_SETDEPLOYPOSITION needs 3 Parameters!");
			}
			StructureClass* pStructure = dynamic_cast<StructureClass*>(currentGame->getObjectManager().getObject(Parameter[0]));
			pStructure->DoSetDeployPosition((int) Parameter[1],(int) Parameter[2]);
		} break;

		case CMD_STRUCTURE_REPAIR: {
			if(Parameter.size() != 1) {
				throw std::invalid_argument("Command::executeCommand(): CMD_STRUCTURE_REPAIR needs 1 Parameter!");
			}
			StructureClass* pStructure = dynamic_cast<StructureClass*>(currentGame->getObjectManager().getObject(Parameter[0]));

			pStructure->DoRepair();
		} break;

		case CMD_BUILDER_UPGRADE: {
			if(Parameter.size() != 1) {
				throw std::invalid_argument("Command::executeCommand(): CMD_BUILDER_UPGRADE needs 1 Parameter!");
			}
			BuilderClass* pBuilder = dynamic_cast<BuilderClass*>(currentGame->getObjectManager().getObject(Parameter[0]));
			pBuilder->DoUpgrade();
		} break;

		case CMD_BUILDER_PRODUCEITEM: {
			if(Parameter.size() != 3) {
				throw std::invalid_argument("Command::executeCommand(): CMD_BUILDER_PRODUCEITEM needs 3 Parameter!");
			}
			BuilderClass* pBuilder = dynamic_cast<BuilderClass*>(currentGame->getObjectManager().getObject(Parameter[0]));
			pBuilder->DoProduceItem(Parameter[1],(bool) Parameter[2]);
		} break;

		case CMD_BUILDER_CANCELITEM: {
			if(Parameter.size() != 3) {
				throw std::invalid_argument("Command::executeCommand(): CMD_BUILDER_CANCELITEM needs 3 Parameter!");
			}
			BuilderClass* pBuilder = dynamic_cast<BuilderClass*>(currentGame->getObjectManager().getObject(Parameter[0]));
			pBuilder->DoCancelItem(Parameter[1],(bool) Parameter[2]);
		} break;

		case CMD_BUILDER_SETONHOLD: {
			if(Parameter.size() != 2) {
				throw std::invalid_argument("Command::executeCommand(): CMD_BUILDER_SETONHOLD needs 2 Parameters!");
			}
			BuilderClass* pBuilder = dynamic_cast<BuilderClass*>(currentGame->getObjectManager().getObject(Parameter[0]));
			pBuilder->DoSetOnHold((bool) Parameter[1]);
		} break;

		case CMD_PALACE_SPECIALWEAPON: {
			if(Parameter.size() != 1) {
				throw std::invalid_argument("Command::executeCommand(): CMD_PALACE_SPECIALWEAPON needs 1 Parameter!");
			}
			PalaceClass* palace = dynamic_cast<PalaceClass*>(currentGame->getObjectManager().getObject(Parameter[0]));
			palace->DoSpecialWeapon();
		} break;

        case CMD_PALACE_DEATHHAND: {
			if(Parameter.size() != 3) {
				throw std::invalid_argument("Command::executeCommand(): CMD_PALACE_DEATHHAND needs 3 Parameter!");
			}
			PalaceClass* palace = dynamic_cast<PalaceClass*>(currentGame->getObjectManager().getObject(Parameter[0]));

			palace->DoLaunchDeathhand((int) Parameter[1], (int) Parameter[2]);
		} break;

		case CMD_STARPORT_PLACEORDER: {
			if(Parameter.size() != 1) {
				throw std::invalid_argument("Command::executeCommand(): CMD_STARPORT_PLACEORDER needs 1 Parameter!");
			}
			StarPortClass* pStarport = dynamic_cast<StarPortClass*>(currentGame->getObjectManager().getObject(Parameter[0]));
			pStarport->DoPlaceOrder();
		} break;

		case CMD_STARPORT_CANCELORDER: {
			if(Parameter.size() != 1) {
				throw std::invalid_argument("Command::executeCommand(): CMD_STARPORT_CANCELORDER needs 1 Parameter!");
			}
			StarPortClass* pStarport = dynamic_cast<StarPortClass*>(currentGame->getObjectManager().getObject(Parameter[0]));
			pStarport->DoCancelOrder();
		} break;

		case CMD_TURRET_ATTACKOBJECT: {
			if(Parameter.size() != 2) {
				throw std::invalid_argument("Command::executeCommand(): CMD_TURRET_ATTACKOBJECT needs 2 Parameters!");
			}
			TurretClass* pTurret = dynamic_cast<TurretClass*>(currentGame->getObjectManager().getObject(Parameter[0]));
			pTurret->DoAttackObject((int) Parameter[1]);
		} break;

		default: {
			throw std::invalid_argument("Command::executeCommand(): Unknown CommandID!");
		} break;
	}

}

