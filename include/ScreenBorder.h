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

#ifndef SCREENBORDER_H
#define SCREENBORDER_H

#include <DataTypes.h>

#include <misc/Stream.h>

#define SCROLLSPEED 6

/// This class manages everything that is related to the current view onto the map.
class ScreenBorder
{
public:
    /**
        Constructor
        \param gameBoardRect    this SDL_Rect specifies the rectangle on the screen where the map is shown
    */
    ScreenBorder(const SDL_Rect& gameBoardRect) {
        this->gameBoardRect = gameBoardRect;

        BottomRightCorner.x = gameBoardRect.w;
        BottomRightCorner.y = gameBoardRect.h;
        TopLeftCornerOnScreen.x = gameBoardRect.x;
        TopLeftCornerOnScreen.y = gameBoardRect.y;
        BottomRightCornerOnScreen.x = gameBoardRect.x + gameBoardRect.w;
        BottomRightCornerOnScreen.y = gameBoardRect.y + gameBoardRect.h;
    };

    /**
        Destructor
    */
    ~ScreenBorder() { ; };

    /**
        Loads the current position on the map from a stream
        \param stream the stream to load from
    */
    void load(Stream& stream) {
        Coord center;
        center.x = stream.readSint16();
        center.y = stream.readSint16();

        SetNewScreenCenter(center);
    }

    /**
        Saves the current map position to a stream
        \param stream the stream to save to
    */
    void save(Stream& stream) const
    {
        Coord center = getCurrentCenter();
        stream.writeSint16(center.x);
        stream.writeSint16(center.y);
    }

    /**
        Get the current center point of the view in world coordinates.
        \return current center point in world coordinates.
    */
    inline Coord getCurrentCenter() const
    {
        return TopLeftCorner + (BottomRightCorner - TopLeftCorner)/2;
    }

    /**
        Returns the top left corner of the view in world coordinates.
        \return current top left corner in world coordinates.
    */
    inline Coord getTopLeftCorner() const
    {
        return TopLeftCorner;
    }

    /**
        Returns the bottom right corner of the view in world coordinates.
        \return current bottom right corner in world coordinates.
    */
    inline Coord getBottomRightCorner() const
    {
        return BottomRightCorner;
    }

    /**
        Returns the position of the left edge of the view in world coordinates.
        \return current left edge in world coordinates.
    */
    inline short getLeft() const
    {
        return TopLeftCorner.x;
    }

    /**
        Returns the position of the top edge of the view in world coordinates.
        \return current top edge in world coordinates.
    */
    inline short getTop() const
    {
        return TopLeftCorner.y;
    }

    /**
        Returns the position of the right edge of the view in world coordinates.
        \return current right edge in world coordinates.
    */
    inline short getRight() const
    {
        return BottomRightCorner.x;
    }

    /**
        Returns the position of the bottom edge of the view in world coordinates.
        \return current bottom edge in world coordinates.
    */
    inline short getBottom() const
    {
        return BottomRightCorner.y;
    }

    /**
        Returns the map coordinate of the top left corner of the current view.
        \return map coordinate of the top left corner
    */
    inline Coord getTopLeftTile() const
    {
        return TopLeftCorner / BLOCKSIZE;
    }

    /**
        Returns the map coordinate of the bottom right corner of the current view.
        \return map coordinate of the bottom right corner
    */
    inline Coord getBottomRightTile() const
    {
        return BottomRightCorner / BLOCKSIZE;
    }

    /**
        There may be a part of the tile at the top left corner that is outside the screen.
        This method returns how much is outside the screen.
        \return the part of the tile that lies outside the screen.
    */
    inline Coord getCornerOffset() const
    {
        return (TopLeftCorner / BLOCKSIZE) * BLOCKSIZE - TopLeftCorner;
    }

