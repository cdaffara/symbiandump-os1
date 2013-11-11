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
 @file PositionerTestStep.cpp
 @internalTechnology
*/
#include "positionerteststep.h"
#include "te_locsrvsuitedefs.h"

CPositionerTestStep::~CPositionerTestStep()
/**
 * Destructor
 */
	{
	}

CPositionerTestStep::CPositionerTestStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KPositionerTestStep);
	}

TVerdict CPositionerTestStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_locsrvSuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CPositionerTestStep::doTestStepL()
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

void CPositionerTestStep::TestByIndexL(TInt aIndex)
    {
    switch (aIndex)
        {
        case 0: 
            _LIT(KTestName0, "All Open Close");
            INFO_PRINTF1(KTestName0);
            TdLocSrv_Positioner_All_Open_CloseL();
            break;
        case 1: 
            _LIT(KTestName1, "Open No Connection");
            INFO_PRINTF1(KTestName1);
            TdLocSrv_Positioner_Open_No_ConnectionL();
            break;
        case 2: 
            _LIT(KTestName2, "Open Twice");
            INFO_PRINTF1(KTestName2);
            TdLocSrv_Positioner_Open_TwiceL();
            break;
        case 3: 
            _LIT(KTestName3, "Request Before Open");
            INFO_PRINTF1(KTestName3);
            TdLocSrv_Positioner_Request_before_OpenL();
            break;
        case 4: 
            _LIT(KTestName4, "Close No Open");
            INFO_PRINTF1(KTestName4);
            TdLocSrv_Positioner_Close_No_OpenL();
            break;
        case 5: 
            _LIT(KTestName5, "Open Specific Module");
            INFO_PRINTF1(KTestName5);
            TdLocSrv_Positioner_Open_Specific_ModuleL();
            break;
        case 6: 
            _LIT(KTestName6, "Open Bad Module");
            INFO_PRINTF1(KTestName6);
            TdLocSrv_Positioner_Open_Bad_ModuleL();
            break;
        case 7: 
            _LIT(KTestName7, "No Requestor");
            INFO_PRINTF1(KTestName7);
            TdLocSrv_Positioner_No_RequestorL();
            break;
        case 8: 
            _LIT(KTestName8, "Requestor");
            INFO_PRINTF1(KTestName8);
            TdLocSrv_Positioner_RequestorL();
            break;
        case 9: 
            break;
        case 10: 
            _LIT(KTestName10, "Set GetUpdateOptions");
            INFO_PRINTF1(KTestName10);
            TdLocSrv_Positioner_Set_GetUpdateOptionsL();
            break;
        case 11: 
            _LIT(KTestName11, "Close With Requests");
            INFO_PRINTF1(KTestName11);
            TdLocSrv_Positioner_Close_With_RequestsL();
            break;
        case 12: 
            _LIT(KTestName12, "Double NotifyPositionUpdate");
            INFO_PRINTF1(KTestName12);
            TdLocSrv_Positioner_Double_NotifyPositionUpdateL();
            break;
        case 13: 
            _LIT(KTestName13, "Double CancelRequest");
            INFO_PRINTF1(KTestName13);
            TdLocSrv_Positioner_Double_CancelRequestL();
            break;
        case 14: 
            break;
        case 15: 
            break;
        case 16: 
            _LIT(KTestName16, "NotifyPositionUpdate Standard");
            INFO_PRINTF1(KTestName16);
            TdLocSrv_Positioner_NotifyPositionUpdate_StandardL();
            break;
        case 17: 
            _LIT(KTestName17, "NotifyPositionUpdate Generic");
            INFO_PRINTF1(KTestName17);
            TdLocSrv_Positioner_NotifyPositionUpdate_GenericL();
            break;
        case 18: 
            _LIT(KTestName18, "NotifyPositionUpdate Extended Support");
            INFO_PRINTF1(KTestName18);
            TdLocSrv_Positioner_NotifyPositionUpdate_Extended_SupportL();
            break;
        case 19: 
            _LIT(KTestName19, "NotifyPositionUpdate Extended Unsupported");
            INFO_PRINTF1(KTestName19);
            TdLocSrv_Positioner_NotifyPositionUpdate_Extended_UnsupportedL();
            break;
        case 20: 
            _LIT(KTestName20, "Tracking Session");
            INFO_PRINTF1(KTestName20);
            TdLocSrv_Positioner_Tracking_SessionL();
            break;
        case 21: 
            _LIT(KTestName21, "Tracking Session Changing");
            INFO_PRINTF1(KTestName21);
            TdLocSrv_Positioner_Tracking_Session_ChangingL();
            break;
        case 22: 
            _LIT(KTestName22, "Cancel Tracking Session");
            INFO_PRINTF1(KTestName22);
            TdLocSrv_Positioner_Cancel_Tracking_SessionL();
            break;
        case 23: 
            _LIT(KTestName23, "Module DisabledEnabled");
            INFO_PRINTF1(KTestName23);
            TdLocSrv_Positioner_ModuleDisabledEnabledL();
            break;
        case 24: 
            _LIT(KTestName24, "Open No Modules");
            INFO_PRINTF1(KTestName24);
            TdLocSrv_Positioner_Open_No_ModulesL();
            break;
        default: 
            _LIT(KTestName25, "This test step doesn't exist");
            ERR_PRINTF1(KTestName25);
            SetTestStepResult(EFail);
            break;
            
        }
    }



TVerdict CPositionerTestStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}


extern TInt SetAuthenticRequestor(RPositioner& aPositioner);

HPositionGenericInfo* CreatePSY6DelayInstructionC(TTimeIntervalMicroSeconds aDelay)
{
    HPositionGenericInfo* genInfo = HPositionGenericInfo::NewLC();
    genInfo->SetRequestedField(KPSY6FieldDelayProcessing);
    genInfo->SetValue(KPSY6FieldDelayProcessing, aDelay);
    return genInfo;
}

extern TBool RequestTimedOut(TRequestStatus&, TUint);

//
// 
// RPositioner
//
//  Open/Close a connection
//
//

// 1.10.1 All Opens
TVerdict CPositionerTestStep::TdLocSrv_Positioner_All_Open_CloseL()
{
    StandardPrepareL();
    
    // Try different open methods (from server, from positioner, etc.)
    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    RPositioner positioner;
    CheckExpectedResult(0, positioner.SubSessionHandle()); // initial state
    CleanupClosePushL(positioner);
    
    // No params
    CheckExpectedResult(KErrNone, positioner.Open(server));
    CheckResult(0 < positioner.SubSessionHandle());
    positioner.Close();
    CheckExpectedResult(0, positioner.SubSessionHandle());

    // By ModuleID
    TPositionModuleId moduleId;
    moduleId.iUid = DUMMY_MODULE_ID1;
    CheckExpectedResult(KErrNone, positioner.Open(server, moduleId));
    CheckResult(0 < positioner.SubSessionHandle());
    positioner.Close();
    CheckExpectedResult(0, positioner.SubSessionHandle());

    // With Criteria
    TPositionCriteria requiredPosCriteria;

#if _CRITERIA_IMPLEMENTED_ || _TEST_SERVER_
    CheckExpectedResult(KErrNone, positioner.Open(server, requiredPosCriteria));
    CheckResult(0 < positioner.SubSessionHandle());
#else
    TInt res = positioner.Open(server, requiredPosCriteria);
    CheckExpectedResult(res, KErrNotFound, KWrongOpenByCriteriaResult);
    CheckResult(0 == positioner.SubSessionHandle());
#endif
    positioner.Close();
    CheckExpectedResult(0, positioner.SubSessionHandle());

    CleanupStack::PopAndDestroy(2, &server);
    return TestStepResult();
}

// 1.10.2 Open w/o connection
TInt RPositioner_Open_No_Connection_Default(TAny* /*aPtr*/)
{
    RPositionServer server;
    RPositioner positioner;

    positioner.Open(server);  // this should panic

    return KErrGeneral;
}
TInt RPositioner_Open_No_Connection_ModuleId(TAny* /*aPtr*/)
{
    RPositionServer server;
    RPositioner positioner;

    TPositionModuleId moduleId;
    moduleId.iUid = DUMMY_MODULE_ID1;
    positioner.Open(server, moduleId);  // this should panic

    return KErrGeneral;
}
TInt RPositioner_Open_No_Connection_Criteria(TAny* /*aPtr*/)
{
    RPositionServer server;
    RPositioner positioner;

    TPositionCriteria criteria;
    criteria.ResetCriteria(); // defaults
    positioner.Open(server, criteria);  // this should panic

    return KErrGeneral; // or this :-)
}

