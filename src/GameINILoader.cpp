#include <GameINILoader.h>

#include <FileClasses/FileManager.h>

#include <Game.h>
#include <House.h>
#include <players/Player.h>
#include <players/AIPlayer.h>
#include <players/HumanPlayer.h>
#include <House.h>
#include <MapClass.h>
#include <ScreenBorder.h>
#include <MapSeed.h>
#include <GameInitSettings.h>
#include <RadarView.h>

#include <structures/StructureClass.h>
#include <units/UnitClass.h>
#include <structures/BuilderClass.h>

#include <Trigger/ReinforcementTrigger.h>
#include <Trigger/TimeoutTrigger.h>

#include <misc/draw_util.h>
#include <misc/string_util.h>

#include <sand.h>
#include <globals.h>

#include <stdio.h>
#include <stdexcept>
#include <algorithm>

GameINILoader::GameINILoader(Game* pGameClass, std::string mapname)
{
    this->pGameClass = pGameClass;
    this->mapname = mapname;

    if(pGameClass->gameType == GAMETYPE_CAMPAIGN || pGameClass->gameType == GAMETYPE_SKIRMISH) {
        // load from PAK-File
        SDL_RWops* mapiniFile = NULL;
        try {

            mapiniFile = pFileManager->OpenFile(this->mapname);
            inifile = std::shared_ptr<INIFile>(new INIFile(mapiniFile));
            SDL_RWclose(mapiniFile);
        } catch (...) {
            if(mapiniFile != NULL) {
                SDL_RWclose(mapiniFile);
            }
            throw;
        }
    } else {
        inifile = std::shared_ptr<INIFile>(new INIFile(mapname));
    }

    load();
}

GameINILoader::~GameINILoader() {
}


/**
	Loads a map from an INI-File.
*/
void GameINILoader::load()
{
    loadMap();
    loadHouses();
    loadUnits();
    loadStructures();
    loadReinforcements();
    loadView();
    loadChoam();
}

