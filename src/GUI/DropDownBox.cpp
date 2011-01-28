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

#include <GUI/DropDownBox.h>

#include <algorithm>

DropDownBox::DropDownBox() : Widget() {
	EnableResizing(true,false);

	color = -1;
	bHover = false;
    UpdateButtonSurface();

	openListBoxButton.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&DropDownBox::OnOpenListBoxButton)));

	listBox.SetOnSelectionChangeMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&DropDownBox::OnSelectionChange)));

	pBackground = NULL;
	pForeground = NULL;
	pActiveForeground = NULL;

	bShowListBox = false;
	bAutocloseListBoxOnSelectionChange = true;

	Resize(GetMinimumSize().x,GetMinimumSize().y);
}

DropDownBox::~DropDownBox() {
	if(pBackground != NULL) {
		SDL_FreeSurface(pBackground);
	}

	if(pForeground != NULL) {
		SDL_FreeSurface(pForeground);
	}

	if(pActiveForeground != NULL) {
		SDL_FreeSurface(pActiveForeground);
	}
}

void DropDownBox::HandleMouseMovement(Sint32 x, Sint32 y, bool insideOverlay) {
    if((x < 0) || (x >= GetSize().x - openListBoxButton.GetSize().x - 1) || (y < 0) || (y >= GetSize().y)) {
		bHover = false;
	} else if(IsEnabled() && !insideOverlay) {
		bHover = true;
	} else {
        bHover = false;
	}

	openListBoxButton.HandleMouseMovement(x - (GetSize().x - openListBoxButton.GetSize().x - 1), y - 1, insideOverlay);

	if(bShowListBox) {
		listBox.HandleMouseMovement(x, y - GetSize().y, insideOverlay);
	}
}

bool DropDownBox::HandleMouseMovementOverlay(Sint32 x, Sint32 y) {
    int newY = y-GetSize().y;
    if(bShowListBox && x >= 0 && x < listBox.GetSize().x && newY >= 0 && newY < listBox.GetSize().y) {
        return true;
    } else {
        return false;
    }
}

bool DropDownBox::HandleMouseLeft(Sint32 x, Sint32 y, bool pressed) {
    if((IsEnabled() == false) || (IsVisible() == false)) {
        return true;
    }

	if(openListBoxButton.HandleMouseLeft(x - (GetSize().x - openListBoxButton.GetSize().x - 1), y - 1, pressed)) {
		SetActive();
		return true;
	} else {
		if((x>=0) && (x < GetSize().x - openListBoxButton.GetSize().x - 1)
			&& (y>=0) && (y < GetSize().y) && (pressed == true)) {

            if(pOnClick) {
                pOnClick();
            } else {
                SetActive();
                OnOpenListBoxButton();
            }
			return true;
		} else {
			return false;
		}
	}
}

bool DropDownBox::HandleMouseLeftOverlay(Sint32 x, Sint32 y, bool pressed) {
    if((IsEnabled() == false) || (IsVisible() == false)) {
        return false;
    }

	if(bShowListBox) {
		return listBox.HandleMouseLeft(x, y - GetSize().y, pressed);
	} else {
		return false;
	}
}

bool DropDownBox::HandleMouseWheel(Sint32 x, Sint32 y, bool up) {
	// forward mouse wheel event to list box
	if(x >= 0 && x < GetSize().x && y >= 0 && y < GetSize().y) {
		if(up) {
			if(listBox.GetSelectedIndex() < 0) {
				listBox.SetSelectedItem(0);
			} else if(listBox.GetSelectedIndex() > 0) {
				listBox.SetSelectedItem(listBox.GetSelectedIndex()-1);
			}
		} else {
			if(listBox.GetSelectedIndex() < 0) {
				listBox.SetSelectedItem(0);
			} else if(listBox.GetSelectedIndex() < listBox.GetNumEntries()-1) {
				listBox.SetSelectedItem(listBox.GetSelectedIndex()+1);
			}
		}
		return true;
	} else {
		return false;
	}
}

bool DropDownBox::HandleMouseWheelOverlay(Sint32 x, Sint32 y, bool up) {
	// forward mouse wheel event to list box
	if(bShowListBox) {
	    int newY = y-GetSize().y;
		listBox.HandleMouseWheel(x,newY,up);
		if(x >= 0 && x < listBox.GetSize().x && newY >= 0 && newY < listBox.GetSize().y) {
            return true;
		} else {
            return false;
		}
	} else {
		return false;
	}
}

