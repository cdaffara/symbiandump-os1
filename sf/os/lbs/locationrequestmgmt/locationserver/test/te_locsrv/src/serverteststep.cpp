// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example CTestStep derived implementation
// 
//

/**
 @file ServerTestStep.cpp
 @internalTechnology
*/
#include "serverteststep.h"

CServerTestStep::~CServerTestStep()
/**
 * Destructor
 */
	{
	}

CServerTestStep::CServerTestStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KServerTestStep);
	}

TVerdict CServerTestStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_locsrvSuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CServerTestStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	   if (TestStepResult()==EPass)
	        {
	        TInt testIndex = KErrNotFound;
	        if(!GetIntFromConfig(ConfigSection(),KTe_locsrvSuiteInt,testIndex))
	            {
	            User::Leave(KErrNotFound);
	            }
	        else
	            {
	            TestByIndexL(testIndex);
	            }
	        }
	    return TestStepResult();
	    }

void CServerTestStep::TestByIndexL(TInt aIndex)
    {
    switch (aIndex)
        {
        case 0: 
            _LIT(KTestName0, "Connect");
            INFO_PRINTF1(KTestName0);
            TdLocSrv_Server_ConnectL();
            break;
        case 1: 
            _LIT(KTestName1, "Close Positioner Open");
            INFO_PRINTF1(KTestName1);
            TdLocSrv_Server_Close_Positioner_OpenL();
            break;
        case 2: 
            _LIT(KTestName2, "Anything Before Connect");
            INFO_PRINTF1(KTestName2);
            TdLocSrv_Server_Anything_Before_ConnectL();
            break;
        case 3: 
            _LIT(KTestName3, "Double_Connect");
            INFO_PRINTF1(KTestName3);
            TdLocSrv_Server_Double_ConnectL();
            break;
        case 4: 
            _LIT(KTestName4, "WrongCancelRequest2");
            INFO_PRINTF1(KTestName4);
            TdLocSrv_Server_WrongCancelRequest2L();
            break;
        case 5: 
            _LIT(KTestName5, "Module Info");
            INFO_PRINTF1(KTestName5);
            TdLocSrv_Server_Module_InfoL();
            break;
        case 6: 
            _LIT(KTestName6, "Module Info Invalid");
            INFO_PRINTF1(KTestName6);
            TdLocSrv_Server_Module_Info_InvalidL();
            break;
        case 7: 
            _LIT(KTestName7, "Module Status Sync");
            INFO_PRINTF1(KTestName7);
            TdLocSrv_Server_Module_Status_SyncL();
            break;
        case 8: 
            _LIT(KTestName8, "Module Status Async");
            INFO_PRINTF1(KTestName8);
            TdLocSrv_Server_Module_Status_AsyncL();
            break;
        case 9: 
            _LIT(KTestName9, "Bad ModuleStatus Requests");
            INFO_PRINTF1(KTestName9);
            TdLocSrv_Server_Bad_Module_Status_RequestsL();
            break;
        case 10: 
            _LIT(KTestName10, "Close With Requests");
            INFO_PRINTF1(KTestName10);
            TdLocSrv_Server_Close_With_RequestsL();
            break;
        case 11: 
            _LIT(KTestName11, "Double CancelRequest");
            INFO_PRINTF1(KTestName11);
            TdLocSrv_Server_Double_CancelRequestL();
            break;
        case 12: 
            _LIT(KTestName12, "Double NotifyStatus");
            INFO_PRINTF1(KTestName12);
            TdLocSrv_Server_Double_NotifyStatusL();
            break;
        case 13: 
            _LIT(KTestName13, "Version");
            INFO_PRINTF1(KTestName13);
            TdLocSrv_Server_VersionL();
            break;
        default: 
            _LIT(KTestName25, "This test step doesn't exist");
            ERR_PRINTF1(KTestName25);
            SetTestStepResult(EFail);
            break;
        }
    }

TVerdict CServerTestStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	
	}

extern TInt SetAuthenticRequestor(RPositioner& aPositioner);

//
// 
// RPositionServer
//
//

