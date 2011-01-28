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

#include <misc/string_util.h>

#include <stdarg.h>
#include <stdio.h>

/**
	Splits a string into several substrings. This strings are separated with ','.
        Example:<br>
        String first, second;<br>
        SplitString("abc,xyz",2,&first, &second);<br>
	\param ParseString  the string to parse
	\param NumStringPointers    the number of pointers to strings following after this parameter
	\return true if successful, false otherwise.
*/
bool SplitString(std::string ParseString, unsigned int NumStringPointers, ...) {
	va_list arg_ptr;
	va_start(arg_ptr, NumStringPointers);

	std::string** pStr;

	if(NumStringPointers == 0)
		return false;

    pStr = new std::string*[NumStringPointers];

	for(unsigned int i = 0; i < NumStringPointers; i++) {
		pStr[i] = va_arg(arg_ptr, std::string* );
	}
	va_end(arg_ptr);

	int startpos = 0;
	unsigned int index = 0;

	for(unsigned int i = 0; i < ParseString.size(); i++) {
		if(ParseString[i] == ',') {
			*(pStr[index]) = ParseString.substr(startpos,i-startpos);
			startpos = i + 1;
			index++;
			if(index >= NumStringPointers) {
				delete [] pStr;
				return false;
			}
		}
	}

	*(pStr[index]) = ParseString.substr(startpos,ParseString.size()-startpos);
	delete [] pStr;
	return true;
}

/*
	Splits a string into several substrings. This strings are separated with ','.
*/
std::vector<std::string> SplitString(std::string ParseString) {
	std::vector<std::string> retVector;
	int startpos = 0;

	for(unsigned int i = 0; i < ParseString.size(); i++) {
		if(ParseString[i] == ',') {
			retVector.push_back(ParseString.substr(startpos,i-startpos));
			startpos = i + 1;
		}
	}

	retVector.push_back(ParseString.substr(startpos,ParseString.size()-startpos));
	return retVector;
}

std::string strprintf(const char* fmt, ...)
{
	va_list arg_ptr;
	va_start(arg_ptr, fmt);

	int length = vsnprintf(NULL, 0, fmt, arg_ptr);
	if(length < 0) {
        throw std::runtime_error("strprintf(): vsnprintf() failed!");
	}

	char* tmpBuffer = new char[length+1];

    va_start(arg_ptr, fmt);
	if(vsnprintf(tmpBuffer, length+1, fmt, arg_ptr) < 0) {
	    delete [] tmpBuffer;
        throw std::runtime_error("strprintf(): vsnprintf() failed!");
	}

	std::string formatedString(tmpBuffer);

	delete [] tmpBuffer;

	return formatedString;
}

std::string convertCP850ToISO8859_1(const std::string& text)
{
    // contains the upper half of cp850 (128 - 255)
    static const unsigned char cp850toISO8859_1[] = {
        0xc7, 0xfc, 0xe9, 0xe2, 0xe4, 0xe0, 0xe5, 0xe7, 0xea, 0xeb, 0xe8, 0xef, 0xee, 0xec, 0xc4, 0xc5,
        0xc9, 0xe6, 0xc6, 0xf4, 0xf6, 0xf2, 0xfb, 0xf9, 0xff, 0xd6, 0xdc, 0xf8, 0xa3, 0xd8, 0xd7, 0x3f,
        0xe1, 0xed, 0xf3, 0xfa, 0xf1, 0xd1, 0xaa, 0xba, 0xbf, 0xae, 0xac, 0xbd, 0xbc, 0xa1, 0xab, 0xbb,
        0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0xc1, 0xc2, 0xc0, 0xa9, 0x3f, 0x3f, 0x3f, 0x3f, 0xa2, 0xa5, 0x3f,
        0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0xe3, 0xc3, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0xa4,
        0xf0, 0xd0, 0xca, 0xcb, 0xc8, 0x3f, 0xcd, 0xce, 0xcf, 0x3f, 0x3f, 0x3f, 0x3f, 0xa6, 0xcc, 0x3f,
        0xd3, 0xdf, 0xd4, 0xd2, 0xf5, 0xd5, 0xb5, 0xfe, 0xde, 0xda, 0xdb, 0xd9, 0xfd, 0xdd, 0xaf, 0xb4,
        0xad, 0xb1, 0x3f, 0xbe, 0xb6, 0xa7, 0xf7, 0xb8, 0xb0, 0xa8, 0xb7, 0xb9, 0xb3, 0xb2, 0x3f, 0xa0
    };

    std::string result;
    for(unsigned int i = 0; i < text.size(); i++) {
        unsigned char c = (unsigned char) text[i];
        if(c < 128) {
            result += c;
        } else {
            result += cp850toISO8859_1[c-128];
        }
    }
    return result;
}
