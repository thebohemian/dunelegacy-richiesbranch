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

#include <misc/FileSystem.h>
#include <misc/string_util.h>

#include <stdio.h>
#include <algorithm>
#include <ctype.h>

#ifdef _WIN32
#include <io.h>
#include <direct.h>
#else
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#endif

std::list<std::string> GetFileNamesList(std::string directory, std::string extension, bool IgnoreCase, FileListOrder fileListOrder)
{
    std::list<FileInfo> Files = GetFileList(directory, extension, IgnoreCase, fileListOrder);

    std::list<std::string> FileNames;

    std::list<FileInfo>::iterator iter;
    for(iter = Files.begin(); iter != Files.end(); ++iter) {
        FileNames.push_back(iter->name);
    }

    return FileNames;
}

bool cmp_Name_Asc(FileInfo& a, FileInfo& b) { return (a.name.compare(b.name) < 0); }
bool cmp_Name_CaseInsensitive_Asc(FileInfo& a, FileInfo& b) {
  unsigned int i=0;
  while((i < a.name.length()) && (i < b.name.length())) {
    if(tolower(a.name[i]) < tolower(b.name[i])) {
        return true;
    } else if (tolower(a.name[i]) > tolower(b.name[i])) {
        return false;
    }
    i++;
  }

  return (a.name.length() < b.name.length());
}

bool cmp_Name_Dsc(FileInfo& a, FileInfo& b) { return (a.name.compare(b.name) > 0); }
bool cmp_Name_CaseInsensitive_Dsc(FileInfo& a, FileInfo& b) {
  unsigned int i=0;
  while((i < a.name.length()) && (i < b.name.length())) {
    if(tolower(a.name[i]) < tolower(b.name[i])) {
        return false;
    } else if (tolower(a.name[i]) > tolower(b.name[i])) {
        return true;
    }
    i++;
  }

  return (a.name.length() > b.name.length());
}

bool cmp_Size_Asc(FileInfo& a, FileInfo& b) { return a.size < b.size; }
bool cmp_Size_Dsc(FileInfo& a, FileInfo& b) { return a.size > b.size; }

bool cmp_ModifyDate_Asc(FileInfo& a, FileInfo& b) { return a.modifydate < b.modifydate; }
bool cmp_ModifyDate_Dsc(FileInfo& a, FileInfo& b) { return a.modifydate > b.modifydate; }

std::list<FileInfo> GetFileList(std::string directory, std::string extension, bool IgnoreCase, FileListOrder fileListOrder)
{

	std::list<FileInfo> Files;

	if(IgnoreCase == true) {
		convertToLower(extension);
	}

#ifdef _WIN32

	long hFile;

	_finddata_t fdata;

	std::string searchString = directory + "*";

	if ((hFile = (long)_findfirst(searchString.c_str(), &fdata)) != -1L) {
		do {
			std::string Filename = fdata.name;
			unsigned int dotposition = Filename.find_last_of('.');

			if(dotposition == std::string::npos) {
				continue;
			}

			std::string ext = Filename.substr(dotposition+1);

			if(IgnoreCase == true) {
				convertToLower(ext);
            }

			if(ext == extension) {
				Files.push_back(FileInfo(Filename, fdata.size, fdata.time_write));
			}
		} while(_findnext(hFile, &fdata) == 0);

		_findclose(hFile);
	}

#else

	DIR * dir = opendir(directory.c_str());
	dirent *curEntry;

	if(dir == NULL) {
		return Files;
	}

	errno = 0;
	while((curEntry = readdir(dir)) != NULL) {
			std::string Filename = curEntry->d_name;
			size_t dotposition = Filename.find_last_of('.');

			if(dotposition == std::string::npos) {
				continue;
			}

			std::string ext = Filename.substr(dotposition+1);

			if(IgnoreCase == true) {
				convertToLower(ext);
			}

			if(ext == extension) {
			    std::string fullpath = directory + "/" + Filename;
			    struct stat fdata;
			    if(stat(fullpath.c_str(), &fdata) != 0) {
                    perror("stat()");
                    continue;
			    }
				Files.push_back(FileInfo(Filename, fdata.st_size, fdata.st_mtime));
			}
	}

	if(errno != 0) {
		perror("readdir()");
	}

	closedir(dir);

#endif

    switch(fileListOrder) {
        case FileListOrder_Name_Asc: {
            Files.sort(cmp_Name_Asc);
        } break;

        case FileListOrder_Name_CaseInsensitive_Asc: {
            Files.sort(cmp_Name_CaseInsensitive_Asc);
        } break;

        case FileListOrder_Name_Dsc: {
            Files.sort(cmp_Name_Dsc);
        } break;

        case FileListOrder_Name_CaseInsensitive_Dsc: {
            Files.sort(cmp_Name_CaseInsensitive_Dsc);
        } break;

        case FileListOrder_Size_Asc: {
            Files.sort(cmp_Size_Asc);
        } break;

        case FileListOrder_Size_Dsc: {
            Files.sort(cmp_Size_Dsc);
        } break;

        case FileListOrder_ModifyDate_Asc: {
            Files.sort(cmp_ModifyDate_Asc);
        } break;

        case FileListOrder_ModifyDate_Dsc: {
            Files.sort(cmp_ModifyDate_Dsc);
        } break;

        case FileListOrder_Unsorted:
        default: {
            // do nothing
        } break;
    }

	return Files;

}

bool GetCaseInsensitiveFilename(std::string& filepath) {

#ifdef _WIN32
    return ExistsFile(filepath);

#else


    std::string filename;
    std::string path;
    size_t separatorPos = filepath.rfind('/');
    if(separatorPos == std::string::npos) {
        // There is no '/' in the filepath => only filename in current working directory specified
        filename = filepath;
        path = ".";
    } else if(separatorPos == filepath.length()-1) {
        // filepath has an '/' at the end => no filename specified
        return false;
    } else {
        filename = filepath.substr(separatorPos+1, std::string::npos);
        path = filepath.substr(0,separatorPos+1); // path with tailing '/'
    }

    DIR* directory = opendir(path.c_str());
    if(directory == NULL) {
        return false;
    }

    while(true) {
        errno = 0;
        dirent* directory_entry = readdir(directory);
        if(directory_entry == NULL) {
            if (errno != 0) {
                closedir(directory);
                return false;
            } else {
                // EOF
                break;
            }
        }

        bool entry_OK = true;
        const char* pEntryName = directory_entry->d_name;
        const char* pFilename = filename.c_str();
        while(true) {
            if((*pEntryName == '\0') && (*pFilename == '\0')) {
                break;
            }

            if(tolower(*pEntryName) != tolower(*pFilename)) {
                entry_OK = false;
                break;
            }
            pEntryName++;
            pFilename++;
        }

        if(entry_OK == true) {
            if(path == ".") {
                filepath = directory_entry->d_name;
            } else {
                filepath = path + directory_entry->d_name;
            }
            closedir(directory);
            return true;
        }
    }
    closedir(directory);
    return false;

#endif

}


bool ExistsFile(const std::string& path) {
	// try opening the file
	FILE* fp = fopen(path.c_str(),"r");

	if(fp == NULL) {
		return false;
	}

	fclose(fp);
	return true;
}
