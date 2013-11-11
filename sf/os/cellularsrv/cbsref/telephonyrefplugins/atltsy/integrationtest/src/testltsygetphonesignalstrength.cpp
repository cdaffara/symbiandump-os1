// TestTelephonyGetPhonesingalStrength.cpp
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestTelephonyGetPhonesingalStrength

//system include
#include <etelmm.h>

//user include

#include "TestLtsyGetPhoneSignalStrength.h"

/**
 * Constructor
 * 
 */
CTestLtsyGetPhoneSignalStrength::CTestLtsyGetPhoneSignalStrength(CTestLtsyModel& aModel)
    :CTestLtsyPhoneStepBase(aModel)
	{
	}

/**
 * Destructor
 * 
 */
CTestLtsyGetPhoneSignalStrength::~CTestLtsyGetPhoneSignalStrength()
	{
	}

/**
 * Invoked by the test execute framework for exercising functionality under test
 * @return	EPass: The test passed succesfully;
		    EFail: The test failed;
		    EInconclusive: The result of the test was inconclusive;
		    ETestSuiteError: Error in the test suite;
		    EAbort: The test was aborted;
		    EIgnore: The test was unexecuted
 */	

TVerdict CTestLtsyGetPhoneSignalStrength::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestTelePhonyGetPhoneInfo::doTestStepL Begin"));
	if (TestStepResult() == EPass)
		{
		
		GetSignalCaps();
		TRequestStatus reqStatus;
		SharedPhone().GetSignalStrength(reqStatus, iSignalStrength, iBar);
		User::WaitForRequest(reqStatus);
		if (reqStatus==KErrNone)
			{
			INFO_PRINTF2(_L("Signal strength (in dBm) : %d\n"), iSignalStrength);
			INFO_PRINTF2(_L("Signal strength (in display bars) : %d\n"), iBar);
			}
		else
			{
			INFO_PRINTF2(_L("Signal strength, Error %d returned.\n"), reqStatus.Int());
			}
	
		}
	return TestStepResult();
	}

/**
 * Get signal caps
 * 
 */
void CTestLtsyGetPhoneSignalStrength::GetSignalCaps()
	{
	TUint32 caps;
	TInt res = SharedPhone().GetSignalCaps(caps);
	if (res != KErrNone)
		{
		ERR_PRINTF2(_L("The result is not expect %d"),&res);
		SetTestStepResult(EFail);
		}
	}

/**
 * Notify signal strength change
 * 
 */
void CTestLtsyGetPhoneSignalStrength::NotifySignalStrengthChangeL()
	{
	TRequestStatus reqStatus;
	TInt32 signalStrength(0);
	TInt8 bar(0);
	TInt32 completeSignalStrength(70);
	TInt8 completeBar(3);
	SharedPhone().NotifySignalStrengthChange(reqStatus, signalStrength, bar);
	User::WaitForRequest(reqStatus);
	if(bar != completeBar)
		{
		ERR_PRINTF1(_L("bar is not expect."));
		SetTestStepResult(EFail);
		}
	if(signalStrength != completeSignalStrength)
		{
		ERR_PRINTF1(_L("signalStrength is not expect."));
		SetTestStepResult(EFail);
		}
	}