// 1.1.1 Connect
TVerdict CServerTestStep::TdLocSrv_Server_ConnectL()
{
    RPositionServer server1, server2;

    // try to make two connections right now

    __UHEAP_MARK;
    CheckExpectedResult( 0, server1.Handle() );
    CheckExpectedResult( KErrNone, server1.Connect() );
    CheckResult( server1.Handle() > 0 );

    CheckExpectedResult( 0, server2.Handle() );
    CheckExpectedResult( KErrNone, server2.Connect() );
    CheckResult( server2.Handle() > 0 );

    server2.Close();
    server1.Close();
    __UHEAP_MARKEND;

    return TestStepResult();
}

/*
// 1.1.2 Connect but server doesn't start
see Noservertests.cpp
*/

// 1.1.3 Closing when positioner is opened
TInt Close_Positioner_Open(TAny*/* aPtr*/)
{
    RPositionServer server;
    server.Connect();
    RPositioner positioner;
    positioner.Open(server);

    server.Close(); // should panic

    TPositionUpdateOptions updateOptions;
    if (KErrDisconnected != positioner.GetUpdateOptions(updateOptions))
        {
        User::Panic(KPosClientFault, KErrGeneral);
        }

    positioner.Close();
    return KErrNone;
}

TVerdict CServerTestStep::TdLocSrv_Server_Close_Positioner_OpenL()
{
    // do LEGAL in other thread
    DO_PANIC_TEST_L(Close_Positioner_Open, _L("KERN-EXEC"), KErrNone, KDefaultTestTimeout);

    return TestStepResult();
}

// 1.1.4 Other methods before Connect()
TInt CancelRequest_Before_Connect(TAny* /*aPtr*/)
{
    RPositionServer server;
    server.CancelRequest(1);
    return KErrGeneral;
}
TInt GetDefaultModuleId_Before_Connect(TAny* /*aPtr*/)
{
    RPositionServer server;
    TPositionModuleId moduleId;
    server.GetDefaultModuleId(moduleId);
    return KErrGeneral;
}
TInt GetNumModules_Before_Connect(TAny* /*aPtr*/)
{
    RPositionServer server;
    TUint numModules;
    server.GetNumModules(numModules);
    return KErrGeneral;
}
TInt GetModuleInfoByIndex_Before_Connect(TAny* /*aPtr*/)
{
    RPositionServer server;
    TPositionModuleInfo modInfo;
    server.GetModuleInfoByIndex(0, modInfo);
    return KErrGeneral;
}
TInt GetModuleInfoById_Before_Connect(TAny* /*aPtr*/)
{
    RPositionServer server;
    TPositionModuleInfo modInfo;
    server.GetModuleInfoById(TUid::Uid(0), modInfo);
    return KErrGeneral;
}
TInt GetModuleStatus_Before_Connect(TAny* /*aPtr*/)
{
    RPositionServer server;
    TPositionModuleStatus modStatus;
    server.GetModuleStatus(modStatus, TUid::Uid(0));
    return KErrGeneral;
}
TInt NotifyModuleStatusEvent_Before_Connect(TAny* /*aPtr*/)
{
    RPositionServer server;
    TRequestStatus status;
    TPositionModuleStatusEvent event;
    server.NotifyModuleStatusEvent(event, status);
    return KErrGeneral;
}
TInt Close_Before_Connect(TAny* /*aPtr*/)
{
    RPositionServer server;
    server.Close();
    return KErrGeneral;
}
TVerdict CServerTestStep::TdLocSrv_Server_Anything_Before_ConnectL()
{
    DO_PANIC_TEST_L(CancelRequest_Before_Connect, KPosClientFault, 
                    EPositionServerBadHandle, KDefaultTestTimeout);
    DO_PANIC_TEST_L(GetDefaultModuleId_Before_Connect, KPosClientFault, 
                    EPositionServerBadHandle, KDefaultTestTimeout);
    DO_PANIC_TEST_L(GetNumModules_Before_Connect, KPosClientFault, 
                    EPositionServerBadHandle, KDefaultTestTimeout);
    DO_PANIC_TEST_L(GetModuleInfoByIndex_Before_Connect, KPosClientFault, 
                    EPositionServerBadHandle, KDefaultTestTimeout);
    DO_PANIC_TEST_L(GetModuleInfoById_Before_Connect, KPosClientFault, 
                    EPositionServerBadHandle, KDefaultTestTimeout);
    DO_PANIC_TEST_L(GetModuleStatus_Before_Connect, KPosClientFault, 
                    EPositionServerBadHandle, KDefaultTestTimeout);
    DO_PANIC_TEST_L(NotifyModuleStatusEvent_Before_Connect, KPosClientFault, 
                    EPositionServerBadHandle, KDefaultTestTimeout);
/* This behavior is allowed 
    DO_PANIC_TEST_L(Close_Before_Connect, KPosClientFault, 
                    EPositionServerBadHandle, KDefaultTestTimeout);
*/

    return TestStepResult();
}

