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

#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include "Widget.h"

#include <string>
#include <SDL.h>


/// A class for a progress bar widget
class ProgressBar : public Widget {
public:

	/// default constructor
	ProgressBar() : Widget() {
		Percent = 0.0;
		color = -1;
		pBackground = NULL;
		pForeground = NULL;
		bFreeBackground = true;
		EnableResizing(true,true);
	}

	/// destructor
	virtual ~ProgressBar() {
		if((bFreeBackground == true) && (pBackground != NULL)) {
			SDL_FreeSurface(pBackground);
		}

		if(pForeground != NULL) {
			SDL_FreeSurface(pForeground);
		}
	}

	/**
		Sets the progress of this progress bar.
		\param Percent	Should be between 0.0 and 100.0
	*/
	void SetProgress(double Percent) {
		if(Percent != this->Percent) {
			this->Percent = Percent;
			if(Percent < 0.0) {
				Percent = 0.0;
			} else if(Percent > 100.0) {
				Percent = 100.0;
			}

			if(pForeground != NULL) {
				SDL_FreeSurface(pForeground);
				pForeground = NULL;
			}

			pForeground = GUIStyle::GetInstance().CreateProgressBarOverlay(GetSize().x, GetSize().y, Percent, color);
		}
	}

	/**
		Return the current progress.
		\return	the current progress in percent
	*/
	double GetProgress() {
		return Percent;
	}

	/**
		Sets the color of the progress bar overlay
		\param	color	the new color (-1 = default)
	*/
	inline void SetColor(int color = -1) {
		this->color = color;
		ResizeAll();
	}

	/**
		This method resized the progress bar to width and height. This method should only
		called if the new size is a valid size for this progress bar (See GetMinumumSize).
		\param	width	the new width of this progress bar
		\param	height	the new height of this progress bar
	*/
	virtual void Resize(Uint32 width, Uint32 height) {
		Widget::Resize(width,height);

        if(pForeground != NULL) {
            SDL_FreeSurface(pForeground);
            pForeground = NULL;
        }

        pForeground = GUIStyle::GetInstance().CreateProgressBarOverlay(GetSize().x, GetSize().y, Percent, color);
	}

	/**
		Draws this progress bar to screen
		\param	screen	Surface to draw on
		\param	Position	Position to draw the progress bar to
	*/
	virtual void Draw(SDL_Surface* screen, Point Position) {
		if(IsVisible() == false) {
			return;
		}

        SDL_Rect dest;
        dest.x = Position.x;
        dest.y = Position.y;

		if(pBackground != NULL) {
            dest.w = pBackground->w;
            dest.h = pBackground->h;
            SDL_BlitSurface(pBackground,NULL,screen,&dest);
		}

		if(pForeground != NULL) {
			dest.w = pForeground->w;
			dest.h = pForeground->h;
			SDL_BlitSurface(pForeground,NULL,screen,&dest);
		}
	}

protected:
	SDL_Surface*	pBackground;
	bool			bFreeBackground;
	SDL_Surface*	pForeground;

	double Percent;				///< Percent from 0.0 to 100.0
	int color;					///< The color of the progress overlay
};

class TextProgressBar : public ProgressBar {
public:
	TextProgressBar() : ProgressBar() {
		Text = "";
		bFreeBackground = true;
        textcolor = -1;
	    textshadowcolor = -1;
	}

	virtual ~TextProgressBar() { ; };

	/**
		This method sets a new text for this progress bar and resizes it
		to fit this text.
		\param	Text The new text for this progress bar
	*/
	virtual inline void SetText(std::string Text) {
		if(this->Text != Text) {
			this->Text = Text;
			ResizeAll();
		}
	}

	/**
		Get the text of this progress bar.
		\return the text of this button
	*/
	inline std::string GetText() { return Text; };

    /**
		Sets the text color for this progress bar.
		\param	textcolor	    the color of the text (-1 = default color)
        \param	textshadowcolor	the color of the shadow of the text (-1 = default color)
	*/
	virtual inline void SetTextColor(int textcolor, int textshadowcolor = -1) {
		this->textcolor = textcolor;
		this->textshadowcolor = textshadowcolor;
		Resize(GetSize().x, GetSize().y);
	}

	/**
		This method resized the progress bar to width and height. This method should only
		called if the new size is a valid size for this progress bar (See GetMinumumSize).
		\param	width	the new width of this progress bar
		\param	height	the new height of this progress bar
	*/
	virtual void Resize(Uint32 width, Uint32 height) {
		Widget::Resize(width,height);

		if(pBackground != NULL) {
			SDL_FreeSurface(pBackground);
			pBackground = NULL;
		}

		if(pForeground != NULL) {
			SDL_FreeSurface(pForeground);
			pForeground = NULL;
		}

		pBackground = GUIStyle::GetInstance().CreateButtonSurface(width, height, Text, true, false, textcolor, textshadowcolor);
		pForeground = GUIStyle::GetInstance().CreateProgressBarOverlay(width, height, Percent, color);

	}

	/**
		Returns the minimum size of this progress bar. The progress bar should not
		resized to a size smaller than this.
		\return the minimum size of this progress bar
	*/
	virtual Point GetMinimumSize() const {
		if(Text == "") {
			return Point(4,4);
		} else {
			return GUIStyle::GetInstance().GetMinimumButtonSize(Text);
		}
	}

protected:
	std::string Text;			///< Text of this progress bar

    int textcolor;
    int textshadowcolor;
};

class PictureProgressBar: public ProgressBar {
public:
	PictureProgressBar() : ProgressBar() {
		EnableResizing(false,false);
	}

	virtual ~PictureProgressBar() { ; }

	void SetSurface(SDL_Surface* pBackground, bool bFreeBackground) {
		if((this->bFreeBackground == true) && (this->pBackground != NULL)) {
			SDL_FreeSurface(this->pBackground);
		}

		this->pBackground = pBackground;
		this->bFreeBackground = bFreeBackground;

		if(this->pBackground != NULL) {
			Resize(this->pBackground->w,this->pBackground->h);
		} else {
			Resize(4,4);
		}

		ResizeAll();
	}

	/**
		This method resized the progress bar to width and height. This method should only
		called if the new size is a valid size for this progress bar (See GetMinumumSize).
		\param	width	the new width of this progress bar
		\param	height	the new height of this progress bar
	*/
	virtual void Resize(Uint32 width, Uint32 height) {
		Widget::Resize(width,height);
	}

	/**
		Returns the minimum size of this progress bar. The progress bar should not
		resized to a size smaller than this.
		\return the minimum size of this progress bar
	*/
	virtual Point GetMinimumSize() const {
		if(pBackground == NULL) {
			return Point(4,4);
		} else {
			return Point(pBackground->w,pBackground->h);
		}
	}
};

#endif // PROGRESSBAR_H
