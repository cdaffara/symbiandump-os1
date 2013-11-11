// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tcmd_step_publishstartupmode.h"
#include "ssmcustomcmdfactory.h"

// class under test
#include "cmdpublishstartupmode.h"

// helper class
#include "activestopper.h"

#include <ssm/ssmuiproviderdll.h>
#include <ssm/startupdomainpskeys.h>
#include <ssm/starterdomaincrkeys.h>
#include <e32property.h>
#include <e32cmn.h>
#include <centralrepository.h>

//Exe name which defines startup PS keys
_LIT (KExeToDefineStartUpPS, "\\sys\\bin\\definestartupps.exe");
_LIT(KStartUpPSKeys, "startupkeys");
const TUint32 KMiscPluginPropertyKey = 0x2000E658;
const TUid KPropertyCategory={0x2000D75B};

CCustomCmdTestPublishStartupMode::~CCustomCmdTestPublishStartupMode()
	{
    delete iActiveScheduler;
    delete iAsyncStopScheduler;
	}

CCustomCmdTestPublishStartupMode::CCustomCmdTestPublishStartupMode(const TDesC& aTestStepName):iTestStepName(aTestStepName)
	{
    if (iTestStepName == KTestPublishStartupModeWithCap)
        {
        SetTestStepName(KTestPublishStartupModeWithCap);
        }
    else
        {
        SetTestStepName(KTestPublishStartupModeWithoutCap);
        }
	}


static TInt CallBackL(TAny* aCCustomCmdTestPublishStartupMode)
    {
    //Call back function to stop active scheduler
    CCustomCmdTestPublishStartupMode* test = reinterpret_cast<CCustomCmdTestPublishStartupMode*>(aCCustomCmdTestPublishStartupMode);
    test->CallBackRunL();
    return KErrNone;
    }

void CCustomCmdTestPublishStartupMode::CallBackRunL()
    {
    //Stop the active scheduler if the request is completed else
    //callback once again
    if(iRequestStatus != KRequestPending)
        {
        CActiveScheduler::Stop();
        }
    else
        {
        iAsyncStopScheduler->CallBack();
        }
    }

