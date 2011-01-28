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

#ifndef GLOBALS_H
#define GLOBALS_H

#include <DataTypes.h>
#include <Definitions.h>
#include <FileClasses/Palette.h>
#include <data.h>
#include <misc/RobustList.h>
#include <SDL.h>


// forward declarations
class SoundPlayer;
class MusicPlayer;

class FileManager;
class GFXManager;
class SFXManager;
class FontManager;
class TextManager;

class Game;
class MapClass;
class ScreenBorder;
class House;
class UnitClass;
class StructureClass;
class BulletClass;

#ifndef SKIP_EXTERN_DEFINITION
 #define EXTERN extern
#else
 #define EXTERN
#endif


// SDL stuff
EXTERN SDL_Surface*         screen;                     ///< the screen
EXTERN Palette              palette;                    ///< the palette for the screen
EXTERN int                  drawnMouseX;                ///< the current mouse position (x coordinate)
EXTERN int                  drawnMouseY;                ///< the current mouse position (y coordinate)
EXTERN int                  cursorFrame;                ///< the current mouse cursor


// abstraction layers
EXTERN SoundPlayer*	        soundPlayer;                ///< manager for playing sfx and voice
EXTERN MusicPlayer*         musicPlayer;                ///< manager for playing background music

EXTERN FileManager*         pFileManager;               ///< manager for loading files from PAKs
EXTERN GFXManager*          pGFXManager;                ///< manager for loading and managing graphics
EXTERN SFXManager*          pSFXManager;                ///< manager for loading and managing sounds
EXTERN FontManager*         pFontManager;               ///< manager for loading and managing fonts
EXTERN TextManager*         pTextManager;               ///< manager for loading and managing texts and providing localization


// game stuff
EXTERN Game*                currentGame;                ///< the current running game
EXTERN ScreenBorder*        screenborder;               ///< the screen border for the current running game
EXTERN MapClass*            currentGameMap;             ///< the map for the current running game
EXTERN House*		        pLocalHouse;                ///< the house of the human player that is playing the current running game on this computer

EXTERN RobustList<UnitClass*>       unitList;           ///< the list of all units
EXTERN RobustList<StructureClass*>  structureList;      ///< the list of all structures
EXTERN RobustList<BulletClass*>     bulletList;         ///< the list of all bullets


// misc
EXTERN SettingsClass    settings;                       ///< the settings read from the settings file

EXTERN bool debug;                                      ///< is set for debugging purposes

EXTERN int lookDist[11];                                ///< lookup table for the circular viewing area
EXTERN int houseColor[NUM_HOUSES];                      ///< the base colors for the different houses

#endif //GLOBALS_H