    /**
        This method checks if some object is (partly) inside or completely outside the current view.
        \param objectPosition   object position in world coordinates
        \param objectSize       the size of the object
        \return true if (partly) inside, false if completly outside
    */
    inline bool isInsideScreen(const Coord& objectPosition, const Coord& objectSize) const
    {
		return (((objectPosition.x + objectSize.x/2) >= TopLeftCorner.x)
                && ((objectPosition.x - objectSize.x/2) <= BottomRightCorner.x)
                && ((objectPosition.y + objectSize.y/2) >= TopLeftCorner.y)
                && ((objectPosition.y - objectSize.y/2) <= BottomRightCorner.y) );
    }

    /**
        This method checks if a tile is (partly) inside the current view.
        \param tileLocation the location of the tile in map coordinates
        \return true if (partly) inside, false if completly outside
    */
    inline bool isTileInsideScreen(const Coord& tileLocation) const
    {
        return isInsideScreen(tileLocation*BLOCKSIZE + Coord(BLOCKSIZE/2, BLOCKSIZE/2), Coord(BLOCKSIZE,BLOCKSIZE));
    }

    /**
        Sets the current view to a new position.
        \param newPosition  the center of the new view in world coordinates
    */
    void SetNewScreenCenter(const Coord& newPosition);

    /**
        This method scrolls left
    */
    bool scrollLeft();

    /**
        This method scrolls right
    */
    bool scrollRight();

        /**
        This method scrolls up
    */
    bool scrollUp();

    /**
        This method scrolls down
    */
    bool scrollDown();

    /**
        This method converts from world to screen coordinates.
        \param x    the x position in world coordinates
        \return the x-coordinate on the screen
    */
    inline int world2screenX(float x) const
    {
        return (int) x - TopLeftCorner.x + TopLeftCornerOnScreen.x;
    }

    /**
        This method converts from world to screen coordinates.
        \param y    the y position in world coordinates
        \return the y-coordinate on the screen
    */
    inline int world2screenY(float y) const
    {
        return (int) y - TopLeftCorner.y + TopLeftCornerOnScreen.y;
    }

    /**
        This method converts from screen to world coordinates.
        \param x    the x coordinate on the screen
        \return the x-position in world coordinates
    */
    inline int screen2worldX(int x) const
    {
        return x - TopLeftCornerOnScreen.x + TopLeftCorner.x;
    }

    /**
        This method converts from screen to world coordinates.
        \param y    the y coordinate on the screen
        \return the y-position in world coordinates
    */
    inline int screen2worldY(int y) const
    {
        return y - TopLeftCornerOnScreen.y + TopLeftCorner.y;
    }

    /**
        This method converts from screen to map coordinates.
        \param x    the x coordinate on the screen
        \return the x-coordinate of the tile at position x in map coordinates
    */
    inline int screen2MapX(int x) const
    {
        return screen2worldX(x)/BLOCKSIZE;
    };

    /**
        This method converts from screen to map coordinates.
        \param y    the y coordinate on the screen
        \return the y-coordinate of the tile at position y in map coordinates
    */
    inline int screen2MapY(int y) const
    {
        return screen2worldY(y)/BLOCKSIZE;
    };

    /**
        This method checks if the specified x,y coordinate is within the map
        \param  x the x coordinate in screen coordinates
        \param  y the y coordinate in screen coordinates
        \return true, if inside, false otherwise
    */
    inline bool isScreenCoordInsideMap(int x, int y) const {
        return (x >= TopLeftCornerOnScreen.x  && x < BottomRightCornerOnScreen.x
                && y >= TopLeftCornerOnScreen.y  && y < BottomRightCornerOnScreen.y);
    }

    /**
        This method adjusts the screen border to the current map size.
    */
    void adjustScreenBorderToMapsize();

private:
    SDL_Rect gameBoardRect;         /// the complete game board rectangle

    Coord TopLeftCorner;            ///< the position of the top left corner in world coordinates
    Coord BottomRightCorner;        ///< the position of the bottom right corner in world coordinates

    Coord TopLeftCornerOnScreen;    ///< the position of the top left corner in screen coordinates
    Coord BottomRightCornerOnScreen;///< the position of the bottom right corner in screen coordinates
};

#endif //SCREENBORDER