/**
    This method loads the game map. This is based on the [MAP] section in the INI file.
*/
void GameINILoader::loadMap() {
    version = inifile->getIntValue("BASIC", "Version", 1);

	pGameClass->winFlags = inifile->getIntValue("BASIC","WinFlags",3);
	pGameClass->loseFlags = inifile->getIntValue("BASIC","LoseFlags",1);

	if(pGameClass->techLevel == 0) {
        pGameClass->techLevel = inifile->getIntValue("BASIC","TechLevel",8);
	}

	int timeout = inifile->getIntValue("BASIC","TIMEOUT",0);

	if((timeout != 0) && ((pGameClass->winFlags & WINLOSEFLAGS_TIMEOUT) != 0)) {
	    std::shared_ptr<Trigger> newTrigger = std::shared_ptr<Trigger>(new TimeoutTrigger(MILLI2CYCLES(timeout * 60 * 1000)));
        pGameClass->GetTriggerManager().addTrigger(newTrigger);
	}

    if(version < 2) {
        if(inifile->hasKey("MAP","Seed") == false) {
            throw std::runtime_error("GameINILoader::loadMap(): Cannot find seed value for this map");
        }

	    // old map format with seed value

	    int mapscale = inifile->getIntValue("BASIC","MapScale",0);

	    switch(mapscale) {
            case 0: {
                sizeX = 62;
                sizeY = 62;
                logicalOffsetX = 1;
                logicalOffsetY = 1;
            } break;

            case 1: {
                sizeX = 32;
                sizeY = 32;
                logicalOffsetX = 16;
                logicalOffsetY = 16;
            } break;

            case 2: {
                sizeX = 21;
                sizeY = 21;
                logicalOffsetX = 11;
                logicalOffsetY = 11;
            } break;

            default: {
                throw std::runtime_error("GameINILoader::loadMap(): Unknown MapScale!");
            } break;
	    }

	    logicalSizeX = 64;
	    logicalSizeY = 64;

	    currentGameMap = new MapClass(sizeX, sizeY);

	    int SeedNum = inifile->getIntValue("MAP","Seed",-1);
        Uint16 SeedMap[64*64];
        createMapWithSeed(SeedNum,SeedMap);

        for (int j = 0; j < currentGameMap->sizeY; j++) {
            for (int i = 0; i < currentGameMap->sizeX; i++) {
                int type = Terrain_Sand;
                unsigned char seedmaptype = SeedMap[(j+logicalOffsetY)*64+i+logicalOffsetX] >> 4;
                switch(seedmaptype) {

                    case 0x7:   /* Sand */
                        type = Terrain_Sand;
                        break;

                    case 0x2:   /* Building */
                    case 0x8:	/* Rock */
                        type = Terrain_Rock;
                        break;

                    case 0x9:   /* Dunes */
                        type = Terrain_Dunes;
                        break;

                    case 0xa:   /* Mountain */
                        type = Terrain_Mountain;
                        break;

                    case 0xb:	/* Spice */
                        type = Terrain_Spice;
                        break;

                    case 0xc:   /* ThickSpice */
                        type = Terrain_ThickSpice;
                        break;

                    default:
                        throw std::runtime_error("GameINILoader::loadMap(): Unknown maptype!");
                        break;
                }

                currentGameMap->cell[i][j].setType(type);
            }

        }

        currentGameMap->createSandRegions();

        std::string BloomString = inifile->getStringValue("MAP","Bloom");
        if(BloomString != "") {
            std::vector<std::string> BloomPositions  = SplitString(BloomString);

            for(unsigned int i=0; i < BloomPositions.size();i++) {
                // set bloom
                int BloomPos = atol(BloomPositions[i].c_str());
                if((BloomPos != 0) || (BloomPositions[i] == "0")) {
                    int xpos = getXPos(BloomPos);
                    int ypos = getYPos(BloomPos);
                    if(currentGameMap->cellExists(xpos,ypos) && currentGameMap->getCell(xpos,ypos)->getType() == Terrain_Sand) {
                        currentGameMap->cell[xpos][ypos].setType(Terrain_SpiceBloom);
                    } else {
                        fprintf(stderr,"GameINILoader::loadMap(): Cannot set bloom at %d, %d\n",xpos, ypos);
                    }
                }
            }

        }

        std::string SpecialString = inifile->getStringValue("MAP","Special");
        if(SpecialString != "") {
            std::vector<std::string> SpecialPositions  = SplitString(SpecialString);

            for(unsigned int i=0; i < SpecialPositions.size();i++) {
                // set special
                int SpecialPos = atol(SpecialPositions[i].c_str());
                if((SpecialPos != 0) || (SpecialPositions[i] == "0")) {
                    int xpos = getXPos(SpecialPos);
                    int ypos = getYPos(SpecialPos);
                    if(currentGameMap->cellExists(xpos,ypos) && currentGameMap->getCell(xpos,ypos)->getType() == Terrain_Sand) {
                        currentGameMap->cell[xpos][ypos].setType(Terrain_SpecialBloom);
                    } else {
                        fprintf(stderr,"GameINILoader::loadMap(): Cannot set special at %d, %d\n",xpos, ypos);
                    }
                }
            }

        }

        std::string FieldString = inifile->getStringValue("MAP","Field");
        if(FieldString != "") {
            std::vector<std::string> FieldPositions  = SplitString(FieldString);

            for(unsigned int i=0; i < FieldPositions.size();i++) {
                // set bloom
                int FieldPos = atol(FieldPositions[i].c_str());
                if((FieldPos != 0) || (FieldPositions[i] == "0")) {
                    int xpos = getXPos(FieldPos);
                    int ypos = getYPos(FieldPos);
                    if(currentGameMap->cellExists(xpos,ypos)) {
                        for(int i = -5; i <= 5; i++) {
                            for(int j = -5; j <= 5; j++) {
                                if(currentGameMap->cellExists(xpos + i, ypos + j)) {
                                    TerrainClass* cell = currentGameMap->getCell(xpos + i, ypos + j);

                                    if(cell->isSand() && (distance_from(xpos, ypos, xpos + i, ypos + j) <= 5)) {
                                        cell->setType(((i==0)&&(j==0)) ? Terrain_ThickSpice : Terrain_Spice);
                                    }
                                }
                            }
                        }
                    } else {
                        fprintf(stderr,"GameINILoader::loadMap(): Cannot set field at %d, %d\n",xpos, ypos);
                    }
                }
            }

        }

	} else {
        // new map format with saved map
        sizeX = inifile->getIntValue("MAP","SizeX", 0);
        sizeY = inifile->getIntValue("MAP","SizeY", 0);

        logicalSizeX = sizeX;
        logicalSizeY = sizeY;
	    logicalOffsetX = 0;
	    logicalOffsetY = 0;

        currentGameMap = new MapClass(sizeX, sizeY);

        for(int y=0;y<sizeY;y++) {
            std::string rowKey = strprintf("%.3d", y);
            std::string rowString = inifile->getStringValue("MAP",rowKey);
            for(int x=0;x<sizeX;x++) {
                int type = Terrain_Sand;

                switch(rowString.at(x)) {
                    case '-': {
                        // Normal sand
                        type = Terrain_Sand;
                    } break;

                    case '^': {
                        // Sand dunes
                        type = Terrain_Dunes;
                    } break;

                    case '~': {
                        // Spice
                        type = Terrain_Spice;
                    } break;

                    case '+': {
                        // Thick spice
                        type = Terrain_ThickSpice;
                    } break;

                    case '%': {
                        // Rock
                        type = Terrain_Rock;
                    } break;

                    case '@': {
                        // Mountain
                        type = Terrain_Mountain;
                    } break;

                    case 'O': {
                        // Spice Bloom
                        type = Terrain_SpiceBloom;
                    } break;

                    case 'Q': {
                        // Special Bloom
                        type = Terrain_SpecialBloom;
                    } break;
                }

                currentGameMap->cell[x][y].setType(type);
            }
        }

        currentGameMap->createSandRegions();
	}

	screenborder->adjustScreenBorderToMapsize();
}

