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

#ifndef SOUND_UTIL_H
#define SOUND_UTIL_H

#include <string>
#include <SDL_mixer.h>

Mix_Chunk* Concat2Chunks(Mix_Chunk* sound1, Mix_Chunk* sound2);
Mix_Chunk* Concat3Chunks(Mix_Chunk* sound1, Mix_Chunk* sound2, Mix_Chunk* sound3);
Mix_Chunk* Concat4Chunks(Mix_Chunk* sound1, Mix_Chunk* sound2, Mix_Chunk* sound3, Mix_Chunk* sound4);
Mix_Chunk* CreateEmptyChunk();
Mix_Chunk* CreateSilenceChunk(int length);
Mix_Chunk* GetChunkFromFile(std::string Filename);
Mix_Chunk* GetChunkFromFile(std::string Filename, std::string AlternativeFilename);

#endif // SOUND_UTIL_H
