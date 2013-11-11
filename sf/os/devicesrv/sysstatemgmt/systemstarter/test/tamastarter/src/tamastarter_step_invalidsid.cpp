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
// NOTE: Unlike tamastarter_standalone test cases, where the correct SID is required, here we're testing that AmaStarter.exe
// can't launch if the SID is not the expected one.
// To achieve that, we don't have to do anything, as the default tamastarter_server.exe SID is not the expected KSysStartSID	
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include "appfwk_test_utils.h"
#include "tamastarter_constants.h"
#include "tamastarter_step_invalidsid.h"


_LIT(KAmaStarterTest,"amastarter.exe");


//----------------------


void CAppfwkAmaStarterTestStepInvalidSid::LaunchExe(const TDesC& aFilename, const TDesC& aArguments, TInt aExitReason)
	{
	RProcess process;
	TInt ret = process.Create(aFilename, aArguments, EOwnerThread);
	TEST(ret == KErrNone);
	
	TRequestStatus	status;
	process.Logon(status);
	User::SetJustInTime(EFalse);
	process.Resume();
	User::WaitForRequest(status);
	User::SetJustInTime(ETrue);
	TInt exitReason = process.ExitReason();
	process.Close();
	
	// Check the process ended with the expected exit reason
	INFO_PRINTF3(_L("Process finished with ExitReason '%d' (expected was '%d')"), exitReason, aExitReason);
	TEST(exitReason == aExitReason);
	}



//---------

CAppfwkAmaStarterTestStepInvalidSid::CAppfwkAmaStarterTestStepInvalidSid(const TDesC& aStepName)
	{	
	SetTestStepName(aStepName);
	}


CAppfwkAmaStarterTestStepInvalidSid::~CAppfwkAmaStarterTestStepInvalidSid()
	{	
	}

/**
Old Test CaseID 		APPFWK-AMASTARTER-0009
New Test CaseID 		DEVSRVS-SYSSTART-AMASTARTER-0009
 */

TVerdict CAppfwkAmaStarterTestStepInvalidSid::doTestStepL()
	{	
	INFO_PRINTF1(_L("AMAStarter Invalid SID tests started...."));

 	__UHEAP_MARK;
 	
	LaunchExe(KAmaStarterTest, KNullDesC, KErrPermissionDenied);
	
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("...AMAStarter Invalid SID tests finished!!"));

	return TestStepResult();
	}