/**
    This method loads the houses on the map specified by the various house sections in the INI file ([Atreides], [Ordos], [Harkonnen]).
*/
void GameINILoader::loadHouses()
{
    const GameInitSettings::HouseInfoList& houseInfoList = pGameClass->getGameInitSettings().getHouseInfoList();
    GameInitSettings::HouseInfoList::const_iterator iter;

    // find "player?" sections
    std::vector<std::string> playerSectionsOnMap;
    for(int i=1;i<=NUM_HOUSES;i++) {
        std::string sectionname = "player" + stringify(i);
        if(inifile->hasSection(sectionname)) {
            playerSectionsOnMap.push_back(sectionname);
        }
    }

    // find unbounded houses
    std::vector<HOUSETYPE> unboundedHouses;

    for(int h=0;h<NUM_HOUSES;h++) {
        bool bFound = false;
        for(iter = houseInfoList.begin(); iter != houseInfoList.end(); ++iter) {
            if(iter->HouseID == (HOUSETYPE) h) {
                bFound = true;
                break;
            }
        }

        std::string houseName = getHouseNameByNumber((HOUSETYPE) h);
        if((bFound == false) && (inifile->hasSection(houseName) || (playerSectionsOnMap.empty() == false))) {
            unboundedHouses.push_back((HOUSETYPE) h);
        }
    }

    // init housename2house mapping with every house section marked as unused
    for(int i=0;i<NUM_HOUSES;i++) {
        std::string houseName = getHouseNameByNumber((HOUSETYPE) i);
        convertToLower(houseName);

        if(inifile->hasSection(houseName)) {
            housename2house[houseName] = HOUSE_UNUSED;
        }
    }

    // init housename2house mapping with every player section on map marked as unused
    std::vector<std::string>::iterator playerSectionsOnMapIter;
    for(playerSectionsOnMapIter = playerSectionsOnMap.begin(); playerSectionsOnMapIter != playerSectionsOnMap.end(); ++playerSectionsOnMapIter) {
        housename2house[*playerSectionsOnMapIter] = HOUSE_UNUSED;
    }

    // now set up all the houses
    for(iter = houseInfoList.begin(); iter != houseInfoList.end(); ++iter) {

        HOUSETYPE houseID;

        if(iter->HouseID == HOUSE_INVALID) {
            // random house => select one unbound house
            if(unboundedHouses.empty()) {
                // skip this house
                continue;
            }
            int randomIndex = pGameClass->RandomGen.rand(0, (int) unboundedHouses.size() - 1);
            houseID = unboundedHouses[randomIndex];
            unboundedHouses.erase(unboundedHouses.begin() + randomIndex);
        } else {
            houseID = iter->HouseID;
        }

        std::string houseName = getHouseNameByNumber(houseID);
        convertToLower(houseName);

        if(inifile->hasSection(houseName) == false) {
            // select one of the Player sections
            if(playerSectionsOnMap.empty()) {
                // skip this house
                continue;
            }

            int randomIndex = pGameClass->RandomGen.rand(0, (int) playerSectionsOnMap.size() - 1);
            houseName = playerSectionsOnMap[randomIndex];
            playerSectionsOnMap.erase(playerSectionsOnMap.begin() + randomIndex);
        }

        housename2house[houseName] = houseID;

        int startingCredits = inifile->getIntValue(houseName,"Credits",DEFAULT_STARTINGCREDITS);
        int quota = inifile->getIntValue(houseName,"Quota",0);

        House* pNewHouse = new House(houseID, startingCredits, iter->team, quota);
        pGameClass->house[houseID] = pNewHouse;

        // add players
        GameInitSettings::HouseInfo::PlayerInfoList::const_iterator iter2;
        for(iter2 = iter->playerInfoList.begin(); iter2 != iter->playerInfoList.end(); ++iter2) {
            if(iter2->PlayerClass == "HumanPlayer") {
                pNewHouse->addPlayer(std::shared_ptr<Player>(new HumanPlayer(pNewHouse)));
                pLocalHouse = pNewHouse;
            } else if(iter2->PlayerClass == "AIPlayer") {
                DIFFICULTYTYPE difficulty = (DIFFICULTYTYPE) atol(iter2->PlayerClassParameter.c_str());

                pNewHouse->addPlayer(std::shared_ptr<Player>(new AIPlayer(pNewHouse, difficulty)));
                pGameClass->house[houseID] = pNewHouse;
            } else {
                fprintf(stderr, "Warning: Cannot load '%s'!\n", iter2->PlayerClass.c_str());
            }
        }
	}
}

