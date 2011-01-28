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

#include <FileClasses/Fntfile.h>
#include <FileClasses/Palette.h>

#include <SDL_endian.h>
#include <stdlib.h>
#include <string.h>

extern Palette palette;

/// Constructor
/**
	The constructor reads from the RWop all data and saves them internal. The SDL_RWops can be readonly but must support
	seeking. Immediately after the Fntfile-Object is constructed RWops can be closed. All data is saved in the class.
	\param	RWop	SDL_RWops to the fnt-File. (can be readonly)
*/
Fntfile::Fntfile(SDL_RWops* RWop)
{
	unsigned char* Filedata;
	Uint32 FntFilesize;
	if(RWop == NULL) {
		fprintf(stderr, "Fntfile: RWop == NULL!\n");
		exit(EXIT_FAILURE);
	}

	FntFilesize = SDL_RWseek(RWop,0,SEEK_END);
	if(FntFilesize <= 0) {
		fprintf(stderr,"Fntfile: Cannot determine size of this *.fnt-File!\n");
		exit(EXIT_FAILURE);
	}

	if(SDL_RWseek(RWop,0,SEEK_SET) != 0) {
		fprintf(stderr,"Fntfile: Seeking in this *.fnt-File failed!\n");
		exit(EXIT_FAILURE);
	}

	if( (Filedata = (unsigned char*) malloc(FntFilesize)) == NULL) {
		fprintf(stderr,"Fntfile: Allocating memory failed!\n");
		exit(EXIT_FAILURE);
	}

	if(SDL_RWread(RWop, Filedata, FntFilesize, 1) != 1) {
		fprintf(stderr,"Fntfile: Reading this *.fnt-File failed!\n");
		exit(EXIT_FAILURE);
	}

	readfile(Filedata,(Uint16) FntFilesize);

	free(Filedata);
}

/// Destructor
/**
	Frees all memory.
*/
Fntfile::~Fntfile()
{
	if(Character != NULL) {
		for(Uint32 i = 0; i < NumCharacters; i++) {
			if(Character[i].data != NULL) {
				free(Character[i].data);
			}
		}
		free(Character);
	}
}

/// Returns one character in this fnt-File
/**
	This method returns a SDL_Surface containing the nth character in this fnt-File.
	The returned SDL_Surface should be freed with SDL_FreeSurface() if no longer needed.
	\param	IndexOfFile	specifies which character to return (zero based)
	\return	nth picture in this fnt-File
*/
SDL_Surface *Fntfile::getCharacter(Uint32 IndexOfFile)
{
	SDL_Surface *pic = NULL;

	if(IndexOfFile >= NumCharacters) {
		return NULL;
	}
	// create new picture surface
	if((pic = SDL_CreateRGBSurface(SDL_SWSURFACE,Character[IndexOfFile].width,CharacterHeight,8,0,0,0,0))== NULL) {
		return NULL;
	}

    palette.applyToSurface(pic);
	SDL_LockSurface(pic);

	//Now we can copy pixel by pixel
	for(int y = 0; y < CharacterHeight; y++) {
		for(int x = 0; x < Character[IndexOfFile].width; x++) {
			if((Character[IndexOfFile].data[y*Character[IndexOfFile].width+x] == 1)
				|| ((Character[IndexOfFile].data[y*Character[IndexOfFile].width+x] > 3)
				&& (Character[IndexOfFile].data[y*Character[IndexOfFile].width+x] < 0x0E))) {
				*(((unsigned char*) pic->pixels) + y*pic->pitch + x) = 255;
			} else {
				*(((unsigned char*) pic->pixels) + y*pic->pitch + x) = 0;
			}
			//*(((char*) pic->pixels) + y*pic->pitch + x) = Character[IndexOfFile].data[y*Character[IndexOfFile].width+x];

		}
	}

	SDL_UnlockSurface(pic);
/*	DEBUG:
	for(int y = 0; y < CharacterHeight; y++) {
		for(int x = 0; x < Character[IndexOfFile].width; x++) {
			if(Character[IndexOfFile].data[y*Character[IndexOfFile].width+x] == 0) {
				printf(" ");
			} else {
				printf("%X",Character[IndexOfFile].data[y*Character[IndexOfFile].width+x]);
			}
		}
		printf("\n");
	}
	printf("\n");
*/
	return pic;
}



void Fntfile::DrawTextOnSurface(SDL_Surface* pSurface, std::string text, char BaseColor) {
	SDL_LockSurface(pSurface);

	int CurXPos = 0;
	const unsigned char* pText = (unsigned char*) text.c_str();
	while(*pText != '\0') {
		int index = 0;
		if(*pText < NumCharacters) {
			index = *pText;
		}

		//Now we can copy pixel by pixel
		for(int y = 0; y < CharacterHeight; y++) {
			for(int x = 0; x < Character[index].width; x++) {
				/*
				// old way of gettting color out of the font
				if((Character[index].data[y*Character[index].width+x] == 1)
				|| ((Character[index].data[y*Character[index].width+x] > 3)
				&& (Character[index].data[y*Character[index].width+x] < 0x0E))) {
					*(((char*) pSurface->pixels) + y*pSurface->pitch + x + CurXPos) = color;
				}
				*/

				// new way
				char color = Character[index].data[y*Character[index].width+x];
				if(color > 7) {
					color = 15 - color;
				}

				if(color != 0) {
					*(((char*) pSurface->pixels) + y*pSurface->pitch + x + CurXPos) = BaseColor + (color-1);
				}

			}
		}

		CurXPos += Character[index].width;
		pText++;
	}


	SDL_UnlockSurface(pSurface);
}

