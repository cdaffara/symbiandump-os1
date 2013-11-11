// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//These tests are only for the Cleserver when it reconstructs the 
//commands from the commandlist. 

/**
 @file
 @test
 @internalComponent - Internal Symbian test code  
*/

#include <ssm/ssmstartupproperties.h>
#include <f32file.h>
#include <e32property.h>

#include "clesrv.h"
#include "clecli.h"
#include "cmdpublishswp.h"
#include "ssmcustomcommandinfo.h"
#include "cmdcustomcommand.h"
#include "cmdloadsup.h"
#include "cmdpublishsystemstate.h"
#include "cmdreqswpchange.h"
#include "ssmtestapps.h"
#include "cmdstartapp.h"
#include "cmdstartprocess.h"
#include "ssmtestprocgoodsession.h"
#include "cmdpoweroff.h"
#include "cmdamastarter.h"
#include "cmdmultiplewait.h"
#include "cmdcreateswp.h"
#include "cmdpersisthalattributes.h"
#include "cmdfinalisedrives.h"
#include "cmdsetpandskey.h"
#include "cmdwaitforapparcinit.h"
#include "tcmd_runclecmdtest.h"

static const TUint KSwpNullUid = {0};
static const TUint KNewSwpKey = 0xAEAE;
static const TUint KNewSwpKey2 = 0xEAEA;
const TUid KTestPropertyCat={0x1234};
const TUint KTestKey = 678;
const TUid KTestDscId = {0x1234};
const TInt KNumOfRetries = 5;
const TInt KThrottleTime = 10000000; //10s

_LIT(KBadFileName, "c:\\badfile.exe");  //this file does not exist
_LIT(KSlowStartApp, "ssmtestappslow.exe");
_LIT(KEmptyArgs, "");
_LIT(KGoodProc, "ssmtestprocgood.exe");
_LIT(KPolicyFilename, "ssm.swp.policy.test.cmdreqswp");
_LIT(KNoSuchDll, "nosuchdll.dll");

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
typedef TDmTraverseDirection TTestSsmDirection;
TTestSsmDirection testTraverseDefault = ETraverseDefault;
#else //SYMBIAN_ENABLE_SPLIT_HEADERS
typedef TSsmDmTraverseDirection TTestSsmDirection;
TTestSsmDirection testTraverseDefault = ESsmTraverseDefault;
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

CCmdRunCleTest::CCmdRunCleTest()
    {
    }

CCmdRunCleTest::~CCmdRunCleTest()
    {
    delete iActiveScheduler;
    delete iAsyncStopScheduler;
    delete iStartupProperties;
    iStartupProperties = NULL;
    iSem.Close();
    }

static TInt StopScheduler(TAny* /*aCCmdRunCleTest*/)
    {
    CActiveScheduler::Stop();
    return KErrNone;
    }

TVerdict CCmdRunCleTest::doTestStepPreambleL()
    {
    iStartupProperties = CSsmStartupProperties::NewL();
    
    INFO_PRINTF1(_L("Starting scheduler..."));
    iActiveScheduler = new(ELeave) CActiveScheduler;
    CActiveScheduler::Install( iActiveScheduler );
    
    iAsyncStopScheduler = new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
    TCallBack stop(StopScheduler, this);
    iAsyncStopScheduler->Set(stop);
      
    return CTestStep::doTestStepPreambleL();
    }

TVerdict CCmdRunCleTest::doTestStepPostambleL()
    {
    delete iStartupProperties;
    iStartupProperties = NULL;
    return CTestStep::doTestStepPostambleL();
    }