// 1.1.5 Double Connect
TInt ConnectRPositionServerTwice(TAny* /*aPtr*/)
{
    RPositionServer posServer;
    TInt err = posServer.Connect();
    if(err!=KErrNone)
    {
        return err;
    }
    err = posServer.Connect();  //This should panic. If it does return, we return the value.
    return err;
}

TVerdict CServerTestStep::TdLocSrv_Server_Double_ConnectL()
{
    DO_PANIC_TEST_L(ConnectRPositionServerTwice, KPosClientFault, 
                    EPositionServerHandleNotClosed, KDefaultTestTimeout);
    return TestStepResult();
}

// 1.1.16 Wrong Cancel Request 2
TVerdict CServerTestStep::TdLocSrv_Server_WrongCancelRequest2L()
{
    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    RPositioner positioner;
    User::LeaveIfError(positioner.Open(server));
    CleanupClosePushL(positioner);

    TInt res;

    _LIT(KService, "TC1.1.16");
    res = positioner.SetRequestor(CRequestor::ERequestorService, CRequestor::EFormatApplication, KService);

    res = positioner.CancelRequest(EPositionerNotifyPositionUpdate + 1);
    CheckExpectedResult(res, KErrNotSupported);

    res = server.CancelRequest(EPositionServerNotifyModuleStatusEvent + 1);
    CheckExpectedResult(res, KErrNotSupported);

    res = server.CancelRequest(EPositionServerCancelAsyncRequest - 1);
    CheckExpectedResult(res, KErrNotSupported);

    res = server.CancelRequest(EPositionerNotifyPositionUpdate);
    CheckExpectedResult(res, KErrNotSupported);

    res = positioner.CancelRequest(EPositionServerNotifyModuleStatusEvent);
    CheckExpectedResult(res, KErrNotSupported);

    CleanupStack::PopAndDestroy(&positioner);
    CleanupStack::PopAndDestroy(&server);

    return TestStepResult();
}

//
// Module Info
//

// 1.2.1 Getting Module Info
TBool CServerTestStep::CompareModuleInfo(TPositionModuleInfo& m1, TPositionModuleInfo& m2)
{
    CHECK_EQUAL_PROPS(m1, m2, ModuleId().iUid);
    CHECK_EQUAL_PROPS(m1, m2, IsAvailable());
    CHECK_EQUAL_PROPS(m1, m2, TechnologyType());
    CHECK_EQUAL_PROPS(m1, m2, DeviceLocation());
    CHECK_EQUAL_PROPS(m1, m2, Capabilities());
    CHECK_EQUAL_PROPS(m1, m2, ClassesSupported(EPositionInfoFamily));
    CHECK_EQUAL_PROPS(m1, m2, Version().Name());

    return ETrue;
}
TBool CServerTestStep::CompareModuleQuality(TPositionModuleInfo& m1, TPositionModuleInfo& m2)
{
    TPositionQuality q1, q2;
    m1.GetPositionQuality(q1);
    m2.GetPositionQuality(q2);
    CHECK_EQUAL_PROPS(q1, q2, TimeToFirstFix());
    CHECK_EQUAL_PROPS(q1, q2, TimeToNextFix()); 
    CHECK_EQUAL_PROPS(q1, q2, CostIndicator());
    CHECK_EQUAL_PROPS(q1, q2, PowerConsumption());

    TReal v1 = q1.HorizontalAccuracy();
    TReal v2 = q2.HorizontalAccuracy();

    if (!Math::IsNaN(v1) && !Math::IsNaN(v2))
        {
        CheckExpectedResult(v1, v2, KFailedPropertyCheck); 
        }
    else 
        { 
        if(Math::IsNaN(v1) && !Math::IsNaN(v2) ||
                !Math::IsNaN(v1) && Math::IsNaN(v2))
            {
            ERR_PRINTF1(KOnlyOneNaN);
            SetTestStepResult(EFail);
            }
        }

    v1 = q1.VerticalAccuracy();
    v2 = q2.VerticalAccuracy();

    if (!Math::IsNaN(v1) && !Math::IsNaN(v2))
        {
        CheckExpectedResult(v1, v2, KFailedPropertyCheck); 
        }
    else 
        { 
        if(Math::IsNaN(v1) && !Math::IsNaN(v2) ||
                !Math::IsNaN(v1) && Math::IsNaN(v2))
            {
            ERR_PRINTF1(KOnlyOneNaN);
            SetTestStepResult(EFail);
            }
        }

    return ETrue;
}
TBool CServerTestStep::CompareModuleNames(TPositionModuleInfo& m1, TPositionModuleInfo& m2)
{
    TBuf<KPositionMaxModuleName> n1, n2;
    m1.GetModuleName(n1);
    m2.GetModuleName(n2);
    if(n1.Compare(n2) != 0)
        {
        ERR_PRINTF1(KModulesNamesDiffer);
        SetTestStepResult(EFail);
        return EFalse;
        }
    return ETrue;
}

