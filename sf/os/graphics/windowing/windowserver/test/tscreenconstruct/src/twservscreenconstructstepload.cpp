// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent
*/

#include "twservscreenconstructstepload.h"
#include <test/tefunit.h>
#include <e32const.h>	//KNullUidValue
#include <gdi.h>

/**
Constructor of CTWServScreenConstructStepLoad
*/
CTWServScreenConstructStepLoad::CTWServScreenConstructStepLoad()
	{
	SetTestStepName(KTWServScreenConstructStepLoad);
	}

/**
Destructor of CTWServScreenConstructStepLoad
*/
CTWServScreenConstructStepLoad::~CTWServScreenConstructStepLoad()
	{
	delete iGc;
	delete iScreen;
	iWinGroup.Close();
	iWsSession.Flush();
	iWsSession.Close();
	}

/**
Overrides test step preamble.
*/
enum TVerdict CTWServScreenConstructStepLoad::doTestStepPreambleL()
	{
	TVerdict ret = CTestStep::doTestStepPreambleL();

	User::LeaveIfError(iWsSession.Connect());

	iScreen = new (ELeave) CWsScreenDevice(iWsSession);
	User::LeaveIfError(iScreen->Construct());
	iWinGroup = RWindowGroup(iWsSession);
	User::LeaveIfError(iWinGroup.Construct(KNullWsHandle, iScreen));
	iWinGroup.AutoForeground(ETrue);
	iGc = new (ELeave) CWindowGc(iScreen);
	User::LeaveIfError(iGc->Construct());

	return ret;
	}

/**
Overrides test step prostamble.
*/
enum TVerdict CTWServScreenConstructStepLoad::doTestStepPostambleL()
	{
	return TestStepResult();
	}

/**
Starts test step
@return TVerdict pass / fail
*/
enum TVerdict CTWServScreenConstructStepLoad::doTestStepL()
	{
	return TestStepResult();
	}