bool DropDownBox::HandleKeyPress(SDL_KeyboardEvent& key) {

	Widget::HandleKeyPress(key);
	if(IsActive()) {
		// disable autoclosing of the list box
		bool bSavedAutoclose = bAutocloseListBoxOnSelectionChange;
		bAutocloseListBoxOnSelectionChange = false;

		switch(key.keysym.sym) {
			case SDLK_UP: {
				if(listBox.GetSelectedIndex() < 0) {
					listBox.SetSelectedItem(0);
				} else if(listBox.GetSelectedIndex() > 0) {
					listBox.SetSelectedItem(listBox.GetSelectedIndex()-1);
				}
			} break;

			case SDLK_DOWN: {
				if(listBox.GetSelectedIndex() < 0) {
					listBox.SetSelectedItem(0);
				} else if(listBox.GetSelectedIndex() < listBox.GetNumEntries()-1) {
					listBox.SetSelectedItem(listBox.GetSelectedIndex()+1);
				}
			} break;

			case SDLK_SPACE: {
				bShowListBox = !bShowListBox;
			}

			case SDLK_TAB: {
                SetInactive();
            }

			default: {
			} break;
		}

		bAutocloseListBoxOnSelectionChange = bSavedAutoclose;
		return true;
	}

	return false;

}

void DropDownBox::Draw(SDL_Surface* screen, Point Position) {
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

	if(pForeground != NULL && pActiveForeground != NULL) {
		SDL_Rect dest;
		dest.x = Position.x + 2;
		dest.y = Position.y + 2;

		if(((bHover == true) && pOnClick) || IsActive()) {
            dest.w = pActiveForeground->w;
            dest.h = pActiveForeground->h;
            SDL_BlitSurface(pActiveForeground,NULL,screen,&dest);
		} else {
            dest.w = pForeground->w;
            dest.h = pForeground->h;
            SDL_BlitSurface(pForeground,NULL,screen,&dest);
		}
	}

	openListBoxButton.Draw(screen, Position + Point(GetSize().x - openListBoxButton.GetSize().x - 1, 1));
}

void DropDownBox::DrawOverlay(SDL_Surface* screen, Point Position) {
	if(bShowListBox) {
		listBox.Draw(screen, Position + Point(0,GetSize().y));
	}
}

void DropDownBox::Resize(Uint32 width, Uint32 height) {
	Widget::Resize(width,height);

	if(pBackground != NULL) {
		SDL_FreeSurface(pBackground);
	}

	pBackground = GUIStyle::GetInstance().CreateWidgetBackground(width, height);

	UpdateForeground();

    ResizeListBox();
}

void DropDownBox::ResizeListBox() {
    int listBoxHeight = std::max(1,std::min(7,GetNumEntries())) * (int) GUIStyle::GetInstance().GetListBoxEntryHeight() + 2;
    listBox.Resize(GetSize().x-1, listBoxHeight);
}

void DropDownBox::SetActive(bool bActive) {
	if(bActive == false) {
		bShowListBox = false;
		openListBoxButton.SetInactive();
	} else {
        openListBoxButton.SetActive();
	}
	Widget::SetActive(bActive);
}

void DropDownBox::OnSelectionChange() {
    UpdateForeground();

	if(bAutocloseListBoxOnSelectionChange) {
		bShowListBox = false;
	}

	pOnSelectionChange();
}

void DropDownBox::UpdateButtonSurface() {
    openListBoxButton.SetSurfaces(	GUIStyle::GetInstance().CreateDropDownBoxButton(17,false,false,color), true,
									GUIStyle::GetInstance().CreateDropDownBoxButton(17,true,true,color), true,
									GUIStyle::GetInstance().CreateDropDownBoxButton(17,false,true,color), true);
}

void DropDownBox::UpdateForeground() {
    if(pForeground != NULL) {
		SDL_FreeSurface(pForeground);
		pForeground = NULL;
	}

    if(pActiveForeground != NULL) {
		SDL_FreeSurface(pActiveForeground);
		pActiveForeground = NULL;
	}

    if(listBox.GetSelectedIndex() >= 0) {
		pForeground = GUIStyle::GetInstance().CreateListBoxEntry(GetSize().x - 17, listBox.GetEntry(listBox.GetSelectedIndex()), false, color);
		pActiveForeground = GUIStyle::GetInstance().CreateListBoxEntry(GetSize().x - 17, listBox.GetEntry(listBox.GetSelectedIndex()), true, color);
	}
}
