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

#include <FileClasses/Wsafile.h>
#include <FileClasses/Decode.h>
#include <FileClasses/Palette.h>

#include <SDL_endian.h>
#include <stdlib.h>
#include <string.h>

extern Palette palette;

/// Constructor
/**
	The constructor reads from the RWop all data and saves them internal. The SDL_RWops can be readonly but must support
	seeking. Immediately after the Wsafile-Object is constructed RWop can be closed. All data is saved in the class.
	\param	RWop	SDL_RWops to the wsa-File. (can be readonly)
*/
Wsafile::Wsafile(SDL_RWops* RWop)
{
	readdata(1,RWop);
}

/// Constructor
/**
	The constructor reads from the RWops all data and saves them internal. The SDL_RWops can be readonly but must support
	seeking. Immediately after the Wsafile-Object is constructed both RWops can be closed. All data is saved in the class.
	Both animations are concatinated.
	\param	RWop0	SDL_RWops for the first wsa-File. (can be readonly)
	\param	RWop1	SDL_RWops for the second wsa-File. (can be readonly)
*/
Wsafile::Wsafile(SDL_RWops* RWop0, SDL_RWops* RWop1)
{
	readdata(2,RWop0,RWop1);
}

/// Constructor
/**
	The constructor reads from the RWops all data and saves them internal. The SDL_RWops can be readonly but must support
	seeking. Immediately after the Wsafile-Object is constructed both RWops can be closed. All data is saved in the class.
	All three animations are concatinated.
	\param	RWop0	SDL_RWops for the first wsa-File. (can be readonly)
	\param	RWop1	SDL_RWops for the second wsa-File. (can be readonly)
	\param	RWop2	SDL_RWops for the third wsa-File. (can be readonly)
*/
Wsafile::Wsafile(SDL_RWops* RWop0, SDL_RWops* RWop1, SDL_RWops* RWop2)
{
	readdata(3,RWop0,RWop1,RWop2);
}

/// Constructor
/**
	The constructor reads from the RWops all data and saves them internal. The SDL_RWops can be readonly but must support
	seeking. Immediately after the Wsafile-Object is constructed both RWops can be closed. All data is saved in the class.
	All four animations are concatinated.
	\param	RWop0	SDL_RWops for the first wsa-File. (can be readonly)
	\param	RWop1	SDL_RWops for the second wsa-File. (can be readonly)
	\param	RWop2	SDL_RWops for the third wsa-File. (can be readonly)
	\param	RWop3	SDL_RWops for the forth wsa-File. (can be readonly)
*/
Wsafile::Wsafile(SDL_RWops* RWop0, SDL_RWops* RWop1, SDL_RWops* RWop2, SDL_RWops* RWop3)
{
	readdata(4,RWop0,RWop1,RWop2,RWop3);
}

/// Constructor
/**
	The constructor reads from the RWops all data and saves them internal. The SDL_RWops can be readonly but must support
	seeking. Immediately after the Wsafile-Object is constructed both RWops can be closed. All data is saved in the class.
	All animations are concatinated.
	\param	num		Number of Files
	\param	...		SDL_RWops for each wsa-File. (can be readonly)
*/
Wsafile::Wsafile(int num,...) {
	va_list args;
	va_start(args,num);

	readdata(num,args);
	va_end(args);
}

/// Destructor
/**
	Frees all memory.
*/
Wsafile::~Wsafile()
{
	free(decodedFrames);
}

/// Returns a picture in this wsa-File
/**
	This method returns a SDL_Surface containing the nth frame of this animation.
	The returned SDL_Surface should be freed with SDL_FreeSurface() if no longer needed.
	\param	FrameNumber	specifies which frame to return (zero based)
	\return	nth frame in this animation
*/
SDL_Surface * Wsafile::getPicture(Uint32 FrameNumber)
{
	if(FrameNumber >= NumFrames) {
		return NULL;
	}

	SDL_Surface * pic;
	unsigned char * Image = decodedFrames + (FrameNumber * SizeX * SizeY);

	// create new picture surface
	if((pic = SDL_CreateRGBSurface(SDL_HWSURFACE,SizeX,SizeY,8,0,0,0,0))== NULL) {
		return NULL;
	}

	palette.applyToSurface(pic);
	SDL_LockSurface(pic);

	//Now we can copy line by line
	for(int y = 0; y < SizeY;y++) {
		memcpy(	((char*) (pic->pixels)) + y * pic->pitch , Image + y * SizeX, SizeX);
	}

	SDL_UnlockSurface(pic);
	return pic;
}

/// Returns a picture-row
/**
	This method returns a SDL_Surface containing the complete animation.
	The returned SDL_Surface should be freed with SDL_FreeSurface() if no longer needed.
	\return	the complete animation
*/
SDL_Surface * Wsafile::getAnimationAsPictureRow() {
	SDL_Surface * pic;

	// create new picture surface
	if((pic = SDL_CreateRGBSurface(SDL_HWSURFACE,SizeX*NumFrames,SizeY,8,0,0,0,0))== NULL) {
		return NULL;
	}

	palette.applyToSurface(pic);
	SDL_LockSurface(pic);

	for(int i = 0; i < NumFrames; i++) {
		unsigned char * Image = decodedFrames + (i * SizeX * SizeY);

		//Now we can copy this frame line by line
		for(int y = 0; y < SizeY;y++) {
			memcpy(	((char*) (pic->pixels)) + y * pic->pitch + i*SizeX, Image + y * SizeX, SizeX);
		}
	}

	SDL_UnlockSurface(pic);
	return pic;
}

