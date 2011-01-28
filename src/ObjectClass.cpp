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

#include <ObjectClass.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <FileClasses/music/MusicPlayer.h>

#include <Game.h>
#include <House.h>
#include <misc/Stream.h>
#include <SoundPlayer.h>
#include <MapClass.h>
#include <ScreenBorder.h>
#include <GUI/ObjectInterfaces/DefaultObjectInterface.h>

//structures
#include <structures/BarracksClass.h>
#include <structures/ConstructionYardClass.h>
#include <structures/GunTurretClass.h>
#include <structures/HeavyFactoryClass.h>
#include <structures/HighTechFactoryClass.h>
#include <structures/IXClass.h>
#include <structures/LightFactoryClass.h>
#include <structures/PalaceClass.h>
#include <structures/RadarClass.h>
#include <structures/RefineryClass.h>
#include <structures/RepairYardClass.h>
#include <structures/RocketTurretClass.h>
#include <structures/SiloClass.h>
#include <structures/StarPortClass.h>
#include <structures/WallClass.h>
#include <structures/WindTrapClass.h>
#include <structures/WORClass.h>

//units
#include <units/Carryall.h>
#include <units/DevastatorClass.h>
#include <units/DeviatorClass.h>
#include <units/FremenClass.h>
#include <units/Frigate.h>
#include <units/HarvesterClass.h>
#include <units/LauncherClass.h>
#include <units/MCVClass.h>
#include <units/Ornithopter.h>
#include <units/QuadClass.h>
#include <units/RaiderClass.h>
#include <units/Saboteur.h>
#include <units/SandWorm.h>
#include <units/SardaukarClass.h>
#include <units/SiegeTankClass.h>
#include <units/SoldierClass.h>
#include <units/SonicTankClass.h>
#include <units/TankClass.h>
#include <units/TrikeClass.h>
#include <units/TrooperClass.h>

ObjectClass::ObjectClass(House* newOwner) : owner(newOwner)
{
    ObjectClass::init();

	objectID = NONE;

    health = 0;
    badlyDamaged = false;

	location.x = INVALID_POS;
	location.y = INVALID_POS;
    oldLocation.x = INVALID_POS;
	oldLocation.y = INVALID_POS;
	destination.x = INVALID_POS;
	destination.y = INVALID_POS;
	realX = 0.0;
	realY = 0.0;

    drawnAngle = 0;
	angle = drawnAngle;

	active = false;
    respondable = true;
	selected = false;

	forced = false;
    setTarget(NULL);
	targetFriendly = false;
	attackMode = STOP;

	setVisible(VIS_ALL, false);
}

ObjectClass::ObjectClass(Stream& stream)
{
    owner = currentGame->house[stream.readUint32()];

    ObjectClass::init();

    health = stream.readDouble();
    badlyDamaged = stream.readBool();

	location.x = stream.readSint32();
	location.y = stream.readSint32();
    oldLocation.x = stream.readSint32();
	oldLocation.y = stream.readSint32();
	destination.x = stream.readSint32();
	destination.y = stream.readSint32();
	realX = stream.readDouble();
	realY = stream.readDouble();

    angle = stream.readDouble();
    drawnAngle = stream.readSint8();

    active = stream.readBool();
    respondable = stream.readBool();
    selected = stream.readBool();

	forced = stream.readBool();
	target.load(stream);
	targetFriendly = stream.readBool();
	attackMode = (ATTACKMODE) stream.readUint32();

	for(int i = 0; i < MAX_PLAYERS ; i++) {
		visible[i] = stream.readBool();
	}
}

void ObjectClass::init() {
	itemID = ItemID_Invalid;

	aFlyingUnit = false;
	aGroundUnit = false;
	aStructure = false;
	aUnit = false;
	infantry = false;
	aBuilder = false;

	canAttackStuff = false;

	radius = BLOCKSIZE/2;

	GraphicID = -1;
    graphic = NULL;
    imageW = 0;
    imageH = 0;

}

ObjectClass::~ObjectClass() {
}

