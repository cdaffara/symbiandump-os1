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
#include "appfwk_sysstart_test_step_DllBase.h"

/**
  This function compares the results obtained from a system file created
  by the test DLL with those passed in from the test case.

  @param aFileName
  Name of file which contains test results
  
  @param aExpectedResults
  Struct containing the expected results for test cases, to be compared against.
  */
void CAppfwkSysStartTestStepDllBase::TestDllResultsL(const TDesC& aFileName, const TTestDllResults& aExpectedResults)	
	{
	TTestDllResults actualResults;
	ReadTestDllResultsFileL(aFileName, actualResults);
		
	INFO_PRINTF3(_L("Test Function Call Count - Expected: %d, Actual: %d"), aExpectedResults.count, actualResults.count);
	TEST(aExpectedResults.count==actualResults.count);

	INFO_PRINTF3(_L("Test ARG1 Value - Expected: %d, Actual: %d"), aExpectedResults.arg1, actualResults.arg1);
	TEST(aExpectedResults.arg1==actualResults.arg1);
	INFO_PRINTF3(_L("Test ARG2 Value - Expected: %d, Actual: %d"), aExpectedResults.arg2, actualResults.arg2);
	TEST(aExpectedResults.arg2==actualResults.arg2);
	INFO_PRINTF3(_L("Test ARG3 Value - Expected: %d, Actual: %d"), aExpectedResults.arg3, actualResults.arg3);
	TEST(aExpectedResults.arg3==actualResults.arg3);
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
void CAppfwkSysStartTestStepDllBase::TestEpocWindResultsL(const TDesC& aFileName, const TInt& aExpectedResult)
	{
	TInt myActualResult;
	ReadTestDllEpocWindResultsFileL(aFileName, myActualResult);
	
	INFO_PRINTF3(_L("Test call count - Expected: %d, Actual: %d"), aExpectedResult, myActualResult);
	TEST(aExpectedResult==myActualResult);
	}
	
/**
  This function checks that a specified result file does not exist. This
  scenario occurs when SysStart is expected to panic prior to the creation
  of result files. e.g. dll function 2 called, fails, panics, thereby
  preventing dll function 3 from being called, and function 3 result file
  from being created.

  @param aFileName
  Name of file which is expected to be missing.
  */
void CAppfwkSysStartTestStepDllBase::TestResultFileDoesNotExist(const TDesC& aFileName)
	{
	RFile myFile;
	TInt actualErr = myFile.Open(iFs,aFileName,EFileWrite);
	
	INFO_PRINTF3(_L("Test file should be missing - Expected: %d, Actual: %d"), KErrNotFound, actualErr);
	TEST(actualErr==KErrNotFound);
	}


/**
  This function obtains the test result from the epoc wind test result file. The
  expected format of the result file is:
  text=value
  The function uses the '=' character as the delimiter to then grab the value.
  
  @see CAppfwkSysStartTestStepDllBase::TestEpocWindResultsL

  @param aFileName
  Name of file which contains epoc wind test results
  
  @param aResult
  The obtained result for the test case.
  */
void CAppfwkSysStartTestStepDllBase::ReadTestDllEpocWindResultsFileL(const TDesC& aFileName, TInt& aResult)
	{
	RFileReadStream reader;
		
	reader.PushL();
	
	User::LeaveIfError(reader.Open(iFs,aFileName,EFileWrite));
	TChar delimChar('=');
	TBuf8<255> myDes;
	reader.ReadL(myDes, delimChar);
	
	TChar result(reader.ReadUint8L());
	
	aResult = result.GetNumericValue();
	
	reader.Close();

	CleanupStack::PopAndDestroy(&reader); 
	}

/**
  This function obtains the test results from the dll test result file. The
  expected format of the result file is:
  count=value1
  arg1=value2
  arg2=value3
  arg3=value4
  The function uses the '=' character as the delimiter to then grab each 
  value in turn.
  
  @see CAppfwkSysStartTestStepDllBase::TestDllResultsL

  @param aFileName
  Name of file which contains epoc wind test results
  
  @param aResult
  The obtained result for the test case.
  */
void CAppfwkSysStartTestStepDllBase::ReadTestDllResultsFileL(const TDesC& aFileName, TTestDllResults& aTestDllResults)
	{
	RFileReadStream reader;
		
	reader.PushL();
	
	User::LeaveIfError(reader.Open(iFs,aFileName,EFileWrite));
	TChar delimChar('=');
	TBuf8<255> myDes;
	reader.ReadL(myDes, delimChar);
	aTestDllResults.count = reader.ReadInt16L();
	
	reader.ReadL(myDes, delimChar);
	aTestDllResults.arg1 = reader.ReadInt16L();
	reader.ReadL(myDes, delimChar);
	aTestDllResults.arg2 = reader.ReadInt16L();
	reader.ReadL(myDes, delimChar);
	aTestDllResults.arg3 = reader.ReadInt16L();
	
	reader.Close();

	CleanupStack::PopAndDestroy(&reader); 
	}

	
/**
   Destructor
 */
CAppfwkSysStartTestStepDllBase::~CAppfwkSysStartTestStepDllBase()
	{
	}

/**
   Constructor
 */
CAppfwkSysStartTestStepDllBase::CAppfwkSysStartTestStepDllBase()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepDllBase);
	}
