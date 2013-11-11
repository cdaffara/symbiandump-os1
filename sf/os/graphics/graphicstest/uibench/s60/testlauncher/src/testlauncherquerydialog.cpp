 // Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 //


#include "testlauncherquerydialog.h"

#include <avkon.hrh>


CHelloWorldQueryDialog::CHelloWorldQueryDialog(TDes& aBuf, HBufC *aDefInput) : 
        CAknTextQueryDialog(aBuf), iDefInput(*aDefInput)
    {
    // empty
    }

void  CHelloWorldQueryDialog::PreLayoutDynInitL()
    {
    // first we have to execute PreLayoutDynInitL() of the base-class
    CAknTextQueryDialog::PreLayoutDynInitL();

    // acquire pointer to editor-control and set the default input.
    CAknQueryControl* control = QueryControl();
    control->SetTextL(iDefInput);

    // enable OK-button, so that default text can be accepted as it is
    // without modifying the text
    MakeLeftSoftkeyVisible(ETrue);
    }