TVerdict CCmdRunCleTest::doTestStepL()
    {
    __UHEAP_MARK;
    TRAPD(err, doTestRunClePublishSwpL());
    TEST(err == KErrNone);
    __UHEAP_MARKEND;
    
    __UHEAP_MARK;
    TRAP(err, doTestRunCleCustomCommandL());
    TEST(err == KErrNone);
    __UHEAP_MARKEND;
    
    __UHEAP_MARK;
    TRAP(err, doTestRunCleCmdLoadSupL());
    TEST(err ==  KErrNone);
    __UHEAP_MARKEND;
    
    __UHEAP_MARK;
    TRAP(err, doTestRunCleCmdpublishSystemStateL());
    TEST(err ==  KErrNone);
    __UHEAP_MARKEND;
    
    __UHEAP_MARK;
       TRAP(err,  doTestRunCmdCreateSwpL());
       TEST(err ==  KErrNone);
   __UHEAP_MARKEND;
    
    __UHEAP_MARK;
    TRAP(err, doTestRunCleCcmdreqSwpchangeL());
    TEST(err ==  KErrNone);
    __UHEAP_MARKEND;
    
    __UHEAP_MARK;
    TRAP(err, doTestRunCleCmdAmaStarterL());
    TEST(err ==  KErrNone);
    __UHEAP_MARKEND;
    
    __UHEAP_MARK;
    TRAP(err, doTesTRunCleCmdMultipleWaitL());
    TEST(err ==  KErrNone);
    __UHEAP_MARKEND;
        
    __UHEAP_MARK;
    TRAP(err, doTestRunCmdPowerOffL());
    TEST(err ==  KErrNone);
    __UHEAP_MARKEND;
            
    __UHEAP_MARK;
    TRAP(err,  doTestRunCleCmdStartAppL());
    TEST(err ==  KErrNone);
    __UHEAP_MARKEND;
    
    __UHEAP_MARK;
    TRAP(err,  doTestRunCleCmdStartProcessL());
    TEST(err ==  KErrNone);
    __UHEAP_MARKEND;
    
    __UHEAP_MARK;
    TRAP(err,  doTestRunCleCmdPersistHalAttributesL());
    TEST(err ==  KErrNone);
    __UHEAP_MARKEND;
    
    __UHEAP_MARK;
    TRAP(err, doTestRunCleCmdFinaliseDrivesL());
    TEST(err ==  KErrNone);
    __UHEAP_MARKEND;
     
    __UHEAP_MARK;
    TRAP(err,  doTestRunCleCmdSetPAndSKeyL());
    TEST(err ==  KErrNone);
    __UHEAP_MARKEND;
    
    __UHEAP_MARK;
    TRAP(err,  doTestRunCleCmdWaitForApparcInitL());
    TEST(err ==  KErrNone);
    __UHEAP_MARKEND;
          
    err = iSem.CreateGlobal(KStartAppSignalSemaphore, 0);
    INFO_PRINTF2(_L("Created semaphore with err %d"),err);
    
    __UHEAP_MARK;
    TRAP(err, doTestRunCleFandFAppWithMonitorL());
    TEST(err ==  KErrNone);
    __UHEAP_MARKEND;
    
    iSem.Close();

    err = iSem.CreateGlobal(KStartProcSignalSemaphore, 0);
    INFO_PRINTF2(_L("Created semaphore with err %d"),err);
    TEST(err == KErrNone);
    
    __UHEAP_MARK;
    TRAP(err,  doTestRunCleFandFProcWithMonitorL());
    TEST(err ==  KErrNone);
    __UHEAP_MARKEND;
    
    __UHEAP_MARK;
    TRAP(err, doTestRunCleFandFAlreadyRunWithMonitorL());
    TEST(err ==  KErrNone);
    __UHEAP_MARKEND;
         
   iSem.Close();
   return TestStepResult();
   }

//create cle server in separate thread within the Test server process
TInt CCmdRunCleTest::RunTestWithCleAndWaitL(MSsmCommand* aCommand, TBool aMultipleWait)
    {
    _LIT(KCleSrver, "TestCleSrv");
    RCleSession cleSession;// CLE session to execute commands
    CleanupClosePushL(cleSession);
    cleSession.ConnectL(KCleSrver);
    CSsmCommandList* iCmdList = CSsmCommandList::NewL();
    CleanupStack::PushL(iCmdList);
    iCmdList->AppendL(aCommand);
    if(aMultipleWait)
        {
        TInt32 timeout = 10000;
        CCmdMultipleWait* iCmd = CCmdMultipleWait::NewL(timeout);
        CleanupStack::PushL(iCmd);
        iCmdList->AppendL(iCmd);
        CleanupStack::Pop(iCmd);
        }
    //run engine, stop when there are no further transition
    iAsyncStopScheduler->CallBack();
    iActiveScheduler->Start();
    TRequestStatus trs;
    TCmdErrorSeverity severity;
    cleSession.ExecuteCommandList(*iCmdList, trs, severity);
          
    INFO_PRINTF1(_L("Waiting for syscle command to finish..."));
    User::WaitForRequest(trs);
    
    CleanupStack::PopAndDestroy(iCmdList);
    CleanupStack::PopAndDestroy();//for cle session
    return trs.Int();
    }

void CCmdRunCleTest::doTestRunCleCmdWaitForApparcInitL()
    {
    TInt expectedErr = KErrNone;    
    TCmdErrorSeverity severity = ECmdIgnoreFailure;
   
    INFO_PRINTF1(_L("doTestRunCleCmdWaitForApparcInitL, severity:ECmdIgnoreFailure"));
    iCmd = CCmdWaitForApparcInit::NewL(severity);
    TInt err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL;
    INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
    
    severity = ECmdCriticalSeverity;
    INFO_PRINTF1(_L("doTestRunCleCmdWaitForApparcInitL, severity:ECmdCriticalSeverity"));
    iCmd = CCmdWaitForApparcInit::NewL(severity);
    err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL;
    INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
    }

void CCmdRunCleTest::doTestRunCleCmdSetPAndSKeyL()
    {
    TInt expectedErr = KErrNone;    
    TCmdErrorSeverity severity = ECmdIgnoreFailure;
    TInt actualValue;
    
    INFO_PRINTF1(_L("doTestRunCleCmdSetPAndSKeyL, severity:ECmdIgnoreFailure"));
    iCmd = CCmdSetPAndSKey::NewL(severity, KTestPropertyCat, KTestKey, 589);
    TInt err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL;
    INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
    TEST(err == expectedErr);
    err = RProperty::Get(KTestPropertyCat,KTestKey,actualValue);
    TEST(actualValue == 589);
    
    severity = ECmdCriticalSeverity;
    INFO_PRINTF1(_L("doTestRunCleCmdSetPAndSKeyL, severity:ECmdCriticalSeverity"));
    iCmd = CCmdSetPAndSKey::NewL(severity, KTestPropertyCat, KTestKey, 589);
    err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL;
    INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
    TEST(actualValue == 589);
    TEST(err == expectedErr);
    err = err = RProperty::Get(KTestPropertyCat,KTestKey,actualValue);
    TEST(actualValue == 589);
    }

