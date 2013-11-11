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

#ifndef CMVSSYSTEMINFODIALOG_H
#define CMVSSYSTEMINFODIALOG_H

#include <techview/eikdialg.h>
#include "MVSAppUI.h"

class CMVSAppUi;

class CMVSSystemInfoDialog : public CEikDialog
    {
public:
	static void SetupDialogLD(CMVSAppUi* aAppUi); //constructs and launches dialog
    void PreLayoutDynInitL();    //seeds dialog controls
	~CMVSSystemInfoDialog();
private:
    CMVSSystemInfoDialog(CMVSAppUi* aAppUi); //standard C++ constructor
    TBool OkToExitL(TInt aButtonId); //handles button presses for this dialog
	void ConstructL();
private:
    CMVSAppUi* iAppUi;
    };
#endif CMVSSYSTEMINFODIALOG_H
