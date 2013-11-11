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
// file te_sysutiltestdevicetypeinfo.cpp
// 
//

#include <e32debug.h>
#include "te_sysutiltestdevicetypeinfo.h"

// These values should match those retrieved by the CDeviceTypeInformation APIs
_LIT(KManufacturerName, "MyManufacturerName");
_LIT(KModelName, "TestModelName");
_LIT(KModelCode, "Symbian");
_LIT(KRevisionID, "MyRevisionID");
_LIT(KDefaultDeviceName, "Symbian");
_LIT(KUIPlatformName, "TestUIPlatformName");
_LIT(KUIPlatformVersion, "100.3");
const TUint16 KUIPlatformVersionMajor = 100;
const TUint16 KUIPlatformVersionMinor = 3;
_LIT(KOSVersion, "0.0");
const TUint16 KOSVersionMajor = 0;
const TUint16 KOSVersionMinor = 0;
_LIT(KDeviceCreatorAttribute, "MyDeviceCreatorAttribute");

// These values should match those returned by the CDeviceTypeInformation APIs
const TInt KManufacturerNameError = 0;
const TInt KModelNameError = 0;
const TInt KModelCodeError = 1;
const TInt KRevisionIDError = 0;
const TInt KDefaultDeviceNameError = 1;
const TInt KUIPlatformNameError = 0;
const TInt KUIPlatformVersionStringError = 0;
const TInt KUIPlatformVersionIntError = 0;
const TInt KOSVersionStringError = 1;
const TInt KOSVersionIntError = 1;
const TInt KDeviceCreatorAttributeError = 0;

CSysUtilTestDeviceTypeInfoStep::~CSysUtilTestDeviceTypeInfoStep()
	{
	}

CSysUtilTestDeviceTypeInfoStep::CSysUtilTestDeviceTypeInfoStep(const TDesC& aStepName, CSysUtilBurTestServer& aTestServer) : iTestServer(aTestServer)
	{
	SetTestStepName(aStepName);
	}

TVerdict CSysUtilTestDeviceTypeInfoStep::doTestStepL()
	{
	if(TestStepName() == KSysUtilTestDeviceTypeInfoPreBackupRestoreStep)
		{
		SetTestStepResult(PreBackupRestoreL());
		}
	else if(TestStepName() == KSysUtilTestDeviceTypeInfoPostBackupRestoreStep)
		{
		SetTestStepResult(PostBackupRestoreL());
		}
	
	return TestStepResult();
	}

/*
Perform any setup that needs doing before the backup and restore takes places
*/
TVerdict CSysUtilTestDeviceTypeInfoStep::PreBackupRestoreL()
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
	
	iTestServer.SetTestStep(EDeviceTypeInfoAPIsStep);
	
	return TestStepResult();
	}

/*
Perform any cleanup that needs doing before the backup and restore takes places
*/
TVerdict CSysUtilTestDeviceTypeInfoStep::PostBackupRestoreL()
	{
	iTestServer.SetTestStep(ENoStep);
	
	TInt err = iTestServer.GetTestStepError();
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("An error occured when calling the Device Type Info APIs during a backup and restore. Error = %d"), err);
		SetTestStepResult(EFail);
		}
	
	return TestStepResult();
	}

