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

#include <FileClasses/IndexedTextFile.h>

#include <SDL_endian.h>
#include <string>
#include <algorithm>
#include <stdexcept>

IndexedTextFile::IndexedTextFile(SDL_RWops* RWop) {
	int IndexedTextFilesize;
	unsigned char* Filedata;
	Uint16* Index;

	if(RWop == NULL) {
	    throw std::invalid_argument("IndexedTextFile:IndexedTextFile(): RWop == NULL!");
	}

	IndexedTextFilesize = SDL_RWseek(RWop,0,SEEK_END);
	if(IndexedTextFilesize <= 0) {
        throw std::runtime_error("IndexedTextFile:IndexedTextFile(): Cannot determine size of this file!");
	}

	if(IndexedTextFilesize < 2) {
        throw std::runtime_error("IndexedTextFile:IndexedTextFile(): No valid indexed textfile: File too small!");
	}

	if(SDL_RWseek(RWop,0,SEEK_SET) != 0) {
        throw std::runtime_error("IndexedTextFile:IndexedTextFile(): Seeking in this indexed textfile failed!");
	}

	if( (Filedata = (unsigned char*) malloc(IndexedTextFilesize)) == NULL) {
        throw std::bad_alloc();
	}

	if(SDL_RWread(RWop, Filedata, IndexedTextFilesize, 1) != 1) {
	    free(Filedata);
        throw std::runtime_error("IndexedTextFile:IndexedTextFile(): Reading this indexed textfile failed!");
	}

	numIndexedStrings = (SDL_SwapLE16(((Uint16*) Filedata)[0]))/2 - 1;
	Index = (Uint16*) Filedata;
	for(unsigned int i=0; i <= numIndexedStrings; i++) {
		Index[i] = SDL_SwapLE16(Index[i]);
	}

	IndexedStrings = new std::string[numIndexedStrings];

	for(unsigned int i=0; i < numIndexedStrings;i++) {
		IndexedStrings[i] = (const char*) (Filedata+Index[i]);

		// Now convert DOS ASCII to ANSI ASCII
		replace(IndexedStrings[i].begin(),IndexedStrings[i].end(), '\x0D','\x0A');  // '\r' -> '\n'
        replace(IndexedStrings[i].begin(),IndexedStrings[i].end(), '\x84','\xE4');  // german umlaut "ae"
        replace(IndexedStrings[i].begin(),IndexedStrings[i].end(), '\x94','\xF6');  // german umlaut "oe"
        replace(IndexedStrings[i].begin(),IndexedStrings[i].end(), '\x81','\xFC');  // german umlaut "ue"
        replace(IndexedStrings[i].begin(),IndexedStrings[i].end(), '\x8E','\xC4');  // german umlaut "AE"
        replace(IndexedStrings[i].begin(),IndexedStrings[i].end(), '\x99','\xD6');  // german umlaut "OE"
        replace(IndexedStrings[i].begin(),IndexedStrings[i].end(), '\x9A','\xDC');  // german umlaut "UE"
        replace(IndexedStrings[i].begin(),IndexedStrings[i].end(), '\xE1','\xDF');  // german umlaut "ss"
	}

	free(Filedata);
}

IndexedTextFile::~IndexedTextFile() {
	delete [] IndexedStrings;
}
