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

#ifndef LOADSAVEWINDOW_H
#define LOADSAVEWINDOW_H

#include <GUI/Window.h>
#include <GUI/HBox.h>
#include <GUI/VBox.h>
#include <GUI/TextButton.h>
#include <GUI/TextBox.h>
#include <GUI/ListBox.h>
#include <GUI/Label.h>
#include <GUI/CallbackTarget.h>
#include <Definitions.h>

#include <string>

class LoadSaveWindow : public Window
{
public:
	LoadSaveWindow(bool bSave, std::string caption, std::string directory, std::string extension, int color = -1);
	virtual ~LoadSaveWindow();

	void UpdateEntries();
	std::string GetFilename() { return Filename; };

	bool SaveWindow() { return saveWindow; };

	std::string GetDirectory() { return directory; };

	std::string GetExtension() { return extension; };

	virtual bool HandleKeyPress(SDL_KeyboardEvent& key);

	void OnSaveNameCancel();
	void OnSaveNameConfirm();

	/**
		This static method creates a dynamic load/save window.
		The idea behind this method is to simply create a new dialog on the fly and
		add it as a child window of some other window. If the window gets closed it will be freed.
		\param	bSave		true = Save window, false = Load window
		\param	caption		the caption of the window
		\param	directory	the directory to save/load from
		\param	extension	the file extension
		\param  color       the color of the new dialog
		\return	The new dialog box (will be automatically destroyed when it's closed)
	*/
	static LoadSaveWindow* Create(bool bSave, std::string caption, std::string directory, std::string extension, int color = -1) {
		LoadSaveWindow* dlg = new LoadSaveWindow(bSave, caption, directory, extension, color);
		dlg->pAllocated = true;
		return dlg;
	}

private:
	void OnOK();
	void OnCancel();

	void OnSelectionChange();

	HBox	Main_HBox;
	VBox	Main_VBox;
	HBox	FileList_HBox;
	HBox	Button_HBox;

	Label		TitleText;
	ListBox		FileList;
	TextButton	Button_OK;
	TextButton	Button_Cancel;
	TextBox		SaveName;

	bool		saveWindow;
	std::string	Filename;
	std::string	directory;
	std::string extension;
};

#endif //LOADSAVEWINDOW_H