void CCmdRunCleTest::doTestRunCleCmdFinaliseDrivesL()
    {
    TInt expectedErr = KErrNone;    
    TCmdErrorSeverity severity = ECmdIgnoreFailure;
   
    INFO_PRINTF1(_L("doTestRunCleCmdFinaliseDrivesL, severity:ECmdIgnoreFailure"));
    iCmd = CCmdFinaliseDrives::NewL(severity);
    TInt err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL;
    INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
    
    severity = ECmdCriticalSeverity;
    INFO_PRINTF1(_L("doTestRunCleCmdFinaliseDrivesL, severity:ECmdCriticalSeverity"));
    iCmd = CCmdFinaliseDrives::NewL(severity);
    err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL;
    INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
    }

void CCmdRunCleTest::doTestRunCleCmdPersistHalAttributesL()
    {
    TInt expectedErr = KErrNone;    
    TCmdErrorSeverity severity = ECmdIgnoreFailure;
   
    INFO_PRINTF1(_L("doTestRunCleCmdPersistHalAttributesL, severity:ECmdIgnoreFailure"));
    iCmd = CCmdPersistHalAttributes::NewL(severity);
    TInt err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL;
    INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
    
    severity = ECmdCriticalSeverity;
    INFO_PRINTF1(_L("doTestRunCleCmdPersistHalAttributesL, severity:ECmdCriticalSeverity"));
    iCmd = CCmdPersistHalAttributes::NewL(severity);
    err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL;
    INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
    }

void CCmdRunCleTest::doTestRunCmdCreateSwpL()
    {
    TInt expectedType = ESsmCmdCreateSwp;
    TInt expectedErr = KErrNone;    
    TCmdErrorSeverity severity = ECmdIgnoreFailure;
    TSsmSwp swpInfo(KNewSwpKey, 876);
    TPtrC filename = KPolicyFilename();

    INFO_PRINTF1(_L("doTestRunCmdCreateSwpL, severity:ECmdIgnoreFailure"));
    iCmd = CCmdCreateSwp::NewL(severity, swpInfo, filename);
    TEST(iCmd->Type() == expectedType);
    TInt err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL;
    INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
    
    severity = ECmdCriticalSeverity;
    swpInfo.Set(KNewSwpKey2, 767);
    INFO_PRINTF1(_L("doTestRunCmdCreateSwpL, severity:ECmdCriticalSeverity"));
    iCmd = CCmdCreateSwp::NewL(severity, swpInfo, filename);
    TEST(iCmd->Type() == expectedType);
    err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL;
    INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
    }

void CCmdRunCleTest::doTestRunCmdPowerOffL()
    {
    TCmdErrorSeverity severity = ECmdIgnoreFailure;
    TInt err = KErrNone;
    TInt expectedErr = KErrNone;
    TPowerState powerEvent = EPwActive;
    
    INFO_PRINTF1(_L("doTestRunCmdPowerOffL, severity:ECmdIgnoreFailure, powerEvent:EPwActive"));
    iCmd = CCmdPowerOff::NewL(severity, powerEvent);
    err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL;    //ownership of iCmd is with Cle server
    INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
    TEST(err == expectedErr);
    
    severity = ECmdCriticalSeverity;
    INFO_PRINTF1(_L("doTestRunCmdPowerOffL, severity:ECmdCriticalSeverity, powerEvent:EPwActive"));
    iCmd = CCmdPowerOff::NewL(severity, powerEvent);
    err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL;    //ownership of iCmd is with Cle server
    INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, KErrArgument);
    TEST(err == expectedErr | KErrArgument);//we are not testing the power event functionality 
    }

void CCmdRunCleTest::doTesTRunCleCmdMultipleWaitL()
    {
    TInt err = KErrNone;
    TInt expectedErr = KErrNone;
    TCmdErrorSeverity severity = ECmdIgnoreFailure;
    TBool multipleWait = TRUE;
        
    INFO_PRINTF1(_L("doTesTRunCleCmdMultipleWaitL, Test:ESsmDeferredWaitForSignal, severity:ECmdIgnoreFailure"));
    SetStartupPropertiesL(KBadFileName, KEmptyArgs, ESsmDeferredWaitForSignal, ESsmCmdStartApp);
    iCmd = CCmdStartApp::NewL(severity,iStartupProperties);
    err = RunTestWithCleAndWaitL(iCmd,multipleWait);
    INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
    TEST(err == expectedErr);
    
    Reset();
    iCmd = NULL; //ownership of iCmd is with Cle server
    TEST(err == expectedErr);
    }

void CCmdRunCleTest::doTestRunCleCmdAmaStarterL()
    {
    TInt expectedErr = KErrNone;
    TCmdErrorSeverity severity = ECmdIgnoreFailure;
    
    INFO_PRINTF1(_L("doTestRunCleCmdAmaStarterL, Test:ESsmFireAndForget, severity:ECmdIgnoreFailure"));
    iCmd = CCmdAmaStarter::NewL(severity, ESsmFireAndForget, KTestDscId);
    TInt err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL; //ownership of iCmd is with Cle server
    INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
    TEST(err == expectedErr);
    
    severity = ECmdCriticalSeverity;
    INFO_PRINTF1(_L("doTestRunCleCmdAmaStarterL, Test:ESsmFireAndForget, severity:ECmdCriticalSeverity"));
    iCmd = CCmdAmaStarter::NewL(severity, ESsmFireAndForget, KTestDscId);
    err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL; //ownership of iCmd is with Cle server
    INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
    TEST(err == expectedErr);
    }

