// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tgsa_step_statetrantodiffstates.h"
#include "gsatestapps.h"

#include <ssm/ssmstates.hrh>
#include <ssm/ssmstate.h>

/**
Old Test CaseID 		AFSS-GSA-0030
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0032
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0033
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0034
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0035
 */


void CGsaStateTranToDiffStatesTest::DoStateRequestTransitionToDiffStatesL()
	{
	INFO_PRINTF1(_L("DoStateRequestTransitionToDiffStates test started...."));
	
	RFs fs;
	TInt err = fs.Connect();
	TEST(err == KErrNone);
	User::LeaveIfError(err);
	CleanupClosePushL(fs);
		
	if (iProcessName == KTestProcTranFromNormal)
		{
		RFileReadStream fileReadStream1;
		err = fileReadStream1.Open(fs, KGsaTestStateTranFromNormalResult, EFileRead);
		INFO_PRINTF2(_L("Error in opening file: %d"), err);
		TEST(err == KErrNone);
		User::LeaveIfError(err);
		CleanupClosePushL(fileReadStream1);

		//System state before starting the test.
		CurrentSystemStateForStateTranTestL(fileReadStream1, ESsmNormal);

		//Scenario 1 - Request for state change from Normal to invalid state results in KErrNotSupported
		INFO_PRINTF1(_L("Request for state change from Normal to invalid state"));
		TInt stateTranResult = fileReadStream1.ReadInt32L();
		TEST(stateTranResult == KErrNotSupported);
		INFO_PRINTF2(_L("Actual : %d Expected : -5"), stateTranResult);
		CurrentSystemStateForStateTranTestL(fileReadStream1, ESsmNormal);
		
		//Scenario 2 - Request for state change from Normal to fail state with invalid substate results in KErrNotSupported
		INFO_PRINTF1(_L("Request for state change from Normal to fail state with invalid substate"));
		stateTranResult = fileReadStream1.ReadInt32L();
		TEST(stateTranResult == KErrNotSupported);
		INFO_PRINTF2(_L("Actual : %d Expected : -5"), stateTranResult);
		CurrentSystemStateForStateTranTestL(fileReadStream1, ESsmNormal);

		//Scenario 3 - Request for two state changes(1. fail state 2. shutdown state) one after the other without waiting 
		// for the first one to complete. Cancels both transition request. 		
		INFO_PRINTF1(_L("Request for two state changes(1. fail state 2. shutdown state)"));
		INFO_PRINTF1(_L("one after the other without waiting for the first one to complete."));
		INFO_PRINTF1(_L("Cancel the fail state transition request."));
		stateTranResult = fileReadStream1.ReadInt32L();
		TEST(stateTranResult == KErrCancel);
		INFO_PRINTF2(_L("First request to the fail state will be completed with %d Expected -3"), stateTranResult);
		CurrentSystemStateForStateTranTestL(fileReadStream1, ESsmNormal);

		stateTranResult = fileReadStream1.ReadInt32L();
		TEST(stateTranResult == KErrCancel);
		INFO_PRINTF2(_L("Second request to transit to the shutdown state will be completed with %d Expected -3"), stateTranResult);
		CurrentSystemStateForStateTranTestL(fileReadStream1, ESsmNormal);

		INFO_PRINTF2(_L("Test process exits with %d Expected -3"), stateTranResult);

		CleanupStack::PopAndDestroy();
		// Delete the result file
		//err = fs.Delete(KGsaTestStateTranFromNormalResult);

		RFileReadStream fileReadStream2;
		err = fileReadStream2.Open(fs, KGsaTestStateTranToShutdownResult, EFileRead);
		TEST(err == KErrNone);
		User::LeaveIfError(err);
		CleanupClosePushL(fileReadStream2);

		//System state before starting the test.
		CurrentSystemStateForStateTranTestL(fileReadStream2, ESsmNormal);

		//Scenario 1 - Request for state transition to shutdown state - invalid substate
		// request to transit to the shutdown state will be completed with KErrNotSupported
		INFO_PRINTF1(_L("Request for state transition to shutdown state - invalid substate"));
		stateTranResult = fileReadStream2.ReadInt32L();
		TEST(stateTranResult == KErrNotSupported);
		INFO_PRINTF2(_L("Actual : %d Expected : -5"), stateTranResult);
		CurrentSystemStateForStateTranTestL(fileReadStream2, ESsmNormal);
		
		INFO_PRINTF2(_L("Test process exits with %d Expected -5"), stateTranResult);

		CleanupStack::PopAndDestroy();
		// Delete the result file
		//err = fs.Delete(KGsaTestStateTranToShutdownResult);

		RFileReadStream fileReadStream3;
		err = fileReadStream3.Open(fs, KGsaTestStateTranFromShutdownResult, EFileRead);
		TEST(err == KErrNone);
		User::LeaveIfError(err);
		CleanupClosePushL(fileReadStream3);

		//System state before starting the test.
		CurrentSystemStateForStateTranTestL(fileReadStream3, ESsmNormal);

		//Scenario 1 - Request for state change from Normal to shutdown state to critical substate
		// Cancel the request immediately
		INFO_PRINTF1(_L("Request for state change from Normal to shutdown critical substate and Cancel the request"));
		stateTranResult = fileReadStream3.ReadInt32L();
		TEST(stateTranResult == KErrCancel);
		INFO_PRINTF2(_L("Actual : %d Expected : -3"), stateTranResult);
		CurrentSystemStateForStateTranTestL(fileReadStream3, ESsmNormal);

		//Scenario 2 - Request for state change from Normal to shutdown state to any substate
		// Cancel the request immediately, so that we can proceed with the next test scenario.

		INFO_PRINTF1(_L("Request for state change from Normal to shutdown any substate and Cancel the request"));
		stateTranResult = fileReadStream3.ReadInt32L();
		TEST(stateTranResult == KErrCancel);
		INFO_PRINTF2(_L("Actual : %d Expected : -3"), stateTranResult);
		CurrentSystemStateForStateTranTestL(fileReadStream3, ESsmNormal);

		INFO_PRINTF2(_L("Test process exits with %d Expected -3"), stateTranResult);

		CleanupStack::PopAndDestroy();
		// Delete the result file
		//err = fs.Delete(KGsaTestStateTranFromShutdownResult);
		}
	else if(iProcessName == KTestProcTranFromStartup)
		{
		RFileReadStream fileReadStream;
		err = fileReadStream.Open(fs, KGsaTestStateTranFromStartupResult, EFileRead);
		TEST(err == KErrNone);
		User::LeaveIfError(err);
		CleanupClosePushL(fileReadStream);

		//System state before starting the test.
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmStartup);

		//Scenario 1 - Request for state change from Startup to invalid state
		INFO_PRINTF1(_L("Request for state change from Startup to invalid state"));
		TInt stateTranResult = fileReadStream.ReadInt32L();
		TEST(stateTranResult == KErrNotSupported);
		INFO_PRINTF2(_L("Actual : %d Expected : -5"), stateTranResult);
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmStartup);
		
		INFO_PRINTF2(_L("Test process exits with %d Expected -5"), stateTranResult);
		
		CleanupStack::PopAndDestroy();
		// Delete the result file
		//err = fs.Delete(KGsaTestStateTranFromStartupResult);
		}
	else
		{
		TEST(err == KErrArgument);
		return;
		}

	CleanupStack::PopAndDestroy();
	INFO_PRINTF1(_L("case completed\n"));
	}

