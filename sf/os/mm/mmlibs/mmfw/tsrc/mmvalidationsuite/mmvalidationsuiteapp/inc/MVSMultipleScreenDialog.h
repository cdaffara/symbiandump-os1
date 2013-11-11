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

#ifndef MVSMULTIPLESCREENDIALOG_H
#define MVSMULTIPLESCREENDIALOG_H

#include <techview/eikdialg.h>
#include "MVSAppUI.h"


class CMVSAppUi;

class CMVSMultipleScreenDialog : public CEikDialog
	{
public:
    static void SetupDialogLD(TInt aScreenNumber,CMVSAppUi* aAppUi);
    void PreLayoutDynInitL();    //seeds dialog controls

private:
    CMVSMultipleScreenDialog(TInt aScreenNumber,CMVSAppUi* aAppUi);
    TBool OkToExitL(TInt aButtonId); //handles button presses for this dialog

private:
    TInt iScreenNumber;    //The Screen Number
	CMVSAppUi* iAppUi;
    };

#endif  MVSMULTIPLESCREENDIALOG_H
