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

#ifndef MULTIUNITINTERFACE_H
#define MULTIUNITINTERFACE_H

#include "ObjectInterface.h"

#include <globals.h>

#include <GUI/StaticContainer.h>
#include <GUI/HBox.h>
#include <GUI/VBox.h>

#include <FileClasses/GFXManager.h>
#include <FileClasses/TextManager.h>

#include <GUI/TextButton.h>
#include <GUI/SymbolButton.h>

#include <units/UnitClass.h>
#include <units/MCVClass.h>
#include <units/DevastatorClass.h>

class MultiUnitInterface : public ObjectInterface {
public:
	static MultiUnitInterface* Create() {
		MultiUnitInterface* tmp = new MultiUnitInterface();
		tmp->pAllocated = true;
		return tmp;
	}

protected:
	MultiUnitInterface() : ObjectInterface() {
	    int color = houseColor[pLocalHouse->getHouseID()];

        AddWidget(&TopBox,Point(0,0),Point(GAMEBARWIDTH - 25,100));

		AddWidget(&Main_HBox,Point(0,80),Point(GAMEBARWIDTH - 25,screen->h - 80 - 148));

		TopBox.AddWidget(&TopBox_HBox,Point(0,22),Point(GAMEBARWIDTH - 25,100));

		TopBox_HBox.AddWidget(Spacer::Create());
		TopBox_HBox.AddWidget(VSpacer::Create(56));
		TopBox_HBox.AddWidget(Spacer::Create());


		Main_HBox.AddWidget(HSpacer::Create(5));

		Button_VBox.AddWidget(VSpacer::Create(6));

		Button_Move.SetSymbol(pGFXManager->getUIGraphic(UI_CursorMove), false);
		Button_Move.SetTooltipText(pTextManager->getLocalized("Move to a position"));
		Button_Move.SetToggleButton(true);
		Button_Move.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&MultiUnitInterface::OnMove)));
		Action_HBox.AddWidget(&Button_Move);

		Action_HBox.AddWidget(HSpacer::Create(3));

        Button_Attack.SetSymbol(pGFXManager->getUIGraphic(UI_CursorAttack), false);
		Button_Attack.SetTooltipText(pTextManager->getLocalized("Attack a unit, structure or position"));
		Button_Attack.SetToggleButton(true);
		Button_Attack.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&MultiUnitInterface::OnAttack)));
		Action_HBox.AddWidget(&Button_Attack);

        Action_HBox.AddWidget(HSpacer::Create(3));

        Button_Capture.SetSymbol(pGFXManager->getUIGraphic(UI_CursorCapture), false);
        Button_Capture.SetTooltipText(pTextManager->getLocalized("Capture a building"));
		Button_Capture.SetToggleButton(true);
		Button_Capture.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&MultiUnitInterface::OnCapture)));
		Action_HBox.AddWidget(&Button_Capture);

		Button_VBox.AddWidget(&Action_HBox, 28);

		Button_VBox.AddWidget(VSpacer::Create(6));


        Button_Guard.SetText(pTextManager->getLocalized("Guard"));
        Button_Guard.SetTextColor(color+3);
		Button_Guard.SetTooltipText(pTextManager->getLocalized("Unit will not move from location"));
		Button_Guard.SetToggleButton(true);
		Button_Guard.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&MultiUnitInterface::OnGuard)));
		Button_VBox.AddWidget(&Button_Guard);

		Button_VBox.AddWidget(VSpacer::Create(6));

		Button_AreaGuard.SetText(pTextManager->getLocalized("Area Guard"));
        Button_AreaGuard.SetTextColor(color+3);
		Button_AreaGuard.SetTooltipText(pTextManager->getLocalized("Unit will engage any unit within guard range"));
		Button_AreaGuard.SetToggleButton(true);
		Button_AreaGuard.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&MultiUnitInterface::OnAreaGuard)));
		Button_VBox.AddWidget(&Button_AreaGuard);

		Button_VBox.AddWidget(VSpacer::Create(6));

		Button_Stop.SetText(pTextManager->getLocalized("Stop"));
        Button_Stop.SetTextColor(color+3);
		Button_Stop.SetTooltipText(pTextManager->getLocalized("Unit will not move, nor attack"));
		Button_Stop.SetToggleButton(true);
		Button_Stop.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&MultiUnitInterface::OnStop)));
		Button_VBox.AddWidget(&Button_Stop);

		Button_VBox.AddWidget(VSpacer::Create(6));

        Button_Ambush.SetText(pTextManager->getLocalized("Ambush"));
        Button_Ambush.SetTextColor(color+3);
		Button_Ambush.SetTooltipText(pTextManager->getLocalized("Unit will not move until enemy unit spoted"));
		Button_Ambush.SetToggleButton(true);
		Button_Ambush.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&MultiUnitInterface::OnAmbush)));
		Button_VBox.AddWidget(&Button_Ambush);

		Button_VBox.AddWidget(VSpacer::Create(6));

        Button_Hunt.SetText(pTextManager->getLocalized("Hunt"));
        Button_Hunt.SetTextColor(color+3);
		Button_Hunt.SetTooltipText(pTextManager->getLocalized("Unit will immediately start to engage an enemy unit"));
		Button_Hunt.SetToggleButton(true);
		Button_Hunt.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&MultiUnitInterface::OnHunt)));
		Button_VBox.AddWidget(&Button_Hunt);

		Button_VBox.AddWidget(VSpacer::Create(10));

		Button_Deploy.SetText(pTextManager->getLocalized("Deploy"));
        Button_Deploy.SetTextColor(color+3);
		Button_Deploy.SetTooltipText(pTextManager->getLocalized("Build a new construction yard"));
		Button_Deploy.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&MultiUnitInterface::OnDeploy)));
		Button_VBox.AddWidget(&Button_Deploy);

		Button_VBox.AddWidget(VSpacer::Create(6));

		Button_Destruct.SetText(pTextManager->getLocalized("Destruct"));
        Button_Destruct.SetTextColor(color+3);
		Button_Destruct.SetTooltipText(pTextManager->getLocalized("Self-destruct this unit"));
		Button_Destruct.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&MultiUnitInterface::OnDestruct)));
		Button_VBox.AddWidget(&Button_Destruct);

		Button_VBox.AddWidget(VSpacer::Create(30));

		Main_HBox.AddWidget(&Button_VBox);
		Main_HBox.AddWidget(HSpacer::Create(5));

		update();
	}

    void OnMove() {
        currentGame->currentCursorMode = Game::CursorMode_Move;
	}

	void OnAttack() {
        currentGame->currentCursorMode = Game::CursorMode_Attack;
	}

    void OnCapture() {
        currentGame->currentCursorMode = Game::CursorMode_Capture;
	}

    void OnGuard() {
		SetAttackMode(GUARD);
	}

	void OnAreaGuard() {
		SetAttackMode(AREAGUARD);
	}

    void OnStop() {
		SetAttackMode(STOP);
	}

	void OnAmbush() {
		SetAttackMode(AMBUSH);
	}

    void OnHunt() {
		SetAttackMode(HUNT);
	}

	void OnDeploy() {
        std::set<Uint32>::const_iterator iter;
		for(iter = currentGame->getSelectedList().begin(); iter != currentGame->getSelectedList().end(); ++iter) {
            ObjectClass* pObject = currentGame->getObjectManager().getObject(*iter);
            MCVClass* pMCV = dynamic_cast<MCVClass*>(pObject);
            if(pMCV != NULL) {
                pMCV->HandleDeployClick();
            }
		}
	}

	void OnDestruct() {
        std::set<Uint32>::const_iterator iter;
		for(iter = currentGame->getSelectedList().begin(); iter != currentGame->getSelectedList().end(); ++iter) {
            ObjectClass* pObject = currentGame->getObjectManager().getObject(*iter);
            DevastatorClass* pDevastator = dynamic_cast<DevastatorClass*>(pObject);
            if(pDevastator != NULL) {
                pDevastator->HandleStartDevastateClick();
            }
		}
	}

	void SetAttackMode(ATTACKMODE NewAttackMode) {

	    UnitClass* pLastUnit = NULL;
        std::set<Uint32>::const_iterator iter;
		for(iter = currentGame->getSelectedList().begin(); iter != currentGame->getSelectedList().end(); ++iter) {
            ObjectClass* pObject = currentGame->getObjectManager().getObject(*iter);
            UnitClass* pUnit = dynamic_cast<UnitClass*>(pObject);
            if(pUnit != NULL) {
                pLastUnit = pUnit;
                pUnit->HandleSetAttackModeClick(NewAttackMode);
            }
		}

		if(pLastUnit != NULL) {
		    pLastUnit->playConfirmSound();
		}

		update();
	}

	/**
		This method updates the object interface.
		If the object doesn't exists anymore then update returns false.
		\return true = everything ok, false = the object container should be removed
	*/
	virtual bool update() {
	    if(currentGame->getSelectedList().empty() == true) {
			return false;
		}

		Button_Move.SetToggleState(currentGame->currentCursorMode == Game::CursorMode_Move);
		Button_Attack.SetToggleState(currentGame->currentCursorMode == Game::CursorMode_Attack);
		Button_Capture.SetToggleState(currentGame->currentCursorMode == Game::CursorMode_Capture);

		bool bGuard = true;
		bool bAreaGuard = true;
		bool bStop = true;
		bool bAmbush = true;
		bool bHunt = true;

        bool bShowAttack = false;
        bool bShowCapture = false;
		bool bShowDeploy = false;
		bool bShowDevastate = false;

		std::set<Uint32>::const_iterator iter;
		for(iter = currentGame->getSelectedList().begin(); iter != currentGame->getSelectedList().end(); ++iter) {
            ObjectClass* pObject = currentGame->getObjectManager().getObject(*iter);
            UnitClass* pUnit = dynamic_cast<UnitClass*>(pObject);
            if(pUnit != NULL) {
                ATTACKMODE attackMode = pUnit->getAttackMode();
                bGuard = bGuard && (attackMode == GUARD);
                bAreaGuard = bAreaGuard && (attackMode == AREAGUARD);
                bStop = bStop && (attackMode == STOP);
                bAmbush = bAmbush && (attackMode == AMBUSH);
                bHunt = bHunt && (attackMode == HUNT);

                if(pUnit->canAttack()) {
                    bShowAttack = true;
                }

                switch(pUnit->getItemID()) {
                    case Unit_Fremen:
                    case Unit_Sardaukar:
                    case Unit_Soldier:
                    case Unit_Trooper: {
                        bShowCapture = true;
                    } break;

                    case Unit_MCV: {
                        bShowDeploy = true;
                    } break;

                    case Unit_Devastator: {
                        bShowDevastate = true;
                    } break;

                    default: {
                    } break;
                }

            }
		}

        Button_Attack.SetVisible(bShowAttack);
		Button_Capture.SetVisible(bShowCapture);

        Button_Guard.SetToggleState( bGuard );
        Button_AreaGuard.SetToggleState( bAreaGuard );
        Button_Stop.SetToggleState( bStop );
        Button_Ambush.SetToggleState( bAmbush );
        Button_Hunt.SetToggleState( bHunt );

        Button_Deploy.SetVisible(bShowDeploy);
        Button_Destruct.SetVisible(bShowDevastate);

		return true;
	}

	StaticContainer	TopBox;
	HBox			TopBox_HBox;
	HBox			Main_HBox;

	HBox		    Button_HBox;
	VBox		    Button_VBox;
	HBox            Action_HBox;

	SymbolButton    Button_Move;
	SymbolButton    Button_Attack;
    SymbolButton    Button_Capture;

    TextButton	    Button_Guard;
	TextButton	    Button_AreaGuard;
	TextButton      Button_Stop;
	TextButton	    Button_Ambush;
	TextButton      Button_Hunt;
	TextButton	    Button_Deploy;
	TextButton	    Button_Destruct;
};

#endif //MULTIUNITINTERFACE_H
