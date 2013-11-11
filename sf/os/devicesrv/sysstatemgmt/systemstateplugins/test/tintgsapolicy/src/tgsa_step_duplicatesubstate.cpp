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
 @internalComponent - Internal Symbian test code
*/

#include "tgsa_step_duplicatesubstate.h"
#include <f32file.h>
#include <s32file.h>

/**
Old Test CaseID 		AFSS-GSA-0031
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0031
 */


void CGsaDuplicateSubstateTest::DoTestDuplicateSubstateL()
	{
	INFO_PRINTF1(_L("DoTestDuplicateSubstate test started...."));
	
	TestEpocWindResultsL(KTGsaPanicResultFile, 1);

	INFO_PRINTF1(_L("DoTestDuplicateSubstate case completed\n"));
	}

/**
This function compares the result contained within the epoc wind result
file, with the expected result passed in. This applies to test cases 
where Ssma panics causing the system to crash. In these test cases 
the epoc wind log is analysed for the correct behaviour and stored in
a result file, which this function checks.

@param aFileName
Name of file which contains epoc wind test results

@param aExpectedResult
The expected result for the test case, to be compared against.
*/
void CGsaDuplicateSubstateTest::TestEpocWindResultsL(const TDesC& aFileName, const TInt& aExpectedResult)
	{
	RFs myFs;
	myFs.Connect();
	CleanupClosePushL(myFs);
	
	RFileReadStream reader;
	reader.PushL();
	
	User::LeaveIfError(reader.Open(myFs,aFileName,EFileWrite));
	TChar delimChar('=');
	TBuf8<255> myDes;
	reader.ReadL(myDes, delimChar);
	
	TChar result(reader.ReadUint8L());
	TInt myActualResult = result.GetNumericValue();

	CleanupStack::PopAndDestroy(&reader);
	CleanupStack::PopAndDestroy(&myFs);
	
	INFO_PRINTF3(_L("Test call count - Expected: %d, Actual: %d"), aExpectedResult, myActualResult);
	TEST(aExpectedResult == myActualResult);
	}

//---------------- CGsaDuplicateSubstateTest step ---------------------------------------


CGsaDuplicateSubstateTest::~CGsaDuplicateSubstateTest()
	{
	}

CGsaDuplicateSubstateTest::CGsaDuplicateSubstateTest()
	{
	SetTestStepName(KTGsaDuplicateSubstateStep);
	}

TVerdict CGsaDuplicateSubstateTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CGsaDuplicateSubstateTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

TVerdict CGsaDuplicateSubstateTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CGsaDuplicateSubstateTest started...."));
	
	__UHEAP_MARK;
	// Run the tests
	TRAPD(err, DoTestDuplicateSubstateL());
	INFO_PRINTF2(_L("DoTestDuplicateSubstateL test completed with %d...."),err);
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("....CGsaDuplicateSubstateTest completed!!"));
	return TestStepResult();
	}





