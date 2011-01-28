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

#include <FileClasses/INIFile.h>

#include <fstream>
#include <iostream>
#include <cctype>
#include <algorithm>
#include <stdio.h>


INIFile::INIFileLine::INIFileLine(const std::string& completeLine)
 : CompleteLine(completeLine), nextLine(NULL), prevLine(NULL) {
}

INIFile::Key::Key(const std::string& completeLine, int keystringbegin, int keystringlength, int valuestringbegin, int valuestringlength)
 :  INIFileLine(completeLine), KeyStringBegin(keystringbegin), KeyStringLength(keystringlength),
    ValueStringBegin(valuestringbegin), ValueStringLength(valuestringlength),
    nextKey(NULL), prevKey(NULL) {
}

INIFile::Key::Key(const std::string& keyname, const std::string& value)
 :  INIFileLine(keyname + " = " + escapeValue(value)), KeyStringBegin(0), KeyStringLength(keyname.size()),
    ValueStringBegin(keyname.size() + 3 + (escapingValueNeeded(value) ? 1 : 0)), ValueStringLength(value.size()),
    nextKey(NULL), prevKey(NULL) {
}

std::string INIFile::Key::getKeyName() const {
    return CompleteLine.substr(KeyStringBegin,KeyStringLength);
}

std::string INIFile::Key::getStringValue() const {
    return CompleteLine.substr(ValueStringBegin,ValueStringLength);
}

int INIFile::Key::getIntValue(int defaultValue) const {
    std::string value = getStringValue();
    if(value.size() == 0) {
        return defaultValue;
    }

    long ret;
    if(value.at(0) == '-') {
        ret = -(atol(value.c_str()+1));
    } else if (value.at(0) == '+') {
        ret = atol(value.c_str()+1);
    } else {
        ret = atol(value.c_str());
    }

    return ret;
}

bool INIFile::Key::getBoolValue(bool defaultValue) const {
    std::string value = getStringValue();
    if(value.size() == 0) {
        return defaultValue;
    }

    // convert string to lower case
    std::transform(value.begin(),value.end(), value.begin(), (int(*)(int)) tolower);

    if((value == "true") || (value == "enabled") || (value == "on") || (value == "1")) {
        return true;
    } else if((value == "false") || (value == "disabled") || (value == "off") || (value == "0")) {
        return false;
    } else {
        return defaultValue;
    }
}

double INIFile::Key::getDoubleValue(double defaultValue) const {
    std::string value = getStringValue();
    if(value.size() == 0) {
        return defaultValue;
    }

    double ret;
    ret = atof(value.c_str());

    return ret;
}

void INIFile::Key::setStringValue(const std::string& newValue) {
    if(CompleteLine[ValueStringBegin-1] == '"') {
        CompleteLine.replace(ValueStringBegin-1,ValueStringLength+2, escapeValue(newValue));
    } else {
        CompleteLine.replace(ValueStringBegin,ValueStringLength, escapeValue(newValue));
    }
}

void INIFile::Key::setIntValue(int newValue)
{
	char tmp[20];
	sprintf(tmp,"%d",newValue);
	setStringValue(tmp);
}

void INIFile::Key::setBoolValue(bool newValue)
{
	if(newValue == true) {
		setStringValue("true");
	} else {
		setStringValue("false");
	}
}

void INIFile::Key::setDoubleValue(double newValue)
{
	char tmp[30];
	sprintf(tmp,"%f",newValue);
	setStringValue(tmp);
}

bool INIFile::Key::escapingValueNeeded(const std::string& value) {
    if(value == "") {
        return true;
    } else {
        // test for non normal char
        for(unsigned int i = 0; i < value.size(); i++) {
            if(!isNormalChar(value[i])) {
                return true;
            }
        }
        return false;
    }
}

std::string INIFile::Key::escapeValue(const std::string& value) {
    if(escapingValueNeeded(value)) {
        return "\"" + value + "\"";
    } else {
        return value;
    }
}