void ObjectClass::save(Stream& stream) const
{
    stream.writeUint32(owner->getHouseID());

    stream.writeDouble(health);
    stream.writeBool(badlyDamaged);

	stream.writeSint32(location.x);
	stream.writeSint32(location.y);
    stream.writeSint32(oldLocation.x);
	stream.writeSint32(oldLocation.y);
	stream.writeSint32(destination.x);
	stream.writeSint32(destination.y);
	stream.writeDouble(realX);
	stream.writeDouble(realY);

    stream.writeDouble(angle);
    stream.writeSint8(drawnAngle);

	stream.writeBool(active);
	stream.writeBool(respondable);
    stream.writeBool(selected);

	stream.writeBool(forced);
    target.save(stream);
	stream.writeBool(targetFriendly);
    stream.writeUint32(attackMode);

	for(int i = 0; i < MAX_PLAYERS ; i++) {
		stream.writeBool(visible[i]);
	}
}

void ObjectClass::blitToScreen()
{
	SDL_Rect dest, source;
	dest.x = getDrawnX();
	dest.y = getDrawnY();
	dest.w = source.w = imageW;
	dest.h = source.h = imageH;

	source.x = drawnAngle*imageW;
	source.y = owner->getHouseID()*imageH;

	SDL_BlitSurface(graphic, &source, screen, &dest);
}

/**
    Returns the center point of this object
    \return the center point in world coordinates
*/
Coord ObjectClass::getCenterPoint() const
{
    return Coord(lround(realX), lround(realY));
}

Coord ObjectClass::getClosestCenterPoint(const Coord& objectLocation) const
{
	return getCenterPoint();
}


int ObjectClass::getMaxHealth() const
{
    return currentGame->objectData.data[itemID].hitpoints;
}

void ObjectClass::handleDamage(int damage, Uint32 damagerID, House* damagerOwner)
{
    if(damage >= 0) {
        double newHealth = getHealth();

        newHealth -= damage;

        if(newHealth < 0) {
            setHealth(0);

            if(damagerOwner != NULL) {
                damagerOwner->informHasKilled(itemID);
            }
        } else {
            setHealth(newHealth);
        }
    }

    if(getOwner() == pLocalHouse) {
        musicPlayer->changeMusic(MUSIC_ATTACK);
    }

    getOwner()->noteDamageLocation(this, location);
}

void ObjectClass::HandleInterfaceEvent(SDL_Event* event)
{
	;
}

ObjectInterface* ObjectClass::GetInterfaceContainer() {
	return DefaultObjectInterface::Create(objectID);
}

void ObjectClass::removeFromSelectionLists()
{
    currentGame->getSelectedList().erase(getObjectID());
    selected = false;

    for(int i=0; i < NUMSELECTEDLISTS; i++) {
        currentGame->getGroupList(i).erase(getObjectID());

    }
}

void ObjectClass::setDestination(int newX, int newY)
{
	if (currentGameMap->cellExists(newX, newY) || ((newX == INVALID_POS) && (newY == INVALID_POS)))
	{
		destination.x = newX;
		destination.y = newY;
	}
}

void ObjectClass::setHealth(double newHealth)
{
	if ((newHealth >= 0) && (newHealth <= getMaxHealth()))
	{
		health = newHealth;
        badlyDamaged = (health/(double)getMaxHealth() < HEAVILYDAMAGEDRATIO);
	}
}

void ObjectClass::setLocation(int xPos, int yPos)
{
	if((xPos == INVALID_POS) && (yPos == INVALID_POS)) {
		location.x = INVALID_POS;
		location.y = INVALID_POS;
	} else if (currentGameMap->cellExists(xPos, yPos))	{
		location.x = xPos;
		location.y = yPos;
		realX = location.x*BLOCKSIZE;
		realY = location.y*BLOCKSIZE;

		assignToMap(location);
	}
}

void ObjectClass::setObjectID(int newObjectID)
{
	if (newObjectID >= 0)
		objectID = newObjectID;
}

void ObjectClass::setVisible(int team, bool status)
{
	if (team == VIS_ALL) {
		for(int i = 0; i < MAX_PLAYERS; i++)
			visible[i] = status;
	} else if ((team >= 0) && (team < MAX_PLAYERS)) {
		visible[team] = status;
	}
}

void ObjectClass::setTarget(ObjectClass* newTarget)
{
	target.PointTo(newTarget);
	targetFriendly = (target && (target.getObjPointer()->getOwner()->getTeam() == owner->getTeam()) && (target.getObjPointer()->getItemID() != Unit_Sandworm));
}

void ObjectClass::unassignFromMap(const Coord& location)
{
	if (currentGameMap->cellExists(location))
		currentGameMap->getCell(location)->unassignObject(getObjectID());
}

