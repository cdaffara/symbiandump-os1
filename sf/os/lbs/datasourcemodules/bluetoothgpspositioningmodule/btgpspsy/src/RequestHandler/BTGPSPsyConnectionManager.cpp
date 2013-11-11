// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32std.h>
#include "BTGPSPsyConnectionManager.h"
#include "BTGPSPositionerExt.h"
#include "BTGPSPsyConnectionListener.h"
#include "BTGPSPanic.h"
#include "BTGPSLogging.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// Undefined tracking interval
const TInt KTrackingIntervalUndefined = -1;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CBTGPSPsyConnectionManager::NewL
// -----------------------------------------------------------------------------
CBTGPSPsyConnectionManager* CBTGPSPsyConnectionManager::NewL()
    {
    CBTGPSPsyConnectionManager* self = new (ELeave) CBTGPSPsyConnectionManager();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CBTGPSPsyConnectionManager::~CBTGPSPsyConnectionManager
// -----------------------------------------------------------------------------
CBTGPSPsyConnectionManager::~CBTGPSPsyConnectionManager()
    {
    iPsyArray.Close();
    iListenerArray.Close();
    }


// -----------------------------------------------------------------------------
// CBTGPSPsyConnectionManager::ConstructL
// -----------------------------------------------------------------------------
void CBTGPSPsyConnectionManager::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CBTGPSPsyConnectionManager::CBTGPSPsyConnectionManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CBTGPSPsyConnectionManager::CBTGPSPsyConnectionManager()
    {
    }

// -----------------------------------------------------------------------------
// CBTGPSPsyConnectionManager::RegisterPSYL
// -----------------------------------------------------------------------------
void CBTGPSPsyConnectionManager::RegisterPSYL(MBTGPSPositionerExt* aPSY)
    {
    TRACESTRING("CBTGPSPsyConnectionManager::RegisterPSYL start...")
    //Construct PSY status
    TPSYStatusStruct psyStatus;
    psyStatus.iPsy = aPSY;
    psyStatus.iLocationRequest = EFalse;
    psyStatus.iTrackingInterval = KTrackingIntervalUndefined;
    
    //Location FW garentee that PSY can't be registered more than once
    
    //Add psy status to array
    User::LeaveIfError(iPsyArray.Append(psyStatus));
    InformListenersChange();
    TRACESTRING("CBTGPSPsyConnectionManager::RegisterPSYL end")
    }

// -----------------------------------------------------------------------------
// CBTGPSPsyConnectionManager::UnregisterPSY
// -----------------------------------------------------------------------------
void CBTGPSPsyConnectionManager::UnregisterPSY(MBTGPSPositionerExt* aPSY)
    {
    TRACESTRING("CBTGPSPsyConnectionManager::UnregisterPSY start...")
    TInt index = FindPsy(aPSY);
    __ASSERT_DEBUG(index!=KErrNotFound, Panic(EPanicIndexOutOfRange));
    if(index!=KErrNotFound)
        {
        iPsyArray.Remove(index);
        InformListenersChange();
        }
    TRACESTRING("CBTGPSPsyConnectionManager::UnregisterPSY end")
    }

// -----------------------------------------------------------------------------
// CBTGPSPsyConnectionManager::AcquireLocation
// -----------------------------------------------------------------------------
void CBTGPSPsyConnectionManager::AcquireLocation(MBTGPSPositionerExt* aPSY)
    {
    TInt index = FindPsy(aPSY);
    __ASSERT_DEBUG(index!=KErrNotFound, Panic(EPanicIndexOutOfRange));
    if(index!=KErrNotFound)
        {
        iPsyArray[index].iLocationRequest = ETrue;
        InformListenersChange();
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSPsyConnectionManager::CancelAcquireLocation
// -----------------------------------------------------------------------------
void CBTGPSPsyConnectionManager::CancelAcquireLocation(MBTGPSPositionerExt* aPSY)
    {
    TInt index = FindPsy(aPSY);
    __ASSERT_DEBUG(index!=KErrNotFound, Panic(EPanicIndexOutOfRange));
    if(index!=KErrNotFound)
        {
        iPsyArray[index].iLocationRequest = EFalse;
        InformListenersChange();
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSPsyConnectionManager::CompleteRequest
// -----------------------------------------------------------------------------

void CBTGPSPsyConnectionManager::CompleteRequest(
            MBTGPSPositionerExt& aPSY,
            TInt aErr)
    {
    TRACESTRING("CBTGPSPsyConnectionManager::CompleteRequest start...")
    TInt index = FindPsy(&aPSY);
    __ASSERT_DEBUG(index!=KErrNotFound, Panic(EPanicIndexOutOfRange));
    if(index!=KErrNotFound)
        {
        CompleteLocationRequest(index, NULL, aErr);
        }
    TRACESTRING("CBTGPSPsyConnectionManager::CompleteRequest end")
    }
// -----------------------------------------------------------------------------
// CBTGPSPsyConnectionManager::CompleteAllRequests
// -----------------------------------------------------------------------------
void CBTGPSPsyConnectionManager::CompleteAllRequests(TInt aErr)
    {
    TInt count = iPsyArray.Count();
    for(TInt i=0; i<count; i++)
        {
        CompleteLocationRequest(i, NULL, aErr);
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSPsyConnectionManager::LocationFixUpdate
// -----------------------------------------------------------------------------
void CBTGPSPsyConnectionManager::LocationFixUpdate(const CBTGPSFix& aFix)
    {
    TInt count = iPsyArray.Count();
    for(TInt i=0; i<count; i++)
        {
        CompleteLocationRequest(i,&aFix, KErrNone);
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSPsyConnectionManager::Count
// -----------------------------------------------------------------------------
TInt CBTGPSPsyConnectionManager::Count() const
    {
    return iPsyArray.Count();
    }

// -----------------------------------------------------------------------------
// CBTGPSPsyConnectionManager::LocationRequestCount
// -----------------------------------------------------------------------------
TInt CBTGPSPsyConnectionManager::LocationRequestCount() const
    {
    TInt count = iPsyArray.Count();
    TInt locationReqCount = 0;
    for(TInt i=0; i<count; i++)
        {
        if(iPsyArray[i].iLocationRequest)
            {
            locationReqCount++;
            }
        }
    return locationReqCount;
    }

// -----------------------------------------------------------------------------
// CBTGPSPsyConnectionManager::TrackingSessionCount
// -----------------------------------------------------------------------------
TInt CBTGPSPsyConnectionManager::TrackingSessionCount() const
    {
    TInt count = iPsyArray.Count();
    TInt trackingCount = 0;
    for(TInt i=0; i<count; i++)
        {
        if(iPsyArray[i].iTrackingInterval>0)
            {
            trackingCount++;
            }
        }
    return trackingCount;
    }

// -----------------------------------------------------------------------------
// CBTGPSPsyConnectionManager::TrackingSessionStartL
// -----------------------------------------------------------------------------
void CBTGPSPsyConnectionManager::TrackingSessionStartL(
            MBTGPSPositionerExt* aPSY,
            TTimeIntervalMicroSeconds aInterval)
    {
    TInt index = FindPsy(aPSY);
    __ASSERT_DEBUG(index!=KErrNotFound, Panic(EPanicIndexOutOfRange));
    if(index!=KErrNotFound && aInterval > 0)
        {
        iPsyArray[index].iTrackingInterval = aInterval;
        InformListenersChange();
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSPsyConnectionManager::TrackingSessionStop
// -----------------------------------------------------------------------------
void CBTGPSPsyConnectionManager::TrackingSessionStop(MBTGPSPositionerExt* aPSY)
    {
    TInt index = FindPsy(aPSY);
    if(index!=KErrNotFound)
        {
        iPsyArray[index].iTrackingInterval = KTrackingIntervalUndefined;
        InformListenersChange();
        }
    }
    
// -----------------------------------------------------------------------------
// CBTGPSPsyConnectionManager::FindPsy
// -----------------------------------------------------------------------------
TInt CBTGPSPsyConnectionManager::FindPsy(MBTGPSPositionerExt* aPSY)
    {
    TInt count = iPsyArray.Count();
    for(TInt i=0; i<count; i++)
        {
        if(iPsyArray[i].iPsy == aPSY)
            {
            return i;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CBTGPSPsyConnectionManager::CompleteLocationRequest
// -----------------------------------------------------------------------------
void CBTGPSPsyConnectionManager::CompleteLocationRequest(
            TInt aIndex,
            const CBTGPSFix* aFix,
            TInt aErr)
    {
    if(iPsyArray[aIndex].iLocationRequest)
        {
        TBool completed = iPsyArray[aIndex].iPsy->RequestCompleteNotify(aFix, aErr);
        if(completed)
            {
            iPsyArray[aIndex].iLocationRequest = EFalse;
            InformListenersChange();
            }
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSPsyConnectionManager::ReportStatus
// -----------------------------------------------------------------------------
void CBTGPSPsyConnectionManager::ReportStatus(const TPositionModuleStatus& aStatus)
    {
    TRACESTRING("CBTGPSPsyConnectionManager::ReportStatus start...")
    TRACESTRING2("DeviceStatus=%d", aStatus.DeviceStatus())
    TRACESTRING2("DataQuality =%d", aStatus.DataQualityStatus())

    //Report status from one PSY connection only
    if(iPsyArray.Count()>0)
        {
        iPsyArray[0].iPsy->ReportStatus(aStatus);
        }

    TRACESTRING("CBTGPSPsyConnectionManager::ReportStatus end")

    }

// -----------------------------------------------------------------------------
// CBTGPSPsyConnectionManager::AddListenerL
// -----------------------------------------------------------------------------
void CBTGPSPsyConnectionManager::AddListenerL(MBTGPSPsyConnectionListener& aListener)
    {
    User::LeaveIfError(iListenerArray.Append(&aListener));
    }
        
// -----------------------------------------------------------------------------
// CBTGPSPsyConnectionManager::RemoveListener
// -----------------------------------------------------------------------------
void CBTGPSPsyConnectionManager::RemoveListener(MBTGPSPsyConnectionListener& aListener)
    {
    TInt index = iListenerArray.Find(&aListener);
    if(index!=KErrNotFound)
        {
        iListenerArray.Remove(index);
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSPsyConnectionManager::InformListenersChange
// -----------------------------------------------------------------------------
void CBTGPSPsyConnectionManager::InformListenersChange()
    {
    TInt count = iListenerArray.Count();
    for(TInt i=count-1; i>=0; i--)
        {
        iListenerArray[i]->HandlePsyConnectionChange();
        }
    }

//  End of File