void GetModuleInfoFromCenRepL(TPositionModuleId aModuleId, TPositionModuleInfo& aInfo)
{
    __UHEAP_MARK;
    CPosModules* modules = CPosModules::OpenL();
    CleanupStack::PushL(modules);
    modules->GetModuleInfoL(aModuleId, aInfo);
    CleanupStack::PopAndDestroy(modules);
    __UHEAP_MARKEND;
}

TVerdict CServerTestStep::TdLocSrv_Server_Module_InfoL()
{
    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    TInt res = KErrNone;

    // default module
    TPositionModuleId moduleId;
    res = server.GetDefaultModuleId(moduleId);
    if((res != KErrNone) && (res != KErrNotFound))
        {
        ERR_PRINTF1(KFailedGetDefaultModuleId);
        SetTestStepResult(EFail);
        }
    
#ifdef _TEST_SERVER_
    CheckExpectedResult( DUMMY_MODULE_ID_DEF, moduleId.iUid);
#endif

    // number of modules
    TUint numModules;
    res = server.GetNumModules(numModules);
    CheckExpectedResult( res, KErrNone, KFailedGetModulesAmount);
#ifdef _TEST_SERVER_
    CheckExpectedResult( DUMMY_NUM_MODULES, numModules);
#endif

    TBuf<80> logMsg;
    for (TUint i = 0; i < numModules; i++)
    {
        TPositionModuleInfo modInfo1, modInfo2;

        res = server.GetModuleInfoByIndex(i, modInfo1);
        CheckExpectedResult( res, KErrNone, KFailedGetModulesByIndex);
        moduleId = modInfo1.ModuleId();
        res = server.GetModuleInfoById(moduleId, modInfo2);
        CheckExpectedResult( res, KErrNone, KFailedGetModulesByID);

        TBuf<KPositionMaxModuleName> modName;
        modInfo2.GetModuleName(modName);

        INFO_PRINTF3(KCheckingModule, i, &modName);
        
        CompareModuleInfo(modInfo1, modInfo2);
        CompareModuleQuality(modInfo1, modInfo2);
        CompareModuleNames(modInfo1, modInfo2);

        // check if are equal to pre-defined value
        TPositionModuleInfo preInfo;
        
        // search for predefined data
        INFO_PRINTF1(KComparingWithCenRep);
        
        TRAPD(err, GetModuleInfoFromCenRepL(moduleId, preInfo));
        CheckExpectedResult(err, KErrNone, KFailedGetModulesInfoFromCenRep);

        CompareModuleInfo(preInfo, modInfo2);
        CompareModuleQuality(preInfo, modInfo2);
        CompareModuleNames(preInfo, modInfo2);
    }
    INFO_PRINTF1(KDone);
    CleanupStack::PopAndDestroy(&server);

    return TestStepResult();
}

