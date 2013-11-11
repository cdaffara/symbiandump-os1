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


#include "MVSCropDialog.h"
#include "MVSApp.hrh"
#include <techview/eikmfne.h>
#include <coecntrl.h>
#include <techview/eikhopbt.h>
#include <techview/eiklabel.h>
#include "MVSAppUI.h"

const TUint KMicroSecs = 1000000;

void CMVSCropDialog::SetupDialogLD(TTimeIntervalMicroSeconds aPosition,
                                TTimeIntervalMicroSeconds aClipLength,
                                TTimeIntervalMicroSeconds aCropStart,
                                TTimeIntervalMicroSeconds aCropEnd,
                                CMVSAppUi* aAppUi)
	{
    CMVSCropDialog* dialog = new (ELeave) CMVSCropDialog(aPosition, aClipLength,
                                                   aCropStart, aCropEnd, aAppUi);
    dialog->ExecuteLD(R_MVS_DIALOG_CROP);
    }


void CMVSCropDialog::PreLayoutDynInitL()
	{
    TInt start = I64INT(iCropStart.Int64()) > 1? (I64INT(iCropStart.Int64())/KMicroSecs) : 0;
    TInt finish = I64INT(iCropEnd.Int64()) > 1? (I64INT(iCropEnd.Int64())/KMicroSecs) : I64INT(iClipLength.Int64())/KMicroSecs;
    //check the current position is valid.
	if(I64INT(iPosition.Int64())/KMicroSecs < start)
		iPosition = start;
	else if (I64INT(iPosition.Int64())/KMicroSecs > finish)
		iPosition = finish;

	InitControl(TInt(EMVSCmdCropPositionNumber), I64INT(iPosition.Int64())/KMicroSecs, start, finish );

    CEikHorOptionButtonList* button = static_cast<CEikHorOptionButtonList*>(Control(EMVSCmdCropLeftRight));
    button->SetButtonById(EMVSHorOptButtonCropFromBeginning);
	}



//
//CCropDialog()
//
// C++ Constructor, initialises the object member variables to their
// current settings. 
//
CMVSCropDialog::CMVSCropDialog(TTimeIntervalMicroSeconds aPosition,
                         TTimeIntervalMicroSeconds aClipLength,
                         TTimeIntervalMicroSeconds aCropStart,
                         TTimeIntervalMicroSeconds aCropEnd,
                         CMVSAppUi* aAppUi)
: iPosition(aPosition), iCropStart(aCropStart), iCropEnd(aCropEnd), iCropFromBeginning(EFalse), iAppUi(aAppUi)
    {
    if (I64INT(aCropEnd.Int64()) > 1)
    	{
        iClipLength = aCropEnd;
        }
    else
        {
        iClipLength = aClipLength;
        }
    }



TBool CMVSCropDialog::OkToExitL(TInt aButtonId)
	{
    //Get a pointer to the Option Button List control
    CEikHorOptionButtonList* button = static_cast<CEikHorOptionButtonList*>(Control(EMVSCmdCropLeftRight));
	//Get a pointer to the number editor
    CEikNumberEditor* control = static_cast<CEikNumberEditor*> ( Control(EMVSCmdCropPositionNumber) );
	TInt position = control->Number()*KMicroSecs;
    TInt64 pos64(position);
    iPosition = TTimeIntervalMicroSeconds(pos64);
    if(iPosition > iClipLength)
    	{
     	iPosition = iClipLength;
     	}
	if(iPosition < iCropStart)
	 	{
	 	iPosition = iCropStart; 
	 	}
    TInt buttonId = button->LabeledButtonId();
	switch (aButtonId)
    	{
        case(EMVSButtonUpdate):
        	//update prog info based on num-editor
        	if(buttonId == EMVSHorOptButtonCropFromBeginning) //'From Beginning' selected
            	{
               	iCropFromBeginning = ETrue;
               	iCropStart = iPosition;
               	}
          	else
            	{
               	iCropFromBeginning = EFalse;
               	iCropStart = iPosition;
               	}
            iAppUi->SetCropL(iCropStart, iCropFromBeginning);
            break;

        case(EMVSButtonCancel):
            break;

        default:
 		    break;
        }
    return ETrue;    
    }


void CMVSCropDialog::InitControl( const TInt aId, const TInt aPos, const TInt aStart, const TInt aFinish )
	{
	CEikNumberEditor* control = static_cast<CEikNumberEditor*> ( Control(aId) );        
	control->SetNumber(aPos);
	control->SetMinimumAndMaximum( aStart, aFinish );
	}


