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
#include "ctlbsclientpostp217.h"
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>
#include <lbs/epos_cposmoduleidlist.h>

// CONSTANTS
const TInt MAX_AGE_TIME = 10000000; // 10 sec

_LIT(KKallesService, "Kalles Service");

// ================= MEMBER FUNCTIONS =======================

// Constructor.
CT_LbsClientPosTp217::CT_LbsClientPosTp217(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
    {  
    _LIT(KTestName, "TP217 - Max Age, specific Psy");
    SetTestStepName(KTestName); 
    }

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp217::~CT_LbsClientPosTp217()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp217::CloseTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp217::CloseTest()
    {
    ClosePositioner();
    Disconnect();
    }

// This flag must be defined if MaxAge is NOT supported
#ifdef MAX_AGE_NOT_SUPPORTED

// ---------------------------------------------------------
// CT_LbsClientPosTp217::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
// MaxAge NOT supported
void CT_LbsClientPosTp217::StartL()
    {
    TTimeIntervalMicroSeconds maxAge = TTimeIntervalMicroSeconds(MAX_AGE_TIME);

    SetupPsyL(iUidTestPsyMaxAge);

    //connect to EPos
    User::LeaveIfError(iPosServer.Connect());
    TInt err = KErrNone;
    TRequestStatus          myStatus;

    // Open default psy
    User::LeaveIfError(iPositioner.Open(iPosServer));
    TBuf<150> buf;

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
// CT_LbsClientPosTp217::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
// MaxAge supported
void CT_LbsClientPosTp217::StartL()
    {
    TTimeIntervalMicroSeconds maxAge = TTimeIntervalMicroSeconds(MAX_AGE_TIME);

    //connect to EPos
    User::LeaveIfError(iPosServer.Connect());
    TInt err = KErrNone;
    
    SetupPsyL(iUidTestPsyMaxAge);
    
    //Open subsession
    User::LeaveIfError(iPositioner.Open(iPosServer, iUidTestPsyMaxAge));
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
    _LIT(KNotifyPositionUpdate, "Error from NotifyPositionUpdate");
    AssertTrueL(err == KErrNone, KNotifyPositionUpdate, err);
    
    const TTimeIntervalMicroSeconds32 KSomeTime(1000000); //1 sec
    User::After(KSomeTime);

    // Make sure that the psy returns differens positions
    iPositioner.NotifyPositionUpdate(posInfo2, myStatus);
    User::WaitForRequest(myStatus);
    err = myStatus.Int();
    AssertTrueL(err == KErrNone, KNotifyPositionUpdate, err);

    if (ComparePositionL(posInfo, posInfo2))
        {
        _LIT(KErrorAndLeave, "The same position was returned from positioner!!!");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    TPositionUpdateOptions returnPosOption;
    iPositioner.GetUpdateOptions(returnPosOption);
    TTimeIntervalMicroSeconds returnInterval = returnPosOption.MaxUpdateAge();

    _LIT(KMageAge, "Max age should not hold a value");
    AssertTrueL(returnInterval == TTimeIntervalMicroSeconds(0), 
    		KMageAge, KErrGeneral);

    TPositionUpdateOptions posOption;
	posOption.SetMaxUpdateAge(maxAge);
	err = iPositioner.SetUpdateOptions(posOption);

	_LIT(KChange, "ERROR: When setting max age in SetMaxAge, error %d");
	AssertTrueL(err == KErrNone, KChange, err);

    TPositionUpdateOptions returnPosOption2;
    iPositioner.GetUpdateOptions(returnPosOption2);
    
    returnInterval = returnPosOption2.MaxUpdateAge();
    _LIT(KNotMatch, "Max age does not match, %d %d ");
    buf.Format(KNotMatch, returnInterval.Int64(), MAX_AGE_TIME);

    AssertTrueL(returnInterval == maxAge, buf, KErrGeneral);
    
    TPositionInfo posInfo3 = TPositionInfo();
    TPositionInfo posInfo4 = TPositionInfo();
    TPositionInfo posInfo5 = TPositionInfo();
    // Get position, first position
    iPositioner.NotifyPositionUpdate(posInfo3, myStatus);
   
    User::WaitForRequest(myStatus);
    err = myStatus.Int();
    AssertTrueL(err == KErrNone, KNotifyPositionUpdate, err);

    TPosition position = TPosition();
    posInfo.GetPosition(position);

    // Wait half MAX_AGE_TIME
	const TTimeIntervalMicroSeconds32 KHalfTime(MAX_AGE_TIME/2); //5 sec
    User::After(KHalfTime);

    // Get position again, should be the same as first position
    iPositioner.NotifyPositionUpdate(posInfo4, myStatus);
   
    User::WaitForRequest(myStatus);
    err = myStatus.Int();
    AssertTrueL(err == KErrNone, KNotifyPositionUpdate, err);

    if (!ComparePositionL(posInfo3, posInfo4))
        {
        _LIT(KErrorAndLeave, "The same position was returned from positioner!!!");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    // Wait MAX_AGE_TIME
	const TTimeIntervalMicroSeconds32 KTime(MAX_AGE_TIME); //10 sec
    User::After(KTime);

    // Get position again, should NOT be the same as earlier
    iPositioner.NotifyPositionUpdate(posInfo5, myStatus);
   
    User::WaitForRequest(myStatus);
    err = myStatus.Int();
    AssertTrueL(err == KErrNone, KNotifyPositionUpdate, err);

    if (ComparePositionL(posInfo4, posInfo5))
        {
        _LIT(KErrorAndLeave, "The same position was returned from positioner!!!");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    //Try to set an interval that is less than MaxAge
    TTimeIntervalMicroSeconds mySmallInterval = TTimeIntervalMicroSeconds(400000);
    posOption.SetUpdateInterval(mySmallInterval);
    posOption.SetMaxUpdateAge(maxAge);

    err = iPositioner.SetUpdateOptions(posOption);
    _LIT(KErr2, "Setting updateinterval less than MaxAge should result in KErrArgument, instead error %d");
	AssertTrueL(err == KErrArgument, KErr2, err);

    // Check that the MaxAge did not change
    iPositioner.GetUpdateOptions(returnPosOption);
    TTimeIntervalMicroSeconds interval = returnPosOption.MaxUpdateAge();

    if (interval != maxAge)
        {
        _LIT(KErrorAndLeave, "MaxAge does not match %d with %d");
        buf.Format(KErrorAndLeave, interval.Int64(), maxAge.Int64());
        LogErrorAndLeaveL(buf);
        }


    ClosePositioner();
    Disconnect();
    
    ExtendedTesting1();

    ExtendedTesting2();
    }

#endif

// ---------------------------------------------------------
// CT_LbsClientPosTp217::ExtendedTesting1
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp217::ExtendedTesting1()
    {
    _LIT(KFunctionName, "We are inside ExtendedTesting1()");
    INFO_PRINTF1(KFunctionName);
    TTimeIntervalMicroSeconds maxAge = TTimeIntervalMicroSeconds(5000000);
    TTimeIntervalMicroSeconds myInterval = TTimeIntervalMicroSeconds(400000);

    // Set iUidTestPsyMaxAge to defaultpsy
    SetupPsyL(iUidTestPsyMaxAge);

    //connect to EPos
    User::LeaveIfError(iPosServer.Connect());
    TInt err = KErrNone;

    // Open default psy
    User::LeaveIfError(iPositioner.Open(iPosServer));

    TPositionUpdateOptions posOption;

    //Try to set an interval that is less than MaxAge
    posOption.SetUpdateInterval(myInterval);
    posOption.SetMaxUpdateAge(maxAge);

    err = iPositioner.SetUpdateOptions(posOption);
    _LIT(KErr2, "Setting updateinterval less than MaxAge should result in KErrArgument, instead error %d");
	AssertTrueL(err == KErrArgument, KErr2, err);

    // Check that the MaxAge did not change
    TPositionUpdateOptions returnPosOption;
    iPositioner.GetUpdateOptions(returnPosOption);

    //Try to set an interval that is equal to MaxAge
    posOption.SetUpdateInterval(maxAge);
    posOption.SetMaxUpdateAge(maxAge);

    err = iPositioner.SetUpdateOptions(posOption);
    _LIT(KErr3, "Setting updateinterval equal to MaxAge should result in KErrArgument, instead error %d");
	AssertTrueL(err == KErrArgument, KErr3, err);

    ClosePositioner();
    Disconnect();
    _LIT(KFunctionDone, "ExtendedTesting1 Done");
    INFO_PRINTF1(KFunctionDone);
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp217::ExtendedTesting2
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp217::ExtendedTesting2()
    {
    _LIT(KFunctionName, "We are inside ExtendedTesting2()");
    INFO_PRINTF1(KFunctionName);
    TTimeIntervalMicroSeconds maxAge = TTimeIntervalMicroSeconds(30000000); // 30 sec
    TTimeIntervalMicroSeconds maxAge2 = TTimeIntervalMicroSeconds(4000000); // 4 sec

    // The MaxAge psy adds one second to the timeout time, therefore 6 seconds here (just to be sure)
    const TTimeIntervalMicroSeconds32 KSomeTime(6000000); //6 sec

    //connect to EPos
    User::LeaveIfError(iPosServer.Connect());
    TInt err = KErrNone;
    //Open subsession
    User::LeaveIfError(iPositioner.Open(iPosServer, iUidTestPsyMaxAge));

    SetupPsyL(iUidTestPsyMaxAge);

    TPositionInfo posInfo1 = TPositionInfo();
    TPositionInfo posInfo2 = TPositionInfo();
    TPositionInfo posInfo3 = TPositionInfo();
    TPositionInfo posInfo4 = TPositionInfo();

    TRequestStatus          myStatus;

    iPositioner.SetRequestor(CRequestor::ERequestorService,
                             CRequestor::EFormatApplication, KKallesService);

    // Make sure that it is possible to get position from psy
    iPositioner.NotifyPositionUpdate(posInfo1, myStatus);
    User::WaitForRequest(myStatus);
    err = myStatus.Int();
    _LIT(KNotifyPositionUpdate, "Error from NotifyPositionUpdate");
    AssertTrueL(err == KErrNone, KNotifyPositionUpdate, err);

    // Add a short delay here so that the MaxAge Psy has the possibilty 
    // to create a new random position (the randomization seed is created with a time)
    User::After(2000000);

    // Make sure that the psy returns differens positions
    iPositioner.NotifyPositionUpdate(posInfo2, myStatus);
    User::WaitForRequest(myStatus);
    err = myStatus.Int();
    AssertTrueL(err == KErrNone, KNotifyPositionUpdate, err);

    if (ComparePositionL(posInfo1, posInfo2))
        {
        _LIT(KErrorAndLeave, "The same position was returned from positioner!!!");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    // Set MaxAge
    TPositionUpdateOptions posOption;
	posOption.SetMaxUpdateAge(maxAge);
	err = iPositioner.SetUpdateOptions(posOption);

	_LIT(KChange, "ERROR: When setting max age in SetMaxAge, error %d");
	AssertTrueL(err == KErrNone, KChange, err);
    User::After(KSomeTime);

    iPositioner.NotifyPositionUpdate(posInfo3, myStatus);
    User::WaitForRequest(myStatus);
    err = myStatus.Int();
    AssertTrueL(err == KErrNone, KNotifyPositionUpdate, err);

    // This position should be the old one
    if (!ComparePositionL(posInfo2, posInfo3))
        {
        _LIT(KErrorAndLeave, "The same position was returned from positioner!!!");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    // Now change MaxAge again and check that it has been changed
	posOption.SetMaxUpdateAge(maxAge2);
	err = iPositioner.SetUpdateOptions(posOption);

	_LIT(KChange2, "ERROR: When setting max age in SetMaxAge (2), error %d");
	AssertTrueL(err == KErrNone, KChange2, err);
	_LIT(KTimeout, "Waiting for the new maxage-timeout");
    INFO_PRINTF1(KTimeout);
    User::After(KSomeTime);

    iPositioner.NotifyPositionUpdate(posInfo4, myStatus);
    User::WaitForRequest(myStatus);
    err = myStatus.Int();
    AssertTrueL(err == KErrNone, KNotifyPositionUpdate, err);

    // This position should be new since maxage has expired
    if (ComparePositionL(posInfo3, posInfo4))
        {
        _LIT(KErrorAndLeave, "The same position was returned from positioner!!!");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    ClosePositioner();
    Disconnect();
    _LIT(KFunctionDone, "ExtendedTesting2() Done");
    INFO_PRINTF1(KFunctionDone);
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp217::ComparePositionL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CT_LbsClientPosTp217::ComparePositionL(TPositionInfo aPosInfo1, TPositionInfo aPosInfo2)
    {
    _LIT(KFunctionName, "We are inside ComparePositionL");
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
        _LIT(KPosition2, "Position1: Lat %f Long %f Alt %f");
        buf.Format( KPosition2, pos2.Latitude(), pos2.Longitude() ,pos2.Altitude());
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
// CT_LbsClientPosTp217::AssertTrueSecL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp217::AssertTrueL(TBool aCondition, const TDesC& aErrorMsg, TInt aErrorCode)
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


