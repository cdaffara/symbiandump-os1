// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32def.h>
#include "test.h"
#include "testregistry.h"

/**
 * The TTest constructor sets member data and adds a pointer to this test
 * to the RTestRegistry class.
 *
 * @param aName is a pointer to a string of the form 'TestOpen' or 'TestSetRequestor'
 * @param aTestScriptName is a pointer to the name of the cpp file defining the test
 * @param aTestSpecId points to a string containing a four letter ID (eg. TSVR).
 * @param aIntTestId is an additional identifier for the test (ie. TSVR-1, TSVR-2 etc.)
 */
TTest::TTest(const TText* aName, const TText* aTestScriptName, const TText* aTestSpecId, TInt aIntTestId)
 : iName(aName)
 , iTestSpecId(aTestSpecId)
 , iIntTestId(aIntTestId)
 , iResult(EPassed)
 , iLineNumOfTestFailure(0)
	{
	iTestScriptName = aTestScriptName;
	RTestRegistry::Instance().AddTest(*this);
	}

void TurnJITBackOn(TAny* /*aPtr*/)
	{
	User::SetJustInTime(ETrue);
	}

void TTest::DoPanicTestL(TThreadFunction aThreadFunction, TExitCategoryName aExpectedExitCat,
				  TInt aExpectedExitReason, TTimeIntervalMicroSeconds32 aTimeoutValue, 
				  const char* aFile, TInt aLineNo)

	{
	User::SetJustInTime(EFalse);
	CleanupStack::PushL(TCleanupItem(TurnJITBackOn));
	RThread panickingThread;
	User::LeaveIfError(panickingThread.Create(Name(),aThreadFunction,
		KDefaultStackSize,KTestHeapMinSize,KTestHeapMaxSize,NULL));
	CleanupClosePushL(panickingThread);

	TRequestStatus threadStatus(KRequestPending);
	panickingThread.Logon(threadStatus);

	panickingThread.Resume();

	RTimer timeoutTimer;
	User::LeaveIfError(timeoutTimer.CreateLocal());
	
	TRequestStatus timeoutStatus(KRequestPending);
	timeoutTimer.After(timeoutStatus, aTimeoutValue);

	User::WaitForRequest(threadStatus, timeoutStatus);

	if(threadStatus==KRequestPending)
		{
		TEST_FAILED(Thread_Didnt_Die)
		panickingThread.Kill(KErrGeneral);
		User::WaitForRequest(threadStatus);
		}
	else	//Thread is dead, but need to check it did indeed panic with the correct value.
		{
		CheckEqual(this, aExpectedExitCat, panickingThread.ExitCategory(), aFile, aLineNo);
		CheckEqual(this, aExpectedExitReason, panickingThread.ExitReason(), aFile, aLineNo);
		timeoutTimer.Cancel();
		User::WaitForRequest(timeoutStatus);
		}

	CleanupStack::PopAndDestroy(2);	//panickingThread and TurnJITBackOn
	}
