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

#include "tcustcmd_step_simsecuritypincheck.h"
#include <ssm/ssmuiproviderdll.h>
#include <ssm/startupdomainpskeys.h>
#include <w32std.h>
#include <s32file.h>

const TInt KNumOfTestCases = 8;
const TUint32 KMiscPluginPropertyKey = 0x2000E658;

static TInt CustomCmdTestSecurityCheckCallBackL(TAny* aCustomCmdTestSecurityPinCheck)
	{
	CCustomCmdTestSecurityPinCheck* test = reinterpret_cast<CCustomCmdTestSecurityPinCheck*>(aCustomCmdTestSecurityPinCheck);
	CleanupStack::PushL(test);
	test->CallBackForEnterPin();
	CleanupStack::Pop(); 
	return KErrNone;
	}
	
/*
Call back function to enter a PIN 
*/
void CCustomCmdTestSecurityPinCheck::CallBackForEnterPin()
	{
	const TInt okButtonPos1 = 60; //the position of ok button
	const TInt okButtonPos2 = 600; //the position of ok button
	iAsyncStopScheduler->CallBack();
	
	RWsSession wsSession;
	wsSession.Connect();

	TRawEvent eventDown;
	TRawEvent eventUp;
	
	if(iValidPin)
		{
		eventDown.Set(TRawEvent::EKeyDown, EStdKeyComma);
		UserSvr::AddEvent(eventDown);
		eventUp.Set(TRawEvent::EKeyUp, EStdKeyComma);
		UserSvr::AddEvent(eventUp);
		User::After(100000);
		}
	
	eventDown.Set(TRawEvent::EButton1Down, okButtonPos1,okButtonPos2);
	UserSvr::AddEvent(eventDown);
	eventUp.Set(TRawEvent::EButton1Up, okButtonPos1,okButtonPos2);
	UserSvr::AddEvent(eventUp);
	User::After(100000);
		
	wsSession.Flush();
	wsSession.Close();
	}
	
void CCustomCmdTestSecurityPinCheck::StopScheduler()
	{
	//Stop the active scheduler
	RDebug::Printf("CCustomCmdTestSecurityPinCheck::StopScheduler()");	
	iActiveSchedulerWait->AsyncStop();
	}

CCustomCmdTestSecurityPinCheck::~CCustomCmdTestSecurityPinCheck()
	{
	delete iAsyncStopScheduler;
	delete iActiveSchedulerWait;
	delete iActiveScheduler;
	delete iStopSimulatekey;
	}

CCustomCmdTestSecurityPinCheck::CCustomCmdTestSecurityPinCheck():iSwp(TSsmSwp(KTestSwp, 0))
	{
	SetTestStepName(KTCCustomCmdTestSecurityPinCheck);
	}

TVerdict CCustomCmdTestSecurityPinCheck::doTestStepPreambleL()
	{
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install (iActiveScheduler);

	iActiveSchedulerWait = new(ELeave) CActiveSchedulerWait;

	iAsyncStopScheduler = new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
	
	TInt err = RProperty::Define(KPropertyCategory, KMiscPluginPropertyKey, RProperty::EInt);
	TEST ((KErrNone == err) || (KErrAlreadyExists == err));
	err = RProperty::Set(KPropertyCategory, KMiscPluginPropertyKey, 1);
	TEST (KErrNone == err);
	
	//Start the test exe which defines startup related property keys
	RProcess processHandle;
	CleanupClosePushL(processHandle);
	processHandle.Create(KExeToDefineStartUpPS, KNullDesC);
	processHandle.Resume();
	
	// wait for the newly created process to rendezvous
	TRequestStatus status;
	processHandle.Rendezvous(status);
	User::WaitForRequest(status);
	
	TInt retVal = status.Int();
	TEST(KErrNone == retVal);
	// leave if the process has not started properly
	User::LeaveIfError(retVal);
	CleanupStack::PopAndDestroy();
	
	//Start the test exe which defines startup state related property keys
	RProcess processHandle1;
	CleanupClosePushL(processHandle1);
	err = processHandle1.Create(KExeToDefineStartUpStatePS, KNullDesC);
	INFO_PRINTF2(_L("KExeToDefineStartUpStatePS ret is %d"), err);
	User::LeaveIfError(err);
	processHandle1.Resume();
	
	// wait for the newly created process to rendezvous
	processHandle1.Rendezvous(status);
	User::WaitForRequest(status);
	retVal = status.Int();
	//leave if the process has not started properly
	INFO_PRINTF2(_L("KExeToDefineStartUpStatePS rendezvous returns %d"), retVal);
    User::LeaveIfError(retVal);
    CleanupStack::PopAndDestroy();

	//there are 8 different scenarios to test the custom command. Check the test spec for the scenarios.
 	//CMiscAdaptationRef::SecurityStateChange() has been changed to simulate the scenarios.
	//CMiscAdaptationRef::SecurityStateChange() uses KTestCmdSecurityCheckTestFile to get the scenario number
	//connect to file server

    User::LeaveIfError(iFs.Connect());
	err = iFs.MkDirAll(KDirNameOfTestCasesNumFile);
	if (KErrAlreadyExists != err && KErrNone != err)
		{
		User::Leave(err);
		}
	err = iFile.Replace(iFs, KTestCmdSecurityCheckTestFile, EFileWrite | EFileStream);
	TEST(KErrNone == err);
	iFile.Close();
	
	//For stopping the key simulation when execution behaviour is deferredwaitforsignal and fireandforget
	iStopSimulatekey = CStopSimulatekey::NewL(this);
 	
 	//Connect to SsmStateManager
 	const TInt connect = iClient.Connect();
	TEST(KErrNone == connect);
	
	//RProperty property;
	err = iProperty.Define(KCustomcmdServerSID, iSwp.Key(), RProperty::EInt);
	TEST ((KErrNone == err) || (KErrAlreadyExists == err));
	
	// Register mapping between keys and swp policy DLL (done once per ssmserver)
	INFO_PRINTF1(_L("Registering swp..."));
	iClient.RegisterSwpMapping(KTestSwp, KTestSwpPolicyCustomCmd);
	
	RegisterSimSecurityStatus();
	return TestStepResult();
	}
	
