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
#include "ctlbsclientpostp218.h"
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>
#include <lbs/epos_cposmoduleidlist.h>
#include <lbssatellite.h>

// CONSTANTS
const TInt MAX_AGE_TIME = 10000000; // 10 sec
const TInt HALF_MAX_AGE_TIME = 5000000; // 5 sec

_LIT(KKallesService, "Kalles Service");

// ================= MEMBER FUNCTIONS =======================

// Constructor.
CT_LbsClientPosTp218::CT_LbsClientPosTp218(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
    {  
    _LIT(KTestName, "TP218 - Max Age, default Psy");
    SetTestStepName(KTestName); 
    }

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp218::~CT_LbsClientPosTp218()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp218::CloseTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp218::CloseTest()
    {
    ClosePositioner();
    iPositioner1.Close();
    iPositioner2.Close();
    Disconnect();
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp218::SetupDefaultPsyL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp218::SetupDefaultPsyL(const TUid aPsyUid)
    {
    CPosModules* db = CPosModules::OpenL();
    CleanupStack::PushL(db);

	SetupPsyL(aPsyUid);
	//setting priority
	TPositionModuleInfo moduleInfoHp;//Hp - high prio;
	TPositionModuleInfo moduleInfoLp;//Lp - low prio;

    CPosModuleIdList* prioList = db->ModuleIdListLC();
	
    db->GetModuleInfoL(prioList->At(0), moduleInfoHp);

    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();

	if (aPsyUid.iUid == moduleInfoHp.ModuleId().iUid)
		{
        moduleUpdate->SetUpdateAvailability(ETrue);
		db->UpdateModuleL((*prioList)[0], *moduleUpdate);
		}
	else
		{
        db->GetModuleInfoL(aPsyUid, moduleInfoLp);
		db->UpdateModuleL(aPsyUid, *moduleUpdate);
        TUint hp = db -> PriorityL(moduleInfoHp.ModuleId());
		TUint lp = db -> PriorityL(moduleInfoLp.ModuleId());
        db->SetModulePriorityL(aPsyUid, hp); //lp
        db->SetModulePriorityL((*prioList)[0], lp); //hp
		}

	moduleUpdate->SetUpdateAvailability(ETrue);
    db -> UpdateModuleL(aPsyUid, *moduleUpdate);
    
	CleanupStack::PopAndDestroy(moduleUpdate);
	CleanupStack::PopAndDestroy(2); // db, prioList
    }


// This flag must be defined if MaxAge should NOT be tested
#ifdef MAX_AGE_NOT_SUPPORTED
// ---------------------------------------------------------
// CT_LbsClientPosTp218::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
// >>>>>>>>>>>>>>>MaxAge NOT supported<<<<<<<<<<<<<<<<<<<<<<
void CT_LbsClientPosTp218::StartL()
    {
    TTimeIntervalMicroSeconds maxAge = TTimeIntervalMicroSeconds(MAX_AGE_TIME);

    // Set iUidTestPsyMaxAge to defaultpsy
    SetupDefaultPsyL(iUidTestPsyMaxAge);

    //connect to EPos
    User::LeaveIfError(iPosServer.Connect());
    TInt err = KErrNone;
    TRequestStatus          myStatus;

    // Open default psy
    User::LeaveIfError(iPositioner.Open(iPosServer));
    TBuf<150> buf;

    SetupGlobalPrivacyL(TPosPrivacy(TPosPrivacy::EAcceptAll, EFalse));

    TPositionUpdateOptions posOption;
    posOption.SetMaxUpdateAge(maxAge);

    _LIT(KTestMaxAge, "Testing that MaxAge is not supported");
    INFO_PRINTF1(KTestMaxAge);
	err = iPositioner.SetUpdateOptions(posOption);
	_LIT(KErr, "MaxAge should NOT be supported instead code %d was returned");
	AssertTrueL(err == KErrNotSupported, KErr, err);

    TPositionInfo posInfo1 = TPositionInfo();
    TPositionInfo posInfo2 = TPositionInfo();

    iPositioner.SetRequestor(CRequestor::ERequestorService,
                             CRequestor::EFormatApplication, KKallesService);

    // Get position, first position
    iPositioner.NotifyPositionUpdate(posInfo1, myStatus);
   
    User::WaitForRequest(myStatus);
    
    err = myStatus.Int();
    _LIT(KNotifyPositionUpdate, "Error from NotifyPositionUpdate, err %d");
    AssertTrueL(err == KErrNone, KNotifyPositionUpdate, err);

    _LIT(KWaitMaxAge, "Wait for half Max Age...");
    INFO_PRINTF1(KWaitMaxAge);
    // Wait half MAX_AGE_TIME
    User::After(HALF_MAX_AGE_TIME);

    // Get position again, should be the same as first position
    iPositioner.NotifyPositionUpdate(posInfo2, myStatus);
    User::WaitForRequest(myStatus);
    err = myStatus.Int();
    AssertTrueL(err == KErrNone, KNotifyPositionUpdate, err);

    if (ComparePositionL(posInfo1, posInfo2))
        {
        _LIT(KErrorAndLeave, 
        "The old position was returned from positioner, MaxAge seems to be supported!!!");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    }

#else
// ---------------------------------------------------------
// CT_LbsClientPosTp218::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
// >>>>>>>>>>>>>>>>>>>>MaxAge supported<<<<<<<<<<<<<<<<<<<<<<
void CT_LbsClientPosTp218::StartL()
    {   
    TTimeIntervalMicroSeconds maxAge = TTimeIntervalMicroSeconds(MAX_AGE_TIME);

    // Set iUidTestPsyMaxAge to defaultpsy
    SetupDefaultPsyL(iUidTestPsyMaxAge);

    //connect to EPos
    User::LeaveIfError(iPosServer.Connect());
    TInt err = KErrNone;

    // Open default psy
    User::LeaveIfError(iPositioner.Open(iPosServer));
    TBuf<150> buf;

    TPositionInfo posInfo = TPositionInfo();
    TPositionInfo posInfo2 = TPositionInfo();
    TRequestStatus          myStatus;

    iPositioner.SetRequestor(CRequestor::ERequestorService,
                             CRequestor::EFormatApplication, KKallesService);

    // Make sure that it is possible to get position from psy
    iPositioner.NotifyPositionUpdate(posInfo, myStatus);
    User::WaitForRequest(myStatus);
    err = myStatus.Int();
    _LIT(KNotifyPositionUpdate, "Error from NotifyPositionUpdate, err %d");
    AssertTrueL(err == KErrNone, KNotifyPositionUpdate, err);
    
    // Make sure that the psy returns differens positions
    iPositioner.NotifyPositionUpdate(posInfo2, myStatus);
    User::WaitForRequest(myStatus);
    err = myStatus.Int();
    AssertTrueL(err == KErrNone, KNotifyPositionUpdate, err);


    TPositionUpdateOptions returnPosOption;
    iPositioner.GetUpdateOptions(returnPosOption);
    TTimeIntervalMicroSeconds returnInterval = returnPosOption.MaxUpdateAge();

    _LIT(KMaxAge, "Max age should not hold a value");
    AssertTrueL(returnInterval == TTimeIntervalMicroSeconds(0), KMaxAge, KErrGeneral);

    TPositionUpdateOptions posOption;
    posOption.SetMaxUpdateAge(maxAge);

	err = iPositioner.SetUpdateOptions(posOption);
	_LIT(KChange, "Error when setting max age in SetMaxAge, error %d");
	AssertTrueL(err == KErrNone, KChange, err);

    TPositionUpdateOptions returnPosOption2;
    iPositioner.GetUpdateOptions(returnPosOption2);
    
    returnInterval = returnPosOption2.MaxUpdateAge();
    _LIT(KMaxAgeNotMatch, "Max age does not match, %d %d ");
    buf.Format(KMaxAgeNotMatch, returnInterval.Int64(), MAX_AGE_TIME);
    AssertTrueL(returnInterval == maxAge, buf, KErrGeneral);
    
    TPositionInfo posInfo3 = TPositionInfo();
    TPositionInfo posInfo4 = TPositionInfo();
    TPositionInfo posInfo5 = TPositionInfo();
    // Get position, first position
    iPositioner.NotifyPositionUpdate(posInfo3, myStatus);
   
    User::WaitForRequest(myStatus);
    
    err = myStatus.Int();
    AssertTrueL(err == KErrNone, KNotifyPositionUpdate, err);

    _LIT(KWaitMAxAge, "Wait for half Max Age...");
    INFO_PRINTF1(KWaitMAxAge);
    // Wait half MAX_AGE_TIME
    User::After(HALF_MAX_AGE_TIME);

    // Get position again, should be the same as first position
    iPositioner.NotifyPositionUpdate(posInfo4, myStatus);
    User::WaitForRequest(myStatus);
    err = myStatus.Int();
    AssertTrueL(err == KErrNone, KNotifyPositionUpdate, err);

    if (!ComparePositionL(posInfo3, posInfo4))
        {
        _LIT(KErrorAndLeave, "The old position was not returned from positioner!!!");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    // Wait MAX_AGE_TIME
    _LIT(KWait, "Waiting waiting...");
    INFO_PRINTF1(KWait);
    User::After(MAX_AGE_TIME);

    // Get position again, should NOT be the same as earlier
    iPositioner.NotifyPositionUpdate(posInfo5, myStatus);
   
    User::WaitForRequest(myStatus);
    err = myStatus.Int();
    AssertTrueL(err == KErrNone, KNotifyPositionUpdate, err);

    if (ComparePositionL(posInfo4, posInfo5))
        {
        _LIT(KErrorAndLeave, "(1)The old position was returned from positioner!!!");
        LogErrorAndLeaveL(KErrorAndLeave);
        }
    
    _LIT(KChangePSY11, "Changing default psy to Psy11");
    INFO_PRINTF1(KChangePSY11);
    // Set iUidTestPsy11 psy to defaultpsy
    SetupDefaultPsyL(iUidTestPsy11);

    // Wait some time to make changes occur
    User::After(5000000);
    TPositionInfo posInfo6 = TPositionInfo();
    TPositionInfo posInfo7 = TPositionInfo();

    // Get position again, should NOT be the same as earlier
    iPositioner.NotifyPositionUpdate(posInfo6, myStatus);
   
    User::WaitForRequest(myStatus);
    err = myStatus.Int();
    _LIT(KErr1, "Error from NotifyPositionUpdate for default psy, err1: %d");
    AssertTrueL(err == KErrNone, KErr1, err);

    if (ComparePositionL(posInfo5, posInfo6))
        {
        _LIT(KErrorAndLeave, 
        		"(2)The old position was returned from positioner!!!");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    // Get position again, should NOT be the same as earlier
    iPositioner.NotifyPositionUpdate(posInfo7, myStatus);
   
    User::WaitForRequest(myStatus);
    err = myStatus.Int();
    // TestPsy11 returns error every second request
    _LIT(KErr2, "Error from NotifyPositionUpdate for default psy, err2: %d");
    AssertTrueL(err == KPositionPartialUpdate, KErr2, err);

    if (ComparePositionL(posInfo6, posInfo7))
        {
        _LIT(KErrorAndLeave, 
        		"The old position was returned from default positioner!!!");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    ClosePositioner();
    Disconnect();
    }

#endif

// ---------------------------------------------------------
// CT_LbsClientPosTp218::ComparePositionL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CT_LbsClientPosTp218::ComparePositionL(TPositionInfo aPosInfo1, TPositionInfo aPosInfo2)
    {
    _LIT(KFunctionName, "We are inside ComparePositionL()");
    INFO_PRINTF1(KFunctionName);
    TBool result = ETrue;
    TBuf<150> buf;

    TPosition pos1 = TPosition();
    TPosition pos2 = TPosition();
    aPosInfo1.GetPosition(pos1);
    aPosInfo2.GetPosition(pos2);

    TTime time1 = pos1.Time();
    TTime time2 = pos2.Time();

    if (time1 == time2)
        {
        _LIT(KEqualTime, "Equal Time");
        INFO_PRINTF1(KEqualTime);
        }
    else 
    	{
    	_LIT(KNotEqualTime, "NOT Equal Time");
    	INFO_PRINTF1(KNotEqualTime);
    	}

    if ((time1 != time2) || (pos1.Latitude() != pos2.Latitude()) || 
        (pos1.Longitude() != pos2.Longitude()) || 
        (pos1.Altitude() != pos2.Altitude()) )
        {
        // Not equal
        result = EFalse;
        _LIT(KNotEqual, "Not equal");
        INFO_PRINTF1(KNotEqual);
        _LIT(KPosition1, "Position1: Lat %f Long %f Alt %f");
        buf.Format(KPosition1, pos1.Latitude(), pos1.Longitude() ,pos1.Altitude());
        INFO_PRINTF1(buf);
        _LIT(KPosition2, "Position2: Lat %f Long %f Alt %f");
        buf.Format(KPosition2, pos2.Latitude(), pos2.Longitude() ,pos2.Altitude());
        INFO_PRINTF1(buf);
        }
    else
        {
        _LIT(KEqual, "Equal");
        INFO_PRINTF1(KEqual);
        result = ETrue;
        _LIT(KPosition1, "Position1: Lat %f Long %f Alt %f");
        buf.Format( KPosition1, pos1.Latitude(), pos1.Longitude() ,pos1.Altitude());
        INFO_PRINTF1(buf);
        }

    return result;
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp218::AssertTrueSecL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp218::AssertTrueL(TBool aCondition, const TDesC& aErrorMsg, TInt aErrorCode)
    {
  	if (!aCondition)
		{
        // LogErrorAndLeave does not leave if KErrNone is error
        if (aErrorCode == KErrNone) aErrorCode = KErrGeneral;
		TBuf<100> buf;
		buf.Format(aErrorMsg, aErrorCode);
		LogErrorAndLeaveL(buf, aErrorCode);
		}
    }


