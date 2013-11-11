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

#ifndef CMVSRESOURCENOTIFYDIALOG_H
#define CMVSRESOURCENOTIFYDIALOG_H

#include <techview/eikdialg.h>
#include <techview/eikchkbx.h>

class CMVSAppUi;
class CEikProgressInfo;

class CMVSResourceNotifyDialog : public CEikDialog
	{
public:
    static void SetupDialogLD(CMVSAppUi* aAppUi);   
    void PreLayoutDynInitL(); //     //seeds dialog controls
    void HandleControlEventL(CCoeControl *aControl, TCoeEvent aEventType);
                     
private:
    CMVSResourceNotifyDialog(CMVSAppUi* aAudioAppUi);
    TBool OkToExitL(TInt aButtonId); //handles button presses for this dialog
	CEikCheckBox* iRegisteredForNotificationCtl;

private:
    CMVSAppUi* iAppUi;
	TBool iRegisteredForNotification;     
	};

#endif CMVSRESOURCENOTIFYDIALOG_H