TVerdict CPositionerTestStep::TdLocSrv_Positioner_Open_No_ConnectionL()
{
    // default open
    DO_PANIC_TEST_L(
            RPositioner_Open_No_Connection_Default, 
            KPosClientFault,
            EPositionServerBadHandle, 
            KDefaultTestTimeout);
    // open by module id
    DO_PANIC_TEST_L(
            RPositioner_Open_No_Connection_ModuleId, 
            KPosClientFault,
            EPositionServerBadHandle, 
            KDefaultTestTimeout);
    // open by criteria
    DO_PANIC_TEST_L(
            RPositioner_Open_No_Connection_Criteria, 
            KPosClientFault,
            EPositionServerBadHandle, 
            KDefaultTestTimeout);
                
    return TestStepResult();
}

// 1.10.3 Double Open
TInt OpenRPositionerTwice(TAny* /*aPtr*/)
{
    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    RPositioner positioner;
    User::LeaveIfError(positioner.Open(server));
    CleanupClosePushL(positioner);

    positioner.Open(server);    //Should panic.

    CleanupStack::PopAndDestroy(2, &server);

    return KErrGeneral;
}
TVerdict CPositionerTestStep::TdLocSrv_Positioner_Open_TwiceL()
{
    DO_PANIC_TEST_L(OpenRPositionerTwice, KPosClientFault, 
                    EPositionServerHandleNotClosed, KDefaultTestTimeout);
    
    return TestStepResult();
}

// 1.10.4 Restricted calls before Open
TInt Close_Before_Open(TAny* /*aPtr*/)
{
    RPositioner positioner;
    positioner.Close();
    return KErrGeneral;
}
TInt CancelRequest_Before_Open(TAny* /*aPtr*/)
{
    RPositioner positioner;
    positioner.CancelRequest(0);
    return KErrGeneral;
}
TInt SetRequestor_Before_Open(TAny* /*aPtr*/)
{
    RPositioner positioner;
    SetAuthenticRequestor(positioner);
    return KErrGeneral;
}
TInt SetRequestors_Before_Open(TAny* /*aPtr*/)
{
    RPositioner positioner;
    RRequestorStack requestors;
    positioner.SetRequestor(requestors);
    return KErrGeneral;
}
TInt SetUpdateOptions_Before_Open(TAny* /*aPtr*/)
{
    RPositioner positioner;
    TPositionUpdateOptions options;
    positioner.SetUpdateOptions(options);
    return KErrGeneral;
}
TInt GetUpdateOptions_Before_Open(TAny* /*aPtr*/)
{
    RPositioner positioner;
    TPositionUpdateOptions options;
    positioner.GetUpdateOptions(options);
    return KErrGeneral;
}
TInt GetLastKnownPosition_Before_Open(TAny* /*aPtr*/)
{
    RPositioner positioner;
    TRequestStatus request;
    TPositionInfo posInfo;
    positioner.GetLastKnownPosition(posInfo, request);
    return KErrGeneral;
}
TInt NotifyPositionUpdate_Before_Open(TAny* /*aPtr*/)
{
    RPositioner positioner;
    TRequestStatus request;
    TPositionInfo posInfo;
    positioner.NotifyPositionUpdate(posInfo, request);
    return KErrGeneral;
}
TVerdict CPositionerTestStep::TdLocSrv_Positioner_Request_before_OpenL()
{
    DO_PANIC_TEST_L(CancelRequest_Before_Open, KPosClientFault, 
                    EPositionServerBadHandle, KDefaultTestTimeout);
    DO_PANIC_TEST_L(SetRequestor_Before_Open, KPosClientFault, 
                    EPositionServerBadHandle, KDefaultTestTimeout);
    DO_PANIC_TEST_L(SetRequestor_Before_Open, KPosClientFault, 
                    EPositionServerBadHandle, KDefaultTestTimeout);
    DO_PANIC_TEST_L(SetUpdateOptions_Before_Open, KPosClientFault, 
                    EPositionServerBadHandle, KDefaultTestTimeout);
    DO_PANIC_TEST_L(GetUpdateOptions_Before_Open, KPosClientFault, 
                    EPositionServerBadHandle, KDefaultTestTimeout);
    DO_PANIC_TEST_L(GetLastKnownPosition_Before_Open, KPosClientFault, 
                    EPositionServerBadHandle, KDefaultTestTimeout);
    DO_PANIC_TEST_L(NotifyPositionUpdate_Before_Open, KPosClientFault, 
                    EPositionServerBadHandle, KDefaultTestTimeout);
    return TestStepResult();
}

// 1.10.5 Close w/o Open
TVerdict CPositionerTestStep::TdLocSrv_Positioner_Close_No_OpenL()
{
    RPositioner positioner;
    positioner.Close();

    return TestStepResult();
}

// 1.10.6 Open when no modules exist
TVerdict CPositionerTestStep::TdLocSrv_Positioner_Open_No_ModulesL()
{
    StandardPrepareL();

    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    // 1. save list of enabled psys
    CPosModules* modules = CPosModules::OpenL();
    CleanupStack::PushL(modules);
    RArray<TPositionModuleId> enabledPsys;
    CPosModuleIdList* idList = modules->ModuleIdListLC();
    for (TInt i = 0; i < idList->Count(); i++)
        {
        TPositionModuleInfo info;
        modules->GetModuleInfoL((*idList)[i], info);
        if (info.IsAvailable())
            {
            enabledPsys.AppendL(info.ModuleId());
            }
        }
    
    // 2. Disable all PSYs and try to open connection
    TRequestStatus status;
    TPositionInfo posInfo;
    
    DisableAllModulesL();

    RPositioner positioner;
    CleanupClosePushL(positioner);

    // specific PSYs
    for (TInt i = 0; i < idList->Count(); i++)
        {
        CheckExpectedResult(KErrNotFound, positioner.Open(server, (*idList)[i]));
        }
        
    // default proxy
    CheckExpectedResult(KErrNone, positioner.Open(server));
    
    SetAuthenticRequestor(positioner);
    
    // return value from Default Proxy
    positioner.NotifyPositionUpdate(posInfo, status);
    User::WaitForRequest(status);
    CheckExpectedResult(status.Int(), KErrNotFound, KWrongErrorCode);

    // return value from Server
    positioner.GetLastKnownPosition(posInfo, status);
    User::WaitForRequest(status);
    if (status.Int() != KErrNone && status.Int() != KErrUnknown)
        {
        ERR_PRINTF1(KWrongErrorCode);
        SetTestStepResult(EFail);
        }
    
    // 3. Restore PSYs
    CPosModuleUpdate* update = CPosModuleUpdate::NewLC();
    update->SetUpdateAvailability(ETrue);
    for (TInt i = 0; i < enabledPsys.Count(); i++)
        {
        modules->UpdateModuleL(enabledPsys[i], *update);
        }

    enabledPsys.Close();
    CleanupStack::PopAndDestroy(5, &server);
    return TestStepResult();
}

// 1.10.7 Open with No Memory
// See NoMemoryTests.cpp

//
// 
// RPositioner
//
//  Module Selection
//
//

// 1.12.1 Open Specific module
TVerdict CPositionerTestStep::TdLocSrv_Positioner_Open_Specific_ModuleL()
{
    StandardPrepareL();

    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    RPositioner positioner;
    CheckExpectedResult(0, positioner.SubSessionHandle()); // initial state

    // try all available modules, open connection to them
    TUint modules;
    CheckExpectedResult(KErrNone, server.GetNumModules(modules));
    for (unsigned int i = 0; i < modules; i++)
    {
        TPositionModuleInfo modInfo;

        TInt res = server.GetModuleInfoByIndex(TUint(i), modInfo);
        CheckExpectedResult(res, KErrNone, KFailedOpenListedModule);
        TPositionModuleId moduleId;
        moduleId = modInfo.ModuleId();
        CheckResult(DUMMY_MODULE_BAD_ID != moduleId.iUid);

        if (moduleId.iUid == DEF_SIMULATION_PSY_ID)
            {
            INFO_PRINTF2(KSimulationPSYIgnoredRequiresInputFile, moduleId.iUid);
            }

        TBuf<KPositionMaxModuleName> modName;
        modInfo.GetModuleName(modName);

        INFO_PRINTF4(KOpeningModule, i, &modName, moduleId.iUid);

        TInt err = positioner.Open(server, moduleId);
        if (!modInfo.IsAvailable())
            {
            CheckExpectedResult(KErrNotFound, err);
            }
        else
            {
            CheckExpectedResult(KErrNone, err);
            }

        if (err == KErrNone)
        {
            CleanupClosePushL(positioner);

            // get position and check that module selected is same 
            // as requested at connection
            TPositionInfo posInfo;
            TRequestStatus request;
            // set requestor
            CheckExpectedResult(KErrNone, SetAuthenticRequestor(positioner));
            // get fix
            positioner.NotifyPositionUpdate(posInfo, request);
            User::WaitForRequest(request);
            CheckExpectedResult(KErrNone, request.Int());

            if (KErrNone == request.Int())
            {
                CheckExpectedResult(moduleId.iUid, posInfo.ModuleId().iUid);
                CheckResult(DUMMY_MODULE_BAD_ID != posInfo.ModuleId().iUid);
            }

            CleanupStack::Pop(&positioner); 
            positioner.Close();
        }
        else if (err == KErrNotFound)
        {
            INFO_PRINTF2(KCantOpenDisabledModule, &modName);
        }
        else
        {
            ERR_PRINTF3(KFailedOpenModuleError, &modName, err);
            SetTestStepResult(EFail);
        }
        CheckExpectedResult(0, positioner.SubSessionHandle());
    }

    CleanupStack::PopAndDestroy(&server);
    return TestStepResult();
}

