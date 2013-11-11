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

#include "tcmd_step_simsecuritycheck.h"
#include "ssmsecuritychecknotifier.h"
#include "ssmuiproviderdll.h"
#include "ssmcustomcmdfactory.h"
#include <ssm/startupdomainpskeys.h>
#include <e32event.h>
#include <e32std.h>
#include <s32file.h>

//Swp policy file for sim status swp
_LIT(KTestSimStatusSwpPolicy, "ssm.swp.policy.simstatus.dll");

//Exe name which defines startup PS keys
_LIT (KExeToDefineStartUpPS, "\\sys\\bin\\definestartupps.exe");

//Exe name which defines emergency call PS keys
_LIT (KExeToDefineEmergencyCallPS, "\\sys\\bin\\defineemergencycallps.exe");

// Use to write the PIN Check Security test case nos.
_LIT(KTestCmdSecurityCheckTestFile, "c:\\cmdsecuritychecktest\\pinchecksecuritycaseno.txt");

const TUid KPropertyCategory={0x2000D75B};
const TUint32 KMiscPluginPropertyKey = 0x2000E658;

static TInt CallBack2L(TAny* aCCustomCmdTestSecurityCheck);

CCustomCmdTestSimSecurityCheck::~CCustomCmdTestSimSecurityCheck()
	{
	delete iAsyncStopScheduler;
	delete iActiveSchedulerWait;
	delete iActiveScheduler;
	}

CCustomCmdTestSimSecurityCheck::CCustomCmdTestSimSecurityCheck()
	{
	//Set test step name
	SetTestStepName(KTCCustomCmdTestSimSecurityCheck);
	}

TVerdict CCustomCmdTestSimSecurityCheck::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("doTestStepPreambleL"));
	//Create and install active scheduler
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install (iActiveScheduler);
	iActiveSchedulerWait = new(ELeave) CActiveSchedulerWait;
	
	//Needed fro calling calback for stopping active scheduler
	iAsyncStopScheduler = new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
	
    TInt err = RProperty::Define(KPropertyCategory, KMiscPluginPropertyKey, RProperty::EInt);
	INFO_PRINTF2(_L("Define Test Property returns : %d "), err);
	TEST(KErrNone == err || KErrAlreadyExists == err);
    err = RProperty::Set(KPropertyCategory, KMiscPluginPropertyKey, 1);
	INFO_PRINTF2(_L("Set Test Property returns : %d "), err);
	TEST(KErrNone == err);


	RProcess processHandle;
	CleanupClosePushL(processHandle);
	//Start the test exe which defines startup related property keys
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
	
	//Start the test exe which defines emergency call related property keys
	processHandle.Create(KExeToDefineEmergencyCallPS, KNullDesC);
	processHandle.Resume();
	processHandle.Close();
	
	RegisterSimSecurityStatus();
	
	//there are 8 different scenarios to test the custom command. Check the test spec for the scenarios.
 	//CMiscAdaptationRef::SecurityStateChange() has been changed to simulate the scenarios.
 	//CMiscAdaptationRef::SecurityStateChange() function uses pinchecksecuritycaseno.txt file to select the scenarios.
 	//This test case uses scenario 1 to test custom command.
	_LIT(KDirNameOfTestCasesNumFile, "c:\\cmdsecuritychecktest\\");
	
	//connect to file server
	User::LeaveIfError(iFs.Connect());
	err = iFs.MkDirAll(KDirNameOfTestCasesNumFile);
	if (KErrAlreadyExists != err && KErrNone != err)
		{
		INFO_PRINTF1(_L("Leaving as it could not create directory"));
		User::Leave(err);
		}
	RFile file;
	CleanupClosePushL(file);
	err = file.Replace(iFs, KTestCmdSecurityCheckTestFile, EFileWrite | EFileStream);
	RFileWriteStream stream(file);
	CleanupClosePushL(stream);
   
	stream.WriteInt32L(1);
	stream.CommitL();
	CleanupStack::PopAndDestroy(&stream);
	CleanupStack::PopAndDestroy(&file);
	
	User::After(1000000);
	//Create a security check cmd
	iCustomCmdCheck = static_cast<CCustomCmdSimSecurityCheck*>(SsmCustomCmdFactory::CmdSimSecurityCheckNewL());
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CCustomCmdTestSimSecurityCheck::doTestStepPostambleL()
	{
	iFs.Delete(KTestCmdSecurityCheckTestFile);
	iFs.Close();
	TInt err = RProperty::Delete(KPropertyCategory, KMiscPluginPropertyKey);
	TEST(KErrNone == err);
	return CTestStep::doTestStepPostambleL();
	}