/// Returns the number of pixels a text needs
/**
		This methods returns the number of pixels this text would need if printed.
		\param	text	The text to be checked for it's length in pixel
		\return Number of pixels needed
*/
int	Fntfile::getTextWidth(std::string text) {
	int width = 0;
	const unsigned char* pText = (unsigned char*) text.c_str();
	while(*pText != '\0') {
		if(*pText >= NumCharacters) {
			width += Character[0].width;
		} else {
			width += Character[*pText].width;
		}
		pText++;
	}

	return width;
}

void Fntfile::readfile(unsigned char* Filedata, Uint16 Filesize) {
	// a valid fnt-file must be at least 20 bytes big
	if(Filesize < 20) {
		fprintf(stderr,"Fntfile::readfile(): A valid fnt-file must be at least 20 bytes long!\n");
		exit(EXIT_FAILURE);
	}

	Uint16 size = SDL_SwapLE16(((Uint16*) Filedata)[0]);
	Uint16 magic1 = SDL_SwapLE16(((Uint16*) Filedata)[1]);
	Uint16 magic2 = SDL_SwapLE16(((Uint16*) Filedata)[2]);
	Uint16 magic3 = SDL_SwapLE16(((Uint16*) Filedata)[7]);

	if((size != Filesize) || (magic1 != 0x0500)  || (magic2 != 0x000E) || (magic3 != 0x1012)) {
		fprintf(stderr,"Fntfile::readfile(): Invalid filesize or magic number!\n");
		exit(EXIT_FAILURE);
	}

	if(		(SDL_SwapLE16(((Uint16*) Filedata)[4]) >= Filesize)
		||	(SDL_SwapLE16(((Uint16*) Filedata)[5]) >= Filesize)
		||	(SDL_SwapLE16(((Uint16*) Filedata)[6]) >= Filesize)) {
		fprintf(stderr,"Fntfile::readfile(): Invalid data positions!\n");
		exit(EXIT_FAILURE);
	}

	Uint16* chars = (Uint16*) (Filedata + SDL_SwapLE16(((Uint16*) Filedata)[3]));
	Uint8* widths = Filedata + SDL_SwapLE16(((Uint16*) Filedata)[4]);
	//unsigned char* graphics = Filedata + SDL_SwapLE16(((Uint16*) Filedata)[5]);
	Uint16* heights = (Uint16*) (Filedata + SDL_SwapLE16(((Uint16*) Filedata)[6]));

	// Filedata[16] might be the first character index and Filedata[17] the last index,
	// but I don't know for sure.
	NumCharacters =  Filedata[17] + 1;
	CharacterHeight = Filedata[18];
	MaxCharacterWidth = Filedata[19];

	// Check if every array is in the file
	if(	( (unsigned char*) (chars+NumCharacters) > (Filedata+Filesize))
		|| ( (unsigned char*) (widths+NumCharacters) > (Filedata+Filesize))
		|| ( (unsigned char*) (heights+NumCharacters) > (Filedata+Filesize))) {
		fprintf(stderr,"Fntfile::readfile(): Invalid fnt-File. File too small and cannot contain all characters!\n");
		exit(EXIT_FAILURE);
	}

	if((Character = (FntfileCharacter*) calloc(sizeof(FntfileCharacter) * NumCharacters,1)) == NULL) {
		fprintf(stderr,"Fntfile::readfile(): Cannot allocate memory!\n");
		exit(EXIT_FAILURE);
	}

	for(Uint32 i = 0; i < NumCharacters; i++) {
		Character[i].width = widths[i];

		if((Character[i].data = (unsigned char*) calloc(Character[i].width*CharacterHeight,1)) == NULL) {
			fprintf(stderr,"Fntfile::readfile(): Cannot allocate memory!\n");
			exit(EXIT_FAILURE);
		}

		unsigned char* chardata = Filedata+SDL_SwapLE16(chars[i]);
		int charHeight = (SDL_SwapLE16(heights[i])) >> 8;
		int yPos = (SDL_SwapLE16(heights[i])) & 0x00FF;

		if(chardata + charHeight*((Character[i].width+1)/2) > (Filedata+Filesize)) {
			fprintf(stderr,"Fntfile::readfile(): Invalid character graphic. The graphic goes beyond end of file!\n");
			exit(EXIT_FAILURE);
		}

		if(yPos+charHeight > CharacterHeight) {
			fprintf(stderr,"Fntfile::readfile(): Invalid character graphic. The character height is bigger than the font height!\n");
			exit(EXIT_FAILURE);
		}

		int j=0;
		// now we can copy the data
		for(int y = yPos; y < yPos+charHeight; y++) {
			for(int x = 0; x < Character[i].width; x+=2) {
				Character[i].data[y*Character[i].width + x] = chardata[j] & 0x0F;
				if(x+1 < Character[i].width) {
					Character[i].data[y*Character[i].width + x + 1] = chardata[j] >> 4;
				}
				j++;
			}
		}
	}
}
