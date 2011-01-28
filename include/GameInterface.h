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

#ifndef GAMEINTERFACE_H
#define	GAMEINTERFACE_H

#include <GUI/Window.h>
#include <GUI/HBox.h>
#include <GUI/StaticContainer.h>
#include <GUI/Spacer.h>
#include <GUI/PictureButton.h>
#include <GUI/PictureLabel.h>
#include <GUI/dune/ChatManager.h>
#include <GUI/dune/NewsTicker.h>

class ObjectInterface;

/// This class represents the in-game interface.
class GameInterface : public Window {
public:
	/// default constructor
	GameInterface();

	/// destructor
	virtual ~GameInterface();

	/**
		Draws this window to screen. This method should be called every frame.
		\param	screen	Surface to draw on
		\param	Position	Position to draw the window to. The position of the window is added to this.
	*/
	virtual void Draw(SDL_Surface* screen, Point Position);

	/**
		This method adds a message to the news ticker
		\param	text	the message to add
	*/
	virtual void AddToNewsTicker(const std::string& text) {
		newsticker.AddMessage(text);
	}

	/**
		This method adds a urgent message to the news ticker
		\param	text	the urgent message to add
	*/
	virtual void AddUrgentMessageToNewsTicker(const std::string& text) {
		newsticker.AddUrgentMessage(text);
	}

	/**
        Returns the chat manager
        \return the chat manager
	*/
	ChatManager& getChatManager() { return chatManager; };


	/**
		This method updates the object interface
	*/
	virtual void UpdateObjectInterface();

private:
	void RemoveOldContainer();


	ObjectInterface*	pObjectContainer;
	Uint32				ObjectID;

	StaticContainer 	WindowWidget;

	HBox				TopBar_HBox;
	NewsTicker			newsticker;
	PictureButton		Button_Options;
	PictureButton		Button_Mentat;
	PictureLabel		TopBar;

	PictureLabel		GameBar;

	ChatManager         chatManager;
};
#endif // GAMEINTERFACE_H
