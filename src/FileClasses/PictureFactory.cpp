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

#include <FileClasses/PictureFactory.h>

#include <globals.h>

#include <config.h>

#include <FileClasses/FileManager.h>
#include <FileClasses/FontManager.h>
#include <FileClasses/Cpsfile.h>

#include <misc/draw_util.h>

#include <stdexcept>

#include <misc/memory.h>

using std::shared_ptr;

PictureFactory::PictureFactory() {
    shared_ptr<SDL_Surface> ScreenPic = shared_ptr<SDL_Surface>( LoadCPS_RW(pFileManager->OpenFile("SCREEN.CPS"),true), SDL_FreeSurface);
    if(ScreenPic.get() == NULL) {
        throw std::runtime_error("PictureFactory::PictureFactory(): Cannot read SCREEN.CPS!");
    }

    shared_ptr<SDL_Surface> FamePic = shared_ptr<SDL_Surface>( LoadCPS_RW(pFileManager->OpenFile("FAME.CPS"),true), SDL_FreeSurface);
    if(FamePic.get() == NULL) {
        throw std::runtime_error("PictureFactory::PictureFactory(): Cannot read FAME.CPS!");
    }

    shared_ptr<SDL_Surface> ChoamPic = shared_ptr<SDL_Surface>( LoadCPS_RW(pFileManager->OpenFile("CHOAM.CPS"),true), SDL_FreeSurface);
    if(ChoamPic.get() == NULL) {
        throw std::runtime_error("PictureFactory::PictureFactory(): Cannot read CHOAM.CPS!");
    }

    CreditsBorder = shared_ptr<SDL_Surface>(GetSubPicture(ScreenPic.get() ,257,2,63,13), SDL_FreeSurface);

	// background
	Background = shared_ptr<SDL_Surface>( SDL_CreateRGBSurface(SDL_HWSURFACE,settings.Video.Width,settings.Video.Height,8,0,0,0,0), SDL_FreeSurface);
	if(Background.get() == NULL) {
		throw std::runtime_error("PictureFactory::PictureFactory: Cannot create new Picture!");
	}
	palette.applyToSurface(Background.get());

    shared_ptr<SDL_Surface> PatternNormal = shared_ptr<SDL_Surface>( GetSubPicture(FamePic.get(),0,1,63,67), SDL_FreeSurface);
	shared_ptr<SDL_Surface> PatternHFlipped = shared_ptr<SDL_Surface>( FlipHPicture(GetSubPicture(FamePic.get(),0,1,63,67)), SDL_FreeSurface);
	shared_ptr<SDL_Surface> PatternVFlipped = shared_ptr<SDL_Surface>( FlipVPicture(GetSubPicture(FamePic.get(),0,1,63,67)), SDL_FreeSurface);
	shared_ptr<SDL_Surface> PatternHVFlipped = shared_ptr<SDL_Surface>( FlipHPicture(FlipVPicture(GetSubPicture(FamePic.get(),0,1,63,67))), SDL_FreeSurface);

	SDL_Rect dest;
	dest.w = 63;
	dest.h = 67;
	for(dest.y = 0; dest.y < settings.Video.Height; dest.y+= 67) {
		for(dest.x = 0; dest.x < settings.Video.Width; dest.x+= 63) {
			if((dest.x % (63*2) == 0) && (dest.y % (67*2) == 0)) {
				SDL_BlitSurface(PatternNormal.get(), NULL, Background.get(), &dest);
			} else if((dest.x % (63*2) != 0) && (dest.y % (67*2) == 0)) {
				SDL_BlitSurface(PatternHFlipped.get(), NULL, Background.get(), &dest);
			} else if((dest.x % (63*2) == 0) && (dest.y % (67*2) != 0)) {
				SDL_BlitSurface(PatternVFlipped.get(), NULL, Background.get(), &dest);
			} else /*if((dest.x % (63*2) != 0) && (dest.y % (67*2) != 0))*/ {
				SDL_BlitSurface(PatternHVFlipped.get(), NULL, Background.get(), &dest);
			}
		}
	}

	// decoration border
	DecorationBorder.ball = shared_ptr<SDL_Surface>( GetSubPicture(ScreenPic.get(),241,124,12,11), SDL_FreeSurface);
	DecorationBorder.vspacer = shared_ptr<SDL_Surface>( GetSubPicture(ScreenPic.get(),241,118,12,5), SDL_FreeSurface);
	DecorationBorder.hspacer = shared_ptr<SDL_Surface>( RotatePictureRight(copySurface(DecorationBorder.vspacer.get() )), SDL_FreeSurface);
	DecorationBorder.vborder = shared_ptr<SDL_Surface>( GetSubPicture(ScreenPic.get(),241,71,12,13), SDL_FreeSurface);
	DecorationBorder.hborder = shared_ptr<SDL_Surface>( RotatePictureRight(copySurface(DecorationBorder.vborder.get() )), SDL_FreeSurface);

	// simple Frame
	Frame[SimpleFrame].LeftUpperCorner = shared_ptr<SDL_Surface>( GetSubPicture(ChoamPic.get(),120,17,8,8), SDL_FreeSurface);
	putpixel(Frame[SimpleFrame].LeftUpperCorner.get(),7,7,0);
	putpixel(Frame[SimpleFrame].LeftUpperCorner.get(),6,7,0);
	putpixel(Frame[SimpleFrame].LeftUpperCorner.get(),7,6,0);

	Frame[SimpleFrame].RightUpperCorner = shared_ptr<SDL_Surface>( GetSubPicture(ChoamPic.get(),312,17,8,8), SDL_FreeSurface);
	putpixel(Frame[SimpleFrame].RightUpperCorner.get(),0,7,0);
	putpixel(Frame[SimpleFrame].RightUpperCorner.get(),0,6,0);
	putpixel(Frame[SimpleFrame].RightUpperCorner.get(),1,7,0);

	Frame[SimpleFrame].LeftLowerCorner = shared_ptr<SDL_Surface>( GetSubPicture(ChoamPic.get(),120,31,8,8), SDL_FreeSurface);
	putpixel(Frame[SimpleFrame].LeftLowerCorner.get(),7,0,0);
	putpixel(Frame[SimpleFrame].LeftLowerCorner.get(),6,0,0);
	putpixel(Frame[SimpleFrame].LeftLowerCorner.get(),7,1,0);

	Frame[SimpleFrame].RightLowerCorner = shared_ptr<SDL_Surface>( GetSubPicture(ChoamPic.get(),312,31,8,8), SDL_FreeSurface);
	putpixel(Frame[SimpleFrame].RightLowerCorner.get(),0,0,0);
	putpixel(Frame[SimpleFrame].RightLowerCorner.get(),1,0,0);
	putpixel(Frame[SimpleFrame].RightLowerCorner.get(),0,1,0);

	Frame[SimpleFrame].hborder = shared_ptr<SDL_Surface>( GetSubPicture(ChoamPic.get(),128,17,1,4), SDL_FreeSurface);
	Frame[SimpleFrame].vborder = shared_ptr<SDL_Surface>( GetSubPicture(ChoamPic.get(),120,25,4,1), SDL_FreeSurface);

	// Decoration Frame 1
	Frame[DecorationFrame1].LeftUpperCorner = shared_ptr<SDL_Surface>( GetSubPicture(ChoamPic.get(),2,57,11,12), SDL_FreeSurface);
	putpixel(Frame[DecorationFrame1].LeftUpperCorner.get(),10,11,0);
	putpixel(Frame[DecorationFrame1].LeftUpperCorner.get(),9,11,0);
	putpixel(Frame[DecorationFrame1].LeftUpperCorner.get(),10,10,0);

	Frame[DecorationFrame1].RightUpperCorner = shared_ptr<SDL_Surface>( GetSubPicture(ChoamPic.get(),44,57,11,12), SDL_FreeSurface);
	putpixel(Frame[DecorationFrame1].RightUpperCorner.get(),0,11,0);
	putpixel(Frame[DecorationFrame1].RightUpperCorner.get(),0,10,0);
	putpixel(Frame[DecorationFrame1].RightUpperCorner.get(),1,11,0);

	Frame[DecorationFrame1].LeftLowerCorner = shared_ptr<SDL_Surface>( GetSubPicture(ChoamPic.get(),2,132,11,11), SDL_FreeSurface);
	putpixel(Frame[DecorationFrame1].LeftLowerCorner.get(),10,0,0);
	putpixel(Frame[DecorationFrame1].LeftLowerCorner.get(),9,0,0);
	putpixel(Frame[DecorationFrame1].LeftLowerCorner.get(),10,1,0);

	Frame[DecorationFrame1].RightLowerCorner = shared_ptr<SDL_Surface>( GetSubPicture(ChoamPic.get(),44,132,11,11), SDL_FreeSurface);
	putpixel(Frame[DecorationFrame1].RightLowerCorner.get(),0,0,0);
	putpixel(Frame[DecorationFrame1].RightLowerCorner.get(),1,0,0);
	putpixel(Frame[DecorationFrame1].RightLowerCorner.get(),0,1,0);

	Frame[DecorationFrame1].hborder = shared_ptr<SDL_Surface>( GetSubPicture(ChoamPic.get(),13,57,1,4), SDL_FreeSurface);
	Frame[DecorationFrame1].vborder = shared_ptr<SDL_Surface>( GetSubPicture(ChoamPic.get(),2,69,4,1), SDL_FreeSurface);

	// Decoration Frame 2
	Frame[DecorationFrame2].LeftUpperCorner = shared_ptr<SDL_Surface>( GetSubPicture(ChoamPic.get(),121,41,9,9), SDL_FreeSurface);
	drawhline(Frame[DecorationFrame2].LeftUpperCorner.get(),6,6,8,0);
	drawhline(Frame[DecorationFrame2].LeftUpperCorner.get(),6,7,8,0);
	drawhline(Frame[DecorationFrame2].LeftUpperCorner.get(),6,8,8,0);

	Frame[DecorationFrame2].RightUpperCorner = shared_ptr<SDL_Surface>( GetSubPicture(ChoamPic.get(),309,41,10,9), SDL_FreeSurface);
	drawhline(Frame[DecorationFrame2].RightUpperCorner.get(),0,6,3,0);
	drawhline(Frame[DecorationFrame2].RightUpperCorner.get(),0,7,3,0);
	drawhline(Frame[DecorationFrame2].RightUpperCorner.get(),0,8,3,0);

	Frame[DecorationFrame2].LeftLowerCorner = shared_ptr<SDL_Surface>( GetSubPicture(ChoamPic.get(),121,157,9,10), SDL_FreeSurface);
	drawhline(Frame[DecorationFrame2].LeftLowerCorner.get(),6,0,8,0);
	drawhline(Frame[DecorationFrame2].LeftLowerCorner.get(),6,1,8,0);
	drawhline(Frame[DecorationFrame2].LeftLowerCorner.get(),6,2,8,0);
	drawhline(Frame[DecorationFrame2].LeftLowerCorner.get(),7,3,8,0);

	Frame[DecorationFrame2].RightLowerCorner = shared_ptr<SDL_Surface>( GetSubPicture(ChoamPic.get(),309,158,10,9), SDL_FreeSurface);
	drawhline(Frame[DecorationFrame2].RightLowerCorner.get(),0,0,3,0);
	drawhline(Frame[DecorationFrame2].RightLowerCorner.get(),0,1,3,0);
	drawhline(Frame[DecorationFrame2].RightLowerCorner.get(),0,2,3,0);

	Frame[DecorationFrame2].hborder = shared_ptr<SDL_Surface>( GetSubPicture(ChoamPic.get(),133,41,1,4), SDL_FreeSurface);
	Frame[DecorationFrame2].vborder = shared_ptr<SDL_Surface>( GetSubPicture(ChoamPic.get(),121,51,4,1), SDL_FreeSurface);

	for(int i=0; i < NUM_DECORATIONFRAMES; i++) {
		SDL_SetColorKey(Frame[i].LeftUpperCorner.get(), SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
		SDL_SetColorKey(Frame[i].LeftLowerCorner.get(), SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
		SDL_SetColorKey(Frame[i].RightUpperCorner.get(), SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
		SDL_SetColorKey(Frame[i].RightLowerCorner.get(), SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
		SDL_SetColorKey(Frame[i].hborder.get(), SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
		SDL_SetColorKey(Frame[i].vborder.get(), SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
	}

	// House Logos
	HarkonnenLogo = shared_ptr<SDL_Surface>( GetSubPicture(FamePic.get(),10,137,53,54), SDL_FreeSurface);
	AtreidesLogo = shared_ptr<SDL_Surface>( GetSubPicture(FamePic.get(),66,137,53,54), SDL_FreeSurface);
	OrdosLogo = shared_ptr<SDL_Surface>( GetSubPicture(FamePic.get(),122,137,53,54), SDL_FreeSurface);

	GameStatsBackground = shared_ptr<SDL_Surface>( copySurface(Background.get()), SDL_FreeSurface);
	shared_ptr<SDL_Surface> FamePic2 = shared_ptr<SDL_Surface>( DoublePicture(FamePic.get(), false), SDL_FreeSurface);
	shared_ptr<SDL_Surface> pSurface = shared_ptr<SDL_Surface>( GetSubPicture(FamePic2.get(),16,160,610,74), SDL_FreeSurface);
	dest.x = 16;
	dest.y = 234;
	dest.w = pSurface.get()->w;
	dest.h = pSurface.get()->h;
	SDL_BlitSurface(pSurface.get(), NULL, FamePic2.get(), &dest);
	dest.y += 74;
	SDL_BlitSurface(pSurface.get() , NULL, FamePic2.get(), &dest);

	dest.x = (GameStatsBackground.get()->w - FamePic2.get()->w)/2;
	dest.y = (GameStatsBackground.get()->h - FamePic2.get()->h)/2;
	dest.w = FamePic2.get()->w;
	dest.h = FamePic2.get()->h;
	SDL_BlitSurface(FamePic2.get(), NULL, GameStatsBackground.get() , &dest);

	MessageBoxBorder = shared_ptr<SDL_Surface>(GetSubPicture(ScreenPic.get(),0,17,320,22), SDL_FreeSurface);
}

PictureFactory::~PictureFactory() {
}

SDL_Surface* PictureFactory::createTopBar() {
	SDL_Surface* TopBar;
	TopBar = GetSubPicture(Background.get() ,0,0,settings.Video.Width-GAMEBARWIDTH,32+12);
	SDL_Rect dest1 = {0,31,TopBar->w,12};
	SDL_FillRect(TopBar,&dest1,0);

	SDL_Rect dest2 = {0,32,DecorationBorder.hborder.get()->w,DecorationBorder.hborder.get()->h};
	for(dest2.x = 0; dest2.x < TopBar->w; dest2.x+=DecorationBorder.hborder.get()->w) {
		SDL_BlitSurface(DecorationBorder.hborder.get(),NULL,TopBar,&dest2);
	}

	drawvline(TopBar,TopBar->w-7,32,TopBar->h-1,96);

	SDL_Rect dest3 = {TopBar->w - 6,TopBar->h-12,12,5};
	SDL_BlitSurface(DecorationBorder.hspacer.get(),NULL,TopBar,&dest3);

	drawvline(TopBar,TopBar->w-1,0,TopBar->h-1,0);

	return TopBar;
}

SDL_Surface* PictureFactory::createGameBar() {
	SDL_Surface* GameBar;
	GameBar = GetSubPicture(Background.get(),0,0,GAMEBARWIDTH,settings.Video.Height);
	SDL_Rect dest1 = {0,0,13,GameBar->h};
	SDL_FillRect(GameBar,&dest1,0);


	SDL_Rect dest2 = {0,0,DecorationBorder.vborder.get()->w,DecorationBorder.vborder.get()->h};
	for(dest2.y = 0; dest2.y < GameBar->h; dest2.y+=DecorationBorder.vborder.get()->h) {
		SDL_BlitSurface(DecorationBorder.vborder.get(),NULL,GameBar,&dest2);
	}

	SDL_Rect dest3 = {0,32-DecorationBorder.vspacer.get()->h-1,DecorationBorder.vspacer.get()->w,DecorationBorder.vspacer.get()->h};
	SDL_BlitSurface(DecorationBorder.vspacer.get(),NULL,GameBar,&dest3);

	drawhline(GameBar,0,32-DecorationBorder.vspacer.get()->h-2,DecorationBorder.vspacer.get()->w-1,96);
	drawhline(GameBar,0,31,DecorationBorder.vspacer.get()->w-1,0);

	SDL_Rect dest4 = {0,32,DecorationBorder.ball.get()->w,DecorationBorder.ball.get()->h};
	SDL_BlitSurface(DecorationBorder.ball.get(),NULL,GameBar,&dest4);

	drawhline(GameBar,0,43,DecorationBorder.vspacer.get()->w-1,0);
	SDL_Rect dest5 = {0,44,DecorationBorder.vspacer.get()->w,DecorationBorder.vspacer.get()->h};
	SDL_BlitSurface(DecorationBorder.vspacer.get(),NULL,GameBar,&dest5);
	drawhline(GameBar,0,44+DecorationBorder.vspacer.get()->h,DecorationBorder.vspacer.get()->w-1,96);

	SDL_Rect dest6 = {13,0,GameBar->w-1,132};
	SDL_FillRect(GameBar,&dest6,0);
	drawrect(GameBar,13,1,GameBar->w-2,130,115);

	SDL_Rect dest7 = {0,132-DecorationBorder.vspacer.get()->h-1,DecorationBorder.vspacer.get()->w,DecorationBorder.vspacer.get()->h};
	SDL_BlitSurface(DecorationBorder.vspacer.get(),NULL,GameBar,&dest7);

	drawhline(GameBar,0,132-DecorationBorder.vspacer.get()->h-2,DecorationBorder.vspacer.get()->w-1,96);
	drawhline(GameBar,0,131,DecorationBorder.vspacer.get()->w-1,0);

	SDL_Rect dest8 = {0,132,DecorationBorder.ball.get()->w,DecorationBorder.ball.get()->h};
	SDL_BlitSurface(DecorationBorder.ball.get(),NULL,GameBar,&dest8);

	drawhline(GameBar,0,143,DecorationBorder.vspacer.get()->w-1,0);
	SDL_Rect dest9 = {0,144,DecorationBorder.vspacer.get()->w,DecorationBorder.vspacer.get()->h};
	SDL_BlitSurface(DecorationBorder.vspacer.get(),NULL,GameBar,&dest9);
	drawhline(GameBar,0,144+DecorationBorder.vspacer.get()->h,DecorationBorder.vspacer.get()->w-1,96);

	SDL_Rect dest10 = {13,132,DecorationBorder.hspacer.get()->w,DecorationBorder.hspacer.get()->h};
	SDL_BlitSurface(DecorationBorder.hspacer.get(),NULL,GameBar,&dest10);

	drawvline(GameBar,18,132,132+DecorationBorder.hspacer.get()->h-1,96);
	drawhline(GameBar,13,132+DecorationBorder.hspacer.get()->h,GameBar->w-1,0);

	SDL_Rect dest11 = {0,132,DecorationBorder.hborder.get()->w,DecorationBorder.hborder.get()->h};
	for(dest11.x = 19; dest11.x < GameBar->w; dest11.x+=DecorationBorder.hborder.get()->w) {
		SDL_BlitSurface(DecorationBorder.hborder.get(),NULL,GameBar,&dest11);
	}

	SDL_Rect dest12 = {46,132,CreditsBorder.get()->w,CreditsBorder.get()->h};
	SDL_BlitSurface(CreditsBorder.get(),NULL,GameBar,&dest12);

	return GameBar;
}

SDL_Surface* PictureFactory::createValidPlace() {
	SDL_Surface* ValidPlace;
	if((ValidPlace = SDL_CreateRGBSurface(SDL_HWSURFACE,16,16,8,0,0,0,0)) == NULL) {
		fprintf(stderr,"PictureFactory::createValidPlace: Cannot create new Picture!\n");
		exit(EXIT_FAILURE);
	}
	palette.applyToSurface(ValidPlace);

	if (!SDL_MUSTLOCK(ValidPlace) || (SDL_LockSurface(ValidPlace) == 0))
	{
		for(int y = 0; y < 16; y++) {
			for(int x = 0; x < 16; x++) {
				if(x%2 == y%2) {
					*((Uint8 *)ValidPlace->pixels + y * ValidPlace->pitch + x) = 4;
				} else {
					*((Uint8 *)ValidPlace->pixels + y * ValidPlace->pitch + x) = 0;
				}
			}
		}

		if (SDL_MUSTLOCK(ValidPlace))
			SDL_UnlockSurface(ValidPlace);
	}

	SDL_SetColorKey(ValidPlace, SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);

	return ValidPlace;
}

SDL_Surface* PictureFactory::createInvalidPlace() {
	SDL_Surface* InvalidPlace;
	if((InvalidPlace = SDL_CreateRGBSurface(SDL_HWSURFACE,16,16,8,0,0,0,0)) == NULL) {
		fprintf(stderr,"PictureFactory::createInvalidPlace: Cannot create new Picture!\n");
		exit(EXIT_FAILURE);
	}
	palette.applyToSurface(InvalidPlace);

	if (!SDL_MUSTLOCK(InvalidPlace) || (SDL_LockSurface(InvalidPlace) == 0))
	{
		for(int y = 0; y < 16; y++) {
			for(int x = 0; x < 16; x++) {
				if(x%2 == y%2) {
					*((Uint8 *)InvalidPlace->pixels + y * InvalidPlace->pitch + x) = 8;
				} else {
					*((Uint8 *)InvalidPlace->pixels + y * InvalidPlace->pitch + x) = 0;
				}
			}
		}

		if (SDL_MUSTLOCK(InvalidPlace))
			SDL_UnlockSurface(InvalidPlace);
	}

	SDL_SetColorKey(InvalidPlace, SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);

	return InvalidPlace;
}

void PictureFactory::drawFrame(SDL_Surface* Pic, unsigned int DecorationType, SDL_Rect* dest) {
	if(Pic == NULL)
		return;

	if(DecorationType >= NUM_DECORATIONFRAMES)
		return;

	SDL_Rect tmp;
	if(dest == NULL) {
		tmp.x = 0;
		tmp.y = 0;
		tmp.w = Pic->w;
		tmp.h = Pic->h;
		dest = &tmp;
	}

	//corners
	SDL_Rect dest1 = {	dest->x,
						dest->y,
						Frame[DecorationType].LeftUpperCorner.get()->w,
						Frame[DecorationType].LeftUpperCorner.get()->h};
	SDL_BlitSurface(Frame[DecorationType].LeftUpperCorner.get(),NULL,Pic,&dest1);

	SDL_Rect dest2 = {	dest->w - Frame[DecorationType].RightUpperCorner.get()->w,
						dest->y,
						Frame[DecorationType].RightUpperCorner.get()->w,
						Frame[DecorationType].RightUpperCorner.get()->h};
	SDL_BlitSurface(Frame[DecorationType].RightUpperCorner.get(),NULL,Pic,&dest2);

	SDL_Rect dest3 = {	dest->x,
						dest->h - Frame[DecorationType].LeftLowerCorner.get()->h,
						Frame[DecorationType].LeftLowerCorner.get()->w,
						Frame[DecorationType].LeftLowerCorner.get()->h};
	SDL_BlitSurface(Frame[DecorationType].LeftLowerCorner.get(),NULL,Pic,&dest3);

	SDL_Rect dest4 = {	dest->w - Frame[DecorationType].RightLowerCorner.get()->w,
						dest->h - Frame[DecorationType].RightLowerCorner.get()->h,
						Frame[DecorationType].RightLowerCorner.get()->w,
						Frame[DecorationType].RightLowerCorner.get()->h};
	SDL_BlitSurface(Frame[DecorationType].RightLowerCorner.get(),NULL,Pic,&dest4);

	//hborders
	SDL_Rect dest5 = { dest->x,dest->y,Frame[DecorationType].hborder.get()->w,Frame[DecorationType].hborder.get()->h};
	for(dest5.x = Frame[DecorationType].LeftUpperCorner.get()->w + dest->x;
		dest5.x <= dest->w - Frame[DecorationType].RightUpperCorner.get()->w - 1;
		dest5.x += Frame[DecorationType].hborder.get()->w) {
		SDL_BlitSurface(Frame[DecorationType].hborder.get(),NULL,Pic,&dest5);
	}

	SDL_Rect dest6 = { 	dest->x,dest->h - Frame[DecorationType].hborder->h,
						Frame[DecorationType].hborder.get()->w, Frame[DecorationType].hborder.get()->h};
	for(dest6.x = Frame[DecorationType].LeftLowerCorner.get()->w + dest->x;
		dest6.x <= dest->w - Frame[DecorationType].RightLowerCorner.get()->w - 1;
		dest6.x += Frame[DecorationType].hborder.get()->w) {
		SDL_BlitSurface(Frame[DecorationType].hborder.get(),NULL,Pic,&dest6);
	}

	//vborders
	SDL_Rect dest7 = { dest->x,dest->y,Frame[DecorationType].vborder.get()->w,Frame[DecorationType].vborder.get()->h};
	for(dest7.y = Frame[DecorationType].LeftUpperCorner.get()->h + dest->y;
		dest7.y <= dest->h - Frame[DecorationType].LeftLowerCorner.get()->h - 1;
		dest7.y += Frame[DecorationType].vborder.get()->h) {
		SDL_BlitSurface(Frame[DecorationType].vborder.get(),NULL,Pic,&dest7);
	}

	SDL_Rect dest8 = { 	dest->w - Frame[DecorationType].vborder.get()->w,dest->y,
						Frame[DecorationType].vborder.get()->w,Frame[DecorationType].vborder.get()->h};
	for(dest8.y = Frame[DecorationType].RightUpperCorner.get()->h + dest->y;
		dest8.y <= dest->h - Frame[DecorationType].RightLowerCorner.get()->h - 1;
		dest8.y += Frame[DecorationType].vborder.get()->h) {
		SDL_BlitSurface(Frame[DecorationType].vborder.get(),NULL,Pic,&dest8);
	}

}

SDL_Surface* PictureFactory::createFrame(unsigned int DecorationType,int width, int height,bool UseBackground) {
	SDL_Surface* Pic;
	if(UseBackground) {
		Pic = GetSubPicture(Background.get(),0,0,width,height);
	} else {
		if((Pic = SDL_CreateRGBSurface(SDL_HWSURFACE,width,height,8,0,0,0,0)) == NULL) {
			fprintf(stderr,"PictureFactory::createFrame: Cannot create new Picture!\n");
			exit(EXIT_FAILURE);
		}
		palette.applyToSurface(Pic);
		SDL_SetColorKey(Pic, SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
	}

	drawFrame(Pic,DecorationType);

	return Pic;
}

SDL_Surface* PictureFactory::createBackground() {
	return copySurface(Background.get());
}

SDL_Surface* PictureFactory::createMainBackground() {
	SDL_Surface* Pic;
	Pic = copySurface(Background.get());

	SDL_Rect dest0 = { 3,3,Pic->w-3,Pic->h-3};
	drawFrame(Pic,DecorationFrame2,&dest0);

	SDL_Rect dest1 = {11,11,HarkonnenLogo.get()->w,HarkonnenLogo.get()->h};
	SDL_BlitSurface(HarkonnenLogo.get(),NULL,Pic,&dest1);

	SDL_Rect dest2 = {Pic->w - 11 - AtreidesLogo.get()->w,11,AtreidesLogo.get()->w,AtreidesLogo.get()->h};
	SDL_BlitSurface(AtreidesLogo.get(),NULL,Pic,&dest2);

	SDL_Rect dest3 = {11,Pic->h - 11 - OrdosLogo.get()->h,OrdosLogo->w,OrdosLogo.get()->h};
	SDL_BlitSurface(OrdosLogo.get(),NULL,Pic,&dest3);

	SDL_Surface* Version = GetSubPicture(Background.get(),0,0,75,32);

	char versionString[100];
	sprintf(versionString, "%s", VERSION);

	SDL_Surface *VersionText = pFontManager->createSurfaceWithText(versionString, COLOR_BLACK, FONT_STD12);

	SDL_Rect dest4 = {	(Version->w - VersionText->w)/2, (Version->h - VersionText->h)/2 + 2,
						VersionText->w,VersionText->h};
	SDL_BlitSurface(VersionText,NULL,Version,&dest4);

	SDL_FreeSurface(VersionText);

	drawFrame(Version,SimpleFrame);

	SDL_Rect dest5 = {Pic->w - 11 - Version->w,Pic->h - 11 - Version->h,Version->w,Version->h};
	SDL_BlitSurface(Version,NULL,Pic,&dest5);

	SDL_FreeSurface(Version);

	return Pic;
}

SDL_Surface* PictureFactory::createGameStatsBackground(int House) {
    SDL_Surface* pSurface = copySurface(GameStatsBackground.get());

    SDL_Surface* pLogo = NULL;
    switch(House) {
        case HOUSE_HARKONNEN:
        case HOUSE_SARDAUKAR:
            pLogo = copySurface(HarkonnenLogo.get());
            break;

        case HOUSE_ATREIDES:
        case HOUSE_FREMEN:
            pLogo = copySurface(AtreidesLogo.get());
            break;

        case HOUSE_ORDOS:
        case HOUSE_MERCENARY:
            pLogo = copySurface(OrdosLogo.get());
            break;
    }

    pLogo = DoublePicture(pLogo);

    SDL_Rect dest = { GameStatsBackground.get()->w/2 - 320, GameStatsBackground.get()->h/2 - 200 + 16, pLogo->w, pLogo->h };
    SDL_BlitSurface(pLogo, NULL, pSurface, &dest);
    dest.x = GameStatsBackground.get()->w/2 + 320 - pLogo->w;
    SDL_BlitSurface(pLogo, NULL, pSurface, &dest);

    SDL_FreeSurface(pLogo);

    return pSurface;
}

SDL_Surface* PictureFactory::createMenu(SDL_Surface* CaptionPic,int y) {
	if(CaptionPic == NULL)
		return NULL;

	SDL_Surface* Pic = GetSubPicture(Background.get(), 0,0,CaptionPic->w,y);

	SDL_Rect dest1 = {0,0,CaptionPic->w,CaptionPic->h};
	SDL_BlitSurface(CaptionPic, NULL,Pic,&dest1);

	drawFrame(Pic,SimpleFrame,&dest1);

	SDL_Rect dest2 = {0,dest1.h,Pic->w,Pic->h};
	drawFrame(Pic,DecorationFrame1,&dest2);

	return Pic;
}

SDL_Surface* PictureFactory::createOptionsMenu() {
	SDL_Surface* tmp;
	if((tmp = SDL_LoadBMP_RW(pFileManager->OpenFile("UI_OptionsMenu.bmp"),true)) == NULL) {
		fprintf(stderr,"PictureFactory::createOptionsMenu(): Cannot load UI_OptionsMenu.bmp!\n");
		exit(EXIT_FAILURE);
	}
	SDL_SetColorKey(tmp, SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);

	SDL_Surface* Pic = GetSubPicture(Background.get(),0,0,tmp->w,tmp->h);
	SDL_BlitSurface(tmp,NULL,Pic,NULL);

	SDL_Rect dest1 = {0,0,Pic->w,27};
	drawFrame(Pic,SimpleFrame,&dest1);

	SDL_Rect dest2 = {0,dest1.h,Pic->w,Pic->h};
	drawFrame(Pic,DecorationFrame1,&dest2);

	SDL_FreeSurface(tmp);
	return Pic;
}

SDL_Surface* PictureFactory::createMessageBoxBorder() {
	return copySurface(MessageBoxBorder.get());
}

SDL_Surface* PictureFactory::createHouseSelect(SDL_Surface* HouseChoice) {

	unsigned char index2greyindex[] = {
		0, 0, 0, 13, 233, 127, 0, 131, 0, 0, 0, 0, 0, 13, 14, 15,
		0, 127, 0, 0, 0, 14, 0, 130, 24, 131, 131, 0, 0, 29, 0, 183,
		0, 128, 128, 0, 14, 14, 14, 130, 130, 0, 0, 0, 0, 13, 0, 29,
		0, 0, 30, 0, 0, 183, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		126, 0, 0, 126, 128, 0, 0, 0, 0, 0, 0, 0, 14, 0, 0, 0,
		0, 14, 0, 0, 0, 0, 0, 14, 0, 0, 130, 0, 131, 0, 13, 29,
		0, 30, 183, 175, 175, 0, 0, 0, 0, 0, 0, 0, 0, 233, 0, 0,
		14, 0, 14, 130, 24, 0, 0, 0, 131, 0, 175, 0, 24, 0, 0, 0,
		0, 14, 130, 131, 29, 133, 134, 0, 233, 0, 14, 24, 131, 13, 29, 183,
		30, 30, 183, 183, 175, 175, 150, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		24, 13, 29, 183, 175, 0, 0, 30, 0, 0, 13, 0, 0, 30, 174, 175,
		14, 24, 131, 13, 30, 183, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 30, 0, 175, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0,
		0, 0, 131, 13, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 13, 0, 0, 30, 183, 0, 0, 0, 0, 0, 0, 0 };


	SDL_Surface* Pic;
	Pic = copySurface(HouseChoice);

	for(int y = 50; y < Pic->h; y++) {
		for(int x = 0; x < Pic->w; x++) {
			unsigned char inputIndex = *( ((unsigned char*) (Pic->pixels)) + y*Pic->pitch + x);
			unsigned char outputIndex = index2greyindex[inputIndex];
			*( ((unsigned char*) (Pic->pixels)) + y*Pic->pitch + x) = outputIndex;
		}
	}

	drawFrame(Pic,SimpleFrame,NULL);

	return Pic;
}

SDL_Surface* PictureFactory::createMapChoiceScreen(int House) {
	SDL_Surface* MapChoiceScreen;

	if((MapChoiceScreen = LoadCPS_RW(pFileManager->OpenFile("MAPMACH.CPS"),true)) == NULL) {
		fprintf(stderr,"PictureFactory::createMapChoiceScreen(): Cannot read MAPMACH.CPS!\n");
		exit(EXIT_FAILURE);
	}

	SDL_Rect LeftLogo = {2,145,HarkonnenLogo.get()->w,HarkonnenLogo.get()->h};
	SDL_Rect RightLogo = {266,145,HarkonnenLogo.get()->w,HarkonnenLogo.get()->h};

	switch(House) {
		case HOUSE_ATREIDES:
		case HOUSE_FREMEN:
		{
			SDL_BlitSurface(AtreidesLogo.get(),NULL,MapChoiceScreen,&LeftLogo);
			SDL_BlitSurface(AtreidesLogo.get(),NULL,MapChoiceScreen,&RightLogo);
		} break;

		case HOUSE_ORDOS:
		case HOUSE_MERCENARY:
		{
			SDL_BlitSurface(OrdosLogo.get(),NULL,MapChoiceScreen,&LeftLogo);
			SDL_BlitSurface(OrdosLogo.get(),NULL,MapChoiceScreen,&RightLogo);
		} break;

		case HOUSE_HARKONNEN:
		case HOUSE_SARDAUKAR:
		default:
		{
			SDL_BlitSurface(HarkonnenLogo.get(),NULL,MapChoiceScreen,&LeftLogo);
			SDL_BlitSurface(HarkonnenLogo.get(),NULL,MapChoiceScreen,&RightLogo);
		} break;
	}

	switch(settings.General.Language) {
		case LNG_GER:
		{
			SDL_Surface* tmp = GetSubPicture(MapChoiceScreen,8,120, 303, 23);
			SDL_Rect dest = {8,0,303,23};
			SDL_BlitSurface(tmp,NULL,MapChoiceScreen,&dest);
			SDL_FreeSurface(tmp);
		} break;
		case LNG_FRE:
		{
			SDL_Surface* tmp = GetSubPicture(MapChoiceScreen,8,96, 303, 23);
			SDL_Rect dest = {8,0,303,23};
			SDL_BlitSurface(tmp,NULL,MapChoiceScreen,&dest);
			SDL_FreeSurface(tmp);
		} break;
		case LNG_ENG:
		default:
		{
			; // Nothing to do
		} break;
	}

	// clear everything in the middle
	SDL_Rect clearRect = {8,24,304,119};
	SDL_FillRect(MapChoiceScreen,&clearRect,0);

	MapChoiceScreen = DoublePicture(MapChoiceScreen);
	SDL_Surface* FullMapChoiceScreen = copySurface(Background.get());

	SDL_Rect dest = { FullMapChoiceScreen->w/2 - MapChoiceScreen->w/2, FullMapChoiceScreen->h/2 - MapChoiceScreen->h/2, MapChoiceScreen->w, MapChoiceScreen->h };
	SDL_BlitSurface(MapChoiceScreen,NULL,FullMapChoiceScreen,&dest);
	SDL_FreeSurface(MapChoiceScreen);
	return FullMapChoiceScreen;
}
