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

#ifndef MMATH_H
#define MMATH_H

// forward declaration
class Coord;

#include <cmath>

double fixDouble(double number);

int getRandomInt(int min, int max);
int getRandomOf(int numParam, ...);

double dest_angle(const Coord& p1, const Coord& p2);

double distance_from(const Coord& p1, const Coord& p2);
double distance_from(double x, double y, double to_x, double to_y);

double blockDistance(const Coord& p1, const Coord& p2);

#endif //MMATH_H