INIFile::Section::Section(const std::string& completeLine, int sectionstringbegin, int sectionstringlength)
 :  INIFileLine(completeLine), SectionStringBegin(sectionstringbegin), SectionStringLength(sectionstringlength),
    nextSection(NULL), prevSection(NULL), KeyRoot(NULL) {
}

INIFile::Section::Section(const std::string& sectionname)
 :  INIFileLine("[" + sectionname + "]"), SectionStringBegin(1), SectionStringLength(sectionname.size()),
    nextSection(NULL), prevSection(NULL), KeyRoot(NULL) {
}

/// Get the name for this section
/**
    This method returns the name of this section
    \return name of this section
*/
std::string INIFile::Section::getSectionName() const {
    return CompleteLine.substr(SectionStringBegin, SectionStringLength);
}


/// Get a key iterator pointing at the first key in this section
/**
    This method returns a key iterator pointing at the first key in this section.
    \return the iterator
*/
INIFile::KeyIterator INIFile::Section::begin() const {
	return KeyIterator(KeyRoot);
}

/// Get a key iterator pointing past the last key in this section
/**
    This method returns a key iterator pointing past the last key in this section.
    \return the iterator
*/
INIFile::KeyIterator INIFile::Section::end() const {
	return KeyIterator();
}

/**
    This method checks whether the specified key exists in this section.
	\param  key             keyname
	\return true, if the key exists, false otherwise
*/
bool INIFile::Section::hasKey(const std::string& key) const
{
	return (getKey(key) != NULL);
}

INIFile::Key* INIFile::Section::getKey(const std::string& keyname) const {
    for(INIFile::KeyIterator iter = begin(); iter != end(); ++iter) {
        if(iter->KeyStringLength == (int) keyname.size()) {
				if(strncasecmp(keyname.c_str(), iter->CompleteLine.c_str()+iter->KeyStringBegin, keyname.size()) == 0) {
					return &(*iter);
				}
		}
    }

    return NULL;
}


void INIFile::Section::setStringValue(const std::string& key, const std::string& newValue) {
	if(hasKey(key)) {
	    getKey(key)->setStringValue(newValue);
	} else {
		// create new key
		if(isValidKeyName(key) == false) {
			std::cerr << "INIFile: Cannot create key with name " << key << "!" << std::endl;
			return;
		}

        Key* curKey = new Key(key, newValue);
        Key* pKey = KeyRoot;
        if(pKey == NULL) {
            // Section has no key yet
            if(nextLine == NULL) {
                // no line after this section declaration
                nextLine = curKey;
                curKey->prevLine = this;
            } else {
                // lines after this section declaration
                nextLine->prevLine = curKey;
                curKey->nextLine = nextLine;
                nextLine = curKey;
                curKey->prevLine = this;
            }
        } else {
            // Section already has some keys
            while(pKey->nextKey != NULL) {
                pKey = pKey->nextKey;
            }

            if(pKey->nextLine == NULL) {
                // no line after this key
                pKey->nextLine = curKey;
                curKey->prevLine = pKey;
            } else {
                // lines after this section declaration
                pKey->nextLine->prevLine = curKey;
                curKey->nextLine = pKey->nextLine;
                pKey->nextLine = curKey;
                curKey->prevLine = pKey;
            }
        }

		InsertKey(curKey);
	}
}

void INIFile::Section::setIntValue(const std::string& key, int newValue)
{
	char tmp[20];
	sprintf(tmp,"%d",newValue);
	setStringValue(key, tmp);
}

void INIFile::Section::setBoolValue(const std::string& key, bool newValue)
{
	if(newValue == true) {
		setStringValue(key, "true");
	} else {
		setStringValue(key, "false");
	}
}

void INIFile::Section::setDoubleValue(const std::string& key, double newValue)
{
	char tmp[30];
	sprintf(tmp,"%f",newValue);
	setStringValue(key, tmp);
}


void INIFile::Section::InsertKey(Key* newKey) {
	if(KeyRoot == NULL) {
		// New root element
		KeyRoot = newKey;
	} else {
		// insert into list
		Key* curKey = KeyRoot;
		while(curKey->nextKey != NULL) {
			curKey = curKey->nextKey;
		}

		curKey->nextKey = newKey;
		newKey->prevKey = curKey;
	}
}



