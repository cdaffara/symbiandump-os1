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
// file te_sysutiltestversioninfo.cpp
// 
//

#include <e32debug.h>
#include "te_sysutiltestversioninfo.h"

// These values should match those retrieved by the version APIs
_LIT(KSWVersion, "V 04.02\n4-06-01\nNHL-2\n(c)NMP");
_LIT(KLangVersion, "a");
_LIT(KLangSWVersion, "V 04.02\n4-06-01\nNHL-2\n(c)NMP");
_LIT(KPRInformation, "custom build\n20090626\nusb fix xyz");

// These values should match those returned by the version APIs
const TInt KSWVersionError = 0;
const TInt KLangVersionError = 0;
const TInt KLangSWVersionError = 0;
const TInt KPRInformationError = 0;

CSysUtilTestVersionInfoStep::~CSysUtilTestVersionInfoStep()
	{
	}

CSysUtilTestVersionInfoStep::CSysUtilTestVersionInfoStep(const TDesC& aStepName, CSysUtilBurTestServer& aTestServer) : iTestServer(aTestServer)
	{
	SetTestStepName(aStepName);
	}

TVerdict CSysUtilTestVersionInfoStep::doTestStepL()
	{
	if(TestStepName() == KSysUtilTestVersionInfoPreBackupRestoreStep)
		{
		SetTestStepResult(PreBackupRestoreL());
		}
	else if(TestStepName() == KSysUtilTestVersionInfoPostBackupRestoreStep)
		{
		SetTestStepResult(PostBackupRestoreL());
		}
	
	return TestStepResult();
	}

/*
Perform any setup that needs doing before the backup and restore takes places
*/
TVerdict CSysUtilTestVersionInfoStep::PreBackupRestoreL()
	{
	TInt err = DeletePSProperties();
	if(err == KErrNone)
		{
		INFO_PRINTF1(_L("P&S properties successfully deleted."));
		SetTestStepResult(EPass);
		}
	else
		{
		INFO_PRINTF2(_L("P&S properties could not be deleted. Error = %d"), err);
		SetTestStepResult(EAbort);
		}
	
	iTestServer.SetTestStep(EVersionAPIsStep);
	
	return TestStepResult();
	}

/*
Perform any cleanup that needs doing before the backup and restore takes places
*/
TVerdict CSysUtilTestVersionInfoStep::PostBackupRestoreL()
	{
	iTestServer.SetTestStep(ENoStep);
	
	TInt err = iTestServer.GetTestStepError();
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("An error occured when calling the Version Info APIs during a backup/restore. Error = %d"), err);
		SetTestStepResult(EFail);
		}
	
	return TestStepResult();
	}

/*
Call all the version APIs and check that the retrieved values and returned 
error codes are those that we expect.
*/
void CSysUtilTestVersionInfoStep::TestAllAPIsL()
	{
    TBool passed = ETrue;
	TInt error = SetTestPath(ETrue);
	if(KErrNone != error)
	    {
	    RDebug::Printf("Setting test path to ETrue failed with error %d", error);
	    passed = EFalse;
	    }
	
	TBuf16<KSysUtilVersionTextLength> actualValue;
	RDebug::Printf("Testing SW Version \n");
	TInt actualError = SysUtil::GetSWVersion(actualValue);
	if(KSWVersionError != actualError)
		{
		RDebug::Printf("Actual error = %d\n", actualError);
		RDebug::Printf("Expected error = %d\n", KSWVersionError);
		passed = EFalse;
		}
	
	if(actualValue.Compare(KSWVersion) != 0)
		{
		RDebug::Printf("Actual value = %S\n", &actualValue);
		RDebug::Printf("Expected value = %S\n", &KSWVersion);
		passed = EFalse;
		}
	
	RDebug::Printf("Testing Lang Version \n");
	actualValue.Zero();
	actualError = SysUtil::GetLangVersion(actualValue);
	if(KLangVersionError != actualError)
		{
		RDebug::Printf("Actual error = %d\n", actualError);
		RDebug::Printf("Expected error = %d\n", KLangVersionError);
		passed = EFalse;
		}
	
	if(actualValue.Compare(KLangVersion) != 0)
		{
		RDebug::Printf("Actual value = %S\n", &actualValue);
		RDebug::Printf("Expected value = %S\n", &KLangVersion);
		passed = EFalse;
		}
	
	RDebug::Printf("Testing Lang SW Version \n");
	actualValue.Zero();
	actualError = SysUtil::GetLangSWVersion(actualValue);
	if(KLangSWVersionError != actualError)
		{
		RDebug::Printf("Actual error = %d\n", actualError);
		RDebug::Printf("Expected error = %d\n", KLangSWVersionError);
		passed = EFalse;
		}
	
	if(actualValue.Compare(KLangSWVersion) != 0)
		{
		RDebug::Printf("Actual value = %S\n", &actualValue);
		RDebug::Printf("Expected value = %S\n", &KLangSWVersion);
		passed = EFalse;
		}
	
	RDebug::Printf("Testing PR Information \n");
    actualValue.Zero();
    actualError = SysUtil::GetPRInformation(actualValue);
    if(KPRInformationError != actualError)
        {
        RDebug::Printf("Actual error = %d\nExpected error = %d\n", actualError, KPRInformationError);
        passed = EFalse;
        }
    
    if(actualValue.Compare(KPRInformation) != 0)
        {
        RDebug::Printf("Actual value = %S\nExpected value = %S\n", &actualValue, &KPRInformation);
        passed = EFalse;
        }
	    
	
	if(!passed)
		{
		User::Leave(KErrGeneral);
		}
	}
