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
// Sets up the multiple PM stubs
// 
//

/**
 @file
 @internalTechnology
 @test
*/
		  
#include "ctlbsstepsetupmultipleprotocolstubs.h"
#include <lbs/lbsadmin.h>

CT_LbsStep_SetupMultipleProtocolStubs::CT_LbsStep_SetupMultipleProtocolStubs(CT_LbsServer& aParent) : CT_LbsStep(aParent)
	{
	SetTestStepName(KLbsStep_SetupMultipleProtocolStubs);
	}

/**
Static Constructor
*/
CT_LbsStep_SetupMultipleProtocolStubs* CT_LbsStep_SetupMultipleProtocolStubs::New(CT_LbsServer& aParent)
	{
	return new CT_LbsStep_SetupMultipleProtocolStubs(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}


/**
@pre 	
@return Test verdict
@post	GPSMode set to autonomous
*/
TVerdict CT_LbsStep_SetupMultipleProtocolStubs::doTestStepL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsStep_SetupMultipleProtocolStubs::doTestStepL()"));

	CLbsAdmin* admin = CLbsAdmin::NewL();
	CleanupStack::PushL(admin);

	TLbsAdminProtocolModulesInfo info;
	TLbsAdminProtocolModulesInfo::TLbsProtocolModuleIdArray uidArray;
	
	_LIT(KProtocolModuleUid, "ProtocolModuleUid%d");
	TBool found = ETrue;
	TInt numOfPms = 0;
	TInt currentPmId = 1;
	
	//Loop through all the Protocol Modules required
	while(found)
		{
		TPtrC testModuleIdPtr;
		TUint32 testModuleId;
		TBuf<25> pmIdName;
		pmIdName.AppendFormat(KProtocolModuleUid, currentPmId);
		currentPmId++;
		if (GetStringFromConfig(ConfigSection(), pmIdName, testModuleIdPtr))
			{
			TLex lex(testModuleIdPtr);
			User::LeaveIfError(lex.Val(testModuleId, EHex));
			uidArray[numOfPms] = TUid::Uid(testModuleId);
			numOfPms++;
			INFO_PRINTF2(_L("&gt;&gt;ProtocolModuleUid read from .ini file section : %x"), testModuleId);		
			}
		else
			{
			//Cannot find anymore PM's to add
			found = EFalse;
			}
		}
	
	info.SetModuleIds(uidArray,numOfPms);
	User::LeaveIfError(admin->Set(KLbsSettingHomeProtocolModule, info));
	
	//Also, set Admin to handle 10 simultaneous requests (default is 1)
	TUint maxNumberOfRequests = 10;
	User::LeaveIfError(admin->Set(KLbsSettingMaximumExternalLocateRequests, maxNumberOfRequests));
	
	CleanupStack::PopAndDestroy(admin);

	// Alow he setting to be propagated
	User::After(2000000);

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsStep_SetupMultipleProtocolStubs::doTestStepL()"));

	return TestStepResult();
	}

