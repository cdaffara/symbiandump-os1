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



// INCLUDE FILES
#include "ctlbsclientpostp254.h"
#include <f32file.h>
#include <bautils.h>
#include <lbssatellite.h>

// CONSTANTS


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp254::CT_LbsClientPosTp254(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{  
	_LIT(KTestName, "TP254 - Get last known position");
	SetTestStepName(KTestName); 
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp254::~CT_LbsClientPosTp254()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp254::CloseTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp254::CloseTest()
    {
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp254::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp254::StartL()
    {
    User::After(2000000);

	SetupPsyL(iUidTestPsy11);

    RPositionServer	posServer;
    CleanupClosePushL(posServer);
    RPositioner positioner;
    CleanupClosePushL(positioner);
    TPositionInfo positionInfo = TPositionInfo();
    TPositionInfo positionInfoLast = TPositionInfo();

	_LIT(KConnectErr, "Error when connecing to EPos server,  %d");
	TInt err = posServer.Connect();
	AssertTrueL(err == KErrNone, KConnectErr, err);

    TRequestStatus clearDbReqStatus;
    posServer.EmptyLastKnownPositionStore(clearDbReqStatus);
    User::WaitForRequest(clearDbReqStatus);

    _LIT(KClearoutErr, "Error when clearing out last known position via api on session, error code returned was %d");
    AssertTrueL(clearDbReqStatus.Int() == KErrNone, KClearoutErr, clearDbReqStatus.Int());
	
	_LIT(KOpenErr, "Error when opening positioner,  %d");
    err = positioner.Open(posServer,iUidTestPsy11 ); //use defalut psy
	AssertTrueL(err == KErrNone, KOpenErr, err);


    _LIT(KService ,"Service");
	positioner.SetRequestor(CRequestor::ERequestorService,
							 CRequestor::EFormatApplication, KService);

    TRequestStatus status;
    status = KRequestPending;

    positionInfoLast.SetUpdateType(EPositionUpdateUnknown); // just to make a difference
    
    positioner.GetLastKnownPosition(positionInfoLast, status);
    User::WaitForRequest(status);
    err = status.Int();
    _LIT(KErrorRequest, "Get last known position did not return with KErrUnKnown %d");
    AssertTrueL(err == KErrUnknown, KErrorRequest, err);

    //Verification of TR SBUY-5WHGS3 i.e even if the psy has a really fast
    //responce time a last known position should be saved.
    status = KRequestPending;

    positioner.NotifyPositionUpdate(positionInfo, status);
    User::WaitForRequest(status);
    err = status.Int();
    _LIT(KErrPosition, "Not possible to make a request");
    AssertTrueL(err == KErrNone, KErrPosition, err);

    positioner.GetLastKnownPosition(positionInfoLast, status);
    User::WaitForRequest(status);
    err = status.Int();
    _LIT(KErrorFast, "Get last known position after fast psy did not work %d");
    AssertTrueL(err == KErrNone, KErrorFast, err);

    positioner.Close();

    clearDbReqStatus = KRequestPending;
    posServer.EmptyLastKnownPositionStore(clearDbReqStatus);
    User::WaitForRequest(clearDbReqStatus);
    
    posServer.Close();

    const TTimeIntervalMicroSeconds32 KPosWaitServerShutDown( 6000000 );
    User::After(KPosWaitServerShutDown);
    
    SetupPsyL(iUidTestPsyMaxAge);
	err = posServer.Connect();
	AssertTrueL(err == KErrNone, KConnectErr, err);
    err = positioner.Open(posServer); //use defalut psy
	AssertTrueL(err == KErrNone, KOpenErr, err);
	positioner.SetRequestor(CRequestor::ERequestorService,
							 CRequestor::EFormatApplication, KService);

    
    status = KRequestPending;

    positioner.NotifyPositionUpdate(positionInfo, status);
    User::WaitForRequest(status);
    err = status.Int();
    AssertTrueL(err == KErrNone, KErrPosition, err);

    //Now there should exist a last known position
    positioner.GetLastKnownPosition(positionInfoLast, status);
    User::WaitForRequest(status);
    err = status.Int();
    _LIT(KErrorRequestLast, "Get last known position did not return with KErrNone %d");
    AssertTrueL(err == KErrNone, KErrorRequestLast, err);

    CheckPositionInfoL(positionInfo, positionInfoLast);

	TPositionSatelliteInfo satelliteInfoLast = TPositionSatelliteInfo();
    positioner.GetLastKnownPosition(satelliteInfoLast, status);
    User::WaitForRequest(status);
    err = status.Int();
    
    AssertTrueL(err == KErrArgument, KErrorRequest, err);

    positioner.NotifyPositionUpdate(positionInfo, status);
    User::WaitForRequest(status);
    err = status.Int();
    AssertTrueL(err == KErrNone, KErrPosition, err);

    positioner.GetLastKnownPosition(positionInfoLast, status);
    User::WaitForRequest(status);
    err = status.Int();
    AssertTrueL(err == KErrNone, KErrorRequest, err);

    CheckPositionInfoL(positionInfo, positionInfoLast);

    positioner.Close();
    posServer.Close();

    //connect again
	err = posServer.Connect();
	AssertTrueL(err == KErrNone, KConnectErr, err);
    err = positioner.Open(posServer); //use defalut psy
	AssertTrueL(err == KErrNone, KOpenErr, err);
	positioner.SetRequestor(CRequestor::ERequestorService,
							 CRequestor::EFormatApplication, KService);


    TPositionInfo positionInfoLast1 = TPositionInfo();
    positioner.GetLastKnownPosition(positionInfoLast1, status);
    User::WaitForRequest(status);
    err = status.Int();
    AssertTrueL(err == KErrNone, KErrorRequest, err);    
    CheckPositionInfoL(positionInfoLast, positionInfoLast1);

    positioner.Close();
    posServer.Close();
	User::After(6000000);

	// Old DB file will never be present

    //Connect again
	err = posServer.Connect();
	AssertTrueL(err == KErrNone, KConnectErr, err);
    err = positioner.Open(posServer); //use defalut psy
	AssertTrueL(err == KErrNone, KOpenErr, err);
	positioner.SetRequestor(CRequestor::ERequestorService,
							 CRequestor::EFormatApplication, KService);


    TPositionInfo positionInfoLast2 = TPositionInfo();
    positioner.GetLastKnownPosition(positionInfoLast2, status);
    User::WaitForRequest(status);
    err = status.Int();
    AssertTrueL(err == KErrNone, KErrorRequest, err);    
    CheckPositionInfoL(positionInfoLast, positionInfoLast2);

    RPositionServer	posServer1;
    CleanupClosePushL(posServer1);
    RPositioner positioner1;
    CleanupClosePushL(positioner1);
    SetupPsyL(iUidMultiPsy);
	_LIT(KConnect1Err, "Error when connecing to EPos server,  %d");
	err = posServer1.Connect();
	AssertTrueL(err == KErrNone, KConnect1Err, err);

	_LIT(KOpen1Err, "Error when opening positioner,  %d");
    err = positioner1.Open(posServer); //use defalut psy
	AssertTrueL(err == KErrNone, KOpen1Err, err);
	positioner1.SetRequestor(CRequestor::ERequestorService,
							 CRequestor::EFormatApplication, KService);

    positioner1.NotifyPositionUpdate(positionInfo, status);
    User::WaitForRequest(status);
    err = status.Int();
    AssertTrueL(err == KErrNone, KErrPosition, err);

    positioner.GetLastKnownPosition(positionInfoLast, status);
    User::WaitForRequest(status);
    err = status.Int();
    AssertTrueL(err == KErrNone, KErrorRequest, err);
    CheckPositionInfoL(positionInfo, positionInfoLast);

    //just to make sure that positioner1 also gets the correct position
    positioner1.GetLastKnownPosition(positionInfoLast, status);
    User::WaitForRequest(status);
    err = status.Int();
    AssertTrueL(err == KErrNone, KErrorRequest, err);
    CheckPositionInfoL(positionInfo, positionInfoLast);

    SetupPsyL(iUidTestPsyPartialUpdate);
    User::After(1000000);
    TPositionInfo positionInfoPartial = TPositionInfo();
    TPositionUpdateOptions updateOption = TPositionUpdateOptions();
    updateOption.SetAcceptPartialUpdates(ETrue);
    positioner1.SetUpdateOptions(updateOption);

    positioner1.NotifyPositionUpdate(positionInfoPartial, status);
    User::WaitForRequest(status);
    err = status.Int();
    _LIT(KErrPositionPartial, "no partial position returned %d");
    AssertTrueL(err == KPositionPartialUpdate, KErrPositionPartial, err);

    positioner.GetLastKnownPosition(positionInfoLast, status);
    User::WaitForRequest(status);
    err = status.Int();
    AssertTrueL(err == KErrNone, KErrorRequest, err);
    
    TPosition pos;
    TPosition lastPos;
	positionInfoPartial.GetPosition(pos);
    positionInfoLast.GetPosition(lastPos);
    
    if (pos.Time() == lastPos.Time())
		{
		_LIT(KErrPosition, "Partial position was cached");
		LogErrorAndLeaveL(KErrPosition);
		}

    CleanupStack::PopAndDestroy(&positioner1);
    CleanupStack::PopAndDestroy(&posServer1);
    CleanupStack::PopAndDestroy(&positioner);
    CleanupStack::PopAndDestroy(&posServer);

    }

// ---------------------------------------------------------
// CT_LbsClientPosTp254::DeleteFileL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp254::DeleteFileL(const TDesC& aFile)
    {
    RFs fs;
    TInt err = fs.Connect();

    err = fs.Delete(aFile);
    fs.Close();
    if (err == KErrNone)
        {
        _LIT(KDebugText, "TP254: The specified file deleted");
        INFO_PRINTF1(KDebugText);
        }
    else
        {
        if (err == KErrInUse)
            {
            _LIT(KErrText, "TP254: The specified file could not be deleted it is already in use, errorcode %d");
            INFO_PRINTF1(aFile);
            TBuf<100> buf;
            buf.Format(KErrText, err);
            AssertTrueL(err==0, KErrText, err);
            }
        else if (err == KErrNotFound)
            {
            _LIT(KDebugText, "TP254: The specified file could not be found ,errorcode %d");
            TBuf<100> buf;
            buf.Format(KDebugText, err);
            INFO_PRINTF1(buf);
            }
        else if (err == KErrPathNotFound)
            {
            _LIT(KDebugText, "TP254: The path could not be found ,errorcode %d");
            TBuf<100> buf;
            buf.Format(KDebugText, err);
            INFO_PRINTF1(buf);
            }
        else
            {
            _LIT(KErrText, "TP254: The specified file could not be deleted, errorcode %d");
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
TBool CT_LbsClientPosTp254::FileExistsL(const TDesC& aFile)
    {
    RFs fs;
    TInt err = fs.Connect();
    TBool exists = EFalse;
    
    if(KErrNone==err)
    	{
    	exists = BaflUtils::FileExists(fs, aFile);
    	if (exists)
    		{
    		_LIT(KErrText, "TP254: The specified file does not exist");
    		INFO_PRINTF1(KErrText);
    		}
    	else
    		{
    		_LIT(KErrText, "TP254: The specified file exists");
    		INFO_PRINTF1(KErrText);
    		}
    	}
    
    fs.Close();
    return exists;
    }

void CT_LbsClientPosTp254::CheckPositionInfoL(TPositionInfo& aInfo, TPositionInfo& aLastInfo)
	{
    if (aInfo.UpdateType() != aLastInfo.UpdateType())
        {
        _LIT(KErrUpdateType, "Update type not stored correctly");
        LogErrorAndLeaveL(KErrUpdateType);
        }
    if (aLastInfo.ModuleId().iUid != 0)
		{
		_LIT(KIncorrectPsyError,
           "Uid not reseted");
		LogErrorAndLeaveL(KIncorrectPsyError);
		}
	TPosition pos;
    TPosition lastPos;
	aInfo.GetPosition(pos);
    aLastInfo.GetPosition(lastPos);
	CheckPositionL(pos, lastPos);
	}

void CT_LbsClientPosTp254::CheckPositionL(TPosition& aPos, TPosition& aLastPos)
	{
	if (aPos.Time() != aLastPos.Time() ||
		aPos.HorizontalAccuracy() != aLastPos.HorizontalAccuracy() ||
		aPos.VerticalAccuracy() != aLastPos.VerticalAccuracy() ||
		aPos.Latitude() != aLastPos.Latitude() ||
		aPos.Longitude() != aLastPos.Longitude() ||
		aPos.Altitude() != aLastPos.Altitude())
		{
		_LIT(KErrPosition, "Wrong position returned");
		LogErrorAndLeaveL(KErrPosition);
		}
	}

// End of File