/// Returns an animation
/**
	This method returns a new animation object with all pictures from startindex to endindex
	in it. The returned pointer should be freed with delete if no longer needed. If an error
	occured, NULL is returned.
	\param	startindex	index of the first picture
	\param	endindex	index of the last picture
	\param	DoublePic	if true, the picture is scaled up by a factor of 2
	\param	SetColorKey	if true, black is set as transparency
	\return	a new animation object or NULL on error
*/
Animation* Wsafile::getAnimation(unsigned int startindex, unsigned int endindex, bool DoublePic, bool SetColorKey)
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

/// Helper method to decode one frame
/**
	This helper method decodes one frame.
	\param	Filedata		Pointer to the data of this wsa-File
	\param	Index			Array with startoffsets
	\param	NumberOfFrames	Number of frames to decode
	\param	pDecodedFrames	memory to copy decoded frames to (must be x*y*NumberOfFrames bytes long)
	\param	x				x-dimension of one frame
	\param	y				y-dimension of one frame
*/
void Wsafile::decodeFrames(unsigned char* Filedata, Uint32* Index, int NumberOfFrames, unsigned char* pDecodedFrames, int x, int y)
{
	unsigned char *dec80;

	for(int i=0;i<NumberOfFrames;i++) {
		if( (dec80 = (unsigned char*) malloc(x*y*2)) == NULL) {
			fprintf(stderr, "Error: Unable to allocate memory for decoded WSA-Frames!\n");
			exit(EXIT_FAILURE);
		}

		decode80(Filedata + SDL_SwapLE32(Index[i]), dec80, 0);

		decode40(dec80, pDecodedFrames + i * x*y);

		free(dec80);

		if (i < NumberOfFrames - 1) {
			memcpy(pDecodedFrames + (i+1) * x*y, pDecodedFrames + i * x*y,x*y);
		}
	}
}

/// Helper method for reading the complete wsa-file into memory.
/**
	This method reads the complete file into memory. A pointer to this memory is returned and
	should be freed with free when no longer needed.

*/
unsigned char* Wsafile::readfile(SDL_RWops* RWop, int* filesize) {
	unsigned char* Filedata;
	int WsaFilesize;

	if(filesize == NULL) {
		fprintf(stderr, "Wsafile: filesize == NULL!\n");
		exit(EXIT_FAILURE);
	}

	if(RWop == NULL) {
		fprintf(stderr, "Wsafile: RWop == NULL!\n");
		exit(EXIT_FAILURE);
	}

	WsaFilesize = SDL_RWseek(RWop,0,SEEK_END);
	if(WsaFilesize <= 0) {
		fprintf(stderr,"Wsafile: Cannot determine size of this *.wsa-File!\n");
		exit(EXIT_FAILURE);
	}

	if(WsaFilesize < 10) {
		fprintf(stderr, "Wsafile: No valid WSA-File: File too small!\n");
		exit(EXIT_FAILURE);
	}

	if(SDL_RWseek(RWop,0,SEEK_SET) != 0) {
		fprintf(stderr,"Wsafile: Seeking in this *.wsa-File failed!\n");
		exit(EXIT_FAILURE);
	}

	if( (Filedata = (unsigned char*) malloc(WsaFilesize)) == NULL) {
		fprintf(stderr,"Wsafile: Allocating memory failed!\n");
		exit(EXIT_FAILURE);
	}

	if(SDL_RWread(RWop, Filedata, WsaFilesize, 1) != 1) {
		fprintf(stderr,"Wsafile: Reading this *.wsa-File failed!\n");
		exit(EXIT_FAILURE);
	}

	*filesize = WsaFilesize;
	return Filedata;
}


/// Helper method for reading and concatinating various WSA-Files.
/**
	This methods reads from the RWops all data and concatinates all the frames to one animation. The SDL_RWops
	can be readonly but must support seeking.
	\param	NumFiles	Number of SDL_RWops
	\param	...			SDL_RWops for each wsa-File. (can be readonly)
*/
void Wsafile::readdata(int NumFiles, ...) {
	va_list args;
	va_start(args,NumFiles);
	readdata(NumFiles,args);
	va_end(args);
}

