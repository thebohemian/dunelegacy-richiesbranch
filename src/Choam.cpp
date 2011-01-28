#include <Choam.h>

#include <Game.h>
#include <House.h>
#include <globals.h>

#include <FileClasses/TextManager.h>

#include <algorithm>

// change starport prices every minute
#define CHOAM_CHANGE_PRICETIME	(MILLI2CYCLES(60*1000))

// change amount of available units every 30s
#define CHOAM_CHANGE_AMOUNT 	(MILLI2CYCLES(30*1000))

Choam::Choam(House* pHouse) : house(pHouse) {
}

Choam::~Choam() {
        ;
}

void Choam::save(Stream& stream) const {
    stream.writeUint32(availableItems.size());
	std::vector<BuildItem>::const_iterator iter;
	for(iter = availableItems.begin(); iter != availableItems.end(); ++iter) {
		iter->save(stream);
	}
}

void Choam::load(Stream& stream) {
	int num = stream.readUint32();
	for(int i=0;i<num;i++) {
		BuildItem tmp;
		tmp.load(stream);
		availableItems.push_back(tmp);
	}
}

int Choam::getPrice(Uint32 ItemID) const {
    std::vector<BuildItem>::const_iterator iter;
    for(iter = availableItems.begin(); iter != availableItems.end(); ++iter) {
        if(iter->ItemID == ItemID) {
            return iter->price;
        }
    }

    return 0;
}

int Choam::getNumAvailable(Uint32 ItemID) const {
    std::vector<BuildItem>::const_iterator iter;
    for(iter = availableItems.begin(); iter != availableItems.end(); ++iter) {
        if(iter->ItemID == ItemID) {
            return iter->num;
        }
    }

    return INVALID;
}

bool Choam::setNumAvailable(Uint32 ItemID, int newValue) {
    std::vector<BuildItem>::iterator iter;
    for(iter = availableItems.begin(); iter != availableItems.end(); ++iter) {
        if(iter->ItemID == ItemID) {
            iter->num = newValue;
            return (iter->num > 0);
        }
    }

    return false;
}

void Choam::addItem(Uint32 ItemID, int num) {
    BuildItem tmp;
    tmp.ItemID = ItemID;
    tmp.num = num;
    availableItems.push_back(tmp);
}

void Choam::update() {
    if(availableItems.empty()) {
        return;
    }

    if((currentGame->GetGameCycleCount() % CHOAM_CHANGE_AMOUNT) == 0) {
        int index = currentGame->RandomGen.rand((Uint32) 0, availableItems.size() - 1);
        availableItems[index].num = std::min(availableItems[index].num + 1, (Uint32) 10);
    }


    if((currentGame->GetGameCycleCount() % CHOAM_CHANGE_PRICETIME) == 0) {
        std::vector<BuildItem>::iterator iter;
        for(iter = availableItems.begin(); iter != availableItems.end(); ++iter) {
            int price = currentGame->objectData.data[iter->ItemID].price;

            const int min_mod = 4;
            const int max_mod = 15;

            price = std::min(currentGame->RandomGen.rand(min_mod, max_mod)*(price/10), 999);

            iter->price = price;
        }

        if((pLocalHouse == house) && (house->hasStarPort())) {
            currentGame->AddToNewsTicker(pTextManager->getLocalized("New Starport prices"));
        }
    }
}
