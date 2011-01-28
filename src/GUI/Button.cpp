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

#include <GUI/Button.h>

#include <globals.h>

#include <SoundPlayer.h>

Button::Button() : Widget() {
	TooltipText = "";
	TooltipSurface = NULL;

	bPressed = false;
	bHover = false;
	bToggleButton = false;
	bToggleState = false;

	pUnpressedSurface = NULL;
	pPressedSurface = NULL;
	pActiveSurface = NULL;
	bFreeUnpressedSurface = false;
	bFreePressedSurface = false;
	bFreeActiveSurface = false;
}

Button::~Button() {
	FreeSurfaces();

	if(TooltipSurface != NULL) {
		SDL_FreeSurface(TooltipSurface);
		TooltipSurface = NULL;
	}
}

void Button::HandleMouseMovement(Sint32 x, Sint32 y, bool insideOverlay) {
	if((x < 0) || (x >= GetSize().x) || (y < 0) || (y >= GetSize().y)) {
		bPressed = false;
		bHover = false;
	} else if(IsEnabled() && !insideOverlay) {
		bHover = true;
		Tooltip_LastMouseMotion = SDL_GetTicks();
	}
}

bool Button::HandleMouseLeft(Sint32 x, Sint32 y, bool pressed) {
	if((x < 0) || (x >= GetSize().x) || (y < 0) || (y >= GetSize().y)) {
		return false;
	}

	if((IsEnabled() == false) || (IsVisible() == false)) {
		return true;
	}

	if(pressed == true) {
		// button pressed
		bPressed = true;
		soundPlayer->playSound(ButtonClick);
	} else {
		// button released
		if(bPressed == true) {
			bPressed = false;
			if(bToggleButton) {
				bToggleState = !bToggleState;
			}

			pOnClick();
		}
	}
	return true;
}

bool Button::HandleKeyPress(SDL_KeyboardEvent& key) {
	if((IsVisible() == false) || (IsEnabled() == false) || (IsActive() == false)) {
		return true;
	}

	if(key.keysym.sym == SDLK_TAB) {
		SetInactive();
		return true;
	}

	if(key.keysym.sym == SDLK_SPACE) {
		if(bToggleButton) {
			bToggleState = !bToggleState;
		}

		soundPlayer->playSound(ButtonClick);
		pOnClick();
	}

	return true;
}

void Button::Draw(SDL_Surface* screen, Point Position) {
	if(IsVisible() == false) {
		return;
	}

	SDL_Surface* surf;
	if(bToggleState == true) {
		if(pPressedSurface != NULL) {
			surf = pPressedSurface;
		} else {
			if(IsActive() && pActiveSurface != NULL) {
				surf = pActiveSurface;
			} else {
				surf = pUnpressedSurface;
			}
		}
	} else {
		if(bPressed == true) {
			if(pPressedSurface != NULL) {
				surf = pPressedSurface;
			} else {
				if(IsActive() && pActiveSurface != NULL) {
					surf = pActiveSurface;
				} else {
					surf = pUnpressedSurface;
				}
			}
		} else {
			if((IsActive() || bHover) && pActiveSurface != NULL) {
				surf = pActiveSurface;
			} else {
				surf = pUnpressedSurface;
			}
		}
	}

	if(surf == NULL) {
		return;
	}

	SDL_Rect dest;
	dest.x = Position.x;
	dest.y = Position.y;
	dest.w = surf->w;
	dest.h = surf->h;

	SDL_BlitSurface(surf,NULL,screen,&dest);
}

void Button::DrawOverlay(SDL_Surface* screen, Point Pos) {
	if(IsVisible() && IsEnabled() && (bHover == true)) {
		if(TooltipSurface != NULL) {
			if((SDL_GetTicks() - Tooltip_LastMouseMotion) > 750) {
				int x,y;
				SDL_GetMouseState(&x,&y);
				SDL_Rect dest;
				dest.x = x;
				dest.y = y - TooltipSurface->h;
				dest.w = TooltipSurface->w;
				dest.h = TooltipSurface->h;
				if(dest.x + dest.w >= screen->w) {
				    // do not draw tooltip outside screen
                    dest.x = screen->w - dest.w;
				}
				SDL_BlitSurface(TooltipSurface,NULL,screen,&dest);
			}
		}
	}
}

void Button::SetSurfaces(	SDL_Surface* pUnpressedSurface,bool bFreeUnpressedSurface,
							SDL_Surface* pPressedSurface,bool bFreePressedSurface,
							SDL_Surface* pActiveSurface,bool bFreeActiveSurface) {
	FreeSurfaces();
	this->pUnpressedSurface = pUnpressedSurface;
	this->bFreeUnpressedSurface = bFreeUnpressedSurface;
	this->pPressedSurface = pPressedSurface;
	this->bFreePressedSurface = bFreePressedSurface;
	this->pActiveSurface = pActiveSurface;
	this->bFreeActiveSurface = bFreeActiveSurface;
}

void Button::FreeSurfaces() {
	if((bFreeUnpressedSurface == true) && (pUnpressedSurface != NULL)) {
		SDL_FreeSurface(pUnpressedSurface);
		pUnpressedSurface = NULL;
		bFreeUnpressedSurface = false;
	}

	if((bFreePressedSurface == true) && (pPressedSurface != NULL)) {
		SDL_FreeSurface(pPressedSurface);
		pPressedSurface = NULL;
		bFreePressedSurface = false;
	}

	if((bFreeActiveSurface == true) && (pActiveSurface != NULL)) {
		SDL_FreeSurface(pActiveSurface);
		pActiveSurface = NULL;
		bFreeActiveSurface = false;
	}
}
