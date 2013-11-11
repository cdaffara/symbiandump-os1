// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TEST_STEP_CTRL_SET_REPEAT_H
#define __TEST_STEP_CTRL_SET_REPEAT_H

#include <e32base.h>
#include <e32std.h>
#include <e32base.h>

#include <mmf/server/mmffile.h>
#include <mmf/common/mmfstandardcustomcommands.h> // For RMMFAudioPlayControllerSetRepeatsCustomCommands

#include <testframework.h>

const TInt KNameBufSize		= 25;
const TUid KMmfAudioControllerUid  = { 0x101F5022 };
const TInt KDefaultPriority = 11;

class CAsyncTestStepCtrlSetRepeatsCustomCommands : public RAsyncTestStep,
												   public MMMFControllerEventMonitorObserver
	{
public:
	static	CAsyncTestStepCtrlSetRepeatsCustomCommands * NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
	// from RAsyncTestStep
	void KickoffTestL();
	void CloseTest();
private:
	CAsyncTestStepCtrlSetRepeatsCustomCommands( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
protected:
	//from MMMFControllerEventMonitorObserver
	void HandleEvent(const TMMFEvent& aEvent);
private:

	CMMFControllerEventMonitor* iControllerEventMonitor;
	RMMFController iController;
	TMMFPrioritySettings iPriority;
	
	TMMFFileConfig	iFileConfig;

	TBuf<KNameBufSize> iSectName;					// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;					// Key name for retrieving filename
	
	};

#endif //__TEST_STEP_CTRL_SET_REPEAT_H
