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
// file te_sysutilstestpluginoverride.cpp
// 
//

#include <e32debug.h>
#include <e32property.h>
#include "te_sysutilstestpluginoverride.h"

CSysUtilsTestPluginOverrideStep::~CSysUtilsTestPluginOverrideStep()
	{
	}

CSysUtilsTestPluginOverrideStep::CSysUtilsTestPluginOverrideStep()
	{
	SetTestStepName(KSysUtilsTestPluginOverrideStep);
	}

/**
@SYMTestCaseID			SYSLIB-BAFL-CT-4062
@SYMTestCaseDesc		To ensure that the plug-in is only loaded from the the Z drive.
						
						Note: A version of the plug-in should be located in 
						Z:\sys\bin\sysutilplugin.dll. In addition, the P&S properties 
						should be deleted so new ones can be created.
@SYMTestPriority		High
@SYMTestActions			1.	Place a plug-in on the system drive.
						2.	Create an instance of CDeviceTypeInformation by calling 
							SysUtil::GetDeviceTypeInfo.
						3.	Call each of CDeviceTypeInformation member functions.
						4.	Check the retrieved value and return codes.
@SYMTestExpectedResults	The retrieved attribute values should be the defaults and the 
						return codes should be KDefaultValue
@SYMDEF					CR1328
*/
TVerdict CSysUtilsTestPluginOverrideStep::doTestStepL()
	{
	User::LeaveIfError(SetTestPath(EFalse));
	
	TInt err = DeletePSProperties();
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("Could not delete P&S property. Error = %d"), err);
		SetTestStepResult(EAbort);
		return TestStepResult();
		}
	else
		{
		INFO_PRINTF1(_L("Deleted P&S properties successfully."));
		}
	
	CDeviceTypeInformation* attribs = SysUtil::GetDeviceTypeInfoL();
	if(TestNamedAttributeFunctions(*attribs) == EFalse || TestUnnamedAttributeFunctions(*attribs) == EFalse)
		{
		SetTestStepResult(EFail);
		}
	
	delete attribs;
	
	return TestStepResult();
	}
