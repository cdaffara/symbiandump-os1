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
// file te_sysutiltestpswritepolicy.cpp
// 
//

#include <e32debug.h>
#include <e32property.h>
#include "te_sysutiltestpswritepolicy.h"
#include "sysutilconstants.h"

CSysUtilTestPSWritePolicyStep::~CSysUtilTestPSWritePolicyStep()
	{
	}

CSysUtilTestPSWritePolicyStep::CSysUtilTestPSWritePolicyStep()
	{
	}

/**
@SYMTestCaseID			SYSLIB-BAFL-CT-4061
@SYMTestCaseDesc		To ensure the Publish and Subscribe properties have the 
						correct write policy.
						
						Note: This test should be repeated with the following 
						combinations of SID and capabilities:
						-	No capabilities and different SID to SysUtilSetup.exe.
						-	WriteDeviceData capability and different SID to SysUtilSetup.exe.
						-	No capabilities and the same SID as SysUtilSetup.exe.
						-	WriteDeviceData capability and the same SID as SysUtilSetup.exe.
						
						In addition, the P&S properties should be deleted so new ones can 
						be created.
@SYMTestPriority		High
@SYMTestActions			1.	If not a manual test - 
                            CCreate an instance of CDeviceTypeInformation by calling 
							SysUtil::GetDeviceTypeInfo (this makes sure the device 
							attributes P&S property gets set).
						2.	if a manual test -
                            Call SysUtil::GetSWVersion (this ensures the version P&S 
							properties get set).
						3.	Use RProperty::Set to change the data in each P&S property.
						4.	Check the return codes.
@SYMTestExpectedResults In all cases, except the last one, the return codes should be 
						KErrPermissionDenied. In the final case the return codes should 
						be KErrNone.
@SYMDEF					CR1328
@SYMDEF 					DEF141094
*/
TVerdict CSysUtilTestPSWritePolicyStep::doTestStepL()
	{
	_LIT(KManual, "Man");
	_LIT(KSWVersion, "sw version");
	_LIT(KLangSWVersion, "langsw version");
	_LIT(KLangVersion, "lang version");
	_LIT(KDeviceTypeInfo, "device type information");
   	_LIT(KDummyValue, "Dummy");
	_LIT(KResult, "Result");
    _LIT(KPRInformation, "Product Release Information");
    
    TDes& section = ConfigSection();
    TBool manualtest = ((section.Length() >3) && (section.Left(3).Compare(KManual) == 0));
	TInt expectedError;
	TInt err;
	
	TBool ret = GetIntFromConfig(section, KResult, expectedError);
	if(!ret)
		{
		ERR_PRINTF1(_L("Error reading \"Result\" from config ini."));
		SetTestStepResult(EAbort);
		return TestStepResult();
		}

    if (manualtest)
        {
    	TBuf<KSysUtilVersionTextLength> buf;
    	err = SysUtil::GetSWVersion(buf);
    	if(KErrNone != err)
    			{
    			ERR_PRINTF2(_L("Error settings up Version P&S properties = %d"), err);
    			SetTestStepResult(EAbort);
    			return TestStepResult();
    			}
    
    	err = RProperty::Set(KSysUtilSetupUid, KSWVersionUid.iUid, KDummyValue);
    	ProcessError(KSWVersion, err, expectedError);
    	
    	err = RProperty::Set(KSysUtilSetupUid, KLangSWVersionUid.iUid, KDummyValue);
    	ProcessError(KLangSWVersion, err, expectedError);
    	
    	err = RProperty::Set(KSysUtilSetupUid, KLangVersionUid.iUid, KDummyValue);
    	ProcessError(KLangVersion, err, expectedError);
    	
    	err = RProperty::Set(KSysUtilSetupUid, KPRInformationUid.iUid, KDummyValue);
    	ProcessError(KPRInformation, err, expectedError);
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

    	err = RProperty::Set(KSysUtilSetupUid, KDeviceTypeInfoUid.iUid, KDummyValue);
    	ProcessError(KDeviceTypeInfo, err, expectedError);	
	    }
		
	return TestStepResult();
	}

void CSysUtilTestPSWritePolicyStep::ProcessError(const TDesC& aProperty, TInt aError, TInt aExpectedError)
	{
	if(aExpectedError != aError)
		{
		ERR_PRINTF2(_L("RProperty::Set did not return the expected error code for %S property."), &aProperty);
		ERR_PRINTF2(_L("Actual error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), aExpectedError);
		SetTestStepResult(EFail);
		}
	else
		{
		INFO_PRINTF3(_L("RProperty::Set returned %d which is the expected error for %S property."), aError, &aProperty);
		}
	}
