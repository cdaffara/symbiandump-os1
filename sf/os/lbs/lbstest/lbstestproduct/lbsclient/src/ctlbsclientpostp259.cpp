// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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



//  INCLUDES
#include "ctlbsclientpostp259.h"

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp259::CT_LbsClientPosTp259(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent) 
	{
	_LIT(KTestName, "TP259 - Cancel Request with incorrect enum values");
	SetTestStepName(KTestName); 
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp259::~CT_LbsClientPosTp259()
	{
	}
// ---------------------------------------------------------
// CT_LbsClientPosTp259::CloseTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp259::CloseTest()
    {
    ClosePositioner(); 
    Disconnect();
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp259::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp259::StartL()
    {
    // TestPart1 tests the separate case described in Error Report 
    // "SBUY-5W8KJ6-Bug in subsession selection when forwarding requests"
    TestPart1L();
    
    // Testpart2 tests some other scenarios not included in TestPart1L
    TestPart2L();
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp259::TestPart1L
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp259::TestPart1L()
    {
    _LIT(KFunctionsName, "We are inside TestPart1L()");
    INFO_PRINTF1(KFunctionsName);
    ConnectL();

    // First call the RPosServer::CancelRequest method with inparameter 
    // EPositionerNotifyPositionUpdate and then call the RPositioner::CancelRequest
    // method with EPositionServerNotifyModuleStatusEvent

    _LIT(KCancelRequest, "iPosServer.CancelRequest(EPositionerNotifyPositionUpdate)");
    INFO_PRINTF1(KCancelRequest);
    TInt cancelErr = iPosServer.CancelRequest(EPositionerNotifyPositionUpdate);
    if (cancelErr != KErrNotSupported)
    	{
    	_LIT(KErrorAndLeave, "The CancelRequest(EPositionerNotifyPositionUpdate) call did not return KErrNotSupported");
    	LogErrorAndLeaveL(KErrorAndLeave);
    	}
    
    SetupPsyL(iUidMultiPsy);
    User::LeaveIfError(OpenPositionerByName(iUidMultiPsy));

    _LIT(KModuleStatusEvent, "iPositioner.CancelRequest(EPositionServerNotifyModuleStatusEvent);");
    INFO_PRINTF1(KModuleStatusEvent);
    TInt err = iPositioner.CancelRequest(EPositionServerNotifyModuleStatusEvent);
    if (err != KErrNotSupported)
    	{
    	_LIT(KErrorAndLeave, "The CancelRequest(EPositionServerNotifyModuleStatusEvent) call did not return KErrNotFound");
    	LogErrorAndLeaveL(KErrorAndLeave);
    	}
    
    ClosePositioner();
    Disconnect();
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp259::TestPart2L
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp259::TestPart2L()
    {
    _LIT(KFunctionsName,"We are inside TestPart2L()");
    INFO_PRINTF1(KFunctionsName);
    SetupPsyL(iUidMultiPsy);
    ConnectL();
    TInt err = KErrNone;
    User::LeaveIfError(OpenPositionerByName(iUidMultiPsy));

    err = iPositioner.CancelRequest(EPositionServerNotifyModuleStatusEvent);
    if (err != KErrNotSupported)
        {
        _LIT(KErrorAndLeave, "The CancelRequest(EPositionServerNotifyModuleStatusEvent) call did not return KErrNotFound");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    // Verify that a RPosServer.CancelRequest(EPositionServerNotifyModuleStatusEvent) (incorrect call)
    // call does not affect the RPositioner.NotifyPositionUpdate call
    _LIT(KKalle, "Kalle");
    PerformRequestL(KKalle, CRequestor::ERequestorService, CRequestor::EFormatTelephone); 
    err = iPosServer.CancelRequest(EPositionServerNotifyModuleStatusEvent);
    if (err != KErrNotFound)
        {
        _LIT(KErrorAndLeave, "The CancelRequest(EPositionServerNotifyModuleStatusEvent) call did not return KErrNotFound");
        LogErrorAndLeaveL(KErrorAndLeave);
        }
    CheckRequestResultL(KErrNone);

    // Verify that a RPositioner.CancelRequest(EPositionServerNotifyModuleStatusEvent) (incorrect call)
    // call does not affect the RPositioner.NotifyPositionUpdate call
    PerformRequestL(KKalle, CRequestor::ERequestorService, CRequestor::EFormatTelephone); 
    err = iPositioner.CancelRequest(EPositionServerNotifyModuleStatusEvent);
    if (err != KErrNotSupported)
        {
        _LIT(KErrorAndLeave, "The CancelRequest(EPositionServerNotifyModuleStatusEvent) call did not return KErrNotFound");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    CheckRequestResultL(KErrNone);

    err = iPositioner.CancelRequest(EPositionerNotifyPositionUpdate);
    if (err != KErrNotFound) 
        {
        _LIT(KErrorAndLeave, "The CancelRequest(EPositionerNotifyPositionUpdate) call did not return KErrNotFound");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    ClosePositioner();

    // According to error report "SBUY-5W8KJ6 - Bug in subsession selection when forwarding requests"
    // CancelRequest call will return with KErrNotSupported
    TInt cancelErr = iPosServer.CancelRequest(EPositionerNotifyPositionUpdate);
    if (cancelErr != KErrNotSupported)
        {
        _LIT(KErrorAndLeave, "The CancelRequest(EPositionerNotifyPositionUpdate) call did not return KErrNotSupported");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    // According to error report "SBUY-5W8KJ6 - Bug in subsession selection when forwarding requests"
    // CancelRequest call will return with KErrNotSupported
    _LIT(KLastKnownPosition, "iPosServer.CancelRequest(EPositionerGetLastKnownPosition)");
    INFO_PRINTF1(KLastKnownPosition);
    cancelErr = iPosServer.CancelRequest(EPositionerGetLastKnownPosition);
    if (cancelErr != KErrNotSupported)
        {
        _LIT(KErrorAndLeave, "The CancelRequest(EPositionerGetLastKnownPosition) call did not return KErrNotSupported");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    //Make a request and then cancel a GetLastKnownPosition request that hasn't been made
    //and check that the position request is not affected
    User::LeaveIfError(OpenPositionerByName(iUidMultiPsy));
    PerformRequestL(KKalle, CRequestor::ERequestorService, CRequestor::EFormatTelephone);

    cancelErr = iPosServer.CancelRequest(EPositionerGetLastKnownPosition);
    if (cancelErr != KErrNotSupported) 
        {
        _LIT(KErrorAndLeave, "The CancelRequest(EPositionerGetLastKnownPosition call did not return KErrNotSupported");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    CheckRequestResultL(KErrNone);

    ClosePositioner();
    Disconnect();
    }
