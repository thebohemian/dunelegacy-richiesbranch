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

#include <units/TankBaseClass.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <House.h>
#include <Game.h>
#include <MapClass.h>
#include <Explosion.h>
#include <ScreenBorder.h>
#include <SoundPlayer.h>

#include <structures/StructureClass.h>

TankBaseClass::TankBaseClass(House* newOwner) : TrackedUnit(newOwner)
{
    TankBaseClass::init();

    drawnTurretAngle = currentGame->RandomGen.rand(0, 7);
    turretAngle = (double) drawnTurretAngle;
}

TankBaseClass::TankBaseClass(Stream& stream) : TrackedUnit(stream)
{
    TankBaseClass::init();

	turretAngle = stream.readDouble();
    drawnTurretAngle = stream.readSint8();

	closeTarget.load(stream);
}

void TankBaseClass::init()
{
	turreted = true;
	turretTurnSpeed = 0.0625;
	GunGraphicID = -1;
	turretGraphic = NULL;
}

TankBaseClass::~TankBaseClass()
{
}

void TankBaseClass::save(Stream& stream) const
{
	TrackedUnit::save(stream);

	stream.writeDouble(turretAngle);
	stream.writeSint8(drawnTurretAngle);

	closeTarget.save(stream);
}

int TankBaseClass::getCurrentAttackAngle() {
	return drawnTurretAngle;
}

void TankBaseClass::navigate()
{
	if(moving)//change the turret angle so it faces the direction we are moving in
	{
		targetAngle = lround(8.0/256.0*dest_angle(location, destination));

		if (targetAngle == 8)
			targetAngle = 0;
	}
	TrackedUnit::navigate();
}

void TankBaseClass::engageTarget()
{
    int weaponRange = currentGame->objectData.data[itemID].weaponrange;

	if(target || bAttackPos) {
		UnitClass::engageTarget();
	/*
		if ( (target.getObjPointer() == NULL)
			|| ((!canAttack(target.getObjPointer())	|| (!forced && !isInAttackModeRange(target.getObjPointer())))
				&& !targetFriendly))
		{
			findTargetTimer = 0;
			forced = false;
			setTarget(NULL);

			if (attackMode != DEFENSIVE)
				setGuardPoint(&location);

			setDestination(&guardPoint);
			nextSpotFound = false;
			pathList.clear();
		}
		else
		{
			Coord targetLocation = target.getObjPointer()->getClosestPoint(location);
			targetDistance = blockDistance(location, targetLocation);

			if ((destination.x != targetLocation.x)	//targets location has moved, recalculate path
				&& (destination.y != targetLocation.y))
				pathList.clear();

			if (forced)
				setDestination(&targetLocation);

			targetAngle = lround(8.0/256.0*dest_angle(&location, &targetLocation));
			if (targetAngle == 8)
				targetAngle = 0;

			if (targetDistance <= weaponRange)
			{
				if (!targetFriendly)
				{
					if (target.getObjPointer()->isInfantry()
						&& (forced || (attackMode == AGGRESSIVE) || (attackMode == DEFENSIVE)))
						setDestination(&targetLocation);
					else
					{
                        setDestination(&location);
						pathList.clear();
						nextSpotFound = false;
					}

					if (drawnTurretAngle == targetAngle)
						attack();
				}
			}
			else
				setDestination(&targetLocation);
		}*/
	}

	if(closeTarget) {
		//if has a close temporary target

		if ((closeTarget.getObjPointer() == NULL)
			|| !canAttack(closeTarget.getObjPointer())) {
			closeTarget.PointTo(NONE);
		} else if (!target || targetFriendly || (targetDistance > weaponRange)) {
			Coord targetLocation = closeTarget.getObjPointer()->getClosestPoint(location);
			targetDistance = blockDistance(location, targetLocation);

			if(targetDistance > weaponRange) {
				closeTarget.PointTo(NONE);
			}

			if(closeTarget) {
				targetAngle = lround(8.0/256.0*dest_angle(location, targetLocation));
				if (targetAngle == 8) {
					targetAngle = 0;
				}

				if(targetDistance <= weaponRange) {
					if(drawnTurretAngle == targetAngle) {
						ObjectPointer temp = target;
						target = closeTarget;
						attack();
						target = temp;
					}
				}
			}
		}
	}
}

void TankBaseClass::targeting()
{
	if (active && !moving && (attackMode != STOP) && (findTargetTimer == 0)) {
		if(!forced && !target) {
			target.PointTo(findTarget());
			if(target && isInAttackModeRange(target.getObjPointer())) {
				DoAttackObject(target.getObjPointer());
				pathList.clear();
				nextSpotFound = false;
				speedCap = -1.0;
			} else {
				target.PointTo(NONE);

				if(attacking) {
					StructureClass* closestStructure = findClosestTargetStructure(this);
					if (closestStructure) {
						DoAttackObject(closestStructure);
					} else {
						UnitClass* closestUnit = findClosestTargetUnit(this);
						if(closestUnit != NULL) {
                            DoAttackObject(closestUnit);
						}
					}
				}
			}
			findTargetTimer = 100;
		}

		if(!closeTarget) {
			closeTarget = findTarget();	//find a temporary target
			findTargetTimer = 100;
		}
	}

	if(target || closeTarget || bAttackPos) {
		engageTarget();
	}
}

void TankBaseClass::turn()
{
	double	angleLeft = 0,
			angleRight = 0;

	if(!moving) {
		if(nextSpotAngle != -1) {
			if(angle > nextSpotAngle) {
				angleRight = angle - nextSpotAngle;
				angleLeft = fabs(8-angle) + nextSpotAngle;
			} else if (angle < nextSpotAngle) {
				angleRight = abs(8-nextSpotAngle) + angle;
				angleLeft = nextSpotAngle - angle;
			}

			if(angleLeft <= angleRight) {
				turnLeft();
			} else {
				turnRight();
			}
		}
	}

	if(targetAngle != INVALID) {
		if(turretAngle > targetAngle) {
			angleRight = turretAngle - targetAngle;
			angleLeft = fabs(8-turretAngle) + targetAngle;
		} else if (turretAngle < targetAngle) {
			angleRight = abs(8-targetAngle) + turretAngle;
			angleLeft = targetAngle - turretAngle;
		}

		if(angleLeft <= angleRight) {
			turnTurretLeft();
		} else {
			turnTurretRight();
		}
	}
}

void TankBaseClass::turnTurretLeft()
{
	turretAngle += turretTurnSpeed;
	if(turretAngle >= 7.5) {
	    drawnTurretAngle = lround(turretAngle) - 8;
        turretAngle -= 8.0;
	} else {
        drawnTurretAngle = lround(turretAngle);
	}
}

void TankBaseClass::turnTurretRight()
{
	turretAngle -= turretTurnSpeed;
	if(turretAngle <= -0.5) {
	    drawnTurretAngle = lround(turretAngle) + 8;
		turretAngle += 8.0;
	} else {
	    drawnTurretAngle = lround(turretAngle);
	}
}