void CCmdRunCleTest::doTestRunClePublishSwpL()
    {
    TInt expectedErr = KErrNone;
    TCmdErrorSeverity severity = ECmdIgnoreFailure;
    TSsmSwp info(KSwpNullUid, 0);
    
    INFO_PRINTF1(_L("doTestRunClePublishSwp, Test:ESsmFireAndForget, severity:ECmdIgnoreFailure"));
    iCmd = CCmdPublishSwp::NewL(severity, ESsmFireAndForget, info);
    TInt err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL; //ownership of iCmd is with Cle server 
    INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
    TEST(err == expectedErr);
    
    severity = ECmdCriticalSeverity;
    INFO_PRINTF1(_L("doTestRunClePublishSwp, Test:ESsmFireAndForget, severity:ECmdCriticalSeverity"));
    iCmd = CCmdPublishSwp::NewL(severity, ESsmFireAndForget, info);
    err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL; //ownership of iCmd is with Cle server
    INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
    TEST(err == expectedErr);
    
    severity = ECmdHighSeverity;
    TSsmSwp info2(KNewSwpKey, 789);
    INFO_PRINTF1(_L("doTestRunClePublishSwp, Test:ESsmFireAndForget, severity:ECmdHighSeverity"));
    iCmd = CCmdPublishSwp::NewL(severity, ESsmFireAndForget, info2);
    err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL; //ownership of iCmd is with Cle server
    INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
    TEST(err == expectedErr);
    }

void CCmdRunCleTest::doTestRunCleCustomCommandL()
    {
    TInt expectedErr = KErrNone;
    TCmdErrorSeverity severity = ECmdIgnoreFailure;
    TPtrC filename = KNoSuchDll();
    TInt32 ordinal = 1;
    TPtrC8 params = KNullDesC8();
    TCmdCustomCommandLibUnloading unloading = EUnloadOnCommandCompletion;
    
    CSsmCustomCommandInfo* info = new (ELeave) CSsmCustomCommandInfo();
    CleanupStack::PushL(info);
    info->SetL(filename, ordinal, unloading, KNumOfRetries, params);
       
    INFO_PRINTF1(_L("doTestRunCleCustomCommand, Test:ESsmFireAndForget, severity:ECmdIgnoreFailure"));
    iCmd = CCmdCustomCommand::NewL(severity, ESsmFireAndForget, *info);
    TInt err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL; //ownership of iCmd is with Cle server 
    INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
    TEST(err == expectedErr);
    
    severity = ECmdCriticalSeverity;
    INFO_PRINTF1(_L("doTestRunCleCustomCommand, Test:ESsmFireAndForget, severity:ECmdCriticalSeverity"));
    iCmd = CCmdCustomCommand::NewL(severity, ESsmFireAndForget, *info);
    err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL; //ownership of iCmd is with Cle server
    INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
    TEST(err == expectedErr);
    
    severity = ECmdHighSeverity;
    INFO_PRINTF1(_L("doTestRunCleCustomCommand, Test:ESsmFireAndForget, severity:ECmdHighSeverity"));
    iCmd = CCmdCustomCommand::NewL(severity, ESsmFireAndForget, *info);
    err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL; //ownership of iCmd is with Cle server
    CleanupStack::PopAndDestroy(info);
    INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
    TEST(err == expectedErr);
    }

void CCmdRunCleTest::doTestRunCleCmdLoadSupL()
    {
    TInt err = KErrNone;    
    TInt expectedErr = KErrNone;
    TCmdErrorSeverity severity = ECmdIgnoreFailure;

    TUid uid;
    uid.iUid = 0;
    TInt32 ordinal = 1;
    TSsmSupInfo info;
    _LIT(KFileName, "tiCmdgoodsup.dll");
    info.SetL(KFileName, ordinal, uid);
    
    INFO_PRINTF1(_L("doTestRunCleiCmdLoadSup, Test:ESsmFireAndForget, severity:ECmdIgnoreFailure"));
    iCmd = CCmdLoadSup::NewL(severity, ESsmFireAndForget, KNumOfRetries, info);
    err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL;//ownership of iCmd is with Cle server
    INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
    TEST(err == expectedErr);
    
    severity = ECmdCriticalSeverity;
    INFO_PRINTF1(_L("doTestRunCleiCmdLoadSup, Test:ESsmFireAndForget, severity:ECmdCriticalSeverity"));
    iCmd = CCmdLoadSup::NewL(severity, ESsmFireAndForget, KNumOfRetries, info);
    err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL;//ownership of iCmd is with Cle server
    INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
    TEST(err == expectedErr);
    }