void CCustomCmdTestSecurityPinCheck::RegisterSimSecurityStatus()
	{
	INFO_PRINTF1(_L("RegisterSimSecurityStatus"));
	
	_LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
	//Define the property for sim status for swp.
	TInt ret = RProperty::Define(RProcess().SecureId(), CSsmUiSpecific::SimStatusPropertyKey(), RProperty::EInt,KAllowAllPolicy, KAllowAllPolicy);
	TEST(KErrNone == ret || KErrAlreadyExists == ret);


	const TUint KStartupSimSecurityStatusKey = {CSsmUiSpecific::SimStatusPropertyKey()};
	
	//register the swp mapping for Sim security status with its swp policy
	ret = iClient.RegisterSwpMapping(KStartupSimSecurityStatusKey, KTestSimStatusSwpPolicy);
	TEST(KErrNone == ret || KErrAlreadyExists == ret);
	}
		
TVerdict CCustomCmdTestSecurityPinCheck::doTestStepL()
	{
	INFO_PRINTF1(_L("doTestStepL"));
	TInt err = 0;
	__UHEAP_MARK;
	
	iCancelRequest = EFalse;
	TRAP(err,TestHandleSecCustCmdWithExeBehaviourFireAndForgetL());
	TEST(KErrNone == err);
	TRAP(err,TestHandleSecCustCmdWithExeBehaviourDeferredWaitForSignalL());
	TEST(KErrNone == err);
	TRAP(err,TestHandleSecCustCmdWithExeBehaviourWaitForSignalL());	
	TEST(KErrNone == err);

	iCancelRequest = ETrue;
	TRAP(err,TestHandleSecCustCmdWithExeBehaviourWaitForSignalCancelL());
	TEST(KErrNone == err);
	TRAP(err,TestHandleSecCustCmdWithExeBehaviourDeferredWaitForSignalCancelL());	
	TEST(KErrNone == err);	
	
	TRAP(err,TestOOMDevSecCustCmdwithExeBehaviourDWFL());
	TEST(KErrNone == err);	

    return TestStepResult();
	}
	
/**
Old Test CaseID 		APPFWK-CUSTCMD-0019
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0019
 */

