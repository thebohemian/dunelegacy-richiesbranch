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

#ifndef INDEXEDTEXTFILE_H
#define INDEXEDTEXTFILE_H

#include <string>
#include <stdexcept>
#include <SDL.h>
#include <SDL_rwops.h>


/// A class for loading a indexed textfile (e.g. INTRO.ENG).
/**
	This class can read indexed textfiles and return their content in decoded ANSI Code.
*/
class IndexedTextFile
{
public:
	IndexedTextFile(SDL_RWops* RWop);
	~IndexedTextFile();


    /// This method returns nth string in the text file.
	/**
	This method returns the nth text in this text file.
	\param	n		the number of the string to return
	\return	the nth text in the file.
	*/
	std::string getString(unsigned int n) const {
        if(n >= numIndexedStrings) {
            throw std::invalid_argument("IndexedTextFile:getString(): Invalid index!");
        }

        return IndexedStrings[n];
	}


	/// This method returns the number of strings in this file
	/**
	\return the number of strings in this file
	*/
	unsigned int getNumStrings() const {
		return 	numIndexedStrings;
	}

private:
	std::string *IndexedStrings;
	unsigned int numIndexedStrings;
};

#endif // INDEXEDTEXTFILE_H
