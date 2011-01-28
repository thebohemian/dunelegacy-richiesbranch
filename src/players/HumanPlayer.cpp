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

#include <players/HumanPlayer.h>

HumanPlayer::HumanPlayer(House* associatedHouse) : Player(associatedHouse) {
    HumanPlayer::init();
}

HumanPlayer::HumanPlayer(Stream& stream, House* associatedHouse) : Player(associatedHouse) {
    HumanPlayer::init();
}

void HumanPlayer::init() {
}

HumanPlayer::~HumanPlayer() {
}

void HumanPlayer::save(Stream& stream) const {
}

void HumanPlayer::update() {
}

void HumanPlayer::onIncrementStructures(int itemID) {
}

void HumanPlayer::onDecrementStructures(int itemID, const Coord& location) {
}

void HumanPlayer::onDamage(ObjectClass* pObject, const Coord& location) {
}
