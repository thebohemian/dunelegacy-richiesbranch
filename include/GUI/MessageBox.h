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

#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include "Window.h"
#include "Button.h"
#include "Label.h"
#include "Spacer.h"
#include "GUIStyle.h"
#include "Widget.h"
#include "VBox.h"
#include "HBox.h"

#include <SDL.h>

#include <iostream>

extern SDL_Surface* screen;

/// A simple class for a message box
class MessageBox : public Window {
public:

	/**
		This method sets a new text for this message box.
		\param	Text The new text for this message box
	*/
	virtual inline void SetText(std::string Text) {
		textlabel.SetText(Text);
		Resize(std::max(vbox.GetMinimumSize().x,120),vbox.GetMinimumSize().y);
	}

	/**
		Get the text of this message box.
		\return the text of this message box
	*/
	inline std::string GetText() { return textlabel.GetText(); };

    /**
		Sets the text color for this message box.
		\param	textcolor	    the color of the text (-1 = default color)
        \param	textshadowcolor	the color of the shadow of the text (-1 = default color)
	*/
	virtual inline void SetTextColor(int textcolor, int textshadowcolor = -1) {
		textlabel.SetTextColor(textcolor, textshadowcolor);
		okbutton.SetTextColor(textcolor, textshadowcolor);
	}

	/**
		This method resized the message box to width and height. This method should only be
		called if the new size is a valid size for this message box (See ResizingXAllowed,
		ResizingYAllowed, GetMinumumSize).
		\param	width	the new width of this message box
		\param	height	the new height of this message box
	*/
	virtual void Resize(Uint32 width, Uint32 height) {
		Window::Resize(width,height);
		Position.x = (screen->w - GetSize().x)/2;
		Position.y = (screen->h - GetSize().y)/2;
	}

	/**
		This method is called by the window widget if it requests a resizing of
		this window.
	*/
	virtual void ResizeAll() {
		// MessageBox should get bigger if content changes
		if(pWindowWidget != NULL) {
			Point newSize = pWindowWidget->GetMinimumSize();
			newSize.x = std::max(newSize.x,120);
			newSize.y = std::max(newSize.y,30);
			Resize(newSize.x,newSize.y);
		}
	};

	/**
		This static method creates a dynamic message box object with Text as the text in the message box.
		The idea behind this method is to simply create a new message box on the fly and
		add it as a child window of some other window. If the window gets closed it will be freed.
		\param	Text	The message box text
		\return	The new message box (will be automatically destroyed when it's closed)
	*/
	static MessageBox* Create(std::string Text) {
		MessageBox* msgbox = new MessageBox(Text);
		msgbox->pAllocated = true;
		return msgbox;
	}

protected:
	/** protected constructor (See Create)
		\param	Text	Text of this message box
	*/
	MessageBox(std::string Text)
	 : Window(50,50,50,50) {
		init(Text);
	}

	/// destructor
	virtual ~MessageBox() {
    }

private:
	/**
		Initialization helper method.
		\param	Text	Text of this message box
	*/
	void init(std::string Text) {
		SetWindowWidget(&vbox);
		vbox.AddWidget(VSpacer::Create(6));
		vbox.AddWidget(&textlabel);
		vbox.AddWidget(VSpacer::Create(3));
		vbox.AddWidget(&hbox);
		vbox.AddWidget(VSpacer::Create(6));
		hbox.AddWidget(Spacer::Create());
		hbox.AddWidget(&vbox2);
		vbox2.AddWidget(VSpacer::Create(4));
		okbutton.SetText("OK");
		okbutton.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&MessageBox::OnOK)));
		vbox2.AddWidget(&okbutton);
		vbox2.AddWidget(VSpacer::Create(4));
		hbox.AddWidget(Spacer::Create());
		SetText(Text);
		textlabel.SetAlignment(Alignment_HCenter);
	}

	/**
		This method is called when the OK button is pressed.
	*/
	virtual void OnOK() {
		Window* pParentWindow = dynamic_cast<Window*>(GetParent());
		if(pParentWindow != NULL) {
			pParentWindow->CloseChildWindow();
		}
	}

	VBox vbox;					///< vertical box
	HBox hbox;					///< horizontal box
	VBox vbox2;					///< inner vertical box;
	Label textlabel;			///< label that contains the text
	TextButton okbutton;		///< the ok button
};

#endif // MESSAGEBOX_H