// 1.12.2 Open with bad Module Id argument
TVerdict CPositionerTestStep::TdLocSrv_Positioner_Open_Bad_ModuleL()
{
    StandardPrepareL();
    
    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    RPositioner positioner;
    CheckExpectedResult(positioner.SubSessionHandle(), 0, KBadInitialState);

    TPositionModuleId moduleId;
    moduleId.iUid = DUMMY_MODULE_BAD_ID;
    
    TInt res = positioner.Open(server, moduleId);
    CheckExpectedResult(res, KErrNotFound, KOpenedBadModule);
    CleanupClosePushL(positioner);
    CheckExpectedResult(positioner.SubSessionHandle(), 0, KWrongHandle);

    moduleId = KNullUid;

    res = positioner.Open(server, moduleId);
    CheckExpectedResult(res, KErrNotFound, KOpenedWithNullModule);
    CheckExpectedResult(positioner.SubSessionHandle(), 0, KWrongHandle);

    CleanupStack::PopAndDestroy(2, &server);
    return TestStepResult();
}

//
// 
// RPositioner
//
//  Specifying Requestors
//
//

#ifdef _TEST_SERVER_
// these tests could work with Real Server only if individual privacy is set
// this is not always the case, so only test server versions are here

// 1.13.1 Check setting Requestor
TVerdict CPositionerTestStep::TdLocSrv_Positioner_SetRequestorL()
{
    DECLARE_ERROR_LOGGING; 
    StandardPrepareL();
    
    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    RPositioner positioner;
    LEAVE(KErrNone != positioner.Open(server), KFailedOpenPositioner)
    CleanupClosePushL(positioner);

    TPositionInfo posInfo;
    TRequestStatus request;

    // Accepted Requestor
    CheckExpectedResult(KErrNone, positioner.SetRequestor(
            DUMMY_REQUESTOR_TYPE_ACCEPTED1, 
            DUMMY_REQUESTOR_FORMAT_ACCEPTED1, 
            DUMMY_REQUESTOR_NAME_ACCEPTED1)); // service
    // get fix
    positioner.NotifyPositionUpdate(posInfo, request);
    User::WaitForRequest(request);
    CheckExpectedResult(request.Int(), KErrNone, "Request failed");

    // Rejected Requestor
    CheckExpectedResult(KErrNone, positioner.SetRequestor(
            DUMMY_REQUESTOR_TYPE_REJECTED1, 
            DUMMY_REQUESTOR_FORMAT_REJECTED1, 
            DUMMY_REQUESTOR_NAME_REJECTED1));
    // get fix
    positioner.NotifyPositionUpdate(posInfo, request);
    User::WaitForRequest(request);
    CheckExpectedResult(request.Int(), KErrAccessDenied, "Request failed");

    CleanupStack::PopAndDestroy(2, &server);
    return TestStepResult();
}

// 1.13.2 Check setting Stack of Requestors
TVerdict CPositionerTestStep::TdLocSrv_Positioner_SetRequestorStackL()
{
    DECLARE_ERROR_LOGGING; 
    StandardPrepareL();
    
    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    RPositioner positioner;
    LEAVE(KErrNone != positioner.Open(server), KFailedOpenPositioner)
    CleanupClosePushL(positioner);

    TPositionInfo posInfo;
    TRequestStatus request;

    RRequestorStack stack;
    CRequestor* requestor = NULL;

    // All Accepted
    requestor = CRequestor::NewLC(
            DUMMY_REQUESTOR_TYPE_ACCEPTED2,
            DUMMY_REQUESTOR_FORMAT_ACCEPTED2, 
            DUMMY_REQUESTOR_NAME_ACCEPTED2);
    stack.Append(requestor);
    requestor = CRequestor::NewLC(
            DUMMY_REQUESTOR_TYPE_ACCEPTED1,
            DUMMY_REQUESTOR_FORMAT_ACCEPTED1, 
            DUMMY_REQUESTOR_NAME_ACCEPTED1); // service
    stack.Append(requestor);
    // set requestor stack
    CheckExpectedResult(KErrNone, positioner.SetRequestor(stack));
    // get fix
    positioner.NotifyPositionUpdate(posInfo, request);
    User::WaitForRequest(request);
    CheckExpectedResult(KErrNone, request.Int());

    stack.Reset();                  // clean stack
    CleanupStack::PopAndDestroy(2); // requestors

    // Accepted + Rejected
    requestor = CRequestor::NewLC(
            DUMMY_REQUESTOR_TYPE_ACCEPTED2,
            DUMMY_REQUESTOR_FORMAT_ACCEPTED2, 
            DUMMY_REQUESTOR_NAME_ACCEPTED2);
    stack.Append(requestor);
    requestor = CRequestor::NewLC(
            DUMMY_REQUESTOR_TYPE_REJECTED1,
            DUMMY_REQUESTOR_FORMAT_REJECTED1, 
            DUMMY_REQUESTOR_NAME_REJECTED1);
    stack.Append(requestor);
    // set requestor stack
    positioner.SetRequestor(stack);
    // get fix
    positioner.NotifyPositionUpdate(posInfo, request);
    User::WaitForRequest(request);
    CheckExpectedResult(KErrAccessDenied, request.Int());

    stack.Reset();                  // clean stack
    CleanupStack::PopAndDestroy(2); // requestors

    // All Accepted
    requestor = CRequestor::NewLC(
            DUMMY_REQUESTOR_TYPE_REJECTED1,
            DUMMY_REQUESTOR_FORMAT_REJECTED1, 
            DUMMY_REQUESTOR_NAME_REJECTED1);
    stack.Append(requestor);
    requestor = CRequestor::NewLC(
            DUMMY_REQUESTOR_TYPE_REJECTED2,
            DUMMY_REQUESTOR_FORMAT_REJECTED2, 
            DUMMY_REQUESTOR_NAME_REJECTED2);
    stack.Append(requestor);
    // set requestor stack
    CheckExpectedResult(KErrNone, positioner.SetRequestor(stack));
    // get fix
    positioner.NotifyPositionUpdate(posInfo, request);
    User::WaitForRequest(request);
    CheckExpectedResult(KErrAccessDenied, request.Int());

    stack.Reset();                  // clean stack
    CleanupStack::PopAndDestroy(2); // requestors

    CleanupStack::PopAndDestroy(2, &server);
    return TestStepResult();
}

#else
#pragma message ("TC1.13.1 Skipped. Irrelevant without individual privacy")
#pragma message ("TC1.13.2 Skipped. Irrelevant without individual privacy")
#endif

// 1.13.3 Get Position Info w/o setting Requestor
TVerdict CPositionerTestStep::TdLocSrv_Positioner_No_RequestorL()
{
    StandardPrepareL();
    
    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    RPositioner positioner;
    if(KErrNone != positioner.Open(server))
        {
        ERR_PRINTF1(KFailedOpenPositioner);
        SetTestStepResult(EFail);
        return TestStepResult();
        }
    CleanupClosePushL(positioner);

    TRequestStatus request;
    TPositionInfo posInfo;

    // NO requestors are specified, this is now accpetable in the merged API.
    // Therefore test that KErrNone is returned

    // last fix getting
    positioner.GetLastKnownPosition(posInfo, request);
    User::WaitForRequest(request);
    CheckExpectedResult(request.Int(), KErrNone, KWrongErrorCode);

    // new fix retrieving
    positioner.NotifyPositionUpdate(posInfo, request);
    User::WaitForRequest(request);
    CheckExpectedResult(request.Int(), KErrNone, KWrongErrorCode);

    CleanupStack::PopAndDestroy(&positioner);
    CleanupStack::PopAndDestroy(&server);
    return TestStepResult();
}

