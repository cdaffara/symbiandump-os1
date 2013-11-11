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

#include "tsysmon_step_register.h"
#include "tsysmon_server_defs.h"


CRegisterStep::~CRegisterStep()
	{
	}

CRegisterStep::CRegisterStep()
	{
	SetTestStepName(KRegisterStep);
	}

TVerdict CRegisterStep::doTestStepL()
	{
	
	TPtrC filename;
	TPtrC policy;
	TPtrC mode;
	TInt expectedResult = 0;
	TBool deregTested = EFalse;
	TInt deregExpectedResult = 0;
	
	if (!GetStringFromConfig(ConfigSection(), KTSysMonServerFilename, filename) ||
	        !GetStringFromConfig(ConfigSection(), KTSysMonServerPolicy, policy) ||
	        !GetStringFromConfig(ConfigSection(), KTSysMonServerMode, mode) ||
			!GetIntFromConfig(ConfigSection(), KTSysMonServerExpectedResult, expectedResult) ||
			!GetBoolFromConfig(ConfigSection(), KTSysMonServerTestDereg, deregTested) ||
			!GetIntFromConfig(ConfigSection(), KTSysMonServerDeregExpectedResult, deregExpectedResult)
			)
		{
		ERR_PRINTF1(_L("Error reading ini file data"));
		User::Leave(KErrNotFound);
		}

	TBuf<255> args;
	args.Append(ConfigSection()); //The name of the config section is set to the ID of the test step.
	args.Append(' ');
	args.Append(policy);
	args.Append(' ');
	args.Append(mode);

	RProcess slave;
	User::LeaveIfError(slave.Create(filename, args));
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
	
	// tsysmon_app_registercritical.exe tests monitors once for system critical and once for
	// system permanent
	_LIT(KFilenameCritical, "tsysmon_app_registercritical.exe");
	if (filename == KFilenameCritical)
		{
		if (!GetIntFromConfig(ConfigSection(), KTSysMonServerExpectedResult, expectedResult))
			{
			ERR_PRINTF1(_L("Error reading ini file data"));
			User::Leave(KErrNotFound);
			}
		else
			{
			actualResult = 0;
			CSysMonTestHelper::GetResultIntL(ConfigSection(), actualResult);
			INFO_PRINTF2(_L("Actual result for system permanent process: %d"), actualResult);
			TEST(actualResult == expectedResult);
			}
		}

	TInt deregActualResult = 0;
	if (deregTested)
		{
		CSysMonTestHelper::GetDeregResultIntL(ConfigSection(), actualResult);
		INFO_PRINTF2(_L("Actual dereg result: %d"), actualResult);
		TEST(deregActualResult == deregExpectedResult);
		}
	
	return TestStepResult();
	}
