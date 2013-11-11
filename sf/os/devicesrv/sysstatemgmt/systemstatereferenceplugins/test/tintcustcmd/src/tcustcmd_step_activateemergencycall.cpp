// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tcustcmd_step_activateemergencycall.h"
#include "tcustcmd_const.h"

CCustomCmdTestActivateEmergencyCall::~CCustomCmdTestActivateEmergencyCall()
	{
	delete iAsyncStopScheduler;
	delete iActiveSchedulerWait;
	delete iActiveScheduler;
	}

CCustomCmdTestActivateEmergencyCall::CCustomCmdTestActivateEmergencyCall()
	{
	SetTestStepName(KTCCustomCmdTestActivateEmergencyCall);
	}

TVerdict CCustomCmdTestActivateEmergencyCall::doTestStepPreambleL()
	{
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install (iActiveScheduler);

	iActiveSchedulerWait = new(ELeave) CActiveSchedulerWait;

	iAsyncStopScheduler = new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
	
	return TestStepResult();
	}
			
TVerdict CCustomCmdTestActivateEmergencyCall::doTestStepL()
	{
	INFO_PRINTF1(_L("doTestStepL"));
	TInt err = 0;
	__UHEAP_MARK;
	
    err = RProperty::Define(KPropertyCategory, KEmergencyCallRfAdaptationPluginPropertyKey, RProperty::EInt);
    TEST(KErrNone == err || KErrAlreadyExists == err);
    INFO_PRINTF2(_L("RProperty::Define(KPropertyCategory, KEmergencyCallRfAdaptationPluginPropertyKey, RProperty::EInt); returns %d"), err);
    err = RProperty::Set(KPropertyCategory, KEmergencyCallRfAdaptationPluginPropertyKey, 1);
    TEST(KErrNone == err); 
    INFO_PRINTF2(_L("RProperty::Set(KPropertyCategory, KEmergencyCallRfAdaptationPluginPropertyKey, 1); returns %d"), err);
    
	TRAP(err,TestHandleActivateEmergencyCallL());	
	TEST(KErrNone == err);
	TRAP(err,TestHandleActivateEmergencyCallCancelL());
	TEST(KErrNone == err);
	
	err = RProperty::Delete(KPropertyCategory, KEmergencyCallRfAdaptationPluginPropertyKey);
	TEST(KErrNone == err);
	INFO_PRINTF2(_L("RProperty::Delete(KPropertyCategory, KEmergencyCallRfAdaptationPluginPropertyKey); returns %d"), err);
	
	__UHEAP_MARKEND;
	return TestStepResult();
	}
/**
Old Test CaseID 		APPFWK-CUSTCMD-0021	
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0021
 */


void CCustomCmdTestActivateEmergencyCall::TestHandleActivateEmergencyCallL()
	{
	INFO_PRINTF1(_L("TestHandleActivateEmergencyCallL"));
	RSsmStateManager client;
	const TInt connect = client.Connect();
	TEST(KErrNone == connect);

	// Create swps
	INFO_PRINTF1(_L("Creating swp..."));
	TSsmSwp swp(KTestSwp, 11);
	RProperty property;
	TInt err = property.Define(KCustomcmdServerSID, swp.Key(), RProperty::EInt);
	TEST(KErrNone == err || KErrAlreadyExists == err);
	TEST(KErrNone == property.Set(KCustomcmdServerSID, swp.Key(), swp.Value()));
	
	//Ensure the environment is clean
	TInt value = 0;
	TEST(KErrNone == property.Get(KCustomcmdServerSID, swp.Key(), value));
	TESTE(11 == value, value);

	// Register mapping between keys and swp policy DLL (done once per ssmserver)
	err = client.RegisterSwpMapping(KTestSwp, KTestSwpPolicyCustomCmd);
	INFO_PRINTF2(_L("Registering swp...completed with % err"),err);
	
	//Let client request swp changes
	INFO_PRINTF1(_L("Request several swp changes"));
	swp.Set(KTestSwp, -99);
	TRequestStatus status;
	client.RequestSwpChange(swp, status);				
	TEST(status == KRequestPending);
	User::WaitForRequest(status);
	User::After(500000); // Wait for 0.5sec to allow transitions to fully complete
	TEST(status.Int() == KErrNone);	
	// clean-up
	TEST(KErrNone == property.Delete(KCustomcmdServerSID, swp.Key()));
	property.Close();
	client.Close();
	INFO_PRINTF3(_L("TestHandleActivateEmergencyCallL completed with %d Expected %d"),status.Int(),KErrNone);
	}

/**
Old Test CaseID 		APPFWK-CUSTCMD-0022
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0021
 */

void CCustomCmdTestActivateEmergencyCall::TestHandleActivateEmergencyCallCancelL()
	{
	INFO_PRINTF1(_L("TestHandleActivateEmergencyCallCancelL"));
	RSsmStateManager client;
	const TInt connect = client.Connect();
	TEST(KErrNone == connect);

	// Create swps
	INFO_PRINTF1(_L("Creating swp..."));
	TSsmSwp swp(KTestSwp, 11);
	RProperty property;
	
	TInt ret = property.Define(KCustomcmdServerSID, swp.Key(), RProperty::EInt);
	TEST(KErrNone == ret || KErrAlreadyExists == ret);

	TEST(KErrNone == property.Set(KCustomcmdServerSID, swp.Key(), swp.Value()));
	
	//Ensure the environment is clean
	TInt value = 0;
	TEST(KErrNone == property.Get(KCustomcmdServerSID, swp.Key(), value));
	TESTE(11 == value, value);


	//Let client request swp changes
	INFO_PRINTF1(_L("Request several swp changes "));
	swp.Set(KTestSwp, -99);
	TRequestStatus status;
	client.RequestSwpChange(swp, status);				
	TEST(status == KRequestPending);
	client.RequestSwpChangeCancel();
	User::WaitForRequest(status);
	TEST(status.Int() == KErrCancel);
	
	// clean-up
	TEST(KErrNone == property.Delete(KCustomcmdServerSID, swp.Key()));
	property.Close();
	client.Close();
	INFO_PRINTF3(_L("TestHandleActivateEmergencyCallCancelL completed with %d Expected %d"),status.Int(),KErrCancel);
	}
			
TVerdict CCustomCmdTestActivateEmergencyCall::doTestStepPostambleL()
	{	
	return TestStepResult();
	}

				
