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

#include <GUI/dune/DuneStyle.h>
#include <misc/draw_util.h>
#include <GUI/Widget.h>

#include <FileClasses/GFXManager.h>
#include <FileClasses/FontManager.h>
#include <FileClasses/Palette.h>

#include <iostream>
#include <cmath>
#include <algorithm>

extern Palette      palette;
extern GFXManager*  pGFXManager;
extern FontManager* pFontManager;


SDL_Surface* DuneStyle::CreateSurfaceWithText(const char* text, unsigned char color, unsigned int fontsize) {
	if(pFontManager != NULL) {
		return pFontManager->createSurfaceWithText(text, color, fontsize);
	} else {
		SDL_Surface* surface;

		// create surfaces
		if((surface = SDL_CreateRGBSurface(SDL_HWSURFACE,strlen(text)*10,12,8,0,0,0,0))== NULL) {
			return NULL;
		}
		palette.applyToSurface(surface);

		return surface;
	}
}

unsigned int DuneStyle::GetTextHeight(unsigned int FontNum) {
	if(pFontManager != NULL) {
		return pFontManager->getTextHeight(FontNum);
	} else {
		return 12;
	}
}

unsigned int DuneStyle::GetTextWidth(const char* text, unsigned int FontNum)  {
	if(pFontManager != NULL) {
		return pFontManager->getTextWidth(text,FontNum);
	} else {
		return strlen(text)*10;
	}
}


Point DuneStyle::GetMinimumLabelSize(std::string text, int fontID) {
	return Point(GetTextWidth(text.c_str(),fontID) + 12,GetTextHeight(fontID) + 4);
}

