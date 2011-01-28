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

#include <structures/TurretClass.h>

#include <globals.h>

#include <Game.h>
#include <MapClass.h>
#include <BulletClass.h>
#include <SoundPlayer.h>

TurretClass::TurretClass(House* newOwner) : StructureClass(newOwner)
{
    TurretClass::init();

    angle = currentGame->RandomGen.rand(0, 7);
	drawnAngle = lround(angle);

	findTargetTimer = 0;
	weaponTimer = 0;
}

TurretClass::TurretClass(Stream& stream) : StructureClass(stream)
{
    TurretClass::init();

    findTargetTimer = stream.readSint32();
    weaponTimer = stream.readSint32();
}

void TurretClass::init()
{
    structureSize.x = 1;
	structureSize.y = 1;

    canAttackStuff = true;
	attackMode = AREAGUARD;
}

TurretClass::~TurretClass()
{
}

void TurretClass::save(Stream& stream) const
{
	StructureClass::save(stream);

	stream.writeSint32(findTargetTimer);
	stream.writeSint32(weaponTimer);
}

void TurretClass::doSpecificStuff()
{
	if (target && (target.getObjPointer() != NULL)) {
		if (!canAttack(target.getObjPointer()) || !targetInWeaponRange()) {
			setTarget(NULL);
		} else if (targetInWeaponRange()) {
			Coord closestPoint = target.getObjPointer()->getClosestPoint(location);
			double destAngle = dest_angle(location, closestPoint);
			int wantedAngle = lround(8.0/256.0*destAngle);

			if(wantedAngle == 8) {
				wantedAngle = 0;
			}

			if(angle != wantedAngle) {
				// turn
                double  angleLeft = 0.0;
                double  angleRight = 0.0;

                if(angle > wantedAngle) {
                    angleRight = angle - wantedAngle;
                    angleLeft = fabs(8-angle)+wantedAngle;
                }
                else if (angle < wantedAngle) {
                    angleRight = abs(8-wantedAngle) + angle;
                    angleLeft = wantedAngle - angle;
                }

                if(angleLeft <= angleRight) {
                    turnLeft();
                } else {
                    turnRight();
                }
			}

			if(drawnAngle == wantedAngle) {
				attack();
			}

		} else {
			setTarget(NULL);
		}
	} else if ((attackMode != STOP) && (findTargetTimer == 0)) {
		setTarget(findTarget());
		findTargetTimer = 100;
	}

	if (findTargetTimer > 0)
		findTargetTimer--;
	if (weaponTimer > 0)
		weaponTimer--;
}

void TurretClass::HandleActionCommand(int xPos, int yPos)
{
	if (currentGameMap->cellExists(xPos, yPos)) {
		ObjectClass* tempTarget = currentGameMap->getCell(xPos, yPos)->getObject();
		currentGame->GetCommandManager().addCommand(Command(CMD_TURRET_ATTACKOBJECT,objectID,tempTarget->getObjectID()));

	}
}

void TurretClass::DoAttackObject(Uint32 TargetObjectID) {
	ObjectClass* pObject = currentGame->getObjectManager().getObject(TargetObjectID);
	if(pObject == NULL) {
		return;
	}

	setDestination(INVALID_POS,INVALID_POS);
	setTarget(pObject);
	setForced(true);
}

void TurretClass::turnLeft()
{
	angle += currentGame->objectData.data[itemID].turnspeed;
	if (angle >= 7.5)	//must keep drawnangle between 0 and 7
		angle -= 8.0;
	drawnAngle = lround(angle);
	curAnimFrame = FirstAnimFrame = LastAnimFrame = ((10-drawnAngle) % 8) + 2;
}

void TurretClass::turnRight()
{
	angle -= currentGame->objectData.data[itemID].turnspeed;
	if (angle < -0.5)	//must keep drawnangle between 0 and 7
		angle += 8;
	drawnAngle = lround(angle);
	curAnimFrame = FirstAnimFrame = LastAnimFrame = ((10-drawnAngle) % 8) + 2;
}

void TurretClass::attack()
{
	if ((weaponTimer == 0) && (target.getObjPointer() != NULL))
	{
		Coord centerPoint = getCenterPoint();
		Coord targetCenterPoint = target.getObjPointer()->getClosestCenterPoint(location);

		bulletList.push_back( new BulletClass( objectID, &centerPoint, &targetCenterPoint,bulletType,
                                               currentGame->objectData.data[itemID].weapondamage,
                                               target.getObjPointer()->isAFlyingUnit() ) );

		soundPlayer->playSoundAt(attackSound, location);
		weaponTimer = weaponReloadTime;
	}
}