// CCustomCmdTestSimSecurityCheck::RegisterSimSecurityStatus()
// This method is used to define the SIM status proeprty with SSM's secure ID
// and to map the SIM status PandS key with the SIM Status SWP policy
void CCustomCmdTestSimSecurityCheck::RegisterSimSecurityStatus()
    {
    INFO_PRINTF1(_L("RegisterSimSecurityStatus"));
    
    _LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
       
    //Define the property for sim status for swp.
    TInt ret = RProperty::Define(RProcess().SecureId(), CSsmUiSpecific::SimStatusPropertyKey(), RProperty::EInt,
                                         KAllowAllPolicy, KAllowAllPolicy);
    TEST(KErrNone == ret || KErrAlreadyExists == ret);
    
    //Connect to SsmStateManager
    RSsmStateManager stateManager;
    ret = stateManager.Connect();
    TEST(KErrNone == ret);
    const TUint KStartupSimSecurityStatusUid = CSsmUiSpecific::SimStatusPropertyKey();
    
    //register the swp mapping for Sim security status with its swp policy
    ret = stateManager.RegisterSwpMapping(KStartupSimSecurityStatusUid, KTestSimStatusSwpPolicy);
    TEST(KErrNone == ret || KErrAlreadyExists == ret);
    stateManager.Close();
    }

/////////////////////////////////////////////////////////////////////////////////////
/**
Old Test CaseID 		APPFWK-CUSTCMD-0006
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0006
 */



void CCustomCmdTestSimSecurityCheck::TestGeneralL()
	{
	//Testing some of the functions from ui specific dll.
	const TUid KPSStartupUid = {0x2000E65E};
	const TUid KSecurityPinNotifierUid = {0x2000E667};
	
	TUid uid1 = CSsmUiSpecific::StartupPSUid();
	TEST(KPSStartupUid == uid1);
	INFO_PRINTF3(_L("Test GeneralL : KPSStartupUid = %d ; Got from CSsmUiSpecific::StartupPSUid = %d "), KPSStartupUid, uid1);

	TUid uid2 = CSsmUiSpecific::SecurityPinNotifierUid();
	TEST(KSecurityPinNotifierUid == uid2);
	INFO_PRINTF3(_L("Test GeneralL : KSecurityPinNotifierUid = %d ; Got from CSsmUiSpecific::SecurityPinNotifierUid = %d "), KPSStartupUid, uid2);
	
	TEST( CSsmUiSpecific::IsSimlessOfflineSupported() );
	
	TEST( CSsmUiSpecific::IsNormalBoot() );
	
	TEST( CSsmUiSpecific::IsSimChangedReset() );
    
	//Get the instance of ui specific dll
	CSsmUiSpecific* ssmSecurityCustomCmdUid = CSsmUiSpecific::InstanceL();
	
	//Set the security status as passed
	ssmSecurityCustomCmdUid->SetSecurityStatus(EStrtSecurityCheckPassed);
	
	//Get the security status
	TStrtSecurityStatus sst = ssmSecurityCustomCmdUid->SecurityStatus();
	
	//Test the security status with EStrtSecurityCheckPassed which was set
	TEST(EStrtSecurityCheckPassed == sst); 

	//Delete created uispecific object
	CSsmUiSpecific::Release();
	}

//
static TInt CallBack1L(TAny* aCCustomCmdTestSimSecurityCheck)
	{
	//Call back function to stop active scheduler
	CCustomCmdTestSimSecurityCheck* test = reinterpret_cast<CCustomCmdTestSimSecurityCheck*>(aCCustomCmdTestSimSecurityCheck);
	test->CallBack1RunL();
	return KErrNone;
	}

void CCustomCmdTestSimSecurityCheck::CallBack1RunL()
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
		}
	}

/**
Old Test CaseID 		APPFWK-CUSTCMD-0007
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0006
 */

void CCustomCmdTestSimSecurityCheck::TestHandleSIMPresentL()
	{
	TestHandleSIMPresent1L();
	TestHandleSIMPresent2L();
	TestHandleSIMPresent3L();
	}

void CCustomCmdTestSimSecurityCheck::TestHandleSIMPresent1L()
	{
	_LIT(KTESTLOG, "TestHandleSIMPresent1L");
	INFO_PRINTF1(KTESTLOG);

	iCustomCmdCheck->iSecurityStateInfo = StartupAdaptation::EPUK1Required;
	iRequest = KRequestPending;
	iCustomCmdCheck->iExecuteRequest = &iRequest;
	iCustomCmdCheck->HandleSIMPresent();

	TCallBack stop(CallBack1L, this);
	iAsyncStopScheduler->Set(stop);

	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
	TEST(KErrGeneral == iRequest.Int());
	}

void CCustomCmdTestSimSecurityCheck::TestHandleSIMPresent2L()
	{
	_LIT(KTESTLOG, "TestHandleSIMPresent2L");
	TInt result = RProperty::Set(CSsmUiSpecific::StartupPSUid(), CSsmUiSpecific::SimStatusPropertyKey(), ESimStatusUninitialized);
	TEST(KErrNone == result);
	INFO_PRINTF1(KTESTLOG);
	iCustomCmdCheck->iSecurityStateInfo = StartupAdaptation::EYes;
	iRequest = KRequestPending;
	iCustomCmdCheck->iExecuteRequest = &iRequest;
	iCustomCmdCheck->HandleSIMPresent();
	TCallBack stop(CallBack2L, this);

	iAsyncStopScheduler->Set(stop);
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
#ifdef __WINS__	
	TEST(KErrNone == iRequest.Int());
#else
	//Other tests which ran before this might have already loaded sus pluing (ssmclayersup.dll) which 
	//will result in KErrAlreadyExists. This will happen only in armv5 as it runs 
	//all the tests with out rebooting
	TEST(KErrNone == iRequest.Int() || KErrAlreadyExists == iRequest.Int());
#endif
	}

