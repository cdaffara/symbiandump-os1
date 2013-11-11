// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/

#include "te_sysutilssuitestepbase.h"
#include "sysutildomaincrkeys.h"    // for disc space
#include "sysutilinternalpskeys.h"  // for disc space
#include "sysutilpatchdata.h"       // patchable data defining the MMC drive integer identifer
#include <e32property.h>
#include "sysutilconstants.h"

CTeSysUtilsSuiteStepBase::~CTeSysUtilsSuiteStepBase()
	{
	iFs.Close();
	}

CTeSysUtilsSuiteStepBase::CTeSysUtilsSuiteStepBase()
	{
	}

TInt CTeSysUtilsSuiteStepBase::FindValueL(const TUint aPCKey, const TUint aCRKey, const TInt aPatchableData)
	{
	TInt val(0);

	if ( aPatchableData != -1)
		{
		return aPatchableData;
		}
	// Patchable data not set, try to get it from P & S Keys.
	TInt errorCode = RProperty::Get ( KPSUidDiskLevel, aPCKey, val);
	if ( errorCode != KErrNone)
		{
		CRepository* repository = CRepository::NewLC (KCRUidDiskLevel);
		User::LeaveIfError (repository->Get (aCRKey, val));
		CleanupStack::PopAndDestroy ( repository);
		}

	return val;
	}

TBool CTeSysUtilsSuiteStepBase::CheckError(const TDesC& aAttributeError, const TInt aValue)
	{
	TInt expectedError;
	
	TBool ret = GetIntFromConfig(ConfigSection(), aAttributeError, expectedError);
	if(!ret)
		{
		ERR_PRINTF2(_L("Error reading %S from config ini."), &aAttributeError);
		return EFalse;
		}
		
	if(aValue != expectedError)
		{
		ERR_PRINTF3(_L("%S expected error = %d"), &aAttributeError, expectedError);
		ERR_PRINTF3(_L("%S actual error = %d"), &aAttributeError, aValue);
		return EFalse;
		}
	
	return ETrue;
	}

TBool CTeSysUtilsSuiteStepBase::CheckFileL(const TDesC& aFilename)
	{
	TBool fileShouldExist;
	TBool ret = GetBoolFromConfig(ConfigSection(), KFilePresent, fileShouldExist);
	if(!ret)
		{
		ERR_PRINTF2(_L("Error reading %S from config ini."), &KFilePresent);
		User::Leave(KErrGeneral);
		}
	
	TBool filePresent = CheckIsFileExist(aFilename) == KErrNone ? ETrue : EFalse;
	
	return fileShouldExist == filePresent ? ETrue : EFalse;
	}

TBool CTeSysUtilsSuiteStepBase::CheckValue(const TDesC& aAttribute, const TDesC16& aValue)
	{
	TPtrC16 expectedValue;
	
	TBool ret = GetStringFromConfig(ConfigSection(), aAttribute, expectedValue);
	if(!ret)
		{
		ERR_PRINTF2(_L("Error reading %S from config ini."), &aAttribute);
		return EFalse;
		}
	
	if(aValue.Compare(expectedValue) != 0)
		{
		ERR_PRINTF3(_L("%S expected value = %S"), &aAttribute, &expectedValue);
		ERR_PRINTF3(_L("%S actual value = %S"), &aAttribute, &aValue);
		return EFalse;
		}
	
	return ETrue;
	}

TBool CTeSysUtilsSuiteStepBase::CheckValue(const TDesC& aAttribute, const TUint16 aValue)
	{
	TInt expectedValue;
	
	TBool ret = GetIntFromConfig(ConfigSection(), aAttribute, expectedValue);
	if(!ret)
		{
		ERR_PRINTF2(_L("Error reading %S from config ini."), &aAttribute);
		return EFalse;
		}
	
	if(aValue != expectedValue)
		{
		ERR_PRINTF3(_L("%S expected value = %u"), &aAttribute, expectedValue);
		ERR_PRINTF3(_L("%S actual value = %u"), &aAttribute, aValue);
		return EFalse;
		}
	
	return ETrue;
	}

