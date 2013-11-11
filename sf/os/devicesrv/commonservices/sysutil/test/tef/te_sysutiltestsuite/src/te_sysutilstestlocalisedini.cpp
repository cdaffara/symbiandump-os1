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
// file te_sysutilstestlocalisedini.cpp
// 
//

#include <e32debug.h>
#include <e32property.h>
#include <bautils.h>
#include "te_sysutilstestlocalisedini.h"

CSysUtilsTestLocalisedIniStep::~CSysUtilsTestLocalisedIniStep()
	{
	}

CSysUtilsTestLocalisedIniStep::CSysUtilsTestLocalisedIniStep()
	{
	SetTestStepName(KSysUtilsTestLocalisedIniStep);
	}

/**
@SYMTestCaseID			SYSLIB-BAFL-CT-4057
@SYMTestCaseDesc		To ensure the correct localised INI file is used.
						
						Note: This test should be repeated with:
						-	An INI file with all predefined attributes present which are 
							all valid (this means values should be the correct format and 
							less than or equal to 64 characters).
						-	An INI file which matches corresponds with the set language 
							i.e. if the ideal language is set to Spanish the Spanish 
							version of the INI file should be used.
						-	An INI file which is close to the set language but does not 
							correspond with it.
						For each repetition the INI file should be copied in to 
						C:\versions\deviceattributes.ini. In addition, the P&S properties 
						should be deleted so new ones can be created.
@SYMTestExpectedResults	The retrieved attribute values and return codes should be as 
@SYMTestPriority		High
@SYMTestActions			1.	Set the ideal language using BaflUtils::SetIdealLanguage.
						2.	Create an instance of CDeviceTypeInformation by calling 
							SysUtil::GetDeviceTypeInfo.
						3.	Call each of CDeviceTypeInformation member functions.
						4.	Check the retrieved value and return codes.
						described in the in-source documentation. This means either the 
						actual, default or truncated value should be retrieved. In 
						addition to this, an error code should be returned which 
						signifies which of these values has been retrieved.
@SYMDEF					CR1328
*/
TVerdict CSysUtilsTestLocalisedIniStep::doTestStepL()
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
	
	TInt languageCode;
	_LIT(KLanguage, "Language");
	TBool ret = GetIntFromConfig(ConfigSection(), KLanguage, languageCode);
	if(!ret)
		{
		ERR_PRINTF1(_L("Error reading \"Language\" from config ini."));
		SetTestStepResult(EAbort);
		return TestStepResult();
		}
	
	TLanguage oldLanguage = BaflUtils::IdealLanguage();
	BaflUtils::SetIdealLanguage(static_cast<TLanguage>(languageCode));
	
	TLanguage currentLanguage = BaflUtils::IdealLanguage();
	INFO_PRINTF2(_L("Current Language = %d"), currentLanguage);
	
	CDeviceTypeInformation* attribs = SysUtil::GetDeviceTypeInfoL();
	if(TestNamedAttributeFunctions(*attribs) == EFalse || TestUnnamedAttributeFunctions(*attribs) == EFalse)
		{
		SetTestStepResult(EFail);
		}
	
	delete attribs;
	
	BaflUtils::SetIdealLanguage(oldLanguage);
	
	return TestStepResult();
	}
