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

#include <FileClasses/FontManager.h>

#include <globals.h>

#include <FileClasses/FileManager.h>
#include <FileClasses/Fntfile.h>
#include <FileClasses/PictureFont.h>

#include <list>

FontManager::FontManager() {
	Fonts[FONT_STD10] = std::shared_ptr<Font>(new PictureFont(SDL_LoadBMP_RW(pFileManager->OpenFile("Font10.bmp"),true), true));
	Fonts[FONT_STD12] = std::shared_ptr<Font>(new PictureFont(SDL_LoadBMP_RW(pFileManager->OpenFile("Font12.bmp"),true), true));
	Fonts[FONT_STD24] = std::shared_ptr<Font>(new PictureFont(SDL_LoadBMP_RW(pFileManager->OpenFile("Font24.bmp"),true), true));
}

FontManager::~FontManager() {
}

void FontManager::DrawTextOnSurface(SDL_Surface* pSurface, std::string text, unsigned char color, unsigned int FontNum) {
	if(FontNum >= NUM_FONTS) {
		return;
	}

	Fonts[FontNum]->DrawTextOnSurface(pSurface,text,color);
}

int	FontManager::getTextWidth(std::string text, unsigned int FontNum) {
	if(FontNum >= NUM_FONTS) {
		return 0;
	}

	return Fonts[FontNum]->getTextWidth(text);
}

int FontManager::getTextHeight(unsigned int FontNum) {
	if(FontNum >= NUM_FONTS) {
		return 0;
	}

	return Fonts[FontNum]->getTextHeight();
}

SDL_Surface* FontManager::createSurfaceWithText(std::string text, unsigned char color, unsigned int FontNum) {
	if(FontNum >= NUM_FONTS) {
		return 0;
	}

    SDL_Surface* pic;

    int width = Fonts[FontNum]->getTextWidth(text);
    int height = Fonts[FontNum]->getTextHeight();

    // create new picture surface
    if((pic = SDL_CreateRGBSurface(SDL_SWSURFACE,width,height,8,0,0,0,0))== NULL) {
        return NULL;
    }

    palette.applyToSurface(pic);
    SDL_SetColorKey(pic, SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);

    Fonts[FontNum]->DrawTextOnSurface(pic,text,color);

    return pic;
}

SDL_Surface* FontManager::createSurfaceWithMultilineText(std::string text, unsigned char color, unsigned int FontNum, bool bCentered) {
	if(FontNum >= NUM_FONTS) {
		return 0;
	}

    size_t startpos = 0;
	size_t nextpos;
	std::list<std::string> TextLines;
	do {
		nextpos = text.find("\n",startpos);
		if(nextpos == std::string::npos) {
			TextLines.push_back(text.substr(startpos,text.length()-startpos));
		} else {
			TextLines.push_back(text.substr(startpos,nextpos-startpos));
			startpos = nextpos+1;
		}
	} while(nextpos != std::string::npos);

    SDL_Surface* pic;

    int lineHeight = Fonts[FontNum]->getTextHeight();
    int width = Fonts[FontNum]->getTextWidth(text);
    int height = lineHeight * TextLines.size() + (lineHeight * (TextLines.size()-1))/2;

    // create new picture surface
    if((pic = SDL_CreateRGBSurface(SDL_SWSURFACE,width,height,8,0,0,0,0))== NULL) {
        return NULL;
    }

    palette.applyToSurface(pic);
    SDL_SetColorKey(pic, SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);

    SDL_Rect dest = { 0,0,0,lineHeight};
    std::list<std::string>::iterator iter;
    for(iter = TextLines.begin(); iter != TextLines.end(); ++iter, dest.y += lineHeight) {
        SDL_Surface* tmpSurface = createSurfaceWithText(*iter, color, FontNum);

        dest.w = tmpSurface->w;
        dest.x = (bCentered == false) ? 0 : (width - dest.w)/2;

        SDL_BlitSurface(tmpSurface,NULL,pic,&dest);

        SDL_FreeSurface(tmpSurface);
    }

    return pic;
}
