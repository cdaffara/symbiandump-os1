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
#include "ctlbsclientpostp1.h"
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>
#include <lbs/epos_cposmoduleidlist.h>
#include <lbssatellite.h>

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp1::CT_LbsClientPosTp1(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{  
	_LIT(KTestName, "TP1 - Request location from default PSY");
	SetTestStepName(KTestName); 
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp1::~CT_LbsClientPosTp1()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp1::CloseTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp1::CloseTest()
    {
    ClosePositioner();
    Disconnect();
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp1::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp1::StartL()
    {
    ConnectL();
    
    CPosModules* db = CPosModules::OpenL();
    CleanupStack::PushL(db); 

    SetupPsyL(iUidMultiPsy);

    _LIT(KSetupPSY, "TP1: Has Setup Multi PSY");
    INFO_PRINTF1(KSetupPSY);
    
    //setting priority
	TPositionModuleInfo moduleInfoHp;//Hp - high prio;
	TPositionModuleInfo moduleInfoLp;//Lp - low prio;

    CPosModuleIdList* prioList = db->ModuleIdListLC();	
    db->GetModuleInfoL(prioList->At(0), moduleInfoHp);

    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();
    moduleUpdate->SetUpdateAvailability(ETrue);

    if (iUidMultiPsy.iUid == moduleInfoHp.ModuleId().iUid)
		{
		// Case when Multi PSY has highest prio: 
		// Ensuring it is enabled
	    _LIT(KMultiHighest, "TP1: Case when Multi PSY has highest prio");
	    INFO_PRINTF1(KMultiHighest);	    
		db->UpdateModuleL((*prioList)[0], *moduleUpdate);
		}
	else
		{
		// Case when Multi PSY has not highest prio: 
		// 
	    _LIT(KMultiNotHighest, "TP1: Case when Multi PSY has NOT highest prio");	    
	    INFO_PRINTF1(KMultiNotHighest);	    
        db->GetModuleInfoL(iUidMultiPsy, moduleInfoLp);
		// Ensuring it is enabled
		db->UpdateModuleL(iUidMultiPsy, *moduleUpdate);

		TUint hp = db->PriorityL(moduleInfoHp.ModuleId());
		TUint lp = db->PriorityL(moduleInfoLp.ModuleId());

		// Changing prio of highest prio PSY and Multi PSY.
		// Setting Multi PSY to highest prio
		db->SetModulePriorityL(iUidMultiPsy, hp);
        db->SetModulePriorityL((*prioList)[0], lp); 
		}

	_LIT(KTestPSY1On, "TP1: Enabling Test PSY 1");
    INFO_PRINTF1(KTestPSY1On);	    
	db -> UpdateModuleL(iUidTestPsy1, *moduleUpdate);
	_LIT(KTestPSY2On, "TP1: Enabling Test PSY 2");
    INFO_PRINTF1(KTestPSY2On);
	db -> UpdateModuleL(iUidTestPsy2, *moduleUpdate);
    
	CleanupStack::PopAndDestroy(moduleUpdate);
	CleanupStack::PopAndDestroy(2, db); // db, prioList

    User::LeaveIfError(OpenPositioner());

    // Request data from default psy should be Multi psy
    _LIT(KService, "service");
    TPositionInfo posInfo = TPositionInfo();
    TInt err = PerformSyncRequest(KService, &posInfo);
    if (err != KErrNone)
        {
        _LIT(KErrorRequest, "The request was not completed with KErrNone");
        LogErrorAndLeaveL(KErrorRequest, err);
        }
	
    if (iUidMultiPsy.iUid != posInfo.ModuleId().iUid)
		{
		_LIT(KIncorrectPsyError, "TP1. Position from wrong PSY received");
		LogErrorAndLeaveL(KIncorrectPsyError);
		}

	HPositionGenericInfo* genericInfo = HPositionGenericInfo::NewLC();
	err = PerformSyncRequest(KService, &(*genericInfo));
    if (err != KErrNone)
        {
        _LIT(KErrorRequest, "The request was not completed with KErrNone");
        LogErrorAndLeaveL(KErrorRequest);
        }

	 if (iUidMultiPsy.iUid != (genericInfo -> ModuleId()).iUid)
		{
		_LIT(KIncorrectPsyError,
           "TP1. Position from wrong PSY received");
		LogErrorAndLeaveL(KIncorrectPsyError);
		}

	TPositionCourseInfo courseInfo = TPositionCourseInfo();
	err = PerformSyncRequest(KService, &courseInfo);
    if (err != KErrNone)
        {
        _LIT(KErrorRequest, "Course info supported by default, but request failed");
        LogErrorAndLeaveL(KErrorRequest);
        }

	TPositionSatelliteInfo satelliteInfo = TPositionSatelliteInfo();
	err = PerformSyncRequest(KService, &satelliteInfo);
    if (err != KErrNone)
        {
        _LIT(KErrorRequest, "Satellite info supported by default, but request failed");
        LogErrorAndLeaveL(KErrorRequest);
        }	

    //Changing the priority and verifiy that it still possilbe
    //to request updated from a psy if basic position information
    //is used and that it is not possilbe to request sat or cource
    //any more.
    SetupPsyL(iUidTestPsy2);
    User::After(3000000);

    // Request data from default psy should be test psy 2 now
    err = PerformSyncRequest(KService, &posInfo);
    if (err != KErrNone)
        {
        _LIT(KErrorRequest, "The request was not completed with KErrNone");
        LogErrorAndLeaveL(KErrorRequest);
        }
	
    if (iUidTestPsy2.iUid != posInfo.ModuleId().iUid)
		{
		_LIT(KIncorrectPsyError,
           "TP1. Position from wrong PSY received");
		LogErrorAndLeaveL(KIncorrectPsyError);
		}

	err = PerformSyncRequest(KService, &(*genericInfo));
    if (err != KErrNone)
        {
        _LIT(KErrorRequest, "The request was not completed with KErrNone");
        LogErrorAndLeaveL(KErrorRequest);
        }

	 if (iUidTestPsy2.iUid != (genericInfo -> ModuleId()).iUid)
		{
		_LIT(KIncorrectPsyError,
           "TP1. Position from wrong PSY received");
		LogErrorAndLeaveL(KIncorrectPsyError);
        }

    err = PerformSyncRequest(KService, &courseInfo);
    if (err != KErrArgument)
        {
        _LIT(KErrorRequest, "Course info not supported by default, KErrArgument not returned");
        LogErrorAndLeaveL(KErrorRequest);
        }


	err = PerformSyncRequest(KService, &satelliteInfo);
    if (err != KErrArgument)
        {
        _LIT(KErrorRequest, "Satellite info not supported by default, KErrArgument not returned");
        LogErrorAndLeaveL(KErrorRequest);
        }

    //check that it is possilbe to change the different
    //type of class info during periodic request and
    //that the periodic update will fail if the class
    //type is incorrect.
    TTimeIntervalMicroSeconds interval = TTimeIntervalMicroSeconds(1000000);
    TPositionUpdateOptions posOption;
	posOption.SetUpdateInterval(interval);
	_LIT(KUpdateErr, "Error when setting update interval,  %d");
	err = iPositioner.SetUpdateOptions(posOption);
    AssertTrueL(err == KErrNone, KUpdateErr, err);

    _LIT(KErrPeriodic, "The periodic request did not return KErrNone, %d");
    err = PerformSyncRequest(KService, &posInfo);
    AssertTrueL(err == KErrNone , KErrPeriodic, err);
    
    _LIT(KErrPeriodicArr, "The periodic request did not return KErrArgument, %d");
    err = PerformSyncRequest(KService, &satelliteInfo);
    AssertTrueL(err == KErrArgument , KErrPeriodicArr, err);
    SetupPsyL(iUidMultiPsy);
    User::After(3000000);
    err = PerformSyncRequest(KService, &satelliteInfo);
    AssertTrueL(err == KErrNone , KErrPeriodic, err);

    CleanupStack::PopAndDestroy(genericInfo);
    ClosePositioner();
    Disconnect();
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp1::AssertTrueSecL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp1::AssertTrueL(TBool aCondition, const TDesC& aErrorMsg, TInt aErrorCode)
    {
  	if (!aCondition)
		{
		TBuf<100> buf;
		buf.Format(aErrorMsg, aErrorCode);
		LogErrorAndLeaveL(buf);
		}
    }