void CCustomCmdTestSecurityPinCheck::TestHandleSecCustCmdWithExeBehaviourWaitForSignalL()	
	{
	TInt testNum;	
	iExecutionBehaviour = ESsmWaitForSignal;
	iValidPin = ETrue;
	INFO_PRINTF1(_L("TestHandleSecCustCmdWithExeBehaviourWaitForSignal"));	
	for(testNum = 1; testNum <= KNumOfTestCases; testNum++)
		{
		INFO_PRINTF2(_L("EPINCHECK%d started"),testNum);
		WritePinCheckTestCaseNumToFileL(testNum);
		iStopSimulatekey->Start();
		CreateAndExecuteCustCmd();
		StartSimulateKey();
		TEST(KErrNone == iStatus.Int());
		INFO_PRINTF3(_L("Security PIN check completed with %d, expected %d"),iStatus.Int(),KErrNone);
		}
	INFO_PRINTF1(_L("Test security custom command for a invalid PIN "));		
	iValidPin = EFalse;
	for(testNum = 1; testNum < KNumOfTestCases; testNum++)
		{
		INFO_PRINTF2(_L("EPINCHECK%d started"),testNum);
		WritePinCheckTestCaseNumToFileL(testNum);
		iStopSimulatekey->Start();
		CreateAndExecuteCustCmd();
		StartSimulateKey();
		TEST(KErrNone == iStatus.Int());
		INFO_PRINTF3(_L("Security PIN check completed with %d, expected %d"),iStatus.Int(),KErrNone);
		}
	INFO_PRINTF1(_L("TestHandleSecCustCmdWithExeBehaviourWaitForSignal completed"));		
	}

/**
Old Test CaseID 		APPFWK-CUSTCMD-0027
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0019
 */

void CCustomCmdTestSecurityPinCheck::TestHandleSecCustCmdWithExeBehaviourWaitForSignalCancelL()	
	{
	INFO_PRINTF1(_L("TestHandleSecCustCmdWithExeBehaviourWaitForSignalCancelL"));
	TInt testNum = 1;
	iValidPin = ETrue;
	WritePinCheckTestCaseNumToFileL(testNum);
	iExecutionBehaviour = ESsmWaitForSignal;
	iStopSimulatekey->Start();
	CreateAndExecuteCancelCustCmd();
	StartSimulateKey();
	TEST(KErrCancel == iCancelReqStatus.Int());
	RDebug::Print(_L("TestHandleSecCustCmdWithExeBehaviourWaitForSignalCancelL completed with %d, expected %d"),iCancelReqStatus.Int(),KErrCancel);	
	INFO_PRINTF3(_L("TestHandleSecCustCmdWithExeBehaviourWaitForSignalCancelL completed with %d, expected %d"),iCancelReqStatus.Int(),KErrCancel);	
	}

/**
Old Test CaseID 		APPFWK-CUSTCMD-0019
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0019
 */

void CCustomCmdTestSecurityPinCheck::TestHandleSecCustCmdWithExeBehaviourDeferredWaitForSignalL()
	{
	INFO_PRINTF1(_L("TestHandleSecCustCmdWithExeBehaviourDeferredWaitForSignalL"));
	TInt testNum = 1;
	iValidPin = ETrue;
	WritePinCheckTestCaseNumToFileL(testNum);
	iExecutionBehaviour = ESsmDeferredWaitForSignal;
	iStopSimulatekey->Start();
	CreateAndExecuteCustCmd();
	StartSimulateKey();
	TEST(KErrNone == iStatus.Int());
	RDebug::Print(_L("TestHandleSecCustCmdWithExeBehaviourDeferredWaitForSignalL completed with %d, expected %d"),iStatus.Int(),KErrNone);	
	INFO_PRINTF3(_L("TestHandleSecCustCmdWithExeBehaviourDeferredWaitForSignalL completed with %d, expected %d"),iStatus.Int(),KErrNone);	
	}

/**
Old Test CaseID 		APPFWK-CUSTCMD-0027
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0019
 */

void CCustomCmdTestSecurityPinCheck::TestHandleSecCustCmdWithExeBehaviourDeferredWaitForSignalCancelL()	
	{
	INFO_PRINTF1(_L("TestHandleSecCustCmdWithExeBehaviourDeferredWaitForSignalCancelL"));	
	TInt testNum = 1;
	iValidPin = ETrue;
	WritePinCheckTestCaseNumToFileL(testNum);
	iExecutionBehaviour = ESsmDeferredWaitForSignal;
	iStopSimulatekey->Start();
	CreateAndExecuteCancelCustCmd();
	StartSimulateKey();
	TEST(KErrCancel == iCancelReqStatus.Int());
	RDebug::Print(_L("TestHandleSecCustCmdWithExeBehaviourDeferredWaitForSignalCancelL completed with %d, expected %d"),iCancelReqStatus.Int(),KErrCancel);	
	INFO_PRINTF3(_L("TestHandleSecCustCmdWithExeBehaviourDeferredWaitForSignalCancelL completed with %d, expected %d"),iCancelReqStatus.Int(),KErrCancel);	
	}
	