void CCustomCmdTestSimSecurityCheck::TestHandleSIMPresent3L()
	{
	_LIT(KTESTLOG, "TestHandleSIMPresent3L");
	INFO_PRINTF1(KTESTLOG);
	iCustomCmdCheck->iSecurityStateInfo = StartupAdaptation::ENo;
	iRequest = KRequestPending;
	iCustomCmdCheck->iExecuteRequest = &iRequest;
	iCustomCmdCheck->HandleSIMPresent();

	TCallBack stop(CallBack1L, this);
	iAsyncStopScheduler->Set(stop);
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
	TEST(KErrNone == iRequest.Int());
	}

//

/*
Test Case ID            DEVSRVS-SSREFPLUGINS-CUSTCMD-0006
*/

void CCustomCmdTestSimSecurityCheck::TestHandleSIMInvalidL()
    {
    TestHandleSIMInvalid1L();
    TestHandleSIMInvalid2L();
    TestHandleSIMInvalid3L();
    }


void CCustomCmdTestSimSecurityCheck::TestHandleSIMInvalid1L()
    {
    _LIT(KTESTLOG, "TestHandleSIMInvalid1L");
    INFO_PRINTF1(KTESTLOG);
    iCustomCmdCheck->iSecurityStateInfo = StartupAdaptation::EPUK1Required;
    iRequest = KRequestPending;
    iCustomCmdCheck->iExecuteRequest = &iRequest;
    iCustomCmdCheck->HandleSIMInvalid();

    TCallBack stop(CallBack2L, this);
    iAsyncStopScheduler->Set(stop);
    iAsyncStopScheduler->CallBack();
    iActiveSchedulerWait->Start();
    TEST(KErrGeneral == iRequest.Int());
    }

void CCustomCmdTestSimSecurityCheck::TestHandleSIMInvalid2L()
    {
    _LIT(KTESTLOG, "TestHandleSIMInvalid2L");
    INFO_PRINTF1(KTESTLOG);
    iCustomCmdCheck->iSecurityStateInfo = StartupAdaptation::EYes;
    iRequest = KRequestPending;
    iCustomCmdCheck->iExecuteRequest = &iRequest;
    iCustomCmdCheck->HandleSIMInvalid();

    TCallBack stop(CallBack2L, this);
    iAsyncStopScheduler->Set(stop);
    iAsyncStopScheduler->CallBack();
    iActiveSchedulerWait->Start();
    TEST(KErrNone == iRequest.Int());
    }

void CCustomCmdTestSimSecurityCheck::TestHandleSIMInvalid3L()
    {
    _LIT(KTESTLOG, "TestHandleSIMInvalid3L");
    INFO_PRINTF1(KTESTLOG);
    iCustomCmdCheck->iSecurityStateInfo = StartupAdaptation::ENo;
    iRequest = KRequestPending;
    iCustomCmdCheck->iExecuteRequest = &iRequest;
    iCustomCmdCheck->HandleSIMInvalid();

    TCallBack stop(CallBack2L, this);
    iAsyncStopScheduler->Set(stop);
    iAsyncStopScheduler->CallBack();
    iActiveSchedulerWait->Start();
    TEST(KErrNone == iRequest.Int());
    }

//

/**
Old Test CaseID 		APPFWK-CUSTCMD-0008
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0006
 */

void CCustomCmdTestSimSecurityCheck::TestHandleSIMRejectedL()
	{
	TestHandleSIMRejected1L();
	TestHandleSIMRejected2L();
	TestHandleSIMRejected3L();
	}

void CCustomCmdTestSimSecurityCheck::TestHandleSIMRejected1L()
	{
	_LIT(KTESTLOG, "TestHandleSIMRejected1L");
	INFO_PRINTF1(KTESTLOG);
	iCustomCmdCheck->iSecurityStateInfo = StartupAdaptation::EPUK1Required;
	iRequest = KRequestPending;
	iCustomCmdCheck->iExecuteRequest = &iRequest;
	iCustomCmdCheck->HandleSIMRejected();

	TCallBack stop(CallBack1L, this);
	iAsyncStopScheduler->Set(stop);
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
	TEST(KErrGeneral == iRequest.Int());
	}

void CCustomCmdTestSimSecurityCheck::TestHandleSIMRejected2L()
	{
	_LIT(KTESTLOG, "TestHandleSIMRejected2L");
	INFO_PRINTF1(KTESTLOG);
	iCustomCmdCheck->iSecurityStateInfo = StartupAdaptation::EYes;
	iRequest = KRequestPending;
	iCustomCmdCheck->iExecuteRequest = &iRequest;
	iCustomCmdCheck->HandleSIMRejected();

	TCallBack stop(CallBack2L, this);
	iAsyncStopScheduler->Set(stop);
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
	//Security observer might have already been loaded so returning with KErrAlreadyExists
	TEST(KErrNone == iRequest.Int());

	TInt val;
	//Get the startup PS value for sim security status
	TInt result = RProperty::Get(iCustomCmdCheck->iStartupPSUid, KStartupSimSecurityStatus, val);
	TEST(KErrNone == result);
	
	//Test the sim security status value
	TEST(ESimRejected == val);
	}