void CCmdRunCleTest::doTestRunCleCmdpublishSystemStateL()
    {
    TInt err = KErrNone;
    TInt expectedErr = KErrNone;
    TCmdErrorSeverity severity = ECmdIgnoreFailure;
    TSsmPublishSystemStateInfo info;
    TSsmState state(ESsmEmergencyCallsOnly, 0);
    TTestSsmDirection direction = testTraverseDefault;
    
    info.Set(state, direction, KNumOfRetries);
    
    INFO_PRINTF1(_L("doTestRunCleCmdpublishSystemState Test:ESsmFireAndForget severity:ECmdIgnoreFailure"));
    iCmd = CCmdPublishSystemState::NewL(severity, ESsmFireAndForget, info);
    err =  RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL;
    INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
    TEST(err == expectedErr);
    
    severity = ECmdCriticalSeverity;
    INFO_PRINTF1(_L("doTestRunCleCmdpublishSystemState Test:ESsmFireAndForget severity:ECmdCriticalSeverity"));
    iCmd = CCmdPublishSystemState::NewL(severity, ESsmFireAndForget, info);
    err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL;
    INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
    TEST(err == expectedErr);
    }

void CCmdRunCleTest::doTestRunCleCcmdreqSwpchangeL()
    {
    TInt err = KErrNone;
    TInt expectedErr = KErrNone;
    TCmdErrorSeverity severity = ECmdIgnoreFailure;
    TSsmExecutionBehaviour executionBehaviour = ESsmWaitForSignal;
    TSsmSwp info(KNewSwpKey, 876);
        
    INFO_PRINTF1(_L("Test:doTestRunCleCiCmdreqSwpchangeL Test:ESsmWaitForSignal severity:ECmdIgnoreFailure"));
    iCmd = CCmdReqSwpChange::NewL(severity, executionBehaviour, info);
    err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL;
    INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
    TEST(err == expectedErr);
    
    severity = ECmdCriticalSeverity;
    info.Set(KNewSwpKey2, 767);
    INFO_PRINTF1(_L("Test:doTestRunCleCiCmdreqSwpchangeL Test:ESsmWaitForSignal severity:ECmdCriticalSeverity"));
    iCmd = CCmdReqSwpChange::NewL(severity, executionBehaviour, info);
    err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL;
    INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
    TEST(err == expectedErr);
    }

void CCmdRunCleTest::doTestRunCleCmdStartAppL()
    {
    TCmdErrorSeverity severity = ECmdIgnoreFailure;
    TInt expectedErr = KErrNone;
       
    INFO_PRINTF1(_L("doTestRunCleCmdStartAppL Start a non existing application Test:ESsmFireAndForget severity:ECmdIgnoreFailure"));
    SetStartupPropertiesL(KBadFileName, KEmptyArgs, ESsmFireAndForget, ESsmCmdStartApp);
    iCmd = CCmdStartApp::NewL(severity,iStartupProperties);
    TInt err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL;
    INFO_PRINTF3(_L("Test completed with err: %d. Expected err : %d "), err,expectedErr);
    TEST(err == expectedErr);
    Reset();
    
    INFO_PRINTF2(_L("doTestRunCleCmdStartAppL Start a non existing application Test:ESsmFireAndForget severity:ECmdIgnoreFailure and retries %d"), KNumOfRetries);
    SetStartupPropertiesL(KBadFileName, KEmptyArgs, ESsmFireAndForget, ESsmCmdStartApp, KNumOfRetries);
    iCmd = CCmdStartApp::NewL(severity,iStartupProperties);
    err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL;
    INFO_PRINTF3(_L("Test completed with err: %d. Expected err : %d "), err,expectedErr);
    TEST(err == expectedErr);
    Reset();
    
    INFO_PRINTF2(_L("doTestRunCleCmdStartAppL Start a non existing application Test:ESsmFireAndForget severity:ECmdIgnoreFailure and timeout %d "), 10);
    SetStartupPropertiesL(KSlowStartApp, KEmptyArgs, ESsmFireAndForget, ESsmCmdStartApp, 0, 10);   //time out is given 10ms so that F&F iCmd finishes before below tests are finished 
    iCmd = CCmdStartApp::NewL(severity,iStartupProperties);
    err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL;
    INFO_PRINTF3(_L("Test completed with err: %d. Expected err : %d "), err,expectedErr);
    TEST(err == expectedErr);
    Reset();
    
    severity = ECmdHighSeverity;
    INFO_PRINTF1(_L("doTestRunCleCmdStartAppL Start a non existing application Test:ESsmFireAndForget severity:ECmdHighSeverity"));
    SetStartupPropertiesL(KBadFileName, KEmptyArgs, ESsmFireAndForget, ESsmCmdStartApp);
    iCmd = CCmdStartApp::NewL(severity,iStartupProperties);
    err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL;
    INFO_PRINTF3(_L("Test completed with err: %d. Expected err : %d "), err,expectedErr);
    TEST(err == expectedErr);
    Reset();
    
    INFO_PRINTF2(_L("doTestRunCleCmdStartAppL Start a non existing application Test:ESsmFireAndForget severity:ECmdHighSeverity and retries %d"), KNumOfRetries);
    SetStartupPropertiesL(KBadFileName, KEmptyArgs, ESsmFireAndForget, ESsmCmdStartApp, KNumOfRetries);
    iCmd = CCmdStartApp::NewL(severity,iStartupProperties);
    err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL;
    INFO_PRINTF3(_L("Test completed with err: %d. Expected err : %d "), err,expectedErr);
    TEST(err == expectedErr);
    Reset();
    
    INFO_PRINTF2(_L("doTestRunCleCmdStartAppL Start a non existing application Test:ESsmFireAndForget severity:ECmdHighSeverity and timeout %d "), 10);
    SetStartupPropertiesL(KSlowStartApp, KEmptyArgs, ESsmFireAndForget, ESsmCmdStartApp, 0, 10);   //time out is given 10ms so that F&F iCmd finishes before below tests are finished 
    iCmd = CCmdStartApp::NewL(severity,iStartupProperties);
    err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL;
    INFO_PRINTF3(_L("Test completed with err: %d. Expected err : %d "), err,expectedErr);
    TEST(err == expectedErr);
    Reset();
    }

