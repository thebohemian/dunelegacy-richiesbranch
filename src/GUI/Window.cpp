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

#include <GUI/Window.h>
#include <GUI/GUIStyle.h>

Window::Window(Uint32 x, Uint32 y, Uint32 w, Uint32 h) : Widget() {
    bCloseChildWindow = false;
	pChildWindow = NULL;
	pWindowWidget = NULL;

	bTransparentBackground = false;
	pBackground = NULL;
	bFreeBackground = false;
	bSelfGeneratedBackground = true;

	Position = Point(x,y);
	Widget::Resize(w,h);
}

Window::~Window() {
	if(pChildWindow != NULL) {
		pChildWindow->Destroy();
	}

	if(pWindowWidget != NULL) {
		pWindowWidget->Destroy();
	}

	if(((bSelfGeneratedBackground == true) || (bFreeBackground == true)) && (pBackground != NULL)) {
		SDL_FreeSurface(pBackground);
		pBackground = NULL;
	}
}

void Window::OpenWindow(Window* pChildWindow) {
	this->pChildWindow = pChildWindow;
	if(this->pChildWindow != NULL) {
		this->pChildWindow->SetParent(this);
	}
}

void Window::CloseChildWindow() {
    bCloseChildWindow = true;
}

void Window::SetCurrentPosition(Uint32 x, Uint32 y, Uint32 w, Uint32 h) {
	Position.x = x; Position.y = y;
	Resize(w,h);
}

void Window::HandleInput(SDL_Event& event) {
	if(pChildWindow != NULL) {
		pChildWindow->HandleInput(event);

        if(bCloseChildWindow) {
            OnChildWindowClose(pChildWindow);
            pChildWindow->Destroy();
            pChildWindow = NULL;
            bCloseChildWindow = false;
        }

		return;
	}

	switch(event.type) {
		case SDL_KEYDOWN: {
			HandleKeyPress(event.key);
		} break;

		case SDL_MOUSEMOTION: {
			HandleMouseMovement(event.motion.x,event.motion.y);
		} break;

		case SDL_MOUSEBUTTONDOWN: {
			switch(event.button.button) {
				case SDL_BUTTON_LEFT: {
					HandleMouseLeft(event.button.x,event.button.y,true);
				} break;

				case SDL_BUTTON_RIGHT: {
					HandleMouseRight(event.button.x,event.button.y,true);
				} break;

				case SDL_BUTTON_WHEELUP: {
					HandleMouseWheel(event.button.x,event.button.y,true);
				} break;

				case SDL_BUTTON_WHEELDOWN: {
					HandleMouseWheel(event.button.x,event.button.y,false);
				} break;

			}
		} break;

		case SDL_MOUSEBUTTONUP: {
			switch(event.button.button) {
				case SDL_BUTTON_LEFT: {
					HandleMouseLeft(event.button.x,event.button.y,false);
				} break;

				case SDL_BUTTON_RIGHT: {
					HandleMouseRight(event.button.x,event.button.y,false);
				} break;

			}
		} break;
	}
}

void Window::HandleMouseMovement(Sint32 x, Sint32 y) {
	if(pChildWindow != NULL) {
		pChildWindow->HandleMouseMovement(x, y);
		return;
	}

	if(IsEnabled() && (pWindowWidget != NULL)) {
	    bool insideOverlay = pWindowWidget->HandleMouseMovementOverlay(x - GetPosition().x, y - GetPosition().y);
		pWindowWidget->HandleMouseMovement(x - GetPosition().x, y - GetPosition().y, insideOverlay);
	}
}

bool Window::HandleMouseLeft(Sint32 x, Sint32 y, bool pressed) {
	if(pChildWindow != NULL) {
		return pChildWindow->HandleMouseLeft(x, y, pressed);
	}

	if(IsEnabled() && (pWindowWidget != NULL)) {
	    bool bProcessed = pWindowWidget->HandleMouseLeftOverlay(x - GetPosition().x, y - GetPosition().y, pressed)
                            || pWindowWidget->HandleMouseLeft(x - GetPosition().x, y - GetPosition().y, pressed);
        if(pressed && (bProcessed == false)) {
            pWindowWidget->SetActive(false);
            pWindowWidget->SetActive(true);
        }
        return bProcessed;
	} else {
		return false;
	}
}

