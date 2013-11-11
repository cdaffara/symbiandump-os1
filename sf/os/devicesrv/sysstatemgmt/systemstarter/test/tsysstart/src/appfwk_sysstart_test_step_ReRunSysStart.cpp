// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include "appfwk_sysstart_test_step_ReRunSysStart.h"
#include "appfwk_sysstart_test_consts.h"

/**
Old Test CaseID 		APPFWK-SYSSTART-0079
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0079
 */


/**
   Destructor
 */
CAppfwkSysStartTestStepReRunSysStart::~CAppfwkSysStartTestStepReRunSysStart()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepReRunSysStart::CAppfwkSysStartTestStepReRunSysStart()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepReRunSysStart);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepReRunSysStart::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0079: ReRunSysStart - Started"));

 	__UHEAP_MARK;
 	
	TestReRunSysStartL();

	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("APPFWK-SYSSTART-0079: ReRunSysStart - Finished"));
	
	return TestStepResult();
	}

void CAppfwkSysStartTestStepReRunSysStart::TestReRunSysStartL()
	{
	SYSSTART_NEGATIVE_PLATSEC_START;
	TBool havePwrMgmt = RProcess().HasCapability(ECapabilityPowerMgmt,__PLATSEC_DIAGNOSTIC_STRING("Making sure that this thread does not have PowerMgmt capability"));
	SYSSTART_NEGATIVE_PLATSEC_FINISH;
	TEST(!havePwrMgmt);
	RProcess ws;
	_LIT(KSystemStarterName, "z:\\sys\\bin\\SYSSTART.EXE");
	TInt r=ws.Create(KSystemStarterName, KNullDesC);
	TEST(r==KErrNone);
	User::LeaveIfError(r);

	TRequestStatus logon;
	ws.Logon(logon);
	SYSSTART_NEGATIVE_PLATSEC_START;
	ws.Resume();
	SYSSTART_NEGATIVE_PLATSEC_FINISH;

	RTimer timer;
	User::LeaveIfError(timer.CreateLocal());
	TRequestStatus timerStatus;
	timer.After(timerStatus,30000000); // 30 seconds

	User::WaitForRequest(logon,timerStatus);
	
	TEST(logon != KRequestPending);
	if(logon != KRequestPending)
		{
		timer.Cancel();
		User::WaitForRequest(timerStatus);
		TEST(ws.ExitReason() == KErrPermissionDenied);
		}
	else
		{
		ws.LogonCancel(logon);
		User::WaitForRequest(logon);
		}
	ws.Close();
	}