void CCmdRunCleTest::doTestRunCleFandFAppWithMonitorL()
    {
    // The KIntervalForReLaunchRateOfFailure time interval is 30mins by default, its patched to 25secs on emulator in epoc.ini file
    // and for hardware in iby file.
    INFO_PRINTF1(_L("For test to pass in Winscw - (patchdata_sysmon_exe_KIntervalForReLaunchRateOfFailure 25) should be added to epoc32\\data\\epoc.ini file"));

    INFO_PRINTF1(_L("Test:doTestRunCleFandFAppWithMonitorL - command ECmdIgnoreFailure Severity"));
    TCmdErrorSeverity severity = ECmdIgnoreFailure;
    SetStartupPropertiesL(KTestAppGood, KEmptyArgs, ESsmFireAndForget, ESsmCmdStartApp, 1, 0);
    TSsmMonitorInfo monitorInfo;
    monitorInfo.iRestartPolicy = ESsmIgnoreOnFailure;
    monitorInfo.iRestartMode = 0;
    monitorInfo.iRetries = 1;
    monitorInfo.iTimeout = 0;   
    iStartupProperties->SetMonitorInfoL(monitorInfo);
    INFO_PRINTF1(_L("doTestRunCleFandFAppWithMonitorL:Start a application with execution behaviour as ESsmFireAndForget"));
    iCmd = CCmdStartApp::NewL(severity,iStartupProperties);
    TInt err = RunTestWithCleAndWaitL(iCmd);
        
    //Waiting to decrement the count of the semaphore which is being signalled as the application is started first time 
    iSem.Wait();
    
    INFO_PRINTF1(_L("Call Find and Kill now - instances should be one"));
    User::After(1000000);
    //find and kill the application
    TEST(1 == FindAndKill(KTestAppGood));
    
    INFO_PRINTF1(_L("-----------------------------------------------------------------------------"));
    INFO_PRINTF1(_L("Wait for one second and call Find and Kill again - instances should be zero"));
    User::After(KThrottleTime);
    // system monitor waits for 15secs(throttle time) before restarting the application
    TEST(0 == FindAndKill(KTestAppGood)); 
    
    INFO_PRINTF1(_L("-----------------------------------------------------------------------------"));
    INFO_PRINTF1(_L("Wait for Throttle time and call find and kill again - instances should be one"));
    //Waiting for the application to restart using semaphore
    iSem.Wait();
    
    // application is restarted by system monitor
    TEST(1 == FindAndKill(KTestAppGood));
    
    INFO_PRINTF1(_L("-----------------------------------------------------------------------------"));
    INFO_PRINTF1(_L("Wait for one second and call Find and Kill again - instances should be zero"));
    User::After(KThrottleTime);
    // application is restarted by system monitor
    TEST(0 == FindAndKill(KTestAppGood));
    
    INFO_PRINTF1(_L("-----------------------------------------------------------------------------"));
    INFO_PRINTF1(_L("Wait for Throttle time and call find and kill again - instances should be one"));
    //Waiting for the application to restart using semaphore
    iSem.Wait();
    
    // application is restarted by system monitor
    TEST(1 == FindAndKill(KTestAppGood));
    
    INFO_PRINTF1(_L("-----------------------------------------------------------------------------"));
    INFO_PRINTF1(_L("Wait for more than throttle time and call find and kill again - instances should be one"));
    INFO_PRINTF1(_L("Done to ensure that the app is closed and not hanging around"));
    //Waiting for the application to restart using semaphore
    iSem.Wait();
    
    TEST(1 == FindAndKill(KTestAppGood));
    //As this test case is successfully executed, reset all data
    INFO_PRINTF1(_L("Waiting simply now - just to check no proc is leftover"));
    User::After(KThrottleTime);
    TEST(0 == FindAndKill(KTestAppGood));
    iCmd = NULL;
    Reset();
    }

void CCmdRunCleTest::doTestRunCleCmdStartProcessL()
    {
    TCmdErrorSeverity severity = ECmdHighSeverity;
    TInt expectedErr = KErrNotFound;
    
    INFO_PRINTF1(_L("doTestRunCleCmdStartProcessL Start a non existing application Test:ESsmFireAndForget severity:ECmdHighSeverity"));
    // FireAndForget execution behaviour does not dependent on completion code so it will always return KErrNone
    expectedErr = KErrNone;
    SetStartupPropertiesL(KBadFileName, KEmptyArgs, ESsmFireAndForget, ESsmCmdStartProcess);
    iCmd = CCmdStartProcess::NewL(severity,iStartupProperties);
    TInt err = RunTestWithCleAndWaitL(iCmd);
    INFO_PRINTF3(_L("Test completed with err: %d. Expected err : %d "), err,expectedErr);
    iCmd = NULL;
    TEST(err == expectedErr);
    Reset();
        
    INFO_PRINTF2(_L("doTestRunCleCmdStartProcessL Start a non existing application Test:ESsmFireAndForget severity:ECmdHighSeverity and retries %d"), KNumOfRetries);
    SetStartupPropertiesL(KBadFileName, KEmptyArgs, ESsmFireAndForget, ESsmCmdStartProcess, KNumOfRetries);
    iCmd = CCmdStartProcess::NewL(severity,iStartupProperties);
    err = RunTestWithCleAndWaitL(iCmd);
    INFO_PRINTF3(_L("Test completed with err: %d. Expected err : %d "), err,expectedErr);
    iCmd = NULL;
    TEST(err == expectedErr);
    Reset();
        
    INFO_PRINTF2(_L("doTestRunCleCmdStartProcessL Start a non existing application Test:ESsmFireAndForget severity:ECmdHighSeverity and timeout %d "), 10);
    SetStartupPropertiesL(KSlowStartApp, KEmptyArgs, ESsmFireAndForget, ESsmCmdStartProcess, 0, 10);
    iCmd = CCmdStartProcess::NewL(severity,iStartupProperties);
    err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL;
    INFO_PRINTF3(_L("Test completed with err: %d. Expected err : %d "), err,expectedErr);
    TEST(err == expectedErr);
    Reset();
    }

