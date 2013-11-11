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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "tcmd_step_devicesecuritycheck.h"
#include "ssmsecuritychecknotifier.h"
#include "ssmuiproviderdll.h"
#include "ssmcustomcmdfactory.h"
#include <w32std.h>
#include <s32file.h>

const TUid KPropertyCategory={0x2000D75B}; 
const TUint32 KMiscPluginPropertyKey = 0x2000E658;

CCustomCmdTestDeviceSecurityCheck::~CCustomCmdTestDeviceSecurityCheck()
	{
	delete iAsyncStopScheduler;
	delete iActiveSchedulerWait;
	delete iActiveScheduler;
	}

CCustomCmdTestDeviceSecurityCheck::CCustomCmdTestDeviceSecurityCheck()
	{
	//Set test step name
	SetTestStepName(KTCCustomCmdTestDeviceSecurityCheck);
	}

TVerdict CCustomCmdTestDeviceSecurityCheck::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("doTestStepPreambleL"));
	//Create and install active scheduler
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install (iActiveScheduler);
	iActiveSchedulerWait = new(ELeave) CActiveSchedulerWait;
	TInt err = RProperty::Define(KPropertyCategory, KMiscPluginPropertyKey, RProperty::EInt);
	TEST(KErrNone == err || KErrAlreadyExists == err);
	err = RProperty::Set(KPropertyCategory, KMiscPluginPropertyKey, 1);
	TEST(KErrNone == err);

	//Needed for calling calback for stopping active scheduler
	iAsyncStopScheduler = new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CCustomCmdTestDeviceSecurityCheck::doTestStepPostambleL()
	{
	TInt err = RProperty::Delete(KPropertyCategory, KMiscPluginPropertyKey);
	TEST(KErrNone == err);
	return CTestStep::doTestStepPostambleL();
	}

void CCustomCmdTestDeviceSecurityCheck::SimulatePasswordEntry()
	{
    RWsSession wsSession;
    TInt err = wsSession.Connect();
    TEST(KErrNone == err);
    
    const TInt okButtonPos1 = 60; //the position of ok button
    const TInt okButtonPos2 = 600; //the position of ok button
    //Simulate the key press ,(comma) in to pin notifier dialogue
	TRawEvent eventDown;
	TRawEvent eventUp;

	eventDown.Set(TRawEvent::EKeyDown, EStdKeyComma);
	UserSvr::AddEvent(eventDown);
	eventUp.Set(TRawEvent::EKeyUp, EStdKeyComma);
	UserSvr::AddEvent(eventUp);
	User::After(100000);

	//Enter wrong pwd if iWrongPwd is ETrue
	if(iWrongPwd)
		{
		eventDown.Set(TRawEvent::EKeyDown, EStdKeyComma);
		UserSvr::AddEvent(eventDown);
		eventUp.Set(TRawEvent::EKeyUp, EStdKeyComma);
		UserSvr::AddEvent(eventUp);
		User::After(100000);
		
		//Reset it to false as wrong password should be entered only once
		iWrongPwd = EFalse;
		}

    eventDown.Set(TRawEvent::EButton1Down, okButtonPos1,okButtonPos2);
    UserSvr::AddEvent(eventDown);
    eventUp.Set(TRawEvent::EButton1Up, okButtonPos1,okButtonPos2);
    UserSvr::AddEvent(eventUp);
    User::After(100000);
    
    wsSession.Flush();
    wsSession.Close();
	}

static TInt CallBackL(TAny* aCCustomCmdTestSimSecurityCheck)
	{
	//Call back function to stop active scheduler
	CCustomCmdTestDeviceSecurityCheck* test = reinterpret_cast<CCustomCmdTestDeviceSecurityCheck*>(aCCustomCmdTestSimSecurityCheck);
	test->CallBackRunL();
	return KErrNone;
	}

