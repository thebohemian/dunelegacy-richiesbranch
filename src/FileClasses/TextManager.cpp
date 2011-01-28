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

#include <FileClasses/TextManager.h>

#include <globals.h>

#include <FileClasses/FileManager.h>


TextManager::TextManager() {
    // load all breifing texts
	SDL_RWops* text_lng[3];
	text_lng[0] = pFileManager->OpenFile("TEXTA." + settings.General.LanguageExt);
	text_lng[1] = pFileManager->OpenFile("TEXTO." + settings.General.LanguageExt);
	text_lng[2] = pFileManager->OpenFile("TEXTH." + settings.General.LanguageExt);

	for(int i=0;i<3;i++) {
		if(text_lng[i] == NULL) {
			fprintf(stderr,"TextManager::TextManager: Can not open language file\n");
			exit(EXIT_FAILURE);
		}
		BriefingStrings[i] = std::shared_ptr<BriefingText>(new BriefingText(text_lng[i]));
		SDL_RWclose(text_lng[i]);
	}


	// load dune text
    SDL_RWops* dune_lng = pFileManager->OpenFile("DUNE." + settings.General.LanguageExt);
	pDuneText = std::shared_ptr<IndexedTextFile>(new IndexedTextFile(dune_lng));
	SDL_RWclose(dune_lng);


	// init languages
	switch(settings.General.Language) {
        case LNG_FRE:
            Init_Localization_French();
            break;
        case LNG_GER:
            Init_Localization_German();
            break;
        case LNG_ENG:
        default:
            // nothing
            break;
	}
}

TextManager::~TextManager() {
}

std::string	TextManager::GetBriefingText(unsigned int mission, unsigned int texttype, int house) {
	switch(house) {
		case HOUSE_ATREIDES:
		case HOUSE_FREMEN:
			return BriefingStrings[0]->getMissionBriefingString(mission,texttype);
			break;
		case HOUSE_ORDOS:
		case HOUSE_MERCENARY:
			return BriefingStrings[1]->getMissionBriefingString(mission,texttype);
			break;
		case HOUSE_HARKONNEN:
		case HOUSE_SARDAUKAR:
		default:
			return BriefingStrings[2]->getMissionBriefingString(mission,texttype);
			break;
		}
}

void TextManager::Init_Localization_French() {
    // TODO: Add french localization
}

