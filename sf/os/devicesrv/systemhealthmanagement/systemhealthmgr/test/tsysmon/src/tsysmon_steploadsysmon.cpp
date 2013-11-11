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

#include "tsysmon_steploadsysmon.h"

_LIT(KTestAppLoadSysMon, "testapp_loadsysmon.exe");

CStepLoadSysMon::CStepLoadSysMon()
	{
	SetTestStepName(KCTestCaseLoadSysMon);
	}
/**
Old Test CaseID 		APPFWK-SYSMON-0057
New Test CaseID 		DEVSRVS-SHMA-SYSMON-0057
*/


TVerdict CStepLoadSysMon::doTestStepL()
	{
	INFO_PRINTF1(_L("TEST APPFWK-SYSMON-0057"));

	INFO_PRINTF1(_L("Going to test app running"));
	RProcess process;
	CleanupClosePushL(process);
	User::LeaveIfError(process.Create(KTestAppLoadSysMon, KNullDesC));

	TRequestStatus status;
	process.Rendezvous(status);
	if (status == KRequestPending)
		{
		process.Resume();
		}
	else
		{
		process.Kill(KErrGeneral);
		TESTEL(EFalse, status.Int());
		}

	INFO_PRINTF1(_L("Process started"));	
	User::WaitForRequest(status);

	const TInt exitReason = process.ExitReason();
	const TExitType exitType = process.ExitType();
	TExitCategoryName categoryName = process.ExitCategory();

	TBuf<50> msg;
	if (exitType == EExitPanic)
		{
		_LIT(KAppPanicInfo, "Application panic: %S %d");
		msg.Format(KAppPanicInfo, &categoryName, exitReason);
		}
	else
		{
		_LIT(KAppExitInfo, "Application exited with code %d");
		msg.Format(KAppExitInfo, exitReason);
		}
	INFO_PRINTF1(msg);
	INFO_PRINTF2(_L("Process terminated with %d exit type"), exitType);

	TEST(exitReason == KErrNone);
	TEST(exitType == EExitKill);

	CleanupStack::PopAndDestroy(&process);
	
	INFO_PRINTF1(_L("Test Step completed"));
	return TestStepResult();	
	}




