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

#include "ObjectData.h"

#include <globals.h>

#include <FileClasses/FileManager.h>
#include <FileClasses/INIFile.h>
#include <sand.h>

#include <stdlib.h>
#include <iostream>

ObjectData::ObjectData()
{
    // set default values
    for(int i=0;i<ItemID_max;i++) {
        data[i].hitpoints = 0;
        data[i].price = 0;
        data[i].power = 0;
        data[i].viewrange = 0;
        data[i].capacity = 0;
        data[i].weapondamage = 0;
        data[i].weaponrange = 0;
        data[i].maxspeed = 0.0;
        data[i].turnspeed = 0.0;
        data[i].radius = 0;
        data[i].buildtime = 0;
    }
}

ObjectData::~ObjectData()
{
}

void ObjectData::loadFromINIFile(std::string filename)
{
    SDL_RWops *file = pFileManager->OpenFile(filename);
    INIFile objectDataFile(file);
    SDL_RWclose(file);

    // load default structure values
    ObjectDataStruct structureDefaultData;
    structureDefaultData.hitpoints = objectDataFile.getIntValue("default structure", "HitPoints");
    structureDefaultData.price = objectDataFile.getIntValue("default structure", "Price");
    structureDefaultData.power = objectDataFile.getIntValue("default structure", "Power");
    structureDefaultData.viewrange = objectDataFile.getIntValue("default structure", "ViewRange");
    structureDefaultData.capacity = objectDataFile.getIntValue("default structure", "Capacity");
    structureDefaultData.weapondamage = objectDataFile.getIntValue("default structure", "WeaponDamage");
    structureDefaultData.weaponrange = objectDataFile.getIntValue("default structure", "WeaponRange");
    structureDefaultData.maxspeed = objectDataFile.getDoubleValue("default structure", "MaxSpeed");
    structureDefaultData.turnspeed = objectDataFile.getDoubleValue("default structure", "TurnSpeed");
    structureDefaultData.radius = objectDataFile.getIntValue("default structure", "Radius");
    structureDefaultData.buildtime = objectDataFile.getIntValue("default structure", "BuildTime");

    // load default unit values
    ObjectDataStruct unitDefaultData;
    unitDefaultData.hitpoints = objectDataFile.getIntValue("default unit", "HitPoints");
    unitDefaultData.price = objectDataFile.getIntValue("default unit", "Price");
    unitDefaultData.power = objectDataFile.getIntValue("default unit", "Power");
    unitDefaultData.viewrange = objectDataFile.getIntValue("default unit", "ViewRange");
    unitDefaultData.capacity = objectDataFile.getIntValue("default unit", "Capacity");
    unitDefaultData.weapondamage = objectDataFile.getIntValue("default unit", "WeaponDamage");
    unitDefaultData.weaponrange = objectDataFile.getIntValue("default unit", "WeaponRange");
    unitDefaultData.maxspeed = objectDataFile.getDoubleValue("default unit", "MaxSpeed");
    unitDefaultData.turnspeed = objectDataFile.getDoubleValue("default unit", "TurnSpeed");
    unitDefaultData.radius = objectDataFile.getIntValue("default unit", "Radius");
    unitDefaultData.buildtime = objectDataFile.getIntValue("default unit", "BuildTime");

    // set default values
    for(int i=0;i<ItemID_max;i++) {
        if(isStructure(i)) {
            data[i] = structureDefaultData;
        } else {
            data[i] = unitDefaultData;
        }
    }

    INIFile::SectionIterator iter;
    for(iter = objectDataFile.begin(); iter != objectDataFile.end(); ++iter) {
        std::string sectionName = iter->getSectionName();

        if(sectionName == "" || sectionName == "default structure" || sectionName == "default unit") {
            continue;
        }

        Uint32 itemID = getItemIDByName(sectionName);

        if(itemID == ItemID_Invalid) {
            std::cerr << "ObjectData::ObjectData(): \"" << sectionName << "\" is no valid unit/structure name!" << std::endl;
            continue;
        }

        ObjectDataStruct& defaultData = isStructure(itemID) ? structureDefaultData : unitDefaultData;

        data[itemID].hitpoints = objectDataFile.getIntValue(sectionName, "HitPoints", defaultData.hitpoints);
        data[itemID].price = objectDataFile.getIntValue(sectionName, "Price", defaultData.price);
        data[itemID].power = objectDataFile.getIntValue(sectionName, "Power", defaultData.power);
        data[itemID].viewrange = objectDataFile.getIntValue(sectionName, "ViewRange", defaultData.viewrange);
        data[itemID].capacity = objectDataFile.getIntValue(sectionName, "Capacity", defaultData.capacity);
        data[itemID].weapondamage = objectDataFile.getIntValue(sectionName, "WeaponDamage", defaultData.weapondamage);
        data[itemID].weaponrange = objectDataFile.getIntValue(sectionName, "WeaponRange", defaultData.weaponrange);
        data[itemID].maxspeed = objectDataFile.getDoubleValue(sectionName, "MaxSpeed", defaultData.maxspeed);
        data[itemID].turnspeed = objectDataFile.getDoubleValue(sectionName, "TurnSpeed", defaultData.turnspeed);
        data[itemID].radius = objectDataFile.getIntValue(sectionName, "Radius", defaultData.radius);
        data[itemID].buildtime = objectDataFile.getIntValue(sectionName, "BuildTime", defaultData.buildtime);
    }
}

void ObjectData::save(Stream& stream) const
{
    for(int i=0;i<ItemID_max;i++) {
        stream.writeSint32(data[i].hitpoints);
        stream.writeSint32(data[i].price);
        stream.writeSint32(data[i].power);
        stream.writeSint32(data[i].viewrange);
        stream.writeSint32(data[i].capacity);
        stream.writeSint32(data[i].weapondamage);
        stream.writeSint32(data[i].weaponrange);
        stream.writeDouble(data[i].maxspeed);
        stream.writeDouble(data[i].turnspeed);
        stream.writeSint32(data[i].radius);
        stream.writeSint32(data[i].buildtime);
    }
}

void ObjectData::load(Stream& stream)
{
    for(int i=0;i<ItemID_max;i++) {
        data[i].hitpoints = stream.readSint32();
        data[i].price = stream.readSint32();
        data[i].power = stream.readSint32();
        data[i].viewrange = stream.readSint32();
        data[i].capacity = stream.readSint32();
        data[i].weapondamage = stream.readSint32();
        data[i].weaponrange = stream.readSint32();
        data[i].maxspeed = stream.readDouble();
        data[i].turnspeed = stream.readDouble();
        data[i].radius = stream.readSint32();
        data[i].buildtime = stream.readSint32();
    }
}