void CCustomCmdTestDeviceSecurityCheck::CallBackRunL()
	{
	//Stop the active scheduler if the request is completed else
	//callback once again
	if(iRequest != KRequestPending)
		{
		iActiveSchedulerWait->AsyncStop();
		}
	else
		{
		iAsyncStopScheduler->CallBack();
		SimulatePasswordEntry();
		}
	}

/**
Old Test CaseID 		APPFWK-CUSTCMD-0022
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0024
 */

void CCustomCmdTestDeviceSecurityCheck::TestLockPhoneDeviceRespReceivedL()
	{
	TestLockPhoneDeviceRespReceived1L();
	TestLockPhoneDeviceRespReceived2L();
	TestLockPhoneDeviceRespReceived3L();
	TestLockPhoneDeviceRespReceived4L();
	TestLockPhoneDeviceRespReceived5L();
	}

void CCustomCmdTestDeviceSecurityCheck::TestLockPhoneDeviceRespReceivedHelperL()
	{
	TBufC8<1> dummy;
	iCustomCmdDevSecurityCheck->iState = CCustomCmdDeviceSecurityCheck::EGetLockPhoneDevice;
	iCustomCmdDevSecurityCheck->iExecuteRequest = &iRequest;
	iCustomCmdDevSecurityCheck->Execute(dummy, iRequest);

	TCallBack stop(CallBackL, this);
	iAsyncStopScheduler->Set(stop);
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
	}

void CCustomCmdTestDeviceSecurityCheck::TestLockPhoneDeviceRespReceived1L()
	{
	INFO_PRINTF1(_L("TestLockPhoneDeviceRespReceived1L"));
	iCustomCmdDevSecurityCheck->iLockInfo.iStatus = RMobilePhone::EStatusLocked;
	iCustomCmdDevSecurityCheck->iLockInfo.iSetting = RMobilePhone::ELockSetEnabled;

	TestLockPhoneDeviceRespReceivedHelperL();

	INFO_PRINTF3(_L("Result at line %d is %d "),__LINE__ + 1, iRequest.Int());
	TEST(KErrNone == iRequest.Int());

	}

void CCustomCmdTestDeviceSecurityCheck::TestLockPhoneDeviceRespReceived2L()
	{
	INFO_PRINTF1(_L("TestLockPhoneDeviceRespReceived2L"));
	iCustomCmdDevSecurityCheck->iLockInfo.iStatus = RMobilePhone::EStatusLockUnknown;

	TestLockPhoneDeviceRespReceivedHelperL();


#ifdef __WINS__
	INFO_PRINTF3(_L("Result at line %d is %d "),__LINE__ + 1, iRequest.Int());
	TEST(KErrNotFound == iRequest.Int());
#else
    INFO_PRINTF3(_L("Result at line %d is %d "),__LINE__ + 1, iRequest.Int());
	TEST(KErrNone == iRequest.Int());
#endif
    INFO_PRINTF1(_L("TestLockPhoneDeviceRespReceived2L"));
	}

void CCustomCmdTestDeviceSecurityCheck::TestLockPhoneDeviceRespReceived3L()
	{
	INFO_PRINTF1(_L("TestLockPhoneDeviceRespReceived3L"));
	iCustomCmdDevSecurityCheck->iLockInfo.iStatus = RMobilePhone::EStatusLocked;
	iCustomCmdDevSecurityCheck->iLockInfo.iSetting = RMobilePhone::ELockSetEnabled;

	TBufC8<1> dummy;
	//iRequest = KRequestPending;
	iCustomCmdDevSecurityCheck->iExecuteRequest = &iRequest;
	iCustomCmdDevSecurityCheck->Execute(dummy, iRequest);
	iCustomCmdDevSecurityCheck->ExecuteCancel();

	TCallBack stop(CallBackL, this);
	iAsyncStopScheduler->Set(stop);
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();

	INFO_PRINTF3(_L("Result at line %d is %d "),__LINE__ + 1, iRequest.Int());
	TEST(KErrCancel == iRequest.Int());
	}

