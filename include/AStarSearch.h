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

#ifndef ASTARSEARCH_H
#define ASTARSEARCH_H

#include <DataTypes.h>

#include <list>
#include <vector>

class UnitClass;
class MapClass;

class AStarSearch {
public:
    AStarSearch(MapClass* pMap, UnitClass* pUnit, Coord start, Coord destination);
    ~AStarSearch();

    std::list<Coord> getFoundPath() {
        std::list<Coord> path;

        if(bestCoord.x == INVALID_POS || bestCoord.y == INVALID_POS) {
            return path;
        }

        Coord currentCoord = bestCoord;
        while(true) {
            Coord nextCoord = MapData(currentCoord).parentCoord;

            if(nextCoord.x == INVALID_POS || nextCoord.y == INVALID_POS) {
                break;
            }

            path.push_front(currentCoord);
            currentCoord = nextCoord;
        }

        return path;
    };

private:
    struct TileData {
        bool    bClosed;
        bool    bInOpenList;
        size_t  openListIndex;
        Coord   parentCoord;
        float   g;
        float   h;
        float   f;
    };


    inline TileData& MapData(const Coord& coord) const { return mapData[coord.y * sizeX + coord.x]; };

    void trickleUp(int openListIndex) {
        Coord bottom = openList[openListIndex];
        float newf = MapData(bottom).f;

        size_t current = openListIndex;
        size_t parent = (openListIndex - 1)/2;
        while (current > 0 && MapData(openList[parent]).f > newf) {

            // copy parent to position of current
            openList[current] = openList[parent];
            MapData(openList[current]).openListIndex = current;

            // go up one level in the tree
            current = parent;
            parent = (parent - 1)/2;
        }

        openList[current] = bottom;
        MapData(openList[current]).openListIndex = current;
    };

    void putOnOpenListIfBetter(const Coord& coord, const Coord& parentCoord, float g, float h) {
        float f = g + h;

        if(MapData(coord).bInOpenList == false) {
            // not yet in openlist => add at the end of the open list
            MapData(coord).g = g;
            MapData(coord).h = h;
            MapData(coord).f = f;
            MapData(coord).parentCoord = parentCoord;
            MapData(coord).bInOpenList = true;
            openList.push_back(coord);
            MapData(coord).openListIndex = openList.size() - 1;

            trickleUp(openList.size() - 1);
        } else {
            // already on openlist
            if(f >= MapData(coord).f) {
                // new item is worse => don't change anything
                return;
            } else {
                // new item is better => replace
                MapData(coord).g = g;
                MapData(coord).h = h;
                MapData(coord).f = f;
                MapData(coord).parentCoord = parentCoord;
                trickleUp(MapData(coord).openListIndex);
            }
        }
    };

    Coord extractMin() {
        Coord ret = openList[0];
        MapData(ret).bInOpenList = false;

        openList[0] = openList.back();
        MapData(openList[0]).openListIndex = 0;
        openList.pop_back();

        size_t current = 0;
        Coord top = openList[current];  // save root
        float topf = MapData(top).f;
        while(current < openList.size()/2) {

            size_t leftChild = 2*current+1;
            size_t rightChild = leftChild+1;



            // find smaller child
            size_t smallerChild;
            float smallerChildf;
            if(rightChild < openList.size()) {
                float leftf = MapData(openList[leftChild]).f;
                float rightf = MapData(openList[rightChild]).f;

                if(leftf < rightf) {
                    smallerChild = leftChild;
                    smallerChildf = leftf;
                } else {
                    smallerChild = rightChild;
                    smallerChildf = rightf;
                }
            } else {
                // there is only a left child
                smallerChild = leftChild;
                smallerChildf = MapData(openList[leftChild]).f;
            }

            // top >= largerChild?
            if(topf <= smallerChildf)
                break;

            // shift child up
            openList[current] = openList[smallerChild];
            MapData(openList[current]).openListIndex = current;

            // go down one level in the tree
            current = smallerChild;
        }

        openList[current] = top;
        MapData(openList[current]).openListIndex = current;

        return ret;
    };

    int sizeX;
    int sizeY;
    Coord bestCoord;
    TileData* mapData;
    std::vector<Coord> openList;
};

#endif //ASTARSEARCH_H