// public methods

/// Constructor for reading the INI-File from a file.
/**
	This constructor reads the INI-File from the file specified by filename. The file opened in readonly-mode. After
	reading the file is closed immediately. If the file does not exist, it is treated as empty.
	\param	filename	The file to be opened.
*/
INIFile::INIFile(const std::string& filename)
{
	FirstLine = NULL;
	SectionRoot = NULL;
	SDL_RWops * file;

	// open file
	if((file = SDL_RWFromFile(filename.c_str(),"r")) != NULL) {
			readfile(file);
			SDL_RWclose(file);
	} else {
		SectionRoot = new Section("",0,0);
	}
}

/// Constructor for reading the INI-File from a SDL_RWops.
/**
	This constructor reads the INI-File from RWopsFile. The RWopsFile can be readonly.
	\param	RWopsFile	Pointer to RWopsFile (can be readonly)
*/
INIFile::INIFile(SDL_RWops * RWopsFile)
{
	FirstLine = NULL;
	SectionRoot = NULL;

	if(RWopsFile == NULL) {
		std::cerr << "INIFile: RWopsFile == NULL!" << std::endl;
		exit(EXIT_FAILURE);
	}

	readfile(RWopsFile);
}

///	Destructor.
/**
	This is the destructor. Changes to the INI-Files are not automaticly saved. Call INIFile::SaveChangesTo() for that purpose.
*/
INIFile::~INIFile()
{
	INIFileLine* curLine = FirstLine;
	INIFileLine* tmp;
	while(curLine != NULL) {
		tmp = curLine;
		curLine = curLine->nextLine;
		delete tmp;
	}

	// now we have to delete the "" section
	delete SectionRoot;
}


/**
    This method checks whether the specified section exists.
	\param	section			sectionname
	\return true, if the section exists, false otherwise
*/
bool INIFile::hasSection(const std::string& section) const
{
    return (getSection(section) != NULL);
}


/**
    This method checks whether the specified key exists in the specified section.
	\param	section			sectionname
	\param  key             keyname
	\return true, if the key exists, false otherwise
*/
bool INIFile::hasKey(const std::string& section, const std::string& key) const
{
	Section* curSection = getSection(section);
	if(curSection == NULL) {
		return false;
	} else {
        return curSection->hasKey(key);
	}
}


/// Reads the string that is adressed by the section/key pair.
/**
	Returns the value that is adressed by the section/key pair as a string. If the key could not be found in
	this section defaultValue is returned. If no defaultValue is specified then "" is returned.
	\param	section			sectionname
	\param	key				keyname
	\param	defaultValue	default value for defaultValue is ""
	\return	The read value or default
*/
std::string INIFile::getStringValue(const std::string& section, const std::string& key, const std::string& defaultValue) const
{
	Key* curKey = getKey(section,key);
	if(curKey == NULL) {
		return defaultValue;
	} else {
        return curKey->getStringValue();
	}
}


/// Reads the int that is adressed by the section/key pair.
/**
	Returns the value that is adressed by the section/key pair as a int. If the key could not be found in
	this section defaultValue is returned. If no defaultValue is specified then 0 is returned. If the value
	could not be converted to an int 0 is returned.
	\param	section			sectionname
	\param	key				keyname
	\param	defaultValue	default value for defaultValue is 0
	\return	The read number, defaultValue or 0
*/
int INIFile::getIntValue(const std::string& section, const std::string& key, int defaultValue) const
{
    Key* curKey = getKey(section,key);
	if(curKey == NULL) {
		return defaultValue;
	} else {
        return curKey->getIntValue(defaultValue);
	}
}

