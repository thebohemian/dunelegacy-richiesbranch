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

#ifndef LABEL_H
#define LABEL_H

#include "Widget.h"
#include "GUIStyle.h"
#include <SDL.h>
#include <string>
#include <list>

/// a class for a text label
class Label : public Widget {
public:
	/// default constructor
	Label() : Widget() {
        fontID = FONT_STD12;
		textcolor = -1;
		textshadowcolor = -1;
		backgroundcolor = 0;
		alignment = (Alignment_Enum) (Alignment_Left | Alignment_Top);
		pSurface = NULL;
		EnableResizing(true,true);
	}

	/// destructor
	virtual ~Label() {
		if(pSurface != NULL) {
			SDL_FreeSurface(pSurface);
			pSurface = NULL;
		}
	}

    /**
		Sets a font for this label. Default font of a label is FONT_STD12
		\param	fontID	    the ID of the new font
	*/
	virtual inline void SetTextFont(int fontID) {
	    this->fontID = fontID;
	    Resize(GetSize().x, GetSize().y);
	}

    /**
		Gets the font of this label. Default font of a label is FONT_STD12
		\return the font ID of this label
	*/
	virtual inline int GetTextFont() const {
	   return fontID;
	}

	/**
		Sets the text color for this label.
		\param	textcolor	    the color of the text (-1 = default color)
        \param	textshadowcolor	the color of the shadow of the text (-1 = default color)
        \param  backgroundcolor the color of the label background (0 = transparent)
	*/
	virtual inline void SetTextColor(int textcolor, int textshadowcolor = -1, int backgroundcolor = 0) {
		this->textcolor = textcolor;
		this->textshadowcolor = textshadowcolor;
		this->backgroundcolor = backgroundcolor;
		Resize(GetSize().x, GetSize().y);
	}

	/**
		Sets the alignment of the text in this label.
		\param alignment Combination of (Alignment_HCenter, Alignment_Left or Alignment_Right) and (Alignment_VCenter, Alignment_Top or Alignment_Bottom)
	*/
	virtual inline void SetAlignment(Alignment_Enum alignment) {
		this->alignment = alignment;
		Resize(GetSize().x, GetSize().y);
	}

	/**
		Returns the alignment of the text in this label.
		\return Combination of (Alignment_HCenter, Alignment_Left or Alignment_Right) and (Alignment_VCenter, Alignment_Top or Alignment_Bottom)
	*/
	virtual inline Alignment_Enum GetAlignment() const {
		return alignment;
	}

	/**
		This method sets a new text for this label and resizes this label
		to fit this text.
		\param	Text The new text for this button
	*/
	virtual inline void SetText(std::string Text) {
		this->Text = Text;
		ResizeAll();
	}

	/**
		Get the text of this label.
		\return the text of this button
	*/
	inline std::string GetText() const { return Text; };