// 1.13.4 Using SetRequestor
#define CHECK_UPDATE_REQUEST(aErrCode)                  \
    positioner.NotifyPositionUpdate(posInfo, request);  \
    User::WaitForRequest(request);                      \
    CheckExpectedResult(aErrCode, request.Int());

TVerdict CPositionerTestStep::TdLocSrv_Positioner_RequestorL()
{
    StandardPrepareL();
    
    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    RPositioner positioner;
    if(KErrNone != positioner.Open(server))
        {
        ERR_PRINTF1(KFailedOpenPositioner);
        SetTestStepResult(EFail);
        return TestStepResult();
        }
    CleanupClosePushL(positioner);

    RRequestorStack stack;

    // Stack Requestor
    CheckResult(KErrNone == positioner.SetRequestor(stack));
    stack.Close();

	// Single requestor
    TPositionInfo posInfo;
    TRequestStatus request;

    // application
    CheckExpectedResult(KErrNone, positioner.SetRequestor(CRequestor::ERequestorService, CRequestor::EFormatApplication, _L("TestRequestorApp")));

    // service/unknown
    CheckExpectedResult(KErrNone, positioner.SetRequestor(CRequestor::ERequestorService, CRequestor::EFormatUnknown, _L("1234567")));

    // only contact/phone
    CheckExpectedResult(KErrNone, positioner.SetRequestor(CRequestor::ERequestorContact, CRequestor::EFormatTelephone, _L("1234567")));

    CleanupStack::PopAndDestroy(2, &server);    // positioner, server
    return TestStepResult();
}

//
// 
// RPositioner
//
//  Obtaining Position
//
//

// 1.15.1 Set/Get UpdateOptions
TVerdict CPositionerTestStep::TdLocSrv_Positioner_Set_GetUpdateOptionsL()
{
    StandardPrepareL();
    
    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    RPositioner positioner;
    if(KErrNone != positioner.Open(server))
        {
        ERR_PRINTF1(KFailedOpenPositioner);
        SetTestStepResult(EFail);
        return TestStepResult();
        }
    CleanupClosePushL(positioner);

    TPositionUpdateOptions options, options2;

    // set non-default values
    options.SetAcceptPartialUpdates(ETrue);
    options.SetMaxUpdateAge(TTimeIntervalMicroSeconds(1234));
    options.SetUpdateInterval(TTimeIntervalMicroSeconds(4444));
    options.SetUpdateTimeOut(TTimeIntervalMicroSeconds(8888)); // must be greater than interval

    // set requestor
    CheckExpectedResult(KErrNone, SetAuthenticRequestor(positioner));
    // set options
    CheckExpectedResult(KErrNone, positioner.SetUpdateOptions(options));
    // retrieve now 
    CheckExpectedResult(KErrNone, positioner.GetUpdateOptions(options2));

    // compare
    CHECK_EQUAL_PROPS(options, options2, UpdateInterval().Int64());
    CHECK_EQUAL_PROPS(options, options2, UpdateTimeOut().Int64());
    CHECK_EQUAL_PROPS(options, options2, MaxUpdateAge().Int64());
    CHECK_EQUAL_PROPS(options, options2, AcceptPartialUpdates());

    CleanupStack::PopAndDestroy(2, &server); // positioner, server
    return TestStepResult();
}

// 1.15.2 Close before request completed/cancelled
TInt RPositioner_Close_With_Requests(TAny* /*aPtr*/)
{
    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    RPositioner positioner;
    TPositionModuleId moduleId = {KLcfPsy6UidValue};
    User::LeaveIfError(positioner.Open(server, moduleId));
    CleanupClosePushL(positioner);

    User::LeaveIfError(SetAuthenticRequestor(positioner));

    // instruct PSY6 to delay request processing
    const TTimeIntervalMicroSeconds KPsyDelay = 2 * KSecondsToMicro;
    const TTimeIntervalMicroSeconds32 KPsyWait = 1 * KSecondsToMicro;
    HPositionGenericInfo* genInfo = CreatePSY6DelayInstructionC(KPsyDelay);

    TRequestStatus reqStatus;
    positioner.NotifyPositionUpdate(*genInfo, reqStatus);
    User::After(KPsyWait);

    positioner.Close(); // this should panic
    
    User::WaitForRequest(reqStatus);
    CleanupStack::PopAndDestroy(genInfo);
    CleanupStack::PopAndDestroy(2, &server);
    return KErrGeneral;
}
TVerdict CPositionerTestStep::TdLocSrv_Positioner_Close_With_RequestsL()
{
    StandardPrepareL();
    
    DO_PANIC_TEST_L(RPositioner_Close_With_Requests, KPosClientFault, 
                    EPositionRequestsNotCancelled, KDefaultTestTimeout);
                    
    return TestStepResult();
}

// 1.15.3 Double position request 
TInt Double_NotifyPositionUpdate(TAny* /*aPtr*/)
{
    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    RPositioner positioner;
    TPositionModuleId moduleId = {KLcfPsy6UidValue};
    User::LeaveIfError(positioner.Open(server, moduleId));
    CleanupClosePushL(positioner);
    SetAuthenticRequestor(positioner);

    // instruct PSY6 to delay request processing
    const TTimeIntervalMicroSeconds KPsyDelay = 2 * KSecondsToMicro;
    const TTimeIntervalMicroSeconds32 KPsyWait = 1 * KSecondsToMicro;
    HPositionGenericInfo* genInfo = CreatePSY6DelayInstructionC(KPsyDelay);

    TPositionInfo notifyPosInfo;

    TRequestStatus reqStatus1, reqStatus2;
    positioner.NotifyPositionUpdate(*genInfo, reqStatus1);
    User::After(KPsyWait);

    positioner.NotifyPositionUpdate(notifyPosInfo, reqStatus2); // has to panic

    User::After(KHalfSecond);

    CleanupStack::PopAndDestroy(genInfo);
    CleanupStack::Pop(2);
    positioner.Close();
    server.Close();

    return KErrGeneral;
}

TVerdict CPositionerTestStep::TdLocSrv_Positioner_Double_NotifyPositionUpdateL()
{
    StandardPrepareL();
    
    DO_PANIC_TEST_L(Double_NotifyPositionUpdate, KPosClientFault, 
                    EPositionDuplicateRequest, KDefaultTestTimeout);
    
    return TestStepResult();
}

// the behavior being tested by following test
// is a task of server, holding requests. Irrelevant for test server

// 1.15.4 Double request cancel
TVerdict CPositionerTestStep::TdLocSrv_Positioner_Double_CancelRequestL()
{
    StandardPrepareL();
    
    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    RPositioner positioner;
    TPositionModuleId moduleId = {KLcfPsy6UidValue};
    if(KErrNone != positioner.Open(server, moduleId))
        {
        ERR_PRINTF1(KFailedOpenPositionerByModuleID);
        SetTestStepResult(EFail);
        return TestStepResult();
        }
    CleanupClosePushL(positioner);
    CheckExpectedResult(KErrNone, SetAuthenticRequestor(positioner));

    __UHEAP_MARK;

    // before any requests
    CheckExpectedResult(KErrNotFound, positioner.CancelRequest(EPositionerNotifyPositionUpdate));

    TPositionInfo posInfo;
    TRequestStatus reqStatus;

    // instruct PSY6 to delay request processing
    const TTimeIntervalMicroSeconds KPsyDelay = 2 * KSecondsToMicro;
    const TTimeIntervalMicroSeconds32 KPsyWait = 1 * KSecondsToMicro;
    HPositionGenericInfo* genInfo = CreatePSY6DelayInstructionC(KPsyDelay);

    // issue a request and cancel it right away
    positioner.NotifyPositionUpdate(*genInfo, reqStatus);
    User::After(KPsyWait);
    TInt res = positioner.CancelRequest(EPositionerNotifyPositionUpdate);
    CheckExpectedResult(res, KErrNone, KBadResult);
    User::WaitForRequest(reqStatus);
    CheckExpectedResult(reqStatus.Int(), KErrCancel, KRequestNotCancelled);

    // make request, cancel, then cancel again
    positioner.NotifyPositionUpdate(*genInfo, reqStatus);
    User::After(KPsyWait);
    res = positioner.CancelRequest(EPositionerNotifyPositionUpdate);
    CheckExpectedResult(res, KErrNone, KBadResult);
    User::WaitForRequest(reqStatus);

    // second cancel
    res = positioner.CancelRequest(EPositionerNotifyPositionUpdate);
    CheckExpectedResult(res, KErrNotFound, KBadResult);

    CleanupStack::PopAndDestroy(genInfo);

    __UHEAP_MARKEND;

    CleanupStack::PopAndDestroy(2, &server);
    return TestStepResult();
}

