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

#ifndef BRIEFINGTEXT_H
#define BRIEFINGTEXT_H

#include <misc/string_util.h>

#include <string>
#include <vector>
#include <stdexcept>
#include <SDL.h>
#include <SDL_rwops.h>

#define MISSION_DESCRIPTION		0
#define MISSION_WIN				1
#define MISSION_LOSE			2
#define MISSION_ADVICE			3


/// A class for loading a encoded textfile.
/**
	This class can read encoded textfiles and return their content in decoded ANSI Code.
*/
class BriefingText
{
public:
	BriefingText(SDL_RWops* RWop);
	~BriefingText();


    /// This method returns nth string in the text file.
	/**
	This method returns the nth text in this text file.
	\param	n		the number of the string to return
	\return	the nth text in the file.
	*/
	std::string getString(unsigned int n) {
        if(n < BriefingStrings.size()) {
			return BriefingStrings[n];
		} else {
		    throw std::invalid_argument("BriefingText::getString(): string number invalid!");
		}
	}

	/// This method returns the briefing/debriefing text.
	/**
	This method returns the briefing/debriefing text for the mission specified by the parameter mission. The second
	parameter specifies the kind of briefing/debriefing.
	\param	mission		the mission number (0=House description; 1,2,...,9 = mission description).
	\param	texttype	one of MISSION_DESCRIPTION, MISSION_WIN, MISSION_LOSE, MISSION_ADVICE
	\return	the text for this mission and of this type.
	*/
	std::string getMissionBriefingString(unsigned int mission, unsigned int texttype) {
		unsigned int index = mission*4+texttype;

		if(index <  BriefingStrings.size()) {
			return BriefingStrings[index];
		} else {
			throw std::invalid_argument("BriefingText::getMissionBriefingString(): mission number or text type is invalid: mission=" + stringify(mission) + " texttype=" + stringify(texttype));
		}
	}

private:
	std::string decodeString(std::string text);
	std::vector<std::string> BriefingStrings;
};

#endif // BRIEFINGTEXT_H
