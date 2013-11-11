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
// file te_sysutilstestperfdevicetypeinfopsproperty.cpp
// 
//

#include <e32debug.h>
#include <e32property.h>
#include <hal.h>
#include "te_sysutilstestperfdevicetypeinfopsproperty.h"

CSysUtilsTestPerfDeviceTypeInfoPSPropertyStep::~CSysUtilsTestPerfDeviceTypeInfoPSPropertyStep()
	{
	}

CSysUtilsTestPerfDeviceTypeInfoPSPropertyStep::CSysUtilsTestPerfDeviceTypeInfoPSPropertyStep()
	{
	SetTestStepName(KSysUtilsTestPerfDeviceTypeInfoPSPropertyStep);
	}

/**
Time how long it takes to call GetDeviceTypeInfoL.

@return TReal	Time taken to make function call (in milliseconds)
*/
TReal CSysUtilsTestPerfDeviceTypeInfoPSPropertyStep::TimeFuncCallL()
	{
	CDeviceTypeInformation* attribs = NULL;
	
	TUint32 startTime = User::FastCounter();
	attribs = SysUtil::GetDeviceTypeInfoL();
	TUint32 totalTime = User::FastCounter() - startTime;
	
	delete attribs;
	
	TInt freqInHz = 0;
	HAL::Get(HAL::EFastCounterFrequency, freqInHz);
	TReal freqInkHz = freqInHz / 1000;
	return (TReal)totalTime / freqInkHz;
	}

/**
@SYMTestCaseID			SYSLIB-BAFL-CT-4067
@SYMTestCaseDesc		To provide performance information for the GetDeviceTypeInfoL API.
						
						Note: The first call to SysUtil::GetDeviceTypeInfoL() gets the 
						data from the plug-in and sets up the DeviceTypeInfo P&S Property 
						to store this data. This call should take a lot longer than the 
						second call which needs to only read the data from the 
						DeviceTypeInfo P&S property.
						
						A plug-in containing valid device type information must be located 
						in Z:\sys\bin to simulate real usage.
						
						The P&S properties should be deleted so new ones can be created.
						
						It is not possible to use INI files for this test as it will 
						impact the results of SYSLIB-BAFL-CT-4062.
@SYMTestPriority		High
@SYMTestActions			1.	Start timer.
						2.	Call SysUtil::GetDeviceTypeInfoL ().
						3.	Stop timer and record time.
						4.	Start timer.
						5.	Call SysUtil::GetDeviceTypeInfoL ().
						6.	Stop timer and record time.
						7.	Print both sets of times.
@SYMTestExpectedResults	The test should always pass.
@SYMDEF					CR1328
*/
TVerdict CSysUtilsTestPerfDeviceTypeInfoPSPropertyStep::doTestStepL()
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
	
	TReal time1 = TimeFuncCallL();
	INFO_PRINTF2(_L("First call to GetDeviceTypeInfoL took %fms"), time1);
	
	TReal time2 = TimeFuncCallL();
	INFO_PRINTF2(_L("Second call to GetDeviceTypeInfoL took %fms"), time2);
	
	return TestStepResult();
	}