bool ObjectClass::canAttack(const ObjectClass* object) const
{
	if ((object != NULL)
		&& (object->isAStructure()
			|| !object->isAFlyingUnit())
		&& ((object->getOwner()->getTeam() != owner->getTeam())
			|| object->getItemID() == Unit_Sandworm)
		&& object->isVisible(getOwner()->getTeam())) {
		return true;
	} else {
		return false;
	}
}

bool ObjectClass::isOnScreen() const
{
	if(screenborder->isInsideScreen(Coord((int) getRealX(), (int) getRealY()),
                                    Coord(getImageW(), getImageH())) == true)
    {
		return true;
	} else {
		return false;
	}
}

bool ObjectClass::isVisible(int team) const
{
	if ((team >= 0) && (team < MAX_PLAYERS))
		return visible[team];
	else
		return false;
}

bool ObjectClass::isVisible() const
{
    for(int i=0;i<MAX_PLAYERS;i++) {
        if(visible[i]) {
            return true;
        }
    }

    return false;
}

int ObjectClass::getHealthColor() const
{
	double healthPercent = (double)health/(double)getMaxHealth();

	if (healthPercent >= 0.7) {
		return COLOR_LIGHTGREEN;
	} else if (healthPercent >= HEAVILYDAMAGEDRATIO) {
		return COLOR_YELLOW;
	} else {
		return COLOR_RED;
	}
}

Coord ObjectClass::getClosestPoint(const Coord& point) const
{
	return location;
}

StructureClass* ObjectClass::findClosestTargetStructure(ObjectClass* object)
{

	StructureClass	*closestStructure = NULL;
	double			closestDistance = 100000000.0;

    RobustList<StructureClass*>::const_iterator iter;
    for(iter = structureList.begin(); iter != structureList.end(); ++iter) {
        StructureClass* tempStructure = *iter;

        if(object->canAttack(tempStructure)) {
			Coord closestPoint = tempStructure->getClosestPoint(object->getLocation());
			double structureDistance = blockDistance(object->getLocation(), closestPoint);

			if(tempStructure->getItemID() == Structure_Wall) {
					structureDistance += 20000000.0; //so that walls are targeted very last
            }

            if(structureDistance < closestDistance)	{
                closestDistance = structureDistance;
                closestStructure = tempStructure;
            }
        }
	}

	return closestStructure;
}

UnitClass* ObjectClass::findClosestTargetUnit(ObjectClass* object)
{
	UnitClass	*closestUnit = NULL;
	double		closestDistance = 100000000.0;

    RobustList<UnitClass*>::const_iterator iter;
    for(iter = unitList.begin(); iter != unitList.end(); ++iter) {
        UnitClass* tempUnit = *iter;

		if(object->canAttack(tempUnit)) {
			Coord closestPoint = tempUnit->getClosestPoint(object->getLocation());
			double unitDistance = blockDistance(object->getLocation(), closestPoint);

			if(unitDistance < closestDistance) {
                closestDistance = unitDistance;
				closestUnit = tempUnit;
            }
        }
	}

	return closestUnit;
}

ObjectClass* ObjectClass::findTarget()
{
 	ObjectClass	*tempTarget,
				*closestTarget = NULL;

	int	checkRange = 0;
	int	xPos = location.x;
	int	yPos = location.y;

	double closestDistance = 1000000.0;

//searches for a target in an area like as shown below
//                     *****
//                   *********
//                  *****T*****
//                   *********
//                     *****

    switch(attackMode) {
        case GUARD: {
            checkRange = getWeaponRange();
        } break;

        case AREAGUARD: {
            checkRange = getGuardRange();
        } break;

        case AMBUSH: {
            checkRange = getViewRange();
        } break;

        case HUNT: {
            // check whole map
            StructureClass* pTargetStructure = findClosestTargetStructure(this);
            if(pTargetStructure != NULL) {
                return pTargetStructure;
            } else {
                return findClosestTargetUnit(this);
            }
        } break;

        case STOP:
        default: {
            return NULL;
        } break;
    }

	int xCheck = xPos - checkRange;

	if (xCheck < 0)
		xCheck = 0;

	int yCheck;

	while ((xCheck < currentGameMap->sizeX) && ((xCheck - xPos) <=  checkRange))
	{
		yCheck = (yPos - lookDist[abs(xCheck - xPos)]);

		if (yCheck < 0)
			yCheck = 0;

		while ((yCheck < currentGameMap->sizeY) && ((yCheck - yPos) <=  lookDist[abs(xCheck - xPos)]))
		{
			if(currentGameMap->cell[xCheck][yCheck].hasAnObject())
			{
				tempTarget = currentGameMap->cell[xCheck][yCheck].getObject();

				if (((tempTarget->getItemID() != Structure_Wall) || (closestTarget == NULL)) && canAttack(tempTarget))
				{
					double targetDistance = blockDistance(location, tempTarget->getLocation());
					if (targetDistance < closestDistance)
					{
						closestTarget = tempTarget;
						closestDistance = targetDistance;
					}
				}
			}

			yCheck++;
		}

		xCheck++;
		yCheck = yPos;
	}

	return closestTarget;
}

