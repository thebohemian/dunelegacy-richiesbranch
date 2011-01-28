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

#include <main.h>

#include <globals.h>

#include <config.h>

#include <FileClasses/FileManager.h>
#include <FileClasses/GFXManager.h>
#include <FileClasses/SFXManager.h>
#include <FileClasses/FontManager.h>
#include <FileClasses/TextManager.h>
#include <FileClasses/INIFile.h>
#include <FileClasses/Palfile.h>
#include <FileClasses/music/DirectoryPlayer.h>
#include <FileClasses/music/ADLPlayer.h>
#include <FileClasses/music/XMIPlayer.h>

#include <GUI/GUIStyle.h>
#include <GUI/dune/DuneStyle.h>

#include <Menu/MainMenu.h>

#include <misc/fnkdat.h>
#include <misc/FileSystem.h>

#include <SoundPlayer.h>

#include <CutScenes/Intro.h>

#include <SDL.h>
#include <SDL_rwops.h>
#include <iostream>
#include <stdexcept>
#include <time.h>
#include <unistd.h>
//#include <sys/types.h>
//#include <sys/stat.h>
#include <fcntl.h>


#ifdef _WIN32
    #include <windows.h>
#endif

#ifdef __APPLE__
	#include <MacFunctions.h>
#endif

void setVideoMode();
void realign_buttons();