void CCustomCmdTestDeviceSecurityCheck::TestLockPhoneDeviceRespReceived4L()
	{
	INFO_PRINTF1(_L("TestLockPhoneDeviceRespReceived4L"));
	iCustomCmdDevSecurityCheck->iLockInfo.iStatus = RMobilePhone::EStatusLockUnknown;

	iRequest = KRequestPending;
	iCustomCmdDevSecurityCheck->iExecuteRequest = &iRequest;
	iCustomCmdDevSecurityCheck->iStatus = KErrGeneral;
	iCustomCmdDevSecurityCheck->LockPhoneDeviceRespReceivedL();

	INFO_PRINTF3(_L("Result at line %d is %d "),__LINE__ + 1, iRequest.Int());
	TEST(KErrGeneral == iRequest.Int());
	
	//Reset iStatus back
	iCustomCmdDevSecurityCheck->iStatus = KErrNone;
	}

void CCustomCmdTestDeviceSecurityCheck::TestLockPhoneDeviceRespReceived5L()
	{
	INFO_PRINTF1(_L("TestLockPhoneDeviceRespReceived5L"));
	iCustomCmdDevSecurityCheck->iLockInfo.iStatus = RMobilePhone::EStatusLockUnknown;
	iCustomCmdDevSecurityCheck->iState = CCustomCmdDeviceSecurityCheck::EGetLockPhoneDevice;

	TBufC8<1> dummy;
	iRequest = KRequestPending;
	iCustomCmdDevSecurityCheck->iExecuteRequest = &iRequest;
	iCustomCmdDevSecurityCheck->LockPhoneDeviceRespReceivedL();

	TCallBack stop(CallBackL, this);
	iAsyncStopScheduler->Set(stop);
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();

#ifdef __WINS__
    INFO_PRINTF3(_L("Result at line %d is %d "),__LINE__ + 1, iRequest.Int());
	TEST(KErrNotFound == iRequest.Int());
#else
    INFO_PRINTF3(_L("Result at line %d is %d "),__LINE__ + 1, iRequest.Int());	
    TEST(KErrNone == iRequest.Int());
#endif
	}

void CCustomCmdTestDeviceSecurityCheck::TestLockPhoneToIccRespReceivedHelperL()
	{
	iRequest = KRequestPending;
	iCustomCmdDevSecurityCheck->iExecuteRequest = &iRequest;
	iCustomCmdDevSecurityCheck->LockPhoneToIccRespReceivedL();

	TCallBack stop(CallBackL, this);
	iAsyncStopScheduler->Set(stop);

	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
	}

/**
Old Test CaseID 		APPFWK-CUSTCMD-0023
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0024
 */

void CCustomCmdTestDeviceSecurityCheck::TestLockPhoneToIccRespReceivedL()
	{
	TestLockPhoneToIccRespReceived1L();
	TestLockPhoneToIccRespReceived2L();
	TestLockPhoneToIccRespReceived3L();
	}

void CCustomCmdTestDeviceSecurityCheck::TestLockPhoneToIccRespReceived1L()
	{
	INFO_PRINTF1(_L("TestLockPhoneToIccRespReceivedL1"));
	iCustomCmdDevSecurityCheck->iLockInfo.iStatus = RMobilePhone::EStatusLocked;
	iCustomCmdDevSecurityCheck->iLockInfo.iSetting = RMobilePhone::ELockSetEnabled;
	iCustomCmdDevSecurityCheck->iStatus = KErrNone;

	TestLockPhoneToIccRespReceivedHelperL();
	
	INFO_PRINTF3(_L("Result at line %d is %d "),__LINE__ + 1, iRequest.Int());
	TEST(KErrNone == iRequest.Int());
	}

