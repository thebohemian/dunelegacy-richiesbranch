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

#include <GUI/dune/BuilderList.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <FileClasses/FontManager.h>
#include <FileClasses/TextManager.h>
#include <misc/draw_util.h>

#include <Game.h>
#include <House.h>
#include <SoundPlayer.h>
#include <sand.h>

#include <structures/BuilderClass.h>
#include <structures/StarPortClass.h>

#include <sstream>

BuilderList::BuilderList(Uint32 BuilderObjectID) : StaticContainer() {
	EnableResizing(false,true);
	this->BuilderObjectID = BuilderObjectID;

	SDL_Surface* surf, *surfPressed;

	surf = pGFXManager->getUIGraphic(UI_ButtonUp,pLocalHouse->getHouseID());
	surfPressed = pGFXManager->getUIGraphic(UI_ButtonUp_Pressed,pLocalHouse->getHouseID());

	Button_Up.SetSurfaces(surf,false,surfPressed,false);
	Button_Up.Resize(surf->w,surf->h);

	surf = pGFXManager->getUIGraphic(UI_ButtonDown,pLocalHouse->getHouseID());
	surfPressed = pGFXManager->getUIGraphic(UI_ButtonDown_Pressed,pLocalHouse->getHouseID());

	Button_Down.SetSurfaces(surf,false,surfPressed,false);
	Button_Down.Resize(surf->w,surf->h);

	AddWidget(&Button_Up,Point( (WIDGET_WIDTH - ARROWBTN_WIDTH)/2,0),Button_Up.GetSize());
	Button_Up.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&BuilderList::OnUp)));

	AddWidget(&Button_Down,
				Point( (WIDGET_WIDTH - ARROWBTN_WIDTH)/2,
						(ARROWBTN_HEIGHT + BUILDERBTN_SPACING)),
				Button_Down.GetSize());
	Button_Down.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&BuilderList::OnDown)));

	AddWidget(&Button_Order,
				Point(0,(ARROWBTN_HEIGHT + BUILDERBTN_SPACING) + BUILDERBTN_SPACING),
				Point(WIDGET_WIDTH,ORDERBTN_HEIGHT));
	Button_Order.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&BuilderList::OnOrder)));
	Button_Order.SetText(pTextManager->getLocalized("Order"));

	currentListPos = 0;

	MouseLeftButton = -1;
	MouseRightButton = -1;

	pLastTooltip = NULL;
	TooltipText = "";
	LastMouseMovement = (1<<31);

	Resize(GetMinimumSize().x,GetMinimumSize().y);
}

BuilderList::~BuilderList() {
	if(pLastTooltip != NULL) {
		SDL_FreeSurface(pLastTooltip);
		pLastTooltip = NULL;
	}
}

void BuilderList::HandleMouseMovement(Sint32 x, Sint32 y, bool insideOverlay) {
	StaticContainer::HandleMouseMovement(x,y,insideOverlay);

	if((x >= 0) && (x < GetSize().x) && (y >= 0) && (y < GetSize().y) && !insideOverlay) {
        LastMouseMovement = SDL_GetTicks();
		LastMousePos.x = x;
		LastMousePos.y = y;
	} else {
        LastMousePos.x = INVALID_POS;
        LastMousePos.y = INVALID_POS;
	}
}

bool BuilderList::HandleMouseLeft(Sint32 x, Sint32 y, bool pressed) {
	StaticContainer::HandleMouseLeft(x,y,pressed);

    BuilderClass* pBuilder = dynamic_cast<BuilderClass*>(currentGame->getObjectManager().getObject(BuilderObjectID));

	StarPortClass* pStarport = dynamic_cast<StarPortClass*>(pBuilder);
	if((pStarport != NULL) && (pStarport->okToOrder() == false)) {
		return false;
	}

	if(pressed == true) {
		MouseLeftButton = GetButton(x,y);
	} else {
		if(MouseLeftButton == GetButton(x,y)) {
			// button released
			if((GetItemIDFromIndex(MouseLeftButton) == (int) pBuilder->GetCurrentProducedItem()) && (pBuilder->IsWaitingToPlace() == true)) {
				soundPlayer->playSound(ButtonClick);
				if(currentGame->currentCursorMode == Game::CursorMode_Placing) {
                    currentGame->currentCursorMode = Game::CursorMode_Normal;
				} else {
				    currentGame->currentCursorMode = Game::CursorMode_Placing;
				}
			} else if((GetItemIDFromIndex(MouseLeftButton) == (int) pBuilder->GetCurrentProducedItem()) && (pBuilder->IsOnHold() == true)) {
				soundPlayer->playSound(ButtonClick);
                pBuilder->HandleSetOnHoldClick(false);
			} else {
			    if(GetItemIDFromIndex(MouseLeftButton) != ItemID_Invalid) {
                    soundPlayer->playSound(ButtonClick);
                    pBuilder->HandleProduceItemClick(GetItemIDFromIndex(MouseLeftButton), SDL_GetModState() & KMOD_SHIFT);
			    }
			}
		}

		MouseLeftButton = -1;
	}

	return true;
}

