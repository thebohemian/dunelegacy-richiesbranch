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

#include <GUI/ScrollBar.h>
#include <cmath>

ScrollBar::ScrollBar() : Widget() {
    color = -1;
	MinValue = 1;
	MaxValue = 1;
	CurrentValue = 1;
	bigStepSize = 10;
	bDragSlider = false;

	EnableResizing(false,true);

	UpdateArrowButtonSurface();

	Arrow1.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&ScrollBar::OnArrow1)));
	Arrow2.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&ScrollBar::OnArrow2)));

	pBackground = NULL;

	Resize(GetMinimumSize().x,GetMinimumSize().y);
}

ScrollBar::~ScrollBar() {
	if(pBackground != NULL) {
		SDL_FreeSurface(pBackground);
	}
}

void ScrollBar::HandleMouseMovement(Sint32 x, Sint32 y, bool insideOverlay) {
	Arrow1.HandleMouseMovement(x,y,insideOverlay);
	Arrow2.HandleMouseMovement(x,y - GetSize().y + Arrow2.GetSize().y,insideOverlay);

	if(bDragSlider) {
		int SliderAreaHeight = GetSize().y - Arrow1.GetSize().y - Arrow2.GetSize().y;
		int Range = (MaxValue - MinValue + 1);

		double OneTickHeight = ((double)(SliderAreaHeight - SliderButton.GetSize().y)) / ((double) (Range-1));

		SetCurrentValue((int) ((y - dragPositionFromSliderTop - Arrow1.GetSize().y) / OneTickHeight));
	}
}

bool ScrollBar::HandleMouseLeft(Sint32 x, Sint32 y, bool pressed) {
	if(pressed == false) {
		bDragSlider = false;
	}

	if(x >= 0 && x < GetSize().x && y >= 0 && y < GetSize().y) {
		if(Arrow1.HandleMouseLeft(x, y, pressed) || Arrow2.HandleMouseLeft(x, y - GetSize().y + Arrow2.GetSize().y, pressed)) {
			// one of the arrow buttons clicked
			return true;
		} else {
			if(pressed) {
				if(y < SliderPosition.y) {
					// between up arrow and slider
					SetCurrentValue(CurrentValue-bigStepSize);
				} else if(y > SliderPosition.y + SliderButton.GetSize().y) {
					// between slider and down button
					SetCurrentValue(CurrentValue+bigStepSize);
				} else {
					// slider button
					bDragSlider = true;
					dragPositionFromSliderTop = y - SliderPosition.y;
				}
			}
			return true;
		}
	} else {
		return false;
	}
}

bool ScrollBar::HandleMouseWheel(Sint32 x, Sint32 y, bool up)  {
	if((x >= 0) && (x < GetSize().x) && (y >= 0) && (y < GetSize().y)) {
		if(up == true) {
			SetCurrentValue(CurrentValue-1);
		} else {
			SetCurrentValue(CurrentValue+1);
		}
		return true;
	} else {
		return false;
	}
}

bool ScrollBar::HandleKeyPress(SDL_KeyboardEvent& key) {
	return true;
}

void ScrollBar::Draw(SDL_Surface* screen, Point Position) {
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

	Arrow1.Draw(screen,Position);
	Point p = Position;
	p.y = p.y + GetSize().y - Arrow2.GetSize().y;
	Arrow2.Draw(screen,p);
	SliderButton.Draw(screen,Position+SliderPosition);
}

void ScrollBar::Resize(Uint32 width, Uint32 height) {
	Widget::Resize(width,height);

	if(pBackground != NULL) {
		SDL_FreeSurface(pBackground);
	}

	pBackground = GUIStyle::GetInstance().CreateWidgetBackground(width, height);

	UpdateSliderButton();
}

void ScrollBar::UpdateSliderButton() {
	double Range = (double) (MaxValue - MinValue + 1);
	int ArrowHeight = GUIStyle::GetInstance().GetMinimumScrollBarArrowButtonSize().y;
	double SliderAreaHeight = (double) (GetSize().y - 2*ArrowHeight);

	if(SliderAreaHeight < 0.0) {
        SliderAreaHeight = GUIStyle::GetInstance().GetMinimumScrollBarArrowButtonSize().y;
	}

	double SliderButtonHeight;
	double OneTickHeight;

	if(Range <= 1) {
		SliderButtonHeight = SliderAreaHeight;
		OneTickHeight = 0;
	} else {
		SliderButtonHeight = (SliderAreaHeight*bigStepSize) / (Range+1);
		if(SliderButtonHeight <= 5) {
			SliderButtonHeight = 5;
		}
		OneTickHeight = (SliderAreaHeight - SliderButtonHeight)/(Range-1);
	}

	SliderButton.Resize(GetSize().x, lround(SliderButtonHeight));
	SliderPosition.x = 0;
	SliderPosition.y = ArrowHeight +  lround((CurrentValue - MinValue)*OneTickHeight);
}

void ScrollBar::UpdateArrowButtonSurface() {
	Arrow1.SetSurfaces(	GUIStyle::GetInstance().CreateScrollBarArrowButton(false,false,false,color), true,
                        GUIStyle::GetInstance().CreateScrollBarArrowButton(false,true,true,color), true,
                        GUIStyle::GetInstance().CreateScrollBarArrowButton(false,false,true,color), true);

    Arrow2.SetSurfaces(	GUIStyle::GetInstance().CreateScrollBarArrowButton(true,false,false,color), true,
                        GUIStyle::GetInstance().CreateScrollBarArrowButton(true,true,true,color), true,
                        GUIStyle::GetInstance().CreateScrollBarArrowButton(true,false,true,color), true);
}


