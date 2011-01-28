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

#ifndef DEVASTATORCLASS_H
#define DEVASTATORCLASS_H

#include <units/TrackedUnit.h>

class DevastatorClass : public TrackedUnit
{
public:
	DevastatorClass(House* newOwner);
	DevastatorClass(Stream& stream);
	void init();
	virtual ~DevastatorClass();

	void save(Stream& stream) const;

	void blitToScreen();
	void HandleStartDevastateClick();
	void DoStartDevastate();
	virtual void destroy();

    /**
        Updates this devastator.
        \return true if this object still exists, false if it was destroyed
	*/
	virtual bool update();

	void PlayAttackSound();

private:
    // devastator state
	Sint32      devastateTimer;     ///< When will this devastator devastate

    // drawing information
	SDL_Surface*	turretGraphic;  ///< The graphic of the turret
	int             GunGraphicID;   ///< The id of the turret graphic (needed if we want to reload the graphic)
};

#endif // DEVASTATORCLASS_H
