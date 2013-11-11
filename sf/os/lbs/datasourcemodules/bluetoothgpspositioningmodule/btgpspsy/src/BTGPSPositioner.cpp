// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32base.h>
#include <lbs/epos_mpositionerstatus.h>
#include <ecom/implementationproxy.h>
#include <lbssatellite.h>

#include "BTGPSPositioner.h"
#include "BTGPSRequestHandler.h"
#include "BTGPSPanic.h"
#include "btgpspsy.hrh"
#include "BTGPSLogging.h"
#include "BTGPSNmeaBuffer.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
static const TImplementationProxy KFactoryPtr = 
IMPLEMENTATION_PROXY_ENTRY( KPosBTPSYImplUid, CBTGPSPositioner::NewL);

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

/**
*  The extension class for the CBTGPSPositioner
*  Stores the instance variables
*/
class TBTGPSExtension
    {
    public:
        /**
        * default constructor
        */
        TBTGPSExtension():
            iRequestHandler(NULL),
            iActive(EFalse),
            iPosInfo(NULL),
            iStatus(NULL)
            {
            }

        //Pointer to the request handler
        CBTGPSRequestHandler* iRequestHandler;

        //Is there request pending
        TBool iActive;

        //Pointer to the current position info
        TPositionInfoBase* iPosInfo;

        //Pointer to the current request status
        TRequestStatus* iStatus;
    };

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CBTGPSPositioner::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CBTGPSPositioner* CBTGPSPositioner::NewL(TAny* aConstructionParameters)
    {
    CBTGPSPositioner* self = new( ELeave ) CBTGPSPositioner;

    CleanupStack::PushL( self );
    self->ConstructL(aConstructionParameters);
    CleanupStack::Pop(self);

    return self;
    }


// -----------------------------------------------------------------------------
// CBTGPSPositioner::~CBTGPSPositioner
// Destructor.
// -----------------------------------------------------------------------------
//
CBTGPSPositioner::~CBTGPSPositioner()
    {
    TRACESTRING("CBTGPSPositioner::~CBTGPSPositioner")
    if ( iBTGPSExtension )
        {
        if ( iBTGPSExtension->iRequestHandler )
            {
            iBTGPSExtension->iRequestHandler->UnregisterPSY(this);
            }
		delete iBTGPSExtension;
        }
    }

// -----------------------------------------------------------------------------
// NotifyPositionUpdate implements Cpositioner::NotifyPositionUpdate
// Requests position info asynchronously.
// Returns: aPosInfo: A reference to a position info object. This object
//                    must be in scope until the request has completed.
//                    The position (TPosition) of this position info object
//                    must be in WGS84 geodetic datum.
//          aStatus: The request status
//
// -----------------------------------------------------------------------------
//
void CBTGPSPositioner::NotifyPositionUpdate(TPositionInfoBase& aPosInfo,
                                            TRequestStatus& aStatus)
    {
    TRACESTRING("CBTGPSPositioner::NotifyPositionUpdate")

    //Clear position info
    ClearPositionInfo(aPosInfo);
    
    const CBTGPSFix* fix = NULL;
    TTime maxAge;

    aStatus = KRequestPending;

    iBTGPSExtension->iPosInfo = &aPosInfo;
    iBTGPSExtension->iStatus = &aStatus;
    GetMaxAge(maxAge);

    //Check if last location can be used
    TInt result = iBTGPSExtension->iRequestHandler->LastLocation(fix,maxAge,
        IsPartialUpdateAllowed());

    if( result  == KErrNone )
        {
        TInt err(KErrNone);
        
        switch(fix->FixState())
            {
            case CBTGPSFix::EFixValidityUnkown:
                {
                //Should never happen
                TRACESTRING("CBTGPSPositioner::NotifyPositionUpdate: \
INTERNAL ERROR: CBTGPSFix::ENull.")
                err = KPositionQualityLoss;
                break;
                }
            case CBTGPSFix::EFixNotValid:
                {
                TRACESTRING("CBTGPSPositioner::NotifyPositionUpdate: \
Last known location: CBTGPSFix::ENotValid.")
                err = fix->FillPositionInfo(*(iBTGPSExtension->iPosInfo));
                if(err == KErrNone)
                    {
                    err = KPositionPartialUpdate;
                    }
                break;
                }
            case CBTGPSFix::EFixValid2D:
            case CBTGPSFix::EFixValid3D:
                {
                err = fix->FillPositionInfo(*(iBTGPSExtension->iPosInfo));
                break;
                }
            default:
                {
                //Should never happen
                TRACESTRING("CBTGPSPositioner::NotifyPositionUpdate: \
INTERNAL ERROR: fix unknown.")
                err = KErrGeneral;
                break;
                }
            }
            TRACESTRING2("CBTGPSPositioner::Complete request error = %d", err)
            User::RequestComplete(iBTGPSExtension->iStatus, err);
        }
    else
        {
        iBTGPSExtension->iActive = ETrue;
        iBTGPSExtension->iRequestHandler->AcquireLocation(this);
        }

    }


