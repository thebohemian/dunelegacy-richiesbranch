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

#include <FileClasses/Shpfile.h>
#include <FileClasses/Decode.h>
#include <FileClasses/Palette.h>

#include <SDL_endian.h>
#include <stdexcept>

extern Palette palette;

/// Constructor
/**
	The constructor reads from the RWop all data and saves them internal. The SDL_RWops can be readonly but must support
	seeking. Immediately after the Shpfile-Object is constructed RWops can be closed. All data is saved in the class.
	\param	RWop	SDL_RWops to the shp-File. (can be readonly)
	\param  freesrc A non-zero value means it will automatically close/free the rwop for you.
*/
Shpfile::Shpfile(SDL_RWops* RWop, int freesrc)
{
	if(RWop == NULL) {
	    throw std::invalid_argument("Shpfile::Shpfile(): RWop == NULL!");
	}

	ShpFilesize = SDL_RWseek(RWop,0,SEEK_END);
	if(ShpFilesize <= 0) {
		throw std::runtime_error("Shpfile::Shpfile(): Cannot determine size of this *.shp-File!");
	}

	if(SDL_RWseek(RWop,0,SEEK_SET) != 0) {
		throw std::runtime_error("Shpfile::Shpfile(): Seeking in this *.shp-File failed!");
	}

    Filedata = new uint8_t[ShpFilesize];

	if(SDL_RWread(RWop, Filedata, ShpFilesize, 1) != 1) {
	    delete [] Filedata;
		throw std::runtime_error("Shpfile::Shpfile(): Reading this *.shp-File failed!");
	}

    try {
        readIndex();
    } catch (std::exception &e) {
        delete [] Filedata;
        throw;
    }

    if(freesrc) {
        SDL_RWclose(RWop);
    }
}

/// Destructor
/**
	Frees all memory.
*/
Shpfile::~Shpfile()
{
	delete [] Filedata;
}

/// Returns one picture in this shp-File
/**
	This method returns a SDL_Surface containing the nth picture in this shp-File.
	The returned SDL_Surface should be freed with SDL_FreeSurface() if no longer needed.
	\param	IndexOfFile	specifies which picture to return (zero based)
	\return	nth picture in this shp-File
*/
SDL_Surface *Shpfile::getPicture(Uint32 IndexOfFile)
{
	SDL_Surface *pic = NULL;
	unsigned char *DecodeDestination = NULL;
	unsigned char *ImageOut = NULL;

	if(IndexOfFile >= ShpfileEntries.size()) {
		return NULL;
	}

	unsigned char * Fileheader = Filedata + ShpfileEntries[IndexOfFile].StartOffset;

	unsigned char type = Fileheader[0];

	unsigned char sizeY = Fileheader[2];
	unsigned char sizeX = Fileheader[3];

	/* size and also checksum */
	Uint16 size = SDL_SwapLE16(*((Uint16*) (Fileheader + 8)));

	if((ImageOut = (unsigned char*) calloc(1,sizeX*sizeY)) == NULL) {
				return NULL;
	}

	switch(type) {

		case 0:
		{
			if( (DecodeDestination = (unsigned char*) calloc(1,size)) == NULL) {
				free(ImageOut);
				return NULL;
			}

			if(decode80(Fileheader + 10,DecodeDestination,size) == -1) {
				fprintf(stderr,"Warning: Checksum-Error in Shp-File\n");
			}

			shp_correct_lf(DecodeDestination,ImageOut, size);

			free(DecodeDestination);
		} break;

		case 1:
		{
			if( (DecodeDestination = (unsigned char*) calloc(1,size)) == NULL) {
				free(ImageOut);
				return NULL;
			}

			if(decode80(Fileheader + 10 + 16,DecodeDestination,size) == -1) {
				fprintf(stderr,"Warning: Checksum-Error in Shp-File\n");
			}

			shp_correct_lf(DecodeDestination, ImageOut, size);

			apply_pal_offsets(Fileheader + 10,ImageOut,sizeX*sizeY);

			free(DecodeDestination);
		} break;

		case 2:
		{
			shp_correct_lf(Fileheader+10, ImageOut,size);
		} break;

		case 3:
		{

			shp_correct_lf(Fileheader + 10 + 16, ImageOut,size);

			apply_pal_offsets(Fileheader + 10,ImageOut,sizeX*sizeY);
		} break;

		default:
		{
			fprintf(stderr,"Error: Type %d in SHP-Files not supported!\n",type);
			exit(EXIT_FAILURE);
		}
	}

	// create new picture surface
	if((pic = SDL_CreateRGBSurface(SDL_HWSURFACE,sizeX,sizeY,8,0,0,0,0))== NULL) {
		return NULL;
	}

	palette.applyToSurface(pic);
	SDL_LockSurface(pic);

	//Now we can copy line by line
	for(int y = 0; y < sizeY;y++) {
		memcpy(	((char*) (pic->pixels)) + y * pic->pitch , ImageOut + y * sizeX, sizeX);
	}

	SDL_UnlockSurface(pic);

	free(ImageOut);

	return pic;
}