// Tests the current system state
void CGsaStateTranToDiffStatesTest::CurrentSystemStateForStateTranTestL(RFileReadStream& aFileReadStream, TInt aSystemState)
	{
	TInt currentState = aFileReadStream.ReadUint16L();
	TSsmState currentSystemState(currentState, KSsmAnySubState);
	TSsmState expectedSystemState(aSystemState, KSsmAnySubState);

	TEST(currentSystemState == expectedSystemState);
	TPtrC currentSystemStateName(currentSystemState.Name());
	TPtrC expectedSystemStateName(expectedSystemState.Name());
	INFO_PRINTF3(_L("Current system state %S Expected %S"), &currentSystemStateName, &expectedSystemStateName);
	}

//---------------- CGsaStateTranToDiffStatesTest step ---------------------------------------


CGsaStateTranToDiffStatesTest::~CGsaStateTranToDiffStatesTest()
	{
	}

CGsaStateTranToDiffStatesTest::CGsaStateTranToDiffStatesTest(const TDesC& aProcessName) : iProcessName(aProcessName)
	{
	if (iProcessName == KTestProcTranFromNormal)
		SetTestStepName(KTGsaStateTranFromNormalStep);
	else if (iProcessName == KTestProcTranFromStartup)
		SetTestStepName(KTGsaStateTranFromStartupStep);
	else
		
		{
		// If the test script is none of the above then we would fail the test
		TEST(KErrArgument);
		}
	}

TVerdict CGsaStateTranToDiffStatesTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CGsaStateTranToDiffStatesTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

TVerdict CGsaStateTranToDiffStatesTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CGsaStateTranToDiffStatesTest started...."));
	
	__UHEAP_MARK;
	// Run the tests
	TRAPD(err, DoStateRequestTransitionToDiffStatesL());
	INFO_PRINTF2(_L("DoStateRequestTransitionToDiffStatesL completed with %d...."),err);
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("....CGsaStateTranToDiffStatesTest completed!!"));
	return TestStepResult();
	}