TVerdict CCustomCmdTestPublishStartupMode::doTestStepPreambleL()
	{
    INFO_PRINTF1(_L("doTestStepPreambleL"));
    if (iTestStepName == KTestPublishStartupModeWithCap)
        {
        RProcess process;
        CleanupClosePushL(process);
        //Create and install active scheduler
        iActiveScheduler = new(ELeave) CActiveScheduler;
        CActiveScheduler::Install (iActiveScheduler);
    
        //Needed for calling callback for stopping active scheduler
        iAsyncStopScheduler = new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
        
        // Setting the P and S key will route the request to the reference plugins instead of the actual plugins
        TInt err = RProperty::Define(KPropertyCategory, KMiscPluginPropertyKey, RProperty::EInt);
        TEST(KErrNone == err || KErrAlreadyExists == err);
        err = RProperty::Set(KPropertyCategory, KMiscPluginPropertyKey, 1);
        TEST (KErrNone == err);

        INFO_PRINTF1(_L("Define global startup mode property"));
    
        //Start the test exe which defines startup related property keys
        err = process.Create(KExeToDefineStartUpPS, KStartUpPSKeys());
        INFO_PRINTF2(_L("Define global startup mode property process created with %d"), err);
        TEST(KErrNone == err);
        User::LeaveIfError(err);
        process.Resume();
    
        process.Rendezvous(iRequestStatus);
        User::WaitForRequest(iRequestStatus);
        TEST(KErrNone == iRequestStatus.Int());
    
        //Kill the define startup ps process
        process.Kill(KErrNone);
        CleanupStack::PopAndDestroy(&process);
        }
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CCustomCmdTestPublishStartupMode::doTestStepPostambleL()
	{
	if (iTestStepName == KTestPublishStartupModeWithCap)
	    {
	    TInt err = RProperty::Delete(KPropertyCategory, KMiscPluginPropertyKey);
	    TEST (KErrNone == err);
	    }
	return CTestStep::doTestStepPostambleL();
	}

TVerdict CCustomCmdTestPublishStartupMode::doTestStepL()
	{
	INFO_PRINTF1(_L("Entering test for publish startup mode custom command"));
	__UHEAP_MARK;
	RProcess process(KCurrentProcessHandle);
    if(!(process.HasCapability(ECapabilityPowerMgmt) && 
       process.HasCapability(ECapabilityWriteDeviceData)&& 
       process.HasCapability(ECapabilityProtServ)))
		{
		TUid KCentRepId = {0x101f8762}; 
		CRepository* repository = NULL;
		repository  = CRepository::NewL(KCentRepId);
       
		TInt ret = repository->Set(KStartupReason, ENormalStartup); 
		delete repository;
		INFO_PRINTF3(_L("Setting Central Repository key 101f8762 Return value = %d Expected value = %d"),ret, KErrPermissionDenied);
		TEST(ret == KErrPermissionDenied);
		}
    else
        {
        TRAPD(err, doTestCreateExecuteAndDestroyL());
        TEST(err == KErrNone);
        
        TRAP(err, doTestFactoryCreateAndExecuteCancelL());
        TEST(err == KErrNone);
        
        
        }
	__UHEAP_MARKEND;
	INFO_PRINTF1(_L("Leaving test for publish startup mode custom command"));
	return TestStepResult();
	}

/**
Old Test CaseID 		APPFWK-CUSTCMD-0022
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0022
 */

void CCustomCmdTestPublishStartupMode::doTestCreateExecuteAndDestroyL()
	{
	INFO_PRINTF1(_L("Entering test for create, execute and destroy"));
	__UHEAP_MARK;
	
	// Define the startmode central repository.
    TUid KCentRepId = {0x101f8762}; 
    CRepository* repository = CRepository::NewL(KCentRepId);
    TInt ret = repository->Set(KStartupReason, ENormalStartup); 
    delete repository;  
    INFO_PRINTF3(_L("Setting Central Repository key 101f8762 Return value = %d Expected value = %d"),ret, KErrNone);
    TEST(ret == KErrNone);

    TInt startUpMode = -1;
    
    //Setting inital value of startUpMode as -1
    TInt err = RProperty::Set(CSsmUiSpecific::StartupPSUid(), KPSGlobalStartupMode, startUpMode);
    err = RProperty::Get(CSsmUiSpecific::StartupPSUid(), KPSGlobalStartupMode, startUpMode);
    INFO_PRINTF3(_L("Initial value Startup mode is %d ; Expected %d"), startUpMode, -1);
    TEST(startUpMode == -1);

    INFO_PRINTF1(_L("Testing NewL for publish startup mode"));
    CCustomCmdPublishStartupMode* customCommand = CCustomCmdPublishStartupMode::NewL();
    CleanupReleasePushL(*customCommand);

    INFO_PRINTF1(_L("Testing Initialize for publish startup mode"));
    err = customCommand->Initialize(NULL);
    TEST(err == KErrNone);
    CleanupClosePushL(*customCommand);

	INFO_PRINTF1(_L("Testing Execute for publish startup mode"));
	TBuf8<1> nullBuf;
	customCommand->Execute(nullBuf, iRequestStatus);
	TEST(iRequestStatus == KRequestPending);

	TCallBack stop(CallBackL, this);
    iAsyncStopScheduler->Set(stop);
    iAsyncStopScheduler->CallBack();
    iActiveScheduler->Start();

	User::WaitForRequest(iRequestStatus);
	TEST(iRequestStatus == KErrNone);
	
    err = RProperty::Get(CSsmUiSpecific::StartupPSUid(), KPSGlobalStartupMode, startUpMode);
    INFO_PRINTF3(_L("Startup mode is %d ; Expected %d"), startUpMode, EStartupModeNormal);
    TEST(startUpMode == EStartupModeNormal);
	
    // Hidden Reset functionality is not supported in techview\hrp, so the request will be completed with KErrNotSupported
    // And the hidden reset reason would same as we set in the test code, ie., ENormalStartup.
    TInt startUpReason;
    err = RProperty::Get(CSsmUiSpecific::StartupPSUid(), KPSStartupReason, startUpReason);
    INFO_PRINTF3(_L("Startup Reason is %d ; Expected %d"), startUpReason, ENormalStartup);
    TEST(startUpReason == ENormalStartup);

	INFO_PRINTF1(_L("Cleaning up custom command"));
    // Close customCommand
    CleanupStack::PopAndDestroy(customCommand);
    // Release customCommand
    CleanupStack::PopAndDestroy(customCommand);
	
	__UHEAP_MARKEND;
	INFO_PRINTF1(_L("Leaving test for create, execute and destroy"));
	}

/**
Old Test CaseID 		APPFWK-CUSTCMD-0024
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0022
 */


void CCustomCmdTestPublishStartupMode::doTestFactoryCreateAndExecuteCancelL()
	{
	INFO_PRINTF1(_L("Entering test for factory create, execute cancel and destroy"));
		
	__UHEAP_MARK;
	
	INFO_PRINTF1(_L("Testing factory create for publish startup mode"));
	MSsmCustomCommand* customCommand = SsmCustomCmdFactory::CmdPublishStartupModeNewL();
	CleanupStack::PushL(customCommand);
	
	INFO_PRINTF1(_L("Testing Initialize for publish startup mode"));
	TInt err = customCommand->Initialize(NULL);
	TEST(err == KErrNone);
	
	INFO_PRINTF1(_L("Testing Execute for publish startup mode"));
	TBuf8<1> nullBuf;
	customCommand->Execute(nullBuf, iRequestStatus);
	TEST(iRequestStatus == KRequestPending);

	INFO_PRINTF1(_L("Testing cancel"));
	customCommand->ExecuteCancel();
	User::WaitForRequest(iRequestStatus);
	TEST(iRequestStatus == KErrCancel);
	
	iRequestStatus = KRequestPending;
	INFO_PRINTF1(_L("Testing double cancel doesn't fail"));
	customCommand->ExecuteCancel();
	TEST(iRequestStatus == KRequestPending);
	
	INFO_PRINTF1(_L("Cleaning up custom command"));
	customCommand->Close();
	customCommand->Release();
	CleanupStack::Pop(customCommand);
	customCommand = NULL;
	
	__UHEAP_MARKEND;
	INFO_PRINTF1(_L("Leaving test for factory create, execute cancel and destroy"));
	}