// -----------------------------------------------------------------------------
// CancelNotifyPositionUpdate implements Cpositioner::CancelNotifyPositionUpdate
// Cancels position info request.
// -----------------------------------------------------------------------------
//
void CBTGPSPositioner::CancelNotifyPositionUpdate()
    {
    TRACESTRING("CBTGPSPositioner::CancelNotifyPositionUpdate")

    if ( iBTGPSExtension->iActive )
        {
        iBTGPSExtension->iActive = EFalse;
        iBTGPSExtension->iPosInfo = NULL;
        iBTGPSExtension->iRequestHandler->CancelAcquireLocation(this);

        TRACESTRING2("CBTGPSPositioner::Complete request error = %d", KErrCancel)
        User::RequestComplete(iBTGPSExtension->iStatus,KErrCancel);
        }
    }


// -----------------------------------------------------------------------------
// TrackingOverridden implements Cpositioner::TrackingOverridden
// Indicate if the PSY has overridden tracking.
// -----------------------------------------------------------------------------
//
TBool CBTGPSPositioner::TrackingOverridden() const
    {
    return ETrue;
    }


// -----------------------------------------------------------------------------
// TrackingOverridden implements Cpositioner::TrackingOverridden
// Initiate a tracking session.
// -----------------------------------------------------------------------------
//
void CBTGPSPositioner::StartTrackingL(
                                const TTimeIntervalMicroSeconds& aInterval)
    {
    TRACESTRING("CBTGPSPositioner::StartTrackingL")
    iBTGPSExtension->iRequestHandler->TrackingSessionStartL(this,aInterval);
    }

// -----------------------------------------------------------------------------
// StopTracking implements Cpositioner::StopTracking
// Any outstanding requests will be cancelled.
// -----------------------------------------------------------------------------
//
void CBTGPSPositioner::StopTracking()
    {
    TRACESTRING("CBTGPSPositioner::StopTracking")
    iBTGPSExtension->iRequestHandler->TrackingSessionStop(this);
    }

// -----------------------------------------------------------------------------
// CBTGPSPositioner::RequestCompleteNotify implements
// MBTGPSRequestCompleteListener::RequestCompleteNotify
// -----------------------------------------------------------------------------
//
TBool CBTGPSPositioner::RequestCompleteNotify(
        const CBTGPSFix* aFix,
        TInt aErr)
    {
    TRACESTRING("CBTGPSPositioner::RequestCompleteNotify start...")
    TRACESTRING2("error = %d", aErr)
    TBool completed = EFalse;
    
    //Validate the NMEA Buffer index and bottom
    if(aFix!=NULL)
        {
        aFix->ValidateNmeaBufferIndex(
            iNmeaIndex,
            iNmeaBottom);
        }
    
    //If there was error, pass it
    if ( (aErr < 0) || (KPositionQualityLoss == aErr) || (aFix==NULL))
        {
        ClearPositionInfo(*iBTGPSExtension->iPosInfo);
        iBTGPSExtension->iActive = EFalse;

        TRACESTRING2("CBTGPSPositioner::Complete request error = %d", aErr)
        User::RequestComplete(iBTGPSExtension->iStatus,aErr);
        completed = ETrue;
        }
    //No error, complete with fix
    else
        {
        TRACESTRING2("fix type = %d", aFix->FixState());

        TInt err = KErrNone;
        
        switch(aFix->FixState())
            {
            case CBTGPSFix::EFixValidityUnkown:
            case CBTGPSFix::EFixNotValid:
                {
                if ( IsPartialUpdateAllowed() )
                    {
                    //Complete the fix when client accept partial update. 
                    //Otherwise, continue
                    completed = ETrue;
                    err = KPositionPartialUpdate;
                    }
                }
                break;
            case CBTGPSFix::EFixValid2D:
            case CBTGPSFix::EFixValid3D:
                completed = ETrue;
                err = KErrNone;
                break;
            default:
                {
                TRACESTRING("CBTGPSPositioner::RequestCompleteNotify: \
INTERNAL ERROR: fix unknown.")
                completed = ETrue;
                err = KErrGeneral;
                break;
                }
            }
        
        if(completed)
            {
            TInt nmeaIndex = iNmeaIndex;
            TInt fillErr = aFix->FillPositionInfo(
                *iBTGPSExtension->iPosInfo,
                &nmeaIndex);
            
            if(fillErr != KErrNone)
                {
                err = fillErr;
                }
            else
                {
                iNmeaIndex = nmeaIndex;
                }
                
            iBTGPSExtension->iActive = EFalse;

            TRACESTRING2("CBTGPSPositioner::Complete request error = %d", err)
            User::RequestComplete(iBTGPSExtension->iStatus,err);
            }
        
        }
    return completed;
    }

