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

#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include <string>
#include <vector>
#include <algorithm>

#include <sstream>
#include <stdexcept>

bool SplitString(std::string ParseString, unsigned int NumStringPointers,...);
std::vector<std::string> SplitString(std::string ParseString);

std::string strprintf(const char* fmt, ...);

template<typename T>
inline std::string stringify(T x)
{
    std::ostringstream os;
    if (!(os << x))
        throw std::runtime_error("stringify() failed!");
    return os.str();
}


inline void convertToLower(std::string& str) {
    std::transform(str.begin(),str.end(), str.begin(), (int(*)(int)) tolower);
}

inline void convertToUpper(std::string& str) {
    std::transform(str.begin(),str.end(), str.begin(), (int(*)(int)) toupper);
}

std::string convertCP850ToISO8859_1(const std::string& text);

#endif // STRING_UTIL_H