// 1.15.5 GetLastKnownPosition
// 1.15.6 GetLastKnownPosition with Extended Classes
// - Old tests removed

// 1.15.7 NotifyPositionUpdate with TPositionInfo
TVerdict CPositionerTestStep::TdLocSrv_Positioner_NotifyPositionUpdate_StandardL()
{
    StandardPrepareL();
    
    // Check the correct message is always sent to the server upon a
    // call to this function and that the return value is always that
    // of the completed message.
    // Test TPositionInfo, a derivative of TPositionInfoBase
    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    RPositioner positioner;

    TPositionModuleId moduleId;
    moduleId.iUid = DUMMY_MODULE_ID1;
    if(KErrNone != positioner.Open(server, moduleId))
        {
        ERR_PRINTF1(KFailedOpenPositionerByModuleID);
        SetTestStepResult(EFail);
        return TestStepResult();
        }
    CleanupClosePushL(positioner);
    
    CheckExpectedResult(KErrNone, SetAuthenticRequestor(positioner));

    TRequestStatus request;

    // TPositionInfo
    TPositionInfo posInfo;
    positioner.NotifyPositionUpdate(posInfo, request);
    User::WaitForRequest(request);
    CheckExpectedResult( KErrNone, request.Int());

    TPosition position;
    posInfo.GetPosition(position);

    // Server will set the time of the TPosition to the current time.
    // Just check that we are within a seconds of that time right now.
    TTime now;
    now.UniversalTime();

    CheckResult(position.Time() - TTimeIntervalSeconds(2) < now);
    CheckResult(position.Time() + TTimeIntervalSeconds(2) > now);

    // get PSY1 predefined dynamic position values
/*
TODO:
    TPosition psyDynamicPosition;
//    GET_TC_ARRAY_PARAMETER(PSYDynamicPosition, _L8("PSY Dynamic Position"), psyDynamicPosition, 0); 
    // compare
    CHECK_REAL_EQUAL( psyDynamicPosition.Latitude(), position.Latitude());
    CHECK_REAL_EQUAL( psyDynamicPosition.Longitude(), position.Longitude());
    CHECK_REAL_EQUAL( psyDynamicPosition.Altitude(), position.Altitude());
    CHECK_REAL_EQUAL( psyDynamicPosition.HorizontalAccuracy(), position.HorizontalAccuracy());
    CHECK_REAL_EQUAL( psyDynamicPosition.VerticalAccuracy(), position.VerticalAccuracy());
*/
    CleanupStack::PopAndDestroy(2, &server);
    return TestStepResult();
}


// 1.15.8 NotifyPositionUpdate with HPositionGenericInfo
TVerdict CPositionerTestStep::TdLocSrv_Positioner_NotifyPositionUpdate_GenericL()
    { 
    StandardPrepareL();
    
    // Check with multiple TPositionInfo-derived classes, including ones
    // created specifically to test the client API.
    // Test HPositionGenericInfo, a derivative of TPositionInfoBase
    // Note that this only tests the client / server transfer aspect of HPositionGenericInfo.
    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    RPositioner positioner;

    TPositionModuleId moduleId;
    moduleId.iUid = DUMMY_MODULE_ID1;
    if(KErrNone != positioner.Open(server, moduleId))
        {
        ERR_PRINTF1(KFailedOpenPositionerByModuleID); // lcfpsy1
        SetTestStepResult(EFail);
        return TestStepResult();
        }
    CleanupClosePushL(positioner);

    CheckExpectedResult(KErrNone, SetAuthenticRequestor(positioner));

    HPositionGenericInfo* notifyPosInfo = HPositionGenericInfo::NewLC(512);
    TRequestStatus reqStatus;

#ifdef _TEST_SERVER_

    notifyPosInfo->SetRequestedField(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID1);
    notifyPosInfo->SetRequestedField(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID2);
    notifyPosInfo->SetRequestedField(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID3);
    notifyPosInfo->SetRequestedField(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID4);
    notifyPosInfo->SetRequestedField(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID5);
    notifyPosInfo->SetRequestedField(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID6);
    notifyPosInfo->SetRequestedField(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID7);
    notifyPosInfo->SetRequestedField(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID8);
    notifyPosInfo->SetRequestedField(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID9);
    notifyPosInfo->SetRequestedField(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID10);
    notifyPosInfo->SetRequestedField(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID11);
    notifyPosInfo->SetRequestedField(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID12);
    notifyPosInfo->SetRequestedField(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID13);
    // don't request 14 or 15

    positioner.NotifyPositionUpdate(*notifyPosInfo, reqStatus);
    User::WaitForRequest(reqStatus);
    CheckExpectedResult( KErrNone, reqStatus.Int());

    // Test extended data held in generic buffer
    TInt8 theInt8;
    TInt16 theInt16;
    TInt32 theInt32;
    TInt64 theInt64;
    TUint8 theUint8;
    TUint16 theUint16;
    TUint32 theUint32;
    TReal32 theReal32;
    TReal64 theReal64;
    TBuf8<50> theDes8;
    TBuf16<50> theDes16;
    TTime theTime;
    TTimeIntervalMicroSeconds theTimeIntervalMicroSeconds;

    // Check every data type has been copied over from the server.
    CheckExpectedResult( KErrNone, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID1, theInt8));
    CheckExpectedResult( DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA1, theInt8);
    CheckExpectedResult( KErrNone, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID2, theInt16));
    CheckExpectedResult( DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA2, theInt16);
    CheckExpectedResult( KErrNone, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID3, theInt32));
    CheckExpectedResult( DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA3, theInt32);
    CheckExpectedResult( KErrNone, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID4, theInt64));
    CheckExpectedResult( DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA4, theInt64);

    CheckExpectedResult( KErrNone, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID5, theUint8));
    CheckExpectedResult( DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA5, theUint8);
    CheckExpectedResult( KErrNone, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID6, theUint16));
    CheckExpectedResult( DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA6, theUint16);
    CheckExpectedResult( KErrNone, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID7, theUint32));
    CheckExpectedResult( DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA7, theUint32);

    CheckExpectedResult( KErrNone, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID8, theReal32));
    CheckExpectedResult( DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA8, theReal32);
    CheckExpectedResult( KErrNone, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID9, theReal64));
    CheckExpectedResult( DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA9, theReal64);

    CheckExpectedResult( KErrNone, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID10, theDes8));
    CheckExpectedResult( DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA10, theDes8);
    CheckExpectedResult( KErrNone, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID11, theDes16));
    CheckExpectedResult( DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA11, theDes16);

    CheckExpectedResult( KErrNone, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID12, theTime));
    CheckExpectedResult( DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA12, theTime);
    CheckExpectedResult( KErrNone, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID13, theTimeIntervalMicroSeconds));
    CheckExpectedResult( DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA13, theTimeIntervalMicroSeconds);

    // Check un-requested fields are not set
    CheckExpectedResult( KErrNotFound, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID14, theDes16));
    CheckExpectedResult( KErrNotFound, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID15, theDes16));

    // Now request only those fields.
    notifyPosInfo->ClearRequestedFields();
    notifyPosInfo->SetRequestedField(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID14);
    notifyPosInfo->SetRequestedField(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID15);

    positioner.NotifyPositionUpdate(*notifyPosInfo, reqStatus);
    User::WaitForRequest(reqStatus);
    CheckExpectedResult( KErrNone, reqStatus.Int());

    CheckExpectedResult( KErrNone, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID14, theDes16));
    CheckExpectedResult( DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA14, theDes16);
    CheckExpectedResult( KErrNone, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID15, theDes16));
    CheckExpectedResult( DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA15, theDes16);

    // Check that the un-requested fields are not set
    // We can use any type here, since the fields are empty. Using a descriptor for each
    // function call would cause a panic if the fields did contain their associated data type.
    CheckExpectedResult( KErrNotFound, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID1, theDes16));
    CheckExpectedResult( KErrNotFound, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID2, theDes16));
    CheckExpectedResult( KErrNotFound, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID3, theDes16));
    CheckExpectedResult( KErrNotFound, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID4, theDes16));
    CheckExpectedResult( KErrNotFound, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID5, theDes16));
    CheckExpectedResult( KErrNotFound, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID6, theDes16));
    CheckExpectedResult( KErrNotFound, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID7, theDes16));
    CheckExpectedResult( KErrNotFound, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID8, theDes16));
    CheckExpectedResult( KErrNotFound, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID9, theDes16));
    CheckExpectedResult( KErrNotFound, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID10, theDes16));
    CheckExpectedResult( KErrNotFound, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID11, theDes16));
    CheckExpectedResult( KErrNotFound, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID12, theDes16));
    CheckExpectedResult( KErrNotFound, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_ID13, theDes16));

