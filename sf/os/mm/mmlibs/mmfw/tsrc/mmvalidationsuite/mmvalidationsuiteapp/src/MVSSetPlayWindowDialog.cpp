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


#include "MVSSetPlayWindowDialog.h"
#include "MVSApp.hrh"
#include "MVSAppUI.h"


void CMVSSetPlayWindowDialog::SetupDialogLD(TTimeIntervalMicroSeconds aStart,
                                  TTimeIntervalMicroSeconds aEnd,
                                  TTimeIntervalMicroSeconds aMaxDuration, CMVSAppUi* aAppUi)
	{
    CMVSSetPlayWindowDialog* dialog = new (ELeave) CMVSSetPlayWindowDialog(aStart,aEnd,
                                                       aMaxDuration, aAppUi);
    dialog->ExecuteLD(R_MVS_DIALOG_SETPLAYWINDOW);
    }

void CMVSSetPlayWindowDialog::PreLayoutDynInitL()
    {
    // Get a downcasted pointer to the controls
    CEikNumberEditor* control = static_cast<CEikNumberEditor*> ( Control((TInt)EMVSCmdSetPlayWindowStart) );        
	control->SetMinimumAndMaximum(0, (iMaxDuration.Int64()));
	control->SetNumber(iStart.Int64());
    control = static_cast<CEikNumberEditor*> ( Control((TInt)EMVSCmdSetPlayWindowEnd) );
    control->SetMinimumAndMaximum(0, iMaxDuration.Int64());
    control->SetNumber(iEnd.Int64());
    }


CMVSSetPlayWindowDialog::CMVSSetPlayWindowDialog(TTimeIntervalMicroSeconds aStart,
                             TTimeIntervalMicroSeconds aEnd,TTimeIntervalMicroSeconds aMaxDuration, CMVSAppUi* aAppUi)
: iStart(aStart), iEnd(aEnd), iMaxDuration(aMaxDuration), iAppUi(aAppUi)
    {
    
    }


TKeyResponse CMVSSetPlayWindowDialog::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
	{
	if(aKeyEvent.iCode == EKeyEnter && aType == EEventKeyDown)
		{
		OkToExitL(EMVSButtonUpdate);
		}
	return CEikDialog::OfferKeyEventL(aKeyEvent,aType);
	}
	

TBool CMVSSetPlayWindowDialog::OkToExitL(TInt aButtonId)
    {
    // Get a pointer to the progress bar control.
    // Downcast the returned CCoeControl* pointer to the correct type.
    CEikNumberEditor* controlstart = static_cast<CEikNumberEditor*>(Control(EMVSCmdSetPlayWindowStart));
	CEikNumberEditor* controlend = static_cast<CEikNumberEditor*>(Control(EMVSCmdSetPlayWindowEnd));
	TBool result = ETrue;
    switch(aButtonId)
    	{
        case(EMVSButtonUpdate):
        	{
        	TInt positionstart = controlstart->Number();
    		TInt64 posstart64(positionstart);
    		TInt positionend = controlend->Number();
    		TInt64 posend64(positionend);
            //Port these values over to the AppUi
            result = iAppUi->SetPlayWindow(TTimeIntervalMicroSeconds(posstart64), TTimeIntervalMicroSeconds(posend64));
            //if the play window can not be set with the values, restore to previous values
            if(!result)
            	{
            	controlstart->SetNumber(iStart.Int64());
            	controlstart->DrawNow();
            	controlend->SetNumber(iEnd.Int64());
            	controlend->DrawNow();
            	}
        	}
        	break;

        default:
        	break;
        }
     
    return result;
    }