/**
    This method loads the choam section of the INI file
*/
void GameINILoader::loadChoam()
{
    INIFile::KeyIterator iter;

    for(iter = inifile->begin("CHOAM"); iter != inifile->end("CHOAM"); ++iter) {
        std::string UnitStr = iter->getKeyName();

        Uint32 unitID = getItemIDByName(UnitStr);
        if((unitID == ItemID_Invalid) || !isUnit(unitID)) {
            throw std::runtime_error("GameINILoader::loadChoam(): Invalid unit string: " + UnitStr);
        }

        int num = iter->getIntValue(-2);
        if(num == -2) {
            throw std::runtime_error("GameINILoader::loadChoam(): Invalid number");
        }

        if(num == -1) {
            num = 0;
        }

        for(int i=0;i<MAX_PLAYERS;i++) {
            if(pGameClass->house[i] != NULL) {
                pGameClass->house[i]->getChoam().addItem(unitID, num);
            }
        }
    }
}

/**
    This method loads the units specified by the [Units] section.
*/
void GameINILoader::loadUnits()
{
    INIFile::KeyIterator iter;

    for(iter = inifile->begin("UNITS"); iter != inifile->end("UNITS"); ++iter) {
		std::string HouseStr, UnitStr, health, PosStr, rotation, mode;
		SplitString(iter->getStringValue(),6,&HouseStr,&UnitStr,&health,&PosStr,&rotation,&mode);

        int houseID = getHouseID(HouseStr);
        if(houseID == HOUSE_UNUSED) {
            // skip unit for unused house
            continue;
        } else if(houseID == HOUSE_INVALID) {
            throw std::runtime_error("GameINILoader::loadUnits(): Invalid house string: " + HouseStr);
        }

		int pos = atol(PosStr.c_str());

		if(pos <= 0) {
		    throw std::runtime_error("GameINILoader::loadUnits(): Invalid position string: " + PosStr);
		}

		int Num2Place = 1;
		int unitID;

		if(UnitStr == "Infantry")	{
			//make three
			unitID = Unit_Soldier;
			Num2Place = 3;
		} else if (UnitStr == "Troopers") {
			//make three
			unitID = Unit_Trooper;
			Num2Place = 3;
		} else {
            unitID = getItemIDByName(UnitStr);
            if((unitID == ItemID_Invalid) || !isUnit(unitID)) {
                throw std::runtime_error("GameINILoader::loadUnits(): Invalid unit string: " + UnitStr);
            }
		}

		double percentHealth = std::min(((double) atol(health.c_str())) / 256.0, 1.0);

		ATTACKMODE attackmode = getAttackModeByName(mode);
		if(attackmode == ATTACKMODE_INVALID) {
            throw std::runtime_error("GameINILoader::loadUnits(): Invalid attackmode string: " + mode);
		}

		for(int i = 0; i < Num2Place; i++) {
			UnitClass* newUnit = getOrCreateHouse(houseID)->placeUnit(unitID, getXPos(pos), getYPos(pos));
			if(newUnit == NULL) {
				fprintf(stderr, "GameINILoader::loadUnits(): Invalid or occupied position: %d!\n", pos);
			} else {
			    newUnit->setHealth(lround(newUnit->getMaxHealth() * percentHealth));
                newUnit->DoSetAttackMode(attackmode);
			}
		}
	}
}

/**
    This method loads the structures specified by the [Structures] section.
*/
void GameINILoader::loadStructures()
{
    INIFile::KeyIterator iter;

    for(iter = inifile->begin("STRUCTURES"); iter != inifile->end("STRUCTURES"); ++iter) {
		std::string tmpkey = iter->getKeyName();
		std::string tmp = iter->getStringValue();

		if(tmpkey.find("GEN") == 0) {
			// Gen Object/Structure
			std::string PosStr = tmpkey.substr(3,tmpkey.size()-3);
			int pos = atol(PosStr.c_str());

			std::string HouseStr, BuildingStr;
			SplitString(tmp,2,&HouseStr,&BuildingStr);

			int houseID = getHouseID(HouseStr);
			if(houseID == HOUSE_UNUSED) {
                // skip structure for unused house
                continue;
            } else if(houseID == HOUSE_INVALID) {
				throw std::runtime_error("GameINILoader::loadStructures(): Invalid house string: " + HouseStr);
			}

			if(BuildingStr == "Concrete") {
				getOrCreateHouse(houseID)->placeStructure(NONE, Structure_Slab1, getXPos(pos), getYPos(pos));
			} else if(BuildingStr == "Wall") {
				getOrCreateHouse(houseID)->placeStructure(NONE, Structure_Wall, getXPos(pos), getYPos(pos));
			} else {
			    throw std::runtime_error("GameINILoader::loadStructures(): Invalid building string: " + BuildingStr);
			}
		} else {
			// other structure
			std::string HouseStr, BuildingStr, health, PosStr;
			SplitString(tmp,6,&HouseStr,&BuildingStr,&health,&PosStr);

			int pos = atol(PosStr.c_str());

			int houseID = getHouseID(HouseStr);
            if(houseID == HOUSE_UNUSED) {
                // skip structure for unused house
                continue;
            } else if(houseID == HOUSE_INVALID) {
				throw std::runtime_error("GameINILoader::loadStructures(): Invalid house string: " + HouseStr);
			}

			double percentHealth = std::min(((double) atol(health.c_str())) / 256.0, 1.0);

			int itemID = getItemIDByName(BuildingStr);

			if((itemID == ItemID_Invalid) || !isStructure(itemID)) {
				throw std::runtime_error("GameINILoader::loadStructures(): Invalid building string: " + BuildingStr);
			}

			if (itemID != 0) {
				ObjectClass* newStructure = getOrCreateHouse(houseID)->placeStructure(NONE, itemID, getXPos(pos), getYPos(pos));
				if(newStructure == NULL) {
					fprintf(stderr,"GameINILoader::loadStructures(): Invalid or occupied position: %s\n",PosStr.c_str());
				} else {
				    newStructure->setHealth(lround(newStructure->getMaxHealth() * percentHealth));
				}
			}
		}
	}
}

