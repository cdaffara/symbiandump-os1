// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <s32file.h>
#include "appfwk_sysstart_test_step_AppBase.h"

/**
  This function checks that a specified result file does not exist. This
  scenario occurs when SysStart is expected to panic prior to the creation
  of result files. e.g. dll function 2 called, fails, panics, thereby
  preventing dll function 3 from being called, and function 3 result file
  from being created.

  @param aFileName
  Name of file which is expected to be missing.
  */
void CAppfwkSysStartTestStepAppBase::TestResultFileDoesNotExistL(const TDesC& aFileName)
	{
	RFs myFs;
	myFs.Connect();
	CleanupClosePushL(myFs);
	
	RFile myFile;
	TInt actualErr = myFile.Open(myFs,aFileName,EFileWrite);
	CleanupStack::PopAndDestroy(&myFs);
	
	INFO_PRINTF3(_L("Test file should be missing - Expected: %d, Actual: %d"), KErrNotFound, actualErr);
	TEST(actualErr==KErrNotFound);
	}
	
/**
  This function compares the result contained within the epoc wind result
  file, with the expected result passed in. This applies to test cases 
  where SysStart panics causing the system to crash. In these test cases 
  the epoc wind log is analysed for the correct behaviour and stored in
  a result file, which this function checks.

  @param aFileName
  Name of file which contains epoc wind test results
  
  @param aExpectedResult
  The expected result for the test case, to be compared against.
  */
void CAppfwkSysStartTestStepAppBase::TestEpocWindResultsL(const TDesC& aFileName, const TInt& aExpectedResult)
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
	TEST(aExpectedResult==myActualResult);
	}

/**
  This function compares the expected number of times the test app has been started with the
  actual number of times the test app has been started i.e. effectively compares the two values
  passed in as arguments.
  
  @param aExpectedStartCount - the expected number of times the test app has been started
  @param aActualStartCount   - the actual number of times the test app has been started
  */
void CAppfwkSysStartTestStepAppBase::TestAppStartCount(const TInt& aExpectedStartCount, const TInt& aActualStartCount)
	{
	INFO_PRINTF3(_L("Test App Start Count - Expected: %d, Actual: %d"), aExpectedStartCount, aActualStartCount);
	TEST(aExpectedStartCount==aActualStartCount);
	}

/**
  This function compares the expected number of times the test app has rendezvous'd with the
  actual number of times the test app has rendezvous'd i.e. effectively compares the two values
  passed in as arguments.
  
  @param aExpectedRendCount - the expected number of times the test app has been started
  @param aActualRendCount   - the actual number of times the test app has been started
  */
void CAppfwkSysStartTestStepAppBase::TestAppRendCount(const TInt& aExpectedRendCount, const TInt& aActualRendCount)
	{
	INFO_PRINTF3(_L("Test App Rendezvous Count - Expected: %d, Actual: %d"), aExpectedRendCount, aActualRendCount);
	TEST(aExpectedRendCount==aActualRendCount);
	}

/**
  This function compares the expected number of times the test app has failed to rendezvous with the
  actual number of times the test app has failed to rendezvous i.e. effectively compares the two values
  passed in as arguments.
  
  @param aExpectedRendFailCount - the expected number of times the test app has been started
  @param aActualRendFailCount   - the actual number of times the test app has been started
  */
void CAppfwkSysStartTestStepAppBase::TestAppRendFailCount(const TInt& aExpectedRendFailCount, const TInt& aActualRendFailCount)
	{
	INFO_PRINTF3(_L("Test App Rendezvous Fail Count - Expected: %d, Actual: %d"), aExpectedRendFailCount, aActualRendFailCount);
	TEST(aExpectedRendFailCount==aActualRendFailCount);
	}

/**
   Destructor
 */
CAppfwkSysStartTestStepAppBase::~CAppfwkSysStartTestStepAppBase()
	{
	}

/**
   Constructor
 */
CAppfwkSysStartTestStepAppBase::CAppfwkSysStartTestStepAppBase()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KAppfwkSysStartTestStepAppBase);
	}