/// Reads the boolean that is adressed by the section/key pair.
/**
	Returns the value that is adressed by the section/key pair as a boolean. If the key could not be found in
	this section defaultValue is returned. If no defaultValue is specified then false is returned. If the value
	is one of "true", "enabled", "on" or "1" then true is returned; if it is one of "false", "disabled", "off" or
	"0" than false is returned; otherwise defaultValue is returned.
	\param	section			sectionname
	\param	key				keyname
	\param	defaultValue	default value for defaultValue is 0
	\return	true for "true", "enabled", "on" and "1"<br>false for "false", "disabled", "off" and "0"
*/
bool INIFile::getBoolValue(const std::string& section, const std::string& key, bool defaultValue) const
{
    Key* curKey = getKey(section,key);
	if(curKey == NULL) {
		return defaultValue;
	} else {
        return curKey->getBoolValue(defaultValue);
	}
}

/// Reads the double that is adressed by the section/key pair.
/**
	Returns the value that is adressed by the section/key pair as a double. If the key could not be found in
	this section defaultValue is returned. If no defaultValue is specified then 0.0 is returned. If the value
	could not be converted to an double 0.0 is returned.
	\param	section			sectionname
	\param	key				keyname
	\param	defaultValue	default value for defaultValue is 0.0
	\return	The read number, defaultValue or 0.0
*/
double INIFile::getDoubleValue(const std::string& section, const std::string& key, double defaultValue) const
{
    Key* curKey = getKey(section,key);
	if(curKey == NULL) {
		return defaultValue;
	} else {
        return curKey->getDoubleValue(defaultValue);
	}
}

/// Sets the string that is adressed by the section/key pair.
/**
	Sets the string that is adressed by the section/key pair to value. If the section and/or the key does not exist it will
	be created. A valid sectionname/keyname is not allowed to contain '[',']',';' or '#' and can not start or end with
	whitespaces (' ' or '\\t').
	\param	section			sectionname
	\param	key				keyname
	\param	value			value that should be set
*/
void INIFile::setStringValue(const std::string& section, const std::string& key, const std::string& value)
{
	Section* curSection = getSectionOrCreate(section);

	if(curSection == NULL) {
		std::cerr << "INIFile: Cannot create section with name " << section << "!" << std::endl;
        return;
    }

    curSection->setStringValue(key, value);
}

/// Sets the int that is adressed by the section/key pair.
/**
	Sets the int that is adressed by the section/key pair to value. If the section and/or the key does not exist it will
	be created. A valid sectionname/keyname is not allowed to contain '[',']',';' or '#' and can not start or end with
	whitespaces (' ' or '\\t').
	\param	section			sectionname
	\param	key				keyname
	\param	value			value that should be set
*/
void INIFile::setIntValue(const std::string& section, const std::string& key, int value)
{
	char tmp[20];
	sprintf(tmp,"%d",value);
	setStringValue(section, key, tmp);
}

/// Sets the boolean that is adressed by the section/key pair.
/**
	Sets the boolean that is adressed by the section/key pair to value. If the section and/or the key does not exist it will
	be created. A valid sectionname/keyname is not allowed to contain '[',']',';' or '#' and can not start or end with
	whitespaces (' ' or '\\t').
	\param	section			sectionname
	\param	key				keyname
	\param	value			value that should be set
*/
void INIFile::setBoolValue(const std::string& section, const std::string& key, bool value)
{
	if(value == true) {
		setStringValue(section, key, "true");
	} else {
		setStringValue(section, key, "false");
	}
}

/// Sets the double that is adressed by the section/key pair.
/**
	Sets the double that is adressed by the section/key pair to value. If the section and/or the key does not exist it will
	be created. A valid sectionname/keyname is not allowed to contain '[',']',';' or '#' and can not start or end with
	whitespaces (' ' or '\\t').
	\param	section			sectionname
	\param	key				keyname
	\param	value			value that should be set
*/
void INIFile::setDoubleValue(const std::string& section, const std::string& key, double value)
{
	char tmp[30];
	sprintf(tmp,"%f",value);
	setStringValue(section, key, tmp);
}


/// Get a section iterator pointing at the first section
/**
    This method returns a section iterator pointing at the first section (which is the anonymous "" section)
    \return the iterator
*/
INIFile::SectionIterator INIFile::begin() const {
    return SectionIterator(SectionRoot);
}

