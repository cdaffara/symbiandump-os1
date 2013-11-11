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
// file te_sysutilstestperflangswpsproperty.cpp
// 
//

#include <e32debug.h>
#include <e32property.h>
#include <hal.h>
#include "te_sysutilstestperflangswpsproperty.h"

// This is the amount of extra time the new APIs can take over the legacy APIs.
static const TInt percentageDifference = 15;

/*
Baseline: M04604_Symbian_OS_v9.5
Build: debug
Date: 06/08/08

If a new hardware platform is added timelegacyapis.exe should be run on this hardware
platform and the times added here.

Note: The WINSCW times don't matter as they are dependent upon the PC on which the tests 
are being run. Performance testing on WINSCW should only be used for debugging purposes.
*/
#ifdef __WINSCW__
static const TReal KLegacyTime1 = 10;
static const TReal KLegacyTime2 = 10;
#else
const TReal KLegacyTime1 = 5.440417;
const TReal KLegacyTime2 = 5.464083;
#endif

CSysUtilsTestPerfLangSWPSPropertyStep::~CSysUtilsTestPerfLangSWPSPropertyStep()
	{
	}

CSysUtilsTestPerfLangSWPSPropertyStep::CSysUtilsTestPerfLangSWPSPropertyStep()
	{
	SetTestStepName(KSysUtilsTestPerfLangSWPSPropertyStep);
	}

/**
Time how long it takes to call GetLangSWVersion.

@return TReal	Time taken to make function call (in milliseconds)
*/
TReal CSysUtilsTestPerfLangSWPSPropertyStep::TimeFuncCall()
	{
	TBuf16<KSysUtilVersionTextLength> version;
	
	TUint32 startTime = User::FastCounter();
	TInt err = SysUtil::GetLangSWVersion(version);
	TUint32 totalTime = User::FastCounter() - startTime;
	
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("GetLangSWVersion error = %d"), err);
		SetTestStepResult(EAbort);
		}
	
	TInt freqInHz = 0;
	HAL::Get(HAL::EFastCounterFrequency, freqInHz);
	TReal freqInkHz = freqInHz / 1000;
	return (TReal)totalTime / freqInkHz;
	}

/**
@SYMTestCaseID			SYSLIB-BAFL-CT-4066
@SYMTestCaseDesc		To provide performance information for the current version of the 
						API so it can be compared with the legacy version of the API.
						
						Note: The first call to SysUtil::GetLangSWVersion() reads the data 
						from sw.txt, lang.txt and langsw.txt and sets up the Version P&S 
						Properties to store this data. This call should take a lot longer 
						than the second call which needs to only read the data from the 
						LangSW P&S property.
						
						The test must have been run on the legacy SysUtil API to get a 
						performance reading. This needs to be done only once for each 
						hardware build target. To generate times for new build targets, 
						timelegacyapis.exe should be run. This can be found under the 
						helper folder in sysutil tests.
						
						The TXT files (sw.txt, lang.txt and langsw.txt) must be on the Z 
						drive to simulate real usage.
						
						The P&S properties should be deleted so new ones can be created.
@SYMTestPriority		High
@SYMTestActions			1.	Start timer.
						2.	Call SysUtil::GetLangSWVersion().
						3.	Stop timer and record time.
						4.	Start timer.
						5.	Call SysUtil::GetLangSWVersion().
						6.	Stop timer and record time.
						7.	Print the first and second times of the current and legacy APIs.
@SYMTestExpectedResults	The test should always pass.
@SYMDEF					CR1328
*/
TVerdict CSysUtilsTestPerfLangSWPSPropertyStep::doTestStepL()
	{
	TInt err = SetTestPath(EFalse);
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("Could not turn test path off. Error = %d"), err);
		SetTestStepResult(EAbort);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("Test path turned off."));
	
	err = DeletePSProperties();
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not delete P&S properties. Error = %d"), err);
		SetTestStepResult(EAbort);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("Deleted P&S properties successfully."));
	
	INFO_PRINTF1(_L("-------- Legacy API --------"));
	INFO_PRINTF2(_L("First call to GetLangSWVersion took %fms"), KLegacyTime1);
	INFO_PRINTF2(_L("Second call to GetLangSWVersion took %fms"), KLegacyTime2);
	INFO_PRINTF1(_L("-------- Current API --------"));
	
	TReal time1 = TimeFuncCall();
	INFO_PRINTF2(_L("First call to GetLangSWVersion took %fms"), time1);
	
	TReal time2 = TimeFuncCall();
	INFO_PRINTF2(_L("Second call to GetLangSWVersion took %fms"), time2);
	if(time2 > KLegacyTime2 * (1 + (percentageDifference / 100)))
		{
		ERR_PRINTF1(_L("The second call to GetLangSWVersion was slower than the legacy time!"));
		SetTestStepResult(EFail);
		}
	
	return TestStepResult();
	}