void setVideoMode()
{
	int videoFlags = SDL_HWPALETTE;

	if(settings.Video.DoubleBuffering) {
		videoFlags |= SDL_HWSURFACE | SDL_DOUBLEBUF;
	}

	if(settings.Video.Fullscreen) {
		videoFlags |= SDL_FULLSCREEN;
	}

    if(SDL_VideoModeOK(settings.Video.Width, settings.Video.Height, 8, videoFlags) == 0) {
        // should always work
        fprintf(stderr, "WARNING: Falling back to 640x480!\n");
        settings.Video.Width = 640;
        settings.Video.Height = 480;

        if(SDL_VideoModeOK(settings.Video.Width, settings.Video.Height, 8, videoFlags) == 0) {
            // OK, now we switch double buffering, hw-surface and fullscreen off
            fprintf(stderr, "WARNING: Turning off double buffering, hw-surface and fullscreen!\n");
            settings.Video.DoubleBuffering = false;
            settings.Video.Fullscreen = false;
            videoFlags = SDL_HWPALETTE;
        }
    }

	screen = SDL_SetVideoMode(settings.Video.Width, settings.Video.Height, SCREEN_BPP, videoFlags);
	if(screen) {
	    palette.applyToSurface(screen);
		SDL_ShowCursor(SDL_DISABLE);
    } else {
		fprintf(stderr, "ERROR: Couldn't set video mode: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
    }
}

std::string GetConfigFilepath()
{
	// determine path to config file
	char tmp[FILENAME_MAX];
	fnkdat(CONFIGFILENAME, tmp, FILENAME_MAX, FNKDAT_USER | FNKDAT_CREAT);

	return std::string(tmp);
}

std::string GetLogFilepath()
{
	// determine path to config file
	char tmp[FILENAME_MAX];
	fnkdat(LOGFILENAME, tmp, FILENAME_MAX, FNKDAT_USER | FNKDAT_CREAT);

	return std::string(tmp);
}

void CreateDefaultConfigFile(std::string configfilepath, int language) {
	SDL_RWops* file = SDL_RWFromFile(configfilepath.c_str(), "w");

	const char configfile[] =	"[General]\n"
								"Play Intro = false\t\t# Play the intro when starting the game?\n"
								"Concrete Required = true\t# If false we can build without penalties on the bare rock\n"
								"Fog of War = false\t\t# If true explored terrain will become foggy when no unit or structure is next to it\n"
								"Player Name = Player\t\t# The name of the player\n"
								"Language = %s\t\t\t# en = English, fr = French, de = German\n"
								"\n"
								"[Video]\n"
								"# You may decide to use half the resolution of your monitor, e.g. monitor has 1600x1200 => 800x600\n"
								"# Minimum resolution is 640x480\n"
								"Width = 640\n"
								"Height = 480\n"
								"Fullscreen = true\n"
								"Double Buffering = true\n"
								"FrameLimit = true\t\t# Limit the frame rate to save energy\n"
								"\n"
								"[Audio]\n"
								"# There are three different possibilities to play music\n"
								"#  adl\t\t- This option will use the Dune 2 music as used on e.g. SoundBlaster16 cards\n"
								"#  xmi\t\t- This option plays the xmi files of Dune 2. Sounds more midi-like\n"
								"#  directory\t- Plays music from the \"music\"-directory inside your configuration directory\n"
								"#\t\t  The \"music\"-directory should contain 5 subdirectories named attack, intro, peace, win and lose\n"
								"#\t\t  Put any mp3, ogg or mid file there and it will be played in the particular situation\n"
								"Music Type = adl\n";

    char tmpBuffer[sizeof(configfile)];
    const char* lng;

    switch(language) {
        case LNG_ENG:   lng = "en"; break;
        case LNG_FRE:   lng = "fr"; break;
        case LNG_GER:   lng = "de"; break;
        default:
            fprintf(stderr,"CreateDefaultConfigFile(): Unknown language!\n");
            exit(EXIT_FAILURE);
        break;
    }
    // replace language
    sprintf(tmpBuffer, configfile, lng);

	SDL_RWwrite(file, tmpBuffer, 1, strlen(tmpBuffer));

	SDL_RWclose(file);
}

void PrintMissingFilesToScreen() {
	SDL_ShowCursor(SDL_ENABLE);

    std::string instruction = "Dune Legacy uses the data files from original Dune II. The following files are missing:\n";

    std::vector<std::string> MissingFiles = FileManager::getMissingFiles(LNG_ENG);

    std::vector<std::string>::const_iterator iter;
    for(iter = MissingFiles.begin(); iter != MissingFiles.end(); ++iter) {
        instruction += " " + *iter + "\n";
    }

    instruction += "\nPut them in one of the following directories:\n";
    std::vector<std::string> searchPath = FileManager::getSearchPath();
    std::vector<std::string>::const_iterator searchPathIter;
    for(searchPathIter = searchPath.begin(); searchPathIter != searchPath.end(); ++searchPathIter) {
        instruction += " " + *searchPathIter + "\n";
    }

    instruction += "\nYou may want to add GERMAN.PAK or FRENCH.PAK for playing in these languages.\n";
    instruction += "\n\nPress ESC to exit.";

    SDL_Surface* pSurface = pFontManager->createSurfaceWithMultilineText(instruction, 255, FONT_STD12);
    SDL_Rect dest = { 30, 30, pSurface->w, pSurface->h };
    SDL_BlitSurface(pSurface, NULL, screen, &dest);
    SDL_FreeSurface(pSurface);

	SDL_Flip(screen);

	SDL_Event	event;
	bool quiting = false;
	while(!quiting)	{
	    SDL_Delay(20);

		while(SDL_PollEvent(&event)) {
		    //check the events
            switch (event.type)
            {
                case (SDL_KEYDOWN):	// Look for a keypress
                {
                    switch(event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            quiting = true;
                            break;

                        default:
                            break;
                    }
                } break;


                case SDL_QUIT:
                    quiting = true;
                    break;

                default:
                    break;
            }
		}
	}
}

int GetUserLanguage() {
    const char* pLang = NULL;

#if defined (_WIN32)
    char ISO639_LanguageName[10];
    if(GetLocaleInfo(GetUserDefaultLCID(), LOCALE_SISO639LANGNAME, ISO639_LanguageName, sizeof(ISO639_LanguageName)) == 0) {
        return LNG_UNKNOWN;
    } else {
        pLang = ISO639_LanguageName;
    }

#elif defined (__APPLE__)
	pLang = getMacLanguage();
	if(pLang == NULL) {
        return LNG_UNKNOWN;
	}

#else
    // should work most unices
	pLang = getenv("LC_ALL");
    if(pLang == NULL) {
		// try LANG
		pLang = getenv("LANG");
        if(pLang == NULL) {
			return LNG_UNKNOWN;
		}
    }
#endif

    if((pLang[0] == 'f' || pLang[0] == 'F') && (pLang[1] == 'r' || pLang[1] == 'R')) {
        return LNG_FRE;
    } else if((pLang[0] == 'd' || pLang[0] == 'D') && (pLang[1] == 'e' || pLang[1] == 'E')) {
        return LNG_GER;
    } else if((pLang[0] == 'e' || pLang[0] == 'E') && (pLang[1] == 'n' || pLang[1] == 'N')) {
        return LNG_ENG;
    } else {
        return LNG_UNKNOWN;
    }
}

int main(int argc, char *argv[])
{
	// init fnkdat
	if(fnkdat(NULL, NULL, 0, FNKDAT_INIT) < 0) {
      perror("Could not initialize fnkdat");
      exit(EXIT_FAILURE);
	}

	bool bShowDebug = false;
    for(int i=1; i < argc; i++) {
	    //check for overiding params
		if (strcmp(argv[i], "--showlog") == 0)
			bShowDebug = true;
	}

	if(bShowDebug == false) {
        int d = open(GetLogFilepath().c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if(d < 0) {
            perror("Opening logfile failed");
            exit(EXIT_FAILURE);
        }

        if(dup2(d, STDOUT_FILENO) < 0) {
            perror("Redirecting stdout failed");
            exit(EXIT_FAILURE);
        }

        if(dup2(d, STDERR_FILENO) < 0) {
            perror("Redirecting stderr failed");
            exit(EXIT_FAILURE);
        }
	}

    // First check for missing files
    std::vector<std::string> MissingFiles = FileManager::getMissingFiles(LNG_ENG);

    if(MissingFiles.size() > 0) {
        // create data directory inside config directory
        char tmp[FILENAME_MAX];
        fnkdat("data/", tmp, FILENAME_MAX, FNKDAT_USER | FNKDAT_CREAT);

        bool cannotShowMissingScreen = false;
        fprintf(stderr,"The following files are missing:\n");
        std::vector<std::string>::const_iterator iter;
        for(iter = MissingFiles.begin() ; iter != MissingFiles.end(); ++iter) {
            fprintf(stderr," %s\n",iter->c_str());
            if(iter->find("LEGACY.PAK") != std::string::npos) {
                cannotShowMissingScreen = true;
            }
        }

        fprintf(stderr,"Put them in one of the following directories:\n");
        std::vector<std::string> searchPath = FileManager::getSearchPath();
        std::vector<std::string>::const_iterator searchPathIter;
        for(searchPathIter = searchPath.begin(); searchPathIter != searchPath.end(); ++searchPathIter) {
            fprintf(stderr," %s\n",searchPathIter->c_str());
        }

        if(cannotShowMissingScreen == true) {
            return EXIT_FAILURE;
        }
    }

	bool ExitGame = false;
	bool FirstInit = true;
	bool FirstGamestart = false;

    debug = false;
    cursorFrame = UI_CursorNormal;

	do {
		int seed = time(NULL);
		srand(seed);

        // check if configfile exists
        std::string configfilepath = GetConfigFilepath();
        if(ExistsFile(configfilepath) == false) {
            int UserLanguage = GetUserLanguage();
            if(UserLanguage == LNG_UNKNOWN) {
                UserLanguage = LNG_ENG;
            }

            if(MissingFiles.empty() == true) {
                // if all pak files were found we can create the ini file
                FirstGamestart = true;
                CreateDefaultConfigFile(configfilepath, UserLanguage);
            }
        }

		INIFile myINIFile(configfilepath);

		settings.General.PlayIntro = myINIFile.getBoolValue("General","Play Intro",false);
		settings.General.ConcreteRequired = myINIFile.getBoolValue("General","Concrete Required",true);
        settings.General.FogOfWar = myINIFile.getBoolValue("General","Fog of War",false);
		settings.General.PlayerName = myINIFile.getStringValue("General","Player Name","Player");
		settings.Video.Width = myINIFile.getIntValue("Video","Width",640);
		settings.Video.Height = myINIFile.getIntValue("Video","Height",480);
		settings.Video.Fullscreen = myINIFile.getBoolValue("Video","Fullscreen",true);
		settings.Video.DoubleBuffering = myINIFile.getBoolValue("Video","Double Buffering",true);
		settings.Video.FrameLimit = myINIFile.getBoolValue("Video","FrameLimit",true);
		settings.Audio.MusicType = myINIFile.getStringValue("Audio","Music Type","adl");
		std::string Lng = myINIFile.getStringValue("General","Language","en");
		if(Lng == "en") {
			settings.General.setLanguage(LNG_ENG);
		} else if (Lng == "fr") {
			settings.General.setLanguage(LNG_FRE);
		} else if (Lng == "de") {
			settings.General.setLanguage(LNG_GER);
		} else {
			fprintf(stderr,"INI-File: Invalid Language \"%s\"! Default Language (en) is used.\n",Lng.c_str());
			settings.General.setLanguage(LNG_ENG);
		}

		if(FileManager::getMissingFiles(settings.General.Language).size() > 0) {
		    // set back to english
            std::vector<std::string> MissingFiles = FileManager::getMissingFiles(settings.General.Language);
            fprintf(stderr,"The following files are missing for language \"%s\":\n",settings.General.LanguageExt.c_str());
            std::vector<std::string>::const_iterator iter;
            for(iter = MissingFiles.begin() ; iter != MissingFiles.end(); ++iter) {
                fprintf(stderr," %s\n",iter->c_str());
            }
            fprintf(stderr,"Language is changed to English!\n");
            settings.General.setLanguage(LNG_ENG);
		}

		lookDist[0] = 10;lookDist[1] = 10;lookDist[2] = 9;lookDist[3] = 9;lookDist[4] = 9;lookDist[5] = 8;lookDist[6] = 8;lookDist[7] = 7;lookDist[8] = 6;lookDist[9] = 4;lookDist[10] = 1;


		for(int i=1; i < argc; i++) {
		    //check for overiding params
			if((strcmp(argv[i], "-f") == 0) || (strcmp(argv[i], "--fullscreen") == 0))
				settings.Video.Fullscreen = true;
			else if((strcmp(argv[i], "-w") == 0) || (strcmp(argv[i], "--window") == 0))
				settings.Video.Fullscreen = false;
		}

		if(SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0) {
			fprintf(stderr, "ERROR: Couldn't initialise SDL: %s\n", SDL_GetError());
			exit(EXIT_FAILURE);
		}

		if(FirstGamestart == true && FirstInit == true) {
            // detect 800x600 screen resolution
            if(SDL_VideoModeOK(800, 600, 8, SDL_HWSURFACE | SDL_FULLSCREEN) > 0) {
                settings.Video.Width = 800;
                settings.Video.Height = 600;

                myINIFile.setIntValue("Video","Width",settings.Video.Width);
                myINIFile.setIntValue("Video","Height",settings.Video.Height);

                myINIFile.SaveChangesTo(GetConfigFilepath());
            }
		}

		SDL_EnableUNICODE(1);
		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
		char strenv[] = "SDL_VIDEO_CENTERED=center";
        SDL_putenv(strenv);
		SDL_WM_SetCaption("Dune Legacy", "Dune Legacy");

		if(FirstInit == true) {
			fprintf(stdout, "initialising sound..... \t");fflush(stdout);
			if( Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 1024) < 0 ) {
				SDL_Quit();
				fprintf(stderr,"Warning: Couldn't set 22050 Hz 16-bit audio\n- Reason: %s\n",SDL_GetError());
				exit(EXIT_FAILURE);
			} else {
				fprintf(stdout, "allocated %d channels.\n", Mix_AllocateChannels(4)); fflush(stdout);
			}
		}

        pFileManager = new FileManager( (MissingFiles.size() > 0) );

        if(pFileManager->exists("IBM.PAL") == true) {
            palette = LoadPalette_RW(pFileManager->OpenFile("IBM.PAL"), true);
        } else {
            // create dummy palette for showing missing files info
            palette = Palette(256);
            palette[255].r = 255;
            palette[255].g = 255;
            palette[255].b = 255;
        }

		screen = NULL;
		setVideoMode();


		fprintf(stdout, "loading fonts.....");fflush(stdout);
		pFontManager = new FontManager();

		fprintf(stdout, "\t\tfinished\n"); fflush(stdout);

		if(MissingFiles.size() > 0) {
		    // some files are missing
		    ExitGame = true;
		    PrintMissingFilesToScreen();
		    fprintf(stdout, "Deinitialize....."); fflush(stdout);
		} else {
		    // everything is just fine and we can start the game

            //get the house palettes
            houseColor[HOUSE_ATREIDES]  =   COLOR_ATREIDES;
            houseColor[HOUSE_ORDOS]     =   COLOR_ORDOS;
            houseColor[HOUSE_HARKONNEN] =   COLOR_HARKONNEN;
            houseColor[HOUSE_SARDAUKAR] =   COLOR_SARDAUKAR;
            houseColor[HOUSE_FREMEN]    =   COLOR_FREMEN;
            houseColor[HOUSE_MERCENARY] =   COLOR_MERCENARY;

            fprintf(stdout, "loading graphics....."); fflush(stdout);
            if((pGFXManager = new GFXManager()) == NULL) {
                fprintf(stderr,"main: Cannot create GFXManager!\n");
                exit(EXIT_FAILURE);
            }
            fprintf(stdout, "\t\tfinished\n"); fflush(stdout);

            fprintf(stdout, "loading sounds....."); fflush(stdout);
            if((pSFXManager = new SFXManager()) == NULL) {
                fprintf(stderr,"main: Cannot create SFXManager!\n");
                exit(EXIT_FAILURE);
            }
            fprintf(stdout, "\t\tfinished\n"); fflush(stdout);

            fprintf(stdout, "loading texts....."); fflush(stdout);
            if((pTextManager = new TextManager()) == NULL) {
                fprintf(stderr,"main: Cannot create TextManager!\n");
                exit(EXIT_FAILURE);
            }
            fprintf(stdout, "\t\tfinished\n"); fflush(stdout);

            GUIStyle::SetGUIStyle(new DuneStyle);

            if(FirstInit == true) {
                fprintf(stdout, "starting sound player..."); fflush(stdout);
                soundPlayer = new SoundPlayer();
                fprintf(stdout, "\tfinished\n");

                fprintf(stdout, "starting music player...\t"); fflush(stdout);
                if(settings.Audio.MusicType == "directory") {
                    fprintf(stdout, "playing from music directory\n"); fflush(stdout);
                    musicPlayer = new DirectoryPlayer();
                } else if(settings.Audio.MusicType == "adl") {
                    fprintf(stdout, "playing ADL files\n"); fflush(stdout);
                    musicPlayer = new ADLPlayer();
                } else if(settings.Audio.MusicType == "xmi") {
                    fprintf(stdout, "playing XMI files\n"); fflush(stdout);
                    musicPlayer = new XMIPlayer();
                } else {
                    fprintf(stdout, "failed\n"); fflush(stdout);
                    exit(EXIT_FAILURE);
                }

                //musicPlayer->changeMusic(MUSIC_INTRO);
            }

            // Playing intro
            if(((FirstGamestart == true) || (settings.General.PlayIntro == true)) && (FirstInit==true)) {
                fprintf(stdout, "playing intro.....");fflush(stdout);
                Intro* pIntro = new Intro();

                pIntro->run();

                delete pIntro;
                fprintf(stdout, "\t\tfinished\n"); fflush(stdout);
            }

            FirstInit = false;

            fprintf(stdout, "starting main menu.......");fflush(stdout);

            MainMenu * myMenu = new MainMenu();

            fprintf(stdout, "\tfinished\n"); fflush(stdout);

            if(myMenu->showMenu() == -1) {
                ExitGame = true;
            }
            delete myMenu;

            fprintf(stdout, "Deinitialize....."); fflush(stdout);

            GUIStyle::DestroyGUIStyle();

            // clear everything
            if(ExitGame == true) {
                delete musicPlayer;
                delete soundPlayer;
                Mix_HaltMusic();
                Mix_CloseAudio();
            }

            delete pTextManager;
            delete pSFXManager;
            delete pGFXManager;
		}

		delete pFontManager;
		delete pFileManager;
		if(ExitGame == true) {
			SDL_Quit();
		}
		fprintf(stdout, "\t\tfinished\n"); fflush(stdout);
	} while(ExitGame == false);

	// deinit fnkdat
	if(fnkdat(NULL, NULL, 0, FNKDAT_UNINIT) < 0) {
		perror("Could not uninitialize fnkdat");
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}
