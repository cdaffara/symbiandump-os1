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

#include "MVSVolumeDialog.h"
#include "MVSApp.hrh"
#include "MVSAppUI.h"


void CMVSVolumeDialog::SetupDialogLD(TInt aVolume, TTimeIntervalMicroSeconds aRamp,
                                  TTimeIntervalMicroSeconds aClipLength, TBool aPlayback, CMVSAppUi* aAppUi)
	{
    CMVSVolumeDialog* dialog = new (ELeave) CMVSVolumeDialog(aVolume, aRamp, aClipLength, aPlayback, aAppUi);
    dialog->ExecuteLD(R_MVS_DIALOG_SETVOLUME);
    }


void CMVSVolumeDialog::PreLayoutDynInitL()
    {
    _LIT(KTitle1,"SetVolume");
    _LIT(KTitle2,"SetGain");
     if(iPlayback)
     	{
     	SetTitleL(KTitle1);
     	}
	 else
	 	{
	 	SetTitleL(KTitle2);
	 	}
    // Get a downcasted pointer to the controls
    CCoeControl* myControlPtr = this->Control(EMVSCmdSetVolume);
    iProgInfo = static_cast<CEikProgressInfo*>(myControlPtr);  
    //Initial Volume
    iProgInfo->SetAndDraw(iVolume);
    //Initial VolumeRamp
    InitControl((TInt)EMVSCmdSetVolumeRamp, I64INT(iRamp.Int64())/1000, 0, I64INT(iClipLength.Int64())/1000);
    }


CMVSVolumeDialog::CMVSVolumeDialog(TInt aVolume, TTimeIntervalMicroSeconds aRamp,
                             TTimeIntervalMicroSeconds aClipLength, TBool aPlayback, CMVSAppUi* aAppUi)
: iVolume(aVolume), iRamp(aRamp), iClipLength(aClipLength), iPlayback(aPlayback), iAppUi(aAppUi), iOldVolume(aVolume)
	{
    }


TKeyResponse CMVSVolumeDialog::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
	{
	if(aKeyEvent.iCode == EKeyEscape && aType == EEventKeyDown)
		{
		OkToExitL(EMVSButtonCancel);
		}
	else if(aKeyEvent.iCode == EKeyRightArrow)
		{
		OkToExitL(EMVSButtonUp);
		}
	else if(aKeyEvent.iCode == EKeyLeftArrow)
		{
		OkToExitL(EMVSButtonDown);
		}
	
	return CEikDialog::OfferKeyEventL(aKeyEvent,aType);
	}


TBool CMVSVolumeDialog::OkToExitL(TInt aButtonId)
	{
    // Get a pointer to the progress bar control.
    // Downcast the returned CCoeControl* pointer to the correct type.
    CCoeControl* myControlPtr = this->Control(EMVSCmdSetVolume);
    iProgInfo = static_cast<CEikProgressInfo*>(myControlPtr);
    CEikNumberEditor* control = static_cast<CEikNumberEditor*>(Control(EMVSCmdSetVolumeRamp));
    switch(aButtonId)
    	{
    	case (EMVSButtonCancel):
    		iAppUi->SetVolumeL(iOldVolume, iRamp);
         	return ETrue;
         	
        case(EMVSButtonDown):
        	if(iProgInfo)
	        	{
	        	iProgInfo->IncrementAndDraw(-5);
	        	iVolume = iProgInfo->CurrentValue();
	          	iRamp = control->Number()*1000;
	        	iAppUi->SetVolumeL(iVolume, iRamp);
	        	}
        	break;

        case(EMVSButtonUp):
            if(iProgInfo)
	            {
	            iProgInfo->IncrementAndDraw(5);
	        	iVolume = iProgInfo->CurrentValue();
	          	iRamp = control->Number()*1000;
	            iAppUi->SetVolumeL(iVolume, iRamp);
	            }
           	break;

        case(EMVSButtonOk):
        default:
        	return ETrue;
        }
     
    return EFalse;
    }



void CMVSVolumeDialog::InitControl( const TInt aId, const TInt aRamp, const TInt aStart, const TInt aFinish )
	{
	CEikNumberEditor* control = static_cast<CEikNumberEditor*> ( Control(aId) );        
	if(control)
		{
		control->SetNumber(aRamp);
		control->SetMinimumAndMaximum( aStart, aFinish );
		}
	
	}
