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

#include "tcustcmd_step_devicesecuritypincheck.h"
#include <ssm/ssmuiproviderdll.h>
#include <w32std.h>
#include <s32file.h>

static TInt CustomCmdTestDeviceSecurityCheckCallBackL(TAny* aCustomCmdTestDeviceSecurityPinCheck)
	{
	CCustomCmdTestDeviceSecurityPinCheck* test = reinterpret_cast<CCustomCmdTestDeviceSecurityPinCheck*>(aCustomCmdTestDeviceSecurityPinCheck);
	CleanupStack::PushL(test);
	test->CallBackForEnterPin();
	CleanupStack::Pop(); 
	return KErrNone;
	}
	
/*
Call back function to enter a PIN 
*/
void CCustomCmdTestDeviceSecurityPinCheck::CallBackForEnterPin()
	{
	iAsyncStopScheduler->CallBack();
	
	RWsSession wsSession;
	wsSession.Connect();

	TRawEvent eventDown;
	TRawEvent eventUp;
	
	eventDown.Set(TRawEvent::EKeyDown, EStdKeyComma);
	UserSvr::AddEvent(eventDown);
	eventUp.Set(TRawEvent::EKeyUp, EStdKeyComma);
	UserSvr::AddEvent(eventUp);
	User::After(100000);

	eventDown.Set(TRawEvent::EKeyDown, EStdKeyEnter);
	UserSvr::AddEvent(eventDown);
	eventUp.Set(TRawEvent::EKeyUp, EStdKeyEnter);
	UserSvr::AddEvent(eventUp);
	User::After(100000);
	
	wsSession.Flush();
	wsSession.Close();
	}
	
void CCustomCmdTestDeviceSecurityPinCheck::StopScheduler()
	{
	//Stop the active scheduler
	iActiveSchedulerWait->AsyncStop();
	}

CCustomCmdTestDeviceSecurityPinCheck::~CCustomCmdTestDeviceSecurityPinCheck()
	{
	delete iAsyncStopScheduler;
	delete iActiveSchedulerWait;
	delete iActiveScheduler;
	delete iStopSimulatekey;
	}

CCustomCmdTestDeviceSecurityPinCheck::CCustomCmdTestDeviceSecurityPinCheck():iSwp(TSsmSwp(KTestSwp, 0))
	{
	SetTestStepName(KTCCustomCmdTestDeviceSecurityPinCheck);
	}

TVerdict CCustomCmdTestDeviceSecurityPinCheck::doTestStepPreambleL()
	{
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install (iActiveScheduler);

	iActiveSchedulerWait = new(ELeave) CActiveSchedulerWait;

	iAsyncStopScheduler = new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
	
	//Start the test exe which defines startup related property keys
	RProcess processHandle;
	
	TInt temp = processHandle.Create(KExeToDefineStartUpPS, KNullDesC);
	TRequestStatus status1;
	processHandle.Rendezvous(status1);
	processHandle.Resume();
	User::WaitForRequest(status1);
	processHandle.Close();
		
	processHandle.Create(KExeToDefineStartUpStatePS, KNullDesC);
    TRequestStatus status2;
    processHandle.Rendezvous(status2);
    processHandle.Resume();
    User::WaitForRequest(status2);
	processHandle.Close();

	//For stopping the key simulation
	iStopSimulatekey = CStopSimulateDevicekey::NewL(this);
 	
 	//Connect to SsmStateManager
 	const TInt connect = iClient.Connect();
	TEST(KErrNone == connect);
	
	//RProperty property;
	iProperty.Define(KCustomcmdServerSID, iSwp.Key(), RProperty::EInt);
	
	// Register mapping between keys and swp policy DLL (done once per ssmserver)
	INFO_PRINTF1(_L("Registering swp..."));
	iClient.RegisterSwpMapping(KTestSwp, KTestSwpPolicyCustomCmd);
	
	RegisterDeviceSecurityStatus();
	return TestStepResult();
	}
	
void CCustomCmdTestDeviceSecurityPinCheck::RegisterDeviceSecurityStatus()
	{
	INFO_PRINTF1(_L("RegisterDeviceSecurityStatus"));
	
	_LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
	//Define the property for sim status for swp.
	TInt ret = RProperty::Define(RProcess().SecureId(), CSsmUiSpecific::SimStatusPropertyKey(), RProperty::EInt,KAllowAllPolicy, KAllowAllPolicy);
	TEST(KErrNone == ret || KErrAlreadyExists == ret);


	const TUint KStartupDeviceSecurityStatusKey = {CSsmUiSpecific::SimStatusPropertyKey()};
	
	//register the swp mapping for Sim DeviceSecurity status with its swp policy
	ret = iClient.RegisterSwpMapping(KStartupDeviceSecurityStatusKey, KTestSimStatusSwpPolicy);
	TEST(KErrNone == ret || KErrAlreadyExists == ret);
	}
		