#else
    // LCFPSY1 is used here
    notifyPosInfo->SetRequestedField(DUMMY_NOTIFY_POS_UPDATE_FIELD_STREET_ID);
    notifyPosInfo->SetRequestedField(DUMMY_NOTIFY_POS_UPDATE_FIELD_CITY_ID);
    // don't request country

    positioner.NotifyPositionUpdate(*notifyPosInfo, reqStatus);
    User::WaitForRequest(reqStatus);
    CheckExpectedResult( KErrNone, reqStatus.Int());

    // test street and city
    TBuf<50> theDes16;
    CheckExpectedResult( KErrNone, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_STREET_ID, theDes16));
    CheckExpectedResult( DUMMY_NOTIFY_POS_UPDATE_FIELD_STREET_DATA, theDes16);
    CheckExpectedResult( KErrNone, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_CITY_ID, theDes16));
    CheckExpectedResult( DUMMY_NOTIFY_POS_UPDATE_FIELD_CITY_DATA, theDes16);
    // there is no country
    CheckExpectedResult( KErrNotFound, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_COUNTRY_ID, theDes16));

    // now request only country
    notifyPosInfo->ClearRequestedFields();
    notifyPosInfo->ClearPositionData();
    notifyPosInfo->SetRequestedField(DUMMY_NOTIFY_POS_UPDATE_FIELD_COUNTRY_ID);

    positioner.NotifyPositionUpdate(*notifyPosInfo, reqStatus);
    User::WaitForRequest(reqStatus);
    CheckExpectedResult( KErrNone, reqStatus.Int());

    // no street or city
    CheckExpectedResult( KErrNotFound, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_STREET_ID, theDes16));
    CheckExpectedResult( KErrNotFound, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_CITY_ID, theDes16));
    // but country
    CheckExpectedResult( KErrNone, notifyPosInfo->GetValue(DUMMY_NOTIFY_POS_UPDATE_FIELD_COUNTRY_ID, theDes16));
    CheckExpectedResult( DUMMY_NOTIFY_POS_UPDATE_FIELD_COUNTRY_DATA, theDes16);

    // Test base position
    TPosition position;
    notifyPosInfo->GetPosition(position);
    // get PSY1 predefined dynamic position values
/*
TODO:
    TPosition psyDynamicPosition;
//    GET_TC_ARRAY_PARAMETER(PSYDynamicPosition, _L8("PSY Dynamic Position"), psyDynamicPosition, 0); 
    // compare
    CHECK_REAL_EQUAL( psyDynamicPosition.Latitude(), position.Latitude());
    CHECK_REAL_EQUAL( psyDynamicPosition.Longitude(), position.Longitude());
    CHECK_REAL_EQUAL( psyDynamicPosition.Altitude(), position.Altitude());
    CHECK_REAL_EQUAL( psyDynamicPosition.HorizontalAccuracy(), position.HorizontalAccuracy());
    CHECK_REAL_EQUAL( psyDynamicPosition.VerticalAccuracy(), position.VerticalAccuracy());
*/  
#endif

    CleanupStack::PopAndDestroy(3, &server);
    return TestStepResult();
}

//
// 
// RPositioner
//
//  Extended Position Info
//
//

// 1.17.1 NotifyPositionUpdate with Extended classes, when supported
TVerdict CPositionerTestStep::TdLocSrv_Positioner_NotifyPositionUpdate_Extended_SupportL()
{
    StandardPrepareL();
    
    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    RPositioner positioner;

    TPositionModuleId moduleId;
    moduleId.iUid = DUMMY_MODULE_ID3;   // LCFPSY3 supports extended info

    if(KErrNone != positioner.Open(server, moduleId))
        {
        ERR_PRINTF1(KFailedOpenPositionerByModuleID);
        SetTestStepResult(EFail);
        return TestStepResult();
        }
    CleanupClosePushL(positioner);

    User::LeaveIfError(SetAuthenticRequestor(positioner));

    TRequestStatus request;

    // Course Info

    TPositionCourseInfo courseInfo;
    positioner.NotifyPositionUpdate(courseInfo, request);
    User::WaitForRequest(request);
    CheckExpectedResult( KErrNone, request.Int());

    // basic portion
    TPosition pos;
    courseInfo.GetPosition(pos);

    CheckExpectedRealResult( DUMMY_NOTIFY_POS_UPDATE_LATITUDE3, pos.Latitude());
    CheckExpectedRealResult( DUMMY_NOTIFY_POS_UPDATE_LONGITUDE3, pos.Longitude());
    CheckExpectedRealResult( DUMMY_NOTIFY_POS_UPDATE_ALTITUDE3, pos.Altitude());
    CheckExpectedRealResult( DUMMY_NOTIFY_POS_UPDATE_HORIZONTAL_ACCURACY3, pos.HorizontalAccuracy());
    CheckExpectedRealResult( DUMMY_NOTIFY_POS_UPDATE_VERTICAL_ACCURACY3, pos.VerticalAccuracy());

    // course portion
    TCourse course;
    courseInfo.GetCourse(course);

    CheckExpectedRealResult( DUMMY_NOTIFY_POS_UPDATE_SPEED, course.Speed());
    CheckExpectedRealResult( DUMMY_NOTIFY_POS_UPDATE_HEADING, course.Heading());
    CheckExpectedRealResult( DUMMY_NOTIFY_POS_UPDATE_SPEED_ACCURACY, course.SpeedAccuracy());
    CheckExpectedRealResult( DUMMY_NOTIFY_POS_UPDATE_HEADING_ACCURACY, course.HeadingAccuracy());

    // SatelliteInfo

    TPositionSatelliteInfo satInfo;
    positioner.NotifyPositionUpdate(satInfo, request);
    User::WaitForRequest(request);
    CheckExpectedResult(KErrNone, request.Int());

    // basic portion
    courseInfo.GetPosition(pos);

    CheckExpectedRealResult( DUMMY_NOTIFY_POS_UPDATE_LATITUDE3, pos.Latitude());
    CheckExpectedRealResult( DUMMY_NOTIFY_POS_UPDATE_LONGITUDE3, pos.Longitude());
    CheckExpectedRealResult( DUMMY_NOTIFY_POS_UPDATE_ALTITUDE3, pos.Altitude());
    CheckExpectedRealResult( DUMMY_NOTIFY_POS_UPDATE_HORIZONTAL_ACCURACY3, pos.HorizontalAccuracy());
    CheckExpectedRealResult( DUMMY_NOTIFY_POS_UPDATE_VERTICAL_ACCURACY3, pos.VerticalAccuracy());

    // satellite info portion
    CheckExpectedRealResult( DUMMY_NOTIFY_POS_UPDATE_SAT_HDOP, satInfo.HorizontalDoP());
    CheckExpectedRealResult( DUMMY_NOTIFY_POS_UPDATE_SAT_VDOP, satInfo.VerticalDoP());
    CheckExpectedRealResult( DUMMY_NOTIFY_POS_UPDATE_SAT_TDOP, satInfo.TimeDoP());
    CheckExpectedResult( DUMMY_NOTIFY_POS_UPDATE_SAT_TIME, satInfo.SatelliteTime());

    CheckExpectedResult(DUMMY_PSY3_SATELLITE_NUM, satInfo.NumSatellitesInView());

    // satellite portion
    TSatelliteData satellite;
    CheckExpectedResult(KErrNone, satInfo.GetSatelliteData(0, satellite));

    CheckExpectedResult( DUMMY_NOTIFY_POS_UPDATE_SATELLITE_ID1, satellite.SatelliteId());
    CheckExpectedRealResult( DUMMY_NOTIFY_POS_UPDATE_AZIMUTH1, satellite.Azimuth());
    CheckExpectedRealResult( DUMMY_NOTIFY_POS_UPDATE_ELEVATION1, satellite.Elevation());
    CheckExpectedResult( (TBool)DUMMY_NOTIFY_POS_UPDATE_IS_USED1, (TBool)satellite.IsUsed());
    CheckExpectedRealResult( DUMMY_NOTIFY_POS_UPDATE_SIGNAL_STRENGTH1, satellite.SignalStrength());

    CheckExpectedResult(KErrNone, satInfo.GetSatelliteData(1, satellite));

    CheckExpectedResult( DUMMY_NOTIFY_POS_UPDATE_SATELLITE_ID2, satellite.SatelliteId());
    CheckExpectedRealResult( DUMMY_NOTIFY_POS_UPDATE_AZIMUTH2, satellite.Azimuth());
    CheckExpectedRealResult( DUMMY_NOTIFY_POS_UPDATE_ELEVATION2, satellite.Elevation());
    CheckExpectedRealResult( DUMMY_NOTIFY_POS_UPDATE_SIGNAL_STRENGTH2, satellite.SignalStrength());
    CheckExpectedResult( (TBool)DUMMY_NOTIFY_POS_UPDATE_IS_USED2, (TBool)satellite.IsUsed());

    CleanupStack::PopAndDestroy(2, &server);
    return TestStepResult();
}

