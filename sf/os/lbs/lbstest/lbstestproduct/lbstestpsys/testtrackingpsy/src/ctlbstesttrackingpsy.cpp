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
#include <lbspositioninfo.h>
#include "ctlbstesttrackingPsy.h"
#include "testtrackingpsy.hrh"
#include "epos_mpositionerstatus.h"

// CONSTANTS
const TInt KMinimumUpdateInterval = 100000;


// ================= MEMBER FUNCTIONS =======================
// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestTrackingPsy::CT_LbsTestTrackingPsy() : iPositioner(NULL), iSucceed(ETrue), iFirst(ETrue), iTrackingEnabled(EFalse)
    {
    }

// EPOC default constructor can leave.
void CT_LbsTestTrackingPsy::ConstructL(TAny* aConstructionParameters)
	{
	BaseConstructL(aConstructionParameters);
	iPositioner = PositionerStatus();
	iPeriodicTimer = CPeriodic::NewL(CActive::EPriorityStandard);
	iInterval = TTimeIntervalMicroSeconds32(0);
    }

// Two-phased constructor.
CT_LbsTestTrackingPsy* CT_LbsTestTrackingPsy::NewLC(TAny* aConstructionParameters)
    {
    CT_LbsTestTrackingPsy* self = new (ELeave) CT_LbsTestTrackingPsy;
    CleanupStack::PushL(self); 
    self->ConstructL(aConstructionParameters);
    return self;
    }
    
// Two-phased constructor.
CT_LbsTestTrackingPsy* CT_LbsTestTrackingPsy::NewL(TAny* aConstructionParameters)
    {
    CT_LbsTestTrackingPsy* self = NewLC(aConstructionParameters);
    CleanupStack::Pop();
    return self;
    }
    
// Destructor
CT_LbsTestTrackingPsy::~CT_LbsTestTrackingPsy()
    {
	iPeriodicTimer->Cancel();
	delete iPeriodicTimer;
	}

// ---------------------------------------------------------
// CT_LbsTestTrackingPsy::NotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestTrackingPsy::NotifyPositionUpdate(
	TPositionInfoBase& aPosInfo,
    TRequestStatus& aStatus)
	{
    TPositionInfo* position = NULL;
    position = static_cast<TPositionInfo*> (&aPosInfo);
    iStatus = &aStatus;
    *iStatus = KRequestPending;

  	TUid implUid = { KPosImplementationUid };
	position->SetModuleId(implUid);

    if (iTrackingEnabled)
        {
        // Set this position when tracking is enabled (on-going)
        TCoordinate coor(65.0, 65.0, 65.0);
        TLocality loc (coor, 1.0, 1.0);
        TPosition pos (loc, TTime(0));  
        position -> SetPosition(pos);
        }
    else
        {
	    // Set a dummy position
        TCoordinate coor(30.0, 30.0, 30.0);
        TLocality loc (coor, 1.0, 1.0);
        TPosition pos (loc, TTime(0));
        position -> SetPosition(pos);

        User::RequestComplete(iStatus, KErrNone);
        }
    }

// ---------------------------------------------------------
// CT_LbsTestTrackingPsy::CompleteRequest
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CT_LbsTestTrackingPsy::CompleteRequest(TAny* aThis)
	{
	CT_LbsTestTrackingPsy* self = static_cast<CT_LbsTestTrackingPsy*> (aThis);
    if ((self -> iStatus) && *(self -> iStatus) == KRequestPending)
        {
	    if(self->iSucceed)
		    {
		    User::RequestComplete(self->iStatus, KErrNone);
		    self->iSucceed = EFalse;
		    }
	    else
		    {
		    User::RequestComplete(self->iStatus, KPositionQualityLoss);
		    self->iSucceed = ETrue;
		    }
        }
	return KErrNone;
	}
// ---------------------------------------------------------
// CT_LbsTestTrackingPsy::TrackingOverridden
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CT_LbsTestTrackingPsy::TrackingOverridden() const
{
	return ETrue;
}
// ---------------------------------------------------------
// CT_LbsTestTrackingPsy::StartTrackingL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestTrackingPsy::StartTrackingL(const TTimeIntervalMicroSeconds& aInterval)
	{
	iInterval = I64LOW(aInterval.Int64());
	if(iInterval.Int() < KMinimumUpdateInterval)
		{	
		iInterval = TTimeIntervalMicroSeconds32(KMinimumUpdateInterval);
		}
    iFirst = EFalse;
    iTrackingEnabled = ETrue;
    
    iPeriodicTimer->Cancel();

    TCallBack callback(CompleteRequest, this);
    iPeriodicTimer->Start(iInterval, iInterval, callback);

	}

// ---------------------------------------------------------
// CT_LbsTestTrackingPsy::StopTracking
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestTrackingPsy::StopTracking()
	{
	iInterval = TTimeIntervalMicroSeconds32(KMinimumUpdateInterval);
    iTrackingEnabled = EFalse;
    iFirst = ETrue;
    iPeriodicTimer->Cancel();
	}

// ---------------------------------------------------------
// CT_LbsTestTrackingPsy::CancelNotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestTrackingPsy::CancelNotifyPositionUpdate()
	{
    iPeriodicTimer->Cancel();
    User::RequestComplete(iStatus, KErrCancel);
	}

// End of file
