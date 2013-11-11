// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent - Internal Symbian test code   
*/

#include "tsysmon_step_register_twice_different.h"
#include "tsysmon_server_defs.h"

CRegisterTwiceDifferentStep::~CRegisterTwiceDifferentStep()
	{
	}

CRegisterTwiceDifferentStep::CRegisterTwiceDifferentStep()
	{
	SetTestStepName(KRegisterTwiceDifferentStep);
	}

TVerdict CRegisterTwiceDifferentStep::doTestStepL()
	{
	TInt expectedResult = 0;
	if (!GetIntFromConfig(ConfigSection(), KTSysMonServerExpectedResult, expectedResult))
		{
		ERR_PRINTF1(_L("Error reading ini file data"));
		User::Leave(KErrNotFound);
		}

	TBuf<255> args;
	args.Append(ConfigSection()); //The name of the config section is set to the ID of the test step.

	RProcess slave;
	
	User::LeaveIfError(slave.Create(KFilenameRegisterTwice2, args));
	slave.Resume();

	while (slave.ExitType() == EExitPending)
		{
			User::After(10000);
		}
	
	slave.Close();

	TInt actualResult = 0;
	CSysMonTestHelper::GetResultIntL(ConfigSection(), actualResult);
	INFO_PRINTF2(_L("Actual result: %d"), actualResult);
	TEST(actualResult == expectedResult);

	return TestStepResult();
	}
