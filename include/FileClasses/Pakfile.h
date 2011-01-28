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

#ifndef	PAKFILE_H
#define	PAKFILE_H

#include <stdio.h>
#include <SDL_rwops.h>
#include <string>
#include <vector>
#include <inttypes.h>

#define PAKFILE_RWOP_TYPE	0x9A5F17EC

///	A class for reading PAK-Files.
/**
	This class can be used to read PAK-Files. PAK-Files are archive files used by Dune2.
	The files inside the PAK-File can an be read through SDL_RWops.
*/
class Pakfile
{
private:

	/// Internal structure for representing one file in this PAK-File
	struct PakFileEntry {
		uint32_t StartOffset;
		uint32_t EndOffset;
		std::string Filename;
	};

	/// Internal structure used by opened SDL_RWop
	struct RWopData {
		Pakfile* curPakfile;
		unsigned int FileIndex;
		uint32_t FileOffset;
	};

public:
	Pakfile(std::string Pakfilename, bool write = false);
	~Pakfile();

	std::string GetFilename(unsigned int index);

	/// Number of files in this pak-File.
	/**
		Returns the number of files in this pak-File.
		\return	Number of files.
	*/
	inline int GetNumFiles() { return FileEntries.size(); };

	SDL_RWops* OpenFile(std::string filename);

	bool exists(std::string filename);

	void AddFile(SDL_RWops* rwop, std::string filename);

private:
	static int ReadFile(SDL_RWops* pRWop, void *ptr, int size, int n);
	static int WriteFile(SDL_RWops *pRWop, const void *ptr, int size, int n);
	static int SeekFile(SDL_RWops *pRWop, int offset, int whence);
	static int CloseFile(SDL_RWops *pRWop);

	void ReadIndex();

	bool write;
	SDL_RWops * fPakFile;
	std::string Filename;

	char* WriteOutData;
	int	NumWriteOutData;
	std::vector<PakFileEntry> FileEntries;
};

#endif // PAKFILE_H
