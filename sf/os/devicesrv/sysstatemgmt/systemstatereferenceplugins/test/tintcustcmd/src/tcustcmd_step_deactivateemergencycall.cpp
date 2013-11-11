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

#include "tcustcmd_step_deactivateemergencycall.h"
#include "tcustcmd_const.h"


CCustomCmdTestDeActivateEmergencyCall::~CCustomCmdTestDeActivateEmergencyCall()
	{
	delete iAsyncStopScheduler;
	delete iActiveSchedulerWait;
	delete iActiveScheduler;
	}

CCustomCmdTestDeActivateEmergencyCall::CCustomCmdTestDeActivateEmergencyCall()
	{
	SetTestStepName(KTCCustomCmdTestDeActivateEmergencyCall);
	}

TVerdict CCustomCmdTestDeActivateEmergencyCall::doTestStepPreambleL()
	{
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install (iActiveScheduler);

	iActiveSchedulerWait = new(ELeave) CActiveSchedulerWait;

	iAsyncStopScheduler = new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
	
	return TestStepResult();
	}
		
TVerdict CCustomCmdTestDeActivateEmergencyCall::doTestStepL()
	{
	INFO_PRINTF1(_L("doTestStepL"));
	TInt err = 0;
	__UHEAP_MARK;
	
	TInt res = RProperty::Define(KPropertyCategory, KEmergencyCallRfAdaptationPluginPropertyKey, RProperty::EInt);
    INFO_PRINTF2(_L("RProperty::Define(KPropertyCategory, KEmergencyCallRfAdaptationPluginPropertyKey, RProperty::EInt); returns %d"), res);
    res = RProperty::Set(KPropertyCategory, KEmergencyCallRfAdaptationPluginPropertyKey, 1);
    INFO_PRINTF2(_L("RProperty::Set(KPropertyCategory, KEmergencyCallRfAdaptationPluginPropertyKey, 1); returns %d"), res);

	TRAP(err,TestHandleDeActivateEmergencyCallL());
	TEST(KErrNone == err);
	TRAP(err,TestHandleDeActivateEmergencyCallCancelL());
	TEST(KErrNone == err);
	
	res = RProperty::Delete(KPropertyCategory, KEmergencyCallRfAdaptationPluginPropertyKey);
    INFO_PRINTF2(_L("RProperty::Delete(KPropertyCategory, KEmergencyCallRfAdaptationPluginPropertyKey); returns %d"), res);

	return TestStepResult();
	}

/**
Old Test CaseID 		APPFWK-CUSTCMD-0023
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0023
 */

void CCustomCmdTestDeActivateEmergencyCall::TestHandleDeActivateEmergencyCallL()
	{
	INFO_PRINTF1(_L("TestHandleDeActivateEmergencyCallL"));
	RSsmStateManager client;
	const TInt connect = client.Connect();
	TEST(KErrNone == connect);

	// Create swps
	INFO_PRINTF1(_L("Creating swp..."));
	TSsmSwp swp(KTestSwp, 22);
	RProperty property;
	TInt ret = property.Define(KCustomcmdServerSID, swp.Key(), RProperty::EInt);
	TEST(KErrNone == ret || KErrAlreadyExists == ret);

	TEST(KErrNone == property.Set(KCustomcmdServerSID, swp.Key(), swp.Value()));
	
	//Ensure the environment is clean
	TInt value = 0;
	TEST(KErrNone == property.Get(KCustomcmdServerSID, swp.Key(), value));
	TESTE(22 == value, value);

	// Register mapping between keys and swp policy DLL (done once per ssmserver)
	INFO_PRINTF1(_L("Registering swp..."));
	ret = client.RegisterSwpMapping(KTestSwp, KTestSwpPolicyCustomCmd);
	TEST(KErrNone == ret || KErrAlreadyExists == ret);

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
	INFO_PRINTF3(_L("TestHandleDeActivateEmergencyCallL completed with %d, expected %d"),status.Int(),KErrNone);	
	}

/**
Old Test CaseID 		APPFWK-CUSTCMD-0024
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0023
 */

void CCustomCmdTestDeActivateEmergencyCall::TestHandleDeActivateEmergencyCallCancelL()
	{
	INFO_PRINTF1(_L("TestHandleDeActivateEmergencyCallCancelL"));
	RSsmStateManager client;
	const TInt connect = client.Connect();
	TEST(KErrNone == connect);

	// Create swps
	INFO_PRINTF1(_L("Creating swp..."));
	TSsmSwp swp(KTestSwp, 22);
	RProperty property;
	TInt ret = property.Define(KCustomcmdServerSID, swp.Key(), RProperty::EInt);
	TEST(KErrNone == ret || KErrAlreadyExists == ret);
	TEST(KErrNone == property.Set(KCustomcmdServerSID, swp.Key(), swp.Value()));
	
	//Ensure the environment is clean
	TInt value = 0;
	TEST(KErrNone == property.Get(KCustomcmdServerSID, swp.Key(), value));
	TESTE(22 == value, value);

	//Let client request swp changes
	INFO_PRINTF1(_L("Request several swp changes "));
	swp.Set(KTestSwp, -99);
	TRequestStatus status;
	client.RequestSwpChange(swp, status);		
	TEST(status == KRequestPending);
	client.RequestSwpChangeCancel();
	User::WaitForRequest(status);
	TEST(status.Int() == KErrCancel);
	INFO_PRINTF3(_L("TestHandleDeActivateEmergencyCallCancelL completed with %d, expected %d"),status.Int(),KErrCancel);	
	// clean-up
	TEST(KErrNone == property.Delete(KCustomcmdServerSID, swp.Key()));
	property.Close();
	client.Close();
	
	}
		


TVerdict CCustomCmdTestDeActivateEmergencyCall::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;
	return TestStepResult();
	}