int ObjectClass::getViewRange() const
{
    return currentGame->objectData.data[itemID].viewrange;
}

int ObjectClass::getGuardRange() const {

    // small hack for first mission (otherwise all infantry units would attack the player)
    return std::min(DEFAULT_GUARDRANGE,
                    std::max(2*currentGame->objectData.data[itemID].viewrange,
                             (3*currentGame->objectData.data[itemID].weaponrange)/2 ));
}

int ObjectClass::getWeaponRange() const
{
    return currentGame->objectData.data[itemID].weaponrange;
}

int ObjectClass::getRadius() const
{
    return currentGame->objectData.data[itemID].radius;
}

ObjectClass* ObjectClass::createObject(int ItemID, House* Owner, Uint32 ObjectID)
{
	ObjectClass* newObject = NULL;
	switch (ItemID)
	{
		case Structure_Barracks:			newObject = new BarracksClass(Owner); break;
		case Structure_ConstructionYard:	newObject = new ConstructionYardClass(Owner); break;
		case Structure_GunTurret:			newObject = new GunTurretClass(Owner); break;
		case Structure_HeavyFactory:		newObject = new HeavyFactoryClass(Owner); break;
		case Structure_HighTechFactory:		newObject = new HighTechFactoryClass(Owner); break;
		case Structure_IX:					newObject = new IXClass(Owner); break;
		case Structure_LightFactory:		newObject = new LightFactoryClass(Owner); break;
		case Structure_Palace:				newObject = new PalaceClass(Owner); break;
		case Structure_Radar:				newObject = new RadarClass(Owner); break;
		case Structure_Refinery:			newObject = new RefineryClass(Owner); break;
		case Structure_RepairYard:			newObject = new RepairYardClass(Owner); break;
		case Structure_RocketTurret:		newObject = new RocketTurretClass(Owner); break;
		case Structure_Silo:				newObject = new SiloClass(Owner); break;
		case Structure_StarPort:			newObject = new StarPortClass(Owner); break;
		case Structure_Wall:				newObject = new WallClass(Owner); break;
		case Structure_WindTrap:			newObject = new WindTrapClass(Owner); break;
		case Structure_WOR:					newObject = new WORClass(Owner); break;

		case Unit_Carryall:					newObject = new Carryall(Owner); break;
		case Unit_Devastator:				newObject = new DevastatorClass(Owner); break;
		case Unit_Deviator:					newObject = new DeviatorClass(Owner); break;
		case Unit_Frigate:					newObject = new Frigate(Owner); break;
		case Unit_Harvester:				newObject = new HarvesterClass(Owner); break;
		case Unit_Soldier:					newObject = new SoldierClass(Owner); break;
		case Unit_Launcher:					newObject = new LauncherClass(Owner); break;
		case Unit_MCV:						newObject = new MCVClass(Owner); break;
		case Unit_Ornithopter:				newObject = new Ornithopter(Owner); break;
		case Unit_Quad:						newObject = new QuadClass(Owner); break;
		case Unit_Saboteur:					newObject = new Saboteur(Owner); break;
		case Unit_Sandworm:					newObject = new Sandworm(Owner); break;
		case Unit_SiegeTank:				newObject = new SiegeTankClass(Owner); break;
		case Unit_SonicTank:				newObject = new SonicTankClass(Owner); break;
		case Unit_Tank:						newObject = new TankClass(Owner); break;
		case Unit_Trike:					newObject = new TrikeClass(Owner); break;
		case Unit_Raider:					newObject = new RaiderClass(Owner); break;
		case Unit_Trooper:					newObject = new TrooperClass(Owner); break;
		case Unit_Sardaukar:				newObject = new SardaukarClass(Owner); break;
		case Unit_Fremen:					newObject = new FremenClass(Owner); break;

		default:							newObject = NULL;
											fprintf(stderr,"ObjectClass::createObject(): %d is no valid ItemID!\n",ItemID);
											break;
	}

	if(newObject == NULL)
		return NULL;

	if(ObjectID == NONE) {
		ObjectID = currentGame->getObjectManager().AddObject(newObject);
		newObject->setObjectID(ObjectID);
	} else {
		newObject->setObjectID(ObjectID);
	}

	return newObject;
}

