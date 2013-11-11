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

#include "MVSBalanceDialog.h"
#include "MVSApp.hrh"

const TInt KBalanceBarUpdateFactor = 10;//Increment factor for the balance bar every increment
										//would increase/decrease the bar by a factor of 10.

void CMVSBalanceDialog::SetupDialogLD(TInt aBalance,
                                   CMVSAppUi* aAppUi, TBool aPlayBack)
	{
    CMVSBalanceDialog* dialog = new (ELeave) CMVSBalanceDialog(aBalance, aAppUi, aPlayBack);
    dialog->ExecuteLD(R_MVS_DIALOG_SETBALANCE);
    }


void CMVSBalanceDialog::PreLayoutDynInitL()
    {
    _LIT(KTitle1,"SetPlayBalance");
    _LIT(KTitle2,"SetRecordBalance");
    if(iPlayBack)
    	{
     	SetTitleL(KTitle1);
     	}
    else
    	{
     	SetTitleL(KTitle2);
     	}
    // Get a downcasted pointer to the controls
    CCoeControl* myControlPtr = this->Control(EMVSSetBalance);
    iProgInfo = static_cast<CEikProgressInfo*>(myControlPtr);
    if(iProgInfo)
	    {
	    iProgInfo->SetAndDraw((iBalance+100)/2);	
	    }  
    }


CMVSBalanceDialog::CMVSBalanceDialog(TInt aBalance, CMVSAppUi* aAppUi, TBool aPlayBack)
: iBalance(aBalance), iPlayBack(aPlayBack), iAppUi(aAppUi) 
    {
    //Nothing to do here - all done in initialisation list
    }


TKeyResponse CMVSBalanceDialog::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
	{
	if(aKeyEvent.iCode == EKeyEnter && aType == EEventKeyDown)
		{
		OkToExitL(EMVSButtonOk);
		}
	return CEikDialog::OfferKeyEventL(aKeyEvent,aType);
	}


TBool CMVSBalanceDialog::OkToExitL(TInt aButtonId)
    {
    // Get a pointer to the progress bar control.
    // Downcast the returned CCoeControl* pointer to the correct type.
    switch(aButtonId)
    	{
        case(EMVSButtonLeft):
        	iProgInfo->IncrementAndDraw(-(KMVSProgressLabelMaxValue/KBalanceBarUpdateFactor));
          	break;

        case(EMVSButtonRight):
            iProgInfo->IncrementAndDraw(KMVSProgressLabelMaxValue/KBalanceBarUpdateFactor);
          	break;

        case(EMVSButtonOk):
            //Get the value on the progress bar
            //to convert balance from a figure between 0 and 100
            //to a figure between -100 and 100
            iBalance = (iProgInfo->CurrentValue()*2)-100;
            iAppUi->SetBalanceL(iBalance);
            return ETrue;
		
        default: //cancel
        	return ETrue;
        }
    return EFalse;
    }
