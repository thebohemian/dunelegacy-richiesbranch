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

#ifndef WSAFILE_H
#define WSAFILE_H

#include "Animation.h"
#include <SDL.h>
#include <SDL_rwops.h>
#include <stdarg.h>

/// A class for loading a *.WSA-File.
/**
	This class can read the animation in a *.WSA-File and return it as SDL_Surfaces.
*/
class Wsafile
{
public:
	Wsafile(SDL_RWops* RWop);
	Wsafile(SDL_RWops* RWop0, SDL_RWops* RWop1);
	Wsafile(SDL_RWops* RWop0, SDL_RWops* RWop1, SDL_RWops* RWop2);
	Wsafile(SDL_RWops* RWop0, SDL_RWops* RWop1, SDL_RWops* RWop2, SDL_RWops* RWop3);
	Wsafile(int num,...);
	virtual ~Wsafile();

	SDL_Surface * getPicture(Uint32 FrameNumber);
	SDL_Surface * getAnimationAsPictureRow();
	Animation* getAnimation(unsigned int startindex, unsigned int endindex, bool DoublePic=true, bool SetColorKey=true);

	/// Returns the number of frames
	/**
		This method returns the number of frames in this animation
		\return Number of frames.
	*/
	inline int getNumFrames() const { return (int) NumFrames; };

	/// Returns the framerate*1024
	/**
		This method returns the number of (frames per second) * 1024ms (That's how it is saved in the wsa-File; as a fixed-point number).
		\return	Framerate*1024 of this animation.
	*/
	inline Uint32 getFpsTimes1024ms() const { return FpsTimes1024ms; };

	/// Returns the frames per second
	/**
		This method returns the frames per second this movie should be played.
		\return frames per second
	*/
	inline double getFps() const { return FpsTimes1024ms/1024.0; };

	/// Returns whether the animation is looped or not.
	/**
		This method returns whether this animation is looped or not.
		\return	true if looped, false if not
	*/
	inline bool	IsAnimationLooped() const { return looped; };

private:
	void decodeFrames(unsigned char* Filedata, Uint32* Index, int NumberOfFrames, unsigned char* pDecodedFrames, int x, int y);
	unsigned char* readfile(SDL_RWops* RWop, int* filesize);
	void readdata(int NumFiles, ...);
	void readdata(int NumFiles, va_list args);
	unsigned char *decodedFrames;

	Uint16 NumFrames;
	Uint16 SizeX;
	Uint16 SizeY;
	Uint32 FpsTimes1024ms;
	bool   looped;
};

#endif // WSAFILE_H