TVerdict CCustomCmdTestDeviceSecurityPinCheck::doTestStepL()
	{
	INFO_PRINTF1(_L("doTestStepL"));
	TInt err = 0;
	__UHEAP_MARK;

	TRAP(err,TestHandleSecCustCmdWithExeBehaviourWaitForSignal());
	TEST(KErrNone == err);
	TRAP(err,TestHandleSecCustCmdWithExeBehaviourDeferredWaitForSignal());	
	TEST(KErrNone == err);
	
	TRAP(err,TestHandleSecCustCmdWithExeBehaviourWaitForSignalCancel());
	TEST(KErrNone == err);	
	TRAP(err,TestHandleSecCustCmdWithExeBehaviourDeferredWaitForSignalCancel());	
	TEST(KErrNone == err);
	
	TRAP(err, TestOOMDevSecCustCmdWithExeBehaviourWFL());
	TEST(KErrNone == err);	
	TRAP(err, TestOOMDevSecCustCmdWithExeBehaviourDWFL());
	TEST(KErrNone == err);	
	
	return TestStepResult();
	}

/**
Old Test CaseID 		APPFWK-CUSTCMD-0021
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0026
 */
	
		
void CCustomCmdTestDeviceSecurityPinCheck::TestHandleSecCustCmdWithExeBehaviourWaitForSignal()	
	{
	INFO_PRINTF1(_L("TestHandleSecCustCmdWithExeBehaviourWaitForSignal"));	
	iExecutionBehaviour = ESsmWaitForSignal;
	iStopSimulatekey->Start();
	CreateAndExecuteCustCmd();
	StartSimulateKey();
	TEST(KErrNone == iStatus.Int());
	INFO_PRINTF3(_L("DeviceSecurity PIN check completed with %d, expected %d"),iStatus.Int(),KErrNone);
	}

/**
Old Test CaseID 		APPFWK-CUSTCMD-0026
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0026
 */

void CCustomCmdTestDeviceSecurityPinCheck::TestHandleSecCustCmdWithExeBehaviourWaitForSignalCancel()	
	{
	INFO_PRINTF1(_L("TestHandleSecCustCmdWithExeBehaviourWaitForSignalCancelL"));
	iExecutionBehaviour = ESsmWaitForSignal;
	iStopSimulatekey->Start();
	CreateAndExecuteCancelCustCmd();
	StartSimulateKey();
	TEST(KErrCancel == iCancelReqStatus.Int());
	RDebug::Print(_L("TestHandleDeviceSecCustCmdWithExeBehaviourWaitForSignalCancelL completed with %d, expected %d"),iCancelReqStatus.Int(),KErrCancel);	
	INFO_PRINTF3(_L("TestHandleDeviceSecCustCmdWithExeBehaviourWaitForSignalCancelL completed with %d, expected %d"),iCancelReqStatus.Int(),KErrCancel);	
	}
	
/**
Old Test CaseID 		APPFWK-CUSTCMD-0021
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0026
 */
	
void CCustomCmdTestDeviceSecurityPinCheck::TestHandleSecCustCmdWithExeBehaviourDeferredWaitForSignal()
	{
	INFO_PRINTF1(_L("TestHandleSecCustCmdWithExeBehaviourDeferredWaitForSignalL"));
	iExecutionBehaviour = ESsmDeferredWaitForSignal;
	iStopSimulatekey->Start();
	CreateAndExecuteCustCmd();
	StartSimulateKey();
	TEST(KErrNone == iStatus.Int());
	RDebug::Print(_L("TestHandleDeviceSecCustCmdWithExeBehaviourDeferredWaitForSignalL completed with %d, expected %d"),iStatus.Int(),KErrNone);	
	INFO_PRINTF3(_L("TestHandleDeviceSecCustCmdWithExeBehaviourDeferredWaitForSignalL completed with %d, expected %d"),iStatus.Int(),KErrNone);	
	}
	
/**
Old Test CaseID 		APPFWK-CUSTCMD-0026
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0026
 */

