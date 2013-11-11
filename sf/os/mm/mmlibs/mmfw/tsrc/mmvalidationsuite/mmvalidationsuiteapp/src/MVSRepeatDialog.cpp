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

#include "MVSRepeatDialog.h"
#include "MVSApp.hrh"


void CMVSRepeatDialog::SetupDialogLD(TInt aNoRepeats, 
                                  TTimeIntervalMicroSeconds aTrailingSilence,
                                  CMVSAppUi* aAppUi)
	{
    CMVSRepeatDialog* dialog = new (ELeave) CMVSRepeatDialog(aNoRepeats,
                                                       aTrailingSilence,
                                                       aAppUi);
    dialog->ExecuteLD(R_MVS_DIALOG_SETREPEATS);
    }


void CMVSRepeatDialog::PreLayoutDynInitL()
    {
    // Get a downcasted pointer to the controls
    CEikNumberEditor* myRepeatControl = static_cast<CEikNumberEditor*>(Control(EMVSCmdSetRepeats));
    CEikNumberEditor* myTrSilenceControl = static_cast<CEikNumberEditor*>(Control(EMVSCmdSetTrailingSilence));
	
    //Now seed the controls with appropriate values
    if(myRepeatControl)
    	{
    	myRepeatControl->SetNumber(iNoRepeats);	
    	}
    if(myTrSilenceControl)
    	{
    	myTrSilenceControl->SetNumber(I64INT(iTrailingSilence.Int64()/1000000));	
    	}
    }


CMVSRepeatDialog::CMVSRepeatDialog(TInt aNoRepeats, 
                             TTimeIntervalMicroSeconds aTrailingSilence,
                             CMVSAppUi* aAppUi)
: iNoRepeats(aNoRepeats), iAppUi(aAppUi)
    {
    iTrailingSilence = aTrailingSilence.Int64();
    }


TBool CMVSRepeatDialog::OkToExitL(TInt aButtonId)
    {
    if(aButtonId == EMVSButtonCancel)
    	{
        return ETrue;
        }

    //Update number of repeats from the control.
    iNoRepeats = static_cast<CEikNumberEditor*>
                        (Control(EMVSCmdSetRepeats))->Number();

    //Update the trailing silence from the control.
    TInt64 silence64 = (static_cast<CEikNumberEditor*>
                        (Control(EMVSCmdSetTrailingSilence))->Number());  
    TInt silence = I64INT(silence64*1000000);
    TTimeIntervalMicroSeconds theSilence(silence);
    iAppUi->SetRepeats(iNoRepeats, theSilence);
    return ETrue;
    }