bool Window::HandleMouseRight(Sint32 x, Sint32 y, bool pressed) {
	if(pChildWindow != NULL) {
		return pChildWindow->HandleMouseRight(x, y, pressed);
	}

	if(IsEnabled() && (pWindowWidget != NULL)) {
		return pWindowWidget->HandleMouseRightOverlay(x - GetPosition().x, y - GetPosition().y, pressed)
				|| pWindowWidget->HandleMouseRight(x - GetPosition().x, y - GetPosition().y, pressed);
	} else {
		return false;
	}
}

bool Window::HandleMouseWheel(Sint32 x, Sint32 y, bool up)  {
	if(pChildWindow != NULL) {
		return pChildWindow->HandleMouseWheel(x, y, up);
	}

	if(IsEnabled() && (pWindowWidget != NULL)) {
		return pWindowWidget->HandleMouseWheelOverlay(x - GetPosition().x, y - GetPosition().y, up)
				|| pWindowWidget->HandleMouseWheel(x - GetPosition().x, y - GetPosition().y, up);
	} else {
		return false;
	}
}

bool Window::HandleKeyPress(SDL_KeyboardEvent& key) {
	if(pChildWindow != NULL) {
		return pChildWindow->HandleKeyPress(key);
	}

	if(IsEnabled() && (pWindowWidget != NULL)) {
		return pWindowWidget->HandleKeyPressOverlay(key) || pWindowWidget->HandleKeyPress(key);
	} else {
		return false;
	}
}

void Window::Draw(SDL_Surface* screen, Point Position) {
	if(IsVisible()) {
		if(bTransparentBackground == false) {

			if((bSelfGeneratedBackground == true) && (pBackground == NULL)) {
				pBackground = GUIStyle::GetInstance().CreateBackground(GetSize().x,GetSize().y);

			}

			if(pBackground != NULL) {
				// Draw background
				SDL_Rect RectScreen;
				RectScreen.x = GetPosition().x + (GetSize().x - pBackground->w)/2;
				RectScreen.y = GetPosition().y + (GetSize().y - pBackground->h)/2;;
				RectScreen.w = pBackground->w;
				RectScreen.h = pBackground->h;

				SDL_Rect RectBackground;
				RectBackground.x = RectBackground.y = 0;
				RectBackground.w = RectScreen.w;
				RectBackground.h = RectScreen.h;

				SDL_BlitSurface(pBackground,&RectBackground,screen,&RectScreen);
			}
		}

		if(pWindowWidget != NULL) {
			pWindowWidget->Draw(screen, Point(Position.x+GetPosition().x,Position.y+GetPosition().y));
		}
	}

	if(pChildWindow != NULL) {
		pChildWindow->Draw(screen);
	}
}

void Window::DrawOverlay(SDL_Surface* screen, Point Position) {
	if(pChildWindow != NULL) {
		pChildWindow->DrawOverlay(screen);
	} else if(IsVisible() && (pWindowWidget != NULL)) {
		pWindowWidget->DrawOverlay(screen, Point(Position.x+GetPosition().x,Position.y+GetPosition().y));
	}
}

void Window::Resize(Uint32 width, Uint32 height) {
	Widget::Resize(width,height);
	if(pWindowWidget != NULL) {
		pWindowWidget->Resize(width,height);
	}

	if(bSelfGeneratedBackground == true) {
		if(pBackground != NULL) {
			SDL_FreeSurface(pBackground);
			pBackground = NULL;
		}

		// the new background is created when the window is drawn next time
	}
}

void Window::SetBackground(SDL_Surface* pBackground, bool bFreeBackground) {
	if(((bSelfGeneratedBackground == true) || (this->bFreeBackground == true)) && (this->pBackground != NULL)) {
		SDL_FreeSurface(this->pBackground);
		this->pBackground = NULL;
	}

	if(pBackground == NULL) {
		bSelfGeneratedBackground = true;
		this->bFreeBackground = false;
		this->pBackground = NULL;
	} else {
		bSelfGeneratedBackground = false;
		this->pBackground = pBackground;
		this->bFreeBackground = bFreeBackground;
	}
}

void Window::SetTransparentBackground(bool bTransparent) {
	bTransparentBackground = bTransparent;
}