void CCustomCmdTestSimSecurityCheck::TestHandleSIMRejected3L()
	{
	_LIT(KTESTLOG, "TestHandleSIMRejected3L");
	INFO_PRINTF1(KTESTLOG);
	iCustomCmdCheck->iSecurityStateInfo = StartupAdaptation::ENo;
	iRequest = KRequestPending;
	iCustomCmdCheck->iExecuteRequest = &iRequest;
	iCustomCmdCheck->HandleSIMRejected();

	TCallBack stop(CallBack2L, this);
	iAsyncStopScheduler->Set(stop);
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
	TEST(KErrNone == iRequest.Int());
	}

//
static TInt CallBack2L(TAny* aCCustomCmdTestSimSecurityCheck)
	{
	CCustomCmdTestSimSecurityCheck* test = reinterpret_cast<CCustomCmdTestSimSecurityCheck*>(aCCustomCmdTestSimSecurityCheck);
	test->CallBack2RunL();
	return KErrNone;
	}

void CCustomCmdTestSimSecurityCheck::CallBack2RunL()
	{
	if(iRequest != KRequestPending)
		{
		iActiveSchedulerWait->AsyncStop();
		}
	else
		{
		iAsyncStopScheduler->CallBack();
		
		//Simulate entering the password only when it is asked for
		if(iCustomCmdCheck->iState == StartupAdaptation::EAskPIN ||
			iCustomCmdCheck->iState == StartupAdaptation::EAskPUK ||
			iCustomCmdCheck->iState == StartupAdaptation::EAskSEC ||
			iCustomCmdCheck->iState == StartupAdaptation::ESIMLock)
			{
			SimulatePasswordEntry();
			}
		}
	}

/**
Old Test CaseID 		APPFWK-CUSTCMD-0009
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0006
 */


void CCustomCmdTestSimSecurityCheck::TestHandleSIMBlockedL()
	{
	TestHandleSIMBlocked1L();
	TestHandleSIMBlocked2L();
	TestHandleSIMBlocked3L();
	TestHandleSIMBlocked4L();
	}

void CCustomCmdTestSimSecurityCheck::TestHandleSIMBlocked1L()
	{
	_LIT(KTESTLOG, "TestHandleSIMBlocked1L");
	INFO_PRINTF1(KTESTLOG);
	iCustomCmdCheck->iSecurityStateInfo = StartupAdaptation::EPUK1Required;
	iRequest = KRequestPending;
	iCustomCmdCheck->iExecuteRequest = &iRequest;
	iCustomCmdCheck->HandleSIMBlocked();

	TCallBack stop(CallBack2L, this);
	iAsyncStopScheduler->Set(stop);
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
	TEST(KErrNone == iRequest.Int());
	}

void CCustomCmdTestSimSecurityCheck::TestHandleSIMBlocked2L()
	{
	_LIT(KTESTLOG, "TestHandleSIMBlocked2L");
	INFO_PRINTF1(KTESTLOG);
	iCustomCmdCheck->iSecurityStateInfo = StartupAdaptation::EYes;
	iRequest = KRequestPending;
	iCustomCmdCheck->iExecuteRequest = &iRequest;
	iCustomCmdCheck->HandleSIMBlocked();

	TCallBack stop(CallBack1L, this);
	iAsyncStopScheduler->Set(stop);
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
	TEST(KErrGeneral == iRequest.Int());
	}

void CCustomCmdTestSimSecurityCheck::TestHandleSIMBlocked3L()
	{
	_LIT(KTESTLOG, "TestHandleSIMBlocked3L");
	INFO_PRINTF1(KTESTLOG);
	iCustomCmdCheck->iSecurityStateInfo = StartupAdaptation::ENo;
	iRequest = KRequestPending;
	iCustomCmdCheck->iExecuteRequest = &iRequest;
	iCustomCmdCheck->HandleSIMBlocked();

	TCallBack stop(CallBack2L, this);
	iAsyncStopScheduler->Set(stop);
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
	TEST(KErrNone == iRequest.Int());
	}

void CCustomCmdTestSimSecurityCheck::TestHandleSIMBlocked4L()
	{
	_LIT(KTESTLOG, "TestHandleSIMBlocked4L");
	INFO_PRINTF1(KTESTLOG);
	iCustomCmdCheck->iSecurityStateInfo = StartupAdaptation::EUPUKRequired;
	iRequest = KRequestPending;
	iCustomCmdCheck->iExecuteRequest = &iRequest;
	iCustomCmdCheck->HandleSIMBlocked();

	TCallBack stop(CallBack2L, this);
	iAsyncStopScheduler->Set(stop);
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
	TEST(KErrNone == iRequest.Int());
	}
//

/**
Old Test CaseID 		APPFWK-CUSTCMD-0010
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0006
 */


void CCustomCmdTestSimSecurityCheck::TestHandlePINRequiredL()
	{
	TestHandlePINRequired1L();
	TestHandlePINRequired2L();
	TestHandlePINRequired3L();
	TestHandlePINRequired4L();
	}

