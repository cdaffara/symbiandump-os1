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
#include "ctlbsclientpostp189.h"
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>

// CONSTANTS
const TInt32 KUidNonExistingPsy = 0x01111111;
const TInt KMaxMessageSize = 200;
const TInt KNrOfDeviceStatusChanges = 8; // 9 since some are ignored
// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp189::CT_LbsClientPosTp189(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{  
	_LIT(KTestName, "Tp189 - Status Events");
	SetTestStepName(KTestName); 
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp189::~CT_LbsClientPosTp189()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp189::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp189::StartL()
    {
   _LIT(KDeviceStatusValues, "Device Status Values (Unknown = 0, Error = 1, Disabled = 2, Inactive = 3, Initialising = 4, StandBy = 5,  Ready = 6, Active = 7)");
   INFO_PRINTF1(KDeviceStatusValues);
    _LIT(KDataQualityValue, "Data Quality Status (Unknown = 0, Loss = 1, Partial = 2, Normal = 3)");
    INFO_PRINTF1(KDataQualityValue);

    _LIT(KSetTestStatusPsy, "Setting up TestStatusPsy");
    INFO_PRINTF1(KSetTestStatusPsy);

	SetupPsyL(iUidTestStatusPsy);

    TPositionModuleStatusEvent statusEvent2;
    statusEvent2.SetRequestedEvents(TPositionModuleStatusEvent::EEventDeviceStatus); 
	TRequestStatus status2 = KErrNone;
    RPositionServer client2;
	CleanupClosePushL(client2);
	TInt err = client2.Connect();
	_LIT(KConnectError, "TP18 Unable to open connection to Epos, return code %d");
	AssertTrueL(err == KErrNone, KConnectError, err);
    
    // Synchronous request to start server.
    ConnectL();

	// 1. Request to be notified about Module Status Events.
	TPositionModuleStatusEvent statusEvent;
	TRequestStatus status = KErrNone;

	// Open a subsession to PSY.1
	_LIT(KOpenPSY, "Calling OpenPositionerByName");
	INFO_PRINTF1(KOpenPSY);
	err = OpenPositionerByName(iUidTestStatusPsy);
	_LIT(KErrOpen, "1. Error when opening subsession to PSY , error code = %d");
	AssertTrueL(err == KErrNone, KErrOpen, err);

	
	for(TInt i = 0; i < (KNrOfDeviceStatusChanges - 1); i++) //Iterate through different device statuses. Skip first and last
		{
		RequestWaitAndLogEventL(TPositionModuleStatusEvent::EEventDeviceStatus);
		}
	
	RequestWaitAndLogEventL(TPositionModuleStatusEvent::EEventDataQualityStatus); //Test to be notified about data quality changes
	
	// Renew request before proceeding
	statusEvent.SetRequestedEvents(TPositionModuleStatusEvent::EEventDeviceStatus); 
	iPosServer.NotifyModuleStatusEvent(statusEvent, status, iUidTestStatusPsy);
	
	// 2. Disable PSY1.
	CPosModules* db = CPosModules::OpenL();
    CleanupStack::PushL(db);
    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();

    moduleUpdate->SetUpdateAvailability(EFalse);
    db->UpdateModuleL(iUidTestStatusPsy, *moduleUpdate);
    _LIT(KWaitStatus, "Waiting for status after UpdateModuleL1");
	INFO_PRINTF1(KWaitStatus);
	User::WaitForRequest(status);
	
    _LIT(KWait, "Error when request completed, error code = %d");
    iExpectedStatus.SetDeviceStatus(TPositionModuleStatus::EDeviceDisabled);
    iExpectedStatus.SetDataQualityStatus(TPositionModuleStatus::EDataQualityNormal);

    TPositionModuleStatus getStatus;
    statusEvent.GetModuleStatus(getStatus);

    if (iExpectedStatus.DeviceStatus() != getStatus.DeviceStatus() ||
        iExpectedStatus.DataQualityStatus() != getStatus.DataQualityStatus())
        {
        TBuf<100> buf;
        if (iExpectedStatus.DeviceStatus() != getStatus.DeviceStatus())
        {
        	_LIT(KError, "Status is not correct, Status returned %d Status expected %d");
        	buf.Format(KError, getStatus.DeviceStatus(), iExpectedStatus.DeviceStatus());
        	LogErrorAndLeaveL(buf);	
        }
        else
        {
        	_LIT(KError , "Quality is not correct, Quality returned %d Quality expected");
        	buf.Format(KError, getStatus.DataQualityStatus(), iExpectedStatus.DataQualityStatus());
        	LogErrorAndLeaveL(KError);	
        }
        }

	// 3. Renew request
	iPosServer.NotifyModuleStatusEvent(statusEvent, status, iUidTestStatusPsy);
	_LIT(KNotifyModuleStatusEvent, "Called NotifyModuleStatusEvent");
	INFO_PRINTF1(KNotifyModuleStatusEvent);
	
	// 4. Enable PSY1
	moduleUpdate->SetUpdateAvailability(ETrue);
    db->UpdateModuleL(iUidTestStatusPsy, *moduleUpdate);
    _LIT(KWaitUpdateModuleL2, "Waiting for status after UpdateModuleL2");
	INFO_PRINTF1(KWaitUpdateModuleL2);
	
	User::WaitForRequest(status);
	AssertTrueL(status.Int() == KErrNone, KWait, status.Int());

    // 5. Renew request
	iPosServer.NotifyModuleStatusEvent(statusEvent, status, iUidTestStatusPsy);
	client2.NotifyModuleStatusEvent(statusEvent2, status2, iUidMultiPsy);

	// 7. Disable PSY1
	_LIT(KDisable, "Disabling TestStatusPsy");
	INFO_PRINTF1(KDisable);
	moduleUpdate->SetUpdateAvailability(EFalse);
    db->UpdateModuleL(iUidTestStatusPsy, *moduleUpdate);

	// 9. Enable PSY2
    _LIT(KEnable, "Enabling MultiPsy");
	INFO_PRINTF1(KEnable);
	moduleUpdate->SetUpdateAvailability(ETrue);
    db->UpdateModuleL(iUidMultiPsy, *moduleUpdate);

    _LIT(KStatus, "Waiting for status & status2");
	INFO_PRINTF1(KStatus);
	User::WaitForRequest(status, status2);
    if (status == KRequestPending)
        {
        User::WaitForRequest(status);
        }
    else
        {
        User::WaitForRequest(status2);
        }


    AssertTrueL(status.Int() == KErrNone, KWait, status.Int());
    AssertTrueL(status2.Int() == KErrNone, KWait, status2.Int());
	
	// 10. Renew request for all PSYs
	iPosServer.NotifyModuleStatusEvent(statusEvent, status, iUidTestStatusPsy);
	
	// 11. Enable PSY1
	moduleUpdate->SetUpdateAvailability(ETrue);
    db->UpdateModuleL(iUidTestStatusPsy, *moduleUpdate);
    _LIT(KWaitUpdateModuleL3, "Waiting for status after UpdateModuleL3");
	INFO_PRINTF1(KWaitUpdateModuleL3);

	User::WaitForRequest(status);
	AssertTrueL(status.Int() == KErrNone, KWait, status.Int());

	// Open a subsession to PSY.1
	err = iPositioner2.Open(iPosServer, iUidTestStatusPsy);
	_LIT(KErrOpenOther, "7. Error when opening subsession to PSY , error code = %d");
	AssertTrueL(err == KErrNone, KErrOpenOther, err);

    User::After(10000000); //wait for all psy event to timeout
	// Renew request for all PSYs
	iPosServer.NotifyModuleStatusEvent(statusEvent, status);
    User::After(1000000);
	// 14. Close subsession to PSY1. All events execept close event has happen
	iPositioner2.Close();
	_LIT(KStatusClose, "Waiting for status after Close");
	INFO_PRINTF1(KStatusClose);

	User::WaitForRequest(status);
	err = status.Int();
	_LIT(KStatusErr, "status: %d");
	INFO_PRINTF2(KStatusErr, err);
	AssertTrueL(err == KErrNone, KWait, err);


	// 15. Renew request for PSY1
	iPosServer.NotifyModuleStatusEvent(statusEvent, status, iUidTestStatusPsy);

    User::After(600000);
	// 16. Cancel request
	TInt cancelErr = iPosServer.CancelRequest(EPositionServerNotifyModuleStatusEvent);
	_LIT(KCancel, "7. Error canceling request, error code = %d");
	AssertTrueL(cancelErr == KErrNone, KCancel, cancelErr);
	_LIT(KStatusCancelRequest, "Waiting for status after CancelRequest");
	INFO_PRINTF1(KStatusCancelRequest);

	User::WaitForRequest(status);
	AssertTrueL(status.Int() == KErrCancel, KCancel, status.Int());
	
	// 17. Cancel request again
	cancelErr = iPosServer.CancelRequest(EPositionServerNotifyModuleStatusEvent);
	AssertTrueL(cancelErr == KErrNotFound, KCancel, cancelErr);

	// 18. Request for non-existing PSY.
	_LIT(KNonExistingPSY, "Request for non-existing PSY");
	INFO_PRINTF1(KNonExistingPSY);
	TUid aNonExistingUid;
	aNonExistingUid.iUid = KUidNonExistingPsy;
	iPosServer.NotifyModuleStatusEvent(statusEvent, status, aNonExistingUid);
	_LIT(KStatusWaiting, "Waiting for status");
	INFO_PRINTF1(KStatusWaiting);
	
	User::WaitForRequest(status);
	AssertTrueL(status.Int() == KErrNotFound, KWait, status.Int());
	
    
	CleanupStack::PopAndDestroy(moduleUpdate);
    CleanupStack::PopAndDestroy(db);
    CleanupStack::PopAndDestroy(&client2);
	
    _LIT(KClosePositioner, "Calling ClosePositioner");
	INFO_PRINTF1(KClosePositioner);
	ClosePositioner();
	
	Disconnect();
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp189::CheckModuleStatus
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp189::RequestWaitAndLogEventL(const TInt aEventToListenFor)
	{
	TPositionModuleStatusEvent statusEvent;
	TRequestStatus status = KErrNone;

	if(aEventToListenFor == TPositionModuleStatusEvent::EEventDataQualityStatus)
		{
		statusEvent.SetRequestedEvents(TPositionModuleStatusEvent::EEventDataQualityStatus);
		}
	else
		{
		statusEvent.SetRequestedEvents(TPositionModuleStatusEvent::EEventDeviceStatus);
		}

	// Renew request
	iPosServer.NotifyModuleStatusEvent(statusEvent, status, iUidTestStatusPsy);
	
	User::WaitForRequest(status);
	_LIT(KErrLoopOpen, "Error in loop when opening subsession to PSY , error code = %d");
	AssertTrueL(status.Int() == KErrNone, KErrLoopOpen, status.Int());
	
	// Get status information for PSY.
	TPositionModuleStatus moduleStatus = TPositionModuleStatus();
	TInt err = iPosServer.GetModuleStatus(moduleStatus, iUidTestStatusPsy);
	_LIT(KGet, "Error when getting module status from PSY , error code = %d");
	AssertTrueL(err == KErrNone, KGet, err);
	CheckModuleStatusL(moduleStatus);
	}
// ---------------------------------------------------------
// CT_LbsClientPosTp189::CheckModuleStatus
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp189::CheckModuleStatusL(const TPositionModuleStatus& aStatus , const TBool /*aCheckStatus*/)
	{
	TBuf<KMaxMessageSize> buf;	
	_LIT(KStatus,"Checking module status");
	INFO_PRINTF1(KStatus);
	_LIT(KErrBoundary, "The value on status/quaity is out of range");

    _LIT(KDevSta, "Device Status  =  %d");
    TInt status = aStatus.DeviceStatus();
    buf.Format(KDevSta, status);
	INFO_PRINTF1(buf);
	buf.Zero();
	//check that status value is not out of range
    switch(status)
        {
        case TPositionModuleStatus::EDeviceUnknown:
        case TPositionModuleStatus::EDeviceError:
        case TPositionModuleStatus::EDeviceInitialising:
        case TPositionModuleStatus::EDeviceStandBy:
        case TPositionModuleStatus::EDeviceReady:
        case TPositionModuleStatus::EDeviceActive:
        case TPositionModuleStatus::EDeviceInactive: //the psy should not be able to report this
            break;
        case TPositionModuleStatus::EDeviceDisabled: //the psy should not be able to report this
        default:
            LogErrorAndLeaveL(KErrBoundary);
        }

    

	_LIT(KDatQual, "Data Quality Status =  %d");
	TInt quality = aStatus.DataQualityStatus();
    buf.Format(KDatQual, quality);
	INFO_PRINTF1(buf);
	buf.Zero();
    //check that qauality value is not out of range
    switch(quality)
        {
        case TPositionModuleStatus::EDataQualityUnknown:
        case TPositionModuleStatus::EDataQualityLoss:
        case TPositionModuleStatus::EDataQualityPartial:
        case TPositionModuleStatus::EDataQualityNormal:
            break;
        default:
            LogErrorAndLeaveL(KErrBoundary);
        }
	}

// End of file
