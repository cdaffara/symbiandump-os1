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
#include "ctlbsclientpostp188.h"
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>

// CONSTANTS
const TInt32 KUidNonExistingPsy = 0x01111111;
const TInt KMaxMessageSize = 200;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp188::CT_LbsClientPosTp188(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{  
	_LIT(KTestName,"Tp188 - Get Module Status");
	SetTestStepName(KTestName); 
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp188::~CT_LbsClientPosTp188()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp188::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp188::StartL()
    {
	SetupPsyL(iUidTestStatusPsy);

    // Synchronous request to start server.
    ConnectL();

	// Request status information for PSY.
	TPositionModuleStatus moduleStatus = TPositionModuleStatus();
	TInt err = iPosServer.GetModuleStatus(moduleStatus, iUidTestStatusPsy);
	_LIT(KErrFailStatus1, "1. Error when getting module status, error code = %d");
	AssertTrueL(err == KErrNone, KErrFailStatus1, err);
   
    iModuleStatus.SetDeviceStatus(TPositionModuleStatus::EDeviceInactive);
    iModuleStatus.SetDataQualityStatus(TPositionModuleStatus::EDataQualityUnknown);
	
	CheckModuleStatusL(moduleStatus);

	// Disable PSY.
	CPosModules* db = CPosModules::OpenL();
    CleanupStack::PushL(db);
    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();

    moduleUpdate->SetUpdateAvailability(EFalse);
    db->UpdateModuleL(iUidTestStatusPsy, *moduleUpdate);

	User::After(1000000);

	// Request status information for PSY.
	err = iPosServer.GetModuleStatus(moduleStatus, iUidTestStatusPsy);
	_LIT(KErrFailStatus2, "2. Error when getting module status, error code = %d");
	AssertTrueL(err == KErrNone, KErrFailStatus2, err);

    iModuleStatus.SetDeviceStatus(TPositionModuleStatus::EDeviceDisabled);
    iModuleStatus.SetDataQualityStatus(TPositionModuleStatus::EDataQualityUnknown);

	User::After(1000000);

	CheckModuleStatusL(moduleStatus);

	// Request status information for a non-existing PSY.
	TUid aNonExistingUid;
	aNonExistingUid.iUid = KUidNonExistingPsy;
	err = iPosServer.GetModuleStatus(moduleStatus, aNonExistingUid);
	_LIT(KErrFailStatus3, "3. Error when getting module status, error code = %d");
	AssertTrueL(err == KErrNotFound, KErrFailStatus3, err);
	
    SetupPsyL(iUidTestStatusPsy);

	// Request to be notified about Module Status Events.
	TPositionModuleStatusEvent statusEvent;
	TRequestStatus status = KErrNone;
	statusEvent.SetRequestedEvents(TPositionModuleStatusEvent::EEventDeviceStatus); // Obligatory to set Requested events, EEventAll works also.
	iPosServer.NotifyModuleStatusEvent(statusEvent, status, iUidTestStatusPsy);
	
	// Open a subsession to PSY.
	err = OpenPositionerByName(iUidTestStatusPsy);
	_LIT(KErrFailStatus4, "4. Error when opening module , error code = %d");
	AssertTrueL(err == KErrNone, KErrFailStatus4, err);
	
	CleanupStack::PopAndDestroy(moduleUpdate);
    CleanupStack::PopAndDestroy(db);
	
	User::WaitForRequest(status);
	_LIT(KErrFailStatus5, "5. Error in completed request, error code = %d");
	AssertTrueL(status.Int() == KErrNone, KErrFailStatus5, err);

	// Request status information for PSY.
	err = iPosServer.GetModuleStatus(moduleStatus, iUidTestStatusPsy);
	_LIT(KErrFailStatus6, "6. Error whe getting module status, error code = %d");
	AssertTrueL(err == KErrNone, KErrFailStatus6, err);

    iModuleStatus.SetDeviceStatus(TPositionModuleStatus::EDeviceReady);
    iModuleStatus.SetDataQualityStatus(TPositionModuleStatus::EDataQualityNormal);
	CheckModuleStatusL(moduleStatus);
	
	ClosePositioner();


	Disconnect();

	}

// ---------------------------------------------------------
// CT_LbsClientPosTp188::CheckModuleStatus
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp188::CheckModuleStatusL(const TPositionModuleStatus& aStatus)
	{
	TBuf<KMaxMessageSize> buf;	
	_LIT(KStatus,"Checking module status");
	INFO_PRINTF1(KStatus);
	
	_LIT(KDevSta, "Device Status (EDeviceUnknown = 0, EDeviceError = 1, EDeviceDisabled = 2, EDeviceInactive = 3, EDeviceInitialising = 4, EDeviceStandBy = 5,  EDeviceReady = 6, EDeviceActive = 7) :  %d");
	buf.Format(KDevSta, aStatus.DeviceStatus());
	INFO_PRINTF1(buf);
	buf.Zero();
	
	_LIT(KDatQual, "Data Quality Status (EDataQualityUnknown = 0, EDataQualityLoss = 1, EDataQualityPartial = 2, EDataQualityNormal = 3) : ,  %d");
	buf.Format(KDatQual, aStatus.DataQualityStatus());
	INFO_PRINTF1(buf);
	buf.Zero();
	
	//unused TInt myVar = iModuleStatus.DeviceStatus();
	//unused TInt myVar2 = aStatus.DeviceStatus();

    if (iModuleStatus.DeviceStatus() != aStatus.DeviceStatus() ||
        iModuleStatus.DataQualityStatus() != aStatus.DataQualityStatus())
        {
        if (iModuleStatus.DeviceStatus() != aStatus.DeviceStatus())
        {
        	_LIT(KError, "Status is not correct, Status returned %d Status expected %d");
        	buf.Format(KError, aStatus.DeviceStatus(), iModuleStatus.DeviceStatus());
        	LogErrorAndLeaveL(buf);	
        }
        else
        {
        	_LIT(KError , "Quality is not correct, Quality returned %d Quality expected");
        	buf.Format(KError, aStatus.DataQualityStatus(), iModuleStatus.DataQualityStatus());
        	LogErrorAndLeaveL(KError);	
        }
        
        }
	}

// End of file