void CCmdRunCleTest::doTestRunCleFandFProcWithMonitorL()
    {
    // The KIntervalForReLaunchRateOfFailure time interval is 30mins by default, its patched to 25secs on emulator in epoc.ini file
    // and for hardware in iby file.
    INFO_PRINTF1(_L("For test to pass in Winscw - (patchdata_sysmon_exe_KIntervalForReLaunchRateOfFailure 25) should be added to epoc32\\data\\epoc.ini file"));
    
    INFO_PRINTF1(_L("Test:doTestRunCleFandFProcWithMonitorL - command ECmdIgnoreFailure Severity"));
    
    TCmdErrorSeverity severity = ECmdIgnoreFailure;
    SetStartupPropertiesL(KGoodProc, KEmptyArgs, ESsmFireAndForget, ESsmCmdStartProcess, 1, 0);
    TSsmMonitorInfo monitorInfo;
    monitorInfo.iRestartPolicy = ESsmIgnoreOnFailure;
    monitorInfo.iRestartMode = 0;
    monitorInfo.iRetries = 1;
    monitorInfo.iTimeout = 0;   
    iStartupProperties->SetMonitorInfoL(monitorInfo);
    INFO_PRINTF1(_L("doTestRunCleFandFProcWithMonitorL:Start a process with execution behaviour as ESsmFireAndForget"));
    iCmd = CCmdStartProcess::NewL(severity,iStartupProperties);
    TInt err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL;
    
    //Waiting to decrement the count of the semaphore which is being signalled as the application is started first time 
    iSem.Wait();
        
    INFO_PRINTF1(_L("Call Find and Kill now - instances should be one"));
    User::After(1000000);
    //find and kill the process
    TEST(1 == FindAndKill(KGoodProc));
    
    INFO_PRINTF1(_L("-----------------------------------------------------------------------------"));
    INFO_PRINTF1(_L("Wait for one second and call Find and Kill again - instances should be zero"));
    User::After(KThrottleTime);
    // system monitor waits for 15secs(throttle time) before restarting the process
    TEST(0 == FindAndKill(KGoodProc)); 
    
    INFO_PRINTF1(_L("-----------------------------------------------------------------------------"));
    INFO_PRINTF1(_L("Wait for Throttle time and call find and kill again - instances should be one"));
    //Waiting for the process to restart using semaphore
    iSem.Wait();
    // Process is restarted by system monitor
    TEST(1 == FindAndKill(KGoodProc));
    
    INFO_PRINTF1(_L("-----------------------------------------------------------------------------"));
    INFO_PRINTF1(_L("Wait for one second and call Find and Kill again - instances should be zero"));
    User::After(KThrottleTime);
    // application is restarted by system monitor
    TEST(0 == FindAndKill(KGoodProc));
    
    INFO_PRINTF1(_L("-----------------------------------------------------------------------------"));
    INFO_PRINTF1(_L("Wait for Throttle time and call find and kill again - instances should be one"));
    //Waiting for the process to restart using semaphore
    iSem.Wait();
    // application is restarted by system monitor
    TEST(1 == FindAndKill(KGoodProc));
    
    INFO_PRINTF1(_L("-----------------------------------------------------------------------------"));
    INFO_PRINTF1(_L("Wait for more than throttle time and call find and kill again - instances should be one"));
    //INFO_PRINTF1(_L("Done to ensure that the app is closed and not hanging around"));
    //Waiting for the process to restart using semaphore
    iSem.Wait();
    TEST(1 == FindAndKill(KGoodProc));
    
    //As this test case is successfully executed, reset all data
    Reset();
    }