/// Get a section iterator pointing past the last section
/**
    This method returns a section iterator pointing past the last section.
    \return the iterator
*/
INIFile::SectionIterator INIFile::end() const {
    return SectionIterator();
}

/// Get a key iterator pointing at the first key in the specified section
/**
    This method returns a key iterator pointing at the first key in the specified section.
    \param  section the section to iterate over
    \return the iterator
*/
INIFile::KeyIterator INIFile::begin(const std::string& section) const {
	Section* curSection = getSection(section);
	if(curSection == NULL) {
		return KeyIterator(NULL);
	} else {
		return KeyIterator(curSection->KeyRoot);
	}
}

/// Get a key iterator pointing past the end of the specified section
/**
    This method returns a key iterator pointing past the end of the specified section.
    \param  section the section to iterate over
    \return the iterator
*/
INIFile::KeyIterator INIFile::end(const std::string& section) const {
	return KeyIterator();
}

/// Saves the changes made in the INI-File to a file.
/**
	Saves the changes made in the INI-File to a file specified by filename.
	If something goes wrong false is returned otherwise true.
	\param	filename	Filename of the file. This file is opened for writing.
	\return	true on success otherwise false.
*/
bool INIFile::SaveChangesTo(const std::string& filename) const {
	SDL_RWops * file;
	if((file = SDL_RWFromFile(filename.c_str(),"w")) == NULL) {
		return false;
	}

	bool ret = SaveChangesTo(file);
	SDL_RWclose(file);
	return ret;
}

/// Saves the changes made in the INI-File to a RWop.
/**
	Saves the changes made in the INI-File to a RWop specified by file.
	If something goes wrong false is returned otherwise true.
	\param	file	SDL_RWops that is used for writing. (Cannot be readonly)
	\return	true on success otherwise false.
*/
bool INIFile::SaveChangesTo(SDL_RWops * file) const {
	INIFileLine* curLine = FirstLine;

	bool error = false;
	unsigned int written;
	while(curLine != NULL) {
		written = SDL_RWwrite(file, curLine->CompleteLine.c_str(), 1, curLine->CompleteLine.size());
		if(written != curLine->CompleteLine.size()) {
			std::cout << SDL_GetError() << std::endl;
			error = true;
		}
		if((written = SDL_RWwrite(file,"\n",1,1)) != 1) {
			error = true;
		}
		curLine = curLine->nextLine;
	}

	return !error;
}

// private methods

void INIFile::flush() const
{
	INIFileLine* curLine = FirstLine;

	while(curLine != NULL) {
		std::cout << curLine->CompleteLine << std::endl;
		curLine = curLine->nextLine;
	}
}

