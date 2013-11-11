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
// @file ctlbsstepsetprivhandler.cpp
// This is the class implementation for the Set Privacy Controller Test Step
// 
//

/**
 @file
 @internalTechnology
 @test
*/
		  
#include "ctlbsstepsetprivhandler.h"

//Literals Used
_LIT(KHandlerType, "privhandler_type");

CT_LbsStep_SetPrivacyHandler::CT_LbsStep_SetPrivacyHandler(CT_LbsServer& aParent) : CT_LbsStep(aParent)
	{
	SetTestStepName(KLbsStep_SetPrivacyHandler);
	}

/**
Static Constructor
*/
CT_LbsStep_SetPrivacyHandler* CT_LbsStep_SetPrivacyHandler::New(CT_LbsServer& aParent)
	{
	return new CT_LbsStep_SetPrivacyHandler(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}


/**	Let LBS know whether we're using privacy controller or notifier:
 */
TBool CT_LbsStep_SetPrivacyHandler::SetAndVerifyPrivacyHandlerL(CLbsAdmin::TPrivacyHandler aPrivacyHandlerType)
 	{
 	TBool ret = TRUE;
	CLbsAdmin* adminApi = CLbsAdmin::NewL();
	CleanupStack::PushL(adminApi);
	TInt err;
	
	//CLbsAdmin::TPrivacyHandler privacyHandler = CLbsAdmin::EPrivacyHandleByController;
	err = adminApi->Set(KLbsSettingPrivacyHandler, aPrivacyHandlerType);
	
	User::LeaveIfError(err);
	err = adminApi->Get(KLbsSettingPrivacyHandler, aPrivacyHandlerType);	
	User::LeaveIfError(err);
	
	//Now we support 3 Privacy Handlers so need to comment out below check
	//if(aPrivacyHandlerType != CLbsAdmin::EPrivacyHandleByController)
	//	{
	//	INFO_PRINTF1(_L("Admin setting didn't work"));
	//	ret = FALSE;
	//	}	
					
	CleanupStack::PopAndDestroy(adminApi);

	// Allow he setting to be propagated
	User::After(4 * 1000000);
	
	return ret;
 	}

/**
@pre 	
@return Test verdict
@post	GPSMode set to autonomous
*/
TVerdict CT_LbsStep_SetPrivacyHandler::doTestStepL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsStep_SetPrivacyHandler::doTestStepL()"));

	CLbsAdmin::TPrivacyHandler privacyHandlerType = CLbsAdmin::EPrivacyHandleByController;
	TInt privhandlerconfig;

	if (GetIntFromConfig(ConfigSection(), KHandlerType, privhandlerconfig))	
		{
		if(1 == privhandlerconfig)
			{
			privacyHandlerType = CLbsAdmin::EPrivacyHandleByController;
			}
		else if (2 == privhandlerconfig)
			{
			privacyHandlerType = CLbsAdmin::EPrivacyHandleByNotifier;
			}
		else if (3 == privhandlerconfig)
			{
			privacyHandlerType = CLbsAdmin::EPrivacyHandleByAdvancedNotifier;
			}
		else
			{
			INFO_PRINTF2(_L("&gt;&gt;Unrecognised privacyHandlerType: %d"), privacyHandlerType);
			User::Invariant();
			}

		if(!SetAndVerifyPrivacyHandlerL(privacyHandlerType))
			{
			SetTestStepResult(EFail);
			}
		}
	INFO_PRINTF1(_L("&lt;&lt;CT_LbsStep_SetPrivacyHandler::doTestStepL()"));

	return TestStepResult();
	}