void CCustomCmdTestDeviceSecurityPinCheck::TestHandleSecCustCmdWithExeBehaviourDeferredWaitForSignalCancel()	
	{
	INFO_PRINTF1(_L("TestHandleSecCustCmdWithExeBehaviourDeferredWaitForSignalCancelL"));	
	iExecutionBehaviour = ESsmDeferredWaitForSignal;
	iStopSimulatekey->Start();
	CreateAndExecuteCancelCustCmd();
	StartSimulateKey();
	TEST(KErrCancel == iCancelReqStatus.Int());
	RDebug::Print(_L("TestHandleDeviceSecCustCmdWithExeBehaviourDeferredWaitForSignalCancelL completed with %d, expected %d"),iCancelReqStatus.Int(),KErrCancel);	
	INFO_PRINTF3(_L("TestHandleDeviceSecCustCmdWithExeBehaviourDeferredWaitForSignalCancelL completed with %d, expected %d"),iCancelReqStatus.Int(),KErrCancel);	
	}

/**
Old Test CaseID 		APPFWK-CUSTCMD-0028
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0026
 */

// check attempt to 
void CCustomCmdTestDeviceSecurityPinCheck::TestOOMDevSecCustCmdWithExeBehaviourWFL()
	{
	INFO_PRINTF1(_L("*** Starting TestOOMDevSecCustCmdwithExeBehaviourWF"));
	TInt failRate = 1;
	for (;;failRate++)
	    {
	    __UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
	    __UHEAP_MARK;

	    TRAPD(ret, TestHandleSecCustCmdWithExeBehaviourWaitForSignal());
	    	    
        TEST((ret==KErrNone || ret==KErrNoMemory));
        __UHEAP_MARKEND;

	    if (ret==KErrNone)
	        {
	        break;
	        }
	    }	
	__UHEAP_RESET;
	INFO_PRINTF2(_L(" Executed device securitypincheck custom command %d times."),failRate);        
	}
	
/**
Old Test CaseID 		APPFWK-CUSTCMD-0028
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0026
 */

void CCustomCmdTestDeviceSecurityPinCheck::TestOOMDevSecCustCmdWithExeBehaviourDWFL()
	{
	INFO_PRINTF1(_L("*** Starting TestOOMDevSecCustCmdWithExeBehaviourDWF"));
	TInt failRate = 1;
	for (;;failRate++)
	    {
	    __UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
	    __UHEAP_MARK;

	    TRAPD(ret, TestHandleSecCustCmdWithExeBehaviourDeferredWaitForSignal());
	    	    
        TEST((ret==KErrNone || ret==KErrNoMemory));
        __UHEAP_MARKEND;

	    if (ret==KErrNone)
	        {
	        break;
	        }
	    }
	__UHEAP_RESET;
	INFO_PRINTF2(_L(" Executed device securitypincheck custom command %d number of times."),failRate);    
	}
	
void CCustomCmdTestDeviceSecurityPinCheck::CreateAndExecuteCustCmd()
	{
	TInt swpValue = 0;
	switch(iExecutionBehaviour)
		{
		case ESsmWaitForSignal:
			swpValue = ERPropertyValForDeviceSecCustCmdWaitForSignal;
		break;
		case ESsmDeferredWaitForSignal:
			swpValue = ERPropertyValForDeviceSecCustCmdDeferrdWaitForSignal;	
		break;
		case ESsmFireAndForget:
			swpValue = ERPropertyValForDeviceSecCustCmdFireAndForget;
		break;
		default:
		break;
		}
	// Create swps
	INFO_PRINTF1(_L("Creating swp..."));
	//set this property to create appropriate command by ssm.swp.policy.test.customcmd.dll
	iSwp.Set(KTestSwp, swpValue);
	
	TEST(KErrNone == iProperty.Set(KCustomcmdServerSID, iSwp.Key(), iSwp.Value()));
	
	//Ensure the environment is clean
	TInt value = 0;
	TEST(KErrNone == iProperty.Get(KCustomcmdServerSID, iSwp.Key(), value));
	TESTE(swpValue == value, value);
	
	//Let client request swp changes
	INFO_PRINTF1(_L("Request several swp changes "));
	iSwp.Set(KTestSwp, -99);
	
	iClient.RequestSwpChange(iSwp, iStatus);		

	TEST(iStatus == KRequestPending);
	User::WaitForRequest(iStatus);
	User::After(500000); // Wait for 0.5sec to allow transitions to fully complete
	TEST(iStatus.Int() == KErrNone);	
	}
	
