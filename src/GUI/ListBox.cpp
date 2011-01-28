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

#include <GUI/ListBox.h>

ListBox::ListBox() : Widget() {
	EnableResizing(true,true);

	pBackground = NULL;
	pForeground = NULL;
	color = -1;
	bAutohideScrollbar = true;
	FirstVisibleElement = 0;
	SelectedElement = -1;
	LastClickTime = 0;

	scrollbar.SetOnChangeMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&ListBox::OnScrollbarChange)));
	Resize(GetMinimumSize().x,GetMinimumSize().y);
}

ListBox::~ListBox() {
	if(pBackground != NULL) {
		SDL_FreeSurface(pBackground);
	}

	if(pForeground != NULL) {
		SDL_FreeSurface(pForeground);
	}
}

void ListBox::HandleMouseMovement(Sint32 x, Sint32 y, bool insideOverlay) {
	scrollbar.HandleMouseMovement(x - GetSize().x + scrollbar.GetSize().x,y,insideOverlay);
}

bool ListBox::HandleMouseLeft(Sint32 x, Sint32 y, bool pressed) {
	if((x>=0) && (x < GetSize().x - scrollbar.GetSize().x)
		&& (y>=0) && (y < GetSize().y)) {

		if(pressed == true) {
			int index = ((y - 1) / GUIStyle::GetInstance().GetListBoxEntryHeight()) + FirstVisibleElement;
			if((index >= 0) && (index < GetNumEntries())) {
				SelectedElement = index;

				if(SDL_GetTicks() - LastClickTime < 200) {
					pOnDoubleClick();
				} else {
					LastClickTime = SDL_GetTicks();
					UpdateList();
					pOnSelectionChange();
					SetActive();
				}
			}
		}

		scrollbar.HandleMouseLeft(x - GetSize().x + scrollbar.GetSize().x,y,pressed);
		return true;
	} else {
		return scrollbar.HandleMouseLeft(x - GetSize().x + scrollbar.GetSize().x,y,pressed);
	}
}

bool ListBox::HandleMouseWheel(Sint32 x, Sint32 y, bool up)  {
	// forward mouse wheel event to scrollbar
	return scrollbar.HandleMouseWheel(0,0,up);
}

bool ListBox::HandleKeyPress(SDL_KeyboardEvent& key) {
	Widget::HandleKeyPress(key);
	if(IsActive()) {
		switch(key.keysym.sym) {
			case SDLK_UP: {
				if(SelectedElement > 0) {
					SetSelectedItem(SelectedElement-1);
				}
			} break;

			case SDLK_DOWN: {
				if(SelectedElement < GetNumEntries()-1 ) {
					SetSelectedItem(SelectedElement+1);
				}
			} break;

			default: {
			} break;
		}

		scrollbar.HandleKeyPress(key);
	}
	scrollbar.HandleKeyPress(key);
	return true;
}

void ListBox::Draw(SDL_Surface* screen, Point Position) {
	if(IsVisible() == false) {
		return;
	}

	if(pBackground != NULL) {
		SDL_Rect dest;
		dest.x = Position.x;
		dest.y = Position.y;
		dest.w = pBackground->w;
		dest.h = pBackground->h;

		SDL_BlitSurface(pBackground,NULL,screen,&dest);
	}

	SDL_Rect dest;
	dest.x = Position.x + 2;
	dest.y = Position.y + 1;
	dest.w = pForeground->w;
	dest.h = pForeground->h;
	SDL_BlitSurface(pForeground,NULL,screen,&dest);

	Point ScrollBarPos = Position;
	ScrollBarPos.x += GetSize().x - scrollbar.GetSize().x;

    if(!bAutohideScrollbar || (scrollbar.GetRangeMin() != scrollbar.GetRangeMax())) {
        scrollbar.Draw(screen,ScrollBarPos);
    }
}

void ListBox::Resize(Uint32 width, Uint32 height) {
	Widget::Resize(width,height);

	if(pBackground != NULL) {
		SDL_FreeSurface(pBackground);
	}

	pBackground = GUIStyle::GetInstance().CreateWidgetBackground(width, height);

	scrollbar.Resize(scrollbar.GetMinimumSize().x,height);

	UpdateList();
}

void ListBox::SetActive() {
	Widget::SetActive();

	if((SelectedElement == -1) && (GetNumEntries() > 0)) {
		SelectedElement = 0;
		UpdateList();
		pOnSelectionChange();
	}
}

void ListBox::SetSelectedItem(int index) {
    bool bChanged = (index != SelectedElement);

	if(index == -1) {
		SelectedElement = -1;
		UpdateList();
	} else if((index >= 0) && (index < GetNumEntries())) {
		SelectedElement = index;

		int numVisibleElements = ((GetSize().y - 2) / GUIStyle::GetInstance().GetListBoxEntryHeight()) + 1;

		if(SelectedElement >= FirstVisibleElement+numVisibleElements - 1) {
			FirstVisibleElement = SelectedElement-(numVisibleElements-1) + 1;
		} else if(SelectedElement < FirstVisibleElement) {
			FirstVisibleElement = SelectedElement;
		}

		if(FirstVisibleElement > GetNumEntries() - numVisibleElements) {
			FirstVisibleElement = std::max(0,GetNumEntries() - numVisibleElements + 1);
		}

		scrollbar.SetCurrentValue(FirstVisibleElement);

		UpdateList();
	}

	if(bChanged) {
        pOnSelectionChange();
	}
}

void ListBox::UpdateList() {
	if(pForeground != NULL) {
		SDL_FreeSurface(pForeground);
		pForeground = NULL;
	}

	// create surfaces
	int surfaceHeight = GetSize().y - 2;
	if(surfaceHeight < 0) {
        surfaceHeight = 0;
	}
	pForeground = GUIStyle::GetInstance().CreateEmptySurface(GetSize().x - 4, surfaceHeight,true);

	int numVisibleElements = surfaceHeight / GUIStyle::GetInstance().GetListBoxEntryHeight();

	SDL_Rect dest;
	dest.x = 0;
	dest.y = 0;
	for(int i = FirstVisibleElement; i < FirstVisibleElement + numVisibleElements; i++) {
		if(i >= GetNumEntries()) {
			break;
		}

		SDL_Surface* pSurface = GUIStyle::GetInstance().CreateListBoxEntry(GetSize().x - 4, GetEntry(i), (i==SelectedElement), color);
		dest.w = pSurface->w;
		dest.h = pSurface->h;
		SDL_BlitSurface(pSurface,NULL,pForeground,&dest);
		SDL_FreeSurface(pSurface);

		dest.y += GUIStyle::GetInstance().GetListBoxEntryHeight();
	}

	scrollbar.SetRange(0,std::max(0,GetNumEntries() - numVisibleElements));
	scrollbar.SetBigStepSize(std::min(GetNumEntries() - numVisibleElements, numVisibleElements-1));
}