void CCmdRunCleTest::doTestRunCleFandFAlreadyRunWithMonitorL()
    {
    // The KIntervalForReLaunchRateOfFailure time interval is 30mins by default, its patched to 25secs on emulator in epoc.ini file
    // and for hardware in iby file.
    INFO_PRINTF1(_L("For test to pass in Winscw - (patchdata_sysmon_exe_KIntervalForReLaunchRateOfFailure 25) should be added to epoc32\\data\\epoc.ini file"));

    INFO_PRINTF1(_L("Test:doTestRunCleFandFAlreadyRunWithMonitorL - command ECmdIgnoreFailure Severity"));
    
    //Create and launch the process
    RProcess process;
    CleanupClosePushL(process);
    User::LeaveIfError(process.Create(KGoodProc, KLaunchServerCommandLineOption));
    
    TCmdErrorSeverity severity = ECmdIgnoreFailure;
    SetStartupPropertiesL(KGoodProc, KEmptyArgs, ESsmFireAndForget, ESsmCmdStartProcess, 1, 0);
    TSsmMonitorInfo monitorInfo;
    monitorInfo.iRestartPolicy = ESsmIgnoreOnFailure;
    monitorInfo.iRestartMode = 0;
    monitorInfo.iRetries = 1;
    monitorInfo.iTimeout = 0;   
    iStartupProperties->SetMonitorInfoL(monitorInfo);
    INFO_PRINTF1(_L("doTestRunCleFandFAlreadyRunWithMonitorL:Start an already running process with execution behaviour as ESsmFireAndForget"));
    
    //Try and launch the process again, SysMon will monitor already running process
    iCmd = CCmdStartProcess::NewL(severity,iStartupProperties);
    TInt err = RunTestWithCleAndWaitL(iCmd);
    iCmd = NULL;

    //Waiting to decrement the count of the semaphore which is being signalled as the application is started first time 
    iSem.Wait();

    INFO_PRINTF1(_L("Call Find and Kill now - instances should be two"));
    //find and kill both the processes
    User::After(1000000);
    TEST(2 == FindAndKill(KGoodProc));
    
    INFO_PRINTF1(_L("-----------------------------------------------------------------------------"));
    INFO_PRINTF1(_L("Wait for one second and call Find and Kill again - instances should be zero"));
    User::After(KThrottleTime);
    // system monitor waits for 15secs(throttle time) before restarting the process
    TEST(0 == FindAndKill(KGoodProc)); 
    
    INFO_PRINTF1(_L("-----------------------------------------------------------------------------"));
    INFO_PRINTF1(_L("Wait for Throttle time and call find and kill again - instances should be one"));
    //Waiting for the process to restart using semaphore
    iSem.Wait();
    // Process is restarted by system monitor
    TEST(1 == FindAndKill(KGoodProc));
    
    INFO_PRINTF1(_L("-----------------------------------------------------------------------------"));
    INFO_PRINTF1(_L("Wait for one second and call Find and Kill again - instances should be zero"));
    User::After(KThrottleTime);
    // Process is restarted by system monitor
    TEST(0 == FindAndKill(KGoodProc));
    
    INFO_PRINTF1(_L("-----------------------------------------------------------------------------"));
    INFO_PRINTF1(_L("Wait for Throttle time and call find and kill again - instances should be one"));
    //Waiting for the process to restart using semaphore
    iSem.Wait();
    // Process is restarted by system monitor
    TEST(1 == FindAndKill(KGoodProc));
        
    INFO_PRINTF1(_L("-----------------------------------------------------------------------------"));
    INFO_PRINTF1(_L("Wait for more than throttle time and call find and kill again - instances should be one"));
    INFO_PRINTF1(_L("Done to ensure that the app is closed and not hanging around"));
    //Waiting for the process to restart using semaphore
    iSem.Wait();
    TEST(1 == FindAndKill(KGoodProc));
        
    //As this test case is successfully executed, reset all data
    Reset();
    CleanupStack::PopAndDestroy();
    }

/**
Helper functiosn to set specified values in iStartupProperties.
*/
void CCmdRunCleTest::SetStartupPropertiesL(const TDesC& aFileName, const TDesC& aArgs, TSsmExecutionBehaviour aExecutionBehaviour, TSsmCommandType aSsmCommandType, TInt aNumOfRetries, TInt32 aTimeout)
    {
    iStartupProperties->Reset();
    iStartupProperties->SetFileParamsL(aFileName, aArgs);
    iStartupProperties->SetCommandTypeL(aSsmCommandType);
    iStartupProperties->SetExecutionBehaviour(aExecutionBehaviour);
    iStartupProperties->SetRetries(aNumOfRetries);
    iStartupProperties->SetTimeout(aTimeout);
    }

/**
Resets iStartupProperties
*/
void CCmdRunCleTest::Reset()
    {
    iStartupProperties->Reset();
    }

//
// Helper functions for StartApp and StartProcess commands
//
/**
Helper function to find and kill the specified process
*/
TInt CCmdRunCleTest::FindAndKill(const TDesC& aProcessName)
    {
    TFullName searchTerm(aProcessName);
    StripExtension(searchTerm);
    searchTerm += _L("*");
    TFindProcess find(searchTerm);
    TFullName name;
    TInt instancesFound = 0;
    while(find.Next(name) == KErrNone)
        {
        RProcess process;
        const TInt err = process.Open(find);

        if (KErrNone == err)
            {
            if (process.ExitType() == EExitPending)
                {
                instancesFound++;
                process.Kill(KErrCancel);
                INFO_PRINTF3(_L("Process %S found and killed with Exit Reason = %d"), &aProcessName, process.ExitReason());
                }
            process.Close();
            }
        }
    INFO_PRINTF2(_L("No of instances of process killed = %d"), instancesFound);
    return instancesFound;
    }

/**
Helper function to strip extension
*/
void CCmdRunCleTest::StripExtension(TDes& aFilename)
    {
    TInt dot = aFilename.Find(_L("."));

    if(KErrNotFound != dot)
        {
        aFilename.SetLength(dot);
        }
    }