void TextManager::Init_Localization_German() {

    // General strings
    localizedString["OK"] = "OK";
    localizedString["Back"] = "Zur\374ck";
    localizedString["BACK"] = "ZUR\334CK";
    localizedString["Cancel"] = "Abbrechen";
    localizedString["CANCEL"] = "ABBRECHEN";
    localizedString["Next"] = "Weiter";
    localizedString["NEXT"] = "WEITER";
    localizedString["LOAD GAME"] = "SPIEL LADEN";
    localizedString["Load Game"] = "Spiel laden";
    localizedString["SAVE GAME"] = "SPIEL SPEICHERN";
    localizedString["Save Game"] = "Spiel speichern";
    localizedString["LOAD REPLAY"] = "REPLAY LADEN";
    localizedString["Load Replay"] = "Replay laden";

    // Main Menu
    localizedString["SINGLE PLAYER"] = "EINZELSPIELER";
    localizedString["OPTIONS"] = "OPTIONEN";
    localizedString["ABOUT"] = "\334BER";
    localizedString["QUIT"] = "BEENDEN";

    // Options Menu
    localizedString["Player Name"] = "Spielername";
    localizedString["Concrete required"] = "Betonplatten ben\366tigt";
    localizedString["Fog of War"] = "Nebel des Krieges";
    localizedString["Language"] = "Sprache";
    localizedString["Play Intro"] = "Intro abspielen";
    localizedString["Video Resolution"] = "Bildschirmaufl\366sung";
    localizedString["Full Screen"] = "Vollbild";
    localizedString["Double Buffering"] = "Double Buffering";
    localizedString["Accept"] = "annehmen";

    // Single Player Menu
    localizedString["CAMPAIGN"] = "KAMPAGNE";
    localizedString["CUSTOM GAME"] = "FREIES SPIEL";
    localizedString["SKIRMISH"] = "GEPL\304NKEL";

    // Single Player Skirmish Menu
    localizedString["Start"] = "Starten";

    // Custom Game Menu and Custom Player Menu
    localizedString["Custom Game"] = "Freies Spiel";
    localizedString["SP Maps"] = "SP Karten";
    localizedString["SP User Maps"] = "eigene SP K.";
    localizedString["MP Maps"] = "MP Karten";
    localizedString["MP User Maps"] = "eigene MP K.";
    localizedString["Choose map"] = "Karte w\344hlen";
    localizedString["No map available"] = "Karte nicht verf\374g.";
    localizedString["Size"] = "Gr\366\337e";
    localizedString["Players"] = "Spieler";
    localizedString["Author"] = "Ersteller";
    localizedString["License"] = "Lizenz";
    localizedString["Multiple players per house"] = "Mehrere Spieler pro Haus";
    localizedString["House"] = "Haus";
    localizedString["Team"] = "Team";
    localizedString["Player"] = "Spieler";
    localizedString["Random"] = "Zuf\344llig";
    localizedString["open"] = "offen";
    localizedString["closed"] = "geschlossen";
    localizedString["At least 2 houses must be controlled\nby a human player or an AI player!"] = "Mindestens zwei H\344user m\374ssen von einem menschlichen\nSpieler oder dem Computer gespielt werden!";
    localizedString["There must be at least two different teams!"] = "Es muss mindestens zwei verschiedene Teams geben!";

    // Load/Save Window
    localizedString["Load"] = "Laden";
    localizedString["Save"] = "Speichern";

    // InGame Menu
    localizedString["Resume Game"] = "Wiederaufnehmen";
    localizedString["Game Settings"] = "Spieleinstellungen";
    localizedString["Restart Game"] = "Spiel neustarten";
    localizedString["Quit to Menu"] = "Zur\374ck zum Men\374";

    // BuilderList
    localizedString["SOLD OUT"] = "AUS-\nVERKAUFT";
    localizedString["ALREADY\nBUILT"] = "BEREITS\nGEBAUT";
    localizedString["PLACE IT"] = "PLAZIEREN";
    localizedString["ON HOLD"] = "GESTOPPT";
    localizedString["Order"]= "Bestellen";

    // Builder Interface
    localizedString["Upgrade"] = "Verbessern";

    // Unit Interface
    localizedString["Move to a position"] = "Zu einer Position bewegen";
    localizedString["Attack a unit, structure or position"] = "Eine Einheit, Geb\344ude oder Position angreifen";
    localizedString["Capture a building"] = "Ein Geb\344ude einnehmen";
    localizedString["Guard"] = "Bewachen";
    localizedString["Unit will not move from location"] = "Die Einheit wird sich von ihrem Ort nicht wegbewegen";
    localizedString["Area Guard"] = "\334berwachen";
    localizedString["Unit will engage any unit within guard range"] = "Jede feindliche Einheit in der Umgebung verfolgen";
    localizedString["Stop"] = "Stoppen";
    localizedString["Unit will not move, nor attack"] = "Die Einheit wird sich weder bewegen noch angreifen";
    localizedString["Ambush"] = "Hinterhalt";
    localizedString["Unit will not move until enemy unit spoted"] = "Die Einheit bewegt sich erst, wenn sie einen Gegner sieht";
    localizedString["Hunt"] = "Jagd";
    localizedString["Unit will immediately start to engage an enemy unit"] = "Die Einheit sucht sich sofort einen Gegner";
    localizedString["Deploy"] = "Einsetzen";
    localizedString["Build a new construction yard"] = "Bauhof aufbauen";
    localizedString["Destruct"] = "Zerst\366ren";
    localizedString["Self-destruct this unit"] = "Einheit selbst-zerst\366ren";

    // Windtrap Interface
    localizedString["Required"] = "Ben\366t.";
    localizedString["Produced"] = "Prod.";

    // Refinery and Silo Interface
    localizedString["Capacity"] = "Kapaz.";
    localizedString["Stored"] = "Inhalt";

    // Palace Interace
    localizedString["READY"] = "BEREIT";

    // Choam
    localizedString["New Starport prices"] = "Neue Preise am Starport";

    // Messages from Units and Structures
    localizedString["You cannot deploy here."] = "Kann hier nicht eingesetzt werden.";
    localizedString["This unit is sold out"] = "Diese Einheit ist ausverkauft.";
    localizedString["Not enough money"] = "Nicht gen\374gend Credits.";
    localizedString["Palace is ready"] = "Palast ist bereit.";
    localizedString["Unable to spawn Fremen"] = "Kann Fremen nicht rufen.";

    // Game Class
    localizedString["Screenshot saved"] = "Screenshot gespeichert";
    localizedString["Game speed"] = "Spielgeschwindigkeit";
    localizedString["Paused"] = "Pause";
    localizedString["You Have Completed Your Mission."] = "Mission erfolgreich abgeschlossen";
    localizedString["You Have Failed Your Mission."] = "Mission gescheitert.";

    // House
    localizedString["House '%s' has been defeated."] = "Haus '%s' wurde geschlagen.";

}
