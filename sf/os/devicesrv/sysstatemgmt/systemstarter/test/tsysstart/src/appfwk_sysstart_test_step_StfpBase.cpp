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
#include "appfwk_sysstart_test_stfp.h"
#include "appfwk_sysstart_test_step_StfpBase.h"

/**
  This function obtains the test results from the stfp test result file. The
  expected format of the result file is:
  CsCount=value1
  CsFailCount=value2
  CdCount=value3
  CdFailCount=value4
  NcCount=value5
  NcFailCount=value6
  The function uses the '=' character as the delimiter to then grab each 
  value in turn, and set the appropriate member variable.
  
  @see CStateTransFailurePolicy::WriteResultFileL()
  */
void CAppfwkSysStartTestStepStfpBase::GetStfpResultsL()
	{
	RFileReadStream reader;
		
	reader.PushL();
		
	TRAPD(err, reader.Open(iFs,KStfpResultFile,EFileWrite));
	INFO_PRINTF3(_L("Test STFP Result File exists - Expected: %d, Actual: %d"), KErrNone, err);
	TEST(err==KErrNone);
		
	TChar delimChar('=');
	TBuf8<255> myDes;
	reader.ReadL(myDes, delimChar);
	iCsCount = reader.ReadInt8L();
		
	reader.ReadL(myDes, delimChar);
	iCsFailCount = reader.ReadInt8L();
		
	reader.ReadL(myDes, delimChar);
	iCdCount = reader.ReadInt8L();
		
	reader.ReadL(myDes, delimChar);
	iCdFailCount = reader.ReadInt8L();
		
	reader.ReadL(myDes, delimChar);
	iNcCount = reader.ReadInt8L();
		
	reader.ReadL(myDes, delimChar);
	iNcFailCount = reader.ReadInt8L();
		
	CleanupStack::PopAndDestroy(&reader);
	}


/**
  This function obtains the result from the panic result file. When the
  State Transition Failure Policy is set to panic when the state transition
  is failed, then the panic result file should contain an indication
  that a panic has occured.
  
  The expected format is:
  test=1 to indicate a panic has occured.
  */
void CAppfwkSysStartTestStepStfpBase::TestForPanicL()
	{
	RFileReadStream reader;
		
	reader.PushL();
		
	TRAPD(err, reader.Open(iFs,KStfpPanicResultFile,EFileWrite));
	INFO_PRINTF3(_L("Test Panic Result File exists - Expected: %d, Actual: %d"), KErrNone, err);
	TEST(err==KErrNone);
	
	TChar delimChar('=');
	TBuf8<255> myDes;
	reader.ReadL(myDes, delimChar);
	TChar result(reader.ReadUint8L());
	TInt actualCount = result.GetNumericValue();
	TInt expectedCount = 1;
	
	INFO_PRINTF3(_L("Test For Panic - Expected: %d, Actual: %d"), expectedCount, actualCount);
	TEST(actualCount==expectedCount);
		
	CleanupStack::PopAndDestroy(&reader);
	}

/**
  This function compares the expected value passed in, with the member variable, and
  writes the result to the test log.
  
  @param aCsCount - the expected CS Count
  */
void CAppfwkSysStartTestStepStfpBase::TestCsCount(const TInt& aCsCount)
	{
	INFO_PRINTF3(_L("Test Critical Static Count - Expected: %d, Actual: %d"), aCsCount, iCsCount);
	TEST(iCsCount==aCsCount);
	}

/**
  This function compares the expected value passed in, with the member variable, and
  writes the result to the test log.
  
  @param aCsFailCount - the expected CS Fail Count
  */
void CAppfwkSysStartTestStepStfpBase::TestCsFailCount(const TInt& aCsFailCount)
	{
	INFO_PRINTF3(_L("Test Critical Static Fail Count - Expected: %d, Actual: %d"), aCsFailCount, iCsFailCount);
	TEST(iCsFailCount==aCsFailCount);
	}

/**
  This function compares the expected value passed in, with the member variable, and
  writes the result to the test log.
  
  @param aCdCount - the expected CD Count
  */	
void CAppfwkSysStartTestStepStfpBase::TestCdCount(const TInt& aCdCount)
	{
	INFO_PRINTF3(_L("Test Critical Dynamic Count - Expected: %d, Actual: %d"), aCdCount, iCdCount);
	TEST(iCdCount==aCdCount);
	}

/**
  This function compares the expected value passed in, with the member variable, and
  writes the result to the test log.
  
  @param aCdFailCount - the expected CD Fail Count
  */
void CAppfwkSysStartTestStepStfpBase::TestCdFailCount(const TInt& aCdFailCount)
	{
	INFO_PRINTF3(_L("Test Critical Dynamic Fail Count - Expected: %d, Actual: %d"), aCdFailCount, iCdFailCount);
	TEST(iCdFailCount==aCdFailCount);
	}

/**
  This function compares the expected value passed in, with the member variable, and
  writes the result to the test log.
  
  @param aNcCount - the expected NC Count
  */
void CAppfwkSysStartTestStepStfpBase::TestNcCount(const TInt& aNcCount)
	{
	INFO_PRINTF3(_L("Test Non Critical Count - Expected: %d, Actual: %d"), aNcCount, iNcCount);
	TEST(iNcCount==aNcCount);
	}

/**
  This function compares the expected value passed in, with the member variable, and
  writes the result to the test log.
  
  @param aNcFailCount - the expected NC Fail Count
  */
void CAppfwkSysStartTestStepStfpBase::TestNcFailCount(const TInt& aNcFailCount)
	{
	INFO_PRINTF3(_L("Test Non Critical Fail Count - Expected: %d, Actual: %d"), aNcFailCount, iNcFailCount);
	TEST(iNcFailCount==aNcFailCount);
	}

	
/**
   Destructor
 */
CAppfwkSysStartTestStepStfpBase::~CAppfwkSysStartTestStepStfpBase()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepStfpBase::CAppfwkSysStartTestStepStfpBase()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepStfpBase);
	}