void CCustomCmdTestDeviceSecurityCheck::TestLockPhoneToIccRespReceived2L()
	{
	INFO_PRINTF1(_L("TestLockPhoneToIccRespReceived2L"));
	iCustomCmdDevSecurityCheck->iLockInfo.iStatus = RMobilePhone::EStatusLockUnknown;
	iCustomCmdDevSecurityCheck->iStatus = KErrNone;

	TestLockPhoneToIccRespReceivedHelperL();
	
	INFO_PRINTF3(_L("Result at line %d is %d "),__LINE__ + 1, iRequest.Int());
	TEST(KErrNone == iRequest.Int());
	}

void CCustomCmdTestDeviceSecurityCheck::TestLockPhoneToIccRespReceived3L()
	{
	INFO_PRINTF1(_L("TestLockPhoneToIccRespReceived3L"));
	iCustomCmdDevSecurityCheck->iStatus = KErrGeneral;

	TestLockPhoneToIccRespReceivedHelperL();
	
	INFO_PRINTF3(_L("Result at line %d is %d "),__LINE__ + 1, iRequest.Int());
	TEST(KErrGeneral == iRequest.Int());

	//Reset iStatus back to KErrnone
	iCustomCmdDevSecurityCheck->iStatus = KErrNone;
	}

/**
Old Test CaseID 		APPFWK-CUSTCMD-0024
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0024
 */

void CCustomCmdTestDeviceSecurityCheck::TestSecCodeQueryRespReceivedL()
	{
	TestSecCodeQueryRespReceived1L();
	TestSecCodeQueryRespReceived2L();
	}

void CCustomCmdTestDeviceSecurityCheck::TestSecCodeQueryRespReceived1L()
	{
	INFO_PRINTF1(_L("TestLockPhoneToIccRespReceived1L"));
	iRequest = KRequestPending;
	iCustomCmdDevSecurityCheck->iExecuteRequest = &iRequest;
	iCustomCmdDevSecurityCheck->iStatus = KErrGeneral;
	iCustomCmdDevSecurityCheck->SecCodeQueryRespReceivedL();
	
	INFO_PRINTF3(_L("Result at line %d is %d "),__LINE__ + 1, iRequest.Int());
	TEST(KErrGeneral == iRequest.Int());
	
	//Reset iStatus back
	iCustomCmdDevSecurityCheck->iStatus = KErrNone;
	}

void CCustomCmdTestDeviceSecurityCheck::TestSecCodeQueryRespReceived2L()
	{
	INFO_PRINTF1(_L("TestLockPhoneToIccRespReceived2L"));
	
	TBufC8<1> dummy;
	iRequest = KRequestPending;
	iCustomCmdDevSecurityCheck->iExecuteRequest = &iRequest;
	iCustomCmdDevSecurityCheck->iState = CCustomCmdDeviceSecurityCheck::EQuerySecCode;
	iWrongPwd = ETrue;
	iCustomCmdDevSecurityCheck->QuerySecCodeL();

	TCallBack stop(CallBackL, this);
	iAsyncStopScheduler->Set(stop);
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();

	INFO_PRINTF3(_L("Result at line %d is %d "),__LINE__ + 1, iRequest.Int());
	TEST(KErrNone == iRequest.Int());
	}



TVerdict CCustomCmdTestDeviceSecurityCheck::doTestStepL()
	{
	__UHEAP_MARK;
	iCustomCmdDevSecurityCheck = static_cast<CCustomCmdDeviceSecurityCheck*>(SsmCustomCmdFactory::CmdDeviceSecurityCheckNewL());

	CSsmCustomCommandEnv* cmdEnv = NULL;
	//Initialise the command. CSsmCustomCommandEnv is not used inside the Initialize so passing
	//null should be fine.
	iCustomCmdDevSecurityCheck->Initialize(cmdEnv);

	TestLockPhoneDeviceRespReceivedL();
	TestLockPhoneToIccRespReceivedL();
	TestSecCodeQueryRespReceivedL();

	//Close the security check custom custom
	iCustomCmdDevSecurityCheck->Close();

	//Delete the security check custom custom
	iCustomCmdDevSecurityCheck->Release();
	__UHEAP_MARKEND;
	return TestStepResult();
	}