bool BuilderList::HandleMouseRight(Sint32 x, Sint32 y, bool pressed) {
	StaticContainer::HandleMouseRight(x,y,pressed);

    BuilderClass* pBuilder = dynamic_cast<BuilderClass*>(currentGame->getObjectManager().getObject(BuilderObjectID));

	StarPortClass* pStarport = dynamic_cast<StarPortClass*>(pBuilder);
	if((pStarport != NULL) && (pStarport->okToOrder() == false)) {
		return false;
	}

	if(pressed == true) {
		MouseRightButton = GetButton(x,y);
	} else {
		if(MouseRightButton == GetButton(x,y)) {
			// button released
			if((GetItemIDFromIndex(MouseRightButton) == (int) pBuilder->GetCurrentProducedItem()) && (pBuilder->IsOnHold() == false)) {
				soundPlayer->playSound(ButtonClick);
				pBuilder->HandleSetOnHoldClick(true);
			} else {
			    if(GetItemIDFromIndex(MouseRightButton) != ItemID_Invalid) {
			        soundPlayer->playSound(ButtonClick);
                    pBuilder->HandleCancelItemClick(GetItemIDFromIndex(MouseRightButton), SDL_GetModState() & KMOD_SHIFT);
			    }
			}
		}

		MouseRightButton = -1;
	}

	return true;
}

bool BuilderList::HandleKeyPress(SDL_KeyboardEvent& key) {
	return StaticContainer::HandleKeyPress(key);
}

