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
// @file ctlbsstep_setupprotocolstub.cpp
// This is the class implementation for the Set Autonomous Moder Test Step
// 
//

/**
 @file
 @internalTechnology
 @test
*/
		  
#include "ctlbsstepsetupprotocolstub.h"
#include <lbs/lbsadmin.h>

CT_LbsStep_SetupProtocolStub::CT_LbsStep_SetupProtocolStub(CT_LbsServer& aParent) : CT_LbsStep(aParent)
	{
	SetTestStepName(KLbsStep_SetupProtocolStub);
	}

/**
Static Constructor
*/
CT_LbsStep_SetupProtocolStub* CT_LbsStep_SetupProtocolStub::New(CT_LbsServer& aParent)
	{
	return new CT_LbsStep_SetupProtocolStub(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}


/**
@pre 	
@return Test verdict
@post	GPSMode set to autonomous
*/
TVerdict CT_LbsStep_SetupProtocolStub::doTestStepL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsStep_SetupProtocolStub::doTestStepL()"));

	CLbsAdmin* admin = CLbsAdmin::NewL();
	CleanupStack::PushL(admin);
	
	_LIT(KHomeProtocolModuleUid, "HomeProtocolModuleUid");
	TPtrC testModuleIdPtr;
	TUint32 testModuleId;
	if (GetStringFromConfig(ConfigSection(), KHomeProtocolModuleUid, testModuleIdPtr))
		{
		TLex lex(testModuleIdPtr);
		User::LeaveIfError(lex.Val(testModuleId, EHex));
		INFO_PRINTF2(_L("&gt;&gt;HomeProtocolModuleUid read from .ini file section : %x"), testModuleId);		
		}
	else
		{
		// Set the Uid of the stub protocol module plugin
		// to be used for these unit tests.
		testModuleId = 0x10282293;
		INFO_PRINTF2(_L("&gt;&gt;No HomeProtocolModuleUid in .ini file section - defaulting to %d)"), testModuleId);		
		}
	
	User::LeaveIfError(admin->Set(KLbsSettingHomeProtocolModule, TUid::Uid(testModuleId)));
	
	//Also, set Admin to handle 10 simultaneous requests (default is 1)
	TUint maxNumberOfRequests = 10;
	User::LeaveIfError(admin->Set(KLbsSettingMaximumExternalLocateRequests, maxNumberOfRequests));
			
	CleanupStack::PopAndDestroy(admin);

	// Alow he setting to be propagated
	User::After(2000000);

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsStep_SetupProtocolStub::doTestStepL()"));

	return TestStepResult();
	}