void CCustomCmdTestSimSecurityCheck::TestHandlePINRequired1L()
	{
	_LIT(KTESTLOG, "TestHandlePINRequired1L");
	INFO_PRINTF1(KTESTLOG);
	iCustomCmdCheck->iSecurityStateInfo = StartupAdaptation::EUPUKRequired;
	iRequest = KRequestPending;
	iCustomCmdCheck->iExecuteRequest = &iRequest;
	iCustomCmdCheck->HandlePINRequired();

	TCallBack stop(CallBack2L, this);
	iAsyncStopScheduler->Set(stop);
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
	TEST(KErrGeneral == iRequest.Int());
	}

void CCustomCmdTestSimSecurityCheck::TestHandlePINRequired2L()
	{
	_LIT(KTESTLOG, "TestHandlePINRequired2L");
	INFO_PRINTF1(KTESTLOG);
	iCustomCmdCheck->iSecurityStateInfo = StartupAdaptation::EPIN1Required;
	iRequest = KRequestPending;
	iCustomCmdCheck->iExecuteRequest = &iRequest;
	iCustomCmdCheck->HandlePINRequired();

	TCallBack stop(CallBack2L, this);
	iAsyncStopScheduler->Set(stop);
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
	TEST(KErrNone == iRequest.Int());
	}

void CCustomCmdTestSimSecurityCheck::TestHandlePINRequired3L()
	{
	_LIT(KTESTLOG, "TestHandlePINRequired3L");
	INFO_PRINTF1(KTESTLOG);
	iCustomCmdCheck->iSecurityStateInfo = StartupAdaptation::EUPINRequired;
	iRequest = KRequestPending;
	iCustomCmdCheck->iExecuteRequest = &iRequest;
	iCustomCmdCheck->HandlePINRequired();

	TCallBack stop(CallBack2L, this);
	iAsyncStopScheduler->Set(stop);
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
	TEST(KErrNone == iRequest.Int());
	}

void CCustomCmdTestSimSecurityCheck::TestHandlePINRequired4L()
	{
	_LIT(KTESTLOG, "TestHandlePINRequired4L");
	INFO_PRINTF1(KTESTLOG);
	iCustomCmdCheck->iSecurityStateInfo = StartupAdaptation::ENo;
	iRequest = KRequestPending;
	iCustomCmdCheck->iExecuteRequest = &iRequest;
	iCustomCmdCheck->HandlePINRequired();

	TCallBack stop(CallBack2L, this);
	iAsyncStopScheduler->Set(stop);
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
	TEST(KErrNone == iRequest.Int());
	}

//
/**
Old Test CaseID 		APPFWK-CUSTCMD-0012
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0006
 */

void CCustomCmdTestSimSecurityCheck::TestHandleSIMLessOfflineSupportedL()
	{
	_LIT(KTESTLOG, "TestHandleSIMLessOfflineSupportedL");
	INFO_PRINTF1(KTESTLOG);
	
	iRequest = KRequestPending;
	iCustomCmdCheck->iExecuteRequest = &iRequest;
	iCustomCmdCheck->HandleSIMLessOfflineSupported();

	TCallBack stop(CallBack2L, this);
	iAsyncStopScheduler->Set(stop);
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
	TEST(KErrNone == iRequest.Int());
	}

//
/**
Old Test CaseID 		APPFWK-CUSTCMD-0013
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0006
 */


void CCustomCmdTestSimSecurityCheck::TestHandleSIMLockL()
	{
	TestHandleSIMLock1L();
	TestHandleSIMLock2L();
	TestHandleSIMLock3L();
	TestHandleSIMLock4L();
	TestHandleSIMLock5L();
	}

void CCustomCmdTestSimSecurityCheck::TestHandleSIMLock1L()
	{
	_LIT(KTESTLOG, "TestHandleSIMLock1L");
	INFO_PRINTF1(KTESTLOG);
	iCustomCmdCheck->iSecurityStateInfo = StartupAdaptation::EUPUKRequired;
	iRequest = KRequestPending;
	iCustomCmdCheck->iExecuteRequest = &iRequest;
	iCustomCmdCheck->HandleSIMLock();

	TCallBack stop(CallBack1L, this);
	iAsyncStopScheduler->Set(stop);
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
	TEST(KErrGeneral == iRequest.Int());
	}

void CCustomCmdTestSimSecurityCheck::TestHandleSIMLock2L()
	{
	_LIT(KTESTLOG, "TestHandleSIMLock2L");
	INFO_PRINTF1(KTESTLOG);
	iCustomCmdCheck->iSecurityStateInfo = StartupAdaptation::ESimLockRestricted;
	iRequest = KRequestPending;
	iCustomCmdCheck->iExecuteRequest = &iRequest;
	iCustomCmdCheck->HandleSIMLock();

	TCallBack stop(CallBack1L, this);
	iAsyncStopScheduler->Set(stop);
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
	TEST(KErrNone == iRequest.Int());
	}