void BuilderList::Draw(SDL_Surface* screen, Point Position) {
	SDL_Rect dest = {	Position.x,
						Position.y + ARROWBTN_HEIGHT + BUILDERBTN_SPACING,
						GetSize().x,
						GetRealHeight(GetSize().y) - 2*(ARROWBTN_HEIGHT + BUILDERBTN_SPACING) - BUILDERBTN_SPACING - ORDERBTN_HEIGHT };
	SDL_FillRect(screen, &dest, COLOR_BLACK);

    BuilderClass* pBuilder = dynamic_cast<BuilderClass*>(currentGame->getObjectManager().getObject(BuilderObjectID));
	if(pBuilder != NULL) {
		StarPortClass* pStarport = dynamic_cast<StarPortClass*>(pBuilder);

		if(pStarport != NULL) {
			Button_Order.SetVisible(true);
			Button_Order.SetEnabled(pStarport->okToOrder());
		} else {
			Button_Order.SetVisible(false);
		}

		if(GetNumButtons(GetSize().y) < (int) pBuilder->GetBuildList().size()) {
            Button_Up.SetVisible(true);
            Button_Down.SetVisible(true);

            if(currentListPos == 0) {
                Button_Up.SetEnabled(false);
            } else {
                Button_Up.SetEnabled(true);
            }

            if(currentListPos + GetNumButtons(GetSize().y) < (int) pBuilder->GetBuildList().size() ) {
                Button_Down.SetEnabled(true);
            } else {
                Button_Down.SetEnabled(false);
            }
		} else {
            Button_Up.SetVisible(false);
            Button_Down.SetVisible(false);
		}

		int i = 0;
		std::list<BuildItem>::const_iterator iter;
		for(iter = pBuilder->GetBuildList().begin(); iter != pBuilder->GetBuildList().end(); ++iter, i++) {

			if((i >= currentListPos) && (i < currentListPos+GetNumButtons(GetSize().y) )) {
				SDL_Surface* pSurface = resolveItemPicture(iter->ItemID);

				dest.x = Position.x + GetButtonPosition(i - currentListPos).x;
				dest.y = Position.y + GetButtonPosition(i - currentListPos).y;
				dest.w = pSurface->w;
				dest.h = pSurface->h;

				SDL_BlitSurface(pSurface, NULL, screen, &dest);

				if(isStructure(iter->ItemID)) {
                    SDL_Surface* pLattice = pGFXManager->getUIGraphic(UI_StructureSizeLattice);
                    SDL_Rect destLattice = { dest.x + 2, dest.y + 2, pLattice->w, pLattice->h };
                    SDL_BlitSurface(pLattice, NULL, screen, &destLattice);

                    SDL_Surface* pConcrete = pGFXManager->getUIGraphic(UI_StructureSizeConcrete);
                    SDL_Rect srcConcrete = { 0, 0, 1 + getStructureSize(iter->ItemID).x*6, 1 + getStructureSize(iter->ItemID).y*6 };
                    SDL_Rect destConcrete = { dest.x + 2, dest.y + 2, srcConcrete.w, srcConcrete.h };
                    SDL_BlitSurface(pConcrete, &srcConcrete, screen, &destConcrete);
				}

				// draw price
				char text[50];
				sprintf(text, "%d", iter->price);
				SDL_Surface* textSurface = pFontManager->createSurfaceWithText(text, COLOR_WHITE, FONT_STD10);
				SDL_Rect drawLocation;
				drawLocation.x = dest.x + 2;
				drawLocation.y = dest.y + BUILDERBTN_HEIGHT - textSurface->h + 3;
				drawLocation.w = textSurface->w;
				drawLocation.h = textSurface->h;
				SDL_BlitSurface(textSurface, NULL, screen, &drawLocation);
				SDL_FreeSurface(textSurface);

				if(pStarport != NULL) {
				    bool soldOut = (pStarport->getOwner()->getChoam().getNumAvailable(iter->ItemID) == 0);

					if((pStarport->okToOrder() == false) || (soldOut == true)) {
						if (!SDL_MUSTLOCK(screen) || (SDL_LockSurface(screen) == 0)) {
							for (int x = 0; x < BUILDERBTN_WIDTH; x++) {
								for (int y = (x % 2); y < BUILDERBTN_HEIGHT; y+=2) {
									putpixel(screen, x+dest.x, y+dest.y, COLOR_BLACK);
								}
							}

							if (SDL_MUSTLOCK(screen))
								SDL_UnlockSurface(screen);
						}
					}

					if(soldOut == true) {
						SDL_Surface* textSurface = pFontManager->createSurfaceWithMultilineText(pTextManager->getLocalized("SOLD OUT"), COLOR_WHITE, FONT_STD10, true);

						SDL_Rect drawLocation;
						drawLocation.x = dest.x + (BUILDERBTN_WIDTH - textSurface->w)/2;
						drawLocation.y = dest.y + (BUILDERBTN_HEIGHT - textSurface->h)/2;
						drawLocation.w = textSurface->w;
						drawLocation.h = textSurface->h;

						SDL_BlitSurface(textSurface, NULL, screen, &drawLocation);
						SDL_FreeSurface(textSurface);
					}

				} else if(iter->ItemID == Structure_Palace && pBuilder->getOwner()->getNumItems(Structure_Palace) > 0) {
                    if (!SDL_MUSTLOCK(screen) || (SDL_LockSurface(screen) == 0)) {
                        for (int x = 0; x < BUILDERBTN_WIDTH; x++) {
                            for (int y = (x % 2); y < BUILDERBTN_HEIGHT; y+=2) {
                                putpixel(screen, x+dest.x, y+dest.y, COLOR_BLACK);
                            }
                        }

                        if (SDL_MUSTLOCK(screen))
                            SDL_UnlockSurface(screen);
                    }

                    SDL_Surface* textSurface = pFontManager->createSurfaceWithMultilineText(pTextManager->getLocalized("ALREADY\nBUILT"), COLOR_WHITE, FONT_STD10, true);

                    SDL_Rect drawLocation;
                    drawLocation.x = dest.x + (BUILDERBTN_WIDTH - textSurface->w)/2;
                    drawLocation.y = dest.y + (BUILDERBTN_HEIGHT - textSurface->h)/2;
                    drawLocation.w = textSurface->w;
                    drawLocation.h = textSurface->h;

                    SDL_BlitSurface(textSurface, NULL, screen, &drawLocation);
                    SDL_FreeSurface(textSurface);
				} else if(iter->ItemID == pBuilder->GetCurrentProducedItem()) {
					double progress = pBuilder->GetProductionProgress();
					double price = (double) iter->price;
					int max_x = lround((progress/price)*(double)BUILDERBTN_WIDTH);

					if (!SDL_MUSTLOCK(screen) || (SDL_LockSurface(screen) == 0)) {
						for (int x = 0; x < max_x; x++) {
							for (int y = (x % 2); y < BUILDERBTN_HEIGHT; y+=2) {
								putpixel(screen, x+dest.x, y+dest.y, COLOR_BLACK);
							}
						}

						if (SDL_MUSTLOCK(screen))
							SDL_UnlockSurface(screen);
					}

					if(pBuilder->IsWaitingToPlace() == true) {
						SDL_Surface* textSurface = pFontManager->createSurfaceWithMultilineText(pTextManager->getLocalized("PLACE IT"), COLOR_WHITE, FONT_STD10, true);

						SDL_Rect drawLocation;
						drawLocation.x = dest.x + (BUILDERBTN_WIDTH - textSurface->w)/2;
						drawLocation.y = dest.y + (BUILDERBTN_HEIGHT - textSurface->h)/2;
						drawLocation.w = textSurface->w;
						drawLocation.h = textSurface->h;

						SDL_BlitSurface(textSurface, NULL, screen, &drawLocation);
						SDL_FreeSurface(textSurface);
					} else if(pBuilder->IsOnHold() == true) {
						SDL_Surface* textSurface = pFontManager->createSurfaceWithMultilineText(pTextManager->getLocalized("ON HOLD"), COLOR_WHITE, FONT_STD10, true);

						SDL_Rect drawLocation;
						drawLocation.x = dest.x + (BUILDERBTN_WIDTH - textSurface->w)/2;
						drawLocation.y = dest.y + (BUILDERBTN_HEIGHT - textSurface->h)/2;
						drawLocation.w = textSurface->w;
						drawLocation.h = textSurface->h;

						SDL_BlitSurface(textSurface, NULL, screen, &drawLocation);
						SDL_FreeSurface(textSurface);
					}
				}

				if(iter->num > 0) {
					// draw number of this in build list
					sprintf(text, "%d", iter->num);
					textSurface = pFontManager->createSurfaceWithText(text, COLOR_RED, FONT_STD10);
					drawLocation.x = dest.x + BUILDERBTN_WIDTH - textSurface->w - 2;
					drawLocation.y = dest.y + BUILDERBTN_HEIGHT - textSurface->h + 3;
					drawLocation.w = textSurface->w;
					drawLocation.h = textSurface->h;
					SDL_BlitSurface(textSurface, NULL, screen, &drawLocation);
					SDL_FreeSurface(textSurface);
				}
			}
		}
	}

	StaticContainer::Draw(screen,Position);
}