TBool CTeSysUtilsSuiteStepBase::TestNamedAttributeFunctions(CDeviceTypeInformation& aAttribs)
	{
	TBool passed = ETrue;
	TPtrC16 actualValue;
	
	TInt actualError = aAttribs.GetManufacturerName(actualValue);
	if(CheckError(KManufacturerNameError, actualError) == EFalse)
		passed = EFalse;
	
	if(CheckValue(KManufacturerName, actualValue) == EFalse)
		passed = EFalse;
	
	actualError = aAttribs.GetModelName(actualValue);
	if(CheckError(KModelNameError, actualError) == EFalse)
		passed = EFalse;
	
	if(CheckValue(KModelName, actualValue) == EFalse)
		passed = EFalse;
	
	actualError = aAttribs.GetModelCode(actualValue);
	if(CheckError(KModelCodeError, actualError) == EFalse)
		passed = EFalse;
	
	if(CheckValue(KModelCode, actualValue) == EFalse)
		passed = EFalse;
	
	actualError = aAttribs.GetRevisionID(actualValue);
	if(CheckError(KRevisionIDError, actualError) == EFalse)
		passed = EFalse;
	
	if(CheckValue(KRevisionID, actualValue) == EFalse)
		passed = EFalse;
	
	actualError = aAttribs.GetDefaultDeviceName(actualValue);
	if(CheckError(KDefaultDeviceNameError, actualError) == EFalse)
		passed = EFalse;
	
	if(CheckValue(KDefaultDeviceName, actualValue) == EFalse)
		passed = EFalse;
	
	actualError = aAttribs.GetUIPlatformName(actualValue);
	if(CheckError(KUIPlatformNameError, actualError) == EFalse)
		passed = EFalse;
	
	if(CheckValue(KUIPlatformName, actualValue) == EFalse)
		passed = EFalse;
	
	actualError = aAttribs.GetUIPlatformVersion(actualValue);
	if(CheckError(KUIPlatformVersionStringError, actualError) == EFalse)
		passed = EFalse;
	
	if(CheckValue(KUIPlatformVersion, actualValue) == EFalse)
		passed = EFalse;
	
	TUint16 majorValue, minorValue;
	actualError = aAttribs.GetUIPlatformVersion(majorValue, minorValue);
	if(CheckError(KUIPlatformVersionIntError, actualError) == EFalse)
		passed = EFalse;
	
	if(CheckValue(KUIPlatformVersionMajor, majorValue) == EFalse || CheckValue(KUIPlatformVersionMinor, minorValue) == EFalse)
		passed = EFalse;
	
	actualError = aAttribs.GetOSVersion(actualValue);
	if(CheckError(KOSVersionStringError, actualError) == EFalse)
		passed = EFalse;
	
	if(CheckValue(KOSVersion, actualValue) == EFalse)
		passed = EFalse;
	
	actualError = aAttribs.GetOSVersion(majorValue, minorValue);
	if(CheckError(KOSVersionIntError, actualError) == EFalse)
		passed = EFalse;
	
	if(CheckValue(KOSVersionMajor, majorValue) == EFalse || CheckValue(KOSVersionMinor, minorValue) == EFalse)
		passed = EFalse;
	
	return passed;
	}

TBool CTeSysUtilsSuiteStepBase::TestUnnamedAttributeFunctions(CDeviceTypeInformation& aAttribs)
	{
	TBool passed = ETrue;
	TPtrC16 actualValue;
	
	TInt actualError = aAttribs.GetAttribute(KDeviceCreatorAttributeUid , actualValue);
	if(CheckError(KDeviceCreatorAttributeError, actualError) == EFalse)
		passed = EFalse;
	
	if(CheckValue(KDeviceCreatorAttribute, actualValue) == EFalse)
		passed = EFalse;
	
	return passed;
	}