// 1.2.2 Invalid retrieval of module Info
TVerdict CServerTestStep::TdLocSrv_Server_Module_Info_InvalidL()
{
    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    TPositionModuleInfo modInfo;

    // number of modules
    TUint numModules;
    CheckExpectedResult( KErrNone, server.GetNumModules(numModules));
    CheckExpectedResult( KErrNotFound, server.GetModuleInfoByIndex(-1, modInfo));
    CheckExpectedResult( KErrNotFound, server.GetModuleInfoByIndex(numModules, modInfo));
    CheckExpectedResult( KErrNotFound, server.GetModuleInfoByIndex(numModules + 1, modInfo));

    CleanupStack::PopAndDestroy(&server);

    return TestStepResult();
}

//
// Requests
//

// 1.4.1 Synchronous status request
TBool CServerTestStep::CompareModuleStatus(TPositionModuleStatus& s1, TPositionModuleStatus& s2)
{
    CHECK_EQUAL_PROPS(s1, s2, DeviceStatus());
    CHECK_EQUAL_PROPS(s1, s2, DataQualityStatus());

    return ETrue;
}
TVerdict CServerTestStep::TdLocSrv_Server_Module_Status_SyncL()
{
    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    TInt res = KErrNone;

    TUint numModules;
    res = server.GetNumModules(numModules);
    CheckExpectedResult( res, KErrNone, KFailedGetNumModules);
#ifdef _TEST_SERVER_
    CheckExpectedResult( DUMMY_NUM_MODULES, numModules);
#else
    numModules = numModules < DUMMY_NUM_MODULES ? numModules : DUMMY_NUM_MODULES; // we support only 3 modules for real server
#endif
    TPositionModuleStatus modStatus;
    for (TUint i = 0; i < numModules; i++)
    {
        // id by index?
        TPositionModuleInfo modInfo;
        res = server.GetModuleInfoByIndex(i, modInfo);
        CheckExpectedResult(res, KErrNone, KFailedGetModInfoByIndex);
        res = server.GetModuleStatus(modStatus, modInfo.ModuleId());
        CheckExpectedResult(res, KErrNone, KFailedGetModStatus);

#ifdef _TEST_SERVER_
        // check if are equal to pre-defined values
        TPositionModuleStatus preStatus;
        GET_TC_ARRAY_PARAMETER(ModuleStatus, _L8("RPositionServer_Module_Status_Sync")/*KTestID*/, preStatus, i);
        
        CheckResult(CompareModuleStatus(this, preStatus, modStatus));
#endif
    }

    // Check bad module IDs
    res = server.GetModuleStatus(modStatus, TUid::Uid(DUMMY_MODULE_BAD_ID));
    CheckExpectedResult(res, KErrNotFound, KWrongResult);

    CleanupStack::PopAndDestroy(&server);

    return TestStepResult();
}

