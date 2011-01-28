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

#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

#include <Command.h>

#include <misc/Stream.h>

#include <vector>
#include <list>

/**
    The command manager collects all the given user commands (e.g. move unit u to position (x,y)) . These commands might be transfered over a network.
*/
class CommandManager {
public:

    /**
        default constructor
    */
	CommandManager();

	/// destructor
	virtual ~CommandManager();

    /**
        This method sets a stream where all commands are written when they are added to the command manager. This can be used for
        logging the complete game and enable a replay afterwards.
        \param  pStream     pointer to a stream all new commands will be written to (the stream must be created with new). NULL for disabling. pStream is deleted with delete if this command manager is destroyed.
    */
	void setStream(Stream* pStream) { this->pStream = pStream; };

    /**
        Get the stream used for recording new commands (see setStream).
        \return the set stream or NULL if none is set
    */
	Stream* getStream() const { return pStream; };

    /**
        If bReadOnly == true it is impossible to add new commands to this command manager. This is useful for replays.
        \param  bReadOnly   true = addCommand() is a NO-OP, false = addCommand() has normal behaviour
    */
	void setReadOnly(bool bReadOnly) { this->bReadOnly = bReadOnly; };

    /**
        Get the current read-only status.
        \return true = addCommand() is a NO-OP, false = addCommand() has normal behaviour
    */
	bool getReadOnly() const { return bReadOnly; };

    /**
        Save all commands to stream. The read-only status is not saved.
        \param  stream  the stream to write to
    */
	void save(Stream& stream) const;

    /**
        Load commands from stream
        \param  stream  the stream to read from
    */
	void load(Stream& stream);

    /**
        Adds a command at the next possible game cycle
        \param  cmd     the command to add
    */
	void addCommand(Command cmd);

    /**
        Adds a command at the game cycle CycleNumber
        \param  cmd         the command to add
        \param  CycleNumber the game cycle this command shall take effect
    */
	void addCommand(Command cmd, Uint32 CycleNumber);

    /**
        Runs all commands scheduled for game cycle CycleNumber
        \param  CycleNumber the current game cycle
    */
	void executeCommands(Uint32 CycleNumber) const;

private:
	std::vector< std::list<Command> > timeslot;     ///< a vector of lists containing the scheduled commands. At index x is a list of all commands scheduled for game cycle x.
	Stream* pStream;                                ///< a stream all added commands will be written to. May be NULL
	bool bReadOnly;                                 ///< true = addCommand() is a NO-OP, false = addCommand() has normal behaviour
};

#endif // COMMANDMANAGER_H