ObjectClass* ObjectClass::loadObject(Stream& stream, int ItemID, Uint32 ObjectID)
{
	ObjectClass* newObject = NULL;
	switch (ItemID)
	{
		case Structure_Barracks:			newObject = new BarracksClass(stream); break;
		case Structure_ConstructionYard:	newObject = new ConstructionYardClass(stream); break;
		case Structure_GunTurret:			newObject = new GunTurretClass(stream); break;
		case Structure_HeavyFactory:		newObject = new HeavyFactoryClass(stream); break;
		case Structure_HighTechFactory:		newObject = new HighTechFactoryClass(stream); break;
		case Structure_IX:					newObject = new IXClass(stream); break;
		case Structure_LightFactory:		newObject = new LightFactoryClass(stream); break;
		case Structure_Palace:				newObject = new PalaceClass(stream); break;
		case Structure_Radar:				newObject = new RadarClass(stream); break;
		case Structure_Refinery:			newObject = new RefineryClass(stream); break;
		case Structure_RepairYard:			newObject = new RepairYardClass(stream); break;
		case Structure_RocketTurret:		newObject = new RocketTurretClass(stream); break;
		case Structure_Silo:				newObject = new SiloClass(stream); break;
		case Structure_StarPort:			newObject = new StarPortClass(stream); break;
		case Structure_Wall:				newObject = new WallClass(stream); break;
		case Structure_WindTrap:			newObject = new WindTrapClass(stream); break;
		case Structure_WOR:					newObject = new WORClass(stream); break;

		case Unit_Carryall:					newObject = new Carryall(stream); break;
		case Unit_Devastator:				newObject = new DevastatorClass(stream); break;
		case Unit_Deviator:					newObject = new DeviatorClass(stream); break;
		case Unit_Frigate:					newObject = new Frigate(stream); break;
		case Unit_Harvester:				newObject = new HarvesterClass(stream); break;
		case Unit_Soldier:					newObject = new SoldierClass(stream); break;
		case Unit_Launcher:					newObject = new LauncherClass(stream); break;
		case Unit_MCV:						newObject = new MCVClass(stream); break;
		case Unit_Ornithopter:				newObject = new Ornithopter(stream); break;
		case Unit_Quad:						newObject = new QuadClass(stream); break;
		case Unit_Saboteur:					newObject = new Saboteur(stream); break;
		case Unit_Sandworm:					newObject = new Sandworm(stream); break;
		case Unit_SiegeTank:				newObject = new SiegeTankClass(stream); break;
		case Unit_SonicTank:				newObject = new SonicTankClass(stream); break;
		case Unit_Tank:						newObject = new TankClass(stream); break;
		case Unit_Trike:					newObject = new TrikeClass(stream); break;
		case Unit_Raider:					newObject = new RaiderClass(stream); break;
		case Unit_Trooper:					newObject = new TrooperClass(stream); break;
		case Unit_Sardaukar:				newObject = new SardaukarClass(stream); break;
		case Unit_Fremen:					newObject = new FremenClass(stream); break;

		default:							newObject = NULL;
											fprintf(stderr,"ObjectClass::loadObject(): %d is no valid ItemID!\n",ItemID);
											break;
	}

	if(newObject == NULL)
		return NULL;

	newObject->setObjectID(ObjectID);

	return newObject;
}

bool ObjectClass::targetInWeaponRange() const {
    Coord coord = (target.getObjPointer())->getClosestPoint(location);
    double dist = blockDistance(location,coord);
    return ( dist <= currentGame->objectData.data[itemID].weaponrange);
}

