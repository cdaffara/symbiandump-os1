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

#ifndef MVSREPEATDIALOG_H
#define MVSREPEATDIALOG_H

#include <techview/eikdialg.h>
#include "MVSAppUI.h"


class CMVSAppUi;

class CMVSRepeatDialog : public CEikDialog
	{
public:
    static void SetupDialogLD(TInt aNoRepeats, 
                              TTimeIntervalMicroSeconds aTrailingSilence,
                              CMVSAppUi* aAppUi);
    void PreLayoutDynInitL();    //seeds dialog controls

private:
    CMVSRepeatDialog(TInt aNoRepeats, 
                   TTimeIntervalMicroSeconds aTrailingSilence,
                   CMVSAppUi* aAppUi);
    TBool OkToExitL(TInt aButtonId); //handles button presses for this dialog

private:
    TInt iNoRepeats;    //The number of repeats
    TTimeIntervalMicroSeconds iTrailingSilence; //the trailing silence
	CMVSAppUi* iAppUi;
    };

#endif  MVSREPEATDIALOG_H
