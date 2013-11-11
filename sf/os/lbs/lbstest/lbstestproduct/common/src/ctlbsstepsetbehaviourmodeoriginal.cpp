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
// @file ctlbsstepsetbehaviourmodeoriginal.cpp
// This is the class implementation for the Set Behaviour Mode Original Step
// 
//

/**
 @file
 @internalTechnology
 @test
*/
		  
#include "ctlbsstepsetbehaviourmodeoriginal.h"
#include <lbs/lbsadmin.h>

CT_LbsStep_SetBehaviourModeOriginal::CT_LbsStep_SetBehaviourModeOriginal(CT_LbsServer& aParent) : CT_LbsStep(aParent)
	{
	SetTestStepName(KLbsStep_SetBehaviourModeOriginal);
	}

/**
Static Constructor
*/
CT_LbsStep_SetBehaviourModeOriginal* CT_LbsStep_SetBehaviourModeOriginal::New(CT_LbsServer& aParent)
	{
	return new CT_LbsStep_SetBehaviourModeOriginal(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}


/**
 */
TBool CT_LbsStep_SetBehaviourModeOriginal::SetAndVerifyBehaviourModeOriginalL()
 	{
 	TBool ret = TRUE;
	CLbsAdmin* adminApi = CLbsAdmin::NewL();
	CleanupStack::PushL(adminApi);

	CLbsAdmin::TLbsBehaviourMode behaviourMode;
	
	TInt err = adminApi->Set(KLbsSettingBehaviourMode, CLbsAdmin::ELbsBehaviourModeOriginal);
	User::LeaveIfError(err);
	err = adminApi->Get(KLbsSettingBehaviourMode, behaviourMode);	
	User::LeaveIfError(err);
	
	if(behaviourMode != CLbsAdmin::ELbsBehaviourModeOriginal)
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
@post	Lbs Behaviour mode set to "original"
*/
TVerdict CT_LbsStep_SetBehaviourModeOriginal::doTestStepL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsStep_SetBehaviourModeOriginal::doTestStepL()"));

	if(!SetAndVerifyBehaviourModeOriginalL())
		{
		SetTestStepResult(EFail);
		}
	
	INFO_PRINTF1(_L("&lt;&lt;CT_LbsStep_SetBehaviourModeOriginal::doTestStepL()"));

	return TestStepResult();
	}