// 1.17.2 NotifyPositionUpdate with Extended classes, NO SUPPORT
TVerdict CPositionerTestStep::TdLocSrv_Positioner_NotifyPositionUpdate_Extended_UnsupportedL()
{
    StandardPrepareL();
    
    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    RPositioner positioner;

    TPositionModuleId moduleId;
    moduleId.iUid = DUMMY_MODULE_ID1;

    if(KErrNone != positioner.Open(server, moduleId))
        {
        ERR_PRINTF1(KFailedOpenPositionerByModuleID);
        SetTestStepResult(EFail);
        return TestStepResult();
        }
    CleanupClosePushL(positioner);

    CheckExpectedResult(KErrNone, SetAuthenticRequestor(positioner));

    TRequestStatus request;

    // Course Info

    TPositionCourseInfo courseInfo;
    positioner.NotifyPositionUpdate(courseInfo, request);
    User::WaitForRequest(request);
    CheckExpectedResult(KErrArgument, request.Int(), KCourseInfoClassSupported);

    // SatelliteInfo

    TPositionSatelliteInfo satInfo;
    positioner.NotifyPositionUpdate(satInfo, request);
    User::WaitForRequest(request);
    CheckExpectedResult(KErrArgument, request.Int(), KSatelliteInfoClassSupported);

    // custom class
    TExtPosInfo custInfo;
    positioner.NotifyPositionUpdate(custInfo, request);
    User::WaitForRequest(request);
    CheckExpectedResult(KErrArgument, request.Int(), KUnsupportedClassSupported);

    CleanupStack::PopAndDestroy(2, &server);
    return TestStepResult();
}

//
// 
// RPositioner
//
//  Tracking session
//
//

// 1.18.1 Tracking session
TVerdict CPositionerTestStep::TdLocSrv_Positioner_Tracking_SessionL()
{
    StandardPrepareL();
    
    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    RPositioner positioner;

    TPositionModuleId moduleId = {KLcfPsy6UidValue};
    if(KErrNone != positioner.Open(server, moduleId))
        {
        ERR_PRINTF1(KFailedOpenPositionerByModuleID);
        SetTestStepResult(EFail);
        return TestStepResult();
        }
    CleanupClosePushL(positioner);

    CheckExpectedResult(KErrNone, SetAuthenticRequestor(positioner));

    const TTimeIntervalMicroSeconds KPSYDelay = 1 * 100*1000; // 0.1 sec
    const TTimeIntervalMicroSeconds KInterval = 2 * KSecondsToMicro;
    const TTimeIntervalMicroSeconds KMaxDelay = 5 * 100*1000; // 0.5 sec
    const TTimeIntervalMicroSeconds KTimeOut  = 8 * KSecondsToMicro;

    TPositionUpdateOptions updateOptions;
    updateOptions.SetUpdateInterval(KInterval);
    updateOptions.SetUpdateTimeOut(KTimeOut);
    CheckExpectedResult(KErrNone, positioner.SetUpdateOptions(updateOptions));

    TTime stamp0, stamp;
    stamp0.UniversalTime();

    const TUint KTrackingSessionLength = 5;
    for (TUint i = 0; i < KTrackingSessionLength; i++)
        {
        TRequestStatus status;
        TPositionInfo posInfo;

        // set required delay
        HPositionGenericInfo* genInfo = HPositionGenericInfo::NewLC();
        genInfo->SetValue(KPSY6FieldDelayProcessing, KPSYDelay);
        genInfo->SetRequestedField(KPSY6FieldDelayProcessing);

        positioner.NotifyPositionUpdate(*genInfo, status);
        User::WaitForRequest(status);
        CheckExpectedResult(KErrNone, status.Int(), KFailedGetTrackingFix);

        CleanupStack::PopAndDestroy(genInfo);

        // check interval was used
        stamp.UniversalTime();

        TTime expectedTimeStamp = 
            stamp0 + TTimeIntervalMicroSeconds( ((TInt64)i) * KInterval.Int64() );
        TTime expectedMaxDelayedTimeStamp = expectedTimeStamp + KMaxDelay;

        if(stamp < expectedTimeStamp)
            {
            ERR_PRINTF1(KTooFastTrackingFix);
            SetTestStepResult(EFail);
            }
        if(stamp > expectedMaxDelayedTimeStamp)
            {
            ERR_PRINTF1(KTooSlowTrackingFix);
            SetTestStepResult(EFail);
            }
        }

    CleanupStack::PopAndDestroy(2, &server);
    return TestStepResult();
}

// 1.18.2 Tracking session changing
TVerdict CPositionerTestStep::TdLocSrv_Positioner_Tracking_Session_ChangingL()
{
    StandardPrepareL();
    
    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    RPositioner positioner;

    TPositionModuleId moduleId = {KLcfPsy6UidValue};
    if(KErrNone != positioner.Open(server, moduleId))
        {
        ERR_PRINTF1(KFailedOpenPositionerByModuleID); // lcfpsy1
        SetTestStepResult(EFail);
        return TestStepResult();
        }
    CleanupClosePushL(positioner);

    CheckExpectedResult(KErrNone, SetAuthenticRequestor(positioner));

    const TTimeIntervalMicroSeconds KPSYDelay = 1 * 100*1000; // 0.1 sec
    const TTimeIntervalMicroSeconds K1stInterval = 1 * KSecondsToMicro;
    const TTimeIntervalMicroSeconds K2ndInterval = 2 * KSecondsToMicro;
    const TTimeIntervalMicroSeconds KTolerance = 3 * 100*1000; // 0.3 sec
    const TTimeIntervalMicroSeconds KTimeOut  = 8 * KSecondsToMicro;

    TPositionUpdateOptions updateOptions;
    updateOptions.SetUpdateInterval(K1stInterval);
    updateOptions.SetUpdateTimeOut(KTimeOut);
    CheckExpectedResult(KErrNone, positioner.SetUpdateOptions(updateOptions));

    TTime stamp0, stamp;
    stamp0.UniversalTime();

    TBuf<100> logMsg;

    const TUint KTrackingSessionLength = 5;
    for (TUint i = 0; i < KTrackingSessionLength; i++)
        {
        TRequestStatus status;
        TPositionInfo posInfo;

        logMsg.Format(_L("Round 1, Request #%d"), i);

        positioner.NotifyPositionUpdate(posInfo, status);
        User::WaitForRequest(status);
        CheckExpectedResult(KErrNone, status.Int(), KFailedGetTrackingFix);

        // check interval was used
        stamp.UniversalTime();

        TTime expectedTimeStamp = 
            stamp0 + TTimeIntervalMicroSeconds( ((TInt64)i) * K1stInterval.Int64() );
        TTime expectedMaxDelayedTimeStamp = expectedTimeStamp + KTolerance;
        TTime expectedEarliestTimeStamp = expectedTimeStamp - KTolerance;
        if(stamp < expectedEarliestTimeStamp)
            {
            ERR_PRINTF1(KTooFastTrackingFix);
            SetTestStepResult(EFail);
            }
        if(stamp > expectedMaxDelayedTimeStamp)
            {
            ERR_PRINTF1(KTooSlowTrackingFix);
            SetTestStepResult(EFail);
            }
        }

    // Another session
    updateOptions.SetUpdateInterval(K2ndInterval);
    CheckExpectedResult(KErrNone, positioner.SetUpdateOptions(updateOptions));

    // set required delay
    HPositionGenericInfo* genInfo = HPositionGenericInfo::NewLC();
    genInfo->SetValue(KPSY6FieldDelayProcessing, KPSYDelay);
    genInfo->SetRequestedField(KPSY6FieldDelayProcessing);

    TTimeIntervalMicroSeconds KExpectedDelay = 
        K2ndInterval.Int64() + KPSYDelay.Int64();

    for (TUint i = 1; i < KTrackingSessionLength + 1; i++)
        {
        TRequestStatus status;
        TPositionInfo posInfo;

        logMsg.Format(_L("Round 2, Request #%d"), i);

        stamp0.UniversalTime();

        positioner.NotifyPositionUpdate(*genInfo, status);
        User::WaitForRequest(status);
        CheckExpectedResult(KErrNone, status.Int(), KFailedGetTrackingFix);

        stamp.UniversalTime();

        // check interval
        TTimeIntervalMicroSeconds delay = stamp.MicroSecondsFrom(stamp0);
        TTimeIntervalMicroSeconds diff = delay.Int64() - KExpectedDelay.Int64();
        logMsg.Format(_L("Delay: %ld us, Diff: %ld us"), delay.Int64(), diff.Int64());

        if(Abs(diff.Int64()) > KTolerance.Int64())
            {
            ERR_PRINTF1(KDelayOutOfRange);
            SetTestStepResult(EFail);
            }
        }

    CleanupStack::PopAndDestroy(genInfo);
    CleanupStack::PopAndDestroy(2, &server);
    return TestStepResult();
}