void CCustomCmdTestSimSecurityCheck::TestHandleSIMLock3L()
	{
	_LIT(KTESTLOG, "TestHandleSIMLock3L");
	INFO_PRINTF1(KTESTLOG);
	iCustomCmdCheck->iSecurityStateInfo = StartupAdaptation::ESimLockOk;
	iRequest = KRequestPending;
	iCustomCmdCheck->iExecuteRequest = &iRequest;
	iCustomCmdCheck->HandleSIMLock();

	TCallBack stop(CallBack1L, this);
	iAsyncStopScheduler->Set(stop);
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
	TEST(KErrNone == iRequest.Int());
	}

void CCustomCmdTestSimSecurityCheck::TestHandleSIMLock4L()
	{
	_LIT(KTESTLOG, "TestHandleSIMLock4L");
	INFO_PRINTF1(KTESTLOG);
	iCustomCmdCheck->iSecurityStateInfo = StartupAdaptation::ESimLockRestrictionOn;
	iRequest = KRequestPending;
	iCustomCmdCheck->iExecuteRequest = &iRequest;
	iCustomCmdCheck->HandleSIMLock();

	TCallBack stop(CallBack1L, this);
	iAsyncStopScheduler->Set(stop);
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
	TEST(KErrNone == iRequest.Int());
	}

void CCustomCmdTestSimSecurityCheck::TestHandleSIMLock5L()
	{
	_LIT(KTESTLOG, "TestHandleSIMLock5L");
	INFO_PRINTF1(KTESTLOG);
	iCustomCmdCheck->iSecurityStateInfo = StartupAdaptation::ESimLockRestrictionPending;
	iRequest = KRequestPending;
	iCustomCmdCheck->iExecuteRequest = &iRequest;
	iCustomCmdCheck->HandleSIMLock();

	TCallBack stop(CallBack1L, this);
	iAsyncStopScheduler->Set(stop);
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
	TEST(KErrNone == iRequest.Int());
	}

//
/**
Old Test CaseID 		APPFWK-CUSTCMD-0015
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0006
 */

void CCustomCmdTestSimSecurityCheck::TestHandlePINAskedL()
	{
	TestHandlePINAsked1L();
	TestHandlePINAsked2L();
	TestHandlePINAsked3L();
	TestHandlePINAsked4L();
	}

void CCustomCmdTestSimSecurityCheck::TestHandlePINAsked1L()
	{
	_LIT(KTESTLOG, "TestHandlePINAsked1L");
	INFO_PRINTF1(KTESTLOG);
	iRequest = KRequestPending;
	iCustomCmdCheck->iExecuteRequest = &iRequest;
	iCustomCmdCheck->iSsmSecurityCheckNotifier = CSsmSecurityCheckNotifier::NewL(ESecInfoSimLockRestrOn);
	iCustomCmdCheck->HandlePINAsked();

	TCallBack stop(CallBack2L, this);
	iAsyncStopScheduler->Set(stop);
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
	TEST(KErrGeneral == iRequest.Int());

	delete iCustomCmdCheck->iSsmSecurityCheckNotifier;
	iCustomCmdCheck->iSsmSecurityCheckNotifier = NULL;
	}

void CCustomCmdTestSimSecurityCheck::TestHandlePINAsked2L()
	{
	_LIT(KTESTLOG, "TestHandlePINAsked2L");
	INFO_PRINTF1(KTESTLOG);
	iCustomCmdCheck->iNoteType = ESecCodePIN1;
	iRequest = KRequestPending;
	iCustomCmdCheck->iExecuteRequest = &iRequest;
	iCustomCmdCheck->iSsmSecurityCheckNotifier = CSsmSecurityCheckNotifier::NewL(ESecInfoSimLockRestrOn);
	iCustomCmdCheck->ExecuteCancel();
	iCustomCmdCheck->HandlePINAsked();

	TCallBack stop(CallBack2L, this);
	iAsyncStopScheduler->Set(stop);
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
	TEST(KErrCancel == iRequest.Int());

	delete iCustomCmdCheck->iSsmSecurityCheckNotifier;
	iCustomCmdCheck->iSsmSecurityCheckNotifier = NULL;
	}

void CCustomCmdTestSimSecurityCheck::TestHandlePINAsked3L()
	{
	_LIT(KTESTLOG, "TestHandlePINAsked3L");
	INFO_PRINTF1(KTESTLOG);
	iCustomCmdCheck->iNoteType = ESecCodeUPIN;
	iRequest = KRequestPending;
	iCustomCmdCheck->iExecuteRequest = &iRequest;
	iCustomCmdCheck->iStateChangePending = EFalse;
	iCustomCmdCheck->iSsmSecurityCheckNotifier = CSsmSecurityCheckNotifier::NewL(ESecInfoSimLockRestrOn);
	iCustomCmdCheck->Cancel();
	iCustomCmdCheck->HandlePINAsked();

	TCallBack stop(CallBack2L, this);
	iAsyncStopScheduler->Set(stop);
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
	TEST(KErrNone == iRequest.Int());

	delete iCustomCmdCheck->iSsmSecurityCheckNotifier;
	iCustomCmdCheck->iSsmSecurityCheckNotifier = NULL;
	}

