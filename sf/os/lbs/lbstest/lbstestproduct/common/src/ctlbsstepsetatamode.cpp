// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file ctlbsstepsetatamode.cpp
// This is the class implementation for the Set Behaviour Mode Original Step
// 
//

/**
 @file
 @internalTechnology
 @test
*/
		  
#include "ctlbsstepsetatamode.h"
#include <lbs/lbsadmin.h>

CT_LbsStep_SetATAMode::CT_LbsStep_SetATAMode(CT_LbsServer& aParent) : CT_LbsStep(aParent)
	{
	SetTestStepName(KLbsStep_SetATAMode);
	}

/**
Static Constructor
*/
CT_LbsStep_SetATAMode* CT_LbsStep_SetATAMode::New(CT_LbsServer& aParent)
	{
	return new CT_LbsStep_SetATAMode(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}


/**
 */
TBool CT_LbsStep_SetATAMode::SetAndVerifyATAModeL()
 	{
 	TBool ret = TRUE;
	CLbsAdmin* adminApi = CLbsAdmin::NewL();
	CleanupStack::PushL(adminApi);
	
	TInt err = adminApi->Set(KLbsSettingRoamingGpsMode, CLbsAdmin::EGpsAlwaysTerminalAssisted);
	User::LeaveIfError(err);
	err = adminApi->Set(KLbsSettingHomeGpsMode, CLbsAdmin::EGpsAlwaysTerminalAssisted);
	User::LeaveIfError(err);
	// Alow he setting to be propagated
	User::After(2000000);

	CLbsAdmin::TGpsMode gpsMode;
	err = adminApi->Get(KLbsSettingRoamingGpsMode, gpsMode);
	User::LeaveIfError(err);
	if(gpsMode != CLbsAdmin::EGpsAlwaysTerminalAssisted)
		{
		INFO_PRINTF1(_L("Admin setting didn't work"));
		ret = FALSE;
		}					

	err = adminApi->Get(KLbsSettingHomeGpsMode, gpsMode);
	User::LeaveIfError(err);
	if(gpsMode != CLbsAdmin::EGpsAlwaysTerminalAssisted)
		{
		INFO_PRINTF1(_L("Admin setting didn't work"));
		ret = FALSE;
		}					
	CleanupStack::PopAndDestroy(adminApi);
	
	return ret;
 	}

/**
@pre 	
@return Test verdict
@post	Lbs Behaviour mode set to "original"
*/
TVerdict CT_LbsStep_SetATAMode::doTestStepL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsStep_SetATAMode::doTestStepL()"));

	if(!SetAndVerifyATAModeL())
		{
		SetTestStepResult(EFail);
		}
	
	INFO_PRINTF1(_L("&lt;&lt;CT_LbsStep_SetATAMode::doTestStepL()"));

	return TestStepResult();
	}