// 1.18.3 Cancel Tracking session
TVerdict CPositionerTestStep::TdLocSrv_Positioner_Cancel_Tracking_SessionL()
{ 
    StandardPrepareL();
    
    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    RPositioner positioner;

    TPositionModuleId moduleId = {KLcfPsy6UidValue};
    if(KErrNone != positioner.Open(server, moduleId))
        {
        ERR_PRINTF1(KFailedOpenPositionerByModuleID);
        SetTestStepResult(EFail);
        return TestStepResult();
        }
    CleanupClosePushL(positioner);

    CheckExpectedResult(KErrNone, SetAuthenticRequestor(positioner));

    const TTimeIntervalMicroSeconds KPSYDelay = 2 * KSecondsToMicro;
    const TTimeIntervalMicroSeconds KInterval = 3 * KSecondsToMicro;
    const TTimeIntervalMicroSeconds32 KCancelIntervalDelay = 1 * KSecondsToMicro;
    const TTimeIntervalMicroSeconds32 KCancelPsyDelay = 4 * KSecondsToMicro;
    const TTimeIntervalMicroSeconds KTimeOut  = 8 * KSecondsToMicro;

    TPositionUpdateOptions updateOptions;
    updateOptions.SetUpdateInterval(KInterval);
    updateOptions.SetUpdateTimeOut(KTimeOut);

    TRequestStatus status;
    HPositionGenericInfo* genInfo = HPositionGenericInfo::NewLC();

    // I. Cancel During Update Interval

    // warm-up tracking session
    CheckExpectedResult(KErrNone, positioner.SetUpdateOptions(updateOptions));
    positioner.NotifyPositionUpdate(*genInfo, status);
    User::WaitForRequest(status);
    CheckExpectedResult(KErrNone, status.Int(), KFailedStartFirstTrackingSession);

    // 1st request, cancel during interval
    positioner.NotifyPositionUpdate(*genInfo, status);
    User::After(KCancelIntervalDelay);
    positioner.CancelRequest(EPositionerNotifyPositionUpdate);
    User::WaitForRequest(status);
    CheckExpectedResult(KErrCancel, status.Int(), KFailedCancelDuringInterval);

    // stop tracking session
    updateOptions.SetUpdateInterval(0);
    CheckExpectedResult(KErrNone, positioner.SetUpdateOptions(updateOptions));

    // II. Cancel During PSY Delay

    // warm-up new tracking session
    updateOptions.SetUpdateInterval(KInterval);
    CheckExpectedResult(KErrNone, positioner.SetUpdateOptions(updateOptions));

    positioner.NotifyPositionUpdate(*genInfo, status);
    User::WaitForRequest(status);
    CheckExpectedResult(KErrNone, status.Int(), KFailedStartSecondTrackingSession);

    // 2nd request, cancel during psy delay
    genInfo->SetRequestedField(KPSY6FieldDelayProcessing);
    genInfo->SetValue(KPSY6FieldDelayProcessing, KPSYDelay);
    positioner.NotifyPositionUpdate(*genInfo, status);
    User::After(KCancelPsyDelay);
    positioner.CancelRequest(EPositionerNotifyPositionUpdate);
    User::WaitForRequest(status);
    CheckExpectedResult(KErrCancel, status.Int(), KFailedCancelDuringPsyDelay);

    // stop tracking session
    updateOptions.SetUpdateInterval(0);
    CheckExpectedResult(KErrNone, positioner.SetUpdateOptions(updateOptions));

    // III. Cancel After Tracking Stopped

    // start new tracking
    updateOptions.SetUpdateInterval(KInterval);
    CheckExpectedResult(KErrNone, positioner.SetUpdateOptions(updateOptions));

    // warm up, reset PSY's log
    genInfo->SetRequestedField(KPSY6FieldEventLogReset);
    positioner.NotifyPositionUpdate(*genInfo, status);
    User::WaitForRequest(status);
    CheckExpectedResult(KErrNone, status.Int(), KFailedStartSecondTrackingSession);
    genInfo->ClearRequestedFields();

    // 3rd request, cancel during interval delay, after tracking is stopped
    positioner.NotifyPositionUpdate(*genInfo, status); // this won't reach PSY
    User::After(KCancelIntervalDelay);

    // stop tracking
    updateOptions.SetUpdateInterval(0);
    CheckExpectedResult(KErrNone, positioner.SetUpdateOptions(updateOptions));

    // cancel
    positioner.CancelRequest(EPositionerNotifyPositionUpdate);
    User::WaitForRequest(status);
    CheckExpectedResult(KErrCancel, status.Int(), KFailedCancelAfterStoppedSession);

    // non-tracking request
    genInfo->SetRequestedField(KPSY6FieldEventLog);
    positioner.NotifyPositionUpdate(*genInfo, status);
    User::WaitForRequest(status);
    CheckExpectedResult(KErrNone, status.Int(), KFailedStartSecondTrackingSession);

    // Analyze event log from PSY6
    TInt32 logSize;
    if(KErrNone != genInfo->GetValue(KPSY6FieldEventLogSize, logSize))
        {
        ERR_PRINTF1(KFailedReadLogSize);
        SetTestStepResult(EFail);
        return TestStepResult();
        }
    // check psy's log - no tracking events should be visible
    TPtrC8 ptr;
    User::LeaveIfError(genInfo->GetValue(KPSY6FieldEventLog, ptr));
    TQueryEvent* events = reinterpret_cast<TQueryEvent*>(const_cast<TUint8*>(ptr.Ptr()));

    CheckExpectedResult(EUpdate, events->iEventType, KUpdateEventExpected);
    CheckExpectedResult(ETrackingStart, (events + 1)->iEventType, KTrackingStartEventExpected);
    CheckExpectedResult(ETrackingStop, (events + 2)->iEventType, KETrackingStopEventExpected);
    CheckExpectedResult(EUpdate, (events + 3)->iEventType, KUpdateEventExpected);

    CheckExpectedResult(4, logSize, KWrongEventAmount);

    // cleanup
    CleanupStack::PopAndDestroy(3, &server);
    return TestStepResult();
}

//
// 
// RPositioner
//
//  Module events
//
//

// 1.19.1 Modules disabled/enabled
TVerdict CPositionerTestStep::TdLocSrv_Positioner_ModuleDisabledEnabledL()
{ 
    StandardPrepareL();
    
    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    RPositioner positioner;

    TPositionModuleId moduleId = {KLcfPsy1UidValue};
    if(KErrNone != positioner.Open(server, moduleId))
        {
        ERR_PRINTF1(KFailedOpenPositionerByModuleID);
        SetTestStepResult(EFail);
        return TestStepResult();
        }
    CleanupClosePushL(positioner);

    CheckExpectedResult(KErrNone, SetAuthenticRequestor(positioner));

    TRequestStatus status;
    TPositionInfo posInfo;

    // issue a request
    positioner.NotifyPositionUpdate(posInfo, status);
    User::WaitForRequest(status);
    CheckExpectedResult(status.Int(), KErrNone, KFailedLocationRequest);

    // disable psy and issue request again
    TogglePsyL(moduleId, EFalse);

    positioner.NotifyPositionUpdate(posInfo, status);
    User::WaitForRequest(status);
    CheckExpectedResult(status.Int(), KErrNotFound, KWrongRequestResult);

    // enable and issue request
    TogglePsyL(moduleId, ETrue);

    positioner.NotifyPositionUpdate(posInfo, status);
    User::WaitForRequest(status);
    CheckExpectedResult(status.Int(), KErrNone, KFailedLocationRequest);

    // cleanup
    CleanupStack::PopAndDestroy(2, &server);
    return TestStepResult();
}