void CCustomCmdTestSimSecurityCheck::TestHandlePINAsked4L()
	{
	_LIT(KTESTLOG, "TestHandlePINAsked4L");
	INFO_PRINTF1(KTESTLOG);
	iCustomCmdCheck->iNoteType = ESecCodeSPC;
	iRequest = KRequestPending;
	iCustomCmdCheck->iExecuteRequest = &iRequest;
	iCustomCmdCheck->iSsmSecurityCheckNotifier = CSsmSecurityCheckNotifier::NewL(ESecInfoSimLockRestrOn);
	iCustomCmdCheck->Cancel();
	iCustomCmdCheck->HandlePINAsked();
	
	TCallBack stop(CallBack2L, this);
	iAsyncStopScheduler->Set(stop);
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
	TEST(KErrGeneral == iRequest.Int());
	delete iCustomCmdCheck->iSsmSecurityCheckNotifier;
	iCustomCmdCheck->iSsmSecurityCheckNotifier = NULL;
	}

//
/**
Old Test CaseID 		APPFWK-CUSTCMD-0016
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0006
 */


void CCustomCmdTestSimSecurityCheck::TestHandlePUKAskedL()
	{
	_LIT(KTESTLOG, "TestHandlePUKAskedL");
	INFO_PRINTF1(KTESTLOG);
	iCustomCmdCheck->iSecurityStateInfo = StartupAdaptation::EUPUKRequired;
	iRequest = KRequestPending;
	iCustomCmdCheck->iExecuteRequest = &iRequest;
	iCustomCmdCheck->iStateChangePending = EFalse;
	iCustomCmdCheck->iSsmSecurityCheckNotifier = CSsmSecurityCheckNotifier::NewL(ESecInfoSimLockRestrOn);
	iCustomCmdCheck->Cancel();
	iCustomCmdCheck->HandlePUKAsked();

	TCallBack stop(CallBack2L, this);
	iAsyncStopScheduler->Set(stop);
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
	TEST(KErrNone == iRequest.Int());

	TInt val;
	TInt result = RProperty::Get(iCustomCmdCheck->iStartupPSUid, KStartupSimSecurityStatus, val);
	TEST(KErrNone == result);
	TEST(ESimRejected == val);
	}
//
/**
Old Test CaseID 		APPFWK-CUSTCMD-0018
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0006
 */


void CCustomCmdTestSimSecurityCheck::TestSimFunctionsL()
	{
	_LIT(KTESTLOG, "TestSimFunctions");
	INFO_PRINTF1(KTESTLOG);

	TestSimNotSupported();
	TestSimRemoved();
	TestSimReadable();
	TestSimUsable();
	}

void CCustomCmdTestSimSecurityCheck::TestSimNotSupported()
	{
	//Cancel any request if they are active
	iCustomCmdCheck->Cancel();

	TInt result = RProperty::Set(CSsmUiSpecific::StartupPSUid(), CSsmUiSpecific::SimStatusPropertyKey(), ESimStatusUninitialized);
	TEST(KErrNone == result);
	iCustomCmdCheck->SimNotSupported();

	//Wait for PS value to get updated
	User::After(2000000);

	TInt simResult;
	result = RProperty::Get(CSsmUiSpecific::StartupPSUid(), CSsmUiSpecific::SimStatusPropertyKey(), simResult);
	TEST(KErrNone == result);
	TEST(simResult == ESimNotSupported);
	}

void CCustomCmdTestSimSecurityCheck::TestSimRemoved()
	{
	//Cancel any request if they are active
	iCustomCmdCheck->Cancel();

	TInt result = RProperty::Set(CSsmUiSpecific::StartupPSUid(), CSsmUiSpecific::SimStatusPropertyKey(), ESimUsable);
	TEST(KErrNone == result);
	iCustomCmdCheck->SimRemoved();

	//Wait for PS value to get updated
	User::After(2000000);

	TInt simResult;
	result = RProperty::Get(CSsmUiSpecific::StartupPSUid(), CSsmUiSpecific::SimStatusPropertyKey(), simResult);
	TEST(KErrNone == result);
	TEST(simResult == ESimNotPresent);
	}

void CCustomCmdTestSimSecurityCheck::TestSimReadable()
	{
	//Cancel any request if they are active
	iCustomCmdCheck->Cancel();

	TInt result = RProperty::Set(CSsmUiSpecific::StartupPSUid(), CSsmUiSpecific::SimStatusPropertyKey(), ESimNotPresent);
	TEST(KErrNone == result);
	iCustomCmdCheck->SimReadable();

	//Wait for PS value to get updated
	User::After(2000000);

	TInt simResult;
	result = RProperty::Get(CSsmUiSpecific::StartupPSUid(), CSsmUiSpecific::SimStatusPropertyKey(), simResult);
	TEST(KErrNone == result);
	TEST(simResult == ESimReadable);
	}

void CCustomCmdTestSimSecurityCheck::TestSimUsable()
	{
	//Cancel any request if they are active
	iCustomCmdCheck->Cancel();

	TInt result = RProperty::Set(CSsmUiSpecific::StartupPSUid(), CSsmUiSpecific::SimStatusPropertyKey(), ESimReadable);
	TEST(KErrNone == result);
	iCustomCmdCheck->SimUsable();

	//Wait for PS value to get updated
	User::After(2000000);

	TInt simResult;
	result = RProperty::Get(CSsmUiSpecific::StartupPSUid(), CSsmUiSpecific::SimStatusPropertyKey(), simResult);
	TEST(KErrNone == result);
	TEST(simResult == ESimUsable);
	}