/**
    This method loads the reinforcements from the [REINFORCEMENTS] section.
*/
void GameINILoader::loadReinforcements()
{
    INIFile::KeyIterator iter;

    for(iter = inifile->begin("REINFORCEMENTS"); iter != inifile->end("REINFORCEMENTS"); ++iter) {
		std::string strHouseName;
		std::string strUnitName;
		std::string strDropLocation;
		std::string strTime;
		std::string strPlus;

        if(SplitString(iter->getStringValue(), 4, &strHouseName, &strUnitName, &strDropLocation, &strTime) == false) {
            if(SplitString(iter->getStringValue(), 5, &strHouseName, &strUnitName, &strDropLocation, &strTime, &strPlus) == false) {
                throw std::runtime_error("GameINILoader::loadReinforcements(): Invalid reinforcement string: " + iter->getKeyName() + " = " + iter->getStringValue());
            }
        }

        int houseID = getHouseID(strHouseName);
        if(houseID == HOUSE_UNUSED) {
            // skip reinforcement for unused house
            continue;
        } else if(houseID == HOUSE_INVALID) {
            throw std::runtime_error("GameINILoader::loadReinforcements(): Invalid house string \"" + strHouseName + "\": " + iter->getKeyName() + " = " + iter->getStringValue());
        }

        int Num2Drop = 1;
        Uint32 unitID = 0;
        if(strUnitName == "Infantry")	{
			//make three
			unitID = Unit_Soldier;
			Num2Drop = 3;
		} else if (strUnitName == "Troopers") {
			//make three
			unitID = Unit_Trooper;
			Num2Drop = 3;
		} else {
            unitID = getItemIDByName(strUnitName);
            if((unitID == ItemID_Invalid) || !isUnit(unitID)) {
                throw std::runtime_error("GameINILoader::loadReinforcements(): Invalid unit string \"" + strUnitName + "\": " + iter->getKeyName() + " = " + iter->getStringValue());
            }
		}

        convertToLower(strDropLocation);

        ReinforcementTrigger::DropLocation dropLocation;
        if(strDropLocation == "north") {
            dropLocation = ReinforcementTrigger::Drop_North;
        } else if(strDropLocation == "east") {
            dropLocation = ReinforcementTrigger::Drop_East;
        } else if(strDropLocation == "south") {
            dropLocation = ReinforcementTrigger::Drop_South;
        } else if(strDropLocation == "west") {
            dropLocation = ReinforcementTrigger::Drop_West;
        } else if(strDropLocation == "air") {
            dropLocation = ReinforcementTrigger::Drop_Air;
        } else if(strDropLocation == "visible") {
            dropLocation = ReinforcementTrigger::Drop_Visible;
        } else if(strDropLocation == "enemybase") {
            dropLocation = ReinforcementTrigger::Drop_Enemybase;
        } else if(strDropLocation == "homebase") {
            dropLocation = ReinforcementTrigger::Drop_Homebase;
        } else {
            throw std::runtime_error("GameINILoader::loadReinforcements(): Invalid drop location string \"" + strDropLocation + "\": " + iter->getKeyName() + " = " + iter->getStringValue());
        }

        Uint32 droptime = atol(strTime.c_str());
        if(droptime == 0 && strTime != "0" && strTime != "0+") {
            throw std::runtime_error("GameINILoader::loadReinforcements(): Invalid drop time string \"" + strTime + "\": " + iter->getKeyName() + " = " + iter->getStringValue());
        }
        Uint32 dropCycle = MILLI2CYCLES(droptime * 60 * 1000);

        bool bRepeat = (strTime.rfind('+') == (strTime.length() - 1)) || (strPlus == "+");

        for(int i=0;i<Num2Drop;i++) {
            // check if there is a similar trigger at the same time
            const std::list< std::shared_ptr<Trigger> >& triggerList = pGameClass->GetTriggerManager().getTriggers();
            std::list< std::shared_ptr<Trigger> >::const_iterator iter;

            bool bInserted = false;
            for(iter = triggerList.begin(); iter != triggerList.end(); ++iter) {
                Trigger *pTrigger = iter->get();

                ReinforcementTrigger* pReinforcementTrigger = dynamic_cast<ReinforcementTrigger*>(pTrigger);

                if(pReinforcementTrigger != NULL
                    && pReinforcementTrigger->getCycleNumber() == dropCycle
                    && pReinforcementTrigger->getHouseID() == houseID
                    && pReinforcementTrigger->isRepeat() == bRepeat
                    && pReinforcementTrigger->getDropLocation() == dropLocation) {

                    // add the new reinforcement to this reinforcement (call only one carryall)
                    pReinforcementTrigger->addUnit(unitID);
                    bInserted = true;
                    break;
                }
            }

            if(bInserted == false) {
                std::shared_ptr<Trigger> newTrigger = std::shared_ptr<Trigger>(new ReinforcementTrigger(houseID, unitID, dropLocation, bRepeat, dropCycle));
                pGameClass->GetTriggerManager().addTrigger(newTrigger);
            }
        }
	}
}

