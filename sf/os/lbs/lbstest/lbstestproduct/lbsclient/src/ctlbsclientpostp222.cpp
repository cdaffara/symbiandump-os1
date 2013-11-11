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
#include "ctlbsclientpostp222.h"

// CONSTANTS
// ================= MEMBER FUNCTIONS =======================

// Constructor.
CT_LbsClientPosTp222::CT_LbsClientPosTp222(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
    {  
    _LIT(KTestName, "TP222 - Partial Update test");
    SetTestStepName(KTestName); 
    }

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp222::~CT_LbsClientPosTp222()
	{
	}

// ---------------------------------------------------------
// CPosTp222::CloseTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp222::CloseTest()
    {
    ClosePositioner();
    Disconnect();
    }   

// This flag must be defined if testing partial update NOT supported
#ifdef PARTIAL_UPDATE_NOT_SUPPORTED
// ---------------------------------------------------------
// CPosTp222::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
// If partial update is NOT supported
void CT_LbsClientPosTp222::StartL()
    {
    _LIT(KPartialUpdateTest, "-----Testing that PartialUpdate is NOT supported-----");
    INFO_PRINTF1(KPartialUpdateTest);
    SetupPsyL(iUidTestPsyPartialUpdate);
    SetupGlobalPrivacyL(TPosPrivacy(TPosPrivacy::EAcceptAll, EFalse));
    _LIT(KService, "PellesKattlåda");

    ConnectL();
    User::LeaveIfError(OpenPositionerByName(iUidTestPsyPartialUpdate));

    TPositionUpdateOptions updateOptions;
    // Check that partial update is not supported
    updateOptions.SetAcceptPartialUpdates(ETrue);
    
    TInt res = iPositioner.SetUpdateOptions(updateOptions);
    _LIT(KSetUpdateOptions, "SetUpdateOptions should not be supported, instead error %d was return");
    AssertTrueL(res == KErrNotSupported, KSetUpdateOptions, res);
    
    TPositionInfo info = TPositionInfo();
    TInt err = PerformSyncRequest(KService, &info);
    _LIT(KErrRequest2, "error when performing sync request nr2, err: %d");
    AssertTrueL(err == KErrNone, KErrRequest2, err);
    // Check that a "full" position is returned
    TPosition pos = TPosition();
    info.GetPosition(pos);

    if (pos.Latitude() == 30 && pos.Longitude() == 40 && pos.Altitude() == 50)
        {
        _LIT(KErrorAndLeave, 
        	"Partial update position returned from positioner when this feature should be disabled");
        LogErrorAndLeaveL(KErrorAndLeave);
        }   

    ClosePositioner();
    Disconnect();

    _LIT(KFunctionDone, "Test Completed successfully");
    INFO_PRINTF1(KFunctionDone);
    }

#else
// ---------------------------------------------------------
// CPosTp222::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
// If partial update is supported
void CT_LbsClientPosTp222::StartL()
    {
    _LIT(KPartialUpdateTest, "-----Testing that PartialUpdate is supported-----");
    INFO_PRINTF1(KPartialUpdateTest);
    SetupPsyL(iUidTestPsyPartialUpdate);

    ConnectL();
    User::LeaveIfError(OpenPositionerByName(iUidTestPsyPartialUpdate));
    _LIT(KService, "PellesKaninlaada");

    TPositionUpdateOptions updateOptions;
    TPositionUpdateOptions resultOptions;
    // Check that partial update is not set
    TBool res = updateOptions.AcceptPartialUpdates();
    if (res)
        {
        _LIT(KErrorAndLeave, "Partial update default is not correct");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    updateOptions.SetAcceptPartialUpdates(ETrue);
    TInt response = iPositioner.SetUpdateOptions(updateOptions);
    _LIT(KSetUpdateOptions, "SetUpdateOptions returned error: %d");
    AssertTrueL(response == KErrNone, KSetUpdateOptions, res);

    // Check that partial update has been set
    iPositioner.GetUpdateOptions(resultOptions);
    res = resultOptions.AcceptPartialUpdates();
    if (!res)
        {
        _LIT(KErrorAndLeave, "Partial update not correctly set");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    TPositionInfo info = TPositionInfo();
    TInt err = PerformSyncRequest(KService, &info);
    _LIT(KErrRequest, "error when performing sync request, err: %d");
    AssertTrueL(err == KPositionPartialUpdate, KErrRequest, err);
    
    //Check that a "partial update" position is returned
    TPosition pos = TPosition();
    info.GetPosition(pos);

    if (pos.Latitude() != 30 || pos.Longitude() != 40 || pos.Altitude() != 50)
        {
        _LIT(KErrorAndLeave,
        		"Wrong position returned from positioner(partial update)");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    // Disable partialupdate
    updateOptions.SetAcceptPartialUpdates(EFalse);
    response = iPositioner.SetUpdateOptions(updateOptions);
    _LIT(KSetUpdateOptions1, 
    		"SetUpdateOptions returned error when disabling partial update err: %d");
    AssertTrueL(response == KErrNone, KSetUpdateOptions1, res);

    // Check that partial update has been disabled
    iPositioner.GetUpdateOptions(resultOptions);
    res = resultOptions.AcceptPartialUpdates();
    if (res)
        {
        _LIT(KErrorAndLeave, "Partial update should not be set");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    TPositionInfo info2 = TPositionInfo();
    err = PerformSyncRequest(KService, &info2);
    _LIT(KErrRequest2, "error when performing sync request nr2, err: %d");
    AssertTrueL(err == KErrNone, KErrRequest2, err);
    // Check that a "full" position is returned
    TPosition pos2 = TPosition();
    info2.GetPosition(pos2);

    if (pos2.Latitude() == 30 && pos2.Longitude() == 40 && pos2.Altitude() == 50)
        {
        _LIT(KErrorAndLeave, "Partial update position returned from positioner");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    ClosePositioner();
    Disconnect();
    _LIT(KFunctionDone, "Test Completed successfully");
    INFO_PRINTF1(KFunctionDone);
    }
#endif



