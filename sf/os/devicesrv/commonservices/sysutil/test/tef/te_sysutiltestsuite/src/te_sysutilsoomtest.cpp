// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Examine the space left on the given disk
// 
//

/**
 @file 
 @internalTechnology
*/

#include "te_sysutilsoomtest.h"


/**
Destructor
*/
CSysUtilsOOMTestStep::~CSysUtilsOOMTestStep()
	{
	iFs.Close();
	}

/**
Constructor
 */
CSysUtilsOOMTestStep::CSysUtilsOOMTestStep()
	{
	SetTestStepName(KSysUtilsOOMTestStep);
	}


TVerdict CSysUtilsOOMTestStep::doTestStepPreambleL()
	{
	iFs.Connect();
	return TestStepResult();
	}

TVerdict CSysUtilsOOMTestStep::doTestStepPostambleL()
	{
	iFs.Close();
	return TestStepResult();
	}

/**
@SYMTestCaseID			SYSLIB-BAFL-CT-4021
@SYMREQ					REQ1872, REQ1870, CR1328
@SYMDEF					DEF141094
@SYMTestCaseDesc		OOMTest
@SYMTestPriority		HIGH
@SYMTestStatus			Implemented
@SYMTestActions			For each of the SysUtil methods set the !OOM flag in 
@SYMTestActions			the script file then call the method.
@SYMTestExpectedResults Out of memory conditions is handled correctly by each method.
*/
TVerdict CSysUtilsOOMTestStep::doTestStepL()
	{
	_LIT(KSWMethod, "GetSWVersion");
	_LIT(KLangMethod, "GetLangVersion");
	_LIT(KLangSWMethod, "GetLangSWVersion");
	_LIT(KDiskMethod, "DiskSpaceBelowCriticalLevelL");
	_LIT(KFFSMethod, "FFSSpaceBelowCriticalLevelL");
	_LIT(KMmcMethod, "MMCSpaceBelowCriticalLevelL");
	_LIT(KDeviceTypeInfoMethod, "GetDeviceTypeInfoL");
	_LIT(KPRInformationMethod, "GetPRInformation");
	
	_LIT16(kDummy,"xxxxx");
	
	TBuf16<KSysUtilVersionTextLength> version;
	version.Insert(0,kDummy);
	
	TInt err = SetTestPath(EFalse);
	if(err != KErrNone)
	    {
	    WARN_PRINTF2(_L("Could not set test path to EFalse. Error = %d"), err);
	    SetTestStepResult(EAbort);
	    return TestStepResult();
	    }
	err = DeletePSProperties();
	if(err != KErrNone)
		{
		WARN_PRINTF2(_L("Could not delete P&S properties. Error = %d"), err);
		SetTestStepResult(EAbort);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("Deleted P&S properties successfully."));
	
	err = SysUtil::GetSWVersion(version);
	ProcessError(KSWMethod, err);
	
	err = SysUtil::GetLangVersion(version);
	ProcessError(KLangMethod, err);
	
	err = SysUtil::GetLangSWVersion(version);
	ProcessError(KLangSWMethod, err);

	TRAP(err, SysUtil::DiskSpaceBelowCriticalLevelL(&iFs, 0, EDriveC));
	ProcessError(KDiskMethod, err);
	
	TRAP(err, SysUtil::FFSSpaceBelowCriticalLevelL(NULL, 0));
	ProcessError(KFFSMethod, err);
	
	TRAP(err, SysUtil::MMCSpaceBelowCriticalLevelL(NULL, 0));
	ProcessError(KMmcMethod, err);
	
	CDeviceTypeInformation* attribs = NULL;
	TRAP(err, attribs = SysUtil::GetDeviceTypeInfoL());
	ProcessError(KDeviceTypeInfoMethod, err);
	delete attribs;
	
	err = SysUtil::GetPRInformation(version);
	ProcessError(KPRInformationMethod, err);
	    
	return TestStepResult();
	}

void CSysUtilsOOMTestStep::ProcessError(const TDesC& aMethod, TInt aErr)
	{
	//This only prints out if an error occurs as !OOM doesn't check for failures
	if ((aErr !=KErrNone) && (aErr !=KErrNoMemory))
		{
		ERR_PRINTF1(aMethod);
		ERR_PRINTF2(_L("Error occured, error code = %d"), aErr);
		SetTestStepResult (EFail);
		}
	else if (aErr ==KErrNoMemory)
		{
		INFO_PRINTF1(aMethod);
		INFO_PRINTF1(_L("KErrNoMemory caught "));
		}
	}