// 1.4.2 Asyncronous module status request
TVerdict CServerTestStep::TdLocSrv_Server_Module_Status_AsyncL()
{
    StandardPrepareL();

    // Note that stub server does not check type of requested events
    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    TPositionModuleId moduleId;
    moduleId.iUid = KLcfPsy6UidValue;

    RPositioner positioner;
    User::LeaveIfError(positioner.Open(server, moduleId));
    CleanupClosePushL(positioner);
    SetAuthenticRequestor(positioner);

    TPositionModuleStatus status;
    TPckgC<TPositionModuleStatus> pkgStatus(status);
    TPositionModuleStatusEvent statusEvent;
    TRequestStatus serverRequest, positionerRequest, timerRequest;
    RTimer timer;
    timer.CreateLocal();
    CleanupClosePushL(timer);
    
    HPositionGenericInfo* genInfo = HPositionGenericInfo::NewLC();

    TInt requestedEvents = TPositionModuleStatusEvent::EEventDeviceStatus |
                           TPositionModuleStatusEvent::EEventDataQualityStatus;

    TBool subscriptionMade = EFalse;

    // Device Status reported by PSY
    for (TInt deviceStatus = TPositionModuleStatus::EDeviceError;
        deviceStatus < TPositionModuleStatus::EDeviceActive;
        deviceStatus++)
        {
        INFO_PRINTF2(KCheckingDeviceStatus, deviceStatus);
        
        // what we want to receive
        status.SetDeviceStatus(deviceStatus);
        
        TInt qualityStatus = TPositionModuleStatus::EDataQualityUnknown;
        switch (deviceStatus)
            {
            case TPositionModuleStatus::EDeviceError:
            case TPositionModuleStatus::EDeviceDisabled:
                qualityStatus = TPositionModuleStatus::EDataQualityLoss;
                break;
            case TPositionModuleStatus::EDeviceInactive:
            case TPositionModuleStatus::EDeviceInitialising:
            case TPositionModuleStatus::EDeviceStandBy:
                qualityStatus = TPositionModuleStatus::EDataQualityPartial;
                break;
            case TPositionModuleStatus::EDeviceReady:
            case TPositionModuleStatus::EDeviceActive:
                qualityStatus = TPositionModuleStatus::EDataQualityNormal;
                break;
            }
        status.SetDataQualityStatus(qualityStatus);
        
        // instruct psy to fire request
        genInfo->SetRequestedField(KPSY6FieldReportStatus);
        genInfo->SetValue(KPSY6FieldReportStatus, pkgStatus);
        
        // get events from server
        statusEvent.SetRequestedEvents(requestedEvents);

        if (!subscriptionMade)
            {
            // request module status before it happens
            server.NotifyModuleStatusEvent(statusEvent, serverRequest);
            User::After(KFifthSecond); // request reaches server
            }
        
        // psy will fire event
        PositionRequestWithCheck(*genInfo, positionerRequest, KErrNone, moduleId, positioner);
        
        if (subscriptionMade)
            {
            // request module status after it happens
            // server queues events for slow clients
            server.NotifyModuleStatusEvent(statusEvent, serverRequest);
            }

        timer.After(timerRequest, KSecond);
        User::WaitForRequest(timerRequest, serverRequest);
        
        // handle request with timeout
        TInt serverResult = serverRequest.Int();
        TInt timerResult = timerRequest.Int();
        if (timerRequest.Int() != KRequestPending)
            {
            subscriptionMade = EFalse;
            server.CancelRequest(EPositionServerNotifyModuleStatusEvent);
            User::WaitForRequest(serverRequest);
            }
        else
            {
            timer.Cancel();
            User::WaitForRequest(timerRequest);
            }
                
        // this state ignored by server if sent by PSY
        if (deviceStatus != TPositionModuleStatus::EDeviceDisabled) 
            {
            CheckExpectedResult(serverResult, KErrNone, KFailedModuleStatusRequest);
            CheckExpectedResult(timerResult, KRequestPending, KModuleStatusRequestTimedOut);
                
            // check reported data
            TPositionModuleStatus reported;
            statusEvent.GetModuleStatus(reported);
            
            if(!(statusEvent.OccurredEvents() & requestedEvents))
                {
                ERR_PRINTF1(KUnexpectedEvents);
                SetTestStepResult(EFail);
                }
            CheckExpectedResult(reported.DeviceStatus() , deviceStatus, KWrongStatus);
            CheckExpectedResult(reported.DataQualityStatus(), qualityStatus, KWrongQualityStatus);

            subscriptionMade = ETrue; // we are in Server's list from now on
            }
        else
            {
            CheckResult(timerResult == KErrNone);
            CheckExpectedResult(serverResult, KRequestPending, KEDeviceDisabledReportedFromPSY);
    
            subscriptionMade = EFalse;
            }
        }
        
    // Events reported by Location Settings
    // TBD
            
    // Request and Cancel immediately
    server.NotifyModuleStatusEvent(statusEvent, serverRequest, moduleId);
    server.CancelRequest(EPositionServerNotifyModuleStatusEvent);
    User::WaitForRequest(serverRequest);
    CheckExpectedResult(serverRequest.Int(), KErrCancel, KFailedCancelModuleStatusRequest);

    CleanupStack::PopAndDestroy(4, &server); // positioner, timer, genInfo

    StandardCleanup();
    return TestStepResult();
}

// 1.4.3 Bad module status request calls
TVerdict CServerTestStep::TdLocSrv_Server_Bad_Module_Status_RequestsL()
{
    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    // cancel w/o request
    CheckExpectedResult(KErrNotFound, server.CancelRequest(EPositionServerNotifyModuleStatusEvent));

    // bad module id
    TRequestStatus reqStatus;
    TPositionModuleStatusEvent statusEvent;

    statusEvent.SetRequestedEvents(TPositionModuleStatusEvent::EEventAll);
    server.NotifyModuleStatusEvent(statusEvent, reqStatus, TUid::Uid(DUMMY_MODULE_BAD_ID));

    User::WaitForRequest(reqStatus);
    CheckExpectedResult( KErrNotFound, reqStatus.Int());

    CleanupStack::PopAndDestroy(&server);

    return TestStepResult();
}

