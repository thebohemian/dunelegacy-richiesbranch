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

#ifndef DEFAULTUNITINTERFACE_H
#define DEFAULTUNITINTERFACE_H

#include "DefaultObjectInterface.h"

#include <globals.h>

#include <FileClasses/GFXManager.h>
#include <FileClasses/TextManager.h>

#include <GUI/TextButton.h>
#include <GUI/SymbolButton.h>
#include <GUI/HBox.h>
#include <GUI/VBox.h>

#include <units/UnitClass.h>
#include <units/MCVClass.h>
#include <units/DevastatorClass.h>

class UnitInterface : public DefaultObjectInterface {
public:
	static UnitInterface* Create(int ObjectID) {
		UnitInterface* tmp = new UnitInterface(ObjectID);
		tmp->pAllocated = true;
		return tmp;
	}

protected:
	UnitInterface(int ObjectID) : DefaultObjectInterface(ObjectID) {
        int color = houseColor[pLocalHouse->getHouseID()];

		Main_HBox.AddWidget(HSpacer::Create(5));

		Button_VBox.AddWidget(VSpacer::Create(6));

        Button_Move.SetSymbol(pGFXManager->getUIGraphic(UI_CursorMove), false);
		Button_Move.SetTooltipText(pTextManager->getLocalized("Move to a position"));
		Button_Move.SetToggleButton(true);
		Button_Move.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&UnitInterface::OnMove)));
		Action_HBox.AddWidget(&Button_Move);

		Action_HBox.AddWidget(HSpacer::Create(3));

		Button_Attack.SetSymbol(pGFXManager->getUIGraphic(UI_CursorAttack), false);
		Button_Attack.SetTooltipText(pTextManager->getLocalized("Attack a unit, structure or position"));
		Button_Attack.SetToggleButton(true);
		Button_Attack.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&UnitInterface::OnAttack)));
		Action_HBox.AddWidget(&Button_Attack);

        Action_HBox.AddWidget(HSpacer::Create(3));

        Button_Capture.SetSymbol(pGFXManager->getUIGraphic(UI_CursorCapture), false);
        Button_Capture.SetTooltipText(pTextManager->getLocalized("Capture a building"));
        Button_Capture.SetVisible((ItemID == Unit_Fremen) || (ItemID == Unit_Sardaukar) || (ItemID == Unit_Soldier) || (ItemID == Unit_Trooper));
		Button_Capture.SetToggleButton(true);
		Button_Capture.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&UnitInterface::OnCapture)));
		Action_HBox.AddWidget(&Button_Capture);

		Button_VBox.AddWidget(&Action_HBox, 28);

		Button_VBox.AddWidget(VSpacer::Create(6));


        Button_Guard.SetText(pTextManager->getLocalized("Guard"));
        Button_Guard.SetTextColor(color+3);
		Button_Guard.SetTooltipText(pTextManager->getLocalized("Unit will not move from location"));
		Button_Guard.SetToggleButton(true);
		Button_Guard.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&UnitInterface::OnGuard)));
		Button_VBox.AddWidget(&Button_Guard);

		Button_VBox.AddWidget(VSpacer::Create(6));

		Button_AreaGuard.SetText(pTextManager->getLocalized("Area Guard"));
        Button_AreaGuard.SetTextColor(color+3);
		Button_AreaGuard.SetTooltipText(pTextManager->getLocalized("Unit will engage any unit within guard range"));
		Button_AreaGuard.SetToggleButton(true);
		Button_AreaGuard.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&UnitInterface::OnAreaGuard)));
		Button_VBox.AddWidget(&Button_AreaGuard);

		Button_VBox.AddWidget(VSpacer::Create(6));

		Button_Stop.SetText(pTextManager->getLocalized("Stop"));
        Button_Stop.SetTextColor(color+3);
		Button_Stop.SetTooltipText(pTextManager->getLocalized("Unit will not move, nor attack"));
		Button_Stop.SetToggleButton(true);
		Button_Stop.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&UnitInterface::OnStop)));
		Button_VBox.AddWidget(&Button_Stop);

		Button_VBox.AddWidget(VSpacer::Create(6));

        Button_Ambush.SetText(pTextManager->getLocalized("Ambush"));
        Button_Ambush.SetTextColor(color+3);
		Button_Ambush.SetTooltipText(pTextManager->getLocalized("Unit will not move until enemy unit spoted"));
		Button_Ambush.SetToggleButton(true);
		Button_Ambush.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&UnitInterface::OnAmbush)));
		Button_VBox.AddWidget(&Button_Ambush);

		Button_VBox.AddWidget(VSpacer::Create(6));

        Button_Hunt.SetText(pTextManager->getLocalized("Hunt"));
        Button_Hunt.SetTextColor(color+3);
		Button_Hunt.SetTooltipText(pTextManager->getLocalized("Unit will immediately start to engage an enemy unit"));
		Button_Hunt.SetToggleButton(true);
		Button_Hunt.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&UnitInterface::OnHunt)));
		Button_VBox.AddWidget(&Button_Hunt);

		Button_VBox.AddWidget(VSpacer::Create(10));

		Button_Deploy.SetText(pTextManager->getLocalized("Deploy"));
        Button_Deploy.SetTextColor(color+3);
		Button_Deploy.SetTooltipText(pTextManager->getLocalized("Build a new construction yard"));
		Button_Deploy.SetVisible(ItemID == Unit_MCV ? true : false);
		Button_Deploy.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&UnitInterface::OnDeploy)));
		Button_VBox.AddWidget(&Button_Deploy);

		Button_VBox.AddWidget(VSpacer::Create(6));

		Button_Destruct.SetText(pTextManager->getLocalized("Destruct"));
        Button_Destruct.SetTextColor(color+3);
		Button_Destruct.SetTooltipText(pTextManager->getLocalized("Self-destruct this unit"));
		Button_Destruct.SetVisible(ItemID == Unit_Devastator ? true : false);
		Button_Destruct.SetOnClickMethod(MethodPointer(this,(WidgetCallbackWithNoParameter) (&UnitInterface::OnDestruct)));
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
		ObjectClass* pObject = currentGame->getObjectManager().getObject(ObjectID);
		MCVClass* pMCV = dynamic_cast<MCVClass*>(pObject);
		if(pMCV != NULL) {
			pMCV->HandleDeployClick();
		}
	}

	void OnDestruct() {
		ObjectClass* pObject = currentGame->getObjectManager().getObject(ObjectID);
		DevastatorClass* pDevastator = dynamic_cast<DevastatorClass*>(pObject);
		if(pDevastator != NULL) {
			pDevastator->HandleStartDevastateClick();
		}
	}

	void SetAttackMode(ATTACKMODE NewAttackMode) {
		ObjectClass* pObject = currentGame->getObjectManager().getObject(ObjectID);
		UnitClass* pUnit = dynamic_cast<UnitClass*>(pObject);

		if(pUnit != NULL) {
			pUnit->HandleSetAttackModeClick(NewAttackMode);
			pUnit->playConfirmSound();

			update();
		}
	}

	/**
		This method updates the object interface.
		If the object doesn't exists anymore then update returns false.
		\return true = everything ok, false = the object container should be removed
	*/
	virtual bool update() {
	    ObjectClass* pObject = currentGame->getObjectManager().getObject(ObjectID);
		if(pObject == NULL) {
			return false;
		}

        Button_Move.SetToggleState(currentGame->currentCursorMode == Game::CursorMode_Move);
		Button_Attack.SetToggleState(currentGame->currentCursorMode == Game::CursorMode_Attack);
		Button_Attack.SetVisible(pObject->canAttack());
		Button_Capture.SetToggleState(currentGame->currentCursorMode == Game::CursorMode_Capture);

		UnitClass* pUnit = dynamic_cast<UnitClass*>(pObject);
		if(pUnit != NULL) {
			ATTACKMODE AttackMode = pUnit->getAttackMode();

			Button_Guard.SetToggleState( AttackMode == GUARD );
            Button_AreaGuard.SetToggleState( AttackMode == AREAGUARD );
            Button_Stop.SetToggleState( AttackMode == STOP );
            Button_Ambush.SetToggleState( AttackMode == AMBUSH );
            Button_Hunt.SetToggleState( AttackMode == HUNT );
		}

		return true;
	}

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

#endif //DEFAULTUNITINTERFACE_H
