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

#include "tsysmon_step_result_check.h"
#include "tsysmon_server_defs.h"

CResultCheckStep::~CResultCheckStep()
	{
	}

CResultCheckStep::CResultCheckStep()
	{
	SetTestStepName(KResultCheckStep);
	}

TVerdict CResultCheckStep::doTestStepL()
	{
	TInt expectedResult = 0;
	TInt expectedRegisterCount = 0;
	TInt expectedRestartCount = 0;
	if (!GetIntFromConfig(ConfigSection(), KTSysMonServerExpectedResult, expectedResult) ||
		!GetIntFromConfig(ConfigSection(), KTSysMonServerExpectedRegisterCount, expectedRegisterCount) ||
		!GetIntFromConfig(ConfigSection(), KTSysMonServerExpectedRestartCount, expectedRestartCount))
		{
		ERR_PRINTF1(_L("Error reading ini file data"));
		User::Leave(KErrNotFound);
		}

	TInt actualResult = 0;
	TInt actualRegisterCount = 0;
	TInt actualRestartCount = 0;
	CSysMonTestHelper::GetResultIntL(ConfigSection(), actualResult);
	actualRegisterCount = CSysMonTestHelper::GetRegisterCountL();
	actualRestartCount = CSysMonTestHelper::GetRestartCountL();

	INFO_PRINTF2(_L("Actual result: %d"), actualResult);
	TEST(actualResult == expectedResult);
	INFO_PRINTF2(_L("Actual register count: %d"), actualRegisterCount);
	TEST(actualRegisterCount == expectedRegisterCount);
	INFO_PRINTF2(_L("Actual restart count: %d"), actualRestartCount);
	TEST(actualRestartCount == expectedRestartCount);

	return TestStepResult();
	}
