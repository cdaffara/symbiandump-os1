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
#include "ctlbsclientpostp257.h"

// CONSTANTS
_LIT(KLastPosition, "c:\\private\\101f97b2\\LastKnownPos.dat");

// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp257::CT_LbsClientPosTp257(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{  
	_LIT(KTestName, "Tp257-GetLastKnown Position, update options");
	SetTestStepName(KTestName); 
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp257::~CT_LbsClientPosTp257()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp257::CloseTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp257::CloseTest()
    {
    // Disconnect disconnects from "iPosServer"
    Disconnect();
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp257::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp257::StartL()
    {

    // make sure Server is closed
    User::After(2100000);
    
    // Remove getlastknown data file
    DeleteFileL(KLastPosition);

    SetupPsyL(iUidMultiPsy);
    ConnectL();

    TInt err = KErrNone;

    RPositioner positioner1;
    RPositioner positioner2;
    TRequestStatus status1;
    TRequestStatus status2;
    TPositionInfo moduleInfo1;
    TPositionInfo moduleInfo2;
    TPositionInfo moduleInfo3;
    TPositionInfo moduleInfoMaxAge1;
    TPositionInfo moduleInfoMaxAge2;
    
    // 1) Connect to server and open positioner
    err = positioner1.Open(iPosServer);
    CleanupClosePushL(positioner1);
    if (err != KErrNone)
    	{
    	_LIT(KErrorAndLeave, "Could not open positioner");
    	LogErrorAndLeaveL(KErrorAndLeave);
    	}
        
    _LIT(KKalleService, "KallesService");
    err = positioner1.SetRequestor(CRequestor::ERequestorService,
                           CRequestor::EFormatApplication, KKalleService);
    if (err != KErrNone)
    	{
    	_LIT(KErrorAndLeave, "Could not set requestor");
    	LogErrorAndLeaveL(KErrorAndLeave);
    	}

    // 2) Request a position
    positioner1.NotifyPositionUpdate(moduleInfo1, status1);
    User::WaitForRequest(status1);
    if (status1.Int() != KErrNone)
    	{
    	_LIT(KErrorAndLeave, "Wrong status1 returned: ");
    	LogErrorAndLeaveL(KErrorAndLeave, status1.Int());
    	}

    // Not included in TC, just test that it is possible to get a lastknownposition
    positioner1.GetLastKnownPosition(moduleInfo2, status1);
    User::WaitForRequest(status1);
    if (status1.Int() != KErrNone)
    	{
    	_LIT(KErrorAndLeave, "Wrong status3 returned: ");
    	LogErrorAndLeaveL(KErrorAndLeave, status1.Int());
    	}

    if (!ComparePositionL(moduleInfo1, moduleInfo2))
        {
        _LIT(KErrorAndLeave, "1. The old position1 was not returned!!!");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    // 3) Set update interval to 5 seconds, if there are no ongoing 
    // position requests a GetLastKnown request is OK
    TPositionUpdateOptions posOption;
	posOption.SetUpdateInterval(TTimeIntervalMicroSeconds(5000000));

    err = positioner1.SetUpdateOptions(posOption);
	_LIT(KChange, "Error when setting update interval, error %d");
	AssertTrueL(err == KErrNone, KChange, err);

    positioner1.GetLastKnownPosition(moduleInfo2, status1);
    User::WaitForRequest(status1);

    if (status1.Int() != KErrNone)
    	{
    	_LIT(KErrorAndLeave, "KErrNone should be returned from a tracking session");
    	LogErrorAndLeaveL(KErrorAndLeave, status1.Int());
    	}

    // Reset interval
    posOption.SetUpdateInterval(TTimeIntervalMicroSeconds(0));
    err = positioner1.SetUpdateOptions(posOption);

    // Get last known position
    positioner1.GetLastKnownPosition(moduleInfo3, status1);
    User::WaitForRequest(status1);
    if (status1.Int() != KErrNone)
    	{
    	_LIT(KErrorAndLeave, "Wrong status1 returned 2: ");
    	LogErrorAndLeaveL(KErrorAndLeave, status1.Int());
    	}

    if (!ComparePositionL(moduleInfo1, moduleInfo3))
        {
        _LIT(KErrorAndLeave, "2. The old position2 was not returned!!!");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    // 5 ) Set maxage to five seconds
    TPositionUpdateOptions posOption2;
    posOption2.SetMaxUpdateAge(TTimeIntervalMicroSeconds(5000000));
	err = positioner1.SetUpdateOptions(posOption2);
	_LIT(KChange1, "Error when setting max age in SetMaxUpdateAge, error %d");
	AssertTrueL(err == KErrNone, KChange1, err);

    //Wait for five seconds
    User::After(5000000);

    // 6) Get last known position
    positioner1.GetLastKnownPosition(moduleInfo3, status1);
    User::WaitForRequest(status1);
    if (status1.Int() != KErrNone)
    	{
    	_LIT(KErrorAndLeave, "Wrong status1 returned 3");
    	LogErrorAndLeaveL(KErrorAndLeave);
    	}

    if (!ComparePositionL(moduleInfo1, moduleInfo3))
        {
        _LIT(KErrorAndLeave, "3. The old position was not returned from positioner!!!");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    // 7) Open the maxage psy
    SetupPsyL(iUidTestPsyMaxAge);
    err = positioner2.Open(iPosServer, iUidTestPsyMaxAge);
    CleanupClosePushL(positioner2);
    _LIT(KKalleService2, "KallesService2");
    err = positioner2.SetRequestor(CRequestor::ERequestorService,
                           CRequestor::EFormatApplication, KKalleService2);

    _LIT(KErrOepnMaxAge, "Could not open the maxage psy");
	AssertTrueL(err == KErrNone, KErrOepnMaxAge, err);

    positioner2.NotifyPositionUpdate(moduleInfoMaxAge1, status2);
    User::WaitForRequest(status2);
    err = status2.Int();
    if (err != KErrNone)
    	{
    	_LIT(KErrorAndLeave, "Wrong status2 returned");
    	LogErrorAndLeaveL(KErrorAndLeave);
    	}

    // 8)
    positioner1.NotifyPositionUpdate(moduleInfo1, status1);
    User::WaitForRequest(status1);
    err = status1.Int();

    // 9) Set maxage to four seconds for positioner2
    posOption2.SetMaxUpdateAge(TTimeIntervalMicroSeconds(4000000));
	err = positioner2.SetUpdateOptions(posOption2);
	_LIT(KErrSetMaxAge, "Error when setting max age in SetMaxUpdateAge, error %d");
	AssertTrueL(err == KErrNone, KErrSetMaxAge, err);

    positioner2.NotifyPositionUpdate(moduleInfoMaxAge2, status2);
    User::WaitForRequest(status2);
    err = status2.Int();
    if (err != KErrNone) 
    	{
    	_LIT(KErrorAndLeave, "Wrong status2 returned");
    	LogErrorAndLeaveL(KErrorAndLeave);
    	}
    // Check moduleInfoMaxAge2 and moduleInfoMaxAge1 here, should be the same
    _LIT(KCheckMaxAge, "Check MaxAge position");
    INFO_PRINTF1(KCheckMaxAge);
    if (!ComparePositionL(moduleInfoMaxAge1, moduleInfoMaxAge2))
        {
        _LIT(KErrorAndLeave, "The maxage position was not returned from maxage positioner!!!");
        LogErrorAndLeaveL(KErrorAndLeave);
        }
    // Close maxage positioner
    positioner2.Close();

    // 10) Check moduleInfo2 and moduleInfo1 here, should be the same
    positioner1.GetLastKnownPosition(moduleInfo2, status1);
    User::WaitForRequest(status1);
    if (!ComparePositionL(moduleInfo2, moduleInfo1))
        {
        _LIT(KErrorAndLeave, "The same maxage position was not returned from positioner for client 1!!!");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    // 11)
    TPositionUpdateOptions posOption3;
    posOption3.SetUpdateTimeOut(TTimeIntervalMicroSeconds(4000000));
	err = positioner1.SetUpdateOptions(posOption3);
	_LIT(KErrSetTimeout, "Error when setting timeout in SetUpdateOptions, error %d");
	AssertTrueL(err == KErrNone, KErrSetTimeout, err);

	_LIT(KWaitForCancel, "Wait for cancel...");
    INFO_PRINTF1(KWaitForCancel);
    
    // 12)
    positioner1.GetLastKnownPosition(moduleInfo2, status1);

    // Wait for five seconds
    User::After(5000000);
    _LIT(KWaitForRequest, "Wait for WaitForRequest(status1)...");
    INFO_PRINTF1(KWaitForRequest);   
    User::WaitForRequest(status1);
    // Check moduleInfo2 and moduleInfo1 here, should be the same
    if (!ComparePositionL(moduleInfo2, moduleInfo1))
        {
        _LIT(KErrorAndLeave, "4. The same position was not returned from positioner for client 1!!!");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    // 13 ) Check with partial update
    SetupPsyL(iUidTestPsyPartialUpdate);
    err = positioner2.Open(iPosServer, iUidTestPsyPartialUpdate);
    if (err != KErrNone)
    	{
    	_LIT(KErrorAndLeave, "Could not open PartialUpdate positioner");
    	LogErrorAndLeaveL(KErrorAndLeave);
    	}

    _LIT(KPellesService, "PellesService");
    err = positioner2.SetRequestor(CRequestor::ERequestorService,
                           CRequestor::EFormatApplication, KPellesService);

    // Get a position from partial update psy
    positioner2.NotifyPositionUpdate(moduleInfo1, status2);
    User::WaitForRequest(status2);
    err = status2.Int();
    if (err != KErrNone)
    	{
    	_LIT(KErrorAndLeave, "Wrong status returned from partial update psy");
    	LogErrorAndLeaveL(KErrorAndLeave, err);
    	}

    TPositionUpdateOptions updateOptions;
    updateOptions.SetAcceptPartialUpdates(ETrue);
    err = positioner2.SetUpdateOptions(updateOptions);
    _LIT(KErrSetPartialupdate, "Error when setting partialupdate in SetUpdateOptions, error %d");
    AssertTrueL(err == KErrNone, KErrSetPartialupdate, err);

    // 14) GetLastKnownPosition from partial update psy
    positioner2.GetLastKnownPosition(moduleInfo2, status1);
    User::WaitForRequest(status1);
    // Check moduleInfo2 and moduleInfo1 here, should be the same
    if (!ComparePositionL(moduleInfo2, moduleInfo1))
        {
        _LIT(KErrorAndLeave, "5. The same partial update position was not returned from positioner!!!");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    // 15) Get another position from partial update psy, this should be a partial update position
    // and should not be saved as lastknownposition
    positioner2.NotifyPositionUpdate(moduleInfo1, status2);
    User::WaitForRequest(status2);
    err = status2.Int();
    if (err != KPositionPartialUpdate)
    	{
    	_LIT(KErrorAndLeave, "Partial Update PSY did not return KPositionPartialUpdate");
    	LogErrorAndLeaveL(KErrorAndLeave, err);
    	}

    // 16) GetLastKnownPosition from partial update psy
    positioner2.GetLastKnownPosition(moduleInfo3, status1);
    User::WaitForRequest(status1);
    // Check moduleInfo2 and moduleInfo1 here, should NOT be the same
    if (ComparePositionL(moduleInfo1, moduleInfo2))
        {
        _LIT(KErrorAndLeave, "6. The partial update position should NOT have been returned");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    // Check moduleInfo3 and moduleInfo2 here, should be the same
    if (ComparePositionL(moduleInfo3, moduleInfo1))
        {
        _LIT(KErrorAndLeave, "7. The old position was not returned");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    // Close all connections
    positioner1.Close();
    positioner2.Close();
    
    // 17 ) Start a periodic update session 
    _LIT(KPeriodicUpdate, "Checking periodic update session");
    INFO_PRINTF1(KPeriodicUpdate);
	posOption.SetUpdateInterval(TTimeIntervalMicroSeconds(5000000));

    // Connect to server and open positioner
    err = positioner1.Open(iPosServer);
    if (err != KErrNone)
    	{
    	_LIT(KErrorAndLeave, "Could not open positioner");
    	LogErrorAndLeaveL(KErrorAndLeave);
    	}
        
    _LIT(KKallesPeriodicService, "KallesPeriodicService");
    err = positioner1.SetRequestor(CRequestor::ERequestorService,
                           CRequestor::EFormatApplication, KKallesPeriodicService);
    if (err != KErrNone)
    	{
    	_LIT(KErrorAndLeave, "Could not set requestor");
    	LogErrorAndLeaveL(KErrorAndLeave);
    	}

    err = positioner1.SetUpdateOptions(posOption);
    _LIT(KErrSetUpdateOption, "Could not set updateoptions");
	AssertTrueL(err == KErrNone, KErrSetUpdateOption, err);
    
    TRequestStatus status;
    positioner1.NotifyPositionUpdate(moduleInfo1, status);
    User::WaitForRequest(status);

    posOption.SetUpdateInterval(TTimeIntervalMicroSeconds(0));
    // Settings update interval to 0 should stop the periodic update
    err = positioner1.SetUpdateOptions(posOption);
    _LIT(KErrSetUpdateOption2, "Could not set updateoptions2");
	AssertTrueL(err == KErrNone, KErrSetUpdateOption2, err);

    // Get GetLastKnownPosition from the stopped periodic update session
    positioner1.GetLastKnownPosition(moduleInfo2, status);
    User::WaitForRequest(status);
    // Check moduleInfo2 and moduleInfo1 here, should be the same
    if (!ComparePositionL(moduleInfo1, moduleInfo2))
        {
        _LIT(KErrorAndLeave, "1. The periodic update session position was not equal to getlastknown position");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    positioner1.Close();
    moduleInfo1 = TPositionInfo();
    moduleInfo2 = TPositionInfo();

    // 18 ) Start a periodic update session
    INFO_PRINTF1(KPeriodicUpdate);
	posOption.SetUpdateInterval(TTimeIntervalMicroSeconds(5000000));

    // Connect to server and open positioner
    err = positioner1.Open(iPosServer);
    if (err != KErrNone)
    	{
    	_LIT(KErrorAndLeave, "Could not open positioner");
    	LogErrorAndLeaveL(KErrorAndLeave);
    	}
        
    err = positioner1.SetRequestor(CRequestor::ERequestorService,
                           CRequestor::EFormatApplication, KKallesPeriodicService);
    if (err != KErrNone)
    	{
    	_LIT(KErrorAndLeave, "Could not set requestor");
    	LogErrorAndLeaveL(KErrorAndLeave);
    	}

    // Connect to server and open positioner
    err = positioner2.Open(iPosServer);
    if (err != KErrNone)
    	{
    	_LIT(KErrorAndLeave, "Could not open positioner2");
    	LogErrorAndLeaveL(KErrorAndLeave);
    	}
        
    err = positioner2.SetRequestor(CRequestor::ERequestorService,
                           CRequestor::EFormatApplication, KKallesPeriodicService);
    if (err != KErrNone)
    	{
    	_LIT(KErrorAndLeave, "Could not set requestor");
    	LogErrorAndLeaveL(KErrorAndLeave);
    	}


    err = positioner1.SetUpdateOptions(posOption);
	AssertTrueL(err == KErrNone, KErrSetUpdateOption, err);

    positioner1.NotifyPositionUpdate(moduleInfo1, status);
    User::WaitForRequest(status);
    
    posOption.SetUpdateInterval(TTimeIntervalMicroSeconds(0));
    // Settings update interval to 0 should stop the periodic update
    err = positioner1.SetUpdateOptions(posOption);
	AssertTrueL(err == KErrNone, KErrSetUpdateOption2, err);

    positioner2.GetLastKnownPosition(moduleInfo2, status1);
    User::WaitForRequest(status1);

    // Check moduleInfo2 and moduleInfo1 here, should be the same
    if (!ComparePositionL(moduleInfo1, moduleInfo2))
        {
        _LIT(KErrorAndLeave, "2. The periodic update session position was not equal to getlastknown position");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    positioner1.Close();
    positioner2.Close();

    CleanupStack::PopAndDestroy(1); //positioner2
    CleanupStack::PopAndDestroy(1); //positioner1
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp257::ComparePositionL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CT_LbsClientPosTp257::ComparePositionL(TPositionInfo aPosInfo1, TPositionInfo aPosInfo2)
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
        _LIT(KTime, "Equal Time");
        INFO_PRINTF1(KTime);
        }
    else 
    	{
    	_LIT(KTime, "NOT Equal Time");
    	INFO_PRINTF1(KTime);
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
        buf.Format(KPosition1, pos1.Latitude(), pos1.Longitude() ,pos1.Altitude());
        INFO_PRINTF1(buf);
        }
    return result;
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp257::DeleteFileL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp257::DeleteFileL(const TDesC& aFile)
    {
    RFs fs;
    TInt err = fs.Connect();

    err = fs.Delete(aFile);
    fs.Close();
    if (err == KErrNone)
        {
        _LIT(KDebugText, "TP257: The specified file deleted");
        INFO_PRINTF1(KDebugText);
        }
    else
        {
        if (err == KErrInUse)
            {
            _LIT(KErrText, "TP257: The specified file could not be deleted it is already in use, errorcode %d");
            INFO_PRINTF1(aFile);
            TBuf<100> buf;
            buf.Format(KErrText, err);
            AssertTrueL(err==0, KErrText, err);
            }
        else if (err == KErrNotFound)
            {
            _LIT(KDebugText, "TP257: The specified file could not be found ,errorcode %d");
            TBuf<100> buf;
            buf.Format(KDebugText, err);
            INFO_PRINTF1(buf);
            }
        else if (err == KErrPathNotFound)
            {
            _LIT(KDebugText, "TP257: The path could not be found ,errorcode %d");
            TBuf<100> buf;
            buf.Format(KDebugText, err);
            INFO_PRINTF1(buf);
            }
        else
            {
            _LIT(KErrText, "TP257: The specified file could not be deleted, errorcode %d");
            TBuf<100> buf;
            buf.Format(KErrText, err);
            AssertTrueL(err==0, KErrText, err);
            }
        }
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp254::FileExistsL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CT_LbsClientPosTp257::FileExistsL(const TDesC& aFile)
    {
    TBool exist = EFalse;
    RFs fs;
    RFile file;
    TInt err = fs.Connect();

    err = file.Open(fs, aFile, EFileRead);
    if (err==KErrNotFound) // file does not exist
        {
        fs.Close();
        _LIT(KErrText, "TP257: The specified file does not exist");
        INFO_PRINTF1(KErrText);
        }
    else
        {
        exist = ETrue;
        _LIT(KErrText, "TP257: The specified file exists");
        INFO_PRINTF1(KErrText);
        file.Close();
        fs.Close();
        }
    return exist;
    }
