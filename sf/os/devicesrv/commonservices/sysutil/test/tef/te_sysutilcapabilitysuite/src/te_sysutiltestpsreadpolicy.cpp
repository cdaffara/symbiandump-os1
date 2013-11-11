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
// file te_sysutiltestpsreadpolicy.cpp
// 
//

#include <e32debug.h>
#include <e32property.h>
#include "te_sysutiltestpsreadpolicy.h"
#include "sysutilconstants.h"

CSysUtilTestPSReadPolicyStep::~CSysUtilTestPSReadPolicyStep()
	{
	}

CSysUtilTestPSReadPolicyStep::CSysUtilTestPSReadPolicyStep()
	{
	}

/**
@SYMTestCaseID			SYSLIB-BAFL-CT-4060
@SYMTestCaseDesc		To ensure the Publish and Subscribe properties have the 
						correct read policy.
						
						Note: The test executable should have no capabilities and a UID 
						which differs from that of SysUtilSetup.exe. In addition, 
						the P&S properties should be deleted so new ones can be created.
@SYMTestPriority		High
@SYMTestActions			1.	If not a manual test - 
                            Create an instance of CDeviceTypeInformation by calling 
							SysUtil::GetDeviceTypeInfo (this makes sure the device 
							attributes P&S property gets set).
						2.  if a manual test -
                            Call SysUtil::GetSWVersion (this ensures the version P&S 
							properties get set).
						3.	Use RProperty::Get to read each P&S property.
						4.	Check the return codes.
@SYMTestExpectedResults	The return codes should be KErrNone.
@SYMDEF					CR1328
@SYMDEF 					DEF141094
*/
TVerdict CSysUtilTestPSReadPolicyStep::doTestStepL()
	{
	_LIT(KManual, "Man");
	_LIT(KSWVersion, "sw version");
	_LIT(KLangSWVersion, "langsw version");
	_LIT(KLangVersion, "lang version");
	_LIT(KDeviceTypeInfo, "device type information");
	_LIT(KPRInformation, "Product Release Information");
	    
	const TInt bufferSize = 500;
	TInt err;
	TBuf<bufferSize> buf;
	
    TDes& section = ConfigSection();
    TBool manualtest = ((section.Length() >3) && (section.Left(3).Compare(KManual) == 0));

    if (manualtest)
        {	
    	err = SysUtil::GetSWVersion(buf);
    	if(KErrNone != err)
    		{
    		ERR_PRINTF2(_L("Error settings up Version P&S properties = %d"), err);
    		SetTestStepResult(EAbort);
    		return TestStepResult();
    		}
    		
	    err = RProperty::Get(KSysUtilSetupUid, KSWVersionUid.iUid, buf);
	    ProcessError(KSWVersion, err);
	
	    err = RProperty::Get(KSysUtilSetupUid, KLangSWVersionUid.iUid, buf);
	    ProcessError(KLangSWVersion, err);
	
	    err = RProperty::Get(KSysUtilSetupUid, KLangVersionUid.iUid, buf);
	    ProcessError(KLangVersion, err);
	    
	    err = RProperty::Get(KSysUtilSetupUid, KPRInformationUid.iUid, buf);
	    ProcessError(KPRInformation, err);
        }
    else
        {
    	CDeviceTypeInformation* attribs = NULL;
    	TRAP(err, attribs = SysUtil::GetDeviceTypeInfoL());
    	if(KErrNone != err)
    		{
    		ERR_PRINTF2(_L("Error settings up DeviceTypeInfo P&S property = %d"), err);
    		SetTestStepResult(EAbort);
    		return TestStepResult();
    		}
    	delete attribs;

    	err = RProperty::Get(KSysUtilSetupUid, KDeviceTypeInfoUid.iUid, buf);
	    ProcessError(KDeviceTypeInfo, err);
	    }
	
	return TestStepResult();
	}

void CSysUtilTestPSReadPolicyStep::ProcessError(const TDesC& aProperty, TInt aError)
	{
	if(KErrNone != aError)
		{
		ERR_PRINTF2(_L("RProperty::Get did not return the expected error code for %S property."), &aProperty);
		ERR_PRINTF2(_L("Actual error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), KErrNone);
		SetTestStepResult(EFail);
		}
	else
		{
		INFO_PRINTF3(_L("RProperty::Get returned %d which is the expected error for %S property."), aError, &aProperty);
		}
	}
