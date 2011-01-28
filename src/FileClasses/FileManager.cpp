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

#include <FileClasses/FileManager.h>

#include <globals.h>

#include <misc/FileSystem.h>

#include <config.h>
#include <misc/fnkdat.h>
#include <misc/md5.h>

#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <iomanip>

FileManager::FileManager(bool saveMode) {
    fprintf(stderr,"\n");
    fprintf(stderr,"FileManager is loading PAK-Files...\n\n");
    fprintf(stderr,"MD5-Checksum                      Filename\n");

    std::vector<std::string> searchPath = getSearchPath();
    std::vector<std::string> FileList = getNeededFiles(settings.General.Language);

    std::vector<std::string>::const_iterator filenameIter;
    for(filenameIter = FileList.begin(); filenameIter != FileList.end(); ++filenameIter) {

        std::vector<std::string>::const_iterator searchPathIter;
        for(searchPathIter = searchPath.begin(); searchPathIter != searchPath.end(); ++searchPathIter) {
            std::string filepath = *searchPathIter + "/" + *filenameIter;
            if(GetCaseInsensitiveFilename(filepath) == true) {
                try {
                    fprintf(stderr,"%s  %s\n", md5FromFilename(filepath).c_str(), filepath.c_str());
                    PakFiles.push_back(new Pakfile(filepath));
                } catch (std::exception &e) {
                    if(saveMode == false) {
                        while(PakFiles.empty()) {
                            delete PakFiles.back();
                            PakFiles.pop_back();
                        }

                        throw std::runtime_error("FileManager::FileManager(): Error while opening " + filepath + ": " + e.what());
                    }
                }

                // break out of searchPath-loop because we have opened the file in one directory
                break;
            }
        }

    }

    fprintf(stderr,"\n");
}

FileManager::~FileManager() {
    std::vector<Pakfile*>::const_iterator iter;
    for(iter = PakFiles.begin(); iter != PakFiles.end(); ++iter) {
        delete *iter;
    }
}

std::vector<std::string> FileManager::getSearchPath() {
    std::vector<std::string> searchPath;

    searchPath.push_back(DUNELEGACY_DATADIR);
    char tmp[FILENAME_MAX];
	fnkdat("data", tmp, FILENAME_MAX, FNKDAT_USER | FNKDAT_CREAT);
    searchPath.push_back(tmp);

    return searchPath;
}

std::vector<std::string> FileManager::getNeededFiles(int Language) {
    std::vector<std::string> FileList;

    FileList.push_back("LEGACY.PAK");
    FileList.push_back("DUNE.PAK");
    FileList.push_back("SCENARIO.PAK");
    FileList.push_back("MENTAT.PAK");
    FileList.push_back("VOC.PAK");
    FileList.push_back("MERC.PAK");
    FileList.push_back("FINALE.PAK");
    FileList.push_back("INTRO.PAK");
    FileList.push_back("INTROVOC.PAK");
    FileList.push_back("SOUND.PAK");

    switch(Language) {
        case LNG_ENG: {
            FileList.push_back("ENGLISH.PAK");
            FileList.push_back("HARK.PAK");
            FileList.push_back("ATRE.PAK");
            FileList.push_back("ORDOS.PAK");
        } break;

        case LNG_FRE: {
            FileList.push_back("FRENCH.PAK");
        } break;

        case LNG_GER: {
            FileList.push_back("GERMAN.PAK");
        } break;

        default: {
            throw std::runtime_error("FileManager::getNeededFiles(): Unknown language!");
        } break;
    }

    std::sort(FileList.begin(), FileList.end());

    return FileList;
}

std::vector<std::string> FileManager::getMissingFiles(int Language) {
    std::vector<std::string> MissingFiles;
    std::vector<std::string> searchPath = getSearchPath();
    std::vector<std::string> FileList = getNeededFiles(Language);

    std::vector<std::string>::const_iterator filenameIter;
    for(filenameIter = FileList.begin(); filenameIter != FileList.end(); ++filenameIter) {
        bool bFound = false;

        std::vector<std::string>::const_iterator searchPathIter;
        for(searchPathIter = searchPath.begin(); searchPathIter != searchPath.end(); ++searchPathIter) {
            std::string filepath = *searchPathIter + "/" + *filenameIter;
            if(GetCaseInsensitiveFilename(filepath) == true) {
                bFound = true;
                break;
            }
        }

        if(bFound == false) {
            MissingFiles.push_back(*filenameIter);
        }
    }

    return MissingFiles;
}

SDL_RWops* FileManager::OpenFile(std::string Filename) {
	SDL_RWops* ret;

    // try loading external file
    std::vector<std::string> searchPath = getSearchPath();
    std::vector<std::string>::const_iterator searchPathIter;
    for(searchPathIter = searchPath.begin(); searchPathIter != searchPath.end(); ++searchPathIter) {

        std::string externalFilename = *searchPathIter + "/" + Filename;
        if(GetCaseInsensitiveFilename(externalFilename) == true) {
            if((ret = SDL_RWFromFile(externalFilename.c_str(), "rb")) != NULL) {
                return ret;
            }
        }
    }

    // now try loading from pak file
    std::vector<Pakfile*>::const_iterator iter;
    for(iter = PakFiles.begin(); iter != PakFiles.end(); ++iter) {
        ret = (*iter)->OpenFile(Filename);
		if(ret != NULL) {
			return ret;
		}
    }

    throw std::runtime_error("FileManager::OpenFile(): Cannot find " + Filename + "!");
}

bool FileManager::exists(std::string Filename) {

    // try finding external file
    std::vector<std::string> searchPath = getSearchPath();
    std::vector<std::string>::const_iterator searchPathIter;
    for(searchPathIter = searchPath.begin(); searchPathIter != searchPath.end(); ++searchPathIter) {

        std::string externalFilename = *searchPathIter + "/" + Filename;
        if(GetCaseInsensitiveFilename(externalFilename) == true) {
            return true;
        }
    }

    // now try finding in one pak file
    std::vector<Pakfile*>::const_iterator iter;
    for(iter = PakFiles.begin(); iter != PakFiles.end(); ++iter) {
        if((*iter)->exists(Filename) == true) {
            return true;
        }
    }

    return false;
}


std::string FileManager::md5FromFilename(std::string filename) {
	unsigned char md5sum[16];

	if(md5_file(filename.c_str(), md5sum) != 0) {
		throw std::runtime_error("Cannot open or read " + filename + "!");
	} else {

		std::stringstream stream;
		stream << std::setfill('0') << std::hex;
		for(int i=0;i<16;i++) {
			stream << std::setw(2) << (int) md5sum[i];
		}
		return stream.str();
	}
}