	/**
		This method resized the label to width and height. This method should only
		called if the new size is a valid size for this label (See GetMinumumSize).
		\param	width	the new width of this label
		\param	height	the new height of this label
	*/
	virtual void Resize(Uint32 width, Uint32 height) {
		if(pSurface != NULL) {
			SDL_FreeSurface(pSurface);
			pSurface = NULL;
		}

		//split text into single lines at every '\n'
		size_t startpos = 0;
		size_t nextpos;
		std::list<std::string> HardLines;
		do {
			nextpos = Text.find("\n",startpos);
			if(nextpos == std::string::npos) {
				HardLines.push_back(Text.substr(startpos,Text.length()-startpos));
			} else {
				HardLines.push_back(Text.substr(startpos,nextpos-startpos));
				startpos = nextpos+1;
			}
		} while(nextpos != std::string::npos);

		std::list<std::string> TextLines;

		std::list<std::string>::const_iterator iter;
		for(iter = HardLines.begin();iter != HardLines.end();++iter) {
			std::string tmpLine = *iter;

			if(tmpLine == "") {
                TextLines.push_back(" ");
                continue;
			}

			bool EndOfLine = false;
			size_t warppos = 0;
			size_t oldwarppos = 0;
			size_t lastwarp = 0;

			while(EndOfLine == false) {
				while(true) {
					warppos = tmpLine.find(" ", oldwarppos);
					std::string tmp;
					if(warppos == std::string::npos) {
						tmp = tmpLine.substr(lastwarp,tmpLine.length()-lastwarp);
						warppos = tmpLine.length();
						EndOfLine = true;
					} else {
						tmp = tmpLine.substr(lastwarp,warppos-lastwarp);
					}

					if( (int) GUIStyle::GetInstance().GetMinimumLabelSize(tmp, fontID).x - 4 > (int) width) {
						// this line would be too big => in oldwarppos is the last correct warp pos
						EndOfLine = false;
						break;
					} else {
					    if(EndOfLine == true) {
                            oldwarppos = warppos;
                            break;
					    } else {
                            oldwarppos = warppos + 1;
					    }
					}
				}

				if(oldwarppos == lastwarp) {
					// the width of this label is too small for the next character
					// create a dummy entry
					TextLines.push_back(" ");
					lastwarp++;
					oldwarppos++;
				} else {
					std::string tmpStr = tmpLine.substr(lastwarp,oldwarppos-lastwarp);
					TextLines.push_back(tmpStr);
					lastwarp = oldwarppos;
				}
			}
		}


		pSurface = GUIStyle::GetInstance().CreateLabelSurface(width,height,TextLines,fontID,alignment,textcolor,textshadowcolor,backgroundcolor);
		Widget::Resize(width,height);
	}

	/**
		Returns the minimum size of this label. The label should not
		resized to a size smaller than this.
		\return the minimum size of this label
	*/
	virtual Point GetMinimumSize() const {
		Point p(0,0);

		//split text into single lines at every '\n'
		size_t startpos = 0;
		size_t nextpos;
		std::list<std::string> HardLines;
		do {
			nextpos = Text.find("\n",startpos);
			if(nextpos == std::string::npos) {
				HardLines.push_back(Text.substr(startpos,Text.length()-startpos));
			} else {
				HardLines.push_back(Text.substr(startpos,nextpos-startpos));
				startpos = nextpos+1;
			}
		} while(nextpos != std::string::npos);

		std::list<std::string>::const_iterator iter;
		for(iter = HardLines.begin();iter != HardLines.end();++iter) {
			std::string tmp = *iter;
			p.x = std::max(p.x, GUIStyle::GetInstance().GetMinimumLabelSize(tmp, fontID).x);
			p.y += GUIStyle::GetInstance().GetMinimumLabelSize(tmp, fontID).y;
		}
		return p;
	}

	/**
		Draws this label to screen.
		\param	screen	Surface to draw on
		\param	Position	Position to draw the label to
	*/
	virtual void Draw(SDL_Surface* screen, Point Position) {
		if((IsEnabled() == false) || (IsVisible() == false) || (pSurface == NULL)) {
			return;
		}

		SDL_Rect dest;
		dest.x = Position.x + (GetSize().x - pSurface->w)/2;
		dest.y = Position.y + (GetSize().y - pSurface->h)/2;
		dest.w = pSurface->w;
		dest.h = pSurface->h;
		SDL_BlitSurface(pSurface, NULL, screen, &dest);
	};

	/**
		This static method creates a dynamic label object with Text as the label text.
		The idea behind this method is to simply create a new text label on the fly and
		add it to a container. If the container gets destroyed also this label will be freed.
		\param	Text	The label text
		\return	The new created label (will be automatically destroyed when it's parent widget is destroyed)
	*/
	static Label* Create(std::string Text) {
		Label* label = new Label();
		label->SetText(Text);
		label->pAllocated = true;
		return label;
	}

private:
    int fontID;                 ///< the ID of the font to use
	int textcolor;				///< the text color
	int textshadowcolor;        ///< the color of the shadow of the text
	int backgroundcolor;        ///< the color of the label background
	std::string Text;			///< the text of this label
	SDL_Surface* pSurface;		///< the surface of this label
	Alignment_Enum alignment;	///< the alignment of this label
};

#endif // LABEL_H