void CCustomCmdTestDeviceSecurityPinCheck::CreateAndExecuteCancelCustCmd()
	{
	TInt swpValue = 0;
		switch(iExecutionBehaviour)
		{
		case ESsmWaitForSignal:
			swpValue = ERPropertyValForDeviceSecCustCmdWaitForSignal;
		break;
		case ESsmDeferredWaitForSignal:
			swpValue = ERPropertyValForDeviceSecCustCmdDeferrdWaitForSignal;	
		break;
		case ESsmFireAndForget:
			swpValue = ERPropertyValForDeviceSecCustCmdFireAndForget;
		break;
		default:
		break;
		}
	// Create swps
	INFO_PRINTF1(_L("Creating swp..."));
	//set this property to create appropriate command by ssm.swp.policy.test.customcmd.dll
	iSwp.Set(KTestSwp, swpValue);	
	
	TEST(KErrNone == iProperty.Set(KCustomcmdServerSID, iSwp.Key(), iSwp.Value()));
	
	//Ensure the environment is clean
	TInt value = 0;
	TEST(KErrNone == iProperty.Get(KCustomcmdServerSID, iSwp.Key(), value));
	TESTE(swpValue == value, value);
	
	//Let client request swp changes
	INFO_PRINTF1(_L("Request several swp changes "));
	iSwp.Set(KTestSwp, -99);
	
	//Transition engine will no cancel any request that is currently in execution. It will delete only quad request.
	TRequestStatus status;
	iClient.RequestSwpChange(iSwp, iStatus);		
	iClient.RequestSwpChange(iSwp, iCancelReqStatus);			
	TEST(iStatus == KRequestPending);
	//User::After(5000); // Wait for 0.005sec to allow transitions to start
	User::WaitForRequest(iStatus);
	TEST(iStatus.Int() == KErrNone);
	iClient.RequestSwpChangeCancel();
	RDebug::Print(_L("CreateAndExecuteCancelCustCmdL completes with %d err"),iStatus.Int());
	}
			
TVerdict CCustomCmdTestDeviceSecurityPinCheck::doTestStepPostambleL()
	{
	TEST(KErrNone == iProperty.Delete(KCustomcmdServerSID, iSwp.Key()));
	iProperty.Close();
	iClient.Close();
	__UHEAP_MARKEND;
	return TestStepResult();
	}
	
void CCustomCmdTestDeviceSecurityPinCheck::StartSimulateKey()
	{
	//Callback function for simulating the key
	TCallBack stop(CustomCmdTestDeviceSecurityCheckCallBackL, this);
	if(!iAsyncStopScheduler->IsActive())
		{
		iAsyncStopScheduler->Set(stop);
		}
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
	}

//
/**
CStopSimulateDevicekey class is used for stoping the scheduler
*/
CStopSimulateDevicekey::~CStopSimulateDevicekey()
	{
	iProperty.Close();
	Cancel();
	}

CStopSimulateDevicekey::CStopSimulateDevicekey() : CActive(CActive::EPriorityHigh)  //Standard)
	{	
	CActiveScheduler::Add(this);
	}

CStopSimulateDevicekey* CStopSimulateDevicekey::NewL(CCustomCmdTestDeviceSecurityPinCheck* aCustomCmdTestDeviceSecurityPinCheck)
	{
	CStopSimulateDevicekey* self = new (ELeave) CStopSimulateDevicekey();
	CleanupStack::PushL(self);
	self->Construct(aCustomCmdTestDeviceSecurityPinCheck);
	CleanupStack::Pop();
	return self;
	}

void CStopSimulateDevicekey::Construct(CCustomCmdTestDeviceSecurityPinCheck* aCustomCmdTestDeviceSecurityPinCheck)
	{
	// Define a property for stopping active scheduler 
 	TInt r = RProperty::Define(KCustomcmdServerPropertyKey, RProperty::EInt,ECapability_None, ECapability_None);								
	iCustomCmdTestDeviceSecurityPinCheck = aCustomCmdTestDeviceSecurityPinCheck;
	}
		
void CStopSimulateDevicekey::Start()
	{  
	iProperty.Attach(KCustomcmdServerSID,KCustomcmdServerPropertyKey);
	iProperty.Subscribe(iStatus);
	SetActive();				
	}	
 
void CStopSimulateDevicekey::RunL()
	{	
	TInt value;
	TInt err = RProperty::Get(RProcess().SecureId(),KCustomcmdServerPropertyKey,value);	
	//value will be set in policy (tcustcmd_swppolicy_customcmd) in the function HandleCleReturnValue() 
	if( value == ERPropertyValForStopSchedulerCmdDeferredWaitForSignal || value == ERPropertyValForStopSchedulerCmdWaitForSignal)
		{
		RDebug::Printf("CStopSimulateDevicekey::RunL() ExecutionBehaviour = ESsmDeferredWaitForSignal");
		iCustomCmdTestDeviceSecurityPinCheck->StopScheduler();		
		}
	else
		{
		Start();
		}		
	}
	
void CStopSimulateDevicekey::DoCancel()
	{
	iProperty.Cancel();
	}

					