/// Returns multiple pictures in this shp-File
/**
	This method returns a SDL_Surface containing an array of pictures from this shp-File.
	All pictures must be of the same size. tilesX/tilesY specifies how many pictures are in this row/column.
	Afterwards there must be tilesX*tilesY many parameters. Every parameter specifies which picture
	of this shp-File should be used. This indices must be ORed with a parameter specifing hwo they should
	be in the result surface. There are 4 modes and you must OR exactly one:
	 - TILE_NORMAL	Normal
	 - TILE_FLIPH	mirrored horizontally
	 - TILE_FLIPV	mirrored vertically
	 - TILE_ROTATE	Rotated by 180 degress

	Example:
	\code
	pPicture = myShpfile->getPictureArray(4,1, TILE_NORMAL | 20, TILE_FLIPH | 23, TILE_ROTATE | 67, TILE_NORMAL | 68);
	\endcode
	This example would create a surface with four pictures in it. From the left to the right there are
	picture 20,23,67 and 68. picture 23 is mirrored horizontally, 67 is rotated.<br><br>
	The returned SDL_Surface should be freed with SDL_FreeSurface() if no longer needed.
	\param	tilesX	how many pictures in one row
	\param	tilesY	how many pictures in one column
	\return	picture in this shp-File containing all specified pictures
*/
SDL_Surface* Shpfile::getPictureArray(unsigned int tilesX, unsigned int tilesY, ...) {
	SDL_Surface *pic = NULL;
	unsigned char *DecodeDestination = NULL;
	unsigned char *ImageOut = NULL;
	Uint32 i,j;

	Uint32* tiles;

	if((tilesX == 0) || (tilesY == 0)) {
		return NULL;
	}

	if((tiles = (Uint32*) malloc(tilesX*tilesY*sizeof(Uint32))) == NULL) {
		fprintf(stderr,"Shpfile::getPictureArray(): Cannot allocate memory!\n");
		exit(EXIT_FAILURE);
	}

	va_list arg_ptr;
	va_start(arg_ptr, tilesY);

	for(i = 0; i < tilesX*tilesY; i++) {
		tiles[i] = va_arg( arg_ptr, int );
		if(TILE_GETINDEX(tiles[i]) >= ShpfileEntries.size()) {
			free(tiles);
			fprintf(stderr,"Shpfile::getPictureArray(): There exist only %d files in this *.shp.\n", (int) ShpfileEntries.size());
			return NULL;
		}
	}

	va_end(arg_ptr);

	unsigned char sizeY = (Filedata + ShpfileEntries[TILE_GETINDEX(tiles[0])].StartOffset)[2];
	unsigned char sizeX = (Filedata + ShpfileEntries[TILE_GETINDEX(tiles[0])].StartOffset)[3];

	for(i = 1; i < tilesX*tilesY; i++) {
		if(((Filedata + ShpfileEntries[TILE_GETINDEX(tiles[i])].StartOffset)[2] != sizeY)
		 || ((Filedata + ShpfileEntries[TILE_GETINDEX(tiles[i])].StartOffset)[3] != sizeX)) {
			free(tiles);
			fprintf(stderr,"Shpfile::getPictureArray(): Not all pictures have the same size!\n");
			exit(EXIT_FAILURE);
		 }
	}

	// create new picture surface
	if((pic = SDL_CreateRGBSurface(SDL_HWSURFACE,sizeX*tilesX,sizeY*tilesY,8,0,0,0,0)) == NULL) {
		fprintf(stderr,"Shpfile::getPictureArray(): Cannot create Surface.\n");
		exit(EXIT_FAILURE);
	}

	palette.applyToSurface(pic);
	SDL_LockSurface(pic);

	for(j = 0; j < tilesY; j++)	{
		for(i = 0; i < tilesX; i++) {

			unsigned char * Fileheader = Filedata + ShpfileEntries[TILE_GETINDEX(tiles[j*tilesX+i])].StartOffset;
			unsigned char type = Fileheader[0];

			/* size and also checksum */
			Uint16 size = SDL_SwapLE16(*((Uint16*) (Fileheader + 8)));

			if((ImageOut = (unsigned char*) calloc(1,sizeX*sizeY)) == NULL) {
				free(tiles);
				fprintf(stderr,"Shpfile::getPictureArray(): Cannot allocate memory!\n");
				exit(EXIT_FAILURE);
			}

			switch(type) {

				case 0:
				{
					if( (DecodeDestination = (unsigned char*) calloc(1,size)) == NULL) {
						free(ImageOut);
						free(tiles);
						fprintf(stderr,"Shpfile::getPictureArray(): Cannot allocate memory!\n");
						exit(EXIT_FAILURE);
					}

					if(decode80(Fileheader + 10,DecodeDestination,size) == -1) {
						fprintf(stderr,"Warning: Checksum-Error in Shp-File\n");
					}

					shp_correct_lf(DecodeDestination,ImageOut, size);

					free(DecodeDestination);
				} break;

				case 1:
				{
					if( (DecodeDestination = (unsigned char*) calloc(1,size)) == NULL) {
						free(ImageOut);
						free(tiles);
						fprintf(stderr,"Shpfile::getPictureArray(): Cannot allocate memory!\n");
						exit(EXIT_FAILURE);
					}

					if(decode80(Fileheader + 10 + 16,DecodeDestination,size) == -1) {
						fprintf(stderr,"Warning: Checksum-Error in Shp-File\n");
					}

					shp_correct_lf(DecodeDestination, ImageOut, size);

					apply_pal_offsets(Fileheader + 10,ImageOut,sizeX*sizeY);

					free(DecodeDestination);
				} break;

				case 2:
				{
					shp_correct_lf(Fileheader+10, ImageOut,size);
				} break;

				case 3:
				{
					shp_correct_lf(Fileheader + 10 + 16, ImageOut,size);
					apply_pal_offsets(Fileheader + 10,ImageOut,sizeX*sizeY);
				} break;

				default:
				{
					fprintf(stderr,"Shpfile: Type %d in SHP-Files not supported!\n",type);
					exit(EXIT_FAILURE);
				}
			}

			//Now we can copy line by line
			switch(TILE_GETTYPE(tiles[i])) {
				case TILE_NORMAL:
				{
					for(int y = 0; y < sizeY; y++) {
						memcpy(	((char*) (pic->pixels)) + i*sizeX + (y+j*sizeY) * pic->pitch , ImageOut + y * sizeX, sizeX);
					}
				} break;

				case TILE_FLIPH:
				{
					for(int y = 0; y < sizeY; y++) {
						memcpy(	((char*) (pic->pixels)) + i*sizeX + (y+j*sizeY) * pic->pitch , ImageOut + (sizeY-1-y) * sizeX, sizeX);
					}
				} break;

				case TILE_FLIPV:
				{
					for(int y = 0; y < sizeY; y++) {
						for(int x = 0; x < sizeX; x++) {
							*(((char*) (pic->pixels)) + i*sizeX + (y+j*sizeY) * pic->pitch + x) = *(ImageOut + y * sizeX + (sizeX-1-x));
						}
					}
				} break;

				case TILE_ROTATE:
				{
					for(int y = 0; y < sizeY; y++) {
						for(int x = 0; x < sizeX; x++) {
							*(((char*) (pic->pixels)) + i*sizeX + (y+j*sizeY) * pic->pitch + x) = *(ImageOut + (sizeY-1-y) * sizeX + (sizeX-1-x));
						}
					}
				} break;

				default:
				{
					fprintf(stderr,"Shpfile: Invalid type for this parameter. Must be one of TILE_NORMAL, TILE_FLIPH, TILE_FLIPV or TILE_ROTATE!\n");
					exit(EXIT_FAILURE);
				} break;
			}

			free(ImageOut);
		}
	}

	free(tiles);

	SDL_UnlockSurface(pic);
	return pic;
}