SDL_Surface* DuneStyle::CreateLabelSurface(Uint32 width, Uint32 height, std::list<std::string> TextLines, int fontID, Alignment_Enum alignment, int textcolor, int textshadowcolor, int backgroundcolor) {
	SDL_Surface* surface;

	// create surfaces
	if((surface = SDL_CreateRGBSurface(SDL_HWSURFACE,width,height,8,0,0,0,0))== NULL) {
		return NULL;
	}
	palette.applyToSurface(surface);

	SDL_FillRect(surface,NULL, backgroundcolor);

	if(textcolor == -1) textcolor = 147;
	if(textshadowcolor == -1) textshadowcolor = 110;

	std::list<SDL_Surface*> TextSurfaces;
	std::list<std::string>::const_iterator iter;
	for(iter = TextLines.begin(); iter != TextLines.end() ; ++iter) {
		std::string text = *iter;

		// create text background
		TextSurfaces.push_back(CreateSurfaceWithText(text.c_str(), textshadowcolor, fontID));
		// create text foreground
		TextSurfaces.push_back(CreateSurfaceWithText(text.c_str(), textcolor, fontID));
	}

	int fontheight = GetTextHeight(fontID);
	int spacing = 2;

	int textpos_y;

	if(alignment & Alignment_VCenter) {
		int textheight = fontheight * TextLines.size() + spacing * (TextLines.size() - 1);
		textpos_y = (height - textheight) / 2;
	} else if(alignment & Alignment_Bottom) {
		int textheight = fontheight * TextLines.size() + spacing * (TextLines.size() - 1);
		textpos_y = height - textheight - spacing;
	} else {
		// Alignment_Top
		textpos_y = spacing;
	}

	std::list<SDL_Surface*>::const_iterator surfiter = TextSurfaces.begin();
	while(surfiter != TextSurfaces.end()) {
		SDL_Rect textRect;
		SDL_Surface* textSurface;

		textSurface = *surfiter;
		if(alignment & Alignment_Left) {
			textRect.x = 4;
        } else if(alignment & Alignment_Right) {
			textRect.x = width - textSurface->w - 4;
        } else {
            // Alignment_HCenter
			textRect.x = ((surface->w - textSurface->w) / 2)+3;
		}

		textRect.y = textpos_y + 3;
		textRect.w = textSurface->w;
		textRect.h = textSurface->h;
		SDL_BlitSurface(textSurface,NULL,surface,&textRect);
		SDL_FreeSurface(textSurface);
		++surfiter;

        textSurface = *surfiter;

        if(alignment & Alignment_Left) {
            textRect.x = 3;
        } else if(alignment & Alignment_Right) {
            textRect.x = width - textSurface->w - 3;
        } else {
            // Alignment_HCenter
            textRect.x = ((surface->w - textSurface->w) / 2)+2;
        }
        textRect.y = textpos_y + 2;
        textRect.w = textSurface->w;
        textRect.h = textSurface->h;
        SDL_BlitSurface(textSurface,NULL,surface,&textRect);
        SDL_FreeSurface(textSurface);
        ++surfiter;

		textpos_y += fontheight + spacing;
	}


	SDL_SetColorKey(surface, SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
	return surface;
}





Point DuneStyle::GetMinimumCheckboxSize(std::string text) {
	return Point(GetTextWidth(text.c_str(),FONT_STD12) + 20 + 17,GetTextHeight(FONT_STD12) + 8);
}

SDL_Surface* DuneStyle::CreateCheckboxSurface(Uint32 width, Uint32 height, std::string text, bool checked, bool activated, int textcolor, int textshadowcolor, int backgroundcolor) {
	SDL_Surface* surface;

	// create surfaces
	if((surface = SDL_CreateRGBSurface(SDL_HWSURFACE,width,height,8,0,0,0,0))== NULL) {
		return NULL;
	}
	palette.applyToSurface(surface);

	SDL_FillRect(surface,NULL, backgroundcolor);

	if(textcolor == -1) textcolor = 147;
	if(textshadowcolor == -1) textshadowcolor = 110;

    if(activated) {
        textcolor -= 2;
    }

	drawrect(surface, 4, 5, 4 + 17, 5 + 17, textcolor);
	drawrect(surface, 4 + 1, 5 + 1, 4 + 17 - 1, 5 + 17 - 1, textcolor);

	if(checked) {
		int x1 = 4 + 2;
		int y1 = 5 + 2;
		int x2 = 4 + 17 - 2;

		for(int i = 0; i < 15; i++) {
			// North-West to South-East
			putpixel(surface, x1, y1, textcolor);
			putpixel(surface, x1+1, y1, textcolor);
			putpixel(surface, x1-1, y1, textcolor);

			// North-East to South-West
			putpixel(surface, x2, y1, textcolor);
			putpixel(surface, x2+1, y1, textcolor);
			putpixel(surface, x2-1, y1, textcolor);

			x1++;
			y1++;
			x2--;
		}
	}


	SDL_Surface* textSurface;
	SDL_Rect textRect;
	textSurface = CreateSurfaceWithText(text.c_str(), textshadowcolor, FONT_STD12);
	textRect.x = 10+2 + 17;
	textRect.y = ((surface->h - textSurface->h) / 2)+3;
	textRect.w = textSurface->w;
	textRect.h = textSurface->h;
	SDL_BlitSurface(textSurface,NULL,surface,&textRect);
	SDL_FreeSurface(textSurface);

	textSurface = CreateSurfaceWithText(text.c_str(), textcolor, FONT_STD12);
	textRect.x = 10+1+17;
	textRect.y = ((surface->h - textSurface->h) / 2)+2;
	textRect.w = textSurface->w;
	textRect.h = textSurface->h;
	SDL_BlitSurface(textSurface,NULL,surface,&textRect);
	SDL_FreeSurface(textSurface);

	SDL_SetColorKey(surface, SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
	return surface;
}




SDL_Surface* DuneStyle::CreateDropDownBoxButton(Uint32 size, bool pressed, bool activated, int color) {
    if(color == -1) {
        color = 147;
    }

	// create surfaces
	SDL_Surface* surface;
	if((surface = SDL_CreateRGBSurface(SDL_HWSURFACE,size,size,8,0,0,0,0))== NULL) {
		return NULL;
	}
	palette.applyToSurface(surface);


	// create button background
	if(pressed == false) {
		// normal mode
		SDL_FillRect(surface, NULL, 115);
		drawrect(surface, 0, 0, surface->w-1, surface->h-1, 229);
		drawhline(surface, 1, 1, surface->w-2, 108);
		drawvline(surface, 1, 1, surface->h-2, 108);
		drawhline(surface, 1, surface->h-2, surface->w-2, 226);
		drawvline(surface, surface->w-2, 1, surface->h-2, 226);
	} else {
		// pressed button mode
		SDL_FillRect(surface, NULL, 116);
		drawrect(surface, 0, 0, surface->w-1, surface->h-1, 229);
		drawrect(surface, 1, 1, surface->w-2, surface->h-2, 226);
	}

	int col = (pressed | activated) ? (color-2) : color;

	int x1 = 3;
	int x2 = size-3-1;
	int y = size/3 - 1;

	// draw separated hline
	drawhline(surface, x1, y, x2, col);
	y+=2;

	// down arrow
	for(;x1 <= x2; ++x1, --x2, ++y) {
		drawhline(surface, x1, y, x2, col);
	}

	return surface;
}




Point DuneStyle::GetMinimumButtonSize(std::string text) {
	return Point(GetTextWidth(text.c_str(),FONT_STD10)+12,GetTextHeight(FONT_STD10));
}

SDL_Surface* DuneStyle::CreateButtonSurface(Uint32 width, Uint32 height, std::string text, bool pressed, bool activated, int textcolor, int textshadowcolor) {

	// create surfaces
    SDL_Surface* surface;
	if((surface = SDL_CreateRGBSurface(SDL_HWSURFACE,width,height,8,0,0,0,0))== NULL) {
		return NULL;
	}
	palette.applyToSurface(surface);


	// create button background
	if(pressed == false) {
		// normal mode
		SDL_FillRect(surface, NULL, 115);
		drawrect(surface, 0, 0, surface->w-1, surface->h-1, 229);
		drawhline(surface, 1, 1, surface->w-2, 108);
		drawvline(surface, 1, 1, surface->h-2, 108);
		drawhline(surface, 1, surface->h-2, surface->w-2, 226);
		drawvline(surface, surface->w-2, 1, surface->h-2, 226);
	} else {
		// pressed button mode
		SDL_FillRect(surface, NULL, 116);
		drawrect(surface, 0, 0, surface->w-1, surface->h-1, 229);
		drawrect(surface, 1, 1, surface->w-2, surface->h-2, 226);

	}

	// create text on this button
	int fontsize;
	if(	(width < GetTextWidth(text.c_str(),FONT_STD12) + 12) ||
		(height < GetTextHeight(FONT_STD12) + 2)) {
		fontsize = FONT_STD10;
	} else {
		fontsize = FONT_STD12;
	}

    if(textcolor == -1) textcolor = 147;
	if(textshadowcolor == -1) textshadowcolor = 110;

	SDL_Surface* textSurface;
	SDL_Rect textRect;
	textSurface = CreateSurfaceWithText(text.c_str(), textshadowcolor, fontsize);
	textRect.x = ((surface->w - textSurface->w) / 2)+2+(pressed ? 1 : 0);
	textRect.y = ((surface->h - textSurface->h) / 2)+3+(pressed ? 1 : 0);
	textRect.w = textSurface->w;
	textRect.h = textSurface->h;
	SDL_BlitSurface(textSurface,NULL,surface,&textRect);
	SDL_FreeSurface(textSurface);

	textSurface = CreateSurfaceWithText(text.c_str(), (activated == true) ? (textcolor-2) : textcolor, fontsize);
	textRect.x = ((surface->w - textSurface->w) / 2)+1+(pressed ? 1 : 0);
	textRect.y = ((surface->h - textSurface->h) / 2)+2+(pressed ? 1 : 0);
	textRect.w = textSurface->w;
	textRect.h = textSurface->h;
	SDL_BlitSurface(textSurface,NULL,surface,&textRect);
	SDL_FreeSurface(textSurface);

	return surface;
}




Point DuneStyle::GetMinimumTextBoxSize() {
	return Point(10,GetTextHeight(FONT_STD10) + 12);
}

SDL_Surface* DuneStyle::CreateTextBoxSurface(Uint32 width, Uint32 height, std::string text, bool carret, int textcolor, int textshadowcolor) {

	// create surfaces
	SDL_Surface* surface;
	if((surface = SDL_CreateRGBSurface(SDL_HWSURFACE,width,height,8,0,0,0,0))== NULL) {
		return NULL;
	}
	palette.applyToSurface(surface);

	SDL_FillRect(surface, NULL, 115);
	drawrect(surface,0,0,surface->w-1,surface->h-1,229);

	drawhline(surface,1,1,surface->w-2,226);
	drawhline(surface,1,2,surface->w-2,226);
	drawvline(surface,1,1,surface->h-2,226);
	drawvline(surface,2,1,surface->h-2,226);
	drawhline(surface,1,surface->h-2,surface->w-2,108);
	drawvline(surface,surface->w-2,1,surface->h-2,108);

    if(textcolor == -1) textcolor = 147;
	if(textshadowcolor == -1) textshadowcolor = 110;

	SDL_Rect cursorPos;

	// create text in this text box
	if(text.size() != 0) {
		SDL_Surface* textSurface;
		SDL_Rect textRect;
		textSurface = CreateSurfaceWithText(text.c_str(), textshadowcolor, FONT_STD12);
		textRect.x = ((surface->w - textSurface->w) / 2)+3;
		textRect.y = ((surface->h - textSurface->h) / 2)+3;
		textRect.w = textSurface->w;
		textRect.h = textSurface->h;
		if(textRect.w > surface->w - 20) {
			textRect.x -= (textSurface->w - surface->w) / 2;
			textRect.x -= 10;
		}
		SDL_BlitSurface(textSurface,NULL,surface,&textRect);
		SDL_FreeSurface(textSurface);

		textSurface = CreateSurfaceWithText(text.c_str(), textcolor, FONT_STD12);
		textRect.x = ((surface->w - textSurface->w) / 2)+2;
		textRect.y = ((surface->h - textSurface->h) / 2)+2;
		textRect.w = textSurface->w;
		textRect.h = textSurface->h;
		if(textRect.w > surface->w - 20) {
			textRect.x -= (textSurface->w - surface->w) / 2;
			textRect.x -= 10;
		}
		cursorPos.x = textRect.x + textSurface->w + 2;
		SDL_BlitSurface(textSurface,NULL,surface,&textRect);
		SDL_FreeSurface(textSurface);

		cursorPos.w = 1;
	} else {
		cursorPos.x = surface->w / 2;
		cursorPos.w = 1;
	}

	cursorPos.y = surface->h / 2 - 8;
	cursorPos.h = 16;

	if(carret == true) {
		drawvline(surface,cursorPos.x,cursorPos.y,cursorPos.y+cursorPos.h,textcolor);
		drawvline(surface,cursorPos.x+1,cursorPos.y,cursorPos.y+cursorPos.h,textcolor);
	}

	return surface;
}




Point DuneStyle::GetMinimumScrollBarArrowButtonSize() {
	return Point(17,17);
}

SDL_Surface* DuneStyle::CreateScrollBarArrowButton(bool down, bool pressed, bool activated, int color) {
    if(color == -1) {
        color = 147;
    }

	// create surfaces
	SDL_Surface* surface;
	if((surface = SDL_CreateRGBSurface(SDL_HWSURFACE,17,17,8,0,0,0,0))== NULL) {
		return NULL;
	}
	palette.applyToSurface(surface);


	// create button background
	if(pressed == false) {
		// normal mode
		SDL_FillRect(surface, NULL, 115);
		drawrect(surface, 0, 0, surface->w-1, surface->h-1, 229);
		drawhline(surface, 1, 1, surface->w-2, 108);
		drawvline(surface, 1, 1, surface->h-2, 108);
		drawhline(surface, 1, surface->h-2, surface->w-2, 226);
		drawvline(surface, surface->w-2, 1, surface->h-2, 226);
	} else {
		// pressed button mode
		SDL_FillRect(surface, NULL, 116);
		drawrect(surface, 0, 0, surface->w-1, surface->h-1, 229);
		drawrect(surface, 1, 1, surface->w-2, surface->h-2, 226);
	}

	int col = (pressed | activated) ? (color-2) : color;

	// draw arrow
	if(down == true) {
		// down arrow
		drawhline(surface,3,4,13,col);
		drawhline(surface,4,5,12,col);
		drawhline(surface,5,6,11,col);
		drawhline(surface,6,7,10,col);
		drawhline(surface,7,8,9,col);
		drawhline(surface,8,9,8,col);
	} else {
		// up arrow
		drawhline(surface,8,5,8,col);
		drawhline(surface,7,6,9,col);
		drawhline(surface,6,7,10,col);
		drawhline(surface,5,8,11,col);
		drawhline(surface,4,9,12,col);
		drawhline(surface,3,10,13,col);
	}

	return surface;
}




Uint32 DuneStyle::GetListBoxEntryHeight() {
	return 16;
}

SDL_Surface* DuneStyle::CreateListBoxEntry(Uint32 width, std::string text, bool selected, int color) {
    if(color == -1) {
        color = 147;
    }

	// create surfaces
	SDL_Surface* surface;
	if((surface = SDL_CreateRGBSurface(SDL_HWSURFACE,width,GetListBoxEntryHeight(),8,0,0,0,0))== NULL) {
		return NULL;
	}
	palette.applyToSurface(surface);
	if(selected == true) {
		SDL_FillRect(surface, NULL, 115);
	} else {
		SDL_SetColorKey(surface, SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
	}

	SDL_Surface* textSurface;
	SDL_Rect textRect;
	textSurface = CreateSurfaceWithText(text.c_str(), color, FONT_STD10);
	textRect.x = 3;
	textRect.y = ((surface->h - textSurface->h) / 2) + 1;
	textRect.w = textSurface->w;
	textRect.h = textSurface->h;
	SDL_BlitSurface(textSurface,NULL,surface,&textRect);
	SDL_FreeSurface(textSurface);

	return surface;
}




SDL_Surface* DuneStyle::CreateProgressBarOverlay(Uint32 width, Uint32 height, double percent, int color) {

	// create surfaces
	SDL_Surface* pSurface;
	if((pSurface = SDL_CreateRGBSurface(SDL_HWSURFACE,width,height,8,0,0,0,0))== NULL) {
		return NULL;
	}

	palette.applyToSurface(pSurface);
	SDL_SetColorKey(pSurface, SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);

	if(color == -1) {
		// default color

        unsigned int max_i = std::max( (int) lround(percent*(( ((int) width) - 4)/100.0)), 0);

		if (!SDL_MUSTLOCK(pSurface) || (SDL_LockSurface(pSurface) == 0)) {
			for (unsigned int i = 2; i < max_i + 2; i++) {
				for (unsigned int j = (i % 2) + 2; j < height-2; j+=2) {
					putpixel(pSurface, i, j, COLOR_BLACK);
				}
			}

			if (SDL_MUSTLOCK(pSurface))
				SDL_UnlockSurface(pSurface);
		}
	} else {
	    unsigned int max_i = lround(percent*(width/100.0));

		SDL_Rect dest = { 0 , 0 , max_i , height};
        SDL_FillRect(pSurface, &dest, color);
	}

	return pSurface;
}



SDL_Surface* DuneStyle::CreateToolTip(std::string text) {
	SDL_Surface* surface;
	SDL_Surface* helpTextSurface;

	if((helpTextSurface = CreateSurfaceWithText(text.c_str(), COLOR_YELLOW, FONT_STD10)) == NULL) {
		return NULL;
	}

	// create surfaces
	if((surface = SDL_CreateRGBSurface(SDL_HWSURFACE, helpTextSurface->w + 4, helpTextSurface->h + 2,8,0,0,0,0)) == NULL) {
		SDL_FreeSurface(helpTextSurface);
		return NULL;
	}
	palette.applyToSurface(surface);

	SDL_FillRect(surface, NULL, COLOR_BLACK);
	drawrect(surface, 0, 0, helpTextSurface->w + 4 - 1, helpTextSurface->h + 2 - 1, COLOR_YELLOW);

	SDL_Rect textRect;

	textRect.x = 3;
	textRect.y = 3;
	textRect.w = helpTextSurface->w;
	textRect.h = helpTextSurface->h;

	SDL_BlitSurface(helpTextSurface, NULL, surface, &textRect);

	SDL_FreeSurface(helpTextSurface);
	return surface;
}



SDL_Surface* DuneStyle::CreateBackground(Uint32 width, Uint32 height) {
	SDL_Surface* pSurface;
	if(pGFXManager != NULL) {
		pSurface = GetSubPicture(pGFXManager->getUIGraphic(UI_Background), 0, 0, width, height);
		if(pSurface == NULL) {
			return NULL;
		}
	} else {
		// data manager not yet loaded
		if((pSurface = SDL_CreateRGBSurface(SDL_HWSURFACE,width,height,8,0,0,0,0))== NULL) {
			return NULL;
		}
		palette.applyToSurface(pSurface);
		SDL_FillRect(pSurface, NULL, 115);
	}


	drawrect(pSurface, 0, 0, pSurface->w-1, pSurface->h-1, 229);
	drawhline(pSurface, 1, 1, pSurface->w-2, 108);
	drawhline(pSurface, 2, 2, pSurface->w-3, 108);
	drawvline(pSurface, 1, 1, pSurface->h-2, 108);
	drawvline(pSurface, 2, 2, pSurface->h-3, 108);
	drawhline(pSurface, 1, pSurface->h-2, pSurface->w-2, 226);
	drawhline(pSurface, 2, pSurface->h-3, pSurface->w-3, 226);
	drawvline(pSurface, pSurface->w-2, 1, pSurface->h-2, 226);
	drawvline(pSurface, pSurface->w-3, 2, pSurface->h-3, 226);

	return pSurface;
}

SDL_Surface* DuneStyle::CreateWidgetBackground(Uint32 width, Uint32 height) {
	SDL_Surface* surface;

	// create surfaces
	if((surface = SDL_CreateRGBSurface(SDL_HWSURFACE,width,height,8,0,0,0,0))== NULL) {
		return NULL;
	}
	palette.applyToSurface(surface);


	SDL_FillRect(surface, NULL, 116);
	drawrect(surface, 0, 0, surface->w-1, surface->h-1, 229);
	drawrect(surface, 1, 1, surface->w-2, surface->h-2, 226);

	return surface;
}