/**
    This method sets up the view specified by "TacticalPos" in the [BASIC] section.
*/
void GameINILoader::loadView()
{
    if(inifile->hasKey("BASIC", "TacticalPos")) {
        int tacticalPosInt = inifile->getIntValue("BASIC","TacticalPos",-10000) + 64*5 + 7;
        Coord tacticalPos(getXPos(tacticalPosInt), getYPos(tacticalPosInt));

        if(tacticalPos.x < 0 || tacticalPos.x >= sizeX || tacticalPos.y < 0 || tacticalPos.y >= sizeY) {
            fprintf(stderr,"Warning: Invalid tacticalPosInt!\n");
            pGameClass->setupView();
        } else {
            screenborder->SetNewScreenCenter(tacticalPos*BLOCKSIZE);
        }
    } else {
        pGameClass->setupView();
    }
}

/**
    This method returns the house object of the specified house id. If it does not already exist a new AI Player is created and returned.
    \param houseID the house to return or create
    \return the house specified by house
*/
House* GameINILoader::getOrCreateHouse(int houseID) {
    House* pNewHouse = pGameClass->house[houseID];

    if(pNewHouse == NULL) {
        pNewHouse = new House(houseID, houseID, 0, 0);

        const GameInitSettings::HouseInfoList& houseInfoList = pGameClass->getGameInitSettings().getHouseInfoList();

        std::vector<GameInitSettings::HouseInfo>::const_iterator iter;
        for(iter = houseInfoList.begin(); iter != houseInfoList.end(); ++iter) {
            if(iter->HouseID == houseID) {
                GameInitSettings::HouseInfo::PlayerInfoList::const_iterator iter2;
                for(iter2 = iter->playerInfoList.begin(); iter2 != iter->playerInfoList.end(); ++iter2) {
                    if(iter2->PlayerClass == "HumanPlayer") {
                        pNewHouse->addPlayer(std::shared_ptr<Player>(new HumanPlayer(pNewHouse)));
                        pLocalHouse = pNewHouse;
                    } else if(iter2->PlayerClass == "AIPlayer") {
                        DIFFICULTYTYPE difficulty = (DIFFICULTYTYPE) atol(iter2->PlayerClassParameter.c_str());

                        pNewHouse->addPlayer(std::shared_ptr<Player>(new AIPlayer(pNewHouse, difficulty)));
                        pGameClass->house[houseID] = pNewHouse;
                    } else {
                        fprintf(stderr, "Warning: Cannot load '%s'!\n", iter2->PlayerClass.c_str());
                    }
                }
                break;
            }
        }

        pGameClass->house[houseID] = pNewHouse;
    }

    return pNewHouse;
}

HOUSETYPE GameINILoader::getHouseID(std::string name) {
    convertToLower(name);

    if(housename2house.count(name) > 0) {
        return housename2house[name];
    } else {
        return getHouseByName(name);
    }
}

