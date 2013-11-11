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
// file te_sysutilstestdevicetypeinfo.cpp
// 
//

#include <e32debug.h>
#include <e32property.h>
#include "te_sysutilstestdevicetypeinfo.h"

CSysUtilsTestDeviceTypeInfoStep::~CSysUtilsTestDeviceTypeInfoStep()
	{
	}

CSysUtilsTestDeviceTypeInfoStep::CSysUtilsTestDeviceTypeInfoStep()
	{
	SetTestStepName(KSysUtilsTestDeviceTypeInfoStep);
	}

/**
@SYMTestCaseID			SYSLIB-BAFL-CT-4056
@SYMTestCaseDesc		To ensure the correct values are retrieved and the correct error 
						codes are returned when using either the plug-in, INI file or both.
						
						Note: This test should be repeated with the following INI files 
						and plug-in combinations:
						-	No INI file or plug-in.
						-	Empty INI file.
						-	Corrupt INI file (invalid format).
						-	Corrupt INI file (invalid encoding).
						-	INI file with attribute values which are too long i.e. longer 
							than 64 characters. This should be done for predefined and 
							device creator attributes.
						-	INI file with corrupt OS major version number i.e. text 
							instead of numbers.
						-	INI file with corrupt OS minor version number i.e. text 
							instead of numbers.
						-	INI file with missing OS major version number.
						-	INI file with missing OS minor version number.
						-	INI file with corrupt UI Platform major version number i.e. text 
							instead of numbers.
						-	INI file with corrupt UI Platform minor version number i.e. text 
							instead of numbers.
						-	INI file with missing UI Platform major version number.
						-	INI file with missing UI Platform minor version number.
						-	INI file with all predefined attributes present which are all 
							valid (this means values should be the correct format and 
							less than or equal to 64 characters) and with at least one 
							device creator defined attribute.
						-	INI file with a partial set of attributes.
						-	Plug-in.
						-	Plug-in and valid INI file.
						
						For each repetition the INI file should be copied in to 
						C:\versions\deviceattributes.ini and the plug-in should be copied 
						to C:\sys\bin\sysutilplugin.dll. In addition, the P&S properties 
						should be deleted so new ones can be created.
@SYMTestPriority		High
@SYMTestActions			1.	Create an instance of CDeviceTypeInformation by calling 
							SysUtil::GetDeviceTypeInfo.
						2.	Call each of CDeviceTypeInformation member functions.
						3.	Check the retrieved value and return codes.
@SYMTestExpectedResults	The retrieved attribute values and return codes should be as 
						described in the in-source documentation. This means either the 
						actual, default or truncated value should be retrieved. In 
						addition to this, an error code should be returned which 
						signifies which of these values has been retrieved.
						
						Note: In the case of the plug-in and INI file scenario, the 
						plug-in values should take priority over those in the INI files.
@SYMDEF					CR1328
*/
TVerdict CSysUtilsTestDeviceTypeInfoStep::doTestStepL()
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
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("Could not delete P&S properties. Error = %d"), err);
		SetTestStepResult(EAbort);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("Deleted P&S properties successfully."));
	
	// Check if file exists
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));
	TParse parse;
	parse.Set(drive.Name(), &KDeviceAttributesIniTestPath, NULL);
	
	if(CheckFileL(parse.FullName()) == EFalse)
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	CDeviceTypeInformation* attribs = SysUtil::GetDeviceTypeInfoL();
	if(TestNamedAttributeFunctions(*attribs) == EFalse || TestUnnamedAttributeFunctions(*attribs) == EFalse)
		{
		SetTestStepResult(EFail);
		}
	
	delete attribs;
	
	return TestStepResult();
	}