// 1.4.4 Close when requests not completed
TInt RPositionServer_Close_With_Requests(TAny* /*aPtr*/)
{
    // Close with notifications and/or requests outstanding
    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);


    TPositionModuleStatusEvent statusEvent;
    TRequestStatus reqStatus;
    statusEvent.SetRequestedEvents(DUMMY_STATUS_EVENT_REQ_EVENTS);
    server.NotifyModuleStatusEvent(statusEvent, reqStatus);

    User::After(TTimeIntervalMicroSeconds32(50*1000)); // 50 ms, request reaches server

    server.Close(); // this should panic ???
    // Calling close will stop the request ever completing???
    // No need to do a User::WaitForRequest().

    User::After(TTimeIntervalMicroSeconds32(500*1000)); // wait async functions

    CleanupStack::Pop(&server);

    return KErrGeneral;
}
TVerdict CServerTestStep::TdLocSrv_Server_Close_With_RequestsL()
{
    DO_PANIC_TEST_L(RPositionServer_Close_With_Requests, KPosClientFault, 
                    EPositionRequestsNotCancelled, KDefaultTestTimeout);
    return TestStepResult();
}

// the behavior being tested by following test
// is a task of server, holding requests. Irrelevant for test server

// 1.4.5 Double request cancel
TVerdict CServerTestStep::TdLocSrv_Server_Double_CancelRequestL()
{
    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    __UHEAP_MARK;

    // before any requests
    CheckExpectedResult(KErrNotFound, server.CancelRequest(EPositionServerNotifyModuleStatusEvent));

    TRequestStatus reqStatus;
    TPositionModuleStatusEvent statusEvent;
    statusEvent.SetRequestedEvents(DUMMY_STATUS_EVENT_REQ_EVENTS);

    // issue a request and cancel it right away
    server.NotifyModuleStatusEvent(statusEvent, reqStatus);
    CheckExpectedResult(KErrNone, server.CancelRequest(EPositionServerNotifyModuleStatusEvent));
    User::WaitForRequest(reqStatus);
    CheckExpectedResult(reqStatus.Int(), KErrCancel, KWrongErrorCode);

    // make request, cancel, then cancel again
    server.NotifyModuleStatusEvent(statusEvent, reqStatus);
    CheckExpectedResult(KErrNone, server.CancelRequest(EPositionServerNotifyModuleStatusEvent));
    User::WaitForRequest(reqStatus);

    // second one
    CheckExpectedResult(KErrNotFound, server.CancelRequest(EPositionServerNotifyModuleStatusEvent));

    __UHEAP_MARKEND;

    CleanupStack::PopAndDestroy(&server);

    return TestStepResult();
}

// 1.4.6 Double request
TInt RPositionServer_Double_Notify(TAny* /*aPtr*/)
{
    // Close with notifications and/or requests outstanding
    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    TPositionModuleStatusEvent statusEvent;
    TRequestStatus reqStatus;
    statusEvent.SetRequestedEvents(DUMMY_STATUS_EVENT_REQ_EVENTS);

    server.NotifyModuleStatusEvent(statusEvent, reqStatus);
    server.NotifyModuleStatusEvent(statusEvent, reqStatus); // should panic

    User::WaitForAnyRequest(); // wait async functions (actual point of panic)

    CleanupStack::Pop(&server);
    server.Close(); // this should panic ???

    return KErrGeneral;
}
TVerdict CServerTestStep::TdLocSrv_Server_Double_NotifyStatusL()
{
    DO_PANIC_TEST_L(RPositionServer_Double_Notify, KPosClientFault, 
                    EPositionDuplicateRequest, KDefaultTestTimeout);
    return TestStepResult();
}

//
// Version
//

// 1.5.1 Version retrieving
TVerdict CServerTestStep::TdLocSrv_Server_VersionL()
{
    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    TVersion version = server.Version();

    CheckExpectedResult( version.iMajor, KPosMajorVersionNumber);
    CheckExpectedResult( version.iMinor, KPosMinorVersionNumber);
    CheckExpectedResult( version.iBuild, KPosBuildVersionNumber);

    CleanupStack::PopAndDestroy(&server);

    return TestStepResult();
}