/*
Call all the CDeviceTypeInformation APIs and check that the retrieved values and returned 
error codes are those that we expect.
*/
void CSysUtilTestDeviceTypeInfoStep::TestAllAPIsL()
	{
	SetTestPath(ETrue);
	
	TBool passed = ETrue;
	
	CDeviceTypeInformation* attribs = SysUtil::GetDeviceTypeInfoL();

	TPtrC16 actualValue;
	RDebug::Printf("Testing Manufacturer Name\n");
	TInt actualError = attribs->GetManufacturerName(actualValue);
	if(KManufacturerNameError != actualError)
		{
		RDebug::Printf("Actual error = %d\n", actualError);
		RDebug::Printf("Expected error = %d\n", KManufacturerNameError);
		passed = EFalse;
		}
	
	if(actualValue.Compare(KManufacturerName) != 0)
		{
		RDebug::Printf("Actual value = %S\n", &actualValue);
		RDebug::Printf("Expected value = %S\n", &KManufacturerNameError);
		passed = EFalse;
		}
	
	RDebug::Printf("Testing Model Name\n");
	actualError = attribs->GetModelName(actualValue);
	if(KModelNameError != actualError)
		{
		RDebug::Printf("Actual error = %d\n", actualError);
		RDebug::Printf("Expected error = %d\n", KModelNameError);
		passed = EFalse;
		}
	
	if(actualValue.Compare(KModelName) != 0)
		{
		RDebug::Printf("Actual value = %S\n", &actualValue);
		RDebug::Printf("Expected value = %S\n", &KModelName);
		passed = EFalse;
		}
	
	RDebug::Printf("Testing Model Code\n");
	actualError = attribs->GetModelCode(actualValue);
	if(KModelCodeError != actualError)
		{
		RDebug::Printf("Actual error = %d\n", actualError);
		RDebug::Printf("Expected error = %d\n", KModelCodeError);
		passed = EFalse;
		}
	
	if(actualValue.Compare(KModelCode) != 0)
		{
		RDebug::Printf("Actual value = %S\n", &actualValue);
		RDebug::Printf("Expected value = %S\n", &KModelCode);
		passed = EFalse;
		}
	
	RDebug::Printf("Testing Revision ID\n");
	actualError = attribs->GetRevisionID(actualValue);
	if(KRevisionIDError != actualError)
		{
		RDebug::Printf("Actual error = %d\n", actualError);
		RDebug::Printf("Expected error = %d\n", KRevisionIDError);
		passed = EFalse;
		}
	
	if(actualValue.Compare(KRevisionID) != 0)
		{
		RDebug::Printf("Actual value = %S\n", &actualValue);
		RDebug::Printf("Expected value = %S\n", &KRevisionID);
		passed = EFalse;
		}
	
	RDebug::Printf("Testing Default Device Name\n");
	actualError = attribs->GetDefaultDeviceName(actualValue);
	if(KDefaultDeviceNameError != actualError)
		{
		RDebug::Printf("Actual error = %d\n", actualError);
		RDebug::Printf("Expected error = %d\n", KDefaultDeviceNameError);
		passed = EFalse;
		}
	
	if(actualValue.Compare(KDefaultDeviceName) != 0)
		{
		RDebug::Printf("Actual value = %S\n", &actualValue);
		RDebug::Printf("Expected value = %S\n", &KDefaultDeviceName);
		passed = EFalse;
		}
	
	RDebug::Printf("Testing UI Platform name\n");
	actualError = attribs->GetUIPlatformName(actualValue);
	if(KUIPlatformNameError != actualError)
		{
		RDebug::Printf("Actual error = %d\n", actualError);
		RDebug::Printf("Expected error = %d\n", KUIPlatformNameError);
		passed = EFalse;
		}
	
	if(actualValue.Compare(KUIPlatformName) != 0)
		{
		RDebug::Printf("Actual value = %S\n", &actualValue);
		RDebug::Printf("Expected value = %S\n", &KUIPlatformName);
		passed = EFalse;
		}
	
	RDebug::Printf("Testing UI Platform Version (Descriptor)\n");
	actualError = attribs->GetUIPlatformVersion(actualValue);
	if(KUIPlatformVersionStringError != actualError)
		{
		RDebug::Printf("Actual error = %d\n", actualError);
		RDebug::Printf("Expected error = %d\n", KUIPlatformVersionStringError);
		passed = EFalse;
		}
	
	if(actualValue.Compare(KUIPlatformVersion) != 0)
		{
		RDebug::Printf("Actual value = %S\n", &actualValue);
		RDebug::Printf("Expected value = %S\n", &KUIPlatformVersion);
		passed = EFalse;
		}
	
	RDebug::Printf("Testing UI Platform Version (TInts)\n");
	TUint16 majorValue, minorValue;
	actualError = attribs->GetUIPlatformVersion(majorValue, minorValue);
	if(KUIPlatformVersionIntError != actualError)
		{
		RDebug::Printf("Actual error = %d\n", actualError);
		RDebug::Printf("Expected error = %d\n", KUIPlatformVersionIntError);
		passed = EFalse;
		}
	
	if(majorValue != KUIPlatformVersionMajor)
		{
		RDebug::Printf("Actual value = %d\n", majorValue);
		RDebug::Printf("Expected value = %d\n", KUIPlatformVersionMajor);
		passed = EFalse;
		}
	
	if(minorValue != KUIPlatformVersionMinor)
		{
		RDebug::Printf("Actual value = %d\n", minorValue);
		RDebug::Printf("Expected value = %d\n", KUIPlatformVersionMinor);
		passed = EFalse;
		}
	
	RDebug::Printf("Testing OS Version (Descriptor)\n");
	actualError = attribs->GetOSVersion(actualValue);
	if(KOSVersionStringError != actualError)
		{
		RDebug::Printf("Actual error = %d\n", actualError);
		RDebug::Printf("Expected error = %d\n", KOSVersionStringError);
		passed = EFalse;
		}
	
	if(actualValue.Compare(KOSVersion) != 0)
		{
		RDebug::Printf("Actual value = %S\n", &actualValue);
		RDebug::Printf("Expected value = %S\n", &KOSVersion);
		passed = EFalse;
		}
	
	RDebug::Printf("Testing OS Version (TInts)\n");
	actualError = attribs->GetOSVersion(majorValue, minorValue);
	if(KOSVersionIntError != actualError)
		{
		RDebug::Printf("Actual error = %d\n", actualError);
		RDebug::Printf("Expected error = %d\n", KOSVersionIntError);
		passed = EFalse;
		}
	
	if(majorValue != KOSVersionMajor)
		{
		RDebug::Printf("Actual value = %d\n", majorValue);
		RDebug::Printf("Expected value = %d\n", KOSVersionMajor);
		passed = EFalse;
		}
	
	if(minorValue != KOSVersionMinor)
		{
		RDebug::Printf("Actual value = %d\n", minorValue);
		RDebug::Printf("Expected value = %d\n", KOSVersionMinor);
		passed = EFalse;
		}
	
	RDebug::Printf("Testing Device Creator Attribute\n");
	actualError = attribs->GetAttribute(KDeviceCreatorAttributeUid , actualValue);
	if(KDeviceCreatorAttributeError != actualError)
		{
		RDebug::Printf("Actual error = %d\n", actualError);
		RDebug::Printf("Expected error = %d\n", KDeviceCreatorAttributeError);
		passed = EFalse;
		}
	
	if(actualValue.Compare(KDeviceCreatorAttribute) != 0)
		{
		RDebug::Printf("Actual value = %s\n", &actualValue);
		RDebug::Printf("Expected value = %s\n", &KDeviceCreatorAttribute);
		passed = EFalse;
		}
	
	delete attribs;
	
	if(!passed)
		{
		User::Leave(KErrGeneral);
		}
	}
