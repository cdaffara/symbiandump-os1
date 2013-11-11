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
// @file ctlbsstepsetuproamlocate.cpp
// This is the class implementation for the Set Autonomous Moder Test Step
// 
//

/**
 @file
 @internalTechnology
 @test
*/
		  
#include "ctlbsstepsetuproamselflocate.h"
#include <lbs/lbsadmin.h>

CT_LbsStep_SetupRoamSelfLocate::CT_LbsStep_SetupRoamSelfLocate(CT_LbsServer& aParent)
:CT_LbsStep(aParent)
	{
	SetTestStepName(KLbsStep_SetupRoamSelfLocate);
	}

/**
Static Constructor
*/
CT_LbsStep_SetupRoamSelfLocate* CT_LbsStep_SetupRoamSelfLocate::New(CT_LbsServer& aParent)
	{
	return new CT_LbsStep_SetupRoamSelfLocate(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}


/**
@pre 	
@return Test verdict
@post	GPSMode set to autonomous
*/
TVerdict CT_LbsStep_SetupRoamSelfLocate::doTestStepL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsStep_SetupRoamSelfLocate::doTestStepL()"));

 	CLbsAdmin* adminApi = CLbsAdmin::NewL();
	CleanupStack::PushL(adminApi);
	
	// Switch on the self locate API when roaming.
	CLbsAdmin::TSelfLocateService serviceSetting = CLbsAdmin::ESelfLocateUnknown;

	TInt err = adminApi->Set(KLbsSettingRoamingSelfLocate, CLbsAdmin::ESelfLocateOn);
	User::LeaveIfError(err);
	err = adminApi->Get(KLbsSettingRoamingSelfLocate, serviceSetting);	
	User::LeaveIfError(err);
	
	if(serviceSetting != CLbsAdmin::ESelfLocateOn)
		{
		INFO_PRINTF1(_L("Self locate API admin setting didn't work"));
		SetTestStepResult(EFail);
		}					
	
	// Set gps mode when roaming.
	CLbsAdmin::TGpsMode gpsModeSetting = CLbsAdmin::EGpsModeUnknown;
	
	err = adminApi->Set(KLbsSettingRoamingGpsMode, CLbsAdmin::EGpsPreferTerminalBased);
	User::LeaveIfError(err);
	err = adminApi->Get(KLbsSettingRoamingGpsMode, gpsModeSetting);
	User::LeaveIfError(err);
	
	if (gpsModeSetting != CLbsAdmin::EGpsPreferTerminalBased)
		{
		INFO_PRINTF1(_L("Gps mode admin setting didn't work"));
		SetTestStepResult(EFail);
		}
		
	CleanupStack::PopAndDestroy(adminApi);

	// Alow he setting to be propagated
	User::After(2000000);

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsStep_SetupRoamSelfLocate::doTestStepL()"));
	return TestStepResult();
	}

