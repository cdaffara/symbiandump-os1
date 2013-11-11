// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tsysmon_step_run_count_check.h"
#include "tsysmon_server_defs.h"

CRunCountCheckStep::~CRunCountCheckStep()
	{
	}

CRunCountCheckStep::CRunCountCheckStep()
	{
	SetTestStepName(KRunCountCheckStep);
	}

TVerdict CRunCountCheckStep::doTestStepL()
	{
	TPtrC filename;
	TInt expectedRunCount = 0;
	TInt expectedRegisterCount = 0;
	TInt expectedRestartCount = 0;
	TInt processArguments = 0;
	if (!GetStringFromConfig(ConfigSection(), KTSysMonServerFilename, filename) ||
		!GetIntFromConfig(ConfigSection(), KTSysMonServerExpectedRunCount, expectedRunCount) ||
		!GetIntFromConfig(ConfigSection(), KTSysMonServerExpectedRegisterCount, expectedRegisterCount) ||
		!GetIntFromConfig(ConfigSection(), KTSysMonServerExpectedRestartCount, expectedRestartCount) ||
		!GetIntFromConfig(ConfigSection(), KTSysMonServerProcessArguments, processArguments))
		{
		ERR_PRINTF1(_L("Error reading ini file data"));
		User::Leave(KErrNotFound);
		}


	TInt actualRunCount = 0;
	TInt actualRegisterCount = 0;
	TInt actualRestartCount = 0;
	actualRunCount = CSysMonTestHelper::ReadRunCountL(filename);
	actualRegisterCount = CSysMonTestHelper::GetRegisterCount(processArguments);
	actualRestartCount = CSysMonTestHelper::GetRestartCountL();

	INFO_PRINTF2(_L("Actual run count: %d"), actualRunCount);
	TEST(actualRunCount == expectedRunCount);
	INFO_PRINTF2(_L("Actual register count: %d"), actualRegisterCount);
	TEST(actualRegisterCount == expectedRegisterCount);
	INFO_PRINTF2(_L("Actual restart count: %d"), actualRestartCount);
	TEST(actualRestartCount == expectedRestartCount);
	
	return TestStepResult();
	}
