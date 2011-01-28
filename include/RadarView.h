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

#ifndef RADARVIEW_H
#define RADARVIEW_H

#include <DataTypes.h>

#include <SDL.h>

extern SDL_Rect RadarPosition;

typedef enum {
    Mode_RadarOff,
    Mode_RadarOn,
	Mode_AnimationRadarOff,
	Mode_AnimationRadarOn
} RadarViewMode;

/// This class manages the mini map at the top right corner of the screen
class RadarView
{
public:
	/**
        Constructor
        \param  x   x coordinate of the radar on the screen
        \param  y   y coordinate of the radar on the screen
	*/
	RadarView(int x, int y);

	/**
        Destructor
	*/
    virtual ~RadarView();

	/**
        Draws the radar
	*/
	void draw() const;

	/**
        This method updates the radar. It should be called every game tick
	*/
    void update();

	/**
        This method switches the radar on or off
        \param bOn  true = switches the radar on, false = switches the radar off
	*/
	void switchRadarMode(bool bOn);

	/**
        This method sets the radar mode directly. To show the static animation use switchRadarMode().
        \param bStatus  true = switches the radar on, false = switches the radar off
	*/
	void setRadarMode(bool bStatus);

	/**
        This method returns the corresponding world coordinates for a point on the radar
        \param mouseX  the position on the radar screen (in screen coordinates)
        \param mouseY  the position on the radar screen (in screen coordinates)
        \return the world coordinates
	*/
	Coord getWorldCoords(int mouseX, int mouseY) const;

	/**
        This method checks if position is inside the radar view
        \param mouseX the x-coordinate to check (in screen coordinates)
        \param mouseY the y-coordinate to check (in screen coordinates)
        \return true, if inside the radar view; false otherwise
	*/
	bool isOnRadar(int mouseX, int mouseY) const;


    /**
        This method calculates the scale and the offsets that are neccessary to show a minimap centered inside a 128x128 rectangle.
        \param  MapSizeX    The width of the map in tiles
        \param  MapSizeY    The height of the map in tiles
        \param  scale       The scale factor is saved here
        \param  offsetX     The offset in x direction is saved here
        \param  offsetY     The offset in y direction is saved here
    */
	static void calculateScaleAndOffsets(int MapSizeX, int MapSizeY, int& scale, int& offsetX, int& offsetY) {
        scale = 1;
        offsetX = 0;
        offsetY = 0;

        if((MapSizeX == 32) && (MapSizeY == 32)) {
            scale = 4;
        } else if((MapSizeX == 32) && (MapSizeY == 64)) {
            offsetX = 32;
            scale = 2;
        } else if((MapSizeX == 32) && (MapSizeY == 128)) {
            offsetX = 48;
        } else if((MapSizeX == 64) && (MapSizeY == 32)) {
            offsetY = 32;
            scale = 2;
        } else if((MapSizeX == 64) && (MapSizeY == 64)) {
            scale = 2;
        } else if((MapSizeX == 64) && (MapSizeY == 128)) {
            offsetX = 32;
        } else if((MapSizeX == 128) && (MapSizeY == 32)) {
            offsetY = 48;
        } else if((MapSizeX == 128) && (MapSizeY == 64)) {
            offsetY = 32;
        } else if((MapSizeX == 128) && (MapSizeY == 128)) {
            // nothing
        } else if((MapSizeX == 21) && (MapSizeY == 21)) {
            offsetX = 11;
            offsetY = 11;
            scale = 5;
        } else if((MapSizeX == 62) && (MapSizeY == 62)) {
            offsetX = 2;
            offsetY = 2;
            scale = 2;
        }
	}

private:
	RadarViewMode currentRadarMode;         ///< the current mode of the radar

	int animFrame;                          ///< the current animation frame

	int animCounter;                        ///< this counter is for counting the ticks one animation frame is shown

	SDL_Surface* radarStaticAnimation;      ///< holds the animation graphic for radar static

	SDL_Rect radarPosition;                 ///< the position of the radar on the screen
};

#endif // RADARVIEW_H
