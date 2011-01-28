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

#ifndef FNTFILE_H
#define FNTFILE_H

#include <SDL.h>
#include <SDL_rwops.h>
#include <stdarg.h>

#include "Font.h"

/// A class for loading a *.FNT-File.
/**
	This class can read westwood fnt-Files and extract the contained characters.
*/
class Fntfile : public Font
{
private:
	// Internal structure used for storing the character data
	struct FntfileCharacter
	{
		int	width;
		unsigned char* data;
	};

public:
	Fntfile(SDL_RWops* RWop);
	virtual ~Fntfile();

	SDL_Surface* getCharacter(Uint32 IndexOfFile);

	void DrawTextOnSurface(SDL_Surface* pSurface, std::string text, char BaseColor = 128);

	int	getTextWidth(std::string text);

	/// Returns the number of pixels this font needs in y-direction.
	/**
		This methods returns the height of this font.
		\return Number of pixels needed
	*/
	inline int getTextHeight() const { return CharacterHeight; };

	/// Returns the number of contained characters
	/**
		Returns the number of characters in this FNT-File.
		\return	Number of characters in this FNT-File.
	*/
	inline Uint32 getNumCharacters() const { return NumCharacters; };

private:
	void readfile(unsigned char* Filedata, Uint16 Filesize);

	FntfileCharacter * Character;
	Uint32 NumCharacters;
	Uint8 CharacterHeight;
	Uint8 MaxCharacterWidth;
};


#endif //FNTFILE_H
