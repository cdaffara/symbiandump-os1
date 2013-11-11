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
// file te_sysutilstestattributelimit.cpp
// 
//

#include <e32debug.h>
#include <e32property.h>
#include "te_sysutilstestattributeslimit.h"

CSysUtilsTestAttributesLimitStep::~CSysUtilsTestAttributesLimitStep()
	{
	}

CSysUtilsTestAttributesLimitStep::CSysUtilsTestAttributesLimitStep()
	{
	SetTestStepName(KSysUtilsTestAttributesLimitStep);
	}

/**
@SYMTestCaseID			SYSLIB-BAFL-CT-4059
@SYMTestCaseDesc		To ensure that if more than 450 attributes are provisioned, they 
						are handled correctly.
						
						Note: This test should be repeated with an INI file containing 
						the following number of attributes:
						-	450
						-	451
						
						For each repetition the INI file should be copied in to 
						C:\versions\deviceattributes.ini. In addition, the P&S properties 
						should be deleted so new ones can be created.
@SYMTestPriority		High
@SYMTestActions			1.	Create an instance of CDeviceTypeInformation by calling 
							SysUtil::GetDeviceTypeInfoL.
@SYMTestExpectedResults	In the first case, SysUtil::GetDeviceTypeInfoL should return a 
						valid pointer to an instance of CDeviceTypeInformation. In the 
						second case, it should leave with error code of KErrNotReady.
@SYMDEF					CR1328
*/
TVerdict CSysUtilsTestAttributesLimitStep::doTestStepL()
	{
	TInt err = SetTestPath(ETrue);
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("Could not turn test path on. Error = %d"), err);
		SetTestStepResult(EAbort);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("Test path turned on."));
	
	err = DeletePSProperties();
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not delete P&S properties. Error = %d"), err);
		SetTestStepResult(EAbort);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("Deleted P&S properties successfully."));
	
	TInt expectedError;
	_LIT(KError, "Error");
	TBool ret = GetIntFromConfig(ConfigSection(), KError, expectedError);
	if(!ret)
		{
		ERR_PRINTF1(_L("Error reading \"Error\" from config ini."));
		SetTestStepResult(EAbort);
		return TestStepResult(); 
		}
	
	err = KErrNone;
	CDeviceTypeInformation* attribs = NULL;
	TRAP(err, attribs = SysUtil::GetDeviceTypeInfoL());
	delete attribs;
	
	if(err != expectedError)
		{
		ERR_PRINTF2(_L("Expected error = %d"), expectedError);
		ERR_PRINTF2(_L("Actual error = %d"), err);
		SetTestStepResult(EFail);
		}
	
	return TestStepResult();
	}