void INIFile::readfile(SDL_RWops * file)
{
	SectionRoot = new Section("",0,0);

	Section* curSection = SectionRoot;

	std::string completeLine;
	int lineNum = 0;
	bool SyntaxError = false;
	INIFileLine* curLine = NULL;
	INIFileLine* newINIFileLine;
	Section* newSection;
	Key* newKey;

	bool readfinished = false;

	while(!readfinished) {
		lineNum++;

		completeLine = "";
		unsigned char tmp;
		int readbytes;

		while(1) {
			readbytes = SDL_RWread(file,&tmp,1,1);
			if(readbytes <= 0) {
				readfinished = true;
				break;
			} else if(tmp == '\n') {
				break;
			} else if(tmp != '\r') {
				completeLine += tmp;
			}
		}

		const unsigned char* line = (const unsigned char*) completeLine.c_str();
		SyntaxError = false;
		int ret;

		ret = getNextChar(line,0);

		if(ret == -1) {
			// empty line or comment
			newINIFileLine = new INIFileLine(completeLine);

			if(curLine == NULL) {
				FirstLine = newINIFileLine;
				curLine = newINIFileLine;
			} else {
				curLine->nextLine = newINIFileLine;
				newINIFileLine->prevLine = curLine;
				curLine = newINIFileLine;
			}
		} else {

			if(line[ret] == '[') {
				// section line
				int sectionstart = ret+1;
				int sectionend = skipName(line,ret+1);

				if((line[sectionend] != ']') ||	(getNextChar(line,sectionend+1) != -1)) {
					SyntaxError = true;
				} else {
					// valid section line
					newSection = new Section(completeLine,sectionstart,sectionend-sectionstart);

					if(curLine == NULL) {
						FirstLine = newSection;
						curLine = newSection;
					} else {
						curLine->nextLine = newSection;
						newSection->prevLine = curLine;
						curLine = newSection;
					}

					InsertSection(newSection);
					curSection = newSection;
				}
			} else {

				// might be key/value line
				int keystart = ret;
				int keyend = skipKey(line,keystart);

				if(keystart == keyend) {
					SyntaxError = true;
				} else {
					ret = getNextChar(line,keyend);
					if((ret == -1) ||(line[ret] != '=')) {
						SyntaxError = true;
					} else {
						int valuestart = getNextChar(line,ret+1);
						if(valuestart == -1) {
							SyntaxError = true;
						} else {
							if(line[valuestart] == '"') {
								// now get the next '"'

								int valueend = getNextQuote(line,valuestart+1);

								if((valueend == -1) || (getNextChar(line,valueend+1) != -1)) {
									SyntaxError = true;
								} else {
									// valid key/value line
									newKey = new Key(completeLine,keystart,keyend-keystart,valuestart+1,valueend-valuestart-1);

									if(FirstLine == NULL) {
										FirstLine = newKey;
										curLine = newKey;
									} else {
										curLine->nextLine = newKey;
										newKey->prevLine = curLine;
										curLine = newKey;
									}

									curSection->InsertKey(newKey);
								}

							} else {
								int valueend = skipValue(line,valuestart);

								if(getNextChar(line,valueend) != -1) {
									SyntaxError = true;
								} else {
									// valid key/value line
									newKey = new Key(completeLine,keystart,keyend-keystart,valuestart,valueend-valuestart);

									if(FirstLine == NULL) {
										FirstLine = newKey;
										curLine = newKey;
									} else {
										curLine->nextLine = newKey;
										newKey->prevLine = curLine;
										curLine = newKey;
									}

									curSection->InsertKey(newKey);
								}
							}
						}
					}
				}

			}
		}

		if(SyntaxError == true) {
			if(completeLine.size() < 100) {
				// there are some buggy ini-files which have a lot of waste at the end of the file
				// and it makes no sense to print all this stuff out. just skip it
				std::cerr << "INIFile: Syntax-Error in line " << lineNum << ":" << completeLine << " !" << std::endl;
			}
			// save this line as a comment
			newINIFileLine = new INIFileLine(completeLine);

			if(curLine == NULL) {
				FirstLine = newINIFileLine;
				curLine = newINIFileLine;
			} else {
				curLine->nextLine = newINIFileLine;
				newINIFileLine->prevLine = curLine;
				curLine = newINIFileLine;
			}
		}



	}
}

void INIFile::InsertSection(Section* newSection) {
	if(SectionRoot == NULL) {
		// New root element
		SectionRoot = newSection;
	} else {
		// insert into list
		Section* curSection = SectionRoot;
		while(curSection->nextSection != NULL) {
			curSection = curSection->nextSection;
		}

		curSection->nextSection = newSection;
		newSection->prevSection = curSection;
	}
}

INIFile::Section* INIFile::getSection(const std::string& sectionname) const {
	Section* curSection = SectionRoot;
	int sectionnameSize = sectionname.size();

	while(curSection != NULL) {
		if(curSection->SectionStringLength == sectionnameSize) {
				if(strncasecmp(sectionname.c_str(), curSection->CompleteLine.c_str()+curSection->SectionStringBegin, sectionnameSize) == 0) {
					return curSection;
				}
		}

		curSection = curSection->nextSection;
	}

	return NULL;
}

