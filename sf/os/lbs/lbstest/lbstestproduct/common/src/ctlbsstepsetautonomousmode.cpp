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
// @file ctlbsstep_setautonomousmode.cpp
// This is the class implementation for the Set Autonomous Moder Test Step
// 
//

/**
 @file
 @internalTechnology
 @test
*/
		  
#include "ctlbsstepsetautonomousmode.h"
#include <lbs/lbsadmin.h>

CT_LbsStep_SetAutonomousMode::CT_LbsStep_SetAutonomousMode(CT_LbsServer& aParent) : CT_LbsStep(aParent)
	{
	SetTestStepName(KLbsStep_SetAutonomousMode);
	}

/**
Static Constructor
*/
CT_LbsStep_SetAutonomousMode* CT_LbsStep_SetAutonomousMode::New(CT_LbsServer& aParent)
	{
	return new CT_LbsStep_SetAutonomousMode(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}


/**
 */
TBool CT_LbsStep_SetAutonomousMode::SetAndVerifyAutonomousGpsModeL()
 	{
 	TBool ret = TRUE;
	CLbsAdmin* adminApi = CLbsAdmin::NewL();
	CleanupStack::PushL(adminApi);
	CLbsAdmin::TGpsMode mode;
	
	TInt err = adminApi->Set(KLbsSettingHomeGpsMode, CLbsAdmin::EGpsAutonomous);	
	User::LeaveIfError(err);
	err = adminApi->Get(KLbsSettingHomeGpsMode, mode);	
	User::LeaveIfError(err);
	
	if(mode != CLbsAdmin::EGpsAutonomous)
		{
		INFO_PRINTF1(_L("Admin setting didn't work"));
		ret = FALSE;
		}					
	CleanupStack::PopAndDestroy(adminApi);

	// Alow he setting to be propagated
	User::After(2000000);
	
	return ret;
 	}

/**
@pre 	
@return Test verdict
@post	GPSMode set to autonomous
*/
TVerdict CT_LbsStep_SetAutonomousMode::doTestStepL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsStep_SetAutonomousMode::doTestStepL()"));

	if(!SetAndVerifyAutonomousGpsModeL())
		{
		SetTestStepResult(EFail);
		}
	
	INFO_PRINTF1(_L("&lt;&lt;CT_LbsStep_SetAutonomousMode::doTestStepL()"));

	return TestStepResult();
	}

