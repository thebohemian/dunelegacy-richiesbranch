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

#include <CommandManager.h>

#include <globals.h>

#include <Game.h>



CommandManager::CommandManager() {
	pStream = NULL;
	bReadOnly = false;
}

CommandManager::~CommandManager() {
    delete pStream;
    pStream = NULL;
}

void CommandManager::addCommand(Command cmd) {
	Uint32 CycleNumber = currentGame->GetGameCycleCount();
	addCommand(cmd, CycleNumber);
}

void CommandManager::save(Stream& stream) const {
	for(unsigned int i=0;i<timeslot.size();i++) {
		const std::list<Command>& cmdlist = timeslot[i];
		std::list<Command>::const_iterator iter;

		for(iter = cmdlist.begin(); iter != cmdlist.end(); ++iter) {
			stream.writeUint32(i);
			iter->save(stream);
		}
	}
}

void CommandManager::load(Stream& stream) {
	try {
		while(1) {
			Uint32 cycle = stream.readUint32();
			addCommand(Command(stream), cycle);
		}
	} catch (Stream::exception& e) {
		;
	}
}

void CommandManager::addCommand(Command cmd, Uint32 CycleNumber) {
	if(bReadOnly == false) {

		if(CycleNumber >= timeslot.size()) {
			timeslot.resize(CycleNumber+1);
		}

		timeslot[CycleNumber].push_back(cmd);

		if(pStream != NULL) {
			pStream->writeUint32(CycleNumber);
			cmd.save(*pStream);
		}
	}
}

void CommandManager::executeCommands(Uint32 CycleNumber) const {
	if(CycleNumber >= timeslot.size()) {
		return;
	}

	const std::list<Command>& cmdlist = timeslot[CycleNumber];
	std::list<Command>::const_iterator iter;

	for(iter = cmdlist.begin(); iter != cmdlist.end(); ++iter) {
		iter->executeCommand();
	}
}

