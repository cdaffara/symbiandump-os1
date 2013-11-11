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

#include "MVSMultipleScreenDialog.h"
#include "MVSApp.hrh"


void CMVSMultipleScreenDialog::SetupDialogLD(TInt aScreenNumber,CMVSAppUi* aAppUi)
	{
    CMVSMultipleScreenDialog* dialog = new (ELeave) CMVSMultipleScreenDialog(aScreenNumber,
                                                       aAppUi);
    dialog->ExecuteLD(R_MVS_DIALOG_MULTIPLESCREEN);
    }


void CMVSMultipleScreenDialog::PreLayoutDynInitL()
    {
    // Get a downcasted pointer to the controls
    CEikNumberEditor* myScreenControl = static_cast<CEikNumberEditor*>(Control(EMVSCmdMultiScreen));
	
    //Now seed the controls with appropriate values
    if(myScreenControl)
    	{
    	myScreenControl->SetNumber(iScreenNumber);
    	myScreenControl->SetMinimumAndMaximum(0, ControlEnv()->WsSession().NumberOfScreens() - 1);
    	}
    }


CMVSMultipleScreenDialog::CMVSMultipleScreenDialog(TInt aScreenNumber,
                             CMVSAppUi* aAppUi)
: iScreenNumber(aScreenNumber),iAppUi(aAppUi)
    {
    }


TBool CMVSMultipleScreenDialog::OkToExitL(TInt aButtonId)
    {
    if(aButtonId == EMVSButtonCancel)
    	{
        return ETrue;
        }

    //set the desired screen for output
    iScreenNumber = static_cast<CEikNumberEditor*>(Control(EMVSCmdMultiScreen))->Number();        
    
   	iAppUi->SetOutputScreen(iScreenNumber);
   	return ETrue;
    }