void BuilderList::DrawOverlay(SDL_Surface* screen, Point Position) {
	if((SDL_GetTicks() - LastMouseMovement) > 800) {
		// Draw tooltip

		int btn = GetButton(LastMousePos.x,LastMousePos.y);

		if(btn != -1) {

            BuilderClass* pBuilder = dynamic_cast<BuilderClass*>(currentGame->getObjectManager().getObject(BuilderObjectID));

			int j = 0;
			std::list<BuildItem>::const_iterator iter;
			for(iter = pBuilder->GetBuildList().begin(); iter != pBuilder->GetBuildList().end(); ++iter, j++) {
				if(j == btn) {
					break;
				}
			}

            std::string text = resolveItemName(iter->ItemID);

			if(text != TooltipText) {
				if(pLastTooltip != NULL) {
					SDL_FreeSurface(pLastTooltip);
					pLastTooltip = NULL;
				}
			}

			if(pLastTooltip == NULL) {
				pLastTooltip = GUIStyle::GetInstance().CreateToolTip(text);
				TooltipText = text;
			}

			SDL_Rect dest = {Position.x + GetButtonPosition(btn).x - pLastTooltip->w - 5, Position.y + LastMousePos.y,
								pLastTooltip->w,pLastTooltip->h};
			SDL_BlitSurface(pLastTooltip, NULL, screen, &dest);
		}

	}
}


void BuilderList::Resize(Uint32 width, Uint32 height) {
	SetWidgetGeometry(&Button_Up,Point( (WIDGET_WIDTH - ARROWBTN_WIDTH)/2,0),Button_Up.GetSize());
	SetWidgetGeometry(&Button_Down,
						Point( (WIDGET_WIDTH - ARROWBTN_WIDTH)/2, GetRealHeight(height) - ARROWBTN_HEIGHT - ORDERBTN_HEIGHT - BUILDERBTN_SPACING),
						Button_Down.GetSize());

	SetWidgetGeometry(&Button_Order,
						Point( 0, GetRealHeight(height) - ORDERBTN_HEIGHT),
						Point(WIDGET_WIDTH,ORDERBTN_HEIGHT));

	StaticContainer::Resize(width,height);


	// move list to show currently produced item
    BuilderClass* pBuilder = dynamic_cast<BuilderClass*>(currentGame->getObjectManager().getObject(BuilderObjectID));
	if(pBuilder != NULL) {
        int currentProducedItemPos = 0;
        for(std::list<BuildItem>::const_iterator iter = pBuilder->GetBuildList().begin();
             iter != pBuilder->GetBuildList().end();
             ++iter, ++currentProducedItemPos)
        {
            if(iter->ItemID == pBuilder->GetCurrentProducedItem()) {
                break;
            }
        }

        if(currentProducedItemPos < (int)pBuilder->GetBuildList().size()) {
            int biggestLegalPosition = ((int)pBuilder->GetBuildList().size()) - GetNumButtons(GetSize().y);
            currentListPos = std::max(0, std::min(currentProducedItemPos-1,biggestLegalPosition));
        }
	}
}

