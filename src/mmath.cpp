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

#include <mmath.h>

#include <Definitions.h>
#include <DataTypes.h>

#include <stdarg.h>
#include <algorithm>


double fixDouble(double number)
{
	if (fabs(number - (double)((int)number)) < 0.00001)
		number = (double)((int)number);

	return number;
}


int getRandomInt(int min, int max)
{
	max++;
	return ((rand() % (max-min)) + min);
}


int getRandomOf(int numParam, ...) {
	int nthParam = getRandomInt(0,numParam-1);

	va_list arg_ptr;
	va_start(arg_ptr, numParam);

	int ret = va_arg(arg_ptr, int);

	for(int i = 1; i <= nthParam; i++) {
		ret = va_arg(arg_ptr, int);
	}
	va_end(arg_ptr);

	return ret;
}




double dest_angle(const Coord& p1, const Coord& p2)
{
        double  destAngle = 0.0;

        double  diffX = p2.x - p1.x,
                diffY = p2.y - p1.y;

        if (diffX != 0)
        {
                if ((diffX >= 0) && (diffY >= 0))  //if x2 >= x1 && y2 >= y1
                {
                        destAngle = 2*M_PI - atan(fabs(diffY/diffX));
                }
                else if ((diffX < 0) && (diffY >= 0))  //if x2 < x1 && y2 >= y1
                {
                        destAngle = atan(fabs(diffY/diffX)) + M_PI;
                }
                else if ((diffX < 0) && (diffY < 0))  //if x2 < x1 && y2 < y1
                {
                        destAngle = M_PI - atan(fabs(diffY/diffX));
                }
                else if ((diffX >= 0) && (diffY < 0))  //if x2 >= x1 && y2 < y1
                {
                        destAngle = atan(fabs(diffY/diffX));
                }
        }
        else if (diffY <= 0)
        {
                destAngle = M_PI/2;
        }
        else destAngle = 3*M_PI/2;

        if (destAngle == 2*M_PI) destAngle = 0;

        return (destAngle*256.0/(2*M_PI));
}


double distance_from(const Coord& p1, const Coord& p2)
{
	double	first = (p1.x - p2.x),
			second = (p1.y - p2.y);

	return(sqrt(first*first + second*second));
}

double distance_from(double x, double y, double to_x, double to_y)
{
	double	first = (x - to_x),
			second = (y - to_y);

	return(sqrt(first*first + second*second));
}

double blockDistance(const Coord& p1, const Coord& p2)
{
	int xDis = abs(p1.x - p2.x),
		yDis = abs(p1.y - p2.y),

		minDis = std::min(xDis, yDis);

	return ((double)std::max(xDis, yDis) + (double)minDis*(DIAGONALCOST - 1.0));
	//return (((double)minDis)*DIAGONALCOST + max(xDis, yDis) - minDis);
}