/**
    This static method is used to create a mini map of a map file before the map is being played (e.g. in the map selection menu).
    The surface is always 128x128 pixels and the map is scaled appropriately.
    \param  map the ini file of the map
    \return the minimap (128x128)
*/
SDL_Surface* GameINILoader::createMinimapImageOfMap(const INIFile& map) {
    SDL_Surface* pMinimap;
    // create surface
	if((pMinimap = SDL_CreateRGBSurface(SDL_HWSURFACE,128,128,8,0,0,0,0))== NULL) {
		return NULL;
	}
	palette.applyToSurface(pMinimap);

    int version = map.getIntValue("BASIC", "Version", 1);

    int offsetX = 0;
    int offsetY = 0;
    int scale = 1;
    int sizeX = 64;
    int sizeY = 64;
	int logicalSizeX = 64;
	int logicalSizeY = 64;
    int logicalOffsetX = 0;
    int logicalOffsetY = 0;

    if(version < 2) {
        // old map format with seed value
        int SeedNum = map.getIntValue("MAP","Seed",-1);

        if(SeedNum == -1) {
            throw std::runtime_error("GameINILoader::createMinimapImageOfMap(): Cannot read Seed in this map!");
        }

	    int mapscale = map.getIntValue("BASIC","MapScale",0);

	    switch(mapscale) {
            case 0: {
                scale = 2;
                sizeX = 62;
                sizeY = 62;
                offsetX = 2;
                offsetY = 2;
                logicalOffsetX = 1;
                logicalOffsetY = 1;
            } break;

            case 1: {
                scale = 4;
                sizeX = 32;
                sizeY = 32;
                logicalOffsetX = 16;
                logicalOffsetY = 16;
            } break;

            case 2: {
                scale = 5;
                sizeX = 21;
                sizeY = 21;
                offsetX = 11;
                offsetY = 11;
                logicalOffsetX = 11;
                logicalOffsetY = 11;
            } break;

            default: {
                throw std::runtime_error("GameINILoader::createMinimapImageOfMap(): Unknown MapScale!");
            } break;
	    }

	    logicalSizeX = 64;
	    logicalSizeY = 64;


        Uint16 SeedMap[64*64];
        createMapWithSeed(SeedNum,SeedMap);

        // "draw" spice fields into SeedMap
        std::string FieldString = map.getStringValue("MAP","Field");
        if(FieldString != "") {
            std::vector<std::string> FieldPositions  = SplitString(FieldString);

            for(unsigned int i=0; i < FieldPositions.size();i++) {
                // set bloom
                int FieldPos = atol(FieldPositions[i].c_str());
                if((FieldPos != 0) || (FieldPositions[i] == "0")) {
                    int xpos = FieldPos % logicalSizeX;
                    int ypos = FieldPos / logicalSizeX;
                    if(xpos >= 0 && xpos < sizeX && ypos >= 0 && ypos < logicalSizeY) {

                        for(int x = -5; x <= 5; x++) {
                            for(int y = -5; y <= 5; y++) {

                                if(xpos+x >= 0 && xpos+x < logicalSizeX && ypos+y >= 0 && ypos+y < logicalSizeY) {
                                    if((SeedMap[64*(ypos+y) + (xpos+x)] == 0x7)
                                        && (distance_from(xpos, ypos, xpos + x, ypos + y) <= 5)) {

                                        SeedMap[64*(xpos+x) + (ypos+y)] = (x==0 && y==0) ? 0xc : 0xb;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        for(int y = 0; y < sizeY; y++) {
            for(int x = 0; x < sizeX; x++) {
                int color = COLOR_BLACK;
                unsigned char seedmaptype = SeedMap[(y+logicalOffsetY)*64+x+logicalOffsetX] >> 4;
                switch(seedmaptype) {

                    case 0x7: {
                        // Normal sand
                        color = COLOR_DESERTSAND;
                    } break;

                    case 0x2:
                    case 0x8: {
                        // Rock or building
                        color = COLOR_DARKGREY;
                    } break;

                    case 0x9: {
                        // Sand dunes
                        color = COLOR_DESERTSAND;
                    } break;

                    case 0xa: {
                        // Mountain
                        color = COLOR_MOUNTAIN;
                    } break;

                    case 0xb: {
                        // Spice
                        color = COLOR_SPICE;
                    } break;

                    case 0xc: {
                        // Thick spice
                        color = COLOR_THICKSPICE;
                    } break;
                }

                for(int i=0;i<scale;i++) {
                    for(int j=0;j<scale;j++) {
                        putpixel(pMinimap, x*scale + i + offsetX, y*scale + j + offsetY, color);
                    }
                }
            }
        }

        // draw spice blooms
        std::string BloomString = map.getStringValue("MAP","Bloom");
        if(BloomString != "") {
            std::vector<std::string> BloomPositions  = SplitString(BloomString);

            for(unsigned int i=0; i < BloomPositions.size();i++) {
                // set bloom
                int BloomPos = atol(BloomPositions[i].c_str());
                if((BloomPos != 0) || (BloomPositions[i] == "0")) {
                    int xpos = BloomPos % logicalSizeX - logicalOffsetX;
                    int ypos = BloomPos / logicalSizeX - logicalOffsetY;
                    if(xpos >= 0 && xpos < sizeX && ypos >= 0 && ypos < sizeY) {
                        for(int i=0;i<scale;i++) {
                            for(int j=0;j<scale;j++) {
                                putpixel(pMinimap, xpos*scale + i + offsetX, ypos*scale + j + offsetY, COLOR_RED);
                            }
                        }
                    }
                }
            }
        }

        // draw special blooms
        std::string SpecialString = map.getStringValue("MAP","Special");
        if(SpecialString != "") {
            std::vector<std::string> SpecialPositions  = SplitString(SpecialString);

            for(unsigned int i=0; i < SpecialPositions.size();i++) {
                // set bloom
                int SpecialPos = atol(SpecialPositions[i].c_str());
                if((SpecialPos != 0) || (SpecialPositions[i] == "0")) {
                    int xpos = SpecialPos % logicalSizeX - logicalOffsetX;
                    int ypos = SpecialPos / logicalSizeX - logicalOffsetY;
                    if(xpos >= 0 && xpos < sizeX && ypos >= 0 && ypos < sizeY) {
                        for(int i=0;i<scale;i++) {
                            for(int j=0;j<scale;j++) {
                                putpixel(pMinimap, xpos*scale + i + offsetX, ypos*scale + j + offsetY, COLOR_RED);
                            }
                        }
                    }
                }
            }
        }


    } else {
        // new map format with saved map
        sizeX = map.getIntValue("MAP","SizeX", 0);
        sizeY = map.getIntValue("MAP","SizeY", 0);

        logicalSizeX = sizeX;
        logicalSizeY = sizeY;

        RadarView::calculateScaleAndOffsets(sizeX, sizeY, scale, offsetX, offsetY);

        for(int y=0;y<sizeY;y++) {
            std::string rowKey = strprintf("%.3d", y);
            std::string rowString = map.getStringValue("MAP",rowKey);
            for(int x=0;x<sizeX;x++) {
                int color = COLOR_BLACK;
                switch(rowString.at(x)) {
                    case '-': {
                        // Normal sand
                        color = COLOR_DESERTSAND;
                    } break;

                    case '^': {
                        // Sand dunes
                        color = COLOR_DESERTSAND;
                    } break;

                    case '~': {
                        // Spice
                        color = COLOR_SPICE;
                    } break;

                    case '+': {
                        // Thick spice
                        color = COLOR_THICKSPICE;
                    } break;

                    case '%': {
                        // Rock
                        color = COLOR_DARKGREY;
                    } break;

                    case '@': {
                        // Mountain
                        color = COLOR_MOUNTAIN;
                    } break;

                    case 'O':
                    case 'Q': {
                        // Spice Bloom and Special Bloom
                        color = COLOR_RED;
                    } break;
                }

                for(int i=0;i<scale;i++) {
                    for(int j=0;j<scale;j++) {
                        putpixel(pMinimap, x*scale + i + offsetX, y*scale + j + offsetY, color);
                    }
                }
            }
        }
    }

    // draw structures
    INIFile::KeyIterator iter;

    for(iter = map.begin("STRUCTURES"); iter != map.end("STRUCTURES"); ++iter) {
		std::string tmpkey = iter->getKeyName();
		std::string tmp = iter->getStringValue();

		if(tmpkey.find("GEN") == 0) {
			// Gen Object/Structure

			std::string PosStr = tmpkey.substr(3,tmpkey.size()-3);
			int pos = atol(PosStr.c_str());

			std::string HouseStr, BuildingStr;
			SplitString(tmp,2,&HouseStr,&BuildingStr);

			int house = getHouseByName(HouseStr);
			int color = COLOR_WHITE;
			if(house != HOUSE_INVALID) {
				color = houseColor[house];
			} else {
                convertToLower(HouseStr);
			    if(HouseStr.length() == 7 && HouseStr.substr(0,6) == "player") {
			        int playernum = HouseStr.at(6)-'0';

			        if(playernum >= 1 && playernum <= 6) {
			            color = 128 + playernum - 1;
			        }
			    }
			}

			if(BuildingStr == "Concrete") {
				// nothing
			} else if(BuildingStr == "Wall") {
                int x = pos % logicalSizeX - logicalOffsetX;
                int y = pos / logicalSizeX - logicalOffsetY;

                if(x >= 0 && x < sizeX && y >= 0 && y < sizeY) {
                    for(int i=0;i<scale;i++) {
                        for(int j=0;j<scale;j++) {
                            putpixel(pMinimap, x*scale + i + offsetX, y*scale + j + offsetY, color);
                        }
                    }
                }
			}
		} else {
			// other structure
			std::string HouseStr, BuildingStr, health, PosStr;
			SplitString(tmp,6,&HouseStr,&BuildingStr,&health,&PosStr);

			int pos = atol(PosStr.c_str());

			int house = getHouseByName(HouseStr);
			int color = COLOR_WHITE;
			if(house != HOUSE_INVALID) {
				color = houseColor[house];
			} else {
			    convertToLower(HouseStr);
			    if(HouseStr.length() == 7 && HouseStr.substr(0,6) == "player") {
			        int playernum = HouseStr.at(6)-'0';

			        if(playernum >= 1 && playernum <= 6) {
			            color = 128 + playernum - 1;
			        }
			    }
			}

			Coord size = getStructureSize(getItemIDByName(BuildingStr));

            int posX = pos % logicalSizeX - logicalOffsetX;
            int posY = pos / logicalSizeX - logicalOffsetY;
			for(int x = posX; x < posX + size.x; x++) {
                for(int y = posY; y < posY + size.y; y++) {
                    if(x >= 0 && x < sizeX && y >= 0 && y < sizeY) {
                        for(int i=0;i<scale;i++) {
                            for(int j=0;j<scale;j++) {
                                putpixel(pMinimap, x*scale + i + offsetX, y*scale + j + offsetY, color);
                            }
                        }
                    }
                }
			}
		}
	}

    return pMinimap;
}