/**
Old Test CaseID 		APPFWK-CUSTCMD-0019
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0019
 */

void CCustomCmdTestSecurityPinCheck::TestHandleSecCustCmdWithExeBehaviourFireAndForgetL()
	{
	INFO_PRINTF1(_L("TestHandleSecCustCmdWithExeBehaviourFireAndForgetL"));	
	TInt testNum = 1;
	iValidPin = ETrue;
	WritePinCheckTestCaseNumToFileL(testNum);
	iExecutionBehaviour = ESsmFireAndForget;
	iStopSimulatekey->Start();
	CreateAndExecuteCustCmd();
	StartSimulateKey();
	TEST(KErrNone == iStatus.Int());
	RDebug::Print(_L("TestHandleSecCustCmdWithExeBehaviourFireAndForgetL completed with %d, expected %d"),iStatus.Int(),KErrNone);	
	INFO_PRINTF3(_L("TestHandleSecCustCmdWithExeBehaviourFireAndForgetL completed with %d, expected %d"),iStatus.Int(),KErrNone);	
	}
	
/**
Old Test CaseID 		APPFWK-CUSTCMD-0025
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0019
 */

void CCustomCmdTestSecurityPinCheck::TestOOMDevSecCustCmdwithExeBehaviourDWFL()
	{
	INFO_PRINTF1(_L("*** Starting TestOOMDevSecCustCmdwithExeBehaviourDWF"));
	TInt failRate = 1;
	for (;;failRate++)
	    {
	    __UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
	    __UHEAP_MARK;

	    TRAPD(ret, TestHandleSecCustCmdWithExeBehaviourDeferredWaitForSignalL());
	    	    
        TEST((ret==KErrNone || ret==KErrNoMemory));
        __UHEAP_MARKEND;

	    if (ret==KErrNone)
	        {
	        break;
	        }
	    }
	__UHEAP_RESET;
	INFO_PRINTF2(_L(" Executed securitypincheck custom command %d number of times."),failRate);    
	}
		
void CCustomCmdTestSecurityPinCheck::CancelSwpRequest()
	{
	iStopSimulatekey->Start();
	if(iCancelRequest)
		{
		iClient.RequestSwpChangeCancel();
		}
	RDebug::Printf("Cancel Swp Request");

	}
			