//
static TInt CallBack3L(TAny* aCCustomCmdTestSimSecurityCheck)
	{
	CCustomCmdTestSimSecurityCheck* test = reinterpret_cast<CCustomCmdTestSimSecurityCheck*>(aCCustomCmdTestSimSecurityCheck);
	test->CallBack3RunL();
	return KErrNone;
	}

void CCustomCmdTestSimSecurityCheck::SimulatePasswordEntry()
	{
    const TInt okButtonPos1 = 60; //the position of ok button
    const TInt okButtonPos2 = 600; //the position of ok button
	TRawEvent eventDown;
	TRawEvent eventUp;

	//Simulate the key press ,(comma) in to pin notifier dialogue
	eventDown.Set(TRawEvent::EKeyDown, EStdKeyComma);
	UserSvr::AddEvent(eventDown);
	eventUp.Set(TRawEvent::EKeyUp, EStdKeyComma);
	UserSvr::AddEvent(eventUp);
	User::After(100000);

    eventDown.Set(TRawEvent::EButton1Down, okButtonPos1,okButtonPos2);
    UserSvr::AddEvent(eventDown);
    eventUp.Set(TRawEvent::EButton1Up, okButtonPos1, okButtonPos2);
    UserSvr::AddEvent(eventUp);
    User::After(100000);
	}

void CCustomCmdTestSimSecurityCheck::CallBack3RunL()
	{
	if(iRequest != KRequestPending)
		{
		iActiveSchedulerWait->AsyncStop();
		}
	else
		{
		iAsyncStopScheduler->CallBack();
		if(iCustomCmdCheck->iState == StartupAdaptation::EAskPIN ||
			iCustomCmdCheck->iState == StartupAdaptation::EAskPUK ||
			iCustomCmdCheck->iState == StartupAdaptation::EAskSEC ||
			iCustomCmdCheck->iState == StartupAdaptation::ESIMLock)
			{
			//Reset the security code status PS so that test goes further
			TInt result = RProperty::Set(CSsmUiSpecific::StartupPSUid(), KStartupSecurityCodeQueryStatus, ESecurityQueryActive);
			TEST(KErrNone == result);
			SimulatePasswordEntry();
			}
		}
	}

/**
Old Test CaseID 		APPFWK-CUSTCMD-0020
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0006
 */

void CCustomCmdTestSimSecurityCheck::TestSecurityNoteObserverL()
	{
	TestSecurityNoteObserver1L();
	TestSecurityNoteObserver2L();
	}

void CCustomCmdTestSimSecurityCheck::TestSecurityNoteObserver1L()
	{
	_LIT(KTESTLOG, "TestSecurityNoteObserver1L");
	INFO_PRINTF1(KTESTLOG);
	iCustomCmdCheck->iSecurityStateInfo = StartupAdaptation::EPIN1Required;
	iRequest = KRequestPending;
	iCustomCmdCheck->iExecuteRequest = &iRequest;
	iCustomCmdCheck->HandlePINRequired();

	TCallBack stop(CallBack3L, this);
	iAsyncStopScheduler->Set(stop);
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
	TEST(KErrNone == iRequest.Int());
	TInt propertyValue;
	TInt result = RProperty::Get(CSsmUiSpecific::StartupPSUid(), KPSSplashShutdown, propertyValue);
	TEST(KErrNone == result);
	TEST(propertyValue == ESplashShutdown);
	}

void CCustomCmdTestSimSecurityCheck::TestSecurityNoteObserver2L()
	{
	_LIT(KTESTLOG, "TestSecurityNoteObserver2L");
	INFO_PRINTF1(KTESTLOG);
	TBufC8<1> dummy;
	iCustomCmdCheck->StartSecurityNoteObserverL();
	iCustomCmdCheck->Execute(dummy, iRequest);
	iCustomCmdCheck->ExecuteCancel();
	iCustomCmdCheck->iStateChangePending = EFalse;
	
	TCallBack stop(CallBack3L, this);
	iAsyncStopScheduler->Set(stop);
	iAsyncStopScheduler->CallBack();
	iActiveSchedulerWait->Start();
	TEST(KErrCancel == iRequest.Int());
	}

//
TVerdict CCustomCmdTestSimSecurityCheck::doTestStepL()
	{
	__UHEAP_MARK;
	TestGeneralL();
	TestHandleSIMPresentL();
	TestHandleSIMInvalidL();
	TestHandleSIMRejectedL();
	TestHandleSIMBlockedL();
	TestHandlePINRequiredL();
	TestSecurityNoteObserverL();
	TestHandleSIMLessOfflineSupportedL();
	TestHandleSIMLockL();
	TestHandlePINAskedL();
	TestHandlePUKAskedL();
	TestSimFunctionsL();

	//Close the security check custom custom
	iCustomCmdCheck->Close();

	//Delete the security check custom custom
	iCustomCmdCheck->Release();
	__UHEAP_MARKEND;
	return TestStepResult();
	}