int BuilderList::GetRealHeight(int height) {
	int tmp = height;
	tmp -= (ARROWBTN_HEIGHT + BUILDERBTN_SPACING)*2;
	tmp -= BUILDERBTN_SPACING;
	tmp -= ORDERBTN_HEIGHT;
	tmp -= BUILDERBTN_SPACING;
	int NumButtons = tmp / (BUILDERBTN_HEIGHT + BUILDERBTN_SPACING);

	return NumButtons * (BUILDERBTN_HEIGHT + BUILDERBTN_SPACING) + 3*BUILDERBTN_SPACING + 2*ARROWBTN_HEIGHT + ORDERBTN_HEIGHT + BUILDERBTN_SPACING;
}

void BuilderList::OnUp() {
	if(currentListPos > 0) {
		currentListPos--;
	}
}

void BuilderList::OnDown() {
    BuilderClass* pBuilder = dynamic_cast<BuilderClass*>(currentGame->getObjectManager().getObject(BuilderObjectID));

	if(currentListPos < ((int)pBuilder->GetBuildList().size()) - GetNumButtons(GetSize().y)) {
		currentListPos++;
	}
}

void BuilderList::OnOrder() {
    BuilderClass* pBuilder = dynamic_cast<BuilderClass*>(currentGame->getObjectManager().getObject(BuilderObjectID));

	StarPortClass* pStarport = dynamic_cast<StarPortClass*>(pBuilder);
	if(pStarport != NULL) {
		pStarport->HandlePlaceOrderClick();
	}
}

int BuilderList::GetNumButtons(int height) {
	int tmp = height;
	tmp -= (ARROWBTN_HEIGHT + BUILDERBTN_SPACING)*2;
	tmp -= BUILDERBTN_SPACING;
	tmp -= ORDERBTN_HEIGHT;
	tmp -= BUILDERBTN_SPACING;
	return tmp / (BUILDERBTN_HEIGHT + BUILDERBTN_SPACING);
}

Point BuilderList::GetButtonPosition(int BtnNumber) {
	return Point(BUILDERBTN_SPACING,
					ARROWBTN_HEIGHT+2*BUILDERBTN_SPACING
					+ BtnNumber*(BUILDERBTN_HEIGHT+BUILDERBTN_SPACING));

}

int BuilderList::GetButton(int x, int y) {

    BuilderClass* pBuilder = dynamic_cast<BuilderClass*>(currentGame->getObjectManager().getObject(BuilderObjectID));

	if(pBuilder != NULL) {
		int i = 0;
		std::list<BuildItem>::const_iterator iter;
		for(iter = pBuilder->GetBuildList().begin(); iter != pBuilder->GetBuildList().end(); ++iter, i++) {

			if((i >= currentListPos) && (i < currentListPos+GetNumButtons(GetSize().y) )) {
				if(		(x >= GetButtonPosition(i - currentListPos).x)
					&&	(x < GetButtonPosition(i - currentListPos).x + BUILDERBTN_WIDTH)
					&&	(y >= GetButtonPosition(i - currentListPos).y)
					&&	(y < GetButtonPosition(i - currentListPos).y + BUILDERBTN_HEIGHT)) {

					return i;
				}
			}
		}
	}

	return -1;
}

int BuilderList::GetItemIDFromIndex(int i) {

    BuilderClass* pBuilder = dynamic_cast<BuilderClass*>(currentGame->getObjectManager().getObject(BuilderObjectID));

	if(pBuilder != NULL) {
		int j = 0;
		std::list<BuildItem>::const_iterator iter;
		for(iter = pBuilder->GetBuildList().begin(); iter != pBuilder->GetBuildList().end(); ++iter, j++) {
			if(j == i) {
				return iter->ItemID;
			}
		}
	}

	return ItemID_Invalid;
}