/// Returns an animation
/**
	This method returns a new animation object with all pictures from startindex to endindex
	in it. The returned pointer should be freed with delete if no longer needed. If an error
	occured, NULL is returned.
	\param  startindex	index of the first picture
	\param  endindex	index of the last picture
	\param	DoublePic	if true, the picture is scaled up by a factor of 2
	\param	SetColorKey	if true, black is set as transparency
	\return	a new animation object or NULL on error
*/
Animation* Shpfile::getAnimation(unsigned int startindex,unsigned int endindex, bool DoublePic, bool SetColorKey)
{
	Animation* tmpAnimation;
	SDL_Surface* tmp;

	if((tmpAnimation = new Animation()) == NULL) {
		return NULL;
	}

	for(unsigned int i = startindex; i <= endindex; i++) {
		if((tmp = getPicture(i)) == NULL) {
			delete tmpAnimation;
			return NULL;
		}
		tmpAnimation->addFrame(tmp,DoublePic,SetColorKey);
	}
	return tmpAnimation;
}

/// Helper method for reading the index
/**
	This helper method reads the index of this shp-File.
*/
void Shpfile::readIndex()
{
	// First get number of files in shp-file
	uint16_t NumFiles = SDL_SwapLE16( ((Uint16*) Filedata)[0]);

	if(NumFiles == 0) {
		throw std::runtime_error("Shpfile::readIndex(): There is no file in this shp-File!");
	}

	if(NumFiles == 1) {
		/* files with only one image might be different */

		ShpfileEntry newShpfileEntry;
		if (((Uint16*) Filedata)[2] != 0) {
			/* File has special header with only 2 byte offset */
			newShpfileEntry.StartOffset = ((Uint32) SDL_SwapLE16(((Uint16*) Filedata)[1]));
			newShpfileEntry.EndOffset = ((Uint32) SDL_SwapLE16(((Uint16*) Filedata)[2])) - 1;
		} else {
			/* File has normal 4 byte offsets */
			newShpfileEntry.StartOffset = ((Uint32) SDL_SwapLE32(*((Uint32*) (Filedata+2)))) + 2;
			newShpfileEntry.EndOffset = ((Uint32) SDL_SwapLE16(((Uint16*) Filedata)[3])) - 1 + 2;
		}

		ShpfileEntries.push_back(newShpfileEntry);

	} else {
		/* File contains more than one image */

		if (((Uint16*) Filedata)[2] != 0) {
			/* File has special header with only 2 byte offset */

			if( ShpFilesize < (Uint32) ((NumFiles * 2) + 2 + 2)) {
				throw std::runtime_error("Shpfile::readIndex(): Shp-File-Header is not complete! Header too small!");
			}

			// now fill Index with start and end-offsets
			for(int i = 0; i < NumFiles; i++) {
			    ShpfileEntry newShpfileEntry;
				newShpfileEntry.StartOffset = SDL_SwapLE16( ((Uint16*)(Filedata+2))[i]);

				if(ShpfileEntries.empty() == false) {
					ShpfileEntries.back().EndOffset = newShpfileEntry.StartOffset - 1;

					if(newShpfileEntry.StartOffset >= ShpFilesize) {
						throw std::runtime_error("Shpfile::readIndex(): Entry in this SHP-File is beyond the end of this file!");
					}
				}

				ShpfileEntries.push_back(newShpfileEntry);
			}

			// Add the EndOffset for the last file
			ShpfileEntries.back().EndOffset = ((Uint32) SDL_SwapLE16( *((Uint16*) (Filedata + 2 + (NumFiles * 2))))) - 1 + 2;
		} else {
			/* File has normal 4 byte offsets */

			if( ShpFilesize < (Uint32) ((NumFiles * 4) + 2 + 2)) {
				throw std::runtime_error("Shpfile::readIndex(): Shp-File-Header is not complete! Header too small!");
			}

			// now fill Index with start and end-offsets
			for(int i = 0; i < NumFiles; i++) {
                ShpfileEntry newShpfileEntry;
				newShpfileEntry.StartOffset = SDL_SwapLE32( ((Uint32*)(Filedata+2))[i]) + 2;

				if(ShpfileEntries.empty() == false) {
					ShpfileEntries.back().EndOffset = newShpfileEntry.StartOffset - 1;

					if(newShpfileEntry.StartOffset >= ShpFilesize) {
						throw std::runtime_error("Shpfile::readIndex(): Entry in this SHP-File is beyond the end of this file!");
					}
				}

				ShpfileEntries.push_back(newShpfileEntry);
			}

			// Add the EndOffset for the last file
			ShpfileEntries.back().EndOffset = ((Uint32) SDL_SwapLE16( *((Uint16*) (Filedata + 2 + (NumFiles * 4))))) - 1 + 2;
		}
	}
}

/// Helper method for correcting the decoded picture.
/**
	This helper method corrects the decoded picture.
	\param	in	input picture
	\param	out	output picture
	\param	size	size of the input picture
*/
void Shpfile::shp_correct_lf(unsigned char *in, unsigned char *out, int size)
{
	unsigned char *end = in + size;
	while (in < end) {
		unsigned char val = *in;
		in++;

		if (val != 0) {
			*out = val;
			out++;
		} else {
			unsigned char count;
			count = *in;
			in++;
			if (count == 0) {
				return;
			}
			memset(out, 0, count);

			out += count;
		}
	}
}

/// Helper method for correcting the palette of the decoded picture.
/**
	This helper method corrects the palette of the decoded picture.
	\param	offsets	A lookup-table for correcting the palette
	\param	data	the picture to be corrected
	\param	length	size of the picture
*/
void Shpfile::apply_pal_offsets(unsigned char *offsets, unsigned char *data,unsigned int length)
{
	unsigned int i;
	for (i = 0; i < length; i ++)
		data[i] = offsets[data[i]];
}