/// Helper method for reading and concatinating various WSA-Files.
/**
	This methods reads from the RWops all data and concatinates all the frames to one animation. The SDL_RWops
	can be readonly but must support seeking.
	\param	NumFiles	Number of SDL_RWops
	\param	args		SDL_RWops for each wsa-File should be in this va_list. (can be readonly)
*/
void Wsafile::readdata(int NumFiles, va_list args) {
	unsigned char** Filedata;
	Uint32** Index;
	Uint16* NumberOfFrames;
	bool* extended;

	if((Filedata = (unsigned char**) malloc(sizeof(unsigned char*) * NumFiles)) == NULL) {
		fprintf(stderr, "Wsafile::readdata(): Unable to allocate memory!\n");
		exit(EXIT_FAILURE);
	}

	if((Index = (Uint32**) malloc(sizeof(Uint32*) * NumFiles)) == NULL) {
		fprintf(stderr, "Wsafile::readdata(): Unable to allocate memory!\n");
		exit(EXIT_FAILURE);
	}

	if((NumberOfFrames = (Uint16*) malloc(sizeof(Uint16) * NumFiles)) == NULL) {
		fprintf(stderr, "Wsafile::readdata(): Unable to allocate memory!\n");
		exit(EXIT_FAILURE);
	}

	if((extended = (bool*) malloc(sizeof(bool) * NumFiles)) == NULL) {
		fprintf(stderr, "Wsafile::readdata(): Unable to allocate memory!\n");
		exit(EXIT_FAILURE);
	}

	NumFrames = 0;
	looped = false;

	for(int i = 0; i < NumFiles; i++) {
		SDL_RWops* RWop;
		int WsaFilesize;
		RWop = va_arg(args,SDL_RWops*);
		Filedata[i] = readfile(RWop,&WsaFilesize);
		NumberOfFrames[i] = SDL_SwapLE16(*((Uint16*) Filedata[i]) );

		if(i == 0) {
			SizeX = SDL_SwapLE16(*((Uint16*) (Filedata[0] + 2)) );
			SizeY = SDL_SwapLE16(*((Uint16*) (Filedata[0] + 4)) );
			FpsTimes1024ms = SDL_SwapLE16( *((Uint16*) (Filedata[0]+6)) );
		} else {
			if( (SizeX != (SDL_SwapLE16(*((Uint16*) (Filedata[i] + 2)) )))
				|| (SizeY != (SDL_SwapLE16(*((Uint16*) (Filedata[i] + 4)) )))) {
				fprintf(stderr, "Wsafile: The wsa-files have different picture dimensions. Cannot concatinate them!\n");
				exit(EXIT_FAILURE);
			}

            /*
			if(FpsTimes1024ms != SDL_SwapLE32( *((Uint32*) (Filedata[i]+6)) )) {
				Uint32 NewFpsTimes1024ms = SDL_SwapLE32( *((Uint32*) (Filedata[i]+6)) );
				fprintf(stderr, "Wsafile: Warning! The wsa-files have different framerates. 1. File: fps=%d*1024ms=%f, %d. File: fps=%d*1024ms=%f\n",FpsTimes1024ms, FpsTimes1024ms/1024.0,i+1, NewFpsTimes1024ms,NewFpsTimes1024ms/1024.0);
			}
			*/
		}

		if( ((unsigned short *) Filedata[i])[6] == 0) {
			Index[i] = (Uint32 *) (Filedata[i] + 10);
		} else {
			Index[i] = (Uint32 *) (Filedata[i] + 8);
		}

		if(Index[i][0] == 0) {
			// extended animation
			if(i == 0) {
				fprintf(stderr,"Extended WSA-File!\n");
			}
			Index[i]++;
			NumberOfFrames[i]--;
			extended[i] = true;
		} else {
			extended[i] = false;
		}

		if(i == 0) {
			if(Index[0][NumberOfFrames[0]+1] == 0) {
				// Index[NumberOfFrames[0]] point to end of file
				// => no loop
				looped = false;
			} else {
				// Index[NumberOfFrames[0]] point to loop frame
				// => looped animation
				//	fprintf(stderr,"Looped WSA-File!\n");
				looped = true;
			}
		}

		if(Filedata[i] + WsaFilesize < (((unsigned char *) Index[i]) + sizeof(Uint32) * NumberOfFrames[i])) {
			fprintf(stderr, "Wsafile: No valid WSA-File: File too small!\n");
			exit(EXIT_FAILURE);
		}

		NumFrames += NumberOfFrames[i];
	}


	if( (decodedFrames = (unsigned char*) calloc(1,SizeX*SizeY*NumFrames)) == NULL) {
		fprintf(stderr, "Wsafile: Unable to allocate memory for decoded WSA-Frames!\n");
		exit(EXIT_FAILURE);
	}

	decodeFrames(Filedata[0],Index[0],NumberOfFrames[0],decodedFrames,SizeX,SizeY);
	unsigned char* nextFreeFrame = decodedFrames + (NumberOfFrames[0] * SizeX * SizeY);
	free(Filedata[0]);

	for(int i = 1 ; i < NumFiles; i++) {
		if(extended[i]) {
			// copy last frame
			memcpy(nextFreeFrame,nextFreeFrame - (SizeX*SizeY),SizeX*SizeY);
		}
		decodeFrames(Filedata[i],Index[i],NumberOfFrames[i],nextFreeFrame,SizeX,SizeY);
		nextFreeFrame += NumberOfFrames[i] * SizeX * SizeY;
		free(Filedata[i]);
	}

	free(Filedata);
	free(NumberOfFrames);
	free(Index);
	free(extended);
}
