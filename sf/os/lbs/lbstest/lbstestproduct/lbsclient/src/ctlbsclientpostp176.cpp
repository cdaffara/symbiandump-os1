// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "ctlbsclientpostp176.h"
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>
#include <lbs/epos_cposmoduleidlist.h>
// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp176::CT_LbsClientPosTp176(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{  
	_LIT(KTestName, "TP176 - Basic PSY test");
	SetTestStepName(KTestName); 
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp176::~CT_LbsClientPosTp176()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp176::CloseTest
// Always called after the test, even if the test leaves
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp176::CloseTest()
    {
 
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp176::CloseTest
// Always called after the test, even if the test leaves
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp176::InitTestL()
    {

    }

// ---------------------------------------------------------
// CT_LbsClientPosTp176::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp176::StartL()
    {
    _LIT(KService, "service");
    CPosModules *modDb = CPosModules::OpenL();
    CleanupStack::PushL(modDb);
    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();

    moduleUpdate->SetUpdateAvailability(ETrue);
    modDb->UpdateModuleL(iUidTestPsyTp176, *moduleUpdate);
    modDb->UpdateModuleL(iUidTestPsy1, *moduleUpdate);

    CleanupStack::PopAndDestroy(2);

    ConnectL();

    User::LeaveIfError(OpenPositionerByName(iUidTestPsyTp176));
    TPositionInfo info = TPositionInfo();

    // Request data from test psy 176 psy
    TInt err = PerformSyncRequest(KService, &info);

    if (err != KErrNone)
        {
        _LIT(KErrorRequest, "The request was not completed with KErrorNone");
        LogErrorAndLeaveL(KErrorRequest);
        }

    // just to make sure that dynamic quality is updated
    err = PerformSyncRequest(KService, &info);

    if (err != KErrNone)
        {
        _LIT(KErrorRequest, "The request was not completed with KErrorNone");
        LogErrorAndLeaveL(KErrorRequest);
        }


    HPositionGenericInfo* genericInfo = HPositionGenericInfo::NewLC();

    // Request a genericInfo from test psy 176 should fail.    
    err = PerformSyncRequest(KService, genericInfo);


    //Magic number KErrHardwareNotAvailable is used to check that the psy is
    //not responsible for determine if the class is supported. 
    if (err == KErrHardwareNotAvailable)
        {
        _LIT(KErrorRequest, "The type checking was done in the PSY");
        LogErrorAndLeaveL(KErrorRequest);
        }
	else if (err != KErrArgument)
		{
		_LIT(KErrWrongType, "Wrong error type returned");
		LogErrorAndLeaveL(KErrWrongType);
		}

    
    // not set uid
    err = PerformSyncRequest(KService, &info);

    if (err != KErrGeneral)
        {
        _LIT(KErrorRequest, "The request was not completed with KErrGeneral");
        LogErrorAndLeaveL(KErrorRequest);
        }

 
    // incorrect uid, but existing
    err = PerformSyncRequest(KService, &info);
    if (err != KErrGeneral)
        {
        _LIT(KErrorRequest, "The request was not completed with KErrGeneral");
        LogErrorAndLeaveL(KErrorRequest);
        }

    //incorrect uid
    err = PerformSyncRequest(KService, &info);
    if (err != KErrGeneral)
        {
        _LIT(KErrorRequest, "The request was not completed with KErrGeneral");
        LogErrorAndLeaveL(KErrorRequest);
        }


    ClosePositioner();
    Disconnect();

    CleanupStack::PopAndDestroy(); //genericInfo
    
    }



TBool CT_LbsClientPosTp176::IsQualityUpdatedL(TUid aUid)
    {
    TBool updated = ETrue;
    CPosModules *modDb = CPosModules::OpenL();
    CleanupStack::PushL(modDb);
    
    TPositionQuality dynQuality;

    //TTimeIntervalMicroSeconds timeToFirstFix = dynQuality.TimeToNextFix();
    TTimeIntervalMicroSeconds timeToFirstFixBefore;
    TReal32 horAccBefore;
    TReal32 horAccAfter = dynQuality.HorizontalAccuracy();

    if (aUid == iUidTestPsyTp176)
        {
        timeToFirstFixBefore = iQuality.TimeToNextFix();
        horAccBefore = iQuality.HorizontalAccuracy();
        }
    else
        {
        timeToFirstFixBefore = iQualityTestPsy1.TimeToNextFix();
        horAccBefore = iQualityTestPsy1.HorizontalAccuracy();
        }
 

    if (horAccBefore != horAccAfter)
        {
        updated = ETrue;
        iQuality = dynQuality;
        _LIT(KUpdated, "Dynamic Quality is updated");
        INFO_PRINTF1(KUpdated);
        }
    else
        {
        updated = EFalse;
        _LIT(KNotUpdated, "Dynamic Quality is NOT updated");
        INFO_PRINTF1(KNotUpdated);
        }

    CleanupStack::PopAndDestroy();
    return updated;
    }