void CCustomCmdTestSecurityPinCheck::CreateAndExecuteCustCmd()
	{
	//set the SIM status property as ESimStatusUninitialized
	TInt result = RProperty::Set(CSsmUiSpecific::StartupPSUid(), CSsmUiSpecific::SimStatusPropertyKey(), ESimStatusUninitialized);
	TEST(KErrNone == result);   
	
	TInt swpValue = 0;
	switch(iExecutionBehaviour)
		{
		case ESsmWaitForSignal:
			swpValue = ERPropertyValForSecCustCmdWaitForSignal;
		break;
		case ESsmDeferredWaitForSignal:
			swpValue = ERPropertyValForSecCustCmdDeferrdWaitForSignal;	
		break;
		case ESsmFireAndForget:
			swpValue = ERPropertyValForSecCustCmdFireAndForget;
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
	
void CCustomCmdTestSecurityPinCheck::CreateAndExecuteCancelCustCmd()
	{
	TInt swpValue = 0;
		switch(iExecutionBehaviour)
		{
		case ESsmWaitForSignal:
			swpValue = ERPropertyValForSecCustCmdWaitForSignal;
		break;
		case ESsmDeferredWaitForSignal:
			swpValue = ERPropertyValForSecCustCmdDeferrdWaitForSignal;	
		break;
		case ESsmFireAndForget:
			swpValue = ERPropertyValForSecCustCmdFireAndForget;
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
/**
write the pin check test case number to a file so that misc ref plugin can read the 
it and check corresponding PIN Check security plug-in functionality in different SIM status
*/
void CCustomCmdTestSecurityPinCheck::WritePinCheckTestCaseNumToFileL(TInt aTestNum)
	{
	INFO_PRINTF1(_L("WritePinCheckTestCaseNumToFileL"));
    //RFile file;
    const TInt err = iFile.Open(iFs, KTestCmdSecurityCheckTestFile, EFileWrite | EFileStream);
	User::LeaveIfError(err);

	RFileWriteStream stream(iFile);
	CleanupClosePushL(stream);
   
    stream.WriteInt32L(aTestNum);
    stream.CommitL();
    CleanupStack::PopAndDestroy(&stream);
	}

TVerdict CCustomCmdTestSecurityPinCheck::doTestStepPostambleL()
	{
	TEST(KErrNone == iProperty.Delete(KCustomcmdServerSID, iSwp.Key()));
	TInt err = RProperty::Delete(KPropertyCategory, KMiscPluginPropertyKey);
	TEST (KErrNone == err);
	iProperty.Close();
	iClient.Close();
	//delete the file		
    iFs.Delete(KTestCmdSecurityCheckTestFile);
	iFs.Close();
	__UHEAP_MARKEND;
	return TestStepResult();
	}
	
void CCustomCmdTestSecurityPinCheck::StartSimulateKey()
	{
	//Callback function for simulating the key
	TCallBack stop(CustomCmdTestSecurityCheckCallBackL, this);
	if(!iAsyncStopScheduler->IsActive())
		{
		iAsyncStopScheduler->Set(stop);
		}
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
	}

TSsmExecutionBehaviour CCustomCmdTestSecurityPinCheck::GetExecutionBehaviour()	
	{
	return iExecutionBehaviour;	
	}
//
/**
CStopSimulatekey class is used for stoping the scheduler and for canceling the swp request.
*/
CStopSimulatekey::~CStopSimulatekey()
	{
	iProperty.Close();
	Cancel();
	}

CStopSimulatekey::CStopSimulatekey() : CActive(CActive::EPriorityHigh)  //Standard)
	{	
	CActiveScheduler::Add(this);
	}

CStopSimulatekey* CStopSimulatekey::NewL(CCustomCmdTestSecurityPinCheck* aCustomCmdTestSecurityPinCheck)
	{
	CStopSimulatekey* self = new (ELeave) CStopSimulatekey();
	CleanupStack::PushL(self);
	self->Construct(aCustomCmdTestSecurityPinCheck);
	CleanupStack::Pop();
	return self;
	}

void CStopSimulatekey::Construct(CCustomCmdTestSecurityPinCheck* aCustomCmdTestSecurityPinCheck)
	{
	// Define a property for stopping active scheduler 
 	TInt r = RProperty::Define(KCustomcmdServerPropertyKey, RProperty::EInt,ECapability_None, ECapability_None);								
	iCustomCmdTestSecurityPinCheck = aCustomCmdTestSecurityPinCheck;
	}
		
void CStopSimulatekey::Start()
	{  
	iProperty.Attach(KCustomcmdServerSID,KCustomcmdServerPropertyKey);
	iProperty.Subscribe(iStatus);
	SetActive();				
	}	
 
void CStopSimulatekey::RunL()
	{	
	TInt value;
	TInt err = RProperty::Get(RProcess().SecureId(),KCustomcmdServerPropertyKey,value);	
	
	if( value == ERPropertyValForSecCustCmdCancelRequest)		
		{
		RDebug::Printf("CStopSimulatekey::RunL() CustCmdCancelRequest)");
		iCustomCmdTestSecurityPinCheck->CancelSwpRequest();				
		}	
	else if( value == ERPropertyValForStopSchedulerCmdDeferredWaitForSignal && iCustomCmdTestSecurityPinCheck->GetExecutionBehaviour() == ESsmDeferredWaitForSignal)		
		{
		RDebug::Printf("CStopSimulatekey::RunL() ExecutionBehaviour = ESsmDeferredWaitForSignal");
		iCustomCmdTestSecurityPinCheck->StopScheduler();		
		}
	else if( value == ERPropertyValForStopSchedulerCmdWaitForSignal && iCustomCmdTestSecurityPinCheck->GetExecutionBehaviour() == ESsmWaitForSignal)
		{
		RDebug::Printf("iCustomCmdTestSecurityPinCheck->StopScheduler()if( CStopSimulatekey::RunL() ExecutionBehaviour = ESsmWaitForSignal");
		iCustomCmdTestSecurityPinCheck->StopScheduler();		
		}
	else if( value == ERPropertyValForStopSchedulerCmdFireAndForget && iCustomCmdTestSecurityPinCheck->GetExecutionBehaviour() == ESsmFireAndForget )	
		{
		RDebug::Printf("CStopSimulatekey::RunL() ExecutionBehaviour = FireAndForget");		
		User::After(5000000); // Wait 5sec to allow transitions to fully complete
		User::After(5000000); // Wait 5sec to allow transitions to fully complete
		iCustomCmdTestSecurityPinCheck->StopScheduler();		
		}	
	else
		{
		Start();
		}		
	}
	
void CStopSimulatekey::DoCancel()
	{
	iProperty.Cancel();
	}

					