INIFile::Section* INIFile::getSectionOrCreate(const std::string& sectionname) {
	Section* curSection = getSection(sectionname);

	if(curSection == NULL) {
		// create new section

		if(isValidSectionName(sectionname) == false) {
			std::cerr << "INIFile: Cannot create section with name " << sectionname << "!" << std::endl;
			return NULL;
		}

		curSection = new Section(sectionname);

		if(FirstLine == NULL) {
			FirstLine = curSection;
		} else {
            INIFileLine* curLine = FirstLine;
			while(curLine->nextLine != NULL) {
				curLine = curLine->nextLine;
			}

			curLine->nextLine = curSection;
			curSection->prevLine = curLine;
		}

		InsertSection(curSection);
	}
    return curSection;
}

INIFile::Key* INIFile::getKey(const std::string& sectionname, const std::string& keyname) const {
    INIFile::Section* curSection = getSection(sectionname);
    if(curSection == NULL) {
        return NULL;
    }

    return curSection->getKey(keyname);
}



bool INIFile::isValidSectionName(const std::string& sectionname) {
    for(unsigned int i = 0; i < sectionname.size(); i++) {
        if( (!isNormalChar(sectionname[i])) && (!isWhitespace(sectionname[i])) ) {
            return false;
        }
    }

    if(isWhitespace(sectionname[0]) || isWhitespace(sectionname[sectionname.size()-1])) {
        return false;
    } else {
        return true;
    }
}

bool INIFile::isValidKeyName(const std::string& keyname) {
    for(unsigned int i = 0; i < keyname.size(); i++) {
        if( (!isNormalChar(keyname[i])) && (!isWhitespace(keyname[i])) ) {
            return false;
        }
    }

    if(isWhitespace(keyname[0]) || isWhitespace(keyname[keyname.size()-1])) {
        return false;
    }
    return true;
}

int INIFile::getNextChar(const unsigned char* line, int startpos) {
	while(line[startpos] != '\0') {
		if((line[startpos] == ';') || (line[startpos] == '#')) {
			// comment
			return -1;
		} else if(!isWhitespace(line[startpos])) {
			return startpos;
		}
		startpos++;
	}
	return -1;
}

int INIFile::skipName(const unsigned char* line,int startpos) {
	while(line[startpos] != '\0') {
		if(isNormalChar(line[startpos]) || (line[startpos] == ' ') || (line[startpos] == '\t')) {
			startpos++;
		} else {
			return startpos;
		}
	}
	return startpos;
}

int INIFile::skipValue(const unsigned char* line,int startpos) {
	int i = startpos;
	while(line[i] != '\0') {
		if(isNormalChar(line[i]) || isWhitespace(line[i])) {
			i++;
		} else if((line[i] == ';') || (line[i] == '#')) {
			// begin of a comment
			break;
		} else {
			// some invalid character
			return i;
		}
	}

	// now we go backwards
	while(i >= startpos) {
		if(isNormalChar(line[i])) {
			return i+1;
		}
		i--;
	}
	return startpos+1;
}

int INIFile::skipKey(const unsigned char* line,int startpos) {
	int i = startpos;
	while(line[i] != '\0') {
		if(isNormalChar(line[i]) || isWhitespace(line[i])) {
			i++;
		} else if((line[i] == ';') || (line[i] == '#') || (line[i] == '=')) {
			// begin of a comment or '='
			break;
		} else {
			// some invalid character
			return i;
		}
	}

	// now we go backwards
	while(i >= startpos) {
		if(isNormalChar(line[i])) {
			return i+1;
		}
		i--;
	}
	return startpos+1;
}

int INIFile::getNextQuote(const unsigned char* line,int startpos) {
	while(line[startpos] != '\0') {
		if(line[startpos] != '"') {
			startpos++;
		} else {
			return startpos;
		}
	}
	return -1;
}

bool INIFile::isWhitespace(unsigned char s) {
	if((s == ' ') || (s == '\t') || (s == '\n') || (s == '\r')) {
		return true;
	} else {
		return false;
	}
}

bool INIFile::isNormalChar(unsigned char s) {
	if((!isWhitespace(s)) && (s >= 33) && (s != '"') && (s != ';') && (s != '#') && (s != '[') && (s != ']') && (s != '=')) {
		return true;
	} else {
		return false;
	}
}
