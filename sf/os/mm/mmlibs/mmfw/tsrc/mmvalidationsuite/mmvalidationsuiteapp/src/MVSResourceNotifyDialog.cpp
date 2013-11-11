// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Part of the MVS Application for TechView
//

#include "MVSResourceNotifyDialog.h"
#include "MVSApp.hrh"
#include "MVSAppUI.h"


void CMVSResourceNotifyDialog::SetupDialogLD(/*TBool aNotifyFlag,*/CMVSAppUi* aAppUi)
	{
    CMVSResourceNotifyDialog* dialog = new (ELeave) CMVSResourceNotifyDialog(aAppUi);
    dialog->ExecuteLD(R_MVS_DIALOG_REGISTER);
    //iRegisteredForNotification = aNotifyFlag;
    }


void CMVSResourceNotifyDialog::PreLayoutDynInitL()
    {
    CEikCheckBox* registerfornotification = static_cast<CEikCheckBox*>(Control(EMVSRegisterForNotification));
	if(registerfornotification)
    	{
    	iRegisteredForNotificationCtl = registerfornotification;
    	//iRegisteredForNotification = ETrue;
    	iRegisteredForNotificationCtl->SetObserver(this);
    	}
	}


CMVSResourceNotifyDialog::CMVSResourceNotifyDialog(CMVSAppUi* aAppUi)
	{
	iAppUi = aAppUi;
    }


TBool CMVSResourceNotifyDialog::OkToExitL(TInt aButtonId)
	{
	switch(aButtonId)
    	{//to be implemented once Video agents/controller is in place
        case(EMVSButtonCancel):
        	return ETrue;	
        	
    	}
        iAppUi->RegisterForNotification(iRegisteredForNotification);
        return ETrue;	
        }

void CMVSResourceNotifyDialog::HandleControlEventL(CCoeControl *aControl, TCoeEvent aEventType)
	{
	CEikDialog::HandleControlEventL(aControl,aEventType);
	if(iRegisteredForNotificationCtl == aControl && aEventType == EEventStateChanged)
		{
		if(iRegisteredForNotification == EFalse)
			{
			iRegisteredForNotification = ETrue;
			}
		else 
			{
			iRegisteredForNotification = EFalse;
			}
		}
	}