// -----------------------------------------------------------------------------
// CBTGPSPositioner::ReportStatus
// Reports the PSY status to the MLFW
// -----------------------------------------------------------------------------
void CBTGPSPositioner::ReportStatus(const TPositionModuleStatus& aStatus)
    {
    MPositionerStatus* statusInf = this->PositionerStatus();
    statusInf->ReportStatus(aStatus);
    }

// -----------------------------------------------------------------------------
// CBTGPSPositioner::SetInitialNmeaBufferBottom
// -----------------------------------------------------------------------------
void CBTGPSPositioner::SetInitialNmeaBufferBottom(TInt aIndex)
    {
    iNmeaIndex = aIndex;
    }

// -----------------------------------------------------------------------------
// CBTGPSPositioner::ClearPositionInfo
// Clears the position info
// -----------------------------------------------------------------------------
void CBTGPSPositioner::ClearPositionInfo(TPositionInfoBase& aPosInfo)
    {
    if (aPosInfo.PositionClassType() & EPositionSatelliteInfoClass)
        {
        // TPositionSatelliteInfo
        (void) new (&aPosInfo) (TPositionSatelliteInfo);
        }
    else if (aPosInfo.PositionClassType() & EPositionCourseInfoClass)
        {
        // TPositionCourseInfo
        (void) new (&aPosInfo) (TPositionCourseInfo);
        }
    else if (aPosInfo.PositionClassType() & EPositionGenericInfoClass)
        {
        // HPositionGenericInfo
        HPositionGenericInfo* genInfo =
            static_cast<HPositionGenericInfo*> ( &aPosInfo );
        
        genInfo->ClearPositionData();
        }
    else if (aPosInfo.PositionClassType() & EPositionInfoClass)
        {
        // TPositionInfo
        (void) new (&aPosInfo) (TPositionInfo);
        }
    else
        {
        // Unknown type, this should never happen
        // --> Panic if we get here
        Panic(EPanicUnknownPositioningClass);
        }

    aPosInfo.SetModuleId(ImplementationUid());
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CBTGPSPositioner::CBTGPSPositioner()
    {
    iNmeaIndex = KBTGPSNmeaIndexNotSet;
    iNmeaBottom = KBTGPSNmeaIndexNotSet;
    }

// -----------------------------------------------------------------------------
// CBTGPSPositioner::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CBTGPSPositioner::ConstructL(TAny* aConstructionParameters)
    {
    // Must call BaseConstructL first thing during
    // construction.
    CLEARTRACELOG;
    
    TRACESTRING("CBTGPSPositioner::ConstructL")
    BaseConstructL(aConstructionParameters);

    iBTGPSExtension = new (ELeave) TBTGPSExtension;

    iBTGPSExtension->iRequestHandler = CBTGPSRequestHandler::GetInstanceL();
    iBTGPSExtension->iRequestHandler->RegisterPSYL(this);
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount)
    {
    aTableCount =1;
    return &KFactoryPtr;
    }

//  End of File

