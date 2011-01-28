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

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <list>
#include <stdint.h>

class FileInfo {
public:
    FileInfo(std::string _name, uint32_t _size, uint32_t _modifydate)
        : name(_name), size(_size), modifydate(_modifydate) {
    }

    std::string name;
    uint32_t size;
    uint32_t modifydate;
};

enum FileListOrder {
    FileListOrder_Unsorted = 0,
    FileListOrder_Name_Asc,
    FileListOrder_Name_CaseInsensitive_Asc,
    FileListOrder_Name_Dsc,
    FileListOrder_Name_CaseInsensitive_Dsc,
    FileListOrder_Size_Asc,
    FileListOrder_Size_Dsc,
    FileListOrder_ModifyDate_Asc,
    FileListOrder_ModifyDate_Dsc
};

/**
	This function finds all the files in the specified directory with the specified
	extension.
	\param	directory	the directory name
	\param	extension	the extension to search for
	\param	IgnoreCase	true = extension comparison is case insensitive
	\return	a list of all the files with the specified extension
*/
std::list<std::string> GetFileNamesList(std::string directory, std::string extension, bool IgnoreCase = false, FileListOrder fileListOrder = FileListOrder_Unsorted);


/**
	This function finds all the files in the specified directory with the specified
	extension.
	\param	directory	the directory name
	\param	extension	the extension to search for
	\param	IgnoreCase	true = extension comparison is case insensitive
	\return	a list of all the files with the specified extension
*/
std::list<FileInfo> GetFileList(std::string directory, std::string extension, bool IgnoreCase = false, FileListOrder fileListOrder = FileListOrder_Unsorted);

/**
    This function is used to determine a case insensitive filename. The parameter filepath specifies the complete path to the file (relative or absolute).
    The path components are treated case sensitive to determine the directory to search the file in. The filename component of filepath is then compared
    case insensitive with all entries in this directory. If there is one entry found which matches (case insensitive), filepath is changed to point to this file
    (e.g. "data/DUNE.PAK" will become "data/dune.PAK" if there is a "dune.PAK" file in the "data" directory).
    \param  filepath    the path to the file to look for. This string is changed if there is file found with the same name (case insensitive compared)
    \return true if a file was found and filepath was changed accordingly. false if there was no file found or the directory was not found
*/
bool GetCaseInsensitiveFilename(std::string& filepath);

/**
	This function tests if a file exists
	\param path	path to the file
	\return true if it exists, false if not
*/
bool ExistsFile(const std::string& path);

#endif //FILESYSTEM_H
