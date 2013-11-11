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

#ifndef MVSBALANCEDIALOG_H
#define MVSBALANCEDIALOG_H


#include <techview/eikdialg.h>
#include "MVSAppUI.h"



class CMVSAppUi;

class CMVSBalanceDialog : public CEikDialog
	{
public:
    static void SetupDialogLD(TInt aBalance, CMVSAppUi* aAppUi, TBool aPlayBack);//constructs and launches dialog
    void PreLayoutDynInitL();//seeds dialog controls

private:
    CMVSBalanceDialog(TInt aBalance, CMVSAppUi* aAppUi, TBool aPlayBack = ETrue); 
    TBool OkToExitL(TInt aButtonId); //handles button presses for this dialog
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
private:
    TInt iBalance;
    TBool iPlayBack; 
    CEikProgressInfo* iProgInfo;//A pointer to a progress bar control
                                //used in this dialog
    CMVSAppUi* iAppUi;
    };

#endif MVSBALANCEDIALOG_H
