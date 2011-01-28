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

#include <GUI/dune/LoadSaveWindow.h>

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <FileClasses/TextManager.h>
#include <misc/FileSystem.h>

#include <GUI/Spacer.h>

LoadSaveWindow::LoadSaveWindow(bool bSave, std::string caption, std::string directory, std::string extension, int color)
 : Window(0,0,0,0) {
	saveWindow = bSave;
	this->directory = directory;
	this->extension = extension;

	// set up window
	SDL_Surface *surf;
	surf = pGFXManager->getUIGraphic(UI_LoadSaveWindow);

	SetBackground(surf,false);

	int xpos = std::max(0,(screen->w - surf->w)/2);
	int ypos = std::max(0,(screen->h - surf->h)/2);

	SetCurrentPosition(xpos,ypos,surf->w,surf->h);

	SetWindowWidget(&Main_HBox);

	Main_HBox.AddWidget(HSpacer::Create(16));
	Main_HBox.AddWidget(&Main_VBox);
	Main_HBox.AddWidget(HSpacer::Create(16));

	TitleText.SetTextColor(110, COLOR_TRANSPARENT);
	TitleText.SetAlignment((Alignment_Enum) (Alignment_HCenter | Alignment_VCenter));
	TitleText.SetText(caption);
	Main_VBox.AddWidget(&TitleText);

	Main_VBox.AddWidget(VSpacer::Create(8));

	Main_VBox.AddWidget(&FileList_HBox, bSave ? 100 : 130);
	FileList.SetColor(color);
	FileList.SetOnSelectionChangeMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&LoadSaveWindow::OnSelectionChange)));
	FileList.SetOnDoubleClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&LoadSaveWindow::OnOK)));
	FileList_HBox.AddWidget(&FileList);

	Main_VBox.AddWidget(VSpacer::Create(5));

	if(bSave == true) {
	    SaveName.SetTextColor(color);
		Main_VBox.AddWidget(&SaveName);
		Main_VBox.AddWidget(VSpacer::Create(5));
	}

	Main_VBox.AddWidget(&Button_HBox);

	Button_OK.SetText(pTextManager->getLocalized(bSave ? "Save" : "Load"));
	Button_OK.SetTextColor(color);
	Button_OK.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&LoadSaveWindow::OnOK)));

	Button_HBox.AddWidget(&Button_OK);

	Button_HBox.AddWidget(HSpacer::Create(8));

	Button_Cancel.SetText(pTextManager->getLocalized("Cancel"));
    Button_Cancel.SetTextColor(color);
	Button_Cancel.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&LoadSaveWindow::OnCancel)));

	Button_HBox.AddWidget(&Button_Cancel);

	Main_VBox.AddWidget(VSpacer::Create(10));

	UpdateEntries();
}

LoadSaveWindow::~LoadSaveWindow() {
	;
}

void LoadSaveWindow::UpdateEntries() {
	std::list<std::string> Files = GetFileNamesList(directory,extension,false, FileListOrder_ModifyDate_Dsc);

	std::list<std::string>::const_iterator iter;
	for(iter = Files.begin(); iter != Files.end(); ++iter) {
		std::string tmp = *iter;
		FileList.AddEntry(tmp.substr(0, tmp.length() - extension.length() - 1));
	}
}

bool LoadSaveWindow::HandleKeyPress(SDL_KeyboardEvent& key) {
	if(pChildWindow != NULL) {
		bool ret = pChildWindow->HandleKeyPress(key);
		return ret;
	}

	if(IsEnabled() && (pWindowWidget != NULL)) {
		if(key.keysym.sym == SDLK_RETURN) {
			OnOK();
			return true;
		} else {
			return pWindowWidget->HandleKeyPress(key);
		}
	} else {
		return false;
	}
}


void LoadSaveWindow::OnOK() {
	if(saveWindow == false) {
		int index = FileList.GetSelectedIndex();
		if(index >= 0) {
			Filename = directory + FileList.GetEntry(index) + "." + extension;

			Window* pParentWindow = dynamic_cast<Window*>(GetParent());
			if(pParentWindow != NULL) {
				pParentWindow->CloseChildWindow();
			}
		}
	} else {
		Filename = SaveName.GetText();

		if(SaveName.GetText() != "") {
			Filename = directory + SaveName.GetText() + "." + extension;

			Window* pParentWindow = dynamic_cast<Window*>(GetParent());
			if(pParentWindow != NULL) {
				pParentWindow->CloseChildWindow();
			}
		}
	}
}

void LoadSaveWindow::OnCancel() {
	Window* pParentWindow = dynamic_cast<Window*>(GetParent());
	if(pParentWindow != NULL) {
		pParentWindow->CloseChildWindow();
	}
}

void LoadSaveWindow::OnSelectionChange() {
	if(saveWindow == true) {
		int index = FileList.GetSelectedIndex();
		if(index >= 0) {
			SaveName.SetText(FileList.GetEntry(index));
		}
	}
}
